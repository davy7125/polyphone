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

#include "dialog_space.h"
#include "ui_dialog_space.h"
#include "config.h"

// Constructeur, destructeur
DialogSpace::DialogSpace(bool isPrst, int noteMin, int noteMax, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSpace),
    _isPrst(isPrst),
    _noteMin(noteMin),
    _noteMax(noteMax)
{
    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);
    Config * conf = Config::getInstance();
    ui->comboMotif->blockSignals(true);
    ui->comboMotif->setCurrentIndex(conf->getTools_i_space_motif(isPrst));
    ui->comboMotif->blockSignals(false);
    ui->spinNbDivision->blockSignals(true);
    ui->spinNbDivision->setValue(conf->getTools_i_space_divisions(isPrst));
    ui->spinNbDivision->blockSignals(false);
    ui->spinEtalement->blockSignals(true);
    ui->spinEtalement->setValue(conf->getTools_i_space_etalement(isPrst));
    ui->spinEtalement->blockSignals(false);
    ui->spinOccupation->blockSignals(true);
    ui->spinOccupation->setValue(conf->getTools_i_space_occupation(isPrst));
    ui->spinOccupation->blockSignals(false);
    ui->spinOffset->blockSignals(true);
    ui->spinOffset->setValue(conf->getTools_i_space_offset(isPrst));
    ui->spinOffset->blockSignals(false);
    ui->checkSens->blockSignals(true);
    ui->checkSens->setChecked(conf->getTools_i_space_renversement1(isPrst));
    ui->checkSens->blockSignals(false);
    ui->checkSens2->blockSignals(true);
    ui->checkSens2->setChecked(conf->getTools_i_space_renversement2(isPrst));
    ui->checkSens2->blockSignals(false);

    // Activation des renversements et remplissage du graphique
    ui->spinOffset->setEnabled(ui->spinEtalement->value() != 100);
    motifChanged(ui->comboMotif->currentIndex());
}
DialogSpace::~DialogSpace()
{
    delete ui;
}

void DialogSpace::accept()
{
    Config * conf = Config::getInstance();
    conf->setTools_space_motif(_isPrst, ui->comboMotif->currentIndex());
    conf->setTools_space_divisions(_isPrst, ui->spinNbDivision->value());
    conf->setTools_space_etalement(_isPrst, ui->spinEtalement->value());
    conf->setTools_space_occupation(_isPrst, ui->spinOccupation->value());
    conf->setTools_space_offset(_isPrst, ui->spinOffset->value());
    conf->setTools_space_renversement1(_isPrst, ui->checkSens->isChecked());
    conf->setTools_space_renversement2(_isPrst, ui->checkSens2->isChecked());

    QVector<double> x;
    QVector<int> y;
    computeData(x, y);

    QMap<int, double> mapPan;
    for (int i = 0; i < x.size(); i++)
        mapPan.insert(y.at(i), x.at(i));
    emit(accepted(mapPan));

    QDialog::accept();
}

void DialogSpace::motifChanged(int value)
{
    Q_UNUSED(value);
    updateRenversements();
    updateGraph();
}
void DialogSpace::nbDivChanged(int value)
{
    Q_UNUSED(value);
    updateRenversements();
    updateGraph();
}
void DialogSpace::etalementChanged(int value)
{
    ui->spinOffset->setEnabled(value != 100);
    Q_UNUSED(value);
    updateGraph();
}
void DialogSpace::occupationChanged(int value)
{
    Q_UNUSED(value);
    updateGraph();
}
void DialogSpace::offsetChanged(int value)
{
    Q_UNUSED(value);
    updateGraph();
}
void DialogSpace::sensChanged(int value)
{
    Q_UNUSED(value);
    updateGraph();
}
void DialogSpace::sens2Changed(int value)
{
    Q_UNUSED(value);
    updateGraph();
}

void DialogSpace::updateRenversements()
{
    int nbDivisions = ui->spinNbDivision->value();
    switch (ui->comboMotif->currentIndex())
    {
    case 0: case 1:
        ui->checkSens->setEnabled(nbDivisions > 1);
        ui->checkSens2->setEnabled(false);
        break;
    case 2: case 3:
        ui->checkSens->setEnabled(nbDivisions > 1);
        ui->checkSens2->setEnabled(true);
        break;
    default:
        ui->checkSens->setEnabled(false);
        ui->checkSens2->setEnabled(false);
        break;
    }
}

void DialogSpace::updateGraph()
{
    QVector<double> x;
    QVector<int> y;
    computeData(x, y);
    ui->grapheSpace->setData(x, y);
}

