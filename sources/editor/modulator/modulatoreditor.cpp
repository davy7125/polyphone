#include "modulatoreditor.h"
#include "ui_modulatoreditor.h"
#include "contextmanager.h"

QList<ModulatorEditor *> ModulatorEditor::s_instances;

ModulatorEditor::ModulatorEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorEditor)
{
    s_instances << this;
    ui->setupUi(this);

    // Icons
    ui->pushExpand->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_up.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushAdd->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushCopy->setIcon(ContextManager::theme()->getColoredSvg(":/icons/copy.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushPaste->setIcon(ContextManager::theme()->getColoredSvg(":/icons/paste.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushClone->setIcon(ContextManager::theme()->getColoredSvg(":/icons/clone.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushDelete->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushCollapse->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    // Style
    ui->listWidget->setStyleSheet("QListWidget{border:1px solid " +
                                  this->palette().dark().color().name() +
                                  ";border-top:0;border-bottom:0;border-right:0}");
    ui->labelNoModulators->setStyleSheet("QLabel{border:1px solid " +
                                         this->palette().dark().color().name() +
                                         ";border-top:0;border-bottom:0;border-right:0;" +
                                         "background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                         ";color:" + this->palette().dark().color().name() +
                                         "}");

    // Initialize the expanded / collapsed state
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", false).toBool())
        on_pushCollapse_clicked();
    else
        on_pushExpand_clicked();
}

ModulatorEditor::~ModulatorEditor()
{
    s_instances.removeOne(this);
    delete ui;
}

void ModulatorEditor::on_pushExpand_clicked()
{
    foreach (ModulatorEditor * elt, s_instances)
    {
        elt->ui->frameCollapsed->hide();
        elt->ui->frameExpanded->show();
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", false);
}

void ModulatorEditor::on_pushCollapse_clicked()
{
    foreach (ModulatorEditor * elt, s_instances)
    {
        elt->ui->frameExpanded->hide();
        elt->ui->frameCollapsed->show();
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "modulator_section_collapsed", true);
}
