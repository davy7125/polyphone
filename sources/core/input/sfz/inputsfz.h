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
        BLOC_GLOBAL,
        BLOC_GROUP,
        BLOC_REGION
    };

    QList<SfzParameterGroupAssembly> _listeEnsembles;
    Bloc _currentBloc;
    SfzParameterGroup _globalZone;

    void parseFile(QString filename, bool &success, QString &error);
    void changeBloc(QString bloc);
    void addOpcode(QString opcode, QString value);

    void createSf2(int &sf2Index, QString filename, bool isChannel10);
    QString getInstrumentName(QString filePath, int &numBank, int &numPreset);
};

#endif // INPUTSFZ_H
