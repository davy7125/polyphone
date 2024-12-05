/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
    Copyright (C) 2014,      Davy Triponney         <davy.triponney@gmail.com>

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
#include "pianoscene.h"
#include "contextmanager.h"
#include <QApplication>

PianoKeybd::PianoKeybd(QWidget *parent, const int startKey, const int numKeys) : QGraphicsView(parent),
    _scene(nullptr),
    _channel(-1)
{
    initialize();
    initScene(startKey, numKeys);
    this->setMouseTracking(true);
}

PianoKeybd::~PianoKeybd()
{
    delete _scene;
}

void PianoKeybd::initScene(int startKey, int numKeys)
{
    PianoScene * oldScene = _scene;
    _scene = new PianoScene(startKey, numKeys, oldScene, this);
    delete oldScene;
    connect(_scene, SIGNAL(noteOn(int,int)), this, SLOT(onNoteOn(int,int)));
    connect(_scene, SIGNAL(noteOff(int)), this, SLOT(onNoteOff(int)));
    connect(_scene, SIGNAL(mouseOver(int, int)), this, SIGNAL(mouseOver(int, int)));
    connect(_scene, SIGNAL(polyPressureChanged(int,int)), this, SLOT(onPolyPressureChanged(int,int)));
    setScene(_scene);
}

void PianoKeybd::initialize()
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_InputMethodEnabled, false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(MinimalViewportUpdate);
    setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
    setOptimizationFlag(DontSavePainterState, true);
    setOptimizationFlag(DontAdjustForAntialiasing, true);
    this->setStyleSheet("background-color: transparent;");
}

void PianoKeybd::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(_scene->sceneRect());
}

void PianoKeybd::set(KeyboardProperty keyboardProperty, QVariant value)
{
    switch (keyboardProperty)
    {
    case PROPERTY_KEY_MIN:
        setStartKey(value.toInt());
        break;
    case PROPERTY_KEY_NUMBER:
        setNumKeys(value.toInt());
        break;
    case PROPERTY_COLOR_BLACK_KEYS: _scene->setColor(-2, value.value<QColor>()); break;
    case PROPERTY_COLOR_WHITE_KEYS: _scene->setColor(-1, value.value<QColor>()); break;
    case PROPERTY_COLOR_1:          _scene->setColor(0,  value.value<QColor>()); break;
    case PROPERTY_CHANNEL:
        _channel = value.toInt();
        break;
    }
}

QVariant PianoKeybd::get(KeyboardProperty keyboardProperty)
{
    QVariant vRet;
    switch (keyboardProperty)
    {
    case PROPERTY_KEY_MIN:
        vRet = _scene->startKey();
        break;
    case PROPERTY_KEY_NUMBER:
        vRet = _scene->numKeys();
        break;
    case PROPERTY_COLOR_BLACK_KEYS: vRet = _scene->getColor(-2); break;
    case PROPERTY_COLOR_WHITE_KEYS: vRet = _scene->getColor(-1); break;
    case PROPERTY_COLOR_1:          vRet = _scene->getColor(0);  break;
    case PROPERTY_CHANNEL:
        vRet = _channel;
        break;
    }
    return vRet;
}

void PianoKeybd::setStartKey(int startKey)
{
    if (startKey != _scene->startKey())
    {
        initScene(startKey, _scene->numKeys());
        fitInView(_scene->sceneRect());
    }
}

void PianoKeybd::setNumKeys(int numKeys)
{
    if (numKeys != _scene->numKeys())
    {
        initScene(_scene->startKey(), numKeys);
        fitInView(_scene->sceneRect());
    }
}

bool PianoKeybd::hasHeightForWidth() const
{
    return true;
}

QSize PianoKeybd::sizeHint() const
{
    return sceneRect().toRect().size();
}

int PianoKeybd::heightForWidth(int w) const
{
    return static_cast<int>((double)w * sceneRect().height() / sceneRect().width() + 0.5);
}

void PianoKeybd::customize(int key, CustomizationType type, QVariant value)
{
    switch (type)
    {
    case CUSTOMIZATION_TYPE_COLOR:
        _scene->addCustomColor(key, value.value<QColor>());
        break;
    case CUSTOMIZATION_TYPE_MARKER:
        _scene->addMarker(key, (MarkerType)value.toInt());
        break;
    }
}

void PianoKeybd::resetCustomization(int key, CustomizationType type)
{
    _scene->resetCustomization(key, type);
}

void PianoKeybd::clearCustomization()
{
    _scene->clearCustomization();
}

void PianoKeybd::inputNoteOn(int midiNote, int vel)
{
    _scene->showNoteOn(midiNote, vel);
}

void PianoKeybd::inputNoteOff(int midiNote)
{
    _scene->showNoteOff(midiNote);
}

void PianoKeybd::triggerGlowEffect()
{
    _scene->triggerGlowEffect();
}

void PianoKeybd::updateMapping()
{
    _scene->updateMapping();
}

void PianoKeybd::onNoteOn(int k,int v)
{
    ContextManager::midi()->processKeyOn(_channel, k, v);
}

void PianoKeybd::onNoteOff(int k)
{
    ContextManager::midi()->processKeyOff(_channel, k);
}

void PianoKeybd::onPolyPressureChanged(int k,int v)
{
    ContextManager::midi()->processPolyPressureChanged(_channel, k, v);
}
