/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "pagesf2.h"
#include "ui_pagesf2.h"
#include "contextmanager.h"
#include <QDateTime>

PageSf2::PageSf2(QWidget * parent) :
    Page(parent, PAGE_SF2, "page:sf2"),
    ui(new Ui::PageSf2)
{
    ui->setupUi(this);

    // Colors
    QColor highlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor border = this->palette().dark().color();
    QColor highlightedHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" + highlighted.name() + ";color:" + highlightedText.name() + "}");
    connect(ui->lineEdit_date, SIGNAL(focussed(bool)), this, SLOT(dateFocussed(bool)), Qt::QueuedConnection);

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

    // Button "set now" hidden by default
    ui->pushButton_setNow->hide();
}

PageSf2::~PageSf2()
{
    delete ui;
}

bool PageSf2::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    Q_UNUSED(displayOption)

    if (editingSource == getEditingSource())
        return true;

    // Check the selection
    if (!selectedIds.isElementUnique(elementSf2))
        return false;
    _currentID = selectedIds.getFirstId(elementSf2);

    // Count everything
    this->countElements();

    // Mode 24 bits ?
    ui->comboBox->setCurrentIndex(_sf2->get(_currentID, champ_wBpsSave).wValue == 24 ? 1 : 0);

    // Informations (file name and size)
    QString txt = _sf2->getQstr(_currentID, champ_filenameInitial);
    if (!txt.isEmpty())
    {
        QFile file(txt);
        if (file.open(QIODevice::ReadOnly)) {
            int size = file.size();
            file.close();

            if (size > 1073741824)
            {
                // GB
                txt += QString(" (%1 %2)").arg((double)size / 1073741824, 3, 'f', 2).arg(trUtf8("GB", "giga byte"));
            }
            else if (size > 1048576)
            {
                // MB
                txt += QString(" (%1 %2)").arg((double)size / 1048576, 3, 'f', 2).arg(trUtf8("MB", "mega byte"));
            }
            else
            {
                // kB
                txt += QString(" (%1 %2)").arg((double)size / 1024, 3, 'f', 2).arg(trUtf8("kB", "kilo byte"));
            }
        }
    }

    ui->label_filename->setText(txt);
    ui->label_sfVersion->setText(QString("%1.%2")
                                 .arg(_sf2->get(_currentID, champ_IFIL).sfVerValue.wMajor)
                                 .arg(_sf2->get(_currentID, champ_IFIL).sfVerValue.wMinor, 2, 10, QChar('0')));
    ui->label_soundEngine->setText(_sf2->getQstr(_currentID, champ_ISNG));
    txt = QString("%1.%2")
            .arg(_sf2->get(_currentID, champ_IVER).sfVerValue.wMajor)
            .arg(_sf2->get(_currentID, champ_IVER).sfVerValue.wMinor, 2, 10, QChar('0'));
    if (_sf2->getQstr(_currentID, champ_IROM).isEmpty())
        ui->label_romVersion->setText("- (version " + txt + ")");
    else
        ui->label_romVersion->setText(_sf2->getQstr(_currentID, champ_IROM) + " (version " + txt + ")");
    ui->label_software->setText(_sf2->getQstr(_currentID, champ_ISFT));
    ui->lineEdit_name->setText(_sf2->getQstr(_currentID, champ_name));
    ui->lineEdit_copyright->setText(_sf2->getQstr(_currentID, champ_ICOP));
    ui->lineEdit_author->setText(_sf2->getQstr(_currentID, champ_IENG));
    ui->lineEdit_date->setText(_sf2->getQstr(_currentID, champ_ICRD));
    ui->lineEdit_product->setText(_sf2->getQstr(_currentID, champ_IPRD));
    ui->textEdit_Com->setPlainText(_sf2->getQstr(_currentID, champ_ICMT));

    return true;
}

void PageSf2::setName()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_name->text().compare(_sf2->getQstr(_currentID, champ_name)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_name, ui->lineEdit_name->text());
        _sf2->endEditing(getEditingSource());
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
        _sf2->endEditing(getEditingSource());
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
        _sf2->endEditing(getEditingSource());
    }
}

void PageSf2::setDate()
{
    if (_preparingPage)
        return;

    if (ui->lineEdit_date->text().compare(_sf2->getQstr(_currentID, champ_ICRD)) != 0)
    {
        // Soundfont editing
        _sf2->set(_currentID, champ_ICRD, ui->lineEdit_date->text());
        _sf2->endEditing(getEditingSource());
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
        _sf2->endEditing(getEditingSource());
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
        _sf2->endEditing(getEditingSource());
    }
}

void PageSf2::on_pushButton_setNow_pressed()
{
    QString date = QDateTime::currentDateTime().toString("dddd d MMMM yyyy, hh:mm:ss");
    ui->lineEdit_date->setText(date);
    setDate();
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

    // Display
    if (unusedSmpl)
    {
        if (unusedSmpl > 1)
            ui->label_nbSmpl->setText(trUtf8("%1 (unused: %2)", "plural form").arg(usedSmpl).arg(unusedSmpl));
        else
            ui->label_nbSmpl->setText(trUtf8("%1 (unused: %2)", "singular form").arg(usedSmpl).arg(unusedSmpl));
        ui->label_nbSmpl->setStyleSheet(redStr);
    }
    else
    {
        ui->label_nbSmpl->setText(QString::number(usedSmpl));
        ui->label_nbSmpl->setStyleSheet("");
    }
    if (unusedInst)
    {
        if (unusedInst > 1)
            ui->label_nbInst->setText(trUtf8("%1 (unused: %2)", "plural form").arg(usedInst).arg(unusedInst));
        else
            ui->label_nbInst->setText(trUtf8("%1 (unused: %2)", "singular form").arg(usedInst).arg(unusedInst));
        ui->label_nbInst->setStyleSheet(redStr);
    }
    else
    {
        ui->label_nbInst->setText(QString::number(usedInst));
        ui->label_nbInst->setStyleSheet("");
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

void PageSf2::dateFocussed(bool hasFocus)
{
    QApplication::processEvents();
    ui->pushButton_setNow->setVisible(hasFocus);
}

void PageSf2::on_pushViewSamples_clicked()
{
    emit(selectedIdsChanged(EltID(elementRootSmpl, _currentID.indexSf2)));
}

void PageSf2::on_pushViewInstruments_clicked()
{
    emit(selectedIdsChanged(EltID(elementRootInst, _currentID.indexSf2)));
}

void PageSf2::on_pushViewPresets_clicked()
{
    emit(selectedIdsChanged(EltID(elementRootPrst, _currentID.indexSf2)));
}

void PageSf2::on_comboBox_currentIndexChanged(int index)
{
    if (_preparingPage)
        return;

    // Soundfont editing
    AttributeValue valTmp;
    valTmp.wValue = (index == 1 ? 24 : 16);
    _sf2->set(_currentID, champ_wBpsSave, valTmp);
    _sf2->endEditing(getEditingSource());
}

void PageSf2::onShow()
{
    // Nothing special
}
