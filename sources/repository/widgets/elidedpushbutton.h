#ifndef ELIDEDPUSHBUTTON_H
#define ELIDEDPUSHBUTTON_H

#include <QPushButton>
#include <QResizeEvent>

class ElidedPushButton : public QPushButton
{
public:
    /// Constructor
    ElidedPushButton(QWidget * parent = NULL) : QPushButton(parent) {}

    /// Set the text to elide. The displayed text will be set automatically
    void setTextToElide(const QString text, int availableWidth = -1);

    /// Change the available space
    void setAvailableWidth(int availableWidth);

protected:
    void resizeEvent(QResizeEvent * event);

private:
    void computeText(int width);

    QString _text;
    int _availableWidth;
};

#endif // ELIDEDPUSHBUTTON_H
