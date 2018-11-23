#include "dialogcreateelements.h"
#include "ui_dialogcreateelements.h"
#include "contextmanager.h"
#include "soundfontmanager.h"

DialogCreateElements::DialogCreateElements(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateElements)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));
}

DialogCreateElements::~DialogCreateElements()
{
    delete ui;
}

void DialogCreateElements::initialize(IdList ids)
{
    if (ids.empty())
        return;
    _ids = ids;

    if (ids[0].typeElement == elementSmpl)
    {
        if (ids.count() == 1)
        {
            ui->stackedWidget->setCurrentIndex(1);
            ui->labelOneChoice->setText(trUtf8("Créer un instrument contenant l'échantillon %0 ?")
                                        .arg("<b>" + SoundfontManager::getInstance()->getQstr(ids[0], champ_name) + "</b>"));
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->labelChoice->setText(trUtf8("Créer un instrument"));
            ui->radioOnePerElement->setText(trUtf8("pour chaque échantillon"));
            ui->radioOnlyOne->setText(trUtf8("contenant les %0 échantillons").arg(ids.count()));
        }
    }
    else
    {
        if (ids.count() == 1)
        {
            ui->stackedWidget->setCurrentIndex(1);
            ui->labelOneChoice->setText(trUtf8("Créer un preset contenant l'instrument %0 ?")
                                        .arg("<b>" + SoundfontManager::getInstance()->getQstr(ids[0], champ_name) + "</b>"));
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->labelChoice->setText(trUtf8("Créer un preset"));
            ui->radioOnePerElement->setText(trUtf8("pour chaque instrument"));
            ui->radioOnlyOne->setText(trUtf8("contenant les %0 instruments").arg(ids.count()));
        }
    }

    // Restore previous setting
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "create_element_dialog", 0).toInt() == 1)
        ui->radioOnePerElement->setChecked(true);
    else
        ui->radioOnlyOne->setChecked(true);
}

void DialogCreateElements::on_buttonBox_accepted()
{
    // Store the option
    ContextManager::configuration()->setValue(ConfManager::SECTION_NONE, "create_element_dialog", ui->radioOnePerElement->isChecked() ? 1 : 0);

    emit(createElements(_ids, ui->radioOnePerElement->isChecked() || _ids.count() == 1));
}
