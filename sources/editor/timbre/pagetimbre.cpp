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
#include "timbredetailwidget.h"
#include "dialogtimbrecorrection.h"
#include "dialogtimbrecreation.h"
#include "timbrerendering.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "rotatingspinner.h"
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QStackedWidget>
#include <QMenu>
#include <QMessageBox>
#include <QCheckBox>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QEventLoop>
#include <cmath>

PageTimbre::PageTimbre(QWidget * parent) : Page(parent, "page:timbre"),
    _selectedKey(-1),
    _anchorKey(-1),
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
    _settings.reference = static_cast<TimbreSettings::Reference>(
        conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_reference", 0).toInt() == 1 ? 1 : 0);
    _settings.regressionCount = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_regression_count", 3).toInt();
    _settings.tolerance = qBound(0.5, conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_tolerance", _settings.tolerance).toDouble(), 20.);

    TimbreDisplay display;
    int mode = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_mode", TimbreDisplay::MODE_DEVIATION).toInt();
    if (mode == 0) // Former "Absolute levels"
        mode = TimbreDisplay::MODE_LEVELS;
    display.mode = static_cast<TimbreDisplay::Mode>(
        mode >= TimbreDisplay::MODE_LEVELS && mode <= TimbreDisplay::MODE_TRANSPOSITION ? mode : TimbreDisplay::MODE_DEVIATION);
    QList<int> summaries;
    summaries << TimbreDisplay::SUMMARY_LEVEL << TimbreDisplay::SUMMARY_TIMBRE << TimbreDisplay::SUMMARY_TRANSPOSITION;
    QStringList extra = conf->getValue(ConfManager::SECTION_DISPLAY, "timbre_summaries", "").toString().split(",", Qt::SkipEmptyParts);
    foreach (QString value, extra)
    {
        int summary = value.toInt();
        if (summary >= TimbreDisplay::SUMMARY_BRIGHTNESS && summary <= TimbreDisplay::SUMMARY_TUNING && !summaries.contains(summary))
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
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_reference", static_cast<int>(_settings.reference));
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_regression_count", _settings.regressionCount);
    conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_tolerance", _settings.tolerance);
    if (_map != nullptr)
    {
        const TimbreDisplay &display = _map->getDisplay();
        conf->setValue(ConfManager::SECTION_DISPLAY, "timbre_mode", static_cast<int>(display.mode));
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
    QVBoxLayout * pageLayout = new QVBoxLayout(main);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);
    QWidget * upper = new QWidget(main); // Bars with margins, map on the whole width
    QVBoxLayout * mainLayout = new QVBoxLayout(upper);
    mainLayout->setContentsMargins(0, 6, 0, 0);
    pageLayout->addWidget(upper, 1);
    _stack->addWidget(main);
    _labelMessage = new QLabel(_stack);
    _labelMessage->setAlignment(Qt::AlignCenter);
    QFont font = _labelMessage->font();
    font.setBold(true);
    _labelMessage->setFont(font);
    _stack->addWidget(_labelMessage);

    // Top bar
    QHBoxLayout * top = new QHBoxLayout();
    top->setContentsMargins(6, 0, 6, 0);
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
    for (int mode = TimbreDisplay::MODE_LEVELS; mode <= TimbreDisplay::MODE_TRANSPOSITION; mode++)
        _comboMode->addItem(TimbreMapWidget::getModeName(mode), mode);
    _comboMode->setCurrentIndex(_comboMode->findData(static_cast<int>(_pendingDisplay.mode)));
    top->addWidget(_comboMode);
    top->addSpacing(6);
    _legend = new TimbreLegendWidget(main);
    top->addWidget(_legend);
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
    top->setContentsMargins(6, 0, 6, 0);
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

    // Tolerance (the other timbre thresholds following it)
    _labelTolerance = new QLabel(tr("Tolerance"), main);
    top->addWidget(_labelTolerance);
    _spinTolerance = new QDoubleSpinBox(main);
    _spinTolerance->setRange(0.5, 20);
    _spinTolerance->setDecimals(1);
    _spinTolerance->setSingleStep(0.5);
    _spinTolerance->setSuffix(" " + tr("dB", "unit for decibels"));
    _spinTolerance->setValue(_settings.tolerance);
    top->addWidget(_spinTolerance);
    updateToleranceToolTip();
    top->addSpacing(12);

    // Options
    _buttonOptions = new QToolButton(main);
    _buttonOptions->setText(tr("Options"));
    _buttonOptions->setPopupMode(QToolButton::InstantPopup);
    QMenu * menuOptions = new QMenu(_buttonOptions);
    QMenu * menuRows = menuOptions->addMenu(tr("Summaries"));
    for (int summary = TimbreDisplay::SUMMARY_BRIGHTNESS; summary <= TimbreDisplay::SUMMARY_TUNING; summary++)
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
    action = menuPropose->addAction(shapeIcon(TimbrePalette::SHAPE_UP, palette.levelColor(true)),
                                    tr("Correct the deviating levels..."));
    connect(action, &QAction::triggered, this, [this]() { onPropose(101); });
    buttonPropose->setMenu(menuPropose);
    top->addWidget(buttonPropose);
    top->addStretch();
    mainLayout->addLayout(top);

    // Map
    _map = new TimbreMapWidget(main);
    _map->setDisplay(_pendingDisplay);
    _map->setModel(&_model);
    _map->setVelocity(_settings.velocity);
    mainLayout->addWidget(_map, 1);
    updateLegend();

    // Detail, with a fixed height: the one of the buttons (graph and details without margins, only the buttons have some)
    QWidget * detail = new QWidget(main);
    detail->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout * detailLayout = new QHBoxLayout(detail);
    detailLayout->setContentsMargins(0, 0, 0, 0);
    _stackDetail = new QStackedWidget(detail);
    _stackDetail->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    detailLayout->addWidget(_stackDetail, 1);
    QWidget * detailContent = new QWidget(_stackDetail);
    QHBoxLayout * contentLayout = new QHBoxLayout(detailContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    _bars = new TimbreBarsWidget(detailContent);
    _bars->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    contentLayout->addWidget(_bars, 1);
    // The details take the width of their content first (scroll bar if the window is too narrow), the rest goes to the
    // graph: its preferred width is its minimum one
    _detail = new TimbreDetailWidget(detailContent);
    _detail->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    _detail->setMinimumWidth(280);
    contentLayout->addWidget(_detail);
    _stackDetail->addWidget(detailContent);
    _labelNoSelection = new QLabel(tr("Click on a key to see what it plays."), _stackDetail);
    _labelNoSelection->setAlignment(Qt::AlignCenter);
    _labelNoSelection->setWordWrap(true);
    _labelNoSelection->setStyleSheet("QLabel{color:" + ContextManager::theme()->getColor(
                                         ThemeManager::WINDOW_TEXT, ThemeManager::DISABLED).name() + "}");
    _labelNoSelection->setFont(font); // Bold
    _stackDetail->addWidget(_labelNoSelection);
    QVBoxLayout * buttons = new QVBoxLayout();
    buttons->setContentsMargins(0, 6, 6, 6);
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
    detailLayout->addLayout(buttons);
    pageLayout->addWidget(detail);

    // Connections
    connect(_spinVelocity, SIGNAL(valueChanged(int)), this, SLOT(onVelocityChanged(int)));
    connect(_velocityStrip, SIGNAL(velocityClicked(int)), _spinVelocity, SLOT(setValue(int)));
    connect(_comboMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onDisplayChanged()));
    connect(_comboReference, SIGNAL(currentIndexChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_spinRegression, SIGNAL(valueChanged(int)), this, SLOT(onSettingsChanged()));
    connect(_spinTolerance, SIGNAL(valueChanged(double)), this, SLOT(onThresholdsChanged()));
    connect(_map, SIGNAL(keySelected(int,Qt::KeyboardModifiers)), this, SLOT(onKeySelected(int,Qt::KeyboardModifiers)));
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

// Number with an optional unit, "-" if not measured
static QString formatValue(double value, int decimals, QString unit = QString(), bool sign = false)
{
    if (!std::isfinite(value))
        return "-";
    if (std::round(value * std::pow(10.0, decimals)) == 0)
        value = 0; // Neither "-0.0" nor "+0.0"
    QLocale locale;
    QString text = locale.toString(value, 'f', decimals);
    if (sign && value > 0)
        text.prepend(locale.positiveSign());
    return unit.isEmpty() ? text : text + " " + unit;
}

QString PageTimbre::formatDb(double value, int decimals, bool sign)
{
    return formatValue(value, decimals, tr("dB", "unit for decibels"), sign);
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
        _anchorKey = -1;
        _selectedPositions.clear();
        _renderedKey = -1;
        _detail->resetWidth();
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
    _model.compute(_idInst, _settings, false);
    _map->setVelocity(_settings.velocity);
    _map->modelUpdated();
    updateLegend();

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
    recompute();
}

void PageTimbre::onThresholdsChanged()
{
    _settings.tolerance = _spinTolerance->value();
    updateToleranceToolTip();
    _model.applyThresholds(_settings);
    _map->modelUpdated();
    updateLegend();
    updateDetail();
}

void PageTimbre::updateToleranceToolTip()
{
    QString text = tr("Timbre deviation up to which a root key is consistent") + "\n" +
            tr("The other timbre thresholds follow it: too different beyond %1, redundant up to %2, break beyond %3.")
            .arg(formatDb(_settings.maximum()), formatDb(_settings.redundancy()), formatDb(_settings.jump())) + "\n" +
            tr("%1 for organs, about %2 for pianos, guitars, woodwinds and general MIDI banks.")
            .arg(formatDb(TimbreSettings::DEFAULT_TOLERANCE), formatDb(6, 0));
    _spinTolerance->setToolTip(text);
    _labelTolerance->setToolTip(text);
}

void PageTimbre::onDisplayChanged()
{
    TimbreDisplay display = _map->getDisplay();
    display.mode = static_cast<TimbreDisplay::Mode>(_comboMode->currentData().toInt());
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
        else if (action->objectName() == "playClicked")
            _playClickedNotes = action->isChecked();
    }
    display.summaries = summaries;
    _map->setDisplay(display);
    updateLegend();
}

void PageTimbre::updateLegend()
{
    // Unit on the right only, red and blue explained in the tooltip
    double range = _map->getRange();
    QString hatching = tr("Hatched: not reliably measured or being analyzed. Empty: nothing to measure.");
    QString meaning;
    switch (_map->getDisplay().mode)
    {
    case TimbreDisplay::MODE_LEVELS:
        _legend->setScale(false, formatValue(-range, 0), formatDb(0, 0));
        break;
    case TimbreDisplay::MODE_DEVIATION:
        _legend->setScale(true, formatValue(-range, 0, QString(), true), formatDb(range, 0, true));
        meaning = tr("Red: stronger than the reference, blue: weaker.");
        break;
    case TimbreDisplay::MODE_JUMP:
        _legend->setScale(true, formatValue(-range, 0, QString(), true), formatDb(range, 0, true));
        meaning = tr("Red: stronger than the previous key, blue: weaker.");
        break;
    case TimbreDisplay::MODE_TRANSPOSITION:
        _legend->setScale(true, formatValue(-range, 0, QString(), true), formatValue(range, 0, tr("semitones"), true));
        meaning = tr("Red: sample transposed up, blue: transposed down.");
        break;
    }
    _legend->setToolTip(meaning.isEmpty() ? hatching : meaning + "\n" + hatching);
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
    // The selected key stays if it displays a selected root key or is played by one, otherwise the first one is displayed
    if (!positions.isEmpty() && !positions.contains(getPosition(_selectedKey)))
        _selectedKey = _model.getPositions()[positions.first()].anchor;
    if (!positions.contains(_anchorKey))
        _anchorKey = positions.isEmpty() ? -1 : getPosition(_selectedKey);
    _map->setSelection(_selectedKey, _selectedPositions);
    updateDetail();
}

int PageTimbre::getPosition(int key)
{
    // The root key displayed at the key (the key itself, or a root key outside the keys covered by its divisions),
    // otherwise the root key of the main division playing it
    return _model.getPositionOf(key);
}

void PageTimbre::restoreSelection(int key, const QList<int> &positions, int anchorKey)
{
    // Root keys that still exist (a replaced root key is at the same key), otherwise the one now playing the key
    QList<int> kept;
    foreach (int position, positions)
        if (_model.getPositions().contains(position))
            kept << position;
    int playing = getPosition(key);
    if (kept.isEmpty() && playing >= 0)
        kept << playing;

    // The tree selects their divisions again and notifies the page, which can change the selected key meanwhile
    _selectedKey = key;
    selectPositions(kept, true);
    _selectedKey = key;
    _anchorKey = kept.contains(anchorKey) ? anchorKey : (kept.isEmpty() ? -1 : kept.first());
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
    // A key that is not a root key acts on the root key playing it (its divisions are selected in the tree), the
    // details still being those of the key clicked. Several root keys as in the range editor: Ctrl adds or removes
    // one, Shift selects all of them from the root key clicked previously.
    _selectedKey = key;
    int position = getPosition(key);
    if ((modifiers & Qt::ShiftModifier) && _anchorKey >= 0 && position >= 0)
    {
        QList<int> positions;
        foreach (int other, _model.getPositions().keys())
            if (other >= qMin(_anchorKey, position) && other <= qMax(_anchorKey, position))
                positions << other;
        selectPositions(positions, true);
    }
    else if (modifiers & (Qt::ShiftModifier | Qt::ControlModifier))
    {
        QList<int> positions = _selectedPositions;
        if (position >= 0)
        {
            if (positions.contains(position))
                positions.removeAll(position);
            else
            {
                positions << position;
                _anchorKey = position;
            }
        }
        selectPositions(positions, true);
    }
    else if (position >= 0)
    {
        _anchorKey = position;
        selectPositions(QList<int>() << position, true);
    }
    else
    {
        // Key without sample: nothing to select in the tree
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

QList<int> PageTimbre::getTargets()
{
    // The selected root keys if the selected key displays one of them or is played by one of them, otherwise the root
    // key displayed at the selected key
    QList<int> targets;
    int position = getPosition(_selectedKey);
    if (_selectedPositions.contains(position))
        targets = _selectedPositions;
    else if (position >= 0 && _model.getPositions()[position].anchor == _selectedKey)
        targets << position;
    std::sort(targets.begin(), targets.end());
    return targets;
}

void PageTimbre::updateDetail()
{
    TimbrePalette palette(TimbrePalette::SURFACE_WINDOW); // Colors of the graph
    int key = _selectedKey;

    // Root key displayed at the key (the key itself, or a root key outside the keys covered by its divisions) or playing it
    int positionKey = getPosition(key);
    bool displayed = positionKey >= 0 && _model.getPositions()[positionKey].anchor == key;
    bool hasPosition = displayed && positionKey == key;
    bool covered = key >= 0 && !_model.getKey(key).sources.isEmpty();
    bool canCreate = key >= 0 && !_model.getPositions().contains(key) && !displayed && _model.getPositions().count() >= 2;
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
        _stackDetail->setCurrentIndex(0);
        _bars->clear();
        _detail->beginUpdate();
        _detail->addNotice(tr("No division of this instrument plays at the velocity %1.").arg(_settings.velocity));
        _detail->endUpdate();
        return;
    }
    if (key < 0)
    {
        _stackDetail->setCurrentIndex(1);
        _bars->clear();
        return;
    }
    _stackDetail->setCurrentIndex(0);

    QLocale locale;
    SoundfontManager * sm = SoundfontManager::getInstance();
    const TimbreKey &tk = _model.getKey(key);
    const QMap<int, TimbrePosition> &positions = _model.getPositions();
    QMap<int, TimbrePosition>::const_iterator positionIt = positions.constFind(positionKey);
    const TimbrePosition * position = (positionIt != positions.constEnd()) ? &positionIt.value() : nullptr;
    const QString levelName = TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_LEVEL);
    const QString timbreName = TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_TIMBRE);
    auto withThreshold = [this](QString explanation, double threshold) -> QString {
        return explanation + " (" + formatDb(threshold) + ")";
    };
    QList<TimbreBarsWidget::Series> series;
    _detail->beginUpdate();

    // Title: the key, then the label of the root key playing it (same shapes and colors as in the map)
    TimbreDetailWidget::Badge label;
    if (position != nullptr && position->deviation.valid)
    {
        switch (position->label)
        {
        case TimbrePosition::LABEL_CONFORM:
            label = TimbreDetailWidget::makeBadge(TimbrePalette::SHAPE_CHECK, palette.conform(), tr("Consistent"),
                                                  withThreshold(tr("Timbre deviation up to which a root key is consistent"), _settings.tolerance));
            break;
        case TimbrePosition::LABEL_REDUNDANT:
            label = TimbreDetailWidget::makeBadge(TimbrePalette::SHAPE_MINUS, palette.redundant(), tr("Redundant"),
                                                  withThreshold(tr("Largest timbre deviation over the keys of a consistent root key if it were "
                                                                    "removed, for being redundant"), _settings.redundancy()));
            break;
        case TimbrePosition::LABEL_TO_CORRECT:
            label = TimbreDetailWidget::makeBadge(TimbrePalette::SHAPE_TILDE, palette.toCorrect(), tr("To correct"),
                                                  withThreshold(tr("Timbre deviation up to which a root key can be corrected, beyond it is "
                                                                    "too different"), _settings.maximum()));
            break;
        case TimbrePosition::LABEL_TOO_DIFFERENT:
            label = TimbreDetailWidget::makeBadge(TimbrePalette::SHAPE_CROSS, palette.tooDifferent(), tr("Too different"),
                                                  withThreshold(tr("Timbre deviation up to which a root key can be corrected, beyond it is "
                                                                    "too different"), _settings.maximum()));
            break;
        default:
            break;
        }
    }
    if (hasPosition)
        _detail->setTitle(tr("Root key %1").arg(keyName(key)), label);
    else
    {
        _detail->setTitle(tr("Key %1").arg(keyName(key)));
        if (!covered)
            _detail->setSubtitle(tr("No sample"));
        else if (position != nullptr)
            _detail->setSubtitle(tr("Played by the root key %1").arg(keyName(position->key)), label);
    }

    // Notices
    if (targets.count() > 1)
        _detail->addNotice(tr("%n root keys selected: the actions apply to all of them.", "", static_cast<int>(targets.count())));
    if (position != nullptr && !position->deviation.valid && !position->analyzed)
        _detail->addNotice(tr("Analysis in progress..."));

    // Deviation of the root key from its reference
    if (position != nullptr && position->deviation.valid)
    {
        const TimbreComparison &deviation = position->deviation;
        TimbreDetailWidget::Badge levelBadge;
        if (position->levelLabel)
            levelBadge = TimbreDetailWidget::makeBadge(
                        deviation.levelDifference > 0 ? TimbrePalette::SHAPE_UP : TimbrePalette::SHAPE_DOWN,
                        palette.levelColor(deviation.levelDifference > 0),
                        deviation.levelDifference > 0 ? tr("Too loud") : tr("Too soft"),
                        withThreshold(tr("Level deviation from which a correction of the attenuation is proposed"), TimbreSettings::LEVEL));
        // Measured at the root key: its name is given when another key is selected
        _detail->beginSection(TimbreDetailWidget::SECTION_DEVIATION,
                              hasPosition ? TimbreMapWidget::getModeName(TimbreDisplay::MODE_DEVIATION) :
                                            tr("Deviation of the root key %1").arg(keyName(position->key)),
                              timbreName + " " + formatDb(deviation.timbreDifference), levelBadge);
        _detail->addRow(levelName, QStringList(formatDb(deviation.levelDifference, 1, true)), QString(),
                        TimbreDetailWidget::shapeOnly(levelBadge));
        _detail->addRow(timbreName, QStringList(formatDb(deviation.timbreDifference)));
        _detail->addRow(tr("Confidence"),
                        QStringList(tr("%1%", "percentage").arg(qRound(100 * deviation.confidence))));
        if (position->referenceAtEdge)
            _detail->addNote(tr("Reference extrapolated from the neighbors on one side: less reliable"));
        if (position->anchor != position->key)
            _detail->addNote(tr("Root key outside the key range of its divisions: measured by playing them at the root key"));

        // Consequences of a removal
        if (position->redundancyComputed)
        {
            _detail->beginSection(TimbreDetailWidget::SECTION_REMOVAL, tr("If removed"),
                                  timbreName + " " + formatDb(position->redundancy));
            _detail->addColumnHeaders(QStringList() << levelName << timbreName);
            _detail->addRow(tr("Largest deviation"), QStringList() << formatDb(position->redundancyLevel)
                            << formatDb(position->redundancy));
            if (position->newJunction.valid)
                _detail->addRow(tr("New junction"), QStringList() << formatDb(position->newJunction.levelDifference, 1, true)
                                << formatDb(position->newJunction.timbreDifference));
        }
    }

    // Bars of the root key displayed (played at the root key, even outside the keys covered by its divisions, as for
    // its deviation), of the key or of a computed note
    if (position != nullptr && displayed)
    {
        series << TimbreBarsWidget::Series { hasPosition ? tr("Heard") : tr("Played at the root key"),
                                             _model.getRowLevels(position->spectrum), palette.highlight(),
                                             TimbreBarsWidget::STYLE_BARS };
        if (!position->reference.isEmpty())
            series << TimbreBarsWidget::Series { tr("Reference"), _model.getRowLevels(position->reference), palette.text(),
                                                 TimbreBarsWidget::STYLE_MARKERS };
    }
    if (!displayed && covered)
        series << TimbreBarsWidget::Series { tr("Heard"), _model.getRowLevels(tk.spectrum), palette.highlight(),
                                             TimbreBarsWidget::STYLE_BARS };
    if (canCreate)
    {
        bool atEdge = false;
        TimbreSpectrum predicted = _model.predict(key, -1, &atEdge);
        series << TimbreBarsWidget::Series { tr("Computed note"), _model.getRowLevels(predicted), palette.breakColor(),
                                             TimbreBarsWidget::STYLE_OUTLINE };
    }

    // Jumps at the junctions around the key
    const TimbreKey * nextKey = key < 127 ? &_model.getKey(key + 1) : nullptr;
    bool previousJunction = covered && key > 0 && tk.junction && tk.jump.valid;
    bool nextJunction = covered && nextKey != nullptr && nextKey->junction && nextKey->jump.valid;
    if (previousJunction || nextJunction)
    {
        TimbreDetailWidget::Badge breakBadge = TimbreDetailWidget::makeBadge(
                    TimbrePalette::SHAPE_PLUS, palette.breakColor(), tr("Break"), withThreshold(tr("Timbre jump at a junction from which a computed note is proposed"),
                                                                                         _settings.jump()));
        bool hasBreak = (previousJunction && tk.breakLabel) || (nextJunction && nextKey->breakLabel);
        double largest = qMax(previousJunction ? tk.jump.timbreDifference : 0.0, nextJunction ? nextKey->jump.timbreDifference : 0.0);
        _detail->beginSection(TimbreDetailWidget::SECTION_JUNCTIONS, tr("Junctions"), timbreName + " " + formatDb(largest),
                              hasBreak ? breakBadge : TimbreDetailWidget::Badge());
        _detail->addColumnHeaders(QStringList() << levelName << timbreName);
        if (previousJunction)
            _detail->addRow(keyName(key - 1) + " – " + keyName(key), QStringList() << formatDb(tk.jump.levelDifference, 1, true)
                            << formatDb(tk.jump.timbreDifference), QString(),
                            tk.breakLabel ? TimbreDetailWidget::shapeOnly(breakBadge) : TimbreDetailWidget::Badge());
        if (nextJunction)
            _detail->addRow(keyName(key) + " – " + keyName(key + 1), QStringList() << formatDb(nextKey->jump.levelDifference, 1, true)
                            << formatDb(nextKey->jump.timbreDifference), QString(),
                            nextKey->breakLabel ? TimbreDetailWidget::shapeOnly(breakBadge) : TimbreDetailWidget::Badge());
    }

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
            _detail->beginSection(TimbreDetailWidget::SECTION_RENDERING, tr("Rendering against the model"),
                                  timbreName + " " + formatDb(comparison.timbreDifference));
            _detail->addRow(levelName, QStringList(formatDb(comparison.levelDifference, 1, true)));
            _detail->addRow(timbreName, QStringList(formatDb(comparison.timbreDifference)));
            _detail->addRow(tr("Largest deviation of a row"), QStringList(formatDb(largest)));
        }
    }

    if (covered)
    {
        // Samples played and their parameters
        _detail->beginSection(TimbreDetailWidget::SECTION_SAMPLES, tr("Samples"));
        foreach (TimbreVoice voice, tk.voices)
        {
            _detail->addSubtitle(sm->getQstr(EltID(elementSmpl, _idInst.indexSf2, voice.sampleIndex), champ_name));
            _detail->addRow(TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_TRANSPOSITION),
                            QStringList(formatValue(voice.transposition / 100, 2, tr("semitones"), true)));
            _detail->addRow(tr("Attenuation"), QStringList(formatDb(voice.attenuation)));
            _detail->addRow(tr("Filter"), QStringList(locale.toString(qRound(voice.filterFrequency)) + " " +
                                                      tr("Hz", "unit for Herz")));
        }

        // Descriptors of what is heard
        _detail->beginSection(TimbreDetailWidget::SECTION_MEASURES, tr("Measures"));
        _detail->addRow(levelName, QStringList(formatDb(tk.totalLevel)));
        _detail->addRow(TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_BRIGHTNESS), QStringList(formatValue(tk.centroid, 2)),
                        tr("Center of gravity of the partials, as a rank"));
        _detail->addRow(tr("Spectral slope"), QStringList(formatValue(tk.slope, 1, tr("dB/octave", "unit for decibels per octave"), true)));
        _detail->addRow(TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_ODD_EVEN), QStringList(formatDb(tk.oddEven, 1, true)),
                        tr("Odd ranks (3, 5...) against even ranks (2, 4...)"));
        _detail->addRow(TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_NOISE), QStringList(formatDb(tk.hnr)),
                        tr("Harmonics-to-noise ratio"));
        _detail->addRow(TimbreMapWidget::getSummaryName(TimbreDisplay::SUMMARY_TUNING),
                        QStringList(formatValue(tk.tuning - 100 * std::round(tk.tuning / 100), 1, tr("cents"), true)),
                        tr("Deviation from the closest note of the equal temperament"));
        _detail->addRow(tr("Duration factor"), QStringList(formatValue(tk.durationFactor, 3)),
                        tr("Durations of the attack, the tremolo and the beats compared to the sample"));
    }

    _detail->endUpdate();
    _bars->setData(_model.getRows(), series, TimbreSettings::DYNAMIC_RANGE);
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
        TimbreActions::removePosition(sm, _model, excludedPosition, true, creation.replacedRanges.isEmpty());
    return !TimbreActions::applyCreation(sm, creation, warnings).isEmpty();
}

