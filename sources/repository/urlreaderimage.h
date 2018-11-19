#ifndef URLREADERIMAGE_H
#define URLREADERIMAGE_H

#include "urlreader.h"
#include <QPixmap>

class UrlReaderImage : public UrlReader
{
public:
    // Constructor, initialized with a url
    UrlReaderImage(QString url);

    // Get method
    QPixmap getPixmap() { return _pixmap; }
    bool isValid() { return _isValid; }

protected:
    virtual void processData();

private:
    QPixmap _pixmap;
    bool _isValid;
};

#endif // URLREADERIMAGE_H
