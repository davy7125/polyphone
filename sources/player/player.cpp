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

Player::Player(QWidget *parent) : Tab(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
}

Player::~Player()
{
    delete ui;
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
    //ui->treeView->setRootIndex(model->index(3, 0));
    new PlayerTreeProxyModel(indexSf2, ui->treeView, model);

    // Display the player controls
    ui->stackedMain->setCurrentWidget(ui->pagePlay);
}

void Player::tabUpdate(QString editingSource)
{
    // No editing in this mode
    Q_UNUSED(editingSource)
}