/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "player.h"
#include "ui_player.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "playertreeproxymodel.h"
#include "playeroptions.h"

Player::Player(PlayerOptions * playerOptions, QWidget * parent) : Tab(parent),
    ui(new Ui::Player),
    _playerOptions(new PlayerOptions(playerOptions))
{
    ui->setupUi(this);

    QString highlightColorBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightColorText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    ui->topBar->setStyleSheet("QWidget#topBar{background-color:" + highlightColorBackground + "} QLabel{color:" + highlightColorText + "}");

    // Keyboard with all keys (128)
    ui->keyboard->set(PianoKeybd::PROPERTY_KEY_MIN, 0);
    ui->keyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, 128);

    // Initialize the configuration
    _initializing = true;
    ui->comboChannel->setCurrentIndex(_playerOptions->playerChannel());
    ui->comboMultipleSelection->setCurrentIndex(_playerOptions->playerMultipleSelection() ? 1 : 0);
    ui->comboSelectionByKeys->setCurrentIndex(_playerOptions->playerKeySelection());
    _initializing = false;
}

Player::~Player()
{
    delete ui;
    delete _playerOptions;
}

void Player::tabInitializing(QString filename)
{
    Q_UNUSED(filename);
    ui->rotatingSpinner->startAnimation();
}

void Player::tabInError(QString errorMessage)
{
    // Display the error
    ui->labelReason->setText(errorMessage);
    ui->stackedMain->setCurrentWidget(ui->pageError);
}

void Player::tabInitialized(int indexSf2)
{
    // Prepare the list
    QAbstractItemModel * model = SoundfontManager::getInstance()->getModel(indexSf2);
    ui->treeView->setModel(model);
    PlayerTreeProxyModel * proxyModel = new PlayerTreeProxyModel(indexSf2, ui->treeView, model);
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));

    // Display the player controls
    ui->stackedMain->setCurrentWidget(ui->pagePlay);

    // Select the first preset
    QModelIndex firstBankIndex = proxyModel->index(0, 0, QModelIndex());
    if (firstBankIndex.isValid())
        ui->treeView->setCurrentIndex(proxyModel->index(0, 0, firstBankIndex));
}

void Player::tabUpdate(QString editingSource)
{
    // No editing in this mode
    Q_UNUSED(editingSource)
}

void Player::onSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(deselected)
    QModelIndexList indexes = selected.indexes();
    foreach (const QModelIndex &index, indexes)
    {
        _currentId = index.data(Qt::UserRole).value<EltID>();
        qDebug() << _currentId.toString();
    }
}


bool Player::processKey(int channel, int key, int vel)
{
    if (channel != _playerOptions->playerChannel() - 1 && _playerOptions->playerChannel() != 0)
        return false;

    ContextManager::audio()->getSynth()->play(_currentId, channel, key, vel);

    if (vel > 0)
        ui->keyboard->inputNoteOn(key);
    else
        ui->keyboard->inputNoteOff(key);

    return false;
}

void Player::on_comboChannel_currentIndexChanged(int index)
{
    if (_initializing)
        return;
    _playerOptions->setPlayerChannel(index);
    ui->keyboard->set(PianoKeybd::PROPERTY_CHANNEL, index - 1);
}


void Player::on_comboMultipleSelection_currentIndexChanged(int index)
{
    if (_initializing)
        return;
    _playerOptions->setMultipleSelection(index == 1);
}


void Player::on_comboSelectionByKeys_currentIndexChanged(int index)
{
    if (_initializing)
        return;
    _playerOptions->setKeySelection(index);
}

