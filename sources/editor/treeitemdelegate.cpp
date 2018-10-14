#include "treeitemdelegate.h"
#include <QPainter>
#include "treeview.h"
#include "contextmanager.h"
#include "basetypes.h"
#include <QToolTip>
#include <QHelpEvent>

TreeItemDelegate::Icons * TreeItemDelegate::s_icons = NULL;
TreeItemDelegate::Colors * TreeItemDelegate::s_colors = NULL;
const int TreeItemDelegate::MARGIN = 8;

TreeItemDelegate::TreeItemDelegate(QObject * parent) : QStyledItemDelegate(parent)
{
    // Resources
    if (s_colors == NULL)
        s_colors = new Colors();
    if (s_icons == NULL)
        s_icons = new Icons();
}

TreeItemDelegate::Colors::Colors()
{
    _alternateColor = ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND);
    _borderColor = ContextManager::theme()->isDark(ThemeManager::LIST_ALTERNATIVE_BACKGROUND, ThemeManager::LIST_TEXT) ?
                _alternateColor.lighter(180) : _alternateColor.darker(130);
    _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    _highlightColorBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _highlightColorText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
}

QPixmap TreeItemDelegate::Icons::getPixmap(QString name, int colorType)
{
    QString key = name + QString::number(colorType);
    if (!_pixmaps.contains(key))
        _pixmaps[key] = ContextManager::theme()->getColoredSvg(":/icons/" + name + ".svg", QSize(16, 16), (ThemeManager::ColorType)colorType);
    return _pixmaps[key];
}

void TreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    painter->setRenderHint(QPainter::Qt4CompatiblePainting);

    // Draw an entry depending on the type of the element to display
    EltID currentId = index.data(Qt::UserRole).value<EltID>();
    switch (currentId.typeElement)
    {
    case elementSf2:
        drawRoot(painter, option, index, false);
        break;
    case elementRootSmpl:
    case elementRootInst:
    case elementRootPrst:
        drawRoot(painter, option, index, true);
        break;
    case elementSmpl:
        drawElement(painter, option, index, "sample", false, false);
        break;
    case elementInst:
        drawElement(painter, option, index, "instrument", true, false);
        break;
    case elementPrst:
        drawElement(painter, option, index, "preset", true, false);
        break;
    case elementInstSmpl:
        drawElement(painter, option, index, "sample", false, true);
        break;
    case elementPrstInst:
        drawElement(painter, option, index, "instrument", false, true);
        break;
    default:
        break;
    }
}

QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize hint = QStyledItemDelegate::sizeHint(option, index);
    EltID currentId = index.data(Qt::UserRole).value<EltID>();
    if (currentId.typeElement == elementSf2 || currentId.typeElement == elementRootSmpl ||
            currentId.typeElement == elementRootInst || currentId.typeElement == elementRootPrst)
        hint.setHeight(hint.height() * 2.5);
    return hint;
}

void TreeItemDelegate::drawRoot(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, bool expandable) const
{
    // Background
    QRect rect = option.rect;
    if (expandable)
        rect.adjust(MARGIN - 2, MARGIN / 2, -MARGIN, -MARGIN / 2);
    else
        rect.adjust(MARGIN - 2, MARGIN, -MARGIN, -MARGIN / 2);
    QPainterPath path;
    path.addRoundedRect(rect, 2, 2);

    bool highlighted = option.state & QStyle::State_Selected;
    if (highlighted)
    {
        painter->fillPath(path, s_colors->_highlightColorBackground);

        QPen pen(s_colors->_highlightColorBackground, 1);
        painter->setPen(pen);
        painter->drawPath(path);
    }
    else
    {
        painter->fillPath(path, s_colors->_alternateColor);

        QPen pen(s_colors->_borderColor, 1);
        painter->setPen(pen);
        painter->drawPath(path);
    }

    // Right arrow
    int rightAreaWidth = 0;
    if (expandable)
    {
        const TreeView* view = qobject_cast<const TreeView*>(option.widget);
        QPixmap pix = s_icons->getPixmap(view->isExpanded(index) ? "arrow_down" : "arrow_up",
                                         highlighted ? ThemeManager::HIGHLIGHTED_TEXT : ThemeManager::LIST_TEXT);
        int x = rect.right() - pix.width();
        int y = rect.top() + (rect.height() - pix.height()) / 2;
        painter->drawPixmap(x - MARGIN, y, pix);

        painter->setPen(QPen(s_colors->_borderColor, 1));
        painter->drawLine(x - 2 * MARGIN, rect.top(), x - 2 * MARGIN, rect.bottom());

        rightAreaWidth = pix.width() + 2 * MARGIN;
    }

    // Font
    QFont font = option.font;
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.2);

    // Rect
    QRect rectText = rect;
    rectText.adjust(MARGIN, 0, -MARGIN - rightAreaWidth, 0);

    // Text
    QString text = index.data().toString();
    QFontMetrics metrics(font);
    text = metrics.elidedText(text, Qt::ElideMiddle, rectText.width() - 1);

    // Draw
    rectText.adjust(0, (rectText.height() - metrics.height()) / 2, 0, 0);
    painter->setPen(highlighted ? s_colors->_highlightColorText : s_colors->_textColor);
    painter->setFont(font);
    painter->drawText(rectText, text);
}