void DialogSpace::computeData(QVector<double> &x, QVector<int> &y)
{
    // Calcul du numéro de note en fonction du positionnement dans l'espace
    int nbNotes = _noteMax - _noteMin + 1;
    x.resize(nbNotes);
    y.resize(nbNotes);
    for (int i = 0; i < nbNotes; i++)
        y[i] = _noteMin + i;

    int motif = ui->comboMotif->currentIndex();
    switch (motif)
    {
    case 0: case 1:
        for (int i = 0; i < nbNotes; i++)
            x[i] = spaceLinear(i + _noteMin,
                               ui->spinNbDivision->value(),
                               ui->spinEtalement->value(),
                               ui->spinOccupation->value(),
                               ui->spinOffset->value(),
                               ui->checkSens->isChecked(),
                               motif == 0);
        break;
    case 2: case 3:
        for (int i = 0; i < nbNotes; i++)
            x[i] = spaceCurve(i + _noteMin,
                              ui->spinNbDivision->value(),
                              ui->spinEtalement->value(),
                              ui->spinOccupation->value(),
                              ui->spinOffset->value(),
                              ui->checkSens->isChecked(),
                              ui->checkSens2->isChecked(),
                              motif == 2);
        break;
    case 4:{
        // Mélange des notes
        int seed = 0;
        QList<int> listTmp;
        for (int i = 0; i < nbNotes; i++)
            listTmp << i;

        for (int i = 0; i < nbNotes; i++)
        {
            seed += QTime(0, 0).msecsTo(QTime::currentTime());
            qsrand(seed);
            x[listTmp.takeAt(qrand() % listTmp.size())] = spaceLinear(i + _noteMin,
                               ui->spinNbDivision->value(),
                               ui->spinEtalement->value(),
                               ui->spinOccupation->value(),
                               ui->spinOffset->value(),
                               ui->checkSens->isChecked(),
                               motif == 0);
        }
        }break;
    }
}

double DialogSpace::spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending)
{
    // Ajustement note
    int noteRelative = 0;
    if (isAscending)
        noteRelative = _noteMax - note;
    else
        noteRelative = note - _noteMin;

    // Détermination de l'axe autour duquel se trouve la note
    double posAxe = getAxe(noteRelative, nbDiv, sens);

    // Calcul de l'écart à l'axe
    double e = 1. / (_noteMax - _noteMin + nbDiv - 1); // écart unitaire
    double ne_axe = (((double)(_noteMax - _noteMin) / 2) - noteRelative) / nbDiv;
    double ecart_axe = e * ne_axe;


    // Prise en compte de l'occupation
    double mult = (double)(nbDiv) / (nbDiv - 1 + (double)occupation / 100); // dilatation future
    ecart_axe *= (double)occupation / 100;

    // Synthèse
    return (double)etalement / 100        * ((double)mult * (posAxe + ecart_axe - 0.5) + 0.5) +
           (1. - (double)etalement / 100) * (double)offset / 100;
}

double DialogSpace::spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow)
{
    // Ajustement note
    int noteRelative = 0;
    if (isHollow)
    {
        // Calcul décalage
        int resteNotes = (_noteMax - _noteMin + 1) % (2 * nbDiv);
        int decalage = 0;
        if (resteNotes < nbDiv)
            decalage = nbDiv - resteNotes;
        else if (resteNotes > nbDiv)
            decalage = 3 * nbDiv - resteNotes;
        noteRelative = _noteMax - note + decalage;
    }
    else
        noteRelative = note - _noteMin;

    // Détermination de l'axe autour duquel se trouve la note
    double posAxe = getAxe(noteRelative, nbDiv, sens);

    // Calcul de l'écart à l'axe
    double e = 1.0 / (ceil((double)(_noteMax - _noteMin + 1 - nbDiv) / (2 * nbDiv)) * (2 * nbDiv) + nbDiv); // écart unitaire
    int ne_axe = (noteRelative + nbDiv) / (2 * nbDiv);
    double ecart_axe = e * ne_axe;

    // Prise en compte de l'occupation
    double mult = 1; // dilation future
    if (isHollow)
        ecart_axe = (double)(ecart_axe * occupation + (double)(100 - occupation) / (2*nbDiv)) / 100;
    else
    {
        ecart_axe *= (double)occupation / 100;
        mult = (double)(nbDiv) / (nbDiv - 1 + (double)occupation / 100);
    }

    // Côté de l'axe
    int cote = noteRelative % (2 * nbDiv);
    if (cote >= nbDiv)
        ecart_axe = -ecart_axe;
    if (sens2 != 0) ecart_axe = -ecart_axe;

    // Synthèse
    return (double)etalement / 100        * ((double)mult * (posAxe + ecart_axe - 0.5) + 0.5) +
           (1. - (double)etalement / 100) * (double)offset / 100;
}

double DialogSpace::getAxe(int note, int nbDiv, bool sens)
{
    int axe = note % nbDiv;
    if (axe % 2 == 0)
        axe = axe / 2;
    else
        axe = nbDiv - (axe + 1) / 2;
    if (sens)
        axe = nbDiv - 1 - axe;
    return (double)(2 * axe + 1) / (2 * nbDiv);
}


// Graphe space

