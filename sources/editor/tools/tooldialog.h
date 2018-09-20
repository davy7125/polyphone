#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

#include <QDialog>

class AbstractToolGui;
class AbstractToolParameters;


class ToolDialog : public QDialog
{
    Q_OBJECT

public:
    ToolDialog(AbstractToolGui * toolGui, QWidget * parent);

signals:
    void validated();

private slots:
    void canceled();
};

#endif // TOOLDIALOG_H
