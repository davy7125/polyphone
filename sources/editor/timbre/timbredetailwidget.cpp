/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/


#include "timbredetailwidget.h"
#include "contextmanager.h"
#include <QLabel>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QtMath>
#include <functional>

static const char * EXPANDED_KEY = "timbre_detail_expanded";
static const char * EXPANDED_DEFAULT = "0,1,2"; // Deviation, junctions and rendering

namespace
{
// Columns: name, name of the measure for a line written on several rows (level, timbre), value, unit, icons of the
// labels, then an empty column taking the remaining width
enum Column
{
    COLUMN_NAME,
    COLUMN_MEASURE,
    COLUMN_VALUE,
    COLUMN_UNIT,
    COLUMN_BADGE,
    COLUMN_FILL,
    COLUMN_COUNT
};

// Kinds of lines
enum Kind
{
    KIND_SECTION, // Title written over the value columns, then the summary and the icons of the rows if folded
    KIND_GROUP, // Name of a sample, written over the value columns
    KIND_ROW, // Name (written over the column of the measures if there is no measure), value, unit and icon
    KIND_NOTE // Wrapped on the whole width
};

const int ROLE_KIND = Qt::UserRole;
const int ROLE_SECTION = Qt::UserRole + 1;
const int ROLE_SUMMARY = Qt::UserRole + 2;
const int ROLE_ICONS = Qt::UserRole + 3; // List of QIcon, in the column of the labels
const int ROLE_ICON_KEYS = Qt::UserRole + 4; // Shapes and colors of the icons of a section, for not repeating them
const int ROLE_ICON_TOOLTIP = Qt::UserRole + 5; // Tooltips of the icons of a section, shown when folded

// Space between the names and the values
const int PADDING = 12;
const int ICON_SPACING = 4;

int kindOf(const QModelIndex &index)
{
    return index.sibling(index.row(), COLUMN_NAME).data(ROLE_KIND).toInt();
}

bool hasMeasure(const QModelIndex &index)
{
    return !index.sibling(index.row(), COLUMN_MEASURE).data(Qt::DisplayRole).toString().isEmpty();
}
}

// Tree whose row heights are computed again when its width changes (wrapped notes)
class TimbreDetailTree : public QTreeWidget
{
public:
    explicit TimbreDetailTree(QWidget * parent) : QTreeWidget(parent) {}

    QSize cellSize(QTreeWidgetItem * item, int column) const
    {
        return this->sizeHintForIndex(this->indexFromItem(item, column));
    }

    int depth(const QModelIndex &index) const
    {
        int depth = 0;
        for (QModelIndex parent = index.parent(); parent.isValid(); parent = parent.parent())
            depth++;
        return depth;
    }

    int textMargin() const
    {
        return this->style()->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, this) + 1;
    }

    // Left of a column in the viewport
    int columnPosition(int column) const
    {
        return this->header()->sectionViewportPosition(column);
    }

    void relayout()
    {
        this->scheduleDelayedItemsLayout();
    }

protected:
    void resizeEvent(QResizeEvent * event) override
    {
        QTreeWidget::resizeEvent(event);
        if (event->size().width() != event->oldSize().width())
            this->scheduleDelayedItemsLayout();
    }
};

namespace
{
class DetailDelegate : public QStyledItemDelegate
{
public:
    explicit DetailDelegate(TimbreDetailTree * tree, QColor secondary) : QStyledItemDelegate(tree),
        _tree(tree),
        _secondary(secondary)
    {}

    // Natural size of a cell; the widths of the texts written over other columns are dealt with by the widget
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        int kind = kindOf(index);
        if (kind == KIND_NOTE)
        {
            if (index.column() != COLUMN_NAME)
                return QSize(0, 0);
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            int margin = _tree->textMargin();
            int width = qMax(50, _tree->header()->length() - _tree->indentation() * (_tree->depth(index) + 1) - 2 * margin);
            QRect rect = opt.fontMetrics.boundingRect(QRect(0, 0, width, 100000), Qt::AlignLeft | Qt::TextWordWrap, opt.text);
            return QSize(0, rect.height() + 4);
        }
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        if (index.column() == COLUMN_BADGE)
        {
            int count = static_cast<int>(iconsShown(index).count());
            int iconSize = _tree->iconSize().width();
            size.setWidth(count > 0 ? 2 * _tree->textMargin() + count * iconSize + (count - 1) * ICON_SPACING : 0);
            size.setHeight(qMax(size.height(), iconSize));
        }
        else if (kind == KIND_SECTION && index.column() == COLUMN_NAME)
        {
            QString summary = summaryShown(index);
            if (!summary.isEmpty())
                size.rwidth() += PADDING + _tree->fontMetrics().horizontalAdvance(summary);
        }
        return size;
    }

