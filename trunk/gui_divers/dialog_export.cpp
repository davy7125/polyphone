#include "dialog_export.h"
#include "ui_dialog_export.h"
#include "pile_sf2.h"
#include "config.h"

DialogExport::DialogExport(Pile_sf2 *sf2, EltID idSf2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport),
    _initialID(idSf2)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // Remplissage de la liste
    idSf2.typeElement = elementPrst;
    char T[40];
    for (int i = 0; i < sf2->count(idSf2); i++)
    {
        idSf2.indexElt = i;
        if (!sf2->get(idSf2, champ_hidden).bValue)
        {
            sprintf(T, "%.3d:%.3d %s", sf2->get(idSf2, champ_wBank).wValue,
                    sf2->get(idSf2, champ_wPreset).wValue,
                    sf2->getQstr(idSf2, champ_name).toStdString().c_str());
            QListWidgetItem * item = new QListWidgetItem(T);
            item->setCheckState(Qt::Checked);
            item->setData(Qt::UserRole, i);
            ui->listPresets->addItem(item);
        }
    }

    // Tri
    ui->listPresets->sortItems();

    ui->comboFormat->setCurrentIndex(Config::getInstance()->getExportType());
    ui->lineFolder->setText(Config::getInstance()->getLastFile(Config::typeFichierExport));
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::on_pushTick_clicked()
{
    // Tout sélectionner
    for (int i = 0; i < ui->listPresets->count(); i++)
        ui->listPresets->item(i)->setCheckState(Qt::Checked);
}

void DialogExport::on_pushUntick_clicked()
{
    // Tout décocher
    for (int i = 0; i < ui->listPresets->count(); i++)
        ui->listPresets->item(i)->setCheckState(Qt::Unchecked);
}

void DialogExport::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     ui->lineFolder->text());
    ui->lineFolder->setText(qDir);

}

void DialogExport::on_pushAnnuler_clicked()
{
    QDialog::close();
}

void DialogExport::on_pushExport_clicked()
{
    // Sauvegarde des paramètres
    Config::getInstance()->setExportType(ui->comboFormat->currentIndex());
    if (QDir(ui->lineFolder->text()).exists())
        Config::getInstance()->addFile(Config::typeFichierExport, ui->lineFolder->text());
    else
        QDialog::close();

    // Création liste ID
    QList<EltID> listID;
    EltID id = _initialID;
    id.typeElement = elementPrst;
    for (int i = 0; i < ui->listPresets->count(); i++)
    {
        QListWidgetItem *checkBoxItem = ui->listPresets->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
        {
            id.indexElt = checkBoxItem->data(Qt::UserRole).toInt();
            listID.append(id);
        }
    }
    emit(accepted(listID, ui->lineFolder->text(), ui->comboFormat->currentIndex()));
    QDialog::accept();
}
