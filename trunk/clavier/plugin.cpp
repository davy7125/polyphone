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

#include "pianokeybd.h"
#include "plugin.h"

#include <QtPlugin>

PianoKeybdPlugin::PianoKeybdPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void PianoKeybdPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool PianoKeybdPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PianoKeybdPlugin::createWidget(QWidget *parent)
{
    return new PianoKeybd(parent);
}

QString PianoKeybdPlugin::name() const
{
    return "PianoKeybd";
}

QString PianoKeybdPlugin::group() const
{
    return "Audio/MIDI Widgets";
}

QIcon PianoKeybdPlugin::icon() const
{
    return QIcon(":/vpiano/pianokeybd.png");
}

QString PianoKeybdPlugin::toolTip() const
{
    return "Virtual piano keyboard";
}

QString PianoKeybdPlugin::whatsThis() const
{
    return "The virtual piano keyboard is a MIDI keyboard emulator";
}

bool PianoKeybdPlugin::isContainer() const
{
    return false;
}

QString PianoKeybdPlugin::includeFile() const
{
    return "pianokeybd.h";
}

Q_EXPORT_PLUGIN2(vpianokeybd, PianoKeybdPlugin)