    void paint(QPainter * painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        // Cells covered by a text written over them
        int kind = kindOf(index);
        bool title = kind == KIND_SECTION || kind == KIND_GROUP;
        bool alone = kind == KIND_ROW && !hasMeasure(index);
        if ((title && index.column() > COLUMN_NAME && index.column() < COLUMN_BADGE) ||
            (alone && index.column() == COLUMN_MEASURE))
            return;

        // Icons of the labels
        if (index.column() == COLUMN_BADGE)
        {
            QStyleOptionViewItem opt = option;
            initStyleOption(&opt, index);
            opt.text.clear();
            opt.icon = QIcon();
            QStyle * style = opt.widget != nullptr ? opt.widget->style() : QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
            int iconSize = _tree->iconSize().width();
            QRect rect(opt.rect.left() + _tree->textMargin(), opt.rect.top() + (opt.rect.height() - iconSize) / 2, iconSize, iconSize);
            foreach (QVariant icon, iconsShown(index))
            {
                icon.value<QIcon>().paint(painter, rect);
                rect.translate(iconSize + ICON_SPACING, 0);
            }
            return;
        }

        if (index.column() != COLUMN_NAME || !(title || alone))
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        // Background and focus up to the values (a name alone) or up to the icons (a title), then the text
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        QString text = opt.text;
        opt.text.clear();
        opt.rect.setRight(qMax(opt.rect.right(), _tree->columnPosition(title ? COLUMN_BADGE : COLUMN_VALUE) - 1));
        QStyle * style = opt.widget != nullptr ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
        int margin = _tree->textMargin();
        QRect rect = opt.rect.adjusted(margin, 0, -margin, 0);
        painter->save();
        painter->setFont(opt.font);
        painter->setPen(opt.palette.color(QPalette::Text));
        QString elided = opt.fontMetrics.elidedText(text, Qt::ElideRight, rect.width());
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, elided);
        QString summary = kind == KIND_SECTION ? summaryShown(index) : QString();
        if (!summary.isEmpty())
        {
            rect.setLeft(rect.left() + opt.fontMetrics.horizontalAdvance(elided) + PADDING);
            painter->setFont(_tree->font());
            painter->setPen(_secondary);
            painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter,
                              _tree->fontMetrics().elidedText(summary, Qt::ElideRight, rect.width()));
        }
        painter->restore();
    }

protected:
    void initStyleOption(QStyleOptionViewItem * option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);
        if (kindOf(index) == KIND_NOTE)
            option->features |= QStyleOptionViewItem::WrapText;
    }

private:
    bool folded(const QModelIndex &index) const
    {
        return !_tree->isExpanded(index.sibling(index.row(), COLUMN_NAME));
    }

    QString summaryShown(const QModelIndex &index) const
    {
        return folded(index) ? index.sibling(index.row(), COLUMN_NAME).data(ROLE_SUMMARY).toString() : QString();
    }

    // Icons of a row, or of the rows of a folded section
    QVariantList iconsShown(const QModelIndex &index) const
    {
        if (kindOf(index) == KIND_SECTION && !folded(index))
            return QVariantList();
        return index.sibling(index.row(), COLUMN_BADGE).data(ROLE_ICONS).toList();
    }

    TimbreDetailTree * _tree;
    QColor _secondary;
};
}

TimbreDetailWidget::Badge TimbreDetailWidget::makeBadge(TimbrePalette::Shape shape, QColor color, QString text, QString tooltip)
{
    Badge badge;
    badge.valid = true;
    badge.shape = shape;
    badge.color = color;
    badge.text = text;
    badge.tooltip = tooltip;
    return badge;
}

