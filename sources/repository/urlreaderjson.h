#ifndef URLREADERJSON_H
#define URLREADERJSON_H

#include "urlreader.h"
#include <QJsonObject>

class UrlReaderJson : public UrlReader
{
public:
    // Constructor, initialized with a url
    UrlReaderJson(QString url);

    // Get methods
    QString getErrorMessage() { return _errorMessage; }
    QString getErrorCode() { return _errorCode; }
    QString getRespondeId() { return _responseId; }
    QString getApi() { return _api; }
    QString getVersion() { return _version; }
    QJsonObject getData() { return _data; }
    bool isValid() { return _errorMessage.isEmpty(); }

protected:
    virtual void processData();

private:
    void setInvalid(QString message);

    QString _errorMessage;
    QString _errorCode;
    QString _responseId;
    QString _api;
    QString _version;
    QJsonObject _data;
};

#endif // URLREADERJSON_H
