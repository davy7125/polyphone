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

#include "pagesf2.h"
#include "ui_pagesf2.h"
#include "contextmanager.h"
#include <QDateTime>
#include <QMenu>
#include <QWidgetAction>
#include "latinvalidator.h"

PageSf2::PageSf2(QWidget * parent) :
    Page(parent, "page:sf2"),
    ui(new Ui::PageSf2)
{
    ui->setupUi(this);

    // Colors
    QColor highlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor border = ContextManager::theme()->getColor(ThemeManager::BORDER);
    QColor highlightedHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);

    // Style
    ui->frameSampleTitle->setStyleSheet("QFrame{border-radius: 2px;border:0;background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    ui->frameInstrumentTitle->setStyleSheet("QFrame{border-radius: 2px;border:0;background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    ui->framePresetTitle->setStyleSheet("QFrame{border-radius: 2px;border:0;background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    ui->pushViewSamples->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                       "color:" + highlightedText.name() + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                       "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushViewInstruments->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                           "color:" + highlightedText.name() + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                           "QPushButton:hover{background-color:" + highlightedHover.name() + "}");
    ui->pushViewPresets->setStyleSheet("QPushButton{border: 0; border-left: 1px solid " + border.name() + "; padding:0 5px 0 5px;" +
                                       "color:" + highlightedText.name() + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                       "QPushButton:hover{background-color:" + highlightedHover.name() + "}");

    // Icons
    ui->labelUser->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(18, 18), ThemeManager::WINDOW_TEXT));
    ui->labelDate->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(18, 18), ThemeManager::WINDOW_TEXT));
    ui->labelSample->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->labelInstrument->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->labelPreset->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));

    // Information button, opening a menu with the details of the soundfont
    ui->toolButtonInfo->setIcon(ContextManager::theme()->getColoredSvg(":/icons/info.svg", QSize(18, 18), ThemeManager::HIGHLIGHTED_BACKGROUND));
    QColor infoHover = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND), highlighted, 0.2);
    ui->toolButtonInfo->setStyleSheet("QToolButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}"
                                      "QToolButton{border:0;border-radius:2px;padding:3px}"
                                      "QToolButton:hover,QToolButton:pressed{background-color:" + infoHover.name() + "}");
    QMenu * menuInfo = new QMenu(ui->toolButtonInfo);
    menuInfo->setStyleSheet(ContextManager::theme()->getMenuTheme());
    _labelInfo = new QLabel();
    _labelInfo->setStyleSheet("QLabel{padding:10px 15px;color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "}");
    QWidgetAction * actionInfo = new QWidgetAction(menuInfo);
    actionInfo->setDefaultWidget(_labelInfo);
    menuInfo->addAction(actionInfo);
    ui->toolButtonInfo->setMenu(menuInfo);
    ui->toolButtonInfo->setLayoutDirection(Qt::RightToLeft); // The menu is then aligned on the right side of the button

    // Text validator
    ui->lineEdit_name->setValidator(new LatinValidator(ui->lineEdit_name));
    ui->lineEdit_author->setValidator(new LatinValidator(ui->lineEdit_author));
    ui->lineEdit_date->setValidator(new LatinValidator(ui->lineEdit_date));
    ui->lineEdit_product->setValidator(new LatinValidator(ui->lineEdit_product));
    ui->lineEdit_copyright->setValidator(new LatinValidator(ui->lineEdit_copyright));
}

PageSf2::~PageSf2()
{
    delete ui;
}

void PageSf2::updateInterface(QString editingSource)
{
    if (editingSource == _editingSource)
        return;

    // Count everything
    _currentID = _currentIds.getFirstId(elementSf2);
    this->countElements();

    // 24-bit samples?
    ui->comboBitDepth->setCurrentIndex(_sf2->get(_currentID, champ_wBpsSave).wValue == 24 ? 1 : 0);

    // Compression possible?
    ui->comboCompression->setCurrentIndex(_sf2->get(_currentID, champ_IFIL).sfVerValue.wMajor == 3 ? 1 : 0);

    ui->lineEdit_name->setTextToElide(_sf2->getQstr(_currentID, champ_name));
    ui->lineEdit_copyright->setText(_sf2->getQstr(_currentID, champ_ICOP));
    ui->lineEdit_author->setTextToElide(_sf2->getQstr(_currentID, champ_IENG));
    ui->lineEdit_date->setInitialText(_sf2->getQstr(_currentID, champ_ICRD));
    ui->lineEdit_product->setText(_sf2->getQstr(_currentID, champ_IPRD));
    ui->textEdit_Com->setPlainText(_sf2->getQstr(_currentID, champ_ICMT));

    updateInfo();
}