TimbreDetailWidget::TimbreDetailWidget(QWidget * parent) : QWidget(parent),
    _windowPalette(TimbrePalette::SURFACE_WINDOW),
    _listPalette(TimbrePalette::SURFACE_LIST),
    _titleBadge(nullptr),
    _subtitleBadge(nullptr),
    _currentSection(nullptr),
    _currentGroup(nullptr),
    _building(false),
    _neededWidth(0)
{
    // Unfolded sections
    for (int i = 0; i < SECTION_COUNT; i++)
        _expanded[i] = false;
    QStringList expanded = ContextManager::configuration()->getValue(
                ConfManager::SECTION_DISPLAY, EXPANDED_KEY, EXPANDED_DEFAULT).toString().split(",", Qt::SkipEmptyParts);
    foreach (QString value, expanded)
    {
        bool ok;
        int section = value.toInt(&ok);
        if (ok && section >= 0 && section < SECTION_COUNT)
            _expanded[section] = true;
    }

    // Names are darker (or lighter) than the values, units, column names, notes and summaries even more
    _labelColor = ThemeManager::mix(_listPalette.background(), _listPalette.text(), 0.75);
    _secondaryColor = ThemeManager::mix(_listPalette.background(), _listPalette.text(), 0.62);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 6); // Small margin below the list
    layout->setSpacing(0);

    // Fixed header: the key, the label of its root key and the notices
    _header = new QWidget(this);
    QVBoxLayout * headerLayout = new QVBoxLayout(_header);
    headerLayout->setContentsMargins(2, 6, 2, 4);
    headerLayout->setSpacing(2);
    _titleLayout = new QHBoxLayout();
    _titleLayout->setSpacing(10);
    _title = new QLabel(_header);
    _title->setTextFormat(Qt::PlainText);
    QFont font = _title->font();
    font.setBold(true);
    _title->setFont(font);
    _titleLayout->addWidget(_title);
    _titleLayout->addStretch();
    headerLayout->addLayout(_titleLayout);
    _subtitleLine = new QWidget(_header);
    _subtitleLayout = new QHBoxLayout(_subtitleLine);
    _subtitleLayout->setContentsMargins(0, 0, 0, 0);
    _subtitleLayout->setSpacing(10);
    _subtitle = new QLabel(_subtitleLine);
    _subtitle->setTextFormat(Qt::PlainText);
    _subtitleLayout->addWidget(_subtitle);
    _subtitleLayout->addStretch();
    headerLayout->addWidget(_subtitleLine);
    _noticeLayout = new QVBoxLayout();
    _noticeLayout->setContentsMargins(0, 0, 0, 0);
    _noticeLayout->setSpacing(2);
    headerLayout->addLayout(_noticeLayout);
    layout->addWidget(_header);

    // Sections in a tree, on the background of the lists
    _tree = new TimbreDetailTree(this);
    _tree->setColumnCount(COLUMN_COUNT);
    _tree->setHeaderHidden(true);
    _tree->header()->setMinimumSectionSize(0);
    _tree->header()->setDefaultSectionSize(0); // The last column only takes the remaining width
    _tree->header()->setStretchLastSection(true);
    _tree->setSelectionMode(QAbstractItemView::NoSelection);
    _tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tree->setFocusPolicy(Qt::TabFocus); // A click does not take the focus from the map
    _tree->setExpandsOnDoubleClick(false); // A click folds or unfolds
    _tree->setUniformRowHeights(false);
    _tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    _tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    int iconSize = qRound(0.8 * this->fontMetrics().height());
    _tree->setIconSize(QSize(iconSize, iconSize));
    _tree->setItemDelegate(new DetailDelegate(_tree, _secondaryColor));
    layout->addWidget(_tree, 1);
    layout->addStretch(0); // Only when the tree is hidden: the header stays at the top
    connect(_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
    connect(_tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onExpansionChanged(QTreeWidgetItem*)));
    connect(_tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onExpansionChanged(QTreeWidgetItem*)));

    _header->hide();
    _tree->hide();
}

QSize TimbreDetailWidget::sizeHint() const
{
    return QSize(_neededWidth, QWidget::sizeHint().height());
}

void TimbreDetailWidget::resetWidth()
{
    _neededWidth = 0; // Computed again at the next update
}

