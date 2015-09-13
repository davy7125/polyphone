/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
                  2014,      Davy Triponney         <davy.triponney@gmail.com>
                  2014,      Andrea Celani          <acelani74@gmail.com>

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

#ifndef PIANOSCENE_H_
#define PIANOSCENE_H_

#include "pianokeybd.h"
#include "pianokey.h"
#include "keylabel.h"
#include "keyboardmap.h"
#include <QGraphicsScene>
#include <QHash>


class PianoScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    PianoScene (const int startKey, const int numKeys, PianoScene * previousScene = NULL, QObject * parent = NULL);
    
    QSize sizeHint() const;
    double getRatio() const { return m_ratio; }
    static void initKeyboardMap( KeyboardMap* map ) { m_keybdMap = map; }

    // Color management
    void setColorationType(PianoKeybd::ColorationType colorationType);
    PianoKeybd::ColorationType getColorationType() { return m_colorationType; }
    void setColor(int num, QColor color);
    QColor getColor(int num) { return m_palette.value(num); }

    // Customization
    void addCustomColor(int key, QColor color);
    void addMarker(int key, PianoKeybd::MarkerType markerType);
    void clearCustomization();
    void resetCustomization(int key, PianoKeybd::CustomizationType type);

    // Labels
    void setLabelType(PianoKeybd::LabelType type);
    PianoKeybd::LabelType getLabelType() { return m_labelType; }
    void setMiddleC(PianoKeybd::MiddleKey middleC);
    PianoKeybd::MiddleKey getMiddleC() { return m_middleC; }
    void setCustomNoteNames(const QStringList& names);
    QStringList customNoteNames() const { return m_names_custom; }
    void setIndicesInLabels(bool enabled);
    bool indicesInLabels() { return m_indicesInLabels; }

    int getTranspose() const { return m_transpose; }
    void setTranspose(const int transpose);
    bool isKeyboardEnabled() const { return m_keyboardEnabled; }
    void setKeyboardEnabled( const bool enable );
    bool isMouseEnabled() const { return m_mouseEnabled; }
    void setMouseEnabled( const bool enable );
    bool isTouchEnabled() const { return m_touchEnabled; }
    void setTouchEnabled( const bool enable );
    void showNoteOn(const int note, int vel = -1, int channel = -1);
    void showNoteOff(const int note, int channel = -1);
    int numKeys() const { return m_numKeys; }
    int startKey() const { return m_startKey; }
    void allKeysOff();
    void keyNoteOn( const int note, const int velo);
    void keyNoteOff( const int note);
    int getVelocity() { return m_velocity; }
    void setVelocity(const int velocity) { m_velocity = m_lastVelocity = velocity; }
    int getChannel() const { return m_channel; }
    void setChannel(const int channel) { m_channel = channel; }

    void refreshLabels();

signals:
    void noteOn(int n, int v);
    void noteOff(int n);
    void mouseOver(int n, int v);

protected:
    void showKeyOn(PianoKey* key, int vel = -1, int channel = -1);
    void showKeyOff( PianoKey* key );
    void keyOn(PianoKey* key, qreal pressure);
    void keyOff(PianoKey* key);
    int getKeyVelocity();
    PianoKey* getKeyForPos(const QPointF& p) const;
    int getPressureFromPos(const QPointF& p, bool isBlack) const;
    QString noteName(int note);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void keyPressEvent(QKeyEvent * keyEvent );
    void keyReleaseEvent(QKeyEvent * keyEvent );
    bool event(QEvent *event);

private:
    void initConfiguration(PianoScene *previousScene);
    void initLabels();
    void createKeyboard();
    void refreshKeys();
    void triggerNoteOn(const int note, const int vel);
    void triggerNoteOff(const int note);
    static qreal sceneWidth(int startKey, int keys);
    static float getOffsetX(int numKey);
    static QString formatSequence(QKeySequence sequence);

    int m_numKeys;
    int m_startKey;
    int m_minNote;
    int m_maxNote;
    int m_transpose;
    bool m_keyboardEnabled;
    bool m_mouseEnabled;
    bool m_touchEnabled;
    bool m_mousePressed;
    int m_velocity;
    int m_lastVelocity;
    bool m_autoVelocity;
    int m_lastNote;
    int m_lastNoteUp;
    int m_lastNoteDown;
    int m_lastNoteLeft;
    int m_lastNoteRight;
    int m_currentArrow;
    int m_channel;
    PianoKeybd::LabelType m_labelType;
    PianoKeybd::MiddleKey m_middleC;
    PianoKeybd::ColorationType m_colorationType;
    QHash<int, PianoKey*> m_keys;
    QMap<int, KeyLabel*> m_labels;
    QStringList m_names_s, m_names_f, m_names_custom;
    bool m_indicesInLabels;
    QMap<int, QColor> m_palette;
    QMap<int, QColor> m_customColors;
    QMap<int, PianoKeybd::MarkerType> m_markers;
    double m_ratio;

    static int MIN_NOTE, MAX_NOTE, KEYWIDTH, KEYHEIGHT;
    static KeyboardMap* m_keybdMap;
};

#endif /*PIANOSCENE_H_*/
