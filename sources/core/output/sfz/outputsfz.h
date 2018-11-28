#ifndef OUTPUTSFZ_H
#define OUTPUTSFZ_H

#include "abstractoutput.h"
class SoundfontManager;

class OutputSfz : public AbstractOutput
{
    Q_OBJECT
    
public:
    OutputSfz();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;
};

#endif // OUTPUTSFZ_H