void TimbreDetailWidget::updateWidth()
{
    // Widest line of the header (a wrapped notice counts for its longest word) and columns of the tree, the vertical
    // scroll bar being reserved
    int width = _header->isHidden() ? 0 : _header->minimumSizeHint().width();
    if (!_tree->isHidden())
    {
        int columns = 0;
        for (int column = 0; column < COLUMN_FILL; column++)
            if (!_tree->isColumnHidden(column))
                columns += _tree->columnWidth(column);
        width = qMax(width, columns + 2 * _tree->frameWidth() + _tree->verticalScrollBar()->sizeHint().width());
    }
    if (width > _neededWidth)
    {
        _neededWidth = width;
        this->updateGeometry();
    }
}

void TimbreDetailWidget::updateColumns()
{
    // Widths of the unfolded lines: a name without measure is written over the column of the measures and a title over
    // the columns up to the icons, these columns being then widened if needed
    int widths[COLUMN_COUNT] = {};
    int alone = 0; // From the left of the tree to the values
    int over = 0; // From the left of the tree to the icons
    int indentation = _tree->indentation();
    std::function<void(QTreeWidgetItem *, int)> measure = [&](QTreeWidgetItem * item, int depth) {
        int kind = item->data(COLUMN_NAME, ROLE_KIND).toInt();
        int indent = indentation * (depth + 1);
        if (kind == KIND_SECTION || kind == KIND_GROUP)
            over = qMax(over, indent + _tree->cellSize(item, COLUMN_NAME).width());
        else if (kind == KIND_ROW)
        {
            int name = item->text(COLUMN_NAME).isEmpty() ? 0 : indent + _tree->cellSize(item, COLUMN_NAME).width() + PADDING;
            if (item->text(COLUMN_MEASURE).isEmpty())
                alone = qMax(alone, name);
            else
            {
                widths[COLUMN_NAME] = qMax(widths[COLUMN_NAME], name);
                widths[COLUMN_MEASURE] = qMax(widths[COLUMN_MEASURE], _tree->cellSize(item, COLUMN_MEASURE).width() + PADDING);
            }
            widths[COLUMN_VALUE] = qMax(widths[COLUMN_VALUE], _tree->cellSize(item, COLUMN_VALUE).width());
            widths[COLUMN_UNIT] = qMax(widths[COLUMN_UNIT], _tree->cellSize(item, COLUMN_UNIT).width());
        }
        if (kind != KIND_NOTE)
            widths[COLUMN_BADGE] = qMax(widths[COLUMN_BADGE], _tree->cellSize(item, COLUMN_BADGE).width());
        if (item->isExpanded())
            for (int i = 0; i < item->childCount(); i++)
                measure(item->child(i), depth + 1);
    };
    for (int i = 0; i < _tree->topLevelItemCount(); i++)
        measure(_tree->topLevelItem(i), 0);

    // Column of the measures only if a line is written on several rows, kept next to the values
    _tree->setColumnHidden(COLUMN_MEASURE, widths[COLUMN_MEASURE] == 0);
    if (alone > widths[COLUMN_NAME] + widths[COLUMN_MEASURE])
        widths[COLUMN_NAME] = alone - widths[COLUMN_MEASURE];
    int total = 0;
    for (int column = COLUMN_NAME; column <= COLUMN_UNIT; column++)
        total += widths[column];
    if (over > total)
        widths[COLUMN_UNIT] += over - total;

    // The last column takes the remaining width, none if there is no room (otherwise it would keep its previous width)
    _tree->setColumnWidth(COLUMN_FILL, 0);
    for (int column = 0; column < COLUMN_FILL; column++)
        _tree->setColumnWidth(column, widths[column]);
    _tree->relayout(); // Heights of the notes
}

QWidget * TimbreDetailWidget::createBadge(const Badge &badge, QWidget * parent)
{
    QWidget * widget = new QWidget(parent);
    QHBoxLayout * layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // Shape drawn as in the map
    int size = qRound(0.8 * widget->fontMetrics().height());
    qreal ratio = this->devicePixelRatioF();
    QPixmap pixmap(qCeil(size * ratio), qCeil(size * ratio));
    pixmap.setDevicePixelRatio(ratio);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    TimbrePalette::drawShape(painter, badge.shape, QRectF(0, 0, size, size), _windowPalette.readable(badge.color, 3));
    painter.end();
    QLabel * icon = new QLabel(widget);
    icon->setPixmap(pixmap);
    layout->addWidget(icon);

    if (!badge.text.isEmpty())
    {
        QLabel * text = new QLabel(badge.text, widget);
        text->setTextFormat(Qt::PlainText);
        QFont font = text->font();
        font.setBold(true);
        text->setFont(font);
        text->setStyleSheet("QLabel{color:" + _windowPalette.readable(badge.color, 4.5).name() + "}");
        layout->addWidget(text);
    }
    widget->setToolTip(badge.tooltip);
    return widget;
}

