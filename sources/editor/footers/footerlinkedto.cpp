/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "footerlinkedto.h"
#include "ui_footerlinkedto.h"
#include "soundfontmanager.h"

FooterLinkedTo::FooterLinkedTo(QWidget *parent) :
    AbstractFooter(parent),
    ui(new Ui::FooterLinkedTo)
{
    ui->setupUi(this);
    connect(ui->widgetLinkedTo, SIGNAL(itemClicked(EltID)), this, SLOT(onLinkClicked(EltID)));
}

FooterLinkedTo::~FooterLinkedTo()
{
    delete ui;
}

void FooterLinkedTo::updateInterface()
{
    IdList ids = _currentIds.getSelectedIds(elementSmpl);
    bool isSmpl = true;
    if (ids.isEmpty())
    {
        ids = _currentIds.getSelectedIds(elementInst);
        if (ids.empty())
        {
            ui->widgetLinkedTo->initialize(EltID());
            ui->pushStereoEditing->hide();
            return;
        }
        isSmpl = false;
    }

    // Elements linked to the current elements
    if (ids.count() == 1)
    {
        ui->widgetLinkedTo->initialize(ids[0]);
        int nbElt = ui->widgetLinkedTo->getLinkNumber();
        if (isSmpl)
        {
            if (nbElt == 0)
                ui->labelLinkedTo->setText(tr("Sample not linked to an instrument yet."));
            else if (nbElt == 1)
                ui->labelLinkedTo->setText(tr("Sample linked to instrument:"));
            else
                ui->labelLinkedTo->setText(tr("Sample linked to instruments:"));
        }
        else
        {
            if (nbElt == 0)
                ui->labelLinkedTo->setText(tr("Instrument not linked to a preset yet."));
            else if (nbElt == 1)
                ui->labelLinkedTo->setText(tr("Instrument linked to preset:"));
            else
                ui->labelLinkedTo->setText(tr("Instrument linked to presets:"));
        }
    }
    else
    {
        ui->labelLinkedTo->setText("-");
        ui->widgetLinkedTo->initialize(EltID());
    }

    // Visibility of the stereo editing button
    if (isSmpl)
    {
        // Does the selection comprise at least one stereo sample?
        bool withLinkedSample = false;
        SoundfontManager * sf2 = SoundfontManager::getInstance();
        foreach (EltID id, ids)
        {
            SFSampleLink typeLink = sf2->get(id, champ_sfSampleType).sfLinkValue;
            if (typeLink != monoSample && typeLink != RomMonoSample && typeLink != linkInvalid)
            {
                withLinkedSample = true;
                break;
            }
        }
        ui->pushStereoEditing->setVisible(withLinkedSample);
    }
    else
    {
        if (ids.count() == 1)
        {
            // Does the selection within the instrument comprise at least one linked sample?
            bool withLinkedSample = false;
            EltID idSmpl = EltID(elementSmpl, ids[0].indexSf2);
            SoundfontManager * sf2 = SoundfontManager::getInstance();
            foreach (EltID id, _currentIds)
            {
                if (id.typeElement == elementInstSmpl)
                {
                    // Type of the associated sample
                    idSmpl.indexElt = sf2->get(id, champ_sampleID).wValue;
                    SFSampleLink linkType = sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;

                    if (linkType != linkInvalid  && linkType != monoSample && linkType != RomMonoSample)
                    {
                        // Check the linked sample
                        EltID idSmpl2 = idSmpl;
                        idSmpl2.indexElt = sf2->get(idSmpl, champ_wSampleLink).wValue;
                        if (sf2->isValid(idSmpl2))
                        {
                            withLinkedSample = true;
                            break;
                        }
                    }
                }
            }

            // Button visibility
            ui->pushStereoEditing->setVisible(withLinkedSample);
        }
        else
            ui->pushStereoEditing->hide();
    }
}

void FooterLinkedTo::onLinkClicked(EltID id)
{
    emit(selectedIdsChanged(id));
}
