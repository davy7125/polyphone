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
#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>
#include <QtMath>
#include <functional>

static const char * EXPANDED_KEY = "timbre_detail_expanded";
static const char * EXPANDED_DEFAULT = "0,1,2"; // Deviation, junctions and rendering

namespace
{
// Header of a section: arrow, title and summary; a click, the space bar or the return key folds or unfolds it
class SectionHeader : public QWidget
{
public:
    SectionHeader(QWidget * parent, int indent, QColor arrow, QColor hover) : QWidget(parent),
        _indent(indent),
        _arrow(arrow),
        _hover(hover),
        _expanded(false)
    {
        this->setCursor(Qt::PointingHandCursor);
        this->setFocusPolicy(Qt::TabFocus);
        this->setAttribute(Qt::WA_Hover, true); // Repainted when the mouse enters or leaves
    }

    void setExpanded(bool expanded)
    {
        _expanded = expanded;
        this->update();
    }

    std::function<void()> clicked;

protected:
    void paintEvent(QPaintEvent * event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        if (this->underMouse())
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(_hover);
            painter.drawRoundedRect(QRectF(this->rect()), 3, 3);
        }
        if (this->hasFocus())
        {
            QStyleOptionFocusRect option;
            option.initFrom(this);
            this->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &painter, this);
        }

        // Chevron, pointing down if the section is unfolded
        double half = 0.2 * _indent;
        QPointF center(0.5 * _indent, 0.5 * this->height());
        QPolygonF chevron;
        if (_expanded)
            chevron << center + QPointF(-half, -0.5 * half) << center + QPointF(0, 0.5 * half)
                    << center + QPointF(half, -0.5 * half);
        else
            chevron << center + QPointF(-0.5 * half, -half) << center + QPointF(0.5 * half, 0)
                    << center + QPointF(-0.5 * half, half);
        painter.setPen(QPen(_arrow, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolyline(chevron);
    }

    void mousePressEvent(QMouseEvent * event) override
    {
        event->accept(); // The release comes here
    }

    void mouseReleaseEvent(QMouseEvent * event) override
    {
        if (event->button() == Qt::LeftButton && this->rect().contains(event->pos()) && clicked)
            clicked();
    }

    void keyPressEvent(QKeyEvent * event) override
    {
        if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && clicked)
            clicked();
        else
            QWidget::keyPressEvent(event);
    }

private:
    int _indent;
    QColor _arrow, _hover;
    bool _expanded;
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

TimbreDetailWidget::Badge TimbreDetailWidget::shapeOnly(const Badge &badge)
{
    Badge result = badge;
    result.text.clear();
    return result;
}

TimbreDetailWidget::TimbreDetailWidget(QWidget * parent) : QWidget(parent),
    _palette(TimbrePalette::SURFACE_WINDOW),
    _titleBadge(nullptr),
    _subtitleBadge(nullptr),
    _currentSection(SECTION_COUNT),
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

    // Labels are darker (or lighter) than the values, notes and summaries even more
    QColor background = _palette.background();
    QColor text = _palette.text();
    QString rowLabel = ThemeManager::mix(background, text, 0.75).name();
    QString secondary = ThemeManager::mix(background, text, 0.62).name();
    this->setStyleSheet("QLabel#detailRowLabel{color:" + rowLabel + "}"
                        "QLabel#detailSummary,QLabel#detailColumnHeader,QLabel#detailNote{color:" + secondary + "}"
                        "QFrame#detailSeparator{background-color:" + ThemeManager::mix(background, text, 0.15).name() + "}");
    _indent = this->fontMetrics().height();

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Fixed header: the key and the label of its root key
    _header = new QWidget(this);
    QVBoxLayout * headerLayout = new QVBoxLayout(_header);
    headerLayout->setContentsMargins(6, 6, 6, 0);
    headerLayout->setSpacing(2);
    _titleLayout = new QHBoxLayout();
    _titleLayout->setSpacing(10);
    _title = new QLabel(_header);
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
    _subtitleLayout->addWidget(_subtitle);
    _subtitleLayout->addStretch();
    headerLayout->addWidget(_subtitleLine);
    headerLayout->addSpacing(3);
    QFrame * separator = new QFrame(_header);
    separator->setObjectName("detailSeparator");
    separator->setFixedHeight(1);
    headerLayout->addWidget(separator);
    layout->addWidget(_header);