void PageTimbre::onCreate()
{
    int key = _selectedKey;
    if (key < 0 || _model.getPositions().contains(key) || _model.getPositionDisplayedAt(key) >= 0 ||
        _model.getPositions().count() < 2)
        return;
    DialogTimbreCreation dialog(_model, key, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    QStringList warnings;
    int anchorKey = _anchorKey;
    QList<int> selectedPositions = _selectedPositions;
    bool created = createNote(key, dialog.getClosestKey(), dialog.getSecondKey(), -1, warnings);
    SoundfontManager::getInstance()->endEditing(_editingSource);
    recompute(); // The page is not updated by its own editions

    // The new root key is selected, otherwise the selection is restored (divisions covered by the new one are removed)
    if (created)
        restoreSelection(key, QList<int>() << key, key);
    else
        restoreSelection(key, selectedPositions, anchorKey);
    if (!warnings.isEmpty())
        QMessageBox::warning(this, tr("Warning"), warnings.join("<br/>"));
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
    // Free key (neither a root key nor displaying one) in the middle of the keys where the root keys on both sides of
    // the junction are heard
    int previous = _model.getKey(junctionKey - 1).position;
    int next = _model.getKey(junctionKey).position;
    if (!_model.getPositions().contains(previous) || !_model.getPositions().contains(next))
        return -1;
    int low = qMin(_model.getPositions()[previous].anchor, _model.getPositions()[next].anchor);
    int high = qMax(_model.getPositions()[previous].anchor, _model.getPositions()[next].anchor);
    int best = -1;
    for (int key = low + 1; key < high; key++)
        if (!_model.getPositions().contains(key) && _model.getPositionDisplayedAt(key) < 0 &&
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
        _selectedKey = _model.getPositions()[targets.first()].anchor;
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

    // Selection restored afterwards: the tree selects other elements when divisions are removed
    int selectedKey = _selectedKey, anchorKey = _anchorKey;
    QList<int> selectedPositions = _selectedPositions;

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
    restoreSelection(selectedKey, selectedPositions, anchorKey);
    if (!canceled && (!warnings.isEmpty() || !skipped.isEmpty()))
    {
        QStringList parts;
        if (!skipped.isEmpty())
            parts << tr("Not processed (the label changed or nothing to do):") + " " + skipped.join(", ");
        parts << warnings;
        QMessageBox::warning(this, tr("Warning"), parts.join("<br/>"));
    }
}
