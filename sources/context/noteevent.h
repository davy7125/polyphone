#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include <QEvent>

class NoteEvent : public QEvent
{
public:
    NoteEvent(unsigned char note, unsigned char val) : QEvent(QEvent::User),
          m_note(note),
          m_velocity(val) {}

    unsigned char getNote() const
    {
        return m_note;
    }

    unsigned char getVelocity() const
    {
        return m_velocity;
    }

protected:
    unsigned char m_note;
    unsigned char m_velocity;
};

#endif // NOTEEVENT_H
