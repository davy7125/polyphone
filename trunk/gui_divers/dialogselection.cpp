#include "dialogselection.h"
#include "ui_dialogselection.h"
#include "pile_sf2.h"

DialogSelection::DialogSelection(Pile_sf2 *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelection)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);

    // Remplissage de la liste
    char T[40];
    int initIndex = id.indexElt;
    for (int i = 0; i < sf2->count(id); i++)
    {
        if (i != initIndex)
        {
            id.indexElt = i;
            if (!sf2->get(id, champ_hidden).bValue)
            {
                if (id.typeElement == elementInst)
                    sprintf(T, "%s", sf2->getQstr(id, champ_name).toStdString().c_str());
                else
                    sprintf(T, "%.3d:%.3d %s", sf2->get(id, champ_wBank).wValue,
                            sf2->get(id, champ_wPreset).wValue,
                            sf2->getQstr(id, champ_name).toStdString().c_str());
                QListWidgetItem * item = new QListWidgetItem(T);
                item->setCheckState(Qt::Checked);
                item->setData(Qt::UserRole, i);
                ui->listWidget->addItem(item);
            }
        }
    }

    // Tri
    ui->listWidget->sortItems();
}

DialogSelection::~DialogSelection()
{
    delete ui;
}

void DialogSelection::on_pushSelectAll_clicked()
{
    // Tout sélectionner
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
}

void DialogSelection::on_pushSelectNone_clicked()
{
    // Tout décocher
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
}

void DialogSelection::on_pushCancel_clicked()
{
    QDialog::close();
}

void DialogSelection::on_pushOk_clicked()
{
    // Création liste index
    QList<int> listIndex;
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem *checkBoxItem = ui->listWidget->item(i);
        if (checkBoxItem->checkState() == Qt::Checked)
            listIndex << checkBoxItem->data(Qt::UserRole).toInt();
    }

    emit(listChosen(listIndex));
    QDialog::accept();
}
