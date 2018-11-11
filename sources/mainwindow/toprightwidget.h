#ifndef TOPRIGHTWIDGET_H
#define TOPRIGHTWIDGET_H

#include <QWidget>
#include <QMap>
#include "usermanager.h"
class MainMenu;

namespace Ui {
class TopRightWidget;
}

class TopRightWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TopRightWidget(QWidget *parent = 0);
    ~TopRightWidget();

public slots:
    void onEditorOpen(bool isOpen);

signals:
    void openClicked();
    void newClicked();
    void openSettingsClicked();
    void onlineHelpClicked();
    void aboutClicked();
    void closeFileClicked();
    void closeClicked();
    void fullScreenTriggered();
    void save();
    void saveAs();
    void userClicked();

private slots:
    void userStateChanged(UserManager::ConnectionState state);
    void on_pushUser_clicked();

private:
    Ui::TopRightWidget *ui;
    MainMenu * _menu;
    QMap<QString, QString> _colorReplacement;
};

#endif // TOPRIGHTWIDGET_H
