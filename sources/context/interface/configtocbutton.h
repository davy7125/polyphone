#ifndef CONFIGTOCBUTTON_H
#define CONFIGTOCBUTTON_H

#include <QToolButton>

class ConfigTocButton : public QToolButton
{
public:
    ConfigTocButton(QWidget * parent = nullptr);
    void setIconName(QString iconName);

protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);

private:
    QPixmap _icon;
    QPixmap _iconHovered;
    static QSize ICON_SIZE;
};

#endif // CONFIGTOCBUTTON_H