    // Notices and sections, scrolled
    _scroll = new QScrollArea(this);
    _scroll->setWidgetResizable(true);
    _scroll->setFrameShape(QFrame::NoFrame);
    _content = new QWidget(_scroll);
    _contentLayout = new QVBoxLayout(_content);
    _contentLayout->setContentsMargins(6, 4, 6, 6);
    _contentLayout->setSpacing(2);
    for (int i = 0; i < SECTION_COUNT; i++)
        createSection(static_cast<Section>(i));
    _contentLayout->addStretch(1);
    _scroll->setWidget(_content);
    layout->addWidget(_scroll, 1);

    _header->hide();
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
    // Widest line of the header and of the visible content (a wrapped text counts for its longest word), the vertical
    // scroll bar being reserved
    int width = _content->minimumSizeHint().width() + _scroll->verticalScrollBar()->sizeHint().width();
    if (!_header->isHidden())
        width = qMax(width, _header->minimumSizeHint().width());
    if (width > _neededWidth)
    {
        _neededWidth = width;
        this->updateGeometry();
    }
}

QLabel * TimbreDetailWidget::createLabel(QString text, QString objectName, QWidget * parent)
{
    QLabel * label = new QLabel(text, parent);
    label->setObjectName(objectName);
    label->setTextFormat(Qt::PlainText);
    return label;
}

void TimbreDetailWidget::createSection(Section section)
{
    SectionWidgets &s = _sections[section];
    s.container = new QWidget();
    QVBoxLayout * layout = new QVBoxLayout(s.container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    SectionHeader * header = new SectionHeader(s.container, _indent, ThemeManager::mix(_palette.background(), _palette.text(), 0.6),
                                               ThemeManager::mix(_palette.background(), _palette.text(), 0.08));
    header->clicked = [this, section]() { setExpanded(section, !_expanded[section]); };
    QHBoxLayout * headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(_indent, 2, 4, 2);
    headerLayout->setSpacing(10);
    s.title = new QLabel(header);
    QFont font = s.title->font();
    font.setBold(true);
    s.title->setFont(font);
    headerLayout->addWidget(s.title);
    headerLayout->addStretch(); // The badge is inserted before
    s.summary = createLabel("", "detailSummary", header);
    headerLayout->addWidget(s.summary);
    layout->addWidget(header);
    s.header = header;
    s.headerLayout = headerLayout;

    s.container->hide();
    _contentLayout->addWidget(s.container);
}

void TimbreDetailWidget::setExpanded(Section section, bool expanded)
{
    _expanded[section] = expanded;
    updateSection(section);
    updateWidth();

    QStringList values;
    for (int i = 0; i < SECTION_COUNT; i++)
        if (_expanded[i])
            values << QString::number(i);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, EXPANDED_KEY, values.join(","));
}

