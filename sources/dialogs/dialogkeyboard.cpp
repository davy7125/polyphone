#include "dialogkeyboard.h"
#include "ui_dialogkeyboard.h"
#include "contextmanager.h"
#include "editortoolbar.h"

DialogKeyboard::DialogKeyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogKeyboard)
{
    ui->setupUi(this);
}

DialogKeyboard::~DialogKeyboard()
{
    delete ui;
}

void DialogKeyboard::hideEvent(QHideEvent * event)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", this->saveGeometry());
    QDialog::hideEvent(event);
}

void DialogKeyboard::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    QByteArray geometry = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "keyboardGeometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

void DialogKeyboard::closeEvent(QCloseEvent * event)
{
    QDialog::closeEvent(event);
    EditorToolBar::updateKeyboardButtonsState(false);
}
