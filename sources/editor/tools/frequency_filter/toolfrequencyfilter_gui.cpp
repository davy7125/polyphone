#include "toolfrequencyfilter_gui.h"
#include "ui_toolfrequencyfilter_gui.h"
#include "toolfrequencyfilter_parameters.h"
#include "soundfontmanager.h"

ToolFrequencyFilter_gui::ToolFrequencyFilter_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolFrequencyFilter_gui)
{
    ui->setupUi(this);
}

ToolFrequencyFilter_gui::~ToolFrequencyFilter_gui()
{
    delete ui;
}

void ToolFrequencyFilter_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    ToolFrequencyFilter_parameters * params = (ToolFrequencyFilter_parameters *) parameters;

    // Filter used
    ui->graphFilterFrequencies->setValues(params->getCurve());

    // Number of Fourier transforms to add
    int nbFourier = qMin(ids.count(), 6);
    ui->graphFilterFrequencies->setNbFourier(nbFourier);

    // Add the fourier transforms of the first samples in the graph
    SoundfontManager * sm = SoundfontManager::getInstance();
    for (int i = 0; i < nbFourier; i++)
    {
        EltID id = ids[i];
        quint32 sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
        QByteArray baData = sm->getData(id, champ_sampleData16);

        // Get the Fourier transform of the sample
        int length = baData.size() / 2;
        QVector<float> fData;
        fData.resize(length);
        qint16 * data = (qint16*)baData.data();
        for (int i = 0; i < length; i++)
            fData[i] = (float)data[i];
        fData = Sound::getFourierTransform(fData);

        // Display it
        ui->graphFilterFrequencies->addFourierTransform(fData, sampleRate);
    }
}

void ToolFrequencyFilter_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolFrequencyFilter_parameters * params = (ToolFrequencyFilter_parameters *) parameters;

    // Save parameters
    params->setCurve(ui->graphFilterFrequencies->getValues());
}

void ToolFrequencyFilter_gui::on_buttonBox_accepted()
{
    emit(this->validated());
}

void ToolFrequencyFilter_gui::on_buttonBox_rejected()
{
    emit(this->canceled());
}
