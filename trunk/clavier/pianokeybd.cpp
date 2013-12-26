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

PianoKeybd::PianoKeybd(QWidget *parent) 
    : QGraphicsView(parent), m_rotation(0), m_scene(NULL)
{
    initialize();
    initScene(3, 5);
}

PianoKeybd::PianoKeybd(const int baseOctave, const int numOctaves, QWidget *parent) 
    : QGraphicsView(parent), m_rotation(0), m_scene(NULL)
{
    initialize();
    initScene(baseOctave, numOctaves);
}

PianoKeybd::~PianoKeybd()
{
}

void PianoKeybd::initScene(int base, int num, const QColor& c)
{
    m_scene = new PianoScene(base, num, c, this);
    connect(m_scene, SIGNAL(noteOn(int)), SIGNAL(noteOn(int)));
    connect(m_scene, SIGNAL(noteOff(int)), SIGNAL(noteOff(int)));
    connect(m_scene, SIGNAL(mouseOver(int)), SIGNAL(mouseOver(int)));
    setScene(m_scene);
}

void PianoKeybd::initialize()
{
    setAttribute(Qt::WA_InputMethodEnabled, false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(MinimalViewportUpdate);
    setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
    setOptimizationFlag(DontClipPainter, true);
    setOptimizationFlag(DontSavePainterState, true);
    setOptimizationFlag(DontAdjustForAntialiasing, true);
    //setBackgroundBrush(QApplication::palette().background());
    this->setStyleSheet("background-color: transparent;");
}

void PianoKeybd::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void PianoKeybd::showNoteOn(int midiNote, int vel)
{
    m_scene->showNoteOn(midiNote, vel);
}

void PianoKeybd::showNoteOff(int midiNote, int vel)
{
    m_scene->showNoteOff(midiNote, vel);
}

void PianoKeybd::setNumOctaves(const int numOctaves)
{
    if (numOctaves != m_scene->numOctaves()) {
        int baseOctave = m_scene->baseOctave();
        QColor color = m_scene->getKeyPressedColor();
        PianoHandler* handler = m_scene->getPianoHandler();
        delete m_scene;
        initScene(baseOctave, numOctaves, color);
        m_scene->setPianoHandler(handler);
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void PianoKeybd::setRotation(int r)
{
    if (r != m_rotation) {
        m_rotation = r;
        resetTransform();
        rotate(m_rotation);
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

QSize PianoKeybd::sizeHint() const 
{ 
    return mapFromScene(sceneRect()).boundingRect().size();
}
