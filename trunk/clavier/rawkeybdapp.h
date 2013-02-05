/*
    Virtual Piano Widget for Qt4
    Copyright (C) 2008-2010, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RAWKEYBDAPP_H
#define RAWKEYBDAPP_H

#include "pianodefs.h"
#include <QtGui/QApplication>

class RawKbdHandler {
public:
    virtual ~RawKbdHandler() {}
    virtual bool handleKeyPressed(int keycode) = 0;
    virtual bool handleKeyReleased(int keycode) = 0;
};

class VPIANO_EXPORT RawKeybdApp : public QApplication
{
public:
    RawKeybdApp( int & argc, char ** argv ) : QApplication(argc, argv),
        m_enabled(false), m_handler(NULL) {}
    virtual ~RawKeybdApp() {}

    RawKbdHandler *getRawKbdHandler() { return m_handler; }
    void setRawKbdHandler(RawKbdHandler* h) { m_handler = h; }
    bool getRawKbdEnable() { return m_enabled; }
    void setRawKbdEnable(bool b) { m_enabled = b; }

#if defined(Q_WS_X11)
    bool x11EventFilter ( XEvent * event );
#endif

#if defined(Q_WS_MAC)
    bool macEventFilter ( EventHandlerCallRef caller, EventRef event );
#endif

#if defined(Q_WS_WIN)
    bool winEventFilter ( MSG * msg, long * result );
#endif

private:
    bool m_enabled;
    RawKbdHandler *m_handler;
};

#endif // RAWKEYBDAPP_H