void TimbreDetailWidget::setBadge(QHBoxLayout * layout, QWidget *&current, const Badge &badge)
{
    delete current;
    current = nullptr;
    if (badge.valid)
    {
        current = createBadge(badge, layout->parentWidget());
        layout->insertWidget(1, current); // After the text
    }
}

void TimbreDetailWidget::setBadge(QTreeWidgetItem * item, const Badge &badge)
{
    if (!badge.valid)
        return;

    // Shape drawn as in the map, color made readable on the background of the list, named by the tooltip
    int size = _tree->iconSize().width();
    qreal ratio = this->devicePixelRatioF();
    QPixmap pixmap(qCeil(size * ratio), qCeil(size * ratio));
    pixmap.setDevicePixelRatio(ratio);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    TimbrePalette::drawShape(painter, badge.shape, QRectF(0, 0, size, size), _listPalette.readable(badge.color, 3));
    painter.end();
    QVariant icon = QVariant::fromValue(QIcon(pixmap));
    QString tooltip = badge.text;
    if (!badge.tooltip.isEmpty())
        tooltip += (tooltip.isEmpty() ? "" : "\n") + badge.tooltip;
    item->setData(COLUMN_BADGE, ROLE_ICONS, QVariantList() << icon);
    item->setToolTip(COLUMN_BADGE, tooltip);

    // Also on the line of the section, once per shape and color, shown when the section is folded
    QString key = QString::number(static_cast<int>(badge.shape)) + badge.color.name();
    QStringList keys = _currentSection->data(COLUMN_BADGE, ROLE_ICON_KEYS).toStringList();
    if (!keys.contains(key))
    {
        _currentSection->setData(COLUMN_BADGE, ROLE_ICON_KEYS, keys << key);
        _currentSection->setData(COLUMN_BADGE, ROLE_ICONS, _currentSection->data(COLUMN_BADGE, ROLE_ICONS).toList() << icon);
        QString tooltips = _currentSection->data(COLUMN_BADGE, ROLE_ICON_TOOLTIP).toString();
        _currentSection->setData(COLUMN_BADGE, ROLE_ICON_TOOLTIP, tooltips.isEmpty() ? tooltip : tooltips + "\n\n" + tooltip);
    }
}

void TimbreDetailWidget::updateSectionBadges()
{
    // Tooltip of the icons of a section only when they are shown
    for (int i = 0; i < _tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = _tree->topLevelItem(i);
        item->setToolTip(COLUMN_BADGE, item->isExpanded() ? QString() : item->data(COLUMN_BADGE, ROLE_ICON_TOOLTIP).toString());
    }
}

QTreeWidgetItem * TimbreDetailWidget::createItem(int kind)
{
    QTreeWidgetItem * item;
    if (kind == KIND_SECTION)
        item = new QTreeWidgetItem(_tree);
    else if (_currentGroup != nullptr && kind != KIND_GROUP)
        item = new QTreeWidgetItem(_currentGroup);
    else
        item = new QTreeWidgetItem(_currentSection);
    item->setFlags(Qt::ItemIsEnabled);
    item->setData(COLUMN_NAME, ROLE_KIND, kind);
    return item;
}

void TimbreDetailWidget::beginUpdate()
{
    this->setUpdatesEnabled(false);
    _building = true;
    qDeleteAll(_notices);
    _notices.clear();
    _tree->clear();
    _currentSection = _currentGroup = nullptr;
    setTitle("");
    setSubtitle("");
}

void TimbreDetailWidget::setTitle(QString text, const Badge &badge)
{
    _title->setText(text);
    setBadge(_titleLayout, _titleBadge, badge);
}

void TimbreDetailWidget::setSubtitle(QString text, const Badge &badge)
{
    _subtitle->setText(text);
    setBadge(_subtitleLayout, _subtitleBadge, badge);
    _subtitleLine->setVisible(!text.isEmpty());
}

void TimbreDetailWidget::addNotice(QString text)
{
    QLabel * label = new QLabel(text, _header);
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);
    QFont font = label->font();
    font.setItalic(true);
    label->setFont(font);
    _noticeLayout->addWidget(label);
    _notices << label;
}

