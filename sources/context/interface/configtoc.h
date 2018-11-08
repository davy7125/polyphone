#ifndef CONFIGTOC_H
#define CONFIGTOC_H

#include <QWidget>

namespace Ui {
class ConfigToc;
}

class ConfigToc : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigToc(QWidget *parent = 0);
    ~ConfigToc();

signals:
    void sectionClicked(int sectionNumber);

private slots:
    void on_buttonGeneral_clicked();
    void on_buttonInterface_clicked();
    void on_buttonSound_clicked();
    void on_buttonKeyboard_clicked();
    void on_buttonRepository_clicked();

private:
    Ui::ConfigToc *ui;
};

#endif // CONFIGTOC_H
