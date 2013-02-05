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
    : QGraphicsView(parent), m_rotation(0), m_scene(NULL), m_rawMap(NULL)
{
    initialize();
    initScene(3, 5);
}

PianoKeybd::PianoKeybd(const int baseOctave, const int numOctaves, QWidget *parent) 
    : QGraphicsView(parent), m_rotation(0), m_scene(NULL), m_rawMap(NULL)
{
    initialize();
    initScene(baseOctave, numOctaves);
}

PianoKeybd::~PianoKeybd()
{
    setRawKeyboardMode(false);
    setRawKeyboardMap(NULL);
}

void PianoKeybd::initScene(int base, int num, const QColor& c)
{
    m_scene = new PianoScene(base, num, c, this);
    m_scene->setKeyboardMap(&m_defaultMap);
    connect(m_scene, SIGNAL(noteOn(int)), SIGNAL(noteOn(int)));
    connect(m_scene, SIGNAL(noteOff(int)), SIGNAL(noteOff(int)));
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
    setBackgroundBrush(QApplication::palette().background());
    initDefaultMap();
    RawKeybdApp* rapp = dynamic_cast<RawKeybdApp*>(qApp);
    if (rapp != NULL) {
        rapp->setRawKbdHandler(this);
    }
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

void PianoKeybd::initDefaultMap()
{
    // Default translated Keyboard Map
    m_defaultMap.insert(Qt::Key_Z, 12);
    m_defaultMap.insert(Qt::Key_S, 13);
    m_defaultMap.insert(Qt::Key_X, 14);
    m_defaultMap.insert(Qt::Key_D, 15);
    m_defaultMap.insert(Qt::Key_C, 16);
    m_defaultMap.insert(Qt::Key_V, 17);
    m_defaultMap.insert(Qt::Key_G, 18);
    m_defaultMap.insert(Qt::Key_B, 19);
    m_defaultMap.insert(Qt::Key_H, 20);
    m_defaultMap.insert(Qt::Key_N, 21);
    m_defaultMap.insert(Qt::Key_J, 22);
    m_defaultMap.insert(Qt::Key_M, 23);
    m_defaultMap.insert(Qt::Key_Q, 24);
    m_defaultMap.insert(Qt::Key_2, 25);
    m_defaultMap.insert(Qt::Key_W, 26);
    m_defaultMap.insert(Qt::Key_3, 27);
    m_defaultMap.insert(Qt::Key_E, 28);
    m_defaultMap.insert(Qt::Key_R, 29);
    m_defaultMap.insert(Qt::Key_5, 30);
    m_defaultMap.insert(Qt::Key_T, 31);
    m_defaultMap.insert(Qt::Key_6, 32);
    m_defaultMap.insert(Qt::Key_Y, 33);
    m_defaultMap.insert(Qt::Key_7, 34);
    m_defaultMap.insert(Qt::Key_U, 35);
    m_defaultMap.insert(Qt::Key_I, 36);
    m_defaultMap.insert(Qt::Key_9, 37);
    m_defaultMap.insert(Qt::Key_O, 38);
    m_defaultMap.insert(Qt::Key_0, 39);
    m_defaultMap.insert(Qt::Key_P, 40);

    // Default Raw Keyboard Map
#if defined(Q_WS_X11)
    m_defaultRawMap.insert(94, 11);
    m_defaultRawMap.insert(52, 12);
    m_defaultRawMap.insert(39, 13);
    m_defaultRawMap.insert(53, 14);
    m_defaultRawMap.insert(40, 15);
    m_defaultRawMap.insert(54, 16);
    m_defaultRawMap.insert(55, 17);
    m_defaultRawMap.insert(42, 18);
    m_defaultRawMap.insert(56, 19);
    m_defaultRawMap.insert(43, 20);
    m_defaultRawMap.insert(57, 21);
    m_defaultRawMap.insert(44, 22);
    m_defaultRawMap.insert(58, 23);
    m_defaultRawMap.insert(59, 24);
    m_defaultRawMap.insert(46, 25);
    m_defaultRawMap.insert(60, 26);
    m_defaultRawMap.insert(47, 27);
    m_defaultRawMap.insert(61, 28);

    m_defaultRawMap.insert(24, 29);
    m_defaultRawMap.insert(11, 30);
    m_defaultRawMap.insert(25, 31);
    m_defaultRawMap.insert(12, 32);
    m_defaultRawMap.insert(26, 33);
    m_defaultRawMap.insert(13, 34);
    m_defaultRawMap.insert(27, 35);
    m_defaultRawMap.insert(28, 36);
    m_defaultRawMap.insert(15, 37);
    m_defaultRawMap.insert(29, 38);
    m_defaultRawMap.insert(16, 39);
    m_defaultRawMap.insert(30, 40);
    m_defaultRawMap.insert(31, 41);
    m_defaultRawMap.insert(18, 42);
    m_defaultRawMap.insert(32, 43);
    m_defaultRawMap.insert(19, 44);
    m_defaultRawMap.insert(33, 45);
    m_defaultRawMap.insert(20, 46);
    m_defaultRawMap.insert(34, 47);
    m_defaultRawMap.insert(35, 48);
#endif

#if defined(Q_WS_WIN)
    m_defaultRawMap.insert(86, 11);
    m_defaultRawMap.insert(44, 12);
    m_defaultRawMap.insert(31, 13);
    m_defaultRawMap.insert(45, 14);
    m_defaultRawMap.insert(32, 15);
    m_defaultRawMap.insert(46, 16);
    m_defaultRawMap.insert(47, 17);
    m_defaultRawMap.insert(34, 18);
    m_defaultRawMap.insert(48, 19);
    m_defaultRawMap.insert(35, 20);
    m_defaultRawMap.insert(49, 21);
    m_defaultRawMap.insert(36, 22);
    m_defaultRawMap.insert(50, 23);
    m_defaultRawMap.insert(51, 24);
    m_defaultRawMap.insert(38, 25);
    m_defaultRawMap.insert(52, 26);
    m_defaultRawMap.insert(39, 27);
    m_defaultRawMap.insert(53, 28);

    m_defaultRawMap.insert(16, 29);
    m_defaultRawMap.insert(3, 30);
    m_defaultRawMap.insert(17, 31);
    m_defaultRawMap.insert(4, 32);
    m_defaultRawMap.insert(18, 33);
    m_defaultRawMap.insert(5, 34);
    m_defaultRawMap.insert(19, 35);
    m_defaultRawMap.insert(20, 36);
    m_defaultRawMap.insert(7, 37);
    m_defaultRawMap.insert(21, 38);
    m_defaultRawMap.insert(8, 39);
    m_defaultRawMap.insert(22, 40);
    m_defaultRawMap.insert(23, 41);
    m_defaultRawMap.insert(10, 42);
    m_defaultRawMap.insert(24, 43);
    m_defaultRawMap.insert(11, 44);
    m_defaultRawMap.insert(25, 45);
    m_defaultRawMap.insert(12, 46);
    m_defaultRawMap.insert(26, 47);
    m_defaultRawMap.insert(27, 48);
#endif

#if defined(Q_WS_MAC)
    m_defaultRawMap.insert(50, 11);
    m_defaultRawMap.insert(6, 12);
    m_defaultRawMap.insert(1, 13);
    m_defaultRawMap.insert(7, 14);
    m_defaultRawMap.insert(2, 15);
    m_defaultRawMap.insert(8, 16);
    m_defaultRawMap.insert(9, 17);
    m_defaultRawMap.insert(5, 18);
    m_defaultRawMap.insert(11, 19);
    m_defaultRawMap.insert(4, 20);
    m_defaultRawMap.insert(45, 21);
    m_defaultRawMap.insert(38, 22);
    m_defaultRawMap.insert(46, 23);
    m_defaultRawMap.insert(43, 24);
    m_defaultRawMap.insert(37, 25);
    m_defaultRawMap.insert(47, 26);
    m_defaultRawMap.insert(41, 27);
    m_defaultRawMap.insert(44, 28);

    m_defaultRawMap.insert(12, 29);
    m_defaultRawMap.insert(19, 30);
    m_defaultRawMap.insert(13, 31);
    m_defaultRawMap.insert(20, 32);
    m_defaultRawMap.insert(14, 33);
    m_defaultRawMap.insert(21, 34);
    m_defaultRawMap.insert(15, 35);
    m_defaultRawMap.insert(17, 36);
    m_defaultRawMap.insert(22, 37);
    m_defaultRawMap.insert(16, 38);
    m_defaultRawMap.insert(26, 39);
    m_defaultRawMap.insert(32, 40);
    m_defaultRawMap.insert(34, 41);
    m_defaultRawMap.insert(25, 42);
    m_defaultRawMap.insert(31, 43);
    m_defaultRawMap.insert(29, 44);
    m_defaultRawMap.insert(35, 45);
    m_defaultRawMap.insert(27, 46);
    m_defaultRawMap.insert(33, 47);
    m_defaultRawMap.insert(30, 48);
#endif
    m_rawMap = &m_defaultRawMap;
}

void PianoKeybd::setNumOctaves(const int numOctaves)
{
    if (numOctaves != m_scene->numOctaves()) {
        int baseOctave = m_scene->baseOctave();
        QColor color = m_scene->getKeyPressedColor();
        PianoHandler* handler = m_scene->getPianoHandler();
        KeyboardMap* keyMap = m_scene->getKeyboardMap();
        delete m_scene;
        initScene(baseOctave, numOctaves, color);
        m_scene->setPianoHandler(handler);
        m_scene->setKeyboardMap(keyMap);
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

bool PianoKeybd::handleKeyPressed(int keycode)
{
    if (m_rawMap != NULL && m_rawMap->contains(keycode)) {
        m_scene->keyOn(m_rawMap->value(keycode));
        return true;
    }
    return false;
}

bool PianoKeybd::handleKeyReleased(int keycode)
{
    if (m_rawMap != NULL && m_rawMap->contains(keycode)) {
        m_scene->keyOff(m_rawMap->value(keycode));
        return true;
    }
    return false;
}
