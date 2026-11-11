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


#include "pagetimbre.h"
#include "timbrebarswidget.h"
#include "dialogtimbrecorrection.h"
#include "dialogtimbrecreation.h"
#include "timbrerendering.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "solomanager.h"
#include "rotatingspinner.h"
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QStackedWidget>
#include <QSplitter>
#include <QScrollArea>
#include <QMenu>
#include <QWidgetAction>
#include <QFormLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QEventLoop>
#include <cmath>

PageTimbre::PageTimbre(QWidget * parent) : Page(parent, "page:timbre"),
    _selectedKey(-1),
    _playClickedNotes(false),
    _clickedNote(-1),
    _renderedKey(-1),
    _renderedVelocity(-1),
    _seriesFromProposal(false)
{
    loadSettings();
    buildInterface();
    _recomputeTimer.setSingleShot(true);
    _recomputeTimer.setInterval(150);
    connect(&_recomputeTimer, SIGNAL(timeout()), this, SLOT(recompute()));
    connect(TimbreAnalysisCache::getInstance(), SIGNAL(analysisReady(int)), this, SLOT(onAnalysisReady(int)));
}

PageTimbre::~PageTimbre()
{
    saveSettings();
}

void PageTimbre::loadSettings()
{
    ConfManager * conf = ContextManager::configuration();
    _settings.velocity = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_velocity", 100).toInt();
    _settings.detailedRanks = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_detailed_ranks", 10).toInt();
    _settings.octaveBands = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_octave_bands", false).toBool();
    _settings.upperLimit = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_upper_limit", 16000).toDouble();
    _settings.dynamicRange = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_dynamic_range", 60).toDouble();
    _settings.reference = static_cast<TimbreSettings::Reference>(
        conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_reference", 0).toInt() == 1 ? 1 : 0);
    _settings.regressionCount = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_regression_count", 3).toInt();
    _settings.tolerance = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_tolerance", _settings.tolerance).toDouble();
    _settings.maximum = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_maximum", _settings.maximum).toDouble();
    _settings.redundancy = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_redundancy", _settings.redundancy).toDouble();
    _settings.jump = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_jump", _settings.jump).toDouble();
    _settings.level = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_level", _settings.level).toDouble();

    TimbreDisplay display;
    int mode = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_mode", TimbreDisplay::MODE_DEVIATION).toInt();
    display.mode = static_cast<TimbreDisplay::Mode>(qBound(0, mode, 5));
    display.steps = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_steps", false).toBool();
    display.positionsOnly = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_positions_only", false).toBool();
    display.fullKeyboard = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_full_keyboard", false).toBool();
    display.autoRange = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_auto_range", true).toBool();
    display.fixedRange = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_fixed_range", 12).toDouble();
    QList<int> summaries;
    summaries << TimbreDisplay::SUMMARY_LEVEL << TimbreDisplay::SUMMARY_TIMBRE << TimbreDisplay::SUMMARY_TRANSPOSITION;
    QStringList extra = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_summaries", "").toString().split(",", Qt::SkipEmptyParts);
    foreach (QString value, extra)
    {
        int summary = value.toInt();
        if (summary >= TimbreDisplay::SUMMARY_BRIGHTNESS && summary <= TimbreDisplay::SUMMARY_LOOP && !summaries.contains(summary))
            summaries << summary;
    }
    display.summaries = summaries;
    _playClickedNotes = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_play_clicked", false).toBool();
    _map = nullptr;
    _pendingDisplay = display;
}

void PageTimbre::saveSettings()
{
    ConfManager * conf = ContextManager::configuration();
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_velocity", _settings.velocity);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_detailed_ranks", _settings.detailedRanks);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_octave_bands", _settings.octaveBands);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_upper_limit", _settings.upperLimit);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_dynamic_range", _settings.dynamicRange);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_reference", static_cast<int>(_settings.reference));
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_regression_count", _settings.regressionCount);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_tolerance", _settings.tolerance);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_maximum", _settings.maximum);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_redundancy", _settings.redundancy);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_jump", _settings.jump);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_level", _settings.level);
    if (_map != nullptr)
    {
        const TimbreDisplay &display = _map->getDisplay();
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_mode", static_cast<int>(display.mode));
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_steps", display.steps);
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_positions_only", display.positionsOnly);
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_full_keyboard", display.fullKeyboard);
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_auto_range", display.autoRange);
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_fixed_range", display.fixedRange);
        QStringList extra;
        foreach (int summary, display.summaries)
            if (summary >= TimbreDisplay::SUMMARY_BRIGHTNESS)
                extra << QString::number(summary);
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_summaries", extra.join(","));
    }
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_play_clicked", _playClickedNotes);
}

static QIcon shapeIcon(TimbrePalette::Shape shape, QColor color)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    TimbrePalette::drawShape(painter, shape, QRectF(0, 0, 32, 32), color);
    return QIcon(pixmap);
}

