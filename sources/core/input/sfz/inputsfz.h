#ifndef INPUTSFZ_H
#define INPUTSFZ_H

#include "abstractinput.h"
#include "sfzparametergroupassembly.h"
class SoundfontManager;

class InputSfz : public AbstractInput
{
public:
    InputSfz();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    enum Bloc
    {
        BLOC_UNKNOWN,
        BLOC_CONTROL,
        BLOC_GROUP,
        BLOC_REGION
    };

    QList<SfzParameterGroupAssembly> _listeEnsembles;
    Bloc _currentBloc;

    void changeBloc(QString bloc);
    void addOpcode(QString opcode, QString value);
    QString getNomInstrument(QString filePath, int &numBank, int &numPreset);
};

#endif // INPUTSFZ_H
