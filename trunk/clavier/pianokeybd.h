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

//#if defined(VPIANO_PLUGIN)
//#include <QtDesigner/QDesignerExportWidget>
//#else
//#define QDESIGNER_WIDGET_EXPORT
//#endif

class /*QDESIGNER_WIDGET_EXPORT*/ PianoKeybd : public QGraphicsView
{
    Q_OBJECT

#if defined(VPIANO_PLUGIN)
    Q_CLASSINFO("Author", "Pedro Lopez-Cabanillas <plcl@users.sf.net>")
    Q_CLASSINFO("URL", "http://sourceforge.net/projects/vmpk")
    Q_CLASSINFO("Version", "0.20")
#endif

public:
    enum KeyboardProperty
    {
        PROPERTY_KEY_MIN,
        PROPERTY_KEY_NUMBER,
        PROPERTY_COLORATION_TYPE,
        PROPERTY_COLOR_1,
        PROPERTY_COLOR_2,
        PROPERTY_COLOR_3,
        PROPERTY_COLOR_4,
        PROPERTY_COLOR_5,
        PROPERTY_COLOR_6,
        PROPERTY_COLOR_7,
        PROPERTY_COLOR_8,
        PROPERTY_COLOR_9,
        PROPERTY_COLOR_10,
        PROPERTY_COLOR_11,
        PROPERTY_COLOR_12,
        PROPERTY_COLOR_13,
        PROPERTY_COLOR_14,
        PROPERTY_COLOR_15,
        PROPERTY_COLOR_16,
        PROPERTY_COLOR_WHITE_KEYS,
        PROPERTY_COLOR_BLACK_KEYS,
        PROPERTY_COLOR_TEXT_WHITE_KEYS,
        PROPERTY_COLOR_TEXT_BLACK_KEYS,
        PROPERTY_ROTATION,
        PROPERTY_TRANSPOSE,
        PROPERTY_LABEL_TYPE,
        PROPERTY_CUSTOM_LABELS,
        PROPERTY_MIDDLE_C,
        PROPERTY_LABEL_OCTAVE_INDICE,
        PROPERTY_CHANNEL,
        PROPERTY_VELOCITY,
        PROPERTY_ENABLE_COMPUTER_KEYBOARD,
        PROPERTY_ENABLE_MOUSE,
        PROPERTY_ENABLE_TOUCH,
        PROPERTY_MAPPING_FIRST_NOTE
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

    enum LabelType
    {
        LABEL_TYPE_NONE,
        LABEL_TYPE_KEY_NAME_WITH_SHARPS,
        LABEL_TYPE_KEY_NAME_WITH_FLATS,
        LABEL_TYPE_NUMBER,
        LABEL_TYPE_OCTAVES,
        LABEL_TYPE_MAPPING,
        LABEL_TYPE_CUSTOM,
        LABEL_TYPE_CUSTOM_WITH_OCTAVE_NUMBER
    };

    enum MarkerType
    {
        MARKER_TYPE_NONE,
        MARKER_TYPE_DOT_WHITE,
        MARKER_TYPE_DOT_RED,
        MARKER_TYPE_DOT_BLUE,
        MARKER_TYPE_DOT_YELLOW,
        MARKER_TYPE_DOT_BLACK,
        MARKER_TYPE_STAR_WHITE,
        MARKER_TYPE_STAR_RED,
        MARKER_TYPE_STAR_BLUE,
        MARKER_TYPE_STAR_YELLOW,
        MARKER_TYPE_STAR_BLACK,
        MARKER_TYPE_ARROW_WHITE,
        MARKER_TYPE_ARROW_RED,
        MARKER_TYPE_ARROW_BLUE,
        MARKER_TYPE_ARROW_YELLOW,
        MARKER_TYPE_ARROW_BLACK,
        MARKER_TYPE_CROSS_WHITE,
        MARKER_TYPE_CROSS_RED,
        MARKER_TYPE_CROSS_BLUE,
        MARKER_TYPE_CROSS_YELLOW,
        MARKER_TYPE_CROSS_BLACK
    };

    enum MiddleKey
    {
        MIDDLE_KEY_C3,
        MIDDLE_KEY_C4,
        MIDDLE_KEY_C5
    };

    enum Key
    {
        KEY_C_LEFT,
        KEY_C_SHARP,
        KEY_D,
        KEY_D_SHARP,
        KEY_E,
        KEY_F,
        KEY_F_SHARP,
        KEY_G,
        KEY_G_SHARP,
        KEY_A,
        KEY_A_SHARP,
        KEY_B,
        KEY_C_RIGHT
    };

    PianoKeybd(QWidget *parent = NULL, const int startKey = 36, const int numKeys = 61);
    virtual ~PianoKeybd();

    // Settings
    void set(KeyboardProperty keyboardProperty, QVariant value);
    QVariant get(KeyboardProperty keyboardProperty);

    // Customization
    void customize(int key, CustomizationType type, QVariant value);
    void resetCustomization(int key, CustomizationType type);
    void clearCustomization();

    // Mapping
    void setMapping(Key key, int numOctave, QKeySequence sequence);
    QKeySequence getMapping(Key key, int numOctave);

    QSize sizeHint() const;
    double ratio() const;

    void triggerNote(int key, int velocity);

signals:
    void noteOn(int midiNote, int vel);
    void noteOff(int midiNote);
    void mouseOver(int midiNote, int vel);

public slots:
    void inputNoteOn(int midiNote, int vel = -1, int channel = -1);
    void inputNoteOff(int midiNote, int channel = -1);

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
    KeyboardMap * m_keyboardMap;
};

#endif // PIANOKEYBD_H
