/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
                  2014-2019, Davy Triponney         <davy.triponney@gmail.com>
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
#include "keyboardmap.h"
#include <QGraphicsScene>
#include <QHash>


class PianoScene : public QGraphicsScene
{
    Q_OBJECT
    
public:
    PianoScene (const int startKey, const int numKeys, PianoScene * previousScene = nullptr, QObject * parent = nullptr);
    
    QSize sizeHint() const;
    double getRatio() const { return m_ratio; }

    // Color management
    void setColorationType(PianoKeybd::ColorationType colorationType);
    PianoKeybd::ColorationType getColorationType() { return m_colorationType; }
    void setColor(int num, QColor color);
    QColor getColor(int num) { return m_palette.value(num); }
    void triggerGlowEffect();

    // Customization
    void addCustomColor(int key, QColor color);
    void addMarker(int key, PianoKeybd::MarkerType markerType);
    void clearCustomization();
    void resetCustomization(int key, PianoKeybd::CustomizationType type);

    void updateMapping();
    int getTranspose() const { return m_transpose; }
    void setTranspose(const int transpose);
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

signals:
    void noteOn(int n, int v);
    void noteOff(int n);
    void mouseOver(int n, int v);
    void polyPressureChanged(int n, int p);

protected:
    void showKeyOn(PianoKey* key, int vel = -1, int channel = -1);
    void showKeyOff( PianoKey* key );
    void keyOn(PianoKey* key, qreal pressure);
    void keyOff(PianoKey* key);
    int getKeyVelocity();
    PianoKey* getKeyForPos(const QPointF& p) const;
    int getPressureFromPos(const QPointF& p, bool isBlack) const;

    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void keyPressEvent(QKeyEvent * keyEvent);
    void keyReleaseEvent(QKeyEvent * keyEvent);
    bool event(QEvent *event);

protected slots:
    void updateGlowEffect();

private:
    void initConfiguration(PianoScene *previousScene);
    void createKeyboard();
    void refreshKeys();
    void triggerNoteOn(const int note, const int vel);
    void triggerNoteOff(const int note);
    static qreal sceneWidth(int startKey, int keys);
    static double getOffsetX(int numKey);
    static QString formatSequence(QKeySequence sequence);

    int m_numKeys;
    int m_startKey;
    int m_minNote;
    int m_maxNote;
    int m_transpose;
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
    PianoKeybd::ColorationType m_colorationType;
    QHash<int, PianoKey*> m_keys;
    QMap<int, QColor> m_palette;
    QMap<int, QColor> m_customColors;
    QMap<int, PianoKeybd::MarkerType> m_markers;
    double m_ratio;

    static int MIN_NOTE, MAX_NOTE, KEYWIDTH, KEYHEIGHT;
    KeyboardMap _keybdMap;

    QTimer *_timer;
    double _glowEffect;
};

#endif /*PIANOSCENE_H_*/
