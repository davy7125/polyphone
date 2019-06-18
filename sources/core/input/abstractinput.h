/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef ABSTRACTINPUT_H
#define ABSTRACTINPUT_H

#include <QObject>
#include <QString>

template<class T>
class QFutureWatcher;
class SoundfontManager;

class AbstractInput: public QObject
{
    Q_OBJECT

public:
    AbstractInput();
    virtual ~AbstractInput();

    /// Initialize the parser (done by the InputFactory)
    void initialize(QString fileName, SoundfontManager * sm);

    /// Process a file.
    /// When async is false, the function returns when the job is done
    /// Otherwise the function returns immediately and the signal "finished" is sent later
    void process(bool async);

    /// Return true after having processed the file if it was successful
    bool isSuccess() { return _isSuccess; }

    /// Return a reason when the process couldn't be done (isSuccess being false)
    QString getError() { return _error; }

    /// Index of the soundfont that has been created
    int getSf2Index() { return _sf2Index; }

    /// Name of the file to open
    QString getFileName() { return _fileName; }

signals:
    /// Signal emitted when the file is processed
    void finished();

protected slots:
    virtual void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) = 0;

private:
    void processAsync();

    QFutureWatcher<void> * _futureWatcher;
    SoundfontManager * _sm;
    QString _fileName;
    bool _isSuccess;
    QString _error;
    int _sf2Index;
};

#endif // ABSTRACTINPUT_H
