#ifndef STYLEDLINEEDIT_H
#define STYLEDLINEEDIT_H

#include <QLineEdit>

class StyledLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    StyledLineEdit(QWidget *parent = NULL);

signals:
    void focussed(bool hasFocus);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private slots:
    void onTextEdited(QString text);

private:
    void setStyle(bool hasFocus);
};

#endif // STYLEDLINEEDIT_H
