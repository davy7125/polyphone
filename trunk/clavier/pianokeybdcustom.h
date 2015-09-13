/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef PIANOKEYBDCUSTOM_H
#define PIANOKEYBDCUSTOM_H

#include "pianokeybd.h"
#include "RtMidi.h"
#include <QKeyEvent>

// Events
class NoteEvent : public QEvent
{
public:
    NoteEvent(unsigned char note, unsigned char val) : QEvent(QEvent::User),
          m_note(note),
          m_velocity(val) {}
    unsigned char getNote() const {return m_note;}
    unsigned char getVelocity() const {return m_velocity;}

protected:
    unsigned char m_note;
    unsigned char m_velocity;
};

class ControllerEvent : public QEvent
{
public:
    ControllerEvent(unsigned char numController, unsigned char value) : QEvent((QEvent::Type)(QEvent::User+1)),
          m_numController(numController),
          m_value(value) {}
    unsigned char getNumController() const {return m_numController;}
    unsigned char getValue() const {return m_value;}

protected:
    unsigned char m_numController;
    unsigned char m_value;
};


class PianoKeybdCustom : public PianoKeybd
{
    Q_OBJECT
public:
    PianoKeybdCustom(QWidget * parent);
    ~PianoKeybdCustom();
    QStringList getPortNames();
    void openMidiPort(int val);
    void changeKey(int key, int vel);
    void changeController(int numController, int value);
    void setRangeAndRootKey(int rootKey, int noteMin, int noteMax);
    void clearCustomization();
    void setCurrentRange(int note, int noteMin, int noteMax);

signals:
    void sustainChanged(bool isOn);
    void volumeChanged(int value);

protected:
    void customEvent(QEvent * event)
    {
        if (event->type() == QEvent::User)
        {
            NoteEvent *noteEvent = dynamic_cast<NoteEvent *>(event);
            this->changeKey(noteEvent->getNote(), noteEvent->getVelocity());
            event->accept();
        }
        else if (event->type() == QEvent::User + 1)
        {
            ControllerEvent *controllerEvent = dynamic_cast<ControllerEvent *>(event);
            this->changeController(controllerEvent->getNumController(), controllerEvent->getValue());
            event->accept();
        }
    }
    void keyPressEvent(QKeyEvent *event);

private slots:
    void setKey(int num, int vel);
    void setKeyOff(int num);

private:
    RtMidiIn * midiin;
    QList<int> _currentRange;
    QList<int> _rootKeys;
    QMap<int, QList<int> > _mapPressed;
    static QColor COLOR_PRESSED;
    static QColor COLOR_BLACK_ENABLED;
    static QColor COLOR_WHITE_ENABLED;
    static QColor COLOR_BLACK_DISABLED;
    static QColor COLOR_WHITE_DISABLED;
    static QColor COLOR_BLACK_RANGE;
    static QColor COLOR_WHITE_RANGE;

    void updateRanges();
};


#endif // PIANOKEYBDCUSTOM_H
