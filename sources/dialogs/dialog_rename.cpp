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

#include "dialog_rename.h"
#include "ui_dialog_rename.h"
#include "contextmanager.h"
#include "latinvalidator.h"
#include <QListView>

DialogRename::DialogRename(ElementType type, QString defaultText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRename),
    _type(type)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Possibly hide renaming options
    QListView* view = qobject_cast<QListView *>(ui->comboBox->view());
    view->setRowHidden(0, _type != elementSmpl && _type != elementInstSmpl); // Key as suffix
    view->setRowHidden(1, _type != elementInstSmpl); // Key and velocity as suffix

    // Select a row
    int rowToSelect = ContextManager::configuration()->getValue(ConfManager::SECTION_BULK_RENAME, "option", 0).toInt();
    ui->comboBox->blockSignals(true);
    ui->comboBox->setCurrentIndex(view->isRowHidden(rowToSelect) ? 2 : rowToSelect);
    ui->comboBox->blockSignals(false);

    // Default text and values
    ui->lineText1->setText(defaultText);
    ui->lineText1->setValidator(new LatinValidator(ui->lineText1));
    ui->lineText2->setText(defaultText);
    ui->lineText2->setValidator(new LatinValidator(ui->lineText2));
    ui->spinPos1->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_BULK_RENAME, "int_1", 1).toInt());
    ui->spinPos2->setValue(ContextManager::configuration()->getValue(ConfManager::SECTION_BULK_RENAME, "int_2", 1).toInt());

    on_comboBox_currentIndexChanged(ui->comboBox->currentIndex());

    ui->lineText1->selectAll();
    ui->lineText1->setFocus();
}

DialogRename::~DialogRename()
{
    delete ui;
}

void DialogRename::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: case 1:
        // Overwrite existing name with key name (and velocity) as suffix
        ui->labelPos->hide();
        ui->spinPos1->hide();
        ui->spinPos2->hide();
        ui->labelString1->setText(tr("New name:"));
        ui->labelString1->show();
        ui->lineText1->show();
        ui->labelString2->hide();
        ui->lineText2->hide();
        break;
    case 2:
        // Overwrite existing name with numerical ascending suffix
        ui->labelPos->hide();
        ui->spinPos1->hide();
        ui->spinPos2->hide();
        ui->labelString1->setText(tr("New name:"));
        ui->labelString1->show();
        ui->lineText1->show();
        ui->labelString2->hide();
        ui->lineText2->hide();
        break;
    case 3:
        // Replace characters
        ui->labelPos->hide();
        ui->spinPos1->hide();
        ui->spinPos2->hide();
        ui->labelString1->setText(tr("Find:"));
        ui->labelString1->show();
        ui->lineText1->show();
        ui->labelString2->setText(tr("And replace by:"));
        ui->labelString2->show();
        ui->lineText2->show();
        break;
    case 4:
        // Insert after a specific position
        ui->labelPos->show();
        ui->spinPos1->show();
        ui->spinPos2->hide();
        ui->labelPos->setText(tr("Position"));
        ui->labelString1->setText(tr("Text to insert:"));
        ui->labelString1->show();
        ui->lineText1->show();
        ui->labelString2->hide();
        ui->lineText2->hide();
        break;
    case 5:
        // Delete character range
        ui->labelPos->show();
        ui->spinPos1->show();
        ui->spinPos2->show();
        ui->labelPos->setText(tr("Range"));
        ui->labelString1->hide();
        ui->lineText1->hide();
        ui->labelString2->hide();
        ui->lineText2->hide();
        break;
    default:
        break;
    }

    this->adjustSize();
}

void DialogRename::on_pushCancel_clicked()
{
    QDialog::reject();
}

void DialogRename::on_pushOk_clicked()
{
    int type = ui->comboBox->currentIndex();
    this->updateNames(type, ui->lineText1->text(), ui->lineText2->text(),
                      ui->spinPos1->value(), ui->spinPos2->value());

    // Mémorisation des paramètres
    ContextManager::configuration()->setValue(ConfManager::SECTION_BULK_RENAME, "option", ui->comboBox->currentIndex());
    ContextManager::configuration()->setValue(ConfManager::SECTION_BULK_RENAME, "int_1", ui->spinPos1->value());
    ContextManager::configuration()->setValue(ConfManager::SECTION_BULK_RENAME, "int_2", ui->spinPos2->value());

    QDialog::accept();
}
