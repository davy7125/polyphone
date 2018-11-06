#ifndef DIALOGKEYBOARD_H
#define DIALOGKEYBOARD_H

#include <QDialog>

namespace Ui {
class DialogKeyboard;
}

class DialogKeyboard : public QDialog
{
    Q_OBJECT

public:
    explicit DialogKeyboard(QWidget *parent = 0);
    ~DialogKeyboard();

    /// Focus on the keyboard and animate with a glow effect
    void glow();

protected:
    void hideEvent(QHideEvent * event) override;
    void showEvent(QShowEvent * event) override;
    void closeEvent(QCloseEvent * event) override;

private slots:
    void keyPlayed(int key, int vel);
    void on_comboType_currentIndexChanged(int index);

private:
    Ui::DialogKeyboard *ui;
};

#endif // DIALOGKEYBOARD_H
