#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>

class BackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BackgroundWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPixmap _decoration;
    static const int SIZE;
    static const int OFFSET_X;
    static const int OFFSET_Y;
    static const double COLOR_RATIO;
};

#endif // BACKGROUNDWIDGET_H
