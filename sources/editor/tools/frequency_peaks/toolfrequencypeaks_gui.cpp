/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "toolfrequencypeaks_gui.h"
#include "ui_toolfrequencypeaks_gui.h"
#include "contextmanager.h"
#include "graphiquefourier.h"
#include "soundfontmanager.h"
#include "utils.h"

class RunnableTask: public QRunnable
{
public:
    RunnableTask(ToolFrequencyPeaks_gui * toolGui, EltID id) : QRunnable(),
        _toolGui(toolGui),
        _id(id)
    {}

    void run() override
    {
        // Compute data
        QList<double> frequencies;
        QList<double> factors;
        QList<int> keys;
        QList<int> corrections;
        SoundfontManager * sm = SoundfontManager::getInstance();
        GraphiqueFourier graphTmp(nullptr);
        graphTmp.setData(sm->getData(_id, champ_sampleData16), sm->get(_id, champ_dwSampleRate).dwValue);
        graphTmp.setPos(sm->get(_id, champ_dwStartLoop).dwValue, sm->get(_id, champ_dwEndLoop).dwValue,
                        frequencies, factors, keys, corrections, false);

        // Store data
        SampleFrequencyInfo sampleInfo;
        sampleInfo.name = sm->getQstr(_id, champ_name);
        for (int i = 0; i < frequencies.count(); i++)
        {
            FrequencyInfo info;
            info.frequency = frequencies[i];
            info.factor = factors[i];
            info.key = keys[i];
            info.correction = corrections[i];
            sampleInfo.frequencies << info;
        }

        // Send the data to the tool gui
        _toolGui->peakComputed(_id, sampleInfo);
    }

private:
    ToolFrequencyPeaks_gui * _toolGui;
    EltID _id;
};


ToolFrequencyPeaks_gui::ToolFrequencyPeaks_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolFrequencyPeaks_gui)
{
    ui->setupUi(this);

    // Table header
    ui->table->setColumnCount(6);
    ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem(trUtf8("Sample")));
    ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem(trUtf8("Peak")));
    ui->table->setHorizontalHeaderItem(2, new QTableWidgetItem(trUtf8("Intensity")));
    ui->table->setHorizontalHeaderItem(3, new QTableWidgetItem(trUtf8("Frequency")));
    ui->table->setHorizontalHeaderItem(4, new QTableWidgetItem(trUtf8("Key")));
    ui->table->setHorizontalHeaderItem(5, new QTableWidgetItem(trUtf8("Correction")));

    // Connection (do to different threads)
    qRegisterMetaType<SampleFrequencyInfo>();
    connect(this, SIGNAL(peakComputed(EltID,const SampleFrequencyInfo)), this, SLOT(onPeakComputed(EltID,const SampleFrequencyInfo)), Qt::QueuedConnection);
}

ToolFrequencyPeaks_gui::~ToolFrequencyPeaks_gui()
{
    delete ui;
}

void ToolFrequencyPeaks_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(parameters)

    // Name of the sf2
    _sf2Name = "untitled.csv";
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (!ids.empty())
        _sf2Name = sm->getQstr(EltID(elementSf2, ids[0].indexSf2), champ_name)
                .replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".csv";

    // Initialize the table
    ui->table->clearContents();
    ui->table->setRowCount(0);

    // Compute the peaks
    ui->pushExport->setEnabled(false);
    _data.clear();
    _idsToCompute = ids;
    foreach (EltID id, ids)
        QThreadPool::globalInstance()->start(new RunnableTask(this, id));
}

void ToolFrequencyPeaks_gui::onPeakComputed(EltID id, const SampleFrequencyInfo sfi)
{
    if (_idsToCompute.contains(id))
    {
        _idsToCompute.removeAll(id);

        if (!sfi.frequencies.empty())
        {
            // Store the data that just came
            int pos = 0;
            while (pos < _data.count() && Utils::naturalOrder(Utils::removeAccents(_data[pos].name), Utils::removeAccents(sfi.name)) < 0)
                pos++;
            _data.insert(pos, sfi);

            // Update the table
            int currentRow = ui->table->rowCount();
            ui->table->setRowCount(currentRow + sfi.frequencies.count());
            for (int i = 0; i < sfi.frequencies.count(); i++)
            {
                FrequencyInfo fi = sfi.frequencies[i];
                if (i == 0)
                {
                    ui->table->setItem(currentRow, 0, new QTableWidgetItem(sfi.name));
                    ui->table->item(currentRow, 0)->setBackground(this->palette().color(QPalette::Base));
                }
                ui->table->setItem(currentRow + i, 1, new QTableWidgetItem(QString::number(i + 1)));
                ui->table->setItem(currentRow + i, 2, new QTableWidgetItem(QString::number(fi.factor)));
                ui->table->setItem(currentRow + i, 3, new QTableWidgetItem(QString::number(fi.frequency) + " " + trUtf8("Hz")));
                ui->table->setItem(currentRow + i, 4, new QTableWidgetItem(ContextManager::keyName()->getKeyName(fi.key)));
                ui->table->setItem(currentRow + i, 5, new QTableWidgetItem(QString::number(fi.correction)));
            }
            ui->table->setSpan(currentRow, 0, sfi.frequencies.count(), 1);
        }
    }

    if (_idsToCompute.empty())
        ui->pushExport->setEnabled(true);
}

void ToolFrequencyPeaks_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolFrequencyPeaks_parameters * params = (ToolFrequencyPeaks_parameters *) parameters;

    // Save the path and data
    params->setFilePath(_path);
    params->setSampleFrequencies(_data);
}

void ToolFrequencyPeaks_gui::on_pushExport_clicked()
{
    QString defaultFile = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_FREQUENCIES) + "/" + _sf2Name;
    QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Export peak frequency"),
                                                    defaultFile, trUtf8("Csv file") + " (*.csv)");
    if (!fileName.isEmpty())
    {
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_FREQUENCIES, fileName);
        _path = fileName;
        _idsToCompute.clear();
        emit(this->validated());
    }
}

void ToolFrequencyPeaks_gui::on_pushClose_clicked()
{
    _idsToCompute.clear();
    emit(this->canceled());
}