void PageSf2::setName()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_name->text().compare(_sf2->getQstr(_currentID, champ_name)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_name, ui->lineEdit_name->text());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::setCopyright()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_copyright->text().compare(_sf2->getQstr(_currentID, champ_ICOP)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_ICOP, ui->lineEdit_copyright->text());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::setAuthor()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_author->text().compare(_sf2->getQstr(_currentID, champ_IENG)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_IENG, ui->lineEdit_author->text());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::setDate()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_date->getInitialText().compare(_sf2->getQstr(_currentID, champ_ICRD)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_ICRD, ui->lineEdit_date->getInitialText());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::setProduct()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_product->text().compare(_sf2->getQstr(_currentID, champ_IPRD)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_IPRD, ui->lineEdit_product->text());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::setCommentaire()
{
    if (_preparingPage)
        return;

    // Max size of the comment
    if (ui->textEdit_Com->toPlainText().size() > 65536)
        ui->textEdit_Com->setPlainText(ui->textEdit_Com->toPlainText().left(65536));


    if (ui->textEdit_Com->toPlainText().compare(_sf2->getQstr(_currentID, champ_ICMT)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_ICMT, ui->textEdit_Com->toPlainText());
        _sf2->endEditing(_editingSource);
    }
}

void PageSf2::countElements()
{
    // Count des samples, instruments and presets that are used and unused
    int unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen, instMod, prstMod;
    this->countElements(unusedSmpl, unusedInst, usedSmpl, usedInst, usedPrst, instGen, prstGen, instMod, prstMod);

    // Colors
    QString greenStr = "QLabel { color : " + ContextManager::theme()->getFixedColor(ThemeManager::GREEN, ThemeManager::WINDOW_BACKGROUND).name() + "; }";
    QString redStr = "QLabel { color : " + ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::WINDOW_BACKGROUND).name() +
                     "; font-weight: bold; }";
    QString normalStr = "QLabel { color : " + ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() + "; font-weight: normal; }";

    // Display
    if (unusedSmpl)
    {
        ui->label_nbSmpl->setText(tr("%1 (unused: %n)", "", unusedSmpl).arg(usedSmpl));
        ui->label_nbSmpl->setStyleSheet(redStr);
    }
    else
    {
        ui->label_nbSmpl->setText(QString::number(usedSmpl));
        ui->label_nbSmpl->setStyleSheet(normalStr);
    }
    if (unusedInst)
    {
        ui->label_nbInst->setText(tr("%1 (unused: %n)", "", unusedInst).arg(usedInst));
        ui->label_nbInst->setStyleSheet(redStr);
    }
    else
    {
        ui->label_nbInst->setText(QString::number(usedInst));
        ui->label_nbInst->setStyleSheet(normalStr);
    }
    ui->label_nbPrst->setText(QString::number(usedPrst));

    if (instGen <= 65536)
    {
        ui->label_nbInstGen->setStyleSheet(greenStr);
        ui->label_nbInstGen->setText(QString::number(instGen) + QString::fromUtf8(" (≤ 65536)"));
    }
    else
    {
        ui->label_nbInstGen->setStyleSheet(redStr);
        ui->label_nbInstGen->setText(QString::number(instGen) + " (> 65536)");
    }
    if (prstGen <= 65536)
    {
        ui->label_nbPrstGen->setStyleSheet(greenStr);
        ui->label_nbPrstGen->setText(QString::number(prstGen) + QString::fromUtf8(" (≤ 65536)"));
    }
    else
    {
        ui->label_nbPrstGen->setStyleSheet(redStr);
        ui->label_nbPrstGen->setText(QString::number(prstGen) + " (> 65536)");
    }

    ui->label_nbInstMod->setText(QString::number(instMod));
    ui->label_nbPrstMod->setText(QString::number(prstMod));
}

