#include "editkey.h"

EditKey::EditKey(QWidget *parent) : QLineEdit(parent),
    _combinaison("")
{
}

void EditKey::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        QString combinaison = getSequence(event);
        if (combinaison.isEmpty())
            this->setText("...");
        else
        {
            QString key = QKeySequence(event->key()).toString();
            if (key.isEmpty() || event->key() == Qt::Key_Shift)
            {
                this->setText(combinaison);
            }
            else
            {
                int keyNum = event->key();
                if (keyNum == Qt::Key_Delete)
                    _combinaison = "";
                else if (keyNum != Qt::Key_Escape && keyNum != Qt::Key_Return && keyNum != Qt::Key_Enter)
                    _combinaison = combinaison;

                this->clearFocus();
            }
        }
        event->accept();
    }
}

void EditKey::keyReleaseEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        QString combinaison = getSequence(event);
        if (combinaison.isEmpty())
        {
            if (this->hasFocus())
                this->setText("...");
            else
                this->setText(_combinaison);
        }
        else
            this->setText(combinaison);
        event->accept();
    }
}

void EditKey::focusInEvent(QFocusEvent *)
{
    this->setText("...");
}

void EditKey::focusOutEvent(QFocusEvent *)
{
    this->setText(_combinaison);
    emit(combinaisonChanged(_combinaison));
}

QKeySequence EditKey::getSequence(QKeyEvent * event)
{
    QString modifier = QString::null;
    // Sensibilité à la touche shift uniquement
    if (event->modifiers() & Qt::ShiftModifier)
        modifier += "Shift+";
    QString key = (QString)QKeySequence(event->key());
    return QKeySequence(modifier + key);
}

void EditKey::setCombinaison(const QString &text)
{
    _combinaison = text;
    QLineEdit::setText(text);
}
