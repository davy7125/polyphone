#ifndef SOUNDFONTDESCRIPTIONDATA_H
#define SOUNDFONTDESCRIPTIONDATA_H

#include <QString>
class QJsonObject;

class SoundfontDescriptionData
{
public:
    SoundfontDescriptionData(const QJsonObject &data);

    // Getters
    QString getText() { return _text; }

private:
    QString _text;
};

#endif // SOUNDFONTDESCRIPTIONDATA_H
