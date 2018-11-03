#include "toolsoundfontexport_gui.h"
#include "ui_toolsoundfontexport_gui.h"
#include "toolsoundfontexport_parameters.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include <QFileDialog>
#include <QMessageBox>

ToolSoundfontExport_gui::ToolSoundfontExport_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolSoundfontExport_gui)
{
    ui->setupUi(this);
    ui->pushFolder->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(16, 16), ThemeManager::WINDOW_TEXT));
}

ToolSoundfontExport_gui::~ToolSoundfontExport_gui()
{
    delete ui;
}

void ToolSoundfontExport_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)

    // Populate the tree
    ui->listPresets->clear();
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int i, sm->getSiblings(EltID(elementSf2)))
    {
        EltID id(elementSf2, i);
        QTreeWidgetItem * rootItem = new QTreeWidgetItem(QStringList(sm->getQstr(id, champ_name)));
        ui->listPresets->addTopLevelItem(rootItem);

        id.typeElement = elementPrst;
        foreach (int i, sm->getSiblings(id))
        {
            id.indexElt = i;
            QTreeWidgetItem * item = new QTreeWidgetItem(
                        QStringList() << QString("%1:%2   %3").arg(QString::number(sm->get(id, champ_wBank).wValue), 3, QChar('0'))
                        .arg(QString::number(sm->get(id, champ_wPreset).wValue), 3, QChar('0'))
                        .arg(sm->getQstr(id, champ_name)));
            item->setCheckState(0, Qt::Checked);
            item->setData(0, Qt::UserRole, QPoint(id.indexSf2, id.indexElt));
            rootItem->addChild(item);
            rootItem->setExpanded(true);
        }

        // Tri
        rootItem->sortChildren(0, Qt::AscendingOrder);
    }
    ui->listPresets->resizeColumnToContents(0);

    // Load parameters
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;
    int exportType = params->getFormat();
    if (exportType < 0 || exportType >= ui->comboFormat->count())
        exportType = 0;
    ui->comboFormat->setCurrentIndex(exportType);
    on_comboFormat_currentIndexChanged(exportType);

    ui->lineFolder->setText(params->getDirectory());
    ui->checkBank->setChecked(params->getBankDirectory());
    ui->checkPreset->setChecked(params->getPresetPrefix());
    ui->checkGM->setChecked(params->getGmSort());

    int exportQuality = params->getQuality();
    if (exportQuality < 0 || exportQuality >= ui->comboQuality->count())
        exportQuality = 1;
    ui->comboQuality->setCurrentIndex(2 - exportQuality);
}

void ToolSoundfontExport_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolSoundfontExport_parameters * params = (ToolSoundfontExport_parameters *) parameters;

    // Sauvegarde des paramètres
    params->setDirectory(ui->lineFolder->text());
    params->setFormat(ui->comboFormat->currentIndex());
    params->setQuality(2 - ui->comboQuality->currentIndex());
    params->setPresetPrefix(ui->checkPreset->isChecked());
    params->setBankDirectory(ui->checkBank->isChecked());
    params->setGmSort(ui->checkGM->isChecked());

    // Preset list
    params->setSelectedPresets(_presetList);
}

void ToolSoundfontExport_gui::on_pushTick_clicked()
{
    // Tout sélectionner
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Checked);
    }
}

void ToolSoundfontExport_gui::on_pushUntick_clicked()
{
    // Tout décocher
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Unchecked);
    }
}

void ToolSoundfontExport_gui::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     ui->lineFolder->text());
    if (!qDir.isEmpty())
        ui->lineFolder->setText(qDir);
}

void ToolSoundfontExport_gui::on_pushAnnuler_clicked()
{
    emit(this->canceled());
}

void ToolSoundfontExport_gui::on_pushExport_clicked()
{
    // Check the directory
    if (ui->lineFolder->text().isEmpty() || !QDir(ui->lineFolder->text()).exists())
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le répertoire n'est pas valide."));
        return;
    }

    // Update the preset list by soundfont
    _presetList.clear();
    for (int i = 0; i < ui->listPresets->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        for (int j = 0; j < rootItem->childCount(); j++)
        {
            QTreeWidgetItem * item = rootItem->child(j);
            if (item->checkState(0) == Qt::Checked)
            {
                QPoint dataPoint = item->data(0, Qt::UserRole).toPoint();
                _presetList[dataPoint.x()] << dataPoint.y();
            }
        }
    }

    // Remove empty lists
    QList<int> soundfonts = _presetList.keys();
    foreach (int soundfont, soundfonts)
        if (_presetList[soundfont].empty())
            _presetList.remove(soundfont);

    // Check on the number of selected elements
    if (_presetList.empty())
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Au moins un preset doit être sélectionné."));
        return;
    }

    if (_presetList.count() > 127)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le nombre maximal de soundfonts à exporter est de 127."));
        return;
    }

    int maxNumberOfPresets = 0;
    foreach (int i, _presetList.keys())
        maxNumberOfPresets += qMax(maxNumberOfPresets, _presetList[i].count());
    if (maxNumberOfPresets > 127)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Dans le cas où plusieurs soundfonts sont exportées, "
                                                               "le nombre maximal de presets par soundfont est de 127."));
        return;
    }

    emit(this->validated());
}

void ToolSoundfontExport_gui::on_comboFormat_currentIndexChanged(int index)
{
    // Options for sf3
    ui->labelQuality->setVisible(index == 1);
    ui->comboQuality->setVisible(index == 1);

    // Options for sfz
    ui->checkBank->setVisible(index == 2);
    ui->checkGM->setVisible(index == 2);
    ui->checkPreset->setVisible(index == 2);
}
