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
#include <QRadioButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>

DialogTimbreCreation::DialogTimbreCreation(const TimbreModel &model, int key, QWidget * parent) : QDialog(parent),
    _key(key),
    _rootKeys(model.getPositions().keys())
{
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
    getAutomaticKeys(_rootKeys, key, closest, second);
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
    connect(_comboClosest, &QComboBox::activated, this, [this]() { _radioManual->setChecked(true); });
    connect(_comboSecond, &QComboBox::activated, this, [this]() { _radioManual->setChecked(true); });
    updateWarning();
}

void DialogTimbreCreation::getAutomaticKeys(const QList<int> &rootKeys, int key, int &closest, int &second)
{
    closest = second = -1;
    int below = -1, above = -1;
    for (int i = 0; i < rootKeys.count(); i++)
    {
        if (rootKeys[i] < key)
            below = i;
        else if (rootKeys[i] > key && above == -1)
            above = i;
    }
    if (below >= 0 && above >= 0)
    {
        bool lowerCloser = key - rootKeys[below] < rootKeys[above] - key;
        closest = rootKeys[lowerCloser ? below : above];
        second = rootKeys[lowerCloser ? above : below];
    }
    else if (below >= 1)
    {
        closest = rootKeys[below];
        second = rootKeys[below - 1];
    }
    else if (above >= 0 && above + 1 < rootKeys.count())
    {
        closest = rootKeys[above];
        second = rootKeys[above + 1];
    }
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
        getAutomaticKeys(_rootKeys, _key, closest, second);

    QStringList warnings;
    if (closest == second)
        warnings << tr("The two root keys must be different.");
    else
    {
        bool between = _key > qMin(closest, second) && _key < qMax(closest, second);
        if (!between && std::abs(_key - closest) > 2 * std::abs(second - closest))
            warnings << tr("The key is far from the root keys used: the extrapolation of the harmonics may be inaccurate.");
        if (std::abs(_key - closest) > 12)
            warnings << tr("The closest root key is transposed by more than an octave.");
    }
    _labelWarning->setText(warnings.join("<br/>"));
    _labelWarning->setVisible(!warnings.isEmpty());
}