GraphSpace::GraphSpace(QWidget * parent) : QCustomPlot(parent)
{
    // Couche déco
    this->addGraph();
    QPen graphPen;
    graphPen.setWidthF(1);
    graphPen.setColor(QColor(0, 0, 0, 40));
    graphPen.setStyle(Qt::DashDotLine);
    graph(0)->setPen(graphPen);
    QVector<double> x, y;
    x << 0.5 << 0.5;
    y << -10 << 10;
    graph(0)->setData(x, y);

    // Couche données
    this->addGraph();
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidthF(2);
    graphPen.setStyle(Qt::SolidLine);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    this->graph(1)->setAntialiased(true);

    // Axes
    this->xAxis->setRange(-0.01, 1.01);
    this->yAxis->setRange(0, 1.05);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Ajout G / D
    QCPItemText * text = new QCPItemText(this);
    text->position->setType(QCPItemPosition::ptAxisRectRatio);
    text->position->setCoords(0, 0);
    text->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    text->setTextAlignment(Qt::AlignLeft);
    text->setFont(QFont(font().family(), 10, 100));
    text->setColor(QColor(0, 0, 0, 75));
    text->setText(trUtf8("G"));
    this->addItem(text);
    text = new QCPItemText(this);
    text->position->setType(QCPItemPosition::ptAxisRectRatio);
    text->position->setCoords(1, 0);
    text->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);
    text->setTextAlignment(Qt::AlignRight);
    text->setFont(QFont(font().family(), 10, 100));
    text->setColor(QColor(0, 0, 0, 75));
    text->setText(trUtf8("D"));
    this->addItem(text);

    // Layer aperçu valeurs
    this->addGraph();
    graphPen.setColor(QColor(0, 0, 0));
    graphPen.setWidth(1);
    this->graph(2)->setPen(graphPen);
    this->graph(2)->setScatterStyle(QCPScatterStyle::ssPlus);
    labelCoord = new QCPItemText(this);
    this->addItem(labelCoord);
    labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    labelCoord->setText("");
    QFont fontLabel = QFont(font().family(), 9);
    fontLabel.setBold(true);
    labelCoord->setFont(fontLabel);
    labelCoord->setColor(QColor(0, 0, 0));

    // Filtre sur les événements
    this->installEventFilter(this);
}

void GraphSpace::setData(QVector<double> x, QVector<int> y)
{
    _xPan = x;
    _yKey = y;

    // Longueur max tuyau
    _yLength.resize(_yKey.size());
    double freq = qPow(2., (_yKey.first() + 36.3763) / 12);
    double l = 172 / freq; // en mètres pour un tuyau ouvert
    for (int i = 0; i < _yKey.size(); i++)
    {
        // Longueur tuyau
        freq = qPow(2., (_yKey.at(i) + 36.3763) / 12);
        _yLength[i] = 172 / freq / l;
    }

    // Affichage de la courbe
    this->graph(1)->setData(_xPan, _yLength);
    this->replot();
}

void GraphSpace::mouseMoved(QPoint pos)
{
    // Conversion des coordonnées
    double x = xAxis->pixelToCoord(pos.x());
    double y = yAxis->pixelToCoord(pos.y());

    // Point le plus proche
    double distanceMin = -1;
    int posX = -1;
    for (int i = 0; i < _xPan.size(); i++)
    {
        double distanceTmp = 5 * qAbs(x - _xPan[i]) + qAbs(y - _yLength[i]);
        if (distanceMin == -1 || distanceTmp < distanceMin)
        {
            distanceMin = distanceTmp;
            posX = i;
        }
    }
    if (posX != -1)
        this->afficheCoord(_xPan[posX], _yLength[posX], _yKey[posX]);
    else
        this->afficheCoord(-1, -1, -1);
}

void GraphSpace::mouseLeft()
{
    this->afficheCoord(-1, -1, -1);
}

void GraphSpace::afficheCoord(double xPan, double yLength, int key)
{
    QVector<double> xVector, yVector;
    if (key != -1)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = xPan;
        yVector[0] = yLength;

        // Affichage texte
        if (yLength >= 0.5)
            labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        char T[20];
        sprintf(T, "%.3d: %.1f", key, xPan * 100 - 50);
        labelCoord->setText(Config::getInstance()->getKeyName(key) + ":" +
                            QString::number(xPan * 100 - 50, 'f', 1));

        // Ajustement position
        QFontMetrics fm(labelCoord->font());
        double distX = xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 2);
        if (xPan < distX)
            xPan = distX;
        else if (xPan > 1. - distX)
            xPan = 1. - distX;
        double distY = 1.05 - this->yAxis->pixelToCoord(2);
        if (yLength >= 0.5)
            yLength -= distY;
        else
            yLength += distY;
        labelCoord->position->setCoords(xPan, yLength);
    }
    else
    {
        xVector.resize(0);
        yVector.resize(0);
        labelCoord->setText("");
    }
    this->graph(2)->setData(xVector, yVector);
    this->replot();
}
