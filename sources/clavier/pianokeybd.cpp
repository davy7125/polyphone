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


PianoKeybd::PianoKeybd(QWidget *parent, const int startKey, const int numKeys) : QGraphicsView(parent),
      m_rotation(0),
      m_scene(NULL),
      m_keyboardMap(new KeyboardMap())
{
    initialize();
    PianoScene::initKeyboardMap(m_keyboardMap);
    initScene(startKey, numKeys);
}

PianoKeybd::~PianoKeybd()
{
    delete m_scene;
    delete m_keyboardMap;
}

void PianoKeybd::initScene(int startKey, int numKeys)
{
    PianoScene * oldScene = m_scene;
    m_scene = new PianoScene(startKey, numKeys, oldScene, this);
    delete oldScene;
    connect(m_scene, SIGNAL(noteOn(int,int)), SIGNAL(noteOn(int,int)));
    connect(m_scene, SIGNAL(noteOff(int)), SIGNAL(noteOff(int)));
    connect(m_scene, SIGNAL(mouseOver(int, int)), SIGNAL(mouseOver(int, int)));
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
    setOptimizationFlag(DontClipPainter, true);
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
    case PROPERTY_COLORATION_TYPE:
        m_scene->setColorationType((ColorationType)value.toInt());
        break;
    case PROPERTY_COLOR_TEXT_BLACK_KEYS: m_scene->setColor(-4, value.value<QColor>()); break;
    case PROPERTY_COLOR_TEXT_WHITE_KEYS: m_scene->setColor(-3, value.value<QColor>()); break;
    case PROPERTY_COLOR_BLACK_KEYS: m_scene->setColor(-2, value.value<QColor>()); break;
    case PROPERTY_COLOR_WHITE_KEYS: m_scene->setColor(-1, value.value<QColor>()); break;
    case PROPERTY_COLOR_1:          m_scene->setColor(0,  value.value<QColor>()); break;
    case PROPERTY_ROTATION:
        setRotation(value.toInt());
        break;
    case PROPERTY_TRANSPOSE:
        m_scene->setTranspose(value.toInt());
        break;
    case PROPERTY_CHANNEL:
        m_scene->setChannel(value.toInt());
        break;
    case PROPERTY_VELOCITY:
        m_scene->setVelocity(value.toInt());
        break;
    case PROPERTY_ENABLE_COMPUTER_KEYBOARD:
        m_scene->setKeyboardEnabled(value.toBool());
        break;
    case PROPERTY_ENABLE_MOUSE:
        m_scene->setMouseEnabled(value.toBool());
        break;
    case PROPERTY_ENABLE_TOUCH:
        m_scene->setTouchEnabled(value.toBool());
        break;
    case PROPERTY_LABEL_TYPE:
        m_scene->setLabelType((LabelType)value.toInt());
        break;
    case PROPERTY_MIDDLE_C:
        m_scene->setMiddleC((MiddleKey)value.toInt());
        break;
    case PROPERTY_CUSTOM_LABELS:
        m_scene->setCustomNoteNames(value.toStringList());
        break;
    case PROPERTY_LABEL_OCTAVE_INDICE:
        m_scene->setIndicesInLabels(value.toBool());
        break;
    case PROPERTY_MAPPING_FIRST_NOTE:
        m_keyboardMap->setFirstNote(value.toInt());
        if (m_scene->getLabelType() == LABEL_TYPE_MAPPING)
            m_scene->refreshLabels();
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
    case PROPERTY_COLORATION_TYPE:
        vRet = (int)m_scene->getColorationType();
        break;
    case PROPERTY_COLOR_TEXT_BLACK_KEYS: vRet = m_scene->getColor(-4); break;
    case PROPERTY_COLOR_TEXT_WHITE_KEYS: vRet = m_scene->getColor(-3); break;
    case PROPERTY_COLOR_BLACK_KEYS: vRet = m_scene->getColor(-2); break;
    case PROPERTY_COLOR_WHITE_KEYS: vRet = m_scene->getColor(-1); break;
    case PROPERTY_COLOR_1:          vRet = m_scene->getColor(0);  break;
    case PROPERTY_ROTATION:
        vRet = m_rotation;
        break;
    case PROPERTY_TRANSPOSE:
        vRet = m_scene->getTranspose();
        break;
    case PROPERTY_CHANNEL:
        vRet = m_scene->getChannel();
        break;
    case PROPERTY_VELOCITY:
        vRet = m_scene->getVelocity();
        break;
    case PROPERTY_ENABLE_COMPUTER_KEYBOARD:
        vRet = m_scene->isKeyboardEnabled();
        break;
    case PROPERTY_ENABLE_MOUSE:
        vRet = m_scene->isMouseEnabled();
        break;
    case PROPERTY_ENABLE_TOUCH:
        vRet = m_scene->isTouchEnabled();
        break;
    case PROPERTY_LABEL_TYPE:
        vRet = (int)m_scene->getLabelType();
        break;
    case PROPERTY_MIDDLE_C:
        vRet = (int)m_scene->getMiddleC();
        break;
    case PROPERTY_CUSTOM_LABELS:
        vRet = m_scene->customNoteNames();
        break;
    case PROPERTY_LABEL_OCTAVE_INDICE:
        vRet = m_scene->indicesInLabels();
        break;
    case PROPERTY_MAPPING_FIRST_NOTE:
        vRet = m_keyboardMap->getFirstNote();
        break;
    }
    return vRet;
}

void PianoKeybd::setRotation(int r)
{
    if (r != m_rotation)
    {
        m_rotation = r;
        resetTransform();
        rotate(m_rotation);
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void PianoKeybd::setStartKey(int startKey)
{
    if ( startKey != m_scene->startKey() )
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

void PianoKeybd::inputNoteOn(int midiNote, int vel, int channel)
{
    m_scene->showNoteOn(midiNote, vel, channel);
}

void PianoKeybd::inputNoteOff(int midiNote, int channel)
{
    m_scene->showNoteOff(midiNote, channel);
}

void PianoKeybd::setMapping(Key key, int numOctave, QKeySequence sequence)
{
    m_keyboardMap->setMapping(key, numOctave, sequence);
    if (m_scene->getLabelType() == LABEL_TYPE_MAPPING)
        m_scene->refreshLabels();
}

QKeySequence PianoKeybd::getMapping(Key key, int numOctave)
{
    return m_keyboardMap->getMapping(key, numOctave);
}

void PianoKeybd::triggerNote(int key, int velocity)
{
    if (velocity > 0)
        m_scene->keyNoteOn(key, velocity);
    else
        m_scene->keyNoteOff(key);
}

void PianoKeybd::triggerGlowEffect()
{
    this->m_scene->triggerGlowEffect();
}
