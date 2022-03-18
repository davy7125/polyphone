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
#include "keyboardmap.h"
#include "noteevent.h"
#include "polypressureevent.h"
#include "contextmanager.h"
#include <QApplication>

PianoKeybd::PianoKeybd(QWidget *parent, const int startKey, const int numKeys) : QGraphicsView(parent),
    m_scene(nullptr),
    m_channel(-1)
{
    initialize();
    initScene(startKey, numKeys);
    this->setMouseTracking(true);
}

PianoKeybd::~PianoKeybd()
{
    delete m_scene;
}

void PianoKeybd::initScene(int startKey, int numKeys)
{
    PianoScene * oldScene = m_scene;
    m_scene = new PianoScene(startKey, numKeys, oldScene, this);
    delete oldScene;
    connect(m_scene, SIGNAL(noteOn(int,int)), this, SLOT(onNoteOn(int,int)));
    connect(m_scene, SIGNAL(noteOff(int)), this, SLOT(onNoteOff(int)));
    connect(m_scene, SIGNAL(mouseOver(int, int)), this, SIGNAL(mouseOver(int, int)));
    connect(m_scene, SIGNAL(polyPressureChanged(int,int)), this, SLOT(onPolyPressureChanged(int,int)));
    setScene(m_scene);
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
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
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
    case PROPERTY_COLOR_BLACK_KEYS: m_scene->setColor(-2, value.value<QColor>()); break;
    case PROPERTY_COLOR_WHITE_KEYS: m_scene->setColor(-1, value.value<QColor>()); break;
    case PROPERTY_COLOR_1:          m_scene->setColor(0,  value.value<QColor>()); break;
    case PROPERTY_CHANNEL:
        m_channel = value.toInt();
        break;
    }
}

QVariant PianoKeybd::get(KeyboardProperty keyboardProperty)
{
    QVariant vRet;
    switch (keyboardProperty)
    {
    case PROPERTY_KEY_MIN:
        vRet = m_scene->startKey();
        break;
    case PROPERTY_KEY_NUMBER:
        vRet = m_scene->numKeys();
        break;
    case PROPERTY_COLOR_BLACK_KEYS: vRet = m_scene->getColor(-2); break;
    case PROPERTY_COLOR_WHITE_KEYS: vRet = m_scene->getColor(-1); break;
    case PROPERTY_COLOR_1:          vRet = m_scene->getColor(0);  break;
    case PROPERTY_CHANNEL:
        vRet = m_channel;
        break;
    }
    return vRet;
}

void PianoKeybd::setStartKey(int startKey)
{
    if (startKey != m_scene->startKey())
    {
        initScene(startKey, m_scene->numKeys());
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void PianoKeybd::setNumKeys(int numKeys)
{
    if (numKeys != m_scene->numKeys())
    {
        initScene(m_scene->startKey(), numKeys);
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

QSize PianoKeybd::sizeHint() const 
{ 
    return mapFromScene(sceneRect()).boundingRect().size();
}

double PianoKeybd::ratio() const
{
    return m_scene->getRatio();
}

void PianoKeybd::customize(int key, CustomizationType type, QVariant value)
{
    switch (type)
    {
    case CUSTOMIZATION_TYPE_COLOR:
        m_scene->addCustomColor(key, value.value<QColor>());
        break;
    case CUSTOMIZATION_TYPE_MARKER:
        m_scene->addMarker(key, (MarkerType)value.toInt());
        break;
    }
}

void PianoKeybd::resetCustomization(int key, CustomizationType type)
{
    m_scene->resetCustomization(key, type);
}

void PianoKeybd::clearCustomization()
{
    m_scene->clearCustomization();
}

void PianoKeybd::inputNoteOn(int midiNote, int vel)
{
    m_scene->showNoteOn(midiNote, vel);
}

void PianoKeybd::inputNoteOff(int midiNote)
{
    m_scene->showNoteOff(midiNote);
}

void PianoKeybd::triggerGlowEffect()
{
    m_scene->triggerGlowEffect();
}

void PianoKeybd::updateMapping()
{
    m_scene->updateMapping();
}

void PianoKeybd::onNoteOn(int k,int v)
{
    QApplication::postEvent(ContextManager::midi(), new NoteEvent(m_channel, k, v));
    onNoteOnForChild(k, v);
}

void PianoKeybd::onNoteOff(int k)
{
    QApplication::postEvent(ContextManager::midi(), new NoteEvent(m_channel, k, 0));
    onNoteOffForChild(k);
}

void PianoKeybd::onPolyPressureChanged(int k,int v)
{
    QApplication::postEvent(ContextManager::midi(), new PolyPressureEvent(m_channel, k, v));
}
