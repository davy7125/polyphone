#include "configtoc.h"
#include "ui_configtoc.h"
#include "contextmanager.h"

ConfigToc::ConfigToc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigToc)
{
    ui->setupUi(this);

    // Style
    ui->frame->setStyleSheet("QFrame{background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() + "}");

    // Buttons
    ui->buttonGeneral->setIconName(":/icons/wrench.svg");
    ui->buttonInterface->setIconName(":/icons/flag.svg");
    ui->buttonSound->setIconName(":/icons/sound.svg");
    ui->buttonKeyboard->setIconName(":/icons/keyboard.svg");
    ui->buttonRepository->setIconName(":/icons/globe.svg");
}

ConfigToc::~ConfigToc()
{
    delete ui;
}

void ConfigToc::on_buttonGeneral_clicked()
{
    emit(sectionClicked(0));
}

void ConfigToc::on_buttonInterface_clicked()
{
    emit(sectionClicked(1));
}

void ConfigToc::on_buttonSound_clicked()
{
    emit(sectionClicked(2));
}

void ConfigToc::on_buttonKeyboard_clicked()
{
    emit(sectionClicked(3));
}

void ConfigToc::on_buttonRepository_clicked()
{
    emit(sectionClicked(4));
}
