#include "soundfontdescriptiondata.h"
#include <QJsonObject>

SoundfontDescriptionData::SoundfontDescriptionData(const QJsonObject &data)
{
    if (data.contains("introtext") && data.value("introtext").isString())
        _text = data.value("introtext").toString();
}
