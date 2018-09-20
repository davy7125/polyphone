#ifndef ROTATINGSPINNER_H
#define ROTATINGSPINNER_H

#include <QWidget>
class QPropertyAnimation;


class RotatingSpinner : public QWidget
{
    Q_OBJECT

public:
    explicit RotatingSpinner(QWidget *parent = 0);
    ~RotatingSpinner();

    // Start or restart the animation
    void startAnimation();

protected:
    void paintEvent(QPaintEvent * event);

private:
    QPropertyAnimation * _animation;
    QPixmap _pixmap;
    QSize _size;
};

#endif // ROTATINGSPINNER_H
