/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "dialog_help.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>

DialogHelp::DialogHelp(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint |
            Qt::WindowSystemMenuHint | Qt::Window),
    _valTmp(0)
{
    // Création de l'interface et remplissage de l'arborescence
    createUi();
    fillTree();

    onTreeClicked(_tree->topLevelItem(0), NULL);
    updateNextPreviousStates();
}

DialogHelp::~DialogHelp()
{
}

void DialogHelp::createUi()
{
    // Layout
    QHBoxLayout * layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setContentsMargins(3, 3, 3, 3);

    // Splitter
    _splitter = new QSplitter(this);
    layout->addWidget(_splitter);

    // TreeWidget
    _tree = new QTreeWidget();
    _splitter->addWidget(_tree);
    _tree->header()->hide();
    _tree->setColumnCount(1);

    // TextBrowser avec titre et boutons de navigation
    QWidget * widget = new QWidget();
    _splitter->addWidget(widget);
    QVBoxLayout * layout2 = new QVBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout2);
    QHBoxLayout * layout3 = new QHBoxLayout();
    _buttonLeft = new QPushButton();
    _buttonLeft->setIcon(QIcon(":/icones/arrow_left"));
    _buttonLeft->setMaximumSize(24, 24);
    _buttonLeft->setFlat(true);
    _buttonLeft->setFocusPolicy(Qt::NoFocus);
    layout3->addWidget(_buttonLeft);
    _label = new QLabel();
    QFont font = _label->font();
    font.setBold(true);
    _label->setFont(font);
    _label->setAlignment(Qt::AlignCenter);
    layout3->addWidget(_label);
    _buttonRight = new QPushButton();
    _buttonRight->setIcon(QIcon(":/icones/arrow_right"));
    _buttonRight->setMaximumSize(24, 24);
    _buttonRight->setFlat(true);
    _buttonRight->setFocusPolicy(Qt::NoFocus);
    layout3->addWidget(_buttonRight);
    layout2->addLayout(layout3);
    _textBrowser = new QTextBrowser();
    _textBrowser->setFont(this->font());
    layout2->addWidget(_textBrowser);
    _textBrowser->setOpenExternalLinks(true);

    // Connexions
    connect(_tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onTreeClicked(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(_buttonLeft, SIGNAL(clicked()), this, SLOT(onPreviousClicked()));
    connect(_buttonRight, SIGNAL(clicked()), this, SLOT(onNextClicked()));
    connect(_textBrowser, SIGNAL(sourceChanged(QUrl)), this, SLOT(onTextSourceChanged(QUrl)));
    connect(_textBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalSliderMoved(int)));

    // Redimensionnement et nom de la fenêtre
    this->resize(700, 400);
    this->setWindowTitle(trUtf8("Polyphone") + " - " + trUtf8("aide"));

    // Boutons de la fenêtre
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void DialogHelp::fillTree()
{
    QTreeWidgetItem * level1, * level2;
    createItem(trUtf8("Présentation du logiciel"), "0");

    level1 = createItem(trUtf8("Barre de menu"), "1");
    createItem(trUtf8("Fichier"), "1-0", level1);
    createItem(trUtf8("Édition"), "1-1", level1);
    createItem(trUtf8("Outils"), "1-2", level1);
    createItem(trUtf8("Affichage"), "1-3", level1);
    createItem(trUtf8("Aide"), "1-4", level1);

    level1 = createItem(trUtf8("Barre d'outils"), "2");
    createItem(trUtf8("Raccourcis"), "2-0", level1);
    createItem(trUtf8("Clavier virtuel"), "2-1", level1);

    level1 = createItem(trUtf8("Arborescence"), "3");
    createItem(trUtf8("Structure"), "3-0", level1);
    createItem(trUtf8("Menu contextuel"), "3-1", level1);
    createItem(trUtf8("Glisser / déposer"), "3-2", level1);
    createItem(trUtf8("Recherche"), "3-3", level1);

    level1 = createItem(trUtf8("Pages d'édition"), "4");
    level2 = createItem(trUtf8("Informations générales"), "4-0", level1);
    createItem(trUtf8("Zone d'édition"), "4-0-0", level2);
    createItem(trUtf8("Zone d'informations"), "4-0-1", level2);
    level2 = createItem(trUtf8("Échantillons"), "4-1", level1);
    createItem(trUtf8("Graphique"), "4-1-0", level2);
    createItem(trUtf8("Informations"), "4-1-1", level2);
    createItem(trUtf8("Fréquences"), "4-1-2", level2);
    createItem(trUtf8("Égaliseur"), "4-1-3", level2);
    createItem(trUtf8("Lecteur"), "4-1-4", level2);
    level2 = createItem(trUtf8("Instruments"), "4-2", level1);
    createItem(trUtf8("Tableau"), "4-2-0", level2);
    createItem(trUtf8("Éditeur d'étendues"), "4-2-2", level2);
    createItem(trUtf8("Section modulateur"), "4-2-1", level2);
    createItem(trUtf8("Presets"), "4-3", level1);

    level1 = createItem(trUtf8("Pages d'aperçu"), "9");
    createItem(trUtf8("Échantillons"), "9-0", level1);
    createItem(trUtf8("Instruments"), "9-1", level1);
    createItem(trUtf8("Presets"), "9-2", level1);

    level1 = createItem(trUtf8("Outils"), "5");
    level2 = createItem(trUtf8("Échantillons"), "5-0", level1);
    createItem(trUtf8("Ajuster à la fin de boucle"), "5-0-0", level2);
    createItem(trUtf8("Bouclage automatique"), "5-0-1", level2);
    createItem(trUtf8("Diminuer sifflements"), "5-0-2", level2);
    createItem(trUtf8("Enlever blanc au départ"), "5-0-3", level2);
    createItem(trUtf8("Filtre \"mur de brique\""), "5-0-4", level2);
    createItem(trUtf8("Normaliser volume"), "5-0-5", level2);
    createItem(trUtf8("Réglage balance"), "5-0-6", level2);
    createItem(trUtf8("Transposer"), "5-0-7", level2);

    level2 = createItem(trUtf8("Instruments"), "5-1", level1);
    createItem(trUtf8("Création mixture"), "5-1-0", level2);
    createItem(trUtf8("Accordage céleste"), "5-1-1", level2);
    createItem(trUtf8("Duplication des divisions"), "5-1-2", level2);
    createItem(trUtf8("Élaboration release"), "5-1-3", level2);
    createItem(trUtf8("Paramétrage global"), "5-1-4", level2);
    createItem(trUtf8("Répartition automatique"), "5-1-5", level2);
    createItem(trUtf8("Spatialisation du son"), "5-1-6", level2);
    createItem(trUtf8("Transposition"), "5-1-8", level2);
    createItem(trUtf8("Visualiseur"), "5-1-7", level2);

    level2 = createItem(trUtf8("Presets"), "5-2", level1);
    createItem(trUtf8("Duplication des divisions"), "5-2-0", level2);
    createItem(trUtf8("Paramétrage global"), "5-2-1", level2);
    createItem(trUtf8("Spatialisation du son"), "5-2-2", level2);
    createItem(trUtf8("Visualiseur"), "5-2-3", level2);

    level2 = createItem(trUtf8("Globaux"), "5-3", level1);
    createItem(trUtf8("Association auto échantillons"), "5-3-0", level2);
    createItem(trUtf8("Dissocier les échantillons stéréo"), "5-3-1", level2);
    createItem(trUtf8("Enlever les éléments non utilisés"), "5-3-2", level2);
    createItem(trUtf8("Enlever tous les modulateurs"), "5-3-5", level2);
    createItem(trUtf8("Exporter pics de fréquence (.csv)"), "5-3-4", level2);
    createItem(trUtf8("Régler atténuation minimale"), "5-3-3", level2);

    createItem(trUtf8("Magnétophone"), "5-4", level1);

    level1 = createItem(trUtf8("Préférences du logiciel"), "6");
    createItem(trUtf8("Général"), "6-0", level1);
    createItem(trUtf8("Synthétiseur"), "6-1", level1);
    createItem(trUtf8("Échantillons"), "6-2", level1);
    createItem(trUtf8("Barre d'outils"), "6-3", level1);
    createItem(trUtf8("Clavier"), "6-4", level1);

    level1 = createItem(trUtf8("Annexes"), "7");
    level2 = createItem(trUtf8("Tutoriel"), "8", level1);
    createItem(trUtf8("Préparation des échantillons"), "8-0", level2);
    createItem(trUtf8("Création d'un instrument"), "8-1", level2);
    createItem(trUtf8("Création d'un preset"), "8-2", level2);
    createItem(trUtf8("Soundfonts sf2"), "7-0", level1);
    createItem(trUtf8("Soundfonts sf3"), "7-3", level1);
    createItem(trUtf8("Import / export sfz"), "7-1", level1);
    createItem(trUtf8("Limitations du logiciel"), "7-2", level1);

    // Redimensionnement de l'arborescence
    QList<int> sizes;
    sizes << 30 << this->width() - 30;
    _splitter->setSizes(sizes);
    _tree->resizeColumnToContents(0);
    sizes.clear();
    sizes << _tree->columnWidth(0) + 20 << this->width() - _tree->columnWidth(0) - 20;
    _splitter->setSizes(sizes);
    _splitter->setStretchFactor(1, 100);
}

QTreeWidgetItem * DialogHelp::createItem(QString name, QString htmlPage, QTreeWidgetItem * parent)
{
    QString extension = trUtf8("fr");
    QTreeWidgetItem * item = new QTreeWidgetItem(parent, QStringList(name));
    item->setData(0, Qt::UserRole, "qrc:/aide/" + htmlPage + extension + ".html");
    _tree->addTopLevelItem(item);
    return item;
}

void DialogHelp::onTreeClicked(QTreeWidgetItem* item, QTreeWidgetItem *)
{
    _textBrowser->setSource(QUrl(item->data(0, Qt::UserRole).toString()));
}

void DialogHelp::onTextSourceChanged(const QUrl &arg1, bool store)
{
    QString currentPage = arg1.toString();
    QTreeWidgetItem *itemToSelect = NULL;
    for (int i = 0; i < _tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = findItem(_tree->topLevelItem(i), currentPage);
        if (item)
        {
            itemToSelect = item;
            _label->setText(item->text(0));
        }
    }
    _tree->blockSignals(true);
    _tree->setCurrentItem(itemToSelect, 0);
    _tree->blockSignals(false);

    if (store)
    {
        _urlNext.clear();
        _posNext.clear();
        _urlPrevious.insert(0, currentPage);
        _posPrevious.insert(0, 0);
        while (_urlPrevious.size() > 51)
        {
            _urlPrevious.removeLast();
            _posPrevious.removeLast();
        }
        updateNextPreviousStates();
    }
}

QTreeWidgetItem * DialogHelp::findItem(QTreeWidgetItem * item, QString url)
{
    QTreeWidgetItem * itemRet = NULL;
    if (item->data(0, Qt::UserRole).toString() == url)
        itemRet = item;
    else
    {
        for (int i = 0; i < item->childCount(); i++)
        {
            QTreeWidgetItem * itemTmp = findItem(item->child(i), url);
            if (itemTmp)
                itemRet = itemTmp;
        }
    }
    return itemRet;
}

void DialogHelp::onPreviousClicked()
{
    if (_urlPrevious.size() > 1)
    {
        _posPrevious.first() = _textBrowser->verticalScrollBar()->value();
        _urlNext.insert(0, _urlPrevious.takeFirst());
        _posNext.insert(0, _posPrevious.takeFirst());
        _textBrowser->blockSignals(true);
        int value = _posPrevious.first();
        _textBrowser->setSource(QUrl(_urlPrevious.first()));
        _textBrowser->blockSignals(false);
        _textBrowser->verticalScrollBar()->setValue(value);
        onTextSourceChanged(_urlPrevious.first(), false);
        updateNextPreviousStates();
    }
}

void DialogHelp::onNextClicked()
{
    if (!_urlNext.isEmpty())
    {
        _posPrevious.first() = _textBrowser->verticalScrollBar()->value();
        _urlPrevious.insert(0, _urlNext.takeFirst());
        _posPrevious.insert(0, _posNext.takeFirst());
        _textBrowser->blockSignals(true);
        int value = _posPrevious.first();
        _textBrowser->setSource(QUrl(_urlPrevious.first()));
        _textBrowser->blockSignals(false);
        _textBrowser->verticalScrollBar()->setValue(value);
        onTextSourceChanged(_urlPrevious.first(), false);
        updateNextPreviousStates();
    }
}

void DialogHelp::updateNextPreviousStates()
{
    _buttonLeft->setEnabled(_urlPrevious.size() > 1);
    _buttonRight->setEnabled(!_urlNext.isEmpty());
}

void DialogHelp::verticalSliderMoved(int value)
{
    _posPrevious.first() = _valTmp;
    _valTmp = value;
}
