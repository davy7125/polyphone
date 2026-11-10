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

#include "toolmissingsamples_gui.h"
#include "ui_toolmissingsamples_gui.h"
#include "toolmissingsamples_parameters.h"
#include "contextmanager.h"
#include <QMessageBox>
#include <QRegularExpression>

ToolMissingSamples_gui::ToolMissingSamples_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolMissingSamples_gui)
{
    ui->setupUi(this);
    ui->buttonBox->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
    connect(ui->radioKeys, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
    connect(ui->radioSpacing, SIGNAL(toggled(bool)), this, SLOT(updateEnabledState()));
}

ToolMissingSamples_gui::~ToolMissingSamples_gui()
{
    delete ui;
}

void ToolMissingSamples_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolMissingSamples_parameters * params = dynamic_cast<ToolMissingSamples_parameters *>(parameters);

    if (params->getMode() == ToolMissingSamples_parameters::MODE_KEYS)
        ui->radioKeys->setChecked(true);
    else
        ui->radioSpacing->setChecked(true);
    ui->lineKeys->setText(keysToText(params->getKeys()));
    ui->lineKeys->setPlaceholderText(tr("Example:") + " " + keysToText(QList<int>() << 61 << 63 << 84 << 85 << 86));
    ui->spinSpacing->setValue(params->getMaxSpacing());
    updateEnabledState();
}

void ToolMissingSamples_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolMissingSamples_parameters * params = dynamic_cast<ToolMissingSamples_parameters *>(parameters);

    params->setMode(ui->radioKeys->isChecked() ? ToolMissingSamples_parameters::MODE_KEYS :
                                                 ToolMissingSamples_parameters::MODE_MAX_SPACING);
    QList<int> keys;
    QString error;
    if (parseKeys(ui->lineKeys->text(), keys, error))
        params->setKeys(keys);
    params->setMaxSpacing(ui->spinSpacing->value());
}

void ToolMissingSamples_gui::updateEnabledState()
{
    ui->lineKeys->setEnabled(ui->radioKeys->isChecked());
    ui->spinSpacing->setEnabled(ui->radioSpacing->isChecked());
}

bool ToolMissingSamples_gui::parseKeys(QString text, QList<int> &keys, QString &error)
{
    keys.clear();
    error.clear();

    // Spaces around the range separators removed, then the parts are separated by commas, semicolons or spaces
    static const QRegularExpression spacedSeparator("\\s+-\\s*|\\s*-\\s+");
    static const QRegularExpression partSeparator("[,;\\s]+");
    static const QRegularExpression rangeSeparator("[0-9]-");
    text.replace(spacedSeparator, "-");
    foreach (QString part, text.split(partSeparator, Qt::SkipEmptyParts))
    {
        int first, last;
        int position = static_cast<int>(part.indexOf(rangeSeparator));
        if (position >= 0)
        {
            first = ContextManager::keyName()->getKeyNum(part.left(position + 1));
            last = ContextManager::keyName()->getKeyNum(part.mid(position + 2));
        }
        else
            first = last = ContextManager::keyName()->getKeyNum(part);
        if (first < 0 || last < 0)
        {
            error = part;
            return false;
        }
        if (first > last)
            std::swap(first, last);
        for (int key = first; key <= last; key++)
            if (!keys.contains(key))
                keys << key;
    }
    std::sort(keys.begin(), keys.end());
    return true;
}

QString ToolMissingSamples_gui::keysToText(QList<int> keys)
{
    std::sort(keys.begin(), keys.end());
    QStringList parts;
    int i = 0;
    while (i < keys.count())
    {
        // Consecutive keys
        int j = i;
        while (j + 1 < keys.count() && keys[j + 1] == keys[j] + 1)
            j++;
        QString first = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(keys[i]));
        if (j - i >= 2)
            parts << first + "-" + ContextManager::keyName()->getKeyName(static_cast<unsigned int>(keys[j]));
        else
        {
            parts << first;
            if (j > i)
                parts << ContextManager::keyName()->getKeyName(static_cast<unsigned int>(keys[j]));
        }
        i = j + 1;
    }
    return parts.join(", ");
}

void ToolMissingSamples_gui::on_buttonBox_rejected()
{
    emit this->canceled();
}

void ToolMissingSamples_gui::on_buttonBox_accepted()
{
    if (ui->radioKeys->isChecked())
    {
        QList<int> keys;
        QString error;
        if (!parseKeys(ui->lineKeys->text(), keys, error))
        {
            QMessageBox::warning(this, tr("Warning"), tr("Key \"%1\" is not valid.").arg(error));
            return;
        }
        if (keys.isEmpty())
        {
            QMessageBox::warning(this, tr("Warning"), tr("Please specify the keys to create."));
            return;
        }
    }
    emit this->validated();
}
