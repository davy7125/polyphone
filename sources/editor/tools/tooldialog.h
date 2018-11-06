#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

#include <QDialog>

class AbstractTool;
class AbstractToolGui;
class AbstractToolParameters;


class ToolDialog : public QDialog
{
    Q_OBJECT

public:
    ToolDialog(AbstractToolGui * toolGui, AbstractTool *tool, QWidget * parent);

signals:
    void validated();

private slots:
    void canceled();
};

#endif // TOOLDIALOG_H
