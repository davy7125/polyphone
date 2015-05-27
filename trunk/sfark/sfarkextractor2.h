#ifndef SFARKEXTRACTOR2_H
#define SFARKEXTRACTOR2_H

#include "abstractextractor.h"

class SfArkExtractor2: public AbstractExtractor
{
public:
    SfArkExtractor2(const char * fileName);
    virtual ~SfArkExtractor2();
    virtual void extract();
    virtual bool getData(char *&data, qint32 &size);

    static SfArkFileManager _fileManager;
    bool _error;

private:
    QString _filename;
};

#endif // SFARKEXTRACTOR2_H