void PageSf2::countElements(int &unusedSmpl, int &unusedInst, int &usedSmpl, int &usedInst, int &usedPrst,
                            int &instGen, int &prstGen, int &instMod, int &prstMod)
{
    // Etablissement de la liste des samples, instruments et presets
    // Préparation de la liste des samples et instruments utilisés
    EltID id = _currentID;
    id.typeElement = elementSmpl;
    QList<int> listSmpl = _sf2->getSiblings(id);
    QList<int> listUsedSmpl;

    id.typeElement = elementInst;
    QList<int> listInst = _sf2->getSiblings(id);
    QList<int> listUsedInst;

    id.typeElement = elementPrst;
    QList<int> listPrst = _sf2->getSiblings(id);

    // Nombre de presets
    usedPrst = listPrst.size();

    // Nombre de gen et mod dans les preset
    // En passant on met de côté les instruments utilisés
    prstGen = 0;
    prstMod = 0;
    int iTmp;
    foreach (int elementId, listPrst)
    {
        id.indexElt = elementId;
        id.typeElement = elementPrstGen;
        prstGen += _sf2->getSiblings(id).count();
        id.typeElement = elementPrstMod;
        prstMod += _sf2->getSiblings(id).count();

        id.typeElement = elementPrstInst;
        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexElt2 = i;
            id.typeElement = elementPrstInstGen;
            prstGen += _sf2->getSiblings(id).count();
            id.typeElement = elementPrstInstMod;
            prstMod += _sf2->getSiblings(id).count();

            id.typeElement = elementPrstInst;
            iTmp = _sf2->get(id, champ_instrument).wValue;
            if (listInst.contains(iTmp))
            {
                listInst.removeOne(iTmp);
                listUsedInst << iTmp;
            }
        }
    }

    // Nombre de gen et mod dans les instruments utilisés
    // En passant on met de côté les samples utilisés
    instGen = 0;
    instMod = 0;
    foreach (int elementId, listUsedInst)
    {
        id.indexElt = elementId;
        id.typeElement = elementInstGen;
        instGen += _sf2->getSiblings(id).count();
        id.typeElement = elementInstMod;
        instMod += _sf2->getSiblings(id).count();

        id.typeElement = elementInstSmpl;
        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexElt2 = i;
            id.typeElement = elementInstSmplGen;
            instGen += _sf2->getSiblings(id).count();
            id.typeElement = elementInstSmplMod;
            instMod += _sf2->getSiblings(id).count();

            id.typeElement = elementInstSmpl;
            iTmp = _sf2->get(id, champ_sampleID).wValue;
            if (listSmpl.contains(iTmp))
            {
                listSmpl.removeOne(iTmp);
                listUsedSmpl << iTmp;
            }
        }
    }

    // Nombre de Gen dans les instruments non utilisés
    foreach (int elementId, listInst)
    {
        id.indexElt = elementId;
        id.typeElement = elementInstGen;
        instGen += _sf2->getSiblings(id).count();

        id.typeElement = elementInstSmpl;
        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexElt2 = i;
            id.typeElement = elementInstSmplGen;
            instGen += _sf2->getSiblings(id).count();
            id.typeElement = elementInstSmpl;
        }
    }

    // Nombre de samples et instruments utilisés / non utilisés
    unusedSmpl = listSmpl.size();
    unusedInst = listInst.size();
    usedSmpl = listUsedSmpl.size() + listSmpl.size();
    usedInst = listUsedInst.size() + listInst.size();
}

void PageSf2::on_pushViewSamples_clicked()
{
    emit selectedIdsChanged(EltID(elementRootSmpl, _currentID.indexSf2));
}

void PageSf2::on_pushViewInstruments_clicked()
{
    emit selectedIdsChanged(EltID(elementRootInst, _currentID.indexSf2));
}

void PageSf2::on_pushViewPresets_clicked()
{
    emit selectedIdsChanged(EltID(elementRootPrst, _currentID.indexSf2));
}