void TimbreDetailWidget::beginSection(Section section, QString title, QString summary)
{
    QTreeWidgetItem * item = createItem(KIND_SECTION);
    item->setData(COLUMN_NAME, ROLE_SECTION, static_cast<int>(section));
    item->setData(COLUMN_NAME, ROLE_SUMMARY, summary);
    item->setText(COLUMN_NAME, title);
    QFont font = _tree->font();
    font.setBold(true);
    item->setFont(COLUMN_NAME, font);
    item->setForeground(COLUMN_NAME, _listPalette.text());
    item->setExpanded(_expanded[section]);
    _currentSection = item;
    _currentGroup = nullptr;
}

void TimbreDetailWidget::addRow(QString label, const Value &value, QString tooltip, const Badge &badge)
{
    addRow(label, QString(), value, tooltip, badge);
}

void TimbreDetailWidget::addRow(QString label, QString sublabel, const Value &value, QString tooltip, const Badge &badge)
{
    if (_currentSection == nullptr)
        return;
    QTreeWidgetItem * item = createItem(KIND_ROW);
    item->setText(COLUMN_NAME, label);
    item->setForeground(COLUMN_NAME, _labelColor);
    item->setText(COLUMN_MEASURE, sublabel);
    item->setForeground(COLUMN_MEASURE, _labelColor);
    item->setText(COLUMN_VALUE, value.text);
    item->setTextAlignment(COLUMN_VALUE, Qt::AlignRight | Qt::AlignVCenter);
    item->setForeground(COLUMN_VALUE, _listPalette.text());
    item->setText(COLUMN_UNIT, value.unit);
    item->setForeground(COLUMN_UNIT, _secondaryColor);
    for (int column = COLUMN_NAME; column <= COLUMN_UNIT; column++)
        item->setToolTip(column, tooltip);
    setBadge(item, badge);
}

void TimbreDetailWidget::addSubtitle(QString text)
{
    if (_currentSection == nullptr)
        return;
    QTreeWidgetItem * item = createItem(KIND_GROUP);
    item->setText(COLUMN_NAME, text);
    QFont font = _tree->font();
    font.setWeight(QFont::DemiBold);
    item->setFont(COLUMN_NAME, font);
    item->setForeground(COLUMN_NAME, _listPalette.text());
    item->setExpanded(true);
    _currentGroup = item;
}

void TimbreDetailWidget::addNote(QString text)
{
    if (_currentSection == nullptr)
        return;
    QTreeWidgetItem * item = createItem(KIND_NOTE);
    item->setText(COLUMN_NAME, text);
    QFont font = _tree->font();
    font.setItalic(true);
    item->setFont(COLUMN_NAME, font);
    item->setForeground(COLUMN_NAME, _secondaryColor);
    item->setFirstColumnSpanned(true);
}

void TimbreDetailWidget::endUpdate()
{
    _header->setVisible(!_title->text().isEmpty() || !_notices.isEmpty());
    _title->setVisible(!_title->text().isEmpty());
    _tree->setVisible(_tree->topLevelItemCount() > 0);
    _currentSection = _currentGroup = nullptr;
    _building = false;
    updateSectionBadges();
    updateColumns();
    updateWidth();
    this->setUpdatesEnabled(true);
}

void TimbreDetailWidget::onItemClicked(QTreeWidgetItem * item, int column)
{
    Q_UNUSED(column)
    int kind = item->data(COLUMN_NAME, ROLE_KIND).toInt();
    if (kind == KIND_SECTION || kind == KIND_GROUP)
        item->setExpanded(!item->isExpanded());
}

void TimbreDetailWidget::onExpansionChanged(QTreeWidgetItem * item)
{
    if (_building)
        return;
    if (item->data(COLUMN_NAME, ROLE_KIND).toInt() == KIND_SECTION)
    {
        _expanded[item->data(COLUMN_NAME, ROLE_SECTION).toInt()] = item->isExpanded();
        QStringList values;
        for (int i = 0; i < SECTION_COUNT; i++)
            if (_expanded[i])
                values << QString::number(i);
        ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, EXPANDED_KEY, values.join(","));
    }
    updateSectionBadges();
    updateColumns();
    updateWidth();
}
