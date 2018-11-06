#include "auto_loop/toolautoloop.h"
#include "soundfontmanager.h"

void ToolAutoLoop::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _samplesNotLooped.clear();
}

void ToolAutoLoop::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    // Get data, sample rate, start and end loop
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 dwSmplRate = sm->get(id, champ_dwSampleRate).dwValue;
    qint32 startLoop = sm->get(id, champ_dwStartLoop).dwValue;
    qint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;

    // Loop
    baData = Sound::bouclage(baData, dwSmplRate, startLoop, endLoop, 24);
    if (!baData.isEmpty())
    {
        // Update data, length, startloop and endloop
        sm->set(id, champ_sampleDataFull24, baData);
        AttributeValue val;
        val.dwValue = startLoop;
        sm->set(id, champ_dwStartLoop, val);
        val.dwValue = endLoop;
        sm->set(id, champ_dwEndLoop, val);
        val.dwValue = baData.size() / 3;
        sm->set(id, champ_dwLength, val);
    }
    else
    {
        _mutex.lock();
        _samplesNotLooped << sm->getQstr(id, champ_name);
        _mutex.unlock();
    }
}

QString ToolAutoLoop::getWarning()
{
    QString txt;

    if (!_samplesNotLooped.isEmpty())
    {

        if (_samplesNotLooped.size() == 1)
            txt = trUtf8("L'échantillon « ") + _samplesNotLooped.first() + trUtf8(" » n'a pas pu être bouclé.") + "<br/>";
        else
        {
            txt = trUtf8("Les échantillons suivants n'ont pas pu être bouclés :") + "<ul>";
            for (int i = 0; i < _samplesNotLooped.size(); i++)
                txt += "<li>" + _samplesNotLooped.at(i) + "</li>";
            txt += "</ul>";
        }
        txt += trUtf8("Causes possibles : trop court ou trop turbulent.");
    }

    return txt;
}
