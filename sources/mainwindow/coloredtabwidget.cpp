#include "coloredtabwidget.h"
#include "contextmanager.h"
#include "tabbar.h"

const QString ColoredTabWidget::s_styleSheetLastPart = "QTabWidget:pane {\
        border-right: 0px;\
border-left: 0px;\
border-bottom: 0px;\
top: -1px;\
}\
QTabBar::tab {\
    border-bottom: 0px;\
    border-top-left-radius: 4px;\
    border-top-right-radius: 4px;\
    border-bottom-left-radius: 0px;\
    border-bottom-right-radius: 0px;\
padding: 5px 1px 7px 15px;\
margin: 3px -1px 0 -1px;\
}\
QTabBar::tab:selected {\
                 border-bottom: 0px;\
padding-top: 5px;\
padding-right: 0px;\
margin-top: 0px;\
margin-right: 0px;\
}\
QTabBar::tab:last {\
                 margin-right: 0px;\
}";

ColoredTabWidget::ColoredTabWidget(QWidget *parent) : QTabWidget(parent),
    _lastWidget(NULL)
{
    // First tab doesn't move
    this->setTabBar(new TabBar());

    // Colors
    _defaultWindowColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND);
    _defaultTextColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
    QColor listColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    QColor listColor2 = listColor.darker(110);
    QColor listColor3 = listColor2.darker(130);
    QColor borderColor = ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ?
                _defaultWindowColor.lighter(120) : _defaultWindowColor.darker(150);
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Compute the first part of the stylesheet
    _styleSheetFirstPart = "QTabWidget:pane{border: 1px solid " + borderColor.name() + ";}" +
            "QTabBar::tab{border: 1px solid " + borderColor.name() + ";" +
            "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " + listColor.name() + ", stop: 0.85 " + listColor.name() +
            ", stop: 0.93 " + listColor2.name() + ", stop:1 " + listColor3.name() + ");}" +
            "QTabBar::tab:selected{border: 1px solid " + borderColor.name() + ";" +
            "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 " + highlightColor.name() + ", stop:0.10 " +
            highlightColor.name() + ", stop:0.15 ";

    // Apply default stylesheet, based on windowColor
    this->changeStyleSheet(_defaultWindowColor, _defaultTextColor);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

void ColoredTabWidget::changeStyleSheet(QColor backgroundColor, QColor textColor)
{
    this->setStyleSheet(_styleSheetFirstPart + backgroundColor.name() +
                        ");border-bottom: 1px solid " + backgroundColor.name() +
                        ";color:" + textColor.name() + "}" + s_styleSheetLastPart);
}

int ColoredTabWidget::addColoredTab(QWidget *widget, QString iconName, const QString &label, QColor backgroundColor, QColor textColor)
{
    // Store information about the new tab
    _tabInfo[widget]._backgroundColor = backgroundColor;
    _tabInfo[widget]._textColor = textColor;
    _tabInfo[widget]._iconName = iconName;

    return this->addTab(widget, ContextManager::theme()->getColoredSvg(iconName, QSize(32, 32), ThemeManager::WINDOW_TEXT), label);
}

void ColoredTabWidget::onCurrentChanged(int index)
{
    // Find the corresponding widget
    QWidget * currentWidget = this->widget(index);
    if (_tabInfo.contains(currentWidget))
    {
        changeStyleSheet(_tabInfo[currentWidget]._backgroundColor, _tabInfo[currentWidget]._textColor);
        QMap<QString, QString> replacement;
        replacement["currentColor"] = _tabInfo[currentWidget]._textColor.name();
        this->setTabIcon(index, ContextManager::theme()->getColoredSvg(_tabInfo[currentWidget]._iconName, QSize(32, 32), replacement));
    }
    else
        changeStyleSheet(_defaultWindowColor, _defaultTextColor);

    // Restore the icon of the previous index
    if (_lastWidget != NULL && _tabInfo.contains(_lastWidget) && this->indexOf(_lastWidget) != -1 && currentWidget != _lastWidget)
    {
        int lastIndex = this->indexOf(_lastWidget);
        QMap<QString, QString> replacement;
        replacement["currentColor"] = _defaultTextColor.name();
        this->setTabIcon(lastIndex, ContextManager::theme()->getColoredSvg(_tabInfo[_lastWidget]._iconName, QSize(32, 32), replacement));
    }

    // Store the new widget
    _lastWidget = currentWidget;
}
