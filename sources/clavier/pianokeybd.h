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

#ifndef PIANOKEYBD_H
#define PIANOKEYBD_H


#include <QGraphicsView>

class PianoScene;
class KeyboardMap;

class PianoKeybd : public QGraphicsView
{
    Q_OBJECT

public:
    enum KeyboardProperty
    {
        PROPERTY_KEY_MIN,
        PROPERTY_KEY_NUMBER,
        PROPERTY_COLORATION_TYPE,
        PROPERTY_COLOR_1,
        PROPERTY_COLOR_WHITE_KEYS,
        PROPERTY_COLOR_BLACK_KEYS,
        PROPERTY_ROTATION,
        PROPERTY_TRANSPOSE,
        PROPERTY_CHANNEL,
        PROPERTY_VELOCITY
    };

    enum CustomizationType
    {
        CUSTOMIZATION_TYPE_COLOR,
        CUSTOMIZATION_TYPE_MARKER
    };

    enum ColorationType
    {
        COLORATION_TYPE_NONE,
        COLORATION_TYPE_UNIQUE,
        COLORATION_TYPE_DUAL,
        COLORATION_TYPE_CHANNEL,
        COLORATION_TYPE_DEGREE
    };

    enum MarkerType
    {
        MARKER_TYPE_NONE,
        MARKER_TYPE_DOT_WHITE,
        MARKER_TYPE_DOT_RED,
        MARKER_TYPE_DOT_BLUE,
        MARKER_TYPE_DOT_YELLOW,
        MARKER_TYPE_DOT_BLACK
    };

    PianoKeybd(QWidget *parent = nullptr, const int startKey = 36, const int numKeys = 61);
    virtual ~PianoKeybd();

    // Settings
    void set(KeyboardProperty keyboardProperty, QVariant value);
    QVariant get(KeyboardProperty keyboardProperty);

    // Customization
    void customize(int key, CustomizationType type, QVariant value);
    void resetCustomization(int key, CustomizationType type);
    void clearCustomization();

    QSize sizeHint() const;
    double ratio() const;

    void triggerNote(int key, int velocity);
    void triggerGlowEffect();

signals:
    void noteOn(int midiNote, int vel);
    void noteOff(int midiNote);
    void mouseOver(int midiNote, int vel);
    void polyPressureChanged(int midiNote, int pressure);

public slots:
    void inputNoteOn(int midiNote, int vel = -1, int channel = -1);
    void inputNoteOff(int midiNote, int channel = -1);
    void updateMapping();

protected:
    void initialize();
    void initScene(int startKey, int numKeys);
    void resizeEvent(QResizeEvent *event);

private:
    void setRotation(int r);
    void setStartKey(int key);
    void setNumKeys(int numKeys);

    int m_rotation;
    PianoScene * m_scene;
};

#endif // PIANOKEYBD_H