void PageTimbre::buildInterface()
{
    TimbrePalette palette;
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    _stack = new QStackedWidget(this);
    layout->addWidget(_stack);

    QWidget * main = new QWidget(_stack);
    QVBoxLayout * mainLayout = new QVBoxLayout(main);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    _stack->addWidget(main);
    _labelMessage = new QLabel(_stack);
    _labelMessage->setAlignment(Qt::AlignCenter);
    QFont font = _labelMessage->font();
    font.setBold(true);
    _labelMessage->setFont(font);
    _stack->addWidget(_labelMessage);

    // Top bar
    QHBoxLayout * top = new QHBoxLayout();
    top->addWidget(new QLabel(tr("Velocity"), main));
    _spinVelocity = new QSpinBox(main);
    _spinVelocity->setRange(1, 127);
    _spinVelocity->setValue(_settings.velocity);
    top->addWidget(_spinVelocity);
    _velocityStrip = new TimbreVelocityStrip(main);
    _velocityStrip->setFixedWidth(140);
    _velocityStrip->setVelocity(_settings.velocity);
    top->addWidget(_velocityStrip);
    top->addSpacing(12);

    top->addWidget(new QLabel(tr("Display"), main));
    _comboMode = new QComboBox(main);
    for (int mode = 0; mode <= TimbreDisplay::MODE_CONFIDENCE; mode++)
        _comboMode->addItem(TimbreMapWidget::getModeName(mode));
    _comboMode->setCurrentIndex(_pendingDisplay.mode);
    top->addWidget(_comboMode);
    top->addStretch();
    _spinner = new RotatingSpinner(main);
    _spinner->setFixedSize(16, 16);
    _spinner->hide();
    top->addWidget(_spinner);
    _labelPending = new QLabel(main);
    top->addWidget(_labelPending);
    mainLayout->addLayout(top);

    // Second row
    top = new QHBoxLayout();
    top->addWidget(new QLabel(tr("Reference"), main));
    _comboReference = new QComboBox(main);
    _comboReference->addItem(tr("Neighbors"));
    _comboReference->addItem(tr("Local regression"));
    _comboReference->setToolTip(tr("Reference of a root key: interpolation between its two neighbors, or robust regression on "
                                   "several neighbors on each side"));
    _comboReference->setCurrentIndex(_settings.reference);
    top->addWidget(_comboReference);
    _spinRegression = new QSpinBox(main);
    _spinRegression->setRange(2, 8);
    _spinRegression->setValue(_settings.regressionCount);
    _spinRegression->setToolTip(tr("Neighbors on each side"));
    _spinRegression->setVisible(_settings.reference == TimbreSettings::REFERENCE_REGRESSION);
    top->addWidget(_spinRegression);
    top->addSpacing(12);

    // Partials
    QToolButton * buttonPartials = new QToolButton(main);
    buttonPartials->setText(tr("Partials"));
    buttonPartials->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuPartials = new QMenu(buttonPartials);
    QWidget * partials = new QWidget(menuPartials);
    QFormLayout * formPartials = new QFormLayout(partials);
    _spinDetailed = new QSpinBox(partials);
    _spinDetailed->setRange(4, 32);
    _spinDetailed->setValue(_settings.detailedRanks);
    formPartials->addRow(tr("Ranks displayed one by one"), _spinDetailed);
    _comboBands = new QComboBox(partials);
    _comboBands->addItem(tr("Bands of a third of an octave"));
    _comboBands->addItem(tr("Bands of an octave"));
    _comboBands->setCurrentIndex(_settings.octaveBands ? 1 : 0);
    formPartials->addRow(tr("Next ranks"), _comboBands);
    _spinUpperLimit = new QDoubleSpinBox(partials);
    _spinUpperLimit->setRange(0, 24);
    _spinUpperLimit->setDecimals(1);
    _spinUpperLimit->setSuffix(" " + tr("kHz", "unit for kilohertz"));
    _spinUpperLimit->setSpecialValueText(tr("Half of the sample rate"));
    _spinUpperLimit->setValue(_settings.upperLimit / 1000);
    formPartials->addRow(tr("Upper limit"), _spinUpperLimit);
    _spinDynamic = new QDoubleSpinBox(partials);
    _spinDynamic->setRange(20, 120);
    _spinDynamic->setDecimals(0);
    _spinDynamic->setSuffix(" " + tr("dB", "unit for decibels"));
    _spinDynamic->setValue(_settings.dynamicRange);
    _spinDynamic->setToolTip(tr("Ranks further below the strongest one of the key are ignored"));
    formPartials->addRow(tr("Dynamic range"), _spinDynamic);
    QWidgetAction * actionPartials = new QWidgetAction(menuPartials);
    actionPartials->setDefaultWidget(partials);
    menuPartials->addAction(actionPartials);
    buttonPartials->setMenu(menuPartials);
    top->addWidget(buttonPartials);

    // Thresholds
    QToolButton * buttonThresholds = new QToolButton(main);
    buttonThresholds->setText(tr("Thresholds"));
    buttonThresholds->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuThresholds = new QMenu(buttonThresholds);
    QWidget * thresholds = new QWidget(menuThresholds);
    QFormLayout * formThresholds = new QFormLayout(thresholds);
    auto addThreshold = [&](QDoubleSpinBox *& spin, QString label, QString tooltip, double value) {
        spin = new QDoubleSpinBox(thresholds);
        spin->setRange(0, 40);
        spin->setDecimals(1);
        spin->setSingleStep(0.5);
        spin->setSuffix(" " + tr("dB", "unit for decibels"));
        spin->setValue(value);
        spin->setToolTip(tooltip);
        formThresholds->addRow(label, spin);
    };
    addThreshold(_spinTolerance, tr("Tolerance"), tr("Timbre deviation up to which a root key is consistent"), _settings.tolerance);
    addThreshold(_spinMaximum, tr("Maximum"), tr("Timbre deviation up to which a root key can be corrected, beyond it is too different"),
                 _settings.maximum);
    addThreshold(_spinRedundancy, tr("Redundancy"), tr("Largest timbre deviation over the keys of a consistent root key if it were "
                                                       "removed, for being redundant"), _settings.redundancy);
    addThreshold(_spinJump, tr("Break"), tr("Timbre jump at a junction from which a computed note is proposed"), _settings.jump);
    addThreshold(_spinLevel, tr("Level"), tr("Level deviation from which a correction of the attenuation is proposed"), _settings.level);
    QPushButton * buttonDefault = new QPushButton(tr("Default values"), thresholds);
    formThresholds->addRow(buttonDefault);
    connect(buttonDefault, &QPushButton::clicked, this, [this]() {
        TimbreSettings defaults;
        _spinTolerance->setValue(defaults.tolerance);
        _spinMaximum->setValue(defaults.maximum);
        _spinRedundancy->setValue(defaults.redundancy);
        _spinJump->setValue(defaults.jump);
        _spinLevel->setValue(defaults.level);
    });
    QWidgetAction * actionThresholds = new QWidgetAction(menuThresholds);
    actionThresholds->setDefaultWidget(thresholds);
    menuThresholds->addAction(actionThresholds);
    buttonThresholds->setMenu(menuThresholds);
    top->addWidget(buttonThresholds);

    // Options
    _buttonOptions = new QToolButton(main);
    _buttonOptions->setText(tr("Options"));
    _buttonOptions->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuOptions = new QMenu(_buttonOptions);
    QMenu * menuRows = menuOptions->addMenu(tr("Summaries"));
    for (int summary = TimbreDisplay::SUMMARY_BRIGHTNESS; summary <= TimbreDisplay::SUMMARY_LOOP; summary++)
    {
        QAction * action = menuRows->addAction(TimbreMapWidget::getSummaryName(summary));
        action->setCheckable(true);
        action->setChecked(_pendingDisplay.summaries.contains(summary));
        action->setData(summary);
        connect(action, SIGNAL(toggled(bool)), this, SLOT(onDisplayChanged()));
    }
    auto addOption = [&](QString text, bool checked, QString name) -> QAction * {
        QAction * action = menuOptions->addAction(text);
        action->setCheckable(true);
        action->setChecked(checked);
        action->setObjectName(name);
        connect(action, SIGNAL(toggled(bool)), this, SLOT(onDisplayChanged()));
        return action;
    };
    addOption(tr("Root keys only"), _pendingDisplay.positionsOnly, "positionsOnly");
    addOption(tr("Whole keyboard"), _pendingDisplay.fullKeyboard, "fullKeyboard");
    addOption(tr("Colors by steps of the thresholds"), _pendingDisplay.steps, "steps");
    addOption(tr("Automatic range of the colors"), _pendingDisplay.autoRange, "autoRange");
    menuOptions->addSeparator();
    addOption(tr("Play the clicked keys"), _playClickedNotes, "playClicked");
    _buttonOptions->setMenu(menuOptions);
    top->addWidget(_buttonOptions);

    // Proposals
    QToolButton * buttonPropose = new QToolButton(main);
    buttonPropose->setText(tr("Propose"));
    buttonPropose->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuPropose = new QMenu(buttonPropose);
    QAction * action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_TILDE, palette.toCorrect()),
                                              tr("Correct the root keys marked \"to correct\"..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(TimbrePosition::LABEL_TO_CORRECT); });
    action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_CROSS, palette.tooDifferent()),
                                    tr("Replace the root keys marked \"too different\" with computed notes..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(TimbrePosition::LABEL_TOO_DIFFERENT); });
    action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_MINUS, palette.redundant()),
                                    tr("Remove the redundant root keys..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(TimbrePosition::LABEL_REDUNDANT); });
    action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_PLUS, palette.breakColor()),
                                    tr("Create computed notes at the breaks..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(100); });
    action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_UP, palette.levelColor()),
                                    tr("Correct the deviating levels..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(101); });
    buttonPropose->setMenu(menuPropose);
    top->addWidget(buttonPropose);
    top->addStretch();
    mainLayout->addLayout(top);

    // Map and detail
    QSplitter * splitter = new QSplitter(Qt::Vertical, main);
    _map = new TimbreMapWidget(splitter);
    _map->setDisplay(_pendingDisplay);
    _map->setModel(&_model);
    _map->setVelocity(_settings.velocity);
    splitter->addWidget(_map);
    QWidget * detail = new QWidget(splitter);
    QHBoxLayout * detailLayout = new QHBoxLayout(detail);
    detailLayout->setContentsMargins(0, 4, 0, 0);
    _bars = new TimbreBarsWidget(detail);
    detailLayout->addWidget(_bars, 3);
    QScrollArea * scrollDetail = new QScrollArea(detail);
    scrollDetail->setWidgetResizable(true);
    scrollDetail->setFrameShape(QFrame::NoFrame);
    _labelDetail = new QLabel(scrollDetail);
    _labelDetail->setWordWrap(true);
    _labelDetail->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    _labelDetail->setTextInteractionFlags(Qt::TextSelectableByMouse);
    scrollDetail->setWidget(_labelDetail);
    scrollDetail->setMinimumWidth(280);
    detailLayout->addWidget(scrollDetail, 2);
    QVBoxLayout * buttons = new QVBoxLayout();
    _buttonCreate = new QPushButton(tr("Create a note here..."), detail);
    _buttonRemove = new QPushButton(tr("Remove..."), detail);
    _buttonReplace = new QPushButton(tr("Replace with a computed note..."), detail);
    _buttonCorrect = new QPushButton(tr("Correct the timbre..."), detail);
    _buttonLevel = new QPushButton(tr("Correct the level"), detail);
    _buttonRender = new QPushButton(tr("Check by rendering"), detail);
    _buttonRender->setToolTip(tr("Play the key with the sound engine and measure the harmonics of the result"));
    buttons->addWidget(_buttonCreate);
    buttons->addWidget(_buttonRemove);
    buttons->addWidget(_buttonReplace);
    buttons->addWidget(_buttonCorrect);
    buttons->addWidget(_buttonLevel);
    buttons->addWidget(_buttonRender);
    buttons->addStretch();
    detailLayout->addLayout(buttons);
    splitter->addWidget(detail);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter, 1);

    // Connections
    connect(_spinVelocity, SIGNAL(valueChanged(int)), this, SLOT(onVelocityChanged(int)));
    connect(_velocityStrip, SIGNAL(velocityClicked(int)), _spinVelocity, SLOT(setValue(int)));
    connect(_comboMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onDisplayChanged()));
    connect(_comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_spinRegression, SIGNAL(valueChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_spinDetailed, SIGNAL(valueChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_comboBands, SIGNAL(currentIndexChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_spinUpperLimit, SIGNAL(valueChanged(double)), this, SLOT(onSettingsChanged()));
    connect(_spinDynamic, SIGNAL(valueChanged(double)), this, SLOT(onSettingsChanged()));
    foreach (QDoubleSpinBox * spin, QList<QDoubleSpinBox *>() << _spinTolerance << _spinMaximum << _spinRedundancy
             << _spinJump << _spinLevel)
        connect(spin, SIGNAL(valueChanged(double)), this, SLOT(onThresholdsChanged()));
    connect(_map, SIGNAL(keySelected(int,Qt::KeyboardModifiers)), this, SLOT(onKeySelected(int,Qt::KeyboardModifiers)));
    connect(_map, SIGNAL(rangeSelected(int,int,Qt::KeyboardModifiers)), this, SLOT(onRangeSelected(int,int,Qt::KeyboardModifiers)));
    connect(_map, SIGNAL(contextMenuRequested(int,QPoint)), this, SLOT(onContextMenu(int,QPoint)));
    connect(_buttonCreate, SIGNAL(clicked()), this, SLOT(onCreate()));
    connect(_buttonRemove, SIGNAL(clicked()), this, SLOT(onRemove()));
    connect(_buttonReplace, SIGNAL(clicked()), this, SLOT(onReplace()));
    connect(_buttonCorrect, SIGNAL(clicked()), this, SLOT(onCorrect()));
    connect(_buttonLevel, SIGNAL(clicked()), this, SLOT(onCorrectLevel()));
    connect(_buttonRender, SIGNAL(clicked()), this, SLOT(onRender()));
}

QString PageTimbre::keyName(int key)
{
    return ContextManager::keyName()->getKeyName(static_cast<unsigned int>(qBound(0, key, 127)));
}

QString PageTimbre::formatDb(double value, int decimals)
{
    if (!std::isfinite(value))
        return "-";
    return QLocale().toString(value, 'f', decimals) + " " + tr("dB", "unit for decibels");
}

void PageTimbre::updateInterface(QString editingSource)
{
    // Single instrument
    EltID idInst;
    bool several = false;
    foreach (EltID id, _currentIds)
    {
        if (id.typeElement != elementInst && id.typeElement != elementInstSmpl)
            continue;
        EltID other(elementInst, id.indexSf2, id.indexElt);
        if (idInst.typeElement == elementUnknown)
            idInst = other;
        else if (other != idInst)
            several = true;
    }
    if (idInst.typeElement == elementUnknown || several)
    {
        _idInst = EltID();
        _labelMessage->setText(tr("Select a single instrument\nto display its timbre"));
        _stack->setCurrentIndex(1);
        return;
    }

    bool sameInstrument = (idInst == _idInst);
    _idInst = idInst;
    if (!sameInstrument)
    {
        _selectedKey = -1;
        _selectedPositions.clear();
        _renderedKey = -1;
    }
    if (!sameInstrument || editingSource != "command:selection")
    {
        updateVelocityLayers();
        recompute();
    }
    readTreeSelection();
}

void PageTimbre::hideEvent(QHideEvent * event)
{
    saveSettings();
    Page::hideEvent(event);
}

void PageTimbre::keyPlayedInternal(int key, int velocity)
{
    _map->setPlayedKey(key, velocity > 0);
}

void PageTimbre::onAnalysisReady(int indexSf2)
{
    // Columns appear as the analyses arrive
    if (indexSf2 == _idInst.indexSf2 && this->isVisible() && !_recomputeTimer.isActive())
        _recomputeTimer.start();
}

void PageTimbre::recompute()
{
    _recomputeTimer.stop();
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (_idInst.typeElement != elementInst || !sm->isValid(_idInst))
        return;
    _settings.outputSampleRate = _synth != nullptr && _synth->getSampleRate() > 0 ? _synth->getSampleRate() : 48000;

    // With the solo on selection, the mute follows the selection: it doesn't exclude divisions anymore
    _settings.includeMuted = sm->solo()->isSoloOnSelectionEnabled(_idInst.indexSf2);
    _model.compute(_idInst, _settings, false);
    _map->setVelocity(_settings.velocity);
    _map->modelUpdated();

    _stack->setCurrentIndex(0); // Even without divisions: the velocity can be changed

    // The selection follows the root keys that still exist
    QList<int> positions;
    foreach (int key, _selectedPositions)
        if (_model.getPositions().contains(key))
            positions << key;
    _selectedPositions = positions;
    _map->setSelection(_selectedKey, _selectedPositions);
    updateDetail();
    updatePending();
}

void PageTimbre::updatePending()
{
    int pending = TimbreAnalysisCache::getInstance()->pendingCount();
    if (pending > 0 && !_model.isComplete())
    {
        _labelPending->setText(tr("Analysis of the samples: %n remaining", "", pending));
        if (_spinner->isHidden())
        {
            _spinner->show();
            _spinner->startAnimation();
        }
    }
    else
    {
        _labelPending->clear();
        _spinner->stopAnimation();
        _spinner->hide();
    }
}

void PageTimbre::updateVelocityLayers()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<QPair<int, int> > layers;
    EltID idDivision(elementInstSmpl, _idInst.indexSf2, _idInst.indexElt);
    RangesType defaultRange;
    defaultRange.byLo = 0;
    defaultRange.byHi = 127;
    if (sm->isSet(_idInst, champ_velRange))
        defaultRange = sm->get(_idInst, champ_velRange).rValue;
    foreach (int index, sm->getSiblings(idDivision))
    {
        idDivision.indexElt2 = index;
        RangesType range = sm->isSet(idDivision, champ_velRange) ? sm->get(idDivision, champ_velRange).rValue : defaultRange;
        QPair<int, int> layer(qMin(range.byLo, range.byHi), qMax(range.byLo, range.byHi));
        if (!layers.contains(layer))
            layers << layer;
    }
    std::sort(layers.begin(), layers.end());
    _velocityStrip->setLayers(layers);
}

void PageTimbre::onVelocityChanged(int velocity)
{
    _settings.velocity = velocity;
    _velocityStrip->setVelocity(velocity);
    recompute();
}

void PageTimbre::onSettingsChanged()
{
    _settings.reference = _comboReference->currentIndex() == 1 ? TimbreSettings::REFERENCE_REGRESSION :
                                                                 TimbreSettings::REFERENCE_INTERPOLATION;
    _spinRegression->setVisible(_settings.reference == TimbreSettings::REFERENCE_REGRESSION);
    _settings.regressionCount = _spinRegression->value();
    _settings.detailedRanks = _spinDetailed->value();
    _settings.octaveBands = (_comboBands->currentIndex() == 1);
    _settings.upperLimit = 1000 * _spinUpperLimit->value();
    _settings.dynamicRange = _spinDynamic->value();
    recompute();
}

void PageTimbre::onThresholdsChanged()
{
    _settings.tolerance = _spinTolerance->value();
    _settings.maximum = qMax(_spinMaximum->value(), _settings.tolerance);
    _settings.redundancy = _spinRedundancy->value();
    _settings.jump = _spinJump->value();
    _settings.level = _spinLevel->value();
    _model.applyThresholds(_settings);
    _map->modelUpdated();
    updateDetail();
}

void PageTimbre::onDisplayChanged()
{
    TimbreDisplay display = _map->getDisplay();
    display.mode = static_cast<TimbreDisplay::Mode>(_comboMode->currentIndex());
    QList<int> summaries;
    summaries << TimbreDisplay::SUMMARY_LEVEL << TimbreDisplay::SUMMARY_TIMBRE << TimbreDisplay::SUMMARY_TRANSPOSITION;
    foreach (QAction * action, _buttonOptions->menu()->actions())
    {
        if (action->menu() != nullptr)
        {
            foreach (QAction * subAction, action->menu()->actions())
                if (subAction->isChecked())
                    summaries << subAction->data().toInt();
        }
        else if (action->objectName() == "positionsOnly")
            display.positionsOnly = action->isChecked();
        else if (action->objectName() == "fullKeyboard")
            display.fullKeyboard = action->isChecked();
        else if (action->objectName() == "steps")
            display.steps = action->isChecked();
        else if (action->objectName() == "autoRange")
            display.autoRange = action->isChecked();
        else if (action->objectName() == "playClicked")
            _playClickedNotes = action->isChecked();
    }
    display.summaries = summaries;
    _map->setDisplay(display);
}

void PageTimbre::readTreeSelection()
{
    // Root keys of the divisions selected in the tree
    QList<int> positions;
    foreach (EltID id, _currentIds)
    {
        if (id.typeElement != elementInstSmpl || id.indexSf2 != _idInst.indexSf2 || id.indexElt != _idInst.indexElt)
            continue;
        foreach (TimbreDivision division, _model.getDivisions())
            if (division.index == id.indexElt2 && !positions.contains(division.rootKey))
                positions << division.rootKey;
    }
    _selectedPositions = positions;
    if (!positions.isEmpty() && !positions.contains(_selectedKey))
        _selectedKey = positions.first();
    _map->setSelection(_selectedKey, _selectedPositions);
    updateDetail();
}

void PageTimbre::selectPositions(const QList<int> &keys, bool notifyTree)
{
    _selectedPositions = keys;
    _map->setSelection(_selectedKey, _selectedPositions);
    if (notifyTree)
    {
        IdList ids;
        foreach (int key, keys)
        {
            if (!_model.getPositions().contains(key))
                continue;
            foreach (int index, _model.getPositions()[key].divisions)
                ids << EltID(elementInstSmpl, _idInst.indexSf2, _idInst.indexElt, _model.getDivisions()[index].index);
        }
        if (ids.isEmpty())
            ids << _idInst;
        emit selectedIdsChanged(ids);
    }
    updateDetail();
}

void PageTimbre::onKeySelected(int key, Qt::KeyboardModifiers modifiers)
{
    _selectedKey = key;
    bool isPosition = _model.getPositions().contains(key);
    if (modifiers & (Qt::ShiftModifier | Qt::ControlModifier))
    {
        // Several root keys
        QList<int> positions = _selectedPositions;
        if (isPosition)
        {
            if (positions.contains(key))
                positions.removeAll(key);
            else
                positions << key;
        }
        selectPositions(positions, true);
    }
    else if (isPosition)
        selectPositions(QList<int>() << key, true);
    else
    {
        _map->setSelection(_selectedKey, _selectedPositions);
        updateDetail();
    }

    // Possibly play the key during a second
    if (_playClickedNotes && !_model.getKey(key).sources.isEmpty())
    {
        if (_clickedNote >= 0)
            ContextManager::midi()->processKeyOff(-1, _clickedNote);
        _clickedNote = key;
        ContextManager::midi()->processKeyOn(-1, key, _settings.velocity);
        QTimer::singleShot(1000, this, [this, key]() {
            if (_clickedNote == key)
            {
                ContextManager::midi()->processKeyOff(-1, key);
                _clickedNote = -1;
            }
        });
    }
}

void PageTimbre::onRangeSelected(int firstKey, int lastKey, Qt::KeyboardModifiers modifiers)
{
    QList<int> positions;
    if (modifiers & (Qt::ShiftModifier | Qt::ControlModifier))
        positions = _selectedPositions;
    foreach (int key, _model.getPositions().keys())
        if (key >= firstKey && key <= lastKey && !positions.contains(key))
            positions << key;
    if (!positions.isEmpty())
        _selectedKey = positions.first();
    selectPositions(positions, true);
}

QList<int> PageTimbre::getTargets()
{
    // The selected root keys if the selected key is one of them or is played by one of them, otherwise the root key
    // selected
    QList<int> targets;
    int playedBy = _selectedKey >= 0 ? _model.getKey(_selectedKey).position : -1;
    if (_selectedPositions.contains(_selectedKey) ||
        (!_model.getPositions().contains(_selectedKey) && _selectedPositions.contains(playedBy)))
        targets = _selectedPositions;
    else if (_model.getPositions().contains(_selectedKey))
        targets << _selectedKey;
    std::sort(targets.begin(), targets.end());
    return targets;
}

void PageTimbre::updateDetail()
{
    TimbrePalette palette;
    int key = _selectedKey;
    bool hasPosition = _model.getPositions().contains(key);
    bool covered = key >= 0 && !_model.getKey(key).sources.isEmpty();
    bool canCreate = key >= 0 && !hasPosition && _model.getPositions().count() >= 2;
    QList<int> targets = getTargets();
    _buttonCreate->setEnabled(canCreate);
    _buttonRemove->setEnabled(!targets.isEmpty());
    _buttonReplace->setEnabled(!targets.isEmpty() && _model.getPositions().count() >= 3);
    _buttonCorrect->setEnabled(!targets.isEmpty() && _model.getPositions().count() >= 2);
    _buttonLevel->setEnabled(!targets.isEmpty() && _model.getPositions().count() >= 2);
    _buttonRender->setEnabled(covered);
    _map->setGhost(canCreate ? key : -1);

    if (_model.getDivisions().isEmpty())
    {
        _bars->clear();
        _labelDetail->setText(tr("No division of this instrument plays at the velocity %1.").arg(_settings.velocity));
        return;
    }
    if (key < 0)
    {
        _bars->clear();
        _labelDetail->setText(tr("Click on a key to see what it plays."));
        return;
    }

    QStringList lines;
    QLocale locale;
    SoundfontManager * sm = SoundfontManager::getInstance();
    const TimbreKey &tk = _model.getKey(key);
    lines << "<b>" + tr("Key %1").arg(keyName(key)) + "</b>";
    QList<TimbreBarsWidget::Series> series;

    if (covered)
    {
        // Sources
        foreach (TimbreVoice voice, tk.voices)
        {
            QString name = sm->getQstr(EltID(elementSmpl, _idInst.indexSf2, voice.sampleIndex), champ_name);
            lines << tr("%1: transposition %2 semitones, attenuation %3, filter %4 Hz")
                     .arg(name.toHtmlEscaped(), locale.toString(voice.transposition / 100, 'f', 2), formatDb(voice.attenuation),
                          locale.toString(qRound(voice.filterFrequency)));
        }
        lines << tr("Duration factor: %1 (attack, tremolo and beats)").arg(locale.toString(tk.durationFactor, 'f', 3));
        if (std::isfinite(tk.tuning))
            lines << tr("Pitch: %1 cents from the closest note of the equal temperament").arg(
                         locale.toString(tk.tuning - 100 * std::round(tk.tuning / 100), 'f', 1));
    }
    else
        lines << tr("No sample");

    int positionKey = hasPosition ? key : (covered ? tk.position : -1);
    if (_model.getPositions().contains(positionKey))
    {
        const TimbrePosition &position = _model.getPositions()[positionKey];
        if (!hasPosition)
            lines << tr("Played by the root key %1").arg(keyName(positionKey));
        if (position.deviation.valid)
        {
            QString label;
            switch (position.label)
            {
            case TimbrePosition::LABEL_CONFORM: label = tr("consistent"); break;
            case TimbrePosition::LABEL_REDUNDANT: label = tr("redundant: can be removed"); break;
            case TimbrePosition::LABEL_TO_CORRECT: label = tr("to correct"); break;
            case TimbrePosition::LABEL_TOO_DIFFERENT: label = tr("too different: remove it or replace it with a computed note"); break;
            default: break;
            }
            lines << tr("Root key %1: %2").arg(keyName(positionKey), label);
            lines << tr("Deviation from the reference: level %1, timbre %2 (confidence %3%)")
                     .arg(formatDb(position.deviation.levelDifference), formatDb(position.deviation.timbreDifference))
                     .arg(qRound(100 * position.deviation.confidence));
            if (position.referenceAtEdge)
                lines << "<i>" + tr("Reference extrapolated from the neighbors on one side: less reliable") + "</i>";
            if (position.redundancyComputed)
            {
                QString redundancy = tr("If removed: largest timbre deviation %1, level %2").arg(
                            formatDb(position.redundancy), formatDb(position.redundancyLevel));
                if (position.newJunction.valid)
                    redundancy += ", " + tr("new junction %1").arg(formatDb(position.newJunction.timbreDifference));
                lines << redundancy;
            }
        }
        else if (!position.analyzed)
            lines << tr("Analysis in progress...");

        // Bars of the root key
        if (hasPosition)
        {
            series << TimbreBarsWidget::Series { tr("Heard"), _model.getRowLevels(position.spectrum), palette.highlight(),
                                                 TimbreBarsWidget::STYLE_BARS };
            if (!position.reference.isEmpty())
                series << TimbreBarsWidget::Series { tr("Reference"), _model.getRowLevels(position.reference), palette.text(),
                                                     TimbreBarsWidget::STYLE_MARKERS };
        }
    }
    if (!hasPosition && covered)
        series << TimbreBarsWidget::Series { tr("Heard"), _model.getRowLevels(tk.spectrum), palette.highlight(),
                                             TimbreBarsWidget::STYLE_BARS };
    if (canCreate)
    {
        bool atEdge = false;
        TimbreSpectrum predicted = _model.predict(key, -1, &atEdge);
        series << TimbreBarsWidget::Series { tr("Computed note"), _model.getRowLevels(predicted), palette.breakColor(),
                                             TimbreBarsWidget::STYLE_OUTLINE };
        lines << tr("A computed note can be created here (dashed column).");
    }

    // Jumps at the junctions around the key
    if (covered && tk.junction && tk.jump.valid)
        lines << tr("Junction with the previous key: level %1, timbre %2").arg(
                     formatDb(tk.jump.levelDifference), formatDb(tk.jump.timbreDifference)) +
                 (tk.breakLabel ? " — " + tr("break") : "");
    if (covered && key < 127 && _model.getKey(key + 1).junction && _model.getKey(key + 1).jump.valid)
        lines << tr("Junction with the next key: level %1, timbre %2").arg(
                     formatDb(_model.getKey(key + 1).jump.levelDifference),
                     formatDb(_model.getKey(key + 1).jump.timbreDifference)) +
                 (_model.getKey(key + 1).breakLabel ? " — " + tr("break") : "");

    // Rendering
    if (_renderedKey == key && _renderedVelocity == _settings.velocity && !_renderedSpectrum.isEmpty())
    {
        series << TimbreBarsWidget::Series { tr("Rendered"), _model.getRowLevels(_renderedSpectrum), palette.toCorrect(),
                                             TimbreBarsWidget::STYLE_MARKERS };
        TimbreComparison comparison = _model.compare(_renderedSpectrum, tk.spectrum);
        if (comparison.valid)
        {
            double largest = 0;
            for (int row = 0; row < comparison.rowDifferences.size(); row++)
                if (!std::isnan(comparison.rowDifferences[row]))
                    largest = qMax(largest, std::fabs(comparison.rowDifferences[row] - comparison.levelDifference));
            lines << tr("Rendering against the model: level %1, timbre %2, largest deviation of a row %3").arg(
                         formatDb(comparison.levelDifference), formatDb(comparison.timbreDifference), formatDb(largest));
        }
    }

    if (targets.count() > 1)
        lines << "<i>" + tr("%n root keys selected: the actions apply to all of them.", "", static_cast<int>(targets.count())) + "</i>";
    _labelDetail->setText(lines.join("<br/>"));
    _bars->setData(_model.getRows(), series, _settings.dynamicRange);
}

void PageTimbre::onContextMenu(int key, QPoint position)
{
    if (_selectedKey != key)
        onKeySelected(key, Qt::NoModifier);
    QMenu menu(this);
    if (_buttonCreate->isEnabled())
        menu.addAction(_buttonCreate->text(), this, SLOT(onCreate()));
    if (_buttonRemove->isEnabled())
        menu.addAction(_buttonRemove->text(), this, SLOT(onRemove()));
    if (_buttonReplace->isEnabled())
        menu.addAction(_buttonReplace->text(), this, SLOT(onReplace()));
    if (_buttonCorrect->isEnabled())
        menu.addAction(_buttonCorrect->text(), this, SLOT(onCorrect()));
    if (_buttonLevel->isEnabled())
        menu.addAction(_buttonLevel->text(), this, SLOT(onCorrectLevel()));
    if (!getTargets().isEmpty())
    {
        menu.addSeparator();
        menu.addAction(tr("Mute these divisions"), this, SLOT(onMute()));
    }
    if (_buttonRender->isEnabled())
    {
        menu.addSeparator();
        menu.addAction(_buttonRender->text(), this, SLOT(onRender()));
    }
    if (!menu.isEmpty())
        menu.exec(position);
}

void PageTimbre::runInThread(std::function<void()> function, QString text)
{
    QProgressDialog progress(text, QString(), 0, 0, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(400);
    progress.setCancelButton(nullptr);
    QEventLoop loop;
    QFutureWatcher<void> watcher;
    connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));
    watcher.setFuture(QtConcurrent::run(function));
    if (!watcher.isFinished())
        loop.exec(QEventLoop::ExcludeUserInputEvents);
}

