/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/


#ifndef PAGETIMBRE_H
#define PAGETIMBRE_H

#include "page.h"
#include "timbremodel.h"
#include "timbreactions.h"
#include "timbremapwidget.h"
#include <QTimer>
#include <functional>
class TimbreBarsWidget;
class TimbreVelocityStrip;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QToolButton;
class QStackedWidget;
class RotatingSpinner;

// Timbre of an instrument along the keyboard, as heard at a velocity, with the tools for harmonizing it (creation of
// computed notes, removal, correction of the timbre and of the level)
class PageTimbre : public Page
{
    Q_OBJECT

public:
    explicit PageTimbre(QWidget * parent = nullptr);
    ~PageTimbre() override;

    bool isSuitableFor(ElementType elementType) override
    {
        return elementType == elementInst || elementType == elementInstSmpl;
    }
    QString getLabel() override { return tr("Timbre"); }
    QString getIconName() override { return ":/icons/timbre.svg"; }

protected:
    void updateInterface(QString editingSource) override;
    void keyPlayedInternal(int key, int velocity) override;
    void hideEvent(QHideEvent * event) override;

private slots:
    void onAnalysisReady(int indexSf2);
    void recompute();
    void onVelocityChanged(int velocity);
    void onSettingsChanged();
    void onThresholdsChanged();
    void onDisplayChanged();
    void onKeySelected(int key, Qt::KeyboardModifiers modifiers);
    void onRangeSelected(int firstKey, int lastKey, Qt::KeyboardModifiers modifiers);
    void onContextMenu(int key, QPoint position);
    void onCreate();
    void onRemove();
    void onReplace();
    void onCorrect();
    void onCorrectLevel();
    void onRender();
    void onMute();
    void onPropose(int label);

private:
    enum SeriesType
    {
        SERIES_CORRECT,
        SERIES_LEVEL,
        SERIES_REMOVE,
        SERIES_REPLACE,
        SERIES_CREATE
    };

    void buildInterface();
    void loadSettings();
    void saveSettings();
    void updateVelocityLayers();
    void updateDetail();
    void updatePending();
    void selectPositions(const QList<int> &keys, bool notifyTree);
    void readTreeSelection();
    QList<int> getTargets();
    QString keyName(int key);
    QString formatDb(double value, int decimals = 1);
    void runInThread(std::function<void()> function, QString text);
    bool askSharedSamples(const QList<int> &targets, bool &duplicate);
    void runSeries(SeriesType type, QList<int> targets, const TimbreCorrectionParameters &parameters,
                   bool removeUnusedSamples, bool duplicateShared);
    bool createNote(int key, int closestKey, int secondKey, int excludedPosition, QStringList &warnings);
    int getBreakKey(int junctionKey);

    TimbreModel _model;
    TimbreSettings _settings;
    TimbreDisplay _pendingDisplay; // Read before the map is created
    EltID _idInst;
    int _selectedKey;
    QList<int> _selectedPositions;
    QTimer _recomputeTimer;
    bool _playClickedNotes;
    int _clickedNote;

    // Rendering of a key, for checking the model
    int _renderedKey;
    int _renderedVelocity;
    TimbreSpectrum _renderedSpectrum;
    bool _seriesFromProposal;

    // Interface
    QStackedWidget * _stack;
    QLabel * _labelMessage;
    QSpinBox * _spinVelocity;
    TimbreVelocityStrip * _velocityStrip;
    QComboBox * _comboMode;
    QComboBox * _comboReference;
    QSpinBox * _spinRegression;
    QSpinBox * _spinDetailed;
    QComboBox * _comboBands;
    QDoubleSpinBox * _spinUpperLimit;
    QDoubleSpinBox * _spinDynamic;
    QDoubleSpinBox * _spinTolerance, * _spinMaximum, * _spinRedundancy, * _spinJump, * _spinLevel;
    QToolButton * _buttonOptions;
    RotatingSpinner * _spinner;
    QLabel * _labelPending;
    TimbreMapWidget * _map;
    TimbreBarsWidget * _bars;
    QLabel * _labelDetail;
    QPushButton * _buttonCreate, * _buttonRemove, * _buttonReplace, * _buttonCorrect, * _buttonLevel, * _buttonRender;
};

#endif // PAGETIMBRE_H
