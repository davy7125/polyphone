#include "configtocbutton.h"
#include "contextmanager.h"

QSize ConfigTocButton::ICON_SIZE(42, 42);

ConfigTocButton::ConfigTocButton(QWidget *parent) : QToolButton(parent)
{
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->setCursor(Qt::PointingHandCursor);
    QString styleSheet = "QToolButton:hover{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                ";} QToolButton{padding:12px 10px;border: none;}";
    this->setStyleSheet(styleSheet);
    this->setIconSize(ICON_SIZE);
}

void ConfigTocButton::setIconName(QString iconName)
{
    _icon = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, ThemeManager::LIST_TEXT);
    _iconHovered = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, ThemeManager::HIGHLIGHTED_TEXT);
    this->setIcon(_icon);
}

void ConfigTocButton::enterEvent(QEvent * event)
{
    this->setIcon(_iconHovered);
    QToolButton::enterEvent(event);
}

void ConfigTocButton::leaveEvent(QEvent * event)
{
    this->setIcon(_icon);
    QToolButton::leaveEvent(event);
}
