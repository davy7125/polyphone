#ifndef OUTPUTNOTSUPPORTED_H
#define OUTPUTNOTSUPPORTED_H

#include "abstractoutput.h"
class SoundfontManager;

class OutputNotSupported : public AbstractOutput
{
    Q_OBJECT
    
public:
    OutputNotSupported();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options) override;
};

#endif // OUTPUTNOTSUPPORTED_H