void TimbreDetailWidget::updateSection(Section section)
{
    SectionWidgets &s = _sections[section];
    static_cast<SectionHeader *>(s.header)->setExpanded(_expanded[section]);
    if (s.body != nullptr)
        s.body->setVisible(_expanded[section]);
    s.summary->setVisible(!_expanded[section] && !s.summary->text().isEmpty());
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
    TimbrePalette::drawShape(painter, badge.shape, QRectF(0, 0, size, size), _palette.readable(badge.color, 3));
    painter.end();
    QLabel * icon = new QLabel(widget);
    icon->setPixmap(pixmap);
    layout->addWidget(icon);

    if (!badge.text.isEmpty())
    {
        QLabel * text = createLabel(badge.text, "", widget);
        QFont font = text->font();
        font.setBold(true);
        text->setFont(font);
        text->setStyleSheet("QLabel{color:" + _palette.readable(badge.color, 4.5).name() + "}");
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

void TimbreDetailWidget::beginUpdate()
{
    this->setUpdatesEnabled(false);
    qDeleteAll(_notices);
    _notices.clear();
    for (int i = 0; i < SECTION_COUNT; i++)
        _sections[i].used = false;
    _currentSection = SECTION_COUNT;
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
    QLabel * label = createLabel(text, "detailNotice", nullptr);
    label->setWordWrap(true);
    QFont font = label->font();
    font.setItalic(true);
    label->setFont(font);
    label->setContentsMargins(0, 0, 0, 4);
    _contentLayout->insertWidget(static_cast<int>(_notices.count()), label); // Before the sections
    _notices << label;
}

void TimbreDetailWidget::beginSection(Section section, QString title, QString summary, const Badge &badge)
{
    SectionWidgets &s = _sections[section];
    s.used = true;
    s.title->setText(title);
    s.summary->setText(summary);
    setBadge(s.headerLayout, s.badge, badge);
    delete s.body;
    s.body = new QWidget(s.container);
    QHBoxLayout * bodyLayout = new QHBoxLayout(s.body);
    bodyLayout->setContentsMargins(_indent, 1, 0, 6);
    s.grid = new QGridLayout();
    s.grid->setContentsMargins(0, 0, 0, 0);
    s.grid->setHorizontalSpacing(12);
    s.grid->setVerticalSpacing(1);
    bodyLayout->addLayout(s.grid);
    bodyLayout->addStretch(1); // Values close to their labels
    s.container->layout()->addWidget(s.body);
    s.row = 0;
    _currentSection = section;
}

void TimbreDetailWidget::addColumnHeaders(const QStringList &headers)
{
    if (_currentSection == SECTION_COUNT)
        return;
    SectionWidgets &s = _sections[_currentSection];
    for (int i = 0; i < headers.count(); i++)
    {
        QLabel * label = createLabel(headers[i], "detailColumnHeader", s.body);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        s.grid->addWidget(label, s.row, i + 1);
    }
    s.row++;
}

void TimbreDetailWidget::addRow(QString label, const QStringList &values, QString tooltip, const Badge &badge)
{
    if (_currentSection == SECTION_COUNT)
        return;
    SectionWidgets &s = _sections[_currentSection];
    QLabel * labelWidget = createLabel(label, "detailRowLabel", s.body);
    labelWidget->setToolTip(tooltip);
    s.grid->addWidget(labelWidget, s.row, 0);
    int column = 1;
    foreach (QString value, values)
    {
        QLabel * valueWidget = createLabel(value, "", s.body);
        valueWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        valueWidget->setToolTip(tooltip);
        s.grid->addWidget(valueWidget, s.row, column++);
    }
    if (badge.valid)
        s.grid->addWidget(createBadge(badge, s.body), s.row, column);
    s.row++;
}

void TimbreDetailWidget::addSubtitle(QString text)
{
    if (_currentSection == SECTION_COUNT)
        return;
    SectionWidgets &s = _sections[_currentSection];
    QLabel * label = createLabel(text, "", s.body);
    QFont font = label->font();
    font.setWeight(QFont::DemiBold);
    label->setFont(font);
    if (s.row > 0)
        label->setContentsMargins(0, 4, 0, 0);
    s.grid->addWidget(label, s.row++, 0, 1, -1); // Over the columns used by the rows
}

void TimbreDetailWidget::addNote(QString text)
{
    if (_currentSection == SECTION_COUNT)
        return;
    SectionWidgets &s = _sections[_currentSection];
    QLabel * label = createLabel(text, "detailNote", s.body);
    label->setWordWrap(true);
    QFont font = label->font();
    font.setItalic(true);
    label->setFont(font);
    s.grid->addWidget(label, s.row++, 0, 1, -1);
}

void TimbreDetailWidget::endUpdate()
{
    _header->setVisible(!_title->text().isEmpty());
    for (int i = 0; i < SECTION_COUNT; i++)
    {
        // Each section aligns its own values: aligning all of them on the longest label would be too wide
        SectionWidgets &s = _sections[i];
        s.container->setVisible(s.used);
        if (s.used)
            updateSection(static_cast<Section>(i));
    }
    _currentSection = SECTION_COUNT;
    updateWidth();
    this->setUpdatesEnabled(true);
}
