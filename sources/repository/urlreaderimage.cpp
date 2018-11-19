#include "urlreaderimage.h"

UrlReaderImage::UrlReaderImage(QString url) : UrlReader(url) {}

void UrlReaderImage::processData()
{
    // Extract an image from the data
   _isValid = _pixmap.loadFromData(this->getRawData());
}

