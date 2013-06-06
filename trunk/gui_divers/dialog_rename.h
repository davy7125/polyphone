#ifndef DIALOG_RENAME_H
#define DIALOG_RENAME_H

#include <QDialog>

namespace Ui {
class DialogRename;
}

class DialogRename : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogRename(QString defaultValue, QWidget *parent = 0);
    ~DialogRename();
    
signals:
    void updateNames(QString name, int modificationType);

private slots:
    virtual void accept();

private:
    Ui::DialogRename *ui;
};

#endif // DIALOG_RENAME_H
