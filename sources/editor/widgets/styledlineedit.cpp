#include "styledlineedit.h"

StyledLineEdit::StyledLineEdit(QWidget * parent) : QLineEdit(parent)
{
    this->setStyle(false);
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextEdited(QString)));
}

void StyledLineEdit::focusInEvent(QFocusEvent *e)
{
    this->setStyle(true);
    QLineEdit::focusInEvent(e);
    emit(focussed(true));
}

void StyledLineEdit::focusOutEvent(QFocusEvent *e)
{
    this->setStyle(false);
    QLineEdit::focusOutEvent(e);
    emit(focussed(false));
}

void StyledLineEdit::setStyle(bool hasFocus)
{
    if (hasFocus)
    {
        this->setCursor(Qt::IBeamCursor);
        this->setStyleSheet("");
    }
    else
    {
        this->setCursor(Qt::PointingHandCursor);
        this->setStyleSheet("QLineEdit{background-color:transparent; border: 0}");
    }
}

void StyledLineEdit::onTextEdited(QString text)
{
    // Adapt the width
    QFontMetrics fm(this->font());
    int width = fm.width(text.isEmpty() ? this->placeholderText() : text) + 20;

    this->setMaximumWidth(width);
    this->setMinimumWidth(width);
}
