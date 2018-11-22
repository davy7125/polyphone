#ifndef COLOREDTABWIDGET_H
#define COLOREDTABWIDGET_H

#include <QTabWidget>
#include <QMap>
class QPushButton;

class ColoredTabInfo
{
public:
    QColor _backgroundColor;
    QColor _textColor;
    QString _iconName;
    QPushButton * _closeButton;
};


class ColoredTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    static const QSize TAB_ICON_SIZE;

    ColoredTabWidget(QWidget *parent = nullptr);
    int addColoredTab(QWidget *widget, QString iconName, const QString &label, QColor backgroundColor, QColor textColor);

private slots:
    void onCurrentChanged(int index);
    void onCloseButtonClicked();

private:
    void changeStyleSheet(QColor backgroundColor, QColor textColor);

    QWidget * _lastWidget;
    QColor _defaultWindowColor;
    QColor _defaultTextColor;
    QString _styleSheetFirstPart;
    static const QString s_styleSheetLastPart;
    QMap<QWidget *, ColoredTabInfo> _tabInfo;
};

#endif // COLOREDTABWIDGET_H
