#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <QEvent>

class ControllerEvent : public QEvent
{
public:
    ControllerEvent(unsigned char numController, unsigned char value) : QEvent((QEvent::Type)(QEvent::User+1)),
          m_numController(numController),
          m_value(value) {}

    unsigned char getNumController() const
    {
        return m_numController;
    }

    unsigned char getValue() const
    {
        return m_value;
    }

protected:
    unsigned char m_numController;
    unsigned char m_value;
};

#endif // CONTROLLEREVENT_H
