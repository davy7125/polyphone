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


#include "dialogtimbrecreation.h"
#include "dialogbuttonbox.h"
#include "contextmanager.h"
#include "missing_samples/missingsamplecreator.h"
#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

DialogTimbreCreation::DialogTimbreCreation(const TimbreModel &model, int key, QWidget * parent) : QDialog(parent),
    _key(key),
    _rootKeys(model.getPositions().keys())
{
    foreach (int rootKey, _rootKeys)
        _anchors[rootKey] = model.getPositions()[rootKey].anchor;

    this->setWindowTitle(tr("Create a computed note"));
    this->setWindowFlags((this->windowFlags() | Qt::Tool) & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout * layout = new QVBoxLayout(this);
    KeyNameManager * keyNames = ContextManager::keyName();
    QLabel * label = new QLabel(tr("Note at the key %1: the closest root key is transposed and the levels of its "
                                   "harmonics are interpolated (or extrapolated) from a second root key.")
                                .arg(keyNames->getKeyName(static_cast<unsigned int>(key))), this);
    label->setWordWrap(true);
    layout->addWidget(label);

    int closest, second;
    getAutomaticKeys(_anchors, key, closest, second);
    _radioAutomatic = new QRadioButton(tr("Automatic: %1 transposed, %2 for the harmonics").arg(
                                           keyNames->getKeyName(static_cast<unsigned int>(closest)),
                                           keyNames->getKeyName(static_cast<unsigned int>(second))), this);
    _radioAutomatic->setChecked(true);
    layout->addWidget(_radioAutomatic);
    _radioManual = new QRadioButton(tr("Chosen root keys:"), this);
    layout->addWidget(_radioManual);
    QFormLayout * form = new QFormLayout();
    form->setContentsMargins(20, 0, 0, 0);
    _comboClosest = new QComboBox(this);
    _comboSecond = new QComboBox(this);
    foreach (int rootKey, _rootKeys)
    {
        _comboClosest->addItem(keyNames->getKeyName(static_cast<unsigned int>(rootKey)), rootKey);
        _comboSecond->addItem(keyNames->getKeyName(static_cast<unsigned int>(rootKey)), rootKey);
    }
    _comboClosest->setCurrentIndex(static_cast<int>(_rootKeys.indexOf(closest)));
    _comboSecond->setCurrentIndex(static_cast<int>(_rootKeys.indexOf(second)));
    form->addRow(tr("Root key transposed"), _comboClosest);
    form->addRow(tr("Root key for the harmonics"), _comboSecond);
    layout->addLayout(form);
    _labelWarning = new QLabel(this);
    _labelWarning->setWordWrap(true);
    _labelWarning->setStyleSheet("QLabel{color:" + ContextManager::theme()->getFixedColor(
                                     ThemeManager::RED, ThemeManager::WINDOW_BACKGROUND).name() + "}");
    layout->addWidget(_labelWarning);

    DialogButtonBox * buttonBox = new DialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    connect(_radioAutomatic, SIGNAL(toggled(bool)), this, SLOT(updateWarning()));
    connect(_comboClosest, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWarning()));
    connect(_comboSecond, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWarning()));
    connect(_comboClosest, SIGNAL(activated(int)), this, SLOT(selectManual()));
    connect(_comboSecond, SIGNAL(activated(int)), this, SLOT(selectManual()));
    updateWarning();
}

void DialogTimbreCreation::selectManual()
{
    _radioManual->setChecked(true);
}

void DialogTimbreCreation::getAutomaticKeys(const QMap<int, int> &anchors, int key, int &closest, int &second)
{
    QMap<int, MissingSamplePosition> positions;
    for (QMap<int, int>::const_iterator it = anchors.constBegin(); it != anchors.constEnd(); ++it)
    {
        positions[it.key()].key = it.key();
        positions[it.key()].anchor = it.value();
    }
    MissingSampleJob job = MissingSampleCreator::createJob(EltID(), key, positions);
    closest = job.closest.key;
    second = (closest >= 0 && job.second.key != closest) ? job.second.key : -1;
}

int DialogTimbreCreation::getClosestKey()
{
    return _radioManual->isChecked() ? _comboClosest->currentData().toInt() : -1;
}

int DialogTimbreCreation::getSecondKey()
{
    return _radioManual->isChecked() ? _comboSecond->currentData().toInt() : -1;
}

void DialogTimbreCreation::updateWarning()
{
    int closest, second;
    if (_radioManual->isChecked())
    {
        closest = _comboClosest->currentData().toInt();
        second = _comboSecond->currentData().toInt();
    }
    else
        getAutomaticKeys(_anchors, _key, closest, second);

    QStringList warnings;
    if (closest == second)
        warnings << tr("The two root keys must be different.");
    else
    {
        // Distances between the keys where the root keys are heard (the root keys themselves without extension)
        int closestKey = _anchors.value(closest, closest);
        int secondKey = _anchors.value(second, second);
        bool between = _key > qMin(closestKey, secondKey) && _key < qMax(closestKey, secondKey);
        if (!between && std::abs(_key - closestKey) > 2 * std::abs(secondKey - closestKey))
            warnings << tr("The key is far from the root keys used: the extrapolation of the harmonics may be inaccurate.");
        if (std::abs(_key - closestKey) > 12)
            warnings << tr("The closest root key is transposed by more than an octave.");
    }
    _labelWarning->setText(warnings.join("<br/>"));
    _labelWarning->setVisible(!warnings.isEmpty());
}
