#include "tooltransposesmpl.h"
#include "tooltransposesmpl_gui.h"
#include "tooltransposesmpl_parameters.h"
#include "soundfontmanager.h"
#include "qmath.h"

ToolTransposeSmpl::ToolTransposeSmpl() : AbstractToolIterating(elementSmpl, new ToolTransposeSmpl_parameters(), new ToolTransposeSmpl_gui())
{

}

void ToolTransposeSmpl::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolTransposeSmpl_parameters * params = (ToolTransposeSmpl_parameters *)parameters;

    // Get sample data
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 echFinal = sm->get(id, champ_dwSampleRate).dwValue;

    // Calcul de l'échantillonnage fictif de départ
    double echInit = (double)echFinal * qPow(2, params->getSemiTones() / 12);

    // Resampling
    baData = Sound::resampleMono(baData, echInit, echFinal, 24);
    sm->set(id, champ_sampleDataFull24, baData);

    // Update the length
    Valeur val;
    val.dwValue = baData.size() / 3;
    sm->set(id, champ_dwLength, val);

    // Update loop
    quint32 dwTmp = sm->get(id, champ_dwStartLoop).dwValue;
    dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
    val.dwValue = dwTmp;
    sm->set(id, champ_dwStartLoop, val);
    dwTmp = sm->get(id, champ_dwEndLoop).dwValue;
    dwTmp = ((qint64)dwTmp * (qint64)echFinal) / echInit;
    val.dwValue = dwTmp;
    sm->set(id, champ_dwEndLoop, val);
}
