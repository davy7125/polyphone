#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>
#include <QResizeEvent>

class ElidedLabel : public QLabel
{
public:
    /// Constructor
    ElidedLabel(QWidget * parent = NULL) : QLabel(parent) {}

    /// Set the text to elide. The displayed text will be set automatically
    void setTextToElide(const QString text) { _text = text; }

protected:
    void resizeEvent(QResizeEvent * event)
    {
        QFontMetrics metrics(this->font());
        QString elidedText = metrics.elidedText(_text, Qt::ElideMiddle, event->size().width());
        this->setText(elidedText);
    }

private:
    QString _text;
};

#endif // ELIDEDLABEL_H