void PageSf2::on_comboBitDepth_currentIndexChanged(int index)
{
    if (_preparingPage)
        return;

    AttributeValue valTmp;
    switch (index)
    {
    case 0:
        valTmp.wValue = 16;
        _sf2->set(_currentID, champ_wBpsSave, valTmp);
        break;
    case 1:
        valTmp.wValue = 24;
        _sf2->set(_currentID, champ_wBpsSave, valTmp);

        // Soundfont version is 2.04, no possible compression
        if (100 * _sf2->get(_currentID, champ_IFIL).sfVerValue.wMajor + _sf2->get(_currentID, champ_IFIL).sfVerValue.wMinor != 204)
        {
            valTmp.sfVerValue.wMajor = 2;
            valTmp.sfVerValue.wMinor = 4;
            _sf2->set(_currentID, champ_IFIL, valTmp);

            ui->comboCompression->blockSignals(true);
            ui->comboCompression->setCurrentIndex(0);
            ui->comboCompression->blockSignals(false);
            updateInfo();
        }
        break;
    default:
        return;
    }
    _sf2->endEditing(_editingSource);
}

void PageSf2::on_comboCompression_currentIndexChanged(int index)
{
    if (_preparingPage)
        return;

    AttributeValue valTmp;
    switch (index)
    {
    case 0:
        // No compression: back to soundfont version 2.04
        valTmp.sfVerValue.wMajor = 2;
        valTmp.sfVerValue.wMinor = 4;
        _sf2->set(_currentID, champ_IFIL, valTmp);
        break;
    case 1:
        // Compression is possible in soundfont version 3.00
        valTmp.sfVerValue.wMajor = 3;
        valTmp.sfVerValue.wMinor = 0;
        _sf2->set(_currentID, champ_IFIL, valTmp);

        // Bit depth is 16 with this soundfont version
        if (_sf2->get(_currentID, champ_wBpsSave).wValue != 16)
        {
            valTmp.wValue = 16;
            _sf2->set(_currentID, champ_wBpsSave, valTmp);
            ui->comboBitDepth->blockSignals(true);
            ui->comboBitDepth->setCurrentIndex(0);
            ui->comboBitDepth->blockSignals(false);
        }
        break;
    default:
        return;
    }
    _sf2->endEditing(_editingSource);
    updateInfo();
}

void PageSf2::updateInfo()
{
    QString labelStyle = "padding-right:15px;color:" + ThemeManager::mix(
                             ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                             ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.4).name();
    QString rowPattern = "<tr><td style=\"" + labelStyle + "\">%1</td><td>%2</td></tr>";

    // SoundFont version
    QString tmp = QString("%1.%2")
                      .arg(_sf2->get(_currentID, champ_IFIL).sfVerValue.wMajor)
                      .arg(_sf2->get(_currentID, champ_IFIL).sfVerValue.wMinor, 2, 10, QChar('0'));
    QString info = "<table>" + rowPattern.arg(tr("SoundFont format"), tmp);

    // ROM name / version
    tmp = _sf2->getQstr(_currentID, champ_IROM);
    if (!tmp.isEmpty())
        info += rowPattern.arg(tr("ROM name"), tmp.toHtmlEscaped());
    AttributeValue valTmp = _sf2->get(_currentID, champ_IVER);
    if (valTmp.sfVerValue.wMinor + valTmp.sfVerValue.wMajor > 0)
    {
        QString tmp = QString("%1.%2")
            .arg(valTmp.sfVerValue.wMajor)
            .arg(valTmp.sfVerValue.wMinor, 2, 10, QChar('0'));
        info += rowPattern.arg(tr("ROM version"), tmp);
    }

    // Sound engine
    tmp = _sf2->getQstr(_currentID, champ_ISNG);
    if (!tmp.isEmpty())
        info += rowPattern.arg(tr("Sound engine"), tmp.toHtmlEscaped());

    // Editing software
    tmp = _sf2->getQstr(_currentID, champ_ISFT);
    if (!tmp.isEmpty())
        info += rowPattern.arg(tr("Editing software"), tmp.toHtmlEscaped());

    info += "</table>";
    _labelInfo->setText(info);
}