bool PageTimbre::createNote(int key, int closestKey, int secondKey, int excludedPosition, QStringList &warnings)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    TimbreCreation creation;
    QString error;
    if (!TimbreActions::prepareCreation(sm, _idInst, _settings, key, closestKey, secondKey, excludedPosition, creation, error))
    {
        warnings << error;
        return false;
    }
    runInThread([&creation]() { TimbreActions::computeCreation(creation); },
                tr("Computing the note %1...").arg(keyName(key)));
    if (excludedPosition >= 0)
        TimbreActions::removePosition(sm, _model, excludedPosition, true);
    return !TimbreActions::applyCreation(sm, creation, warnings).isEmpty();
}

void PageTimbre::onCreate()
{
    int key = _selectedKey;
    if (key < 0 || _model.getPositions().contains(key) || _model.getPositions().count() < 2)
        return;
    DialogTimbreCreation dialog(_model, key, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    QStringList warnings;
    bool created = createNote(key, dialog.getClosestKey(), dialog.getSecondKey(), -1, warnings);
    SoundfontManager::getInstance()->endEditing(_editingSource);
    recompute(); // The page is not updated by its own editions
    if (!warnings.isEmpty())
        QMessageBox::warning(this, tr("Warning"), warnings.join("<br/>"));
    if (created)
    {
        _selectedKey = key;
        selectPositions(QList<int>() << key, true);
    }
}

static QString keyList(const QList<int> &keys, std::function<QString(int)> name)
{
    QStringList names;
    foreach (int key, keys)
        names << name(key);
    if (names.count() > 24)
        names = names.mid(0, 24) << "...";
    return names.join(", ");
}

void PageTimbre::onRemove()
{
    QList<int> targets = getTargets();
    if (targets.isEmpty())
        return;

    // Confirmation, with the consequences for a single root key
    QString text;
    if (targets.count() == 1)
    {
        const TimbrePosition &position = _model.getPositions()[targets[0]];
        text = tr("Remove the root key %1 (%n division(s))? The neighbors will play its keys.", "",
                  static_cast<int>(position.divisions.count())).arg(keyName(targets[0]));
        if (position.redundancyComputed)
            text += "<br/>" + tr("Largest timbre deviation over its keys: %1, level: %2.").arg(
                        formatDb(position.redundancy), formatDb(position.redundancyLevel));
        if (position.newJunction.valid)
            text += "<br/>" + tr("Timbre jump at the new junction: %1.").arg(formatDb(position.newJunction.timbreDifference));
    }
    else
        text = tr("Remove the following root keys? The neighbors will play their keys.") + "<br/>" +
               keyList(targets, [this](int k) { return keyName(k); });
    QMessageBox box(QMessageBox::Question, tr("Remove"), text, QMessageBox::Yes | QMessageBox::Cancel, this);
    QCheckBox * check = new QCheckBox(tr("Also remove the samples that are not used anymore"), &box);
    check->setChecked(true);
    box.setCheckBox(check);
    box.setDefaultButton(QMessageBox::Yes);
    if (box.exec() != QMessageBox::Yes)
        return;
    runSeries(SERIES_REMOVE, targets, TimbreCorrectionParameters(), check->isChecked(), false);
}

void PageTimbre::onReplace()
{
    QList<int> targets = getTargets();
    if (targets.isEmpty())
        return;
    QString text = targets.count() == 1 ?
                tr("Replace the root key %1 with a note computed from its neighbors?").arg(keyName(targets[0])) :
                tr("Replace the following root keys with notes computed from their neighbors?") + "<br/>" +
                keyList(targets, [this](int k) { return keyName(k); });
    if (QMessageBox::question(this, tr("Replace"), text, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes) !=
        QMessageBox::Yes)
        return;
    runSeries(SERIES_REPLACE, targets, TimbreCorrectionParameters(), true, false);
}

void PageTimbre::onCorrectLevel()
{
    QList<int> targets = getTargets();
    if (targets.isEmpty())
        return;
    if (targets.count() > 1 &&
        QMessageBox::question(this, tr("Correct the level"),
                              tr("Correct the level of the following root keys?") + "<br/>" +
                              keyList(targets, [this](int k) { return keyName(k); }),
                              QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes) != QMessageBox::Yes)
        return;
    runSeries(SERIES_LEVEL, targets, TimbreCorrectionParameters(), false, false);
}

bool PageTimbre::askSharedSamples(const QList<int> &targets, bool &duplicate)
{
    duplicate = true;
    SoundfontManager * sm = SoundfontManager::getInstance();
    QStringList names;
    foreach (int key, targets)
    {
        if (!_model.getPositions().contains(key))
            continue;
        QList<int> divisions, samples;
        foreach (int index, _model.getPositions()[key].divisions)
        {
            divisions << _model.getDivisions()[index].index;
            samples << _model.getDivisions()[index].sampleIndex;
        }
        foreach (int sampleIndex, TimbreActions::getSharedSamples(sm, _idInst, divisions, samples))
            names << sm->getQstr(EltID(elementSmpl, _idInst.indexSf2, sampleIndex), champ_name).toHtmlEscaped();
    }
    if (names.isEmpty())
        return true;
    if (TimbreActions::s_sharedChoice != TimbreActions::SHARED_ASK)
    {
        duplicate = (TimbreActions::s_sharedChoice == TimbreActions::SHARED_DUPLICATE);
        return true;
    }

    QMessageBox box(QMessageBox::Question, tr("Shared samples"),
                    tr("The following samples are also used by other divisions or instruments:") + "<br/>" +
                    names.mid(0, 20).join(", ") + (names.count() > 20 ? ", ..." : "") + "<br/><br/>" +
                    tr("Duplicate them for the corrected root keys, or modify them everywhere?"), QMessageBox::NoButton, this);
    QPushButton * buttonDuplicate = box.addButton(tr("Duplicate"), QMessageBox::AcceptRole);
    QPushButton * buttonModify = box.addButton(tr("Modify everywhere"), QMessageBox::DestructiveRole);
    box.addButton(QMessageBox::Cancel);
    box.setDefaultButton(buttonDuplicate);
    QCheckBox * check = new QCheckBox(tr("Don't ask again (until Polyphone is closed)"), &box);
    box.setCheckBox(check);
    box.exec();
    if (box.clickedButton() != buttonDuplicate && box.clickedButton() != buttonModify)
        return false;
    duplicate = (box.clickedButton() == buttonDuplicate);
    if (check->isChecked())
        TimbreActions::s_sharedChoice = duplicate ? TimbreActions::SHARED_DUPLICATE : TimbreActions::SHARED_MODIFY_ALL;
    return true;
}

void PageTimbre::onCorrect()
{
    QList<int> targets = getTargets();
    if (targets.isEmpty())
        return;
    DialogTimbreCorrection dialog(&_model, targets, _settings, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    bool duplicate;
    if (!askSharedSamples(targets, duplicate))
        return;
    runSeries(SERIES_CORRECT, targets, dialog.getParameters(), false, duplicate);
}

void PageTimbre::onMute()
{
    QList<int> targets = getTargets();
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int key, targets)
    {
        if (!_model.getPositions().contains(key))
            continue;
        foreach (int index, _model.getPositions()[key].divisions)
            sm->solo()->setMute(EltID(elementInstSmpl, _idInst.indexSf2, _idInst.indexElt, _model.getDivisions()[index].index), true);
    }
    emit sm->editingDone("command:mute", QList<int>() << _idInst.indexSf2);
    recompute();
}

void PageTimbre::onRender()
{
    int key = _selectedKey;
    if (key < 0 || _model.getKey(key).sources.isEmpty())
        return;
    TimbreSpectrum result;
    QString error;
    EltID idInst = _idInst;
    int velocity = _settings.velocity;
    double fundamental = 0;
    foreach (TimbreVoice voice, _model.getKey(key).voices)
        if (voice.fundamental > 0)
            fundamental = voice.fundamental;
    runInThread([&]() { result = TimbreRendering::render(idInst, key, velocity, fundamental, error); },
                tr("Rendering of the key %1...").arg(keyName(key)));
    if (result.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), error.isEmpty() ? tr("The rendering couldn't be analyzed.") : error);
        return;
    }
    _renderedKey = key;
    _renderedVelocity = velocity;
    _renderedSpectrum = result;
    updateDetail();
}

