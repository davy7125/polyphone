#include "elidedpushbutton.h"

void ElidedPushButton::computeText(int width)
{
    QFontMetrics metrics(this->font());
    QString elidedText = metrics.elidedText(_text, Qt::ElideMiddle, width);
    this->setText(elidedText);
}

void ElidedPushButton::setTextToElide(const QString text, int availableWidth)
{
    _text = text;
    _availableWidth = availableWidth;
}

void ElidedPushButton::setAvailableWidth(int availableWidth)
{
    _availableWidth = availableWidth;
    computeText(availableWidth);
}

void ElidedPushButton::resizeEvent(QResizeEvent * event)
{
    computeText(_availableWidth == -1 ? event->size().width() : _availableWidth);
    QPushButton::resizeEvent(event);
}
