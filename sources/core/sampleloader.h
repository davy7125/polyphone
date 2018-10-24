#ifndef SAMPLELOADER_H
#define SAMPLELOADER_H

#include "basetypes.h"
#include <QList>

class QWidget;
class SoundfontManager;

class SampleLoader
{
public:
    SampleLoader(QWidget * parent = NULL);

    /// Add a sample to a soundfont
    IdList load(QString path, int numSf2, int *replace);

private:
    QString getName(QString name, int maxCharacters, int suffixNumber);

    QWidget * _parent;
    SoundfontManager * _sm;
};

#endif // SAMPLELOADER_H