void TreeItemDelegate::drawElement(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, QString iconName, bool expandable, bool withIndent) const
{
    // Background
    bool highlighted = option.state & QStyle::State_Selected;
    if (highlighted)
        painter->fillRect(option.rect, s_colors->_highlightColorBackground);

    // Indent?
    QRect rect = option.rect;
    if (withIndent)
        rect.adjust(16, 0, 0, 0);

    // Left icon
    QPixmap pix = s_icons->getPixmap(iconName, highlighted ? ThemeManager::HIGHLIGHTED_TEXT : ThemeManager::HIGHLIGHTED_BACKGROUND);
    painter->drawPixmap(rect.left() + MARGIN, rect.top() + (rect.height() - pix.height()) / 2, pix);

    // Right arrow
    int rightAreaWidth = 0;
    if (expandable)
    {
        const TreeView* view = qobject_cast<const TreeView*>(option.widget);
        QPixmap pix = s_icons->getPixmap(view->isExpanded(index) ? "arrow_down" : "arrow_up",
                                         highlighted ? ThemeManager::HIGHLIGHTED_TEXT : ThemeManager::HIGHLIGHTED_BACKGROUND);
        int x = rect.right() - pix.width() - MARGIN;
        int y = rect.top() + (rect.height() - pix.height()) / 2;
        painter->drawPixmap(x - MARGIN, y, pix);

        rightAreaWidth = pix.width() + 2 * MARGIN;
    }

    // Text
    QRect rectText = rect;
    rectText.adjust(MARGIN + 20, 0, -MARGIN - rightAreaWidth, 0);
    QString text = index.data().toString();
    QFontMetrics metrics(option.font);
    text = metrics.elidedText(text, Qt::ElideMiddle, rectText.width() - 1);

    // Draw
    rectText.adjust(0, (rectText.height() - metrics.height()) / 2, 0, 0);
    painter->setPen(highlighted ? s_colors->_highlightColorText : s_colors->_textColor);
    painter->setFont(option.font);
    painter->drawText(rectText, text);
}

bool TreeItemDelegate::helpEvent(QHelpEvent* e, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (!e || !view)
        return false;

    ElementType type = index.data(Qt::UserRole).value<EltID>().typeElement;
    if (e->type() == QEvent::ToolTip && type != elementSf2 && type != elementRootSmpl && type != elementRootInst && type != elementRootPrst)
    {
        QRect rect = view->visualRect(index);
        QSize size = sizeHint(option, index);
        int offset = 0;
        switch (type)
        {
        case elementSmpl:
            offset = 36;
            break;
        case elementInst: case elementPrst:
            offset = 61;
            break;
        case elementInstSmpl: case elementPrstInst:
            offset = 54;
            break;
        default:
            break;
        }

        if (rect.width() < size.width() + offset)
        {
            QVariant tooltip = index.data(Qt::DisplayRole);
            if (tooltip.canConvert<QString>())
            {
                QToolTip::showText(e->globalPos(), QString("<div>%1</div>")
                                   .arg(tooltip.toString().toHtmlEscaped()), view);
                return true;
            }
        }
        if (!QStyledItemDelegate::helpEvent(e, view, option, index))
            QToolTip::hideText();
        return true;
    }

    return QStyledItemDelegate::helpEvent(e, view, option, index);
}
