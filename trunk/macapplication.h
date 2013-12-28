#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include <QApplication>
#include <QFileOpenEvent>
#include "sf2_types.h"

class MacApplication: public QApplication
{
    Q_OBJECT
public:
    MacApplication(int &argc, char **argv, QStringList * listPathMac, int flags = ApplicationFlags) :
        QApplication(argc, argv, flags),
        _listPathMac(listPathMac)
    {}
    void stopAppending()
    {
        _listPathMac = NULL;
    }

signals:
    void openFile(QString path);

protected:
    bool event(QEvent * event)
    {
        if (event->type() == QEvent::FileOpen)
        {
            QString path = static_cast<QFileOpenEvent*>(event)->file();
            if (_listPathMac)
                _listPathMac->append(path);
            emit(openFile(path));
            return true;
        }
        return QApplication::event(event);
    }

private:
    QStringList * _listPathMac;
};


#endif // MACAPPLICATION_H
