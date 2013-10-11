#ifndef EDITKEY_H
#define EDITKEY_H

#include <QLineEdit>
#include <QKeySequence>
#include <QKeyEvent>

class EditKey : public QLineEdit
{
    Q_OBJECT

public:
    EditKey(QWidget * parent = NULL);
    void setCombinaison(const QString &text);

signals:
    void combinaisonChanged(QString combinaison);

protected:
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);

private:
    QString _combinaison;
    static QKeySequence getSequence(QKeyEvent * event);
};

#endif // EDITKEY_H
