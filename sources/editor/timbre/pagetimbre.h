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
#include <QMap>
#include <functional>
class TimbreBarsWidget;
class TimbreDetailWidget;
class QLabel;
class QSlider;
class QComboBox;
class QFrame;
class QCheckBox;
class QToolButton;
class QStackedWidget;
class RotatingSpinner;
class InfoButton;

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

    // Items of the list of references, shared with the correction window: interpolation between the two neighbors (data
    // 0), then robust regression on 2 to 8 neighbors on each side (data: count on each side)
    static void addReferenceItems(QComboBox * combo);

    static QString keyName(int key);
    static QString formatDb(double value, int decimals = 1, bool sign = false);

protected:
    void updateInterface(QString editingSource) override;
    void keyPlayedInternal(int key, int velocity) override;
    void hideEvent(QHideEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void onAnalysisReady(int indexSf2);
    void recompute();
    void onVelocityChanged(int velocity);
    void onSettingsChanged();
    void onThresholdsChanged();
    void onDisplayChanged();
    void onKeySelected(int key, Qt::KeyboardModifiers modifiers);
    void onCreate();
    void onRemove();
    void onReplace();
    void onCorrect();
    void onCorrectLevel();
    void onRender();
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
    void updateDetail();
    void updateToleranceInfo();
    void updateLegend();
    void updatePending();
    void placePendingFrame();
    void selectPositions(const QList<int> &keys, bool notifyTree);
    void readTreeSelection();
    void restoreSelection(int key, const QList<int> &positions, int anchorKey);
    QList<int> getTargets();
    int getPosition(int key);
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
    int _anchorKey; // Root key from which Shift + click selects
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
    QSlider * _sliderVelocity;
    QLabel * _labelVelocity;
    TimbreLegendWidget * _legend;
    QComboBox * _comboMode;
    QComboBox * _comboReference;
    QSlider * _sliderTolerance; // Half dB
    QLabel * _labelTolerance;
    InfoButton * _buttonToleranceInfo; // Thresholds
    QMap<QObject *, QCheckBox *> _optionCheckBoxes; // By label
    QList<QCheckBox *> _summaryCheckBoxes;
    QCheckBox * _checkPlayClicked;
    QFrame * _framePending; // Analysis in progress, over the top right corner of the map
    RotatingSpinner * _spinner;
    QLabel * _labelPending;
    TimbreMapWidget * _map;
    QStackedWidget * _stackDetail; // Graph and details, or a message if no key is selected
    TimbreBarsWidget * _bars;
    TimbreDetailWidget * _detail;
    QLabel * _labelNoSelection;
    QPushButton * _buttonCreate, * _buttonRemove, * _buttonReplace, * _buttonCorrect, * _buttonLevel, * _buttonRender;
};

#endif // PAGETIMBRE_H