int PageTimbre::getBreakKey(int junctionKey)
{
    // Free key in the middle of the root keys on both sides of the junction
    int low = qMin(_model.getKey(junctionKey - 1).position, _model.getKey(junctionKey).position);
    int high = qMax(_model.getKey(junctionKey - 1).position, _model.getKey(junctionKey).position);
    int best = -1;
    for (int key = low + 1; key < high; key++)
        if (!_model.getPositions().contains(key) &&
            (best == -1 || std::abs(2 * key - low - high) < std::abs(2 * best - low - high)))
            best = key;
    return best;
}

void PageTimbre::onPropose(int label)
{
    // Preselection of the root keys (or junctions) having the label
    QList<int> targets;
    if (label == 100)
    {
        for (int key = 1; key < 128; key++)
            if (_model.getKey(key).breakLabel)
                targets << key;
    }
    else
    {
        foreach (TimbrePosition position, _model.getPositions().values())
            if ((label == 101 && position.levelLabel) || (label < 100 && position.label == label))
                targets << position.key;
    }
    if (targets.isEmpty())
    {
        QMessageBox::information(this, tr("Propose"), tr("Nothing to propose with the current thresholds."));
        return;
    }
    if (label != 100)
    {
        _selectedKey = targets.first();
        selectPositions(targets, true);
    }

    _seriesFromProposal = true;
    switch (label)
    {
    case TimbrePosition::LABEL_TO_CORRECT:
        onCorrect();
        break;
    case TimbrePosition::LABEL_TOO_DIFFERENT:
        onReplace();
        break;
    case TimbrePosition::LABEL_REDUNDANT:
        onRemove();
        break;
    case 100: {
        QStringList names;
        foreach (int key, targets)
        {
            int breakKey = getBreakKey(key);
            if (breakKey >= 0)
                names << keyName(breakKey);
        }
        if (QMessageBox::question(this, tr("Create computed notes"),
                                  tr("Create computed notes at the following keys?") + "<br/>" + names.join(", "),
                                  QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes)
            runSeries(SERIES_CREATE, targets, TimbreCorrectionParameters(), false, false);
    } break;
    case 101:
        if (QMessageBox::question(this, tr("Correct the level"),
                                  tr("Correct the level of the following root keys?") + "<br/>" +
                                  keyList(targets, [this](int k) { return keyName(k); }),
                                  QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes)
            runSeries(SERIES_LEVEL, targets, TimbreCorrectionParameters(), false, false);
        break;
    default:
        break;
    }
    _seriesFromProposal = false;
}

void PageTimbre::runSeries(SeriesType type, QList<int> targets, const TimbreCorrectionParameters &parameters,
                           bool removeUnusedSamples, bool duplicateShared)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QStringList warnings, skipped;
    QProgressDialog progress(tr("Processing..."), tr("Cancel"), 0, static_cast<int>(targets.count()), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(targets.count() > 1 ? 0 : 400);
    int done = 0;
    bool canceled = false;

    while (!targets.isEmpty())
    {
        // The references change after each action (a defective root key also distorts the reference of its
        // neighbors): the worst remaining target is processed first
        if (done > 0)
            _model.compute(_idInst, _settings, true);
        int best = -1;
        double bestScore = 0;
        foreach (int target, targets)
        {
            double score = -1;
            if (type == SERIES_CREATE)
            {
                if (target > 0 && _model.getKey(target).breakLabel && getBreakKey(target) >= 0)
                    score = _model.getKey(target).jump.timbreDifference;
            }
            else if (_model.getPositions().contains(target))
            {
                const TimbrePosition &position = _model.getPositions()[target];
                // Targets proposed for a label: processed only if they still have it
                switch (type)
                {
                case SERIES_CORRECT: case SERIES_REPLACE:
                    if (position.deviation.valid && (!_seriesFromProposal || position.label ==
                        (type == SERIES_CORRECT ? TimbrePosition::LABEL_TO_CORRECT : TimbrePosition::LABEL_TOO_DIFFERENT)))
                        score = position.deviation.timbreDifference;
                    break;
                case SERIES_LEVEL:
                    if (position.deviation.valid && (!_seriesFromProposal || position.levelLabel))
                        score = std::fabs(position.deviation.levelDifference);
                    break;
                case SERIES_REMOVE:
                    // Most redundant first
                    if (position.redundancyComputed && (!_seriesFromProposal || position.label == TimbrePosition::LABEL_REDUNDANT))
                        score = 1000 - position.redundancy;
                    break;
                default:
                    break;
                }
            }
            if (score >= 0 && (best == -1 || score > bestScore))
            {
                best = target;
                bestScore = score;
            }
        }
        if (best == -1)
        {
            foreach (int target, targets)
                skipped << keyName(target);
            break;
        }
        targets.removeAll(best);
        progress.setLabelText(tr("Processing the key %1...").arg(keyName(best)));
        QCoreApplication::processEvents();

        switch (type)
        {
        case SERIES_REMOVE:
            TimbreActions::removePosition(sm, _model, best, removeUnusedSamples);
            break;
        case SERIES_REPLACE:
            createNote(best, -1, -1, best, warnings);
            break;
        case SERIES_CREATE:
            createNote(getBreakKey(best), -1, -1, -1, warnings);
            break;
        case SERIES_LEVEL: {
            const TimbrePosition &position = _model.getPositions()[best];
            QList<int> divisions;
            foreach (int index, position.divisions)
                divisions << _model.getDivisions()[index].index;
            double delta = position.deviation.levelDifference;
            double applied = TimbreActions::changeAttenuation(sm, _idInst, divisions, delta);
            if (applied < std::fabs(delta) - 0.05)
                warnings << tr("%1: the attenuation reached its limit (%2 missing).").arg(
                                keyName(best), formatDb(std::fabs(delta) - applied));
        } break;
        case SERIES_CORRECT: {
            TimbreCorrectionPlan plan = TimbreActions::planCorrection(_model, best, parameters);
            if (!plan.valid)
            {
                skipped << keyName(best);
                break;
            }
            QList<TimbreCorrectedSample> samples;
            double compensation = 0;
            int indexSf2 = _idInst.indexSf2;
            runInThread([&]() { samples = TimbreActions::computeCorrection(indexSf2, plan, compensation); },
                        tr("Correction of the key %1...").arg(keyName(best)));
            QStringList correctionWarnings;
            TimbreActions::applyCorrection(sm, plan, samples, compensation, duplicateShared, correctionWarnings);
            foreach (QString warning, correctionWarnings)
                warnings << keyName(best) + tr(": ", "separator") + warning;
        } break;
        }

        progress.setValue(++done);
        if (progress.wasCanceled())
        {
            canceled = true;
            break;
        }
    }

    if (canceled)
        sm->revertNewEditing();
    else
        sm->endEditing(_editingSource);
    recompute(); // The page is not updated by its own editions
    if (!canceled && (!warnings.isEmpty() || !skipped.isEmpty()))
    {
        QStringList parts;
        if (!skipped.isEmpty())
            parts << tr("Not processed (the label changed or nothing to do):") + " " + skipped.join(", ");
        parts << warnings;
        QMessageBox::warning(this, tr("Warning"), parts.join("<br/>"));
    }
}
