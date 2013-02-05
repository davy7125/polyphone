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

#include "rawkeybdapp.h"

#if defined(Q_WS_X11)
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
struct qt_auto_repeat_data
{
    // match the window and keycode with timestamp delta of 10 ms
    Window window;
    KeyCode keycode;
    Time timestamp;
    // queue scanner state
    bool release;
    bool error;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static Bool qt_keypress_scanner(Display *, XEvent *event, XPointer arg)
{
    if (event->type != KeyPress && event->type != KeyRelease)
        return false;
    qt_auto_repeat_data *data = (qt_auto_repeat_data *) arg;
    if (data->error)
        return false;
    if (event->xkey.window  != data->window ||
        event->xkey.keycode != data->keycode) {
        // deal breakers: key events in a different window or an event
        // with a different key code
        data->error = true;
        return false;
    }
    if (event->type == KeyPress) {
        data->error = (! data->release || event->xkey.time - data->timestamp > 10);
        return (! data->error);
    }
    // must be XKeyRelease event
    if (data->release) {
        // found a second release
        data->error = true;
        return false;
    }
    // found a single release
    data->release = true;
    data->timestamp = event->xkey.time;
    return false;
}

#if defined(Q_C_CALLBACKS)
}
#endif

bool RawKeybdApp::x11EventFilter ( XEvent * event )
{
    if ( m_enabled && m_handler != NULL && (event->type == KeyPress || event->type == KeyRelease )) {
        bool autorepeat = false;
        Display *dpy =  QX11Info::display();
        // was this the last auto-repeater?
        qt_auto_repeat_data auto_repeat_data;
        auto_repeat_data.window = event->xkey.window;
        auto_repeat_data.keycode = event->xkey.keycode;
        auto_repeat_data.timestamp = event->xkey.time;
        static uint curr_autorep = 0;
        if (event->type == KeyPress) {
            if (curr_autorep == event->xkey.keycode) {
                autorepeat = true;
                curr_autorep = 0;
            }
        } else {
            // look ahead for auto-repeat
            XEvent nextpress;
            auto_repeat_data.release = true;
            auto_repeat_data.error = false;
            autorepeat = XCheckIfEvent( dpy, &nextpress, &qt_keypress_scanner,
                                        (XPointer) &auto_repeat_data );
            curr_autorep = autorepeat ? event->xkey.keycode : 0;
        }
        if (!autorepeat) {
            if (event->type == KeyPress)
                return m_handler->handleKeyPressed(event->xkey.keycode);
            else
                return m_handler->handleKeyReleased(event->xkey.keycode);
        }
        return true;
    }
    return false;
}
#endif

#if defined(Q_WS_WIN)
#include <windows.h>
/* http://msdn.microsoft.com/en-us/library/ms646280(VS.85).aspx */
bool RawKeybdApp::winEventFilter ( MSG * msg, long * result )
{
    if ( m_enabled && m_handler != NULL && (msg->message == WM_KEYDOWN || msg->message == WM_KEYUP )) {
        int keycode = HIWORD(msg->lParam) & 0xff;
        bool autorepeat = (msg->message == WM_KEYDOWN) &&
                          ((HIWORD(msg->lParam) & 0x4000) != 0);
        if (!autorepeat) {
            if ( msg->message == WM_KEYDOWN )
                return m_handler->handleKeyPressed(keycode);
            else
                return m_handler->handleKeyReleased(keycode);
        }
        return true;
    }
    return false;
}
#endif

#if defined(Q_WS_MAC)
#include <Carbon/Carbon.h>
bool RawKeybdApp::macEventFilter( EventHandlerCallRef /*caller*/, EventRef event )
{
    if (m_enabled && m_handler != NULL && GetEventClass(event) == kEventClassKeyboard) {
        UInt32 ekind = GetEventKind(event);
        if (ekind == kEventRawKeyUp || ekind == kEventRawKeyDown) {
            UInt32 keyCode = 0;
            GetEventParameter(event, kEventParamKeyCode, typeUInt32, 0, sizeof(keyCode), 0, &keyCode);
            if ( ekind == kEventRawKeyDown )
                return m_handler->handleKeyPressed(keyCode);
            else
                return m_handler->handleKeyReleased(keyCode);
        }
        return true;
    }
    return false;
}
#endif  // Q_WS_MAC
