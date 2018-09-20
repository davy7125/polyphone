#include "rotatingspinner.h"
#include "contextmanager.h"
#include <QPropertyAnimation>
#include <QPainter>

RotatingSpinner::RotatingSpinner(QWidget *parent) : QWidget(parent),
    _animation(new QPropertyAnimation(this))
{
    // Configure the animation
    _animation->setTargetObject(this);
    _animation->setStartValue(0);
    _animation->setEndValue(360);
    _animation->setDuration(1500);

    connect(_animation, SIGNAL(valueChanged(const QVariant&)), SLOT(update()));
    connect(_animation, SIGNAL(finished()), _animation, SLOT(start()));
}

RotatingSpinner::~RotatingSpinner()
{
    _animation->stop();
    delete _animation;
}

void RotatingSpinner::startAnimation()
{
    _animation->setCurrentTime(0);
    _animation->start();
}

void RotatingSpinner::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    // Get the image and scale it if not already done or if the size changes
    if (_pixmap.isNull() || _size != this->size())
    {
        _pixmap = ContextManager::theme()->getColoredSvg(":/misc/spinner.svg", this->size(), ThemeManager::WINDOW_TEXT);
        _size = this->size();
    }

    // Rotation
    QTransform transform;
    transform.rotate(_animation->currentValue().toDouble());
    QPixmap pix = _pixmap.transformed(transform, Qt::SmoothTransformation);

    // Draw the spinner
    QPainter painter(this);
    double x = ((double)(this->width() - pix.width())) / 2.0;
    double y = ((double)(this->height() - pix.height())) / 2.0;
    painter.drawPixmap(QPointF(x, y), pix);
}
