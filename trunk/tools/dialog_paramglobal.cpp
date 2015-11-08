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

#include "dialog_paramglobal.h"
#include "ui_dialog_paramglobal.h"
#include "config.h"
#include "page.h"

// Constructeur, destructeur
DialogParamGlobal::DialogParamGlobal(Pile_sf2 *sf2, bool isPrst, QWidget *parent) :
    QDialog(parent),
    _sf2(sf2),
    ui(new Ui::DialogParamGlobal),
    _isPrst(isPrst)
{
    // Initialisation liste des champs
    QList<Champ> listeDesChamps;
    listeDesChamps << champ_initialAttenuation
                   << champ_pan;
    if (!_isPrst)
        listeDesChamps << champ_overridingRootKey;
    listeDesChamps << champ_coarseTune
                   << champ_fineTune
                   << champ_scaleTuning
                   << champ_initialFilterFc
                   << champ_initialFilterQ
                   << champ_delayVolEnv
                   << champ_attackVolEnv
                   << champ_holdVolEnv
                   << champ_decayVolEnv
                   << champ_sustainVolEnv
                   << champ_releaseVolEnv
                   << champ_keynumToVolEnvHold
                   << champ_keynumToVolEnvDecay
                   << champ_delayModEnv
                   << champ_attackModEnv
                   << champ_holdModEnv
                   << champ_decayModEnv
                   << champ_sustainModEnv
                   << champ_releaseModEnv
                   << champ_modEnvToPitch
                   << champ_modEnvToFilterFc
                   << champ_keynumToModEnvHold
                   << champ_keynumToModEnvDecay
                   << champ_delayModLFO
                   << champ_freqModLFO
                   << champ_modLfoToPitch
                   << champ_modLfoToFilterFc
                   << champ_modLfoToVolume
                   << champ_delayVibLFO
                   << champ_freqVibLFO
                   << champ_vibLfoToPitch
                   << champ_chorusEffectsSend
                   << champ_reverbEffectsSend;

    ui->setupUi(this);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint) | Qt::Tool);
    Config * conf = Config::getInstance();
    ui->comboValeur->blockSignals(true);
    for (int i = 0; i < listeDesChamps.size(); i++)
    {
        ui->comboValeur->addItem(Page::getGenName(listeDesChamps.at(i), 1 + _isPrst));
        ui->comboValeur->setItemData(i, (int)listeDesChamps.at(i));
    }
    ui->comboValeur->setCurrentIndex(conf->getTools_global_parametre(_isPrst));
    ui->comboValeur->blockSignals(false);
    ui->comboMotif->blockSignals(true);
    ui->comboMotif->setCurrentIndex(conf->getTools_global_motif(_isPrst));
    ui->comboMotif->blockSignals(false);
    ui->doubleSpinRaideur->blockSignals(true);
    ui->doubleSpinRaideur->setValue(conf->getTools_global_raideur(_isPrst));
    ui->doubleSpinRaideur->blockSignals(false);
    ui->doubleSpinMin->blockSignals(true);
    ui->doubleSpinMin->setValue(conf->getTools_global_mini(_isPrst));
    ui->doubleSpinMin->blockSignals(false);
    ui->doubleSpinMax->blockSignals(true);
    ui->doubleSpinMax->setValue(conf->getTools_global_maxi(_isPrst));
    ui->doubleSpinMax->blockSignals(false);
    ui->comboModif->blockSignals(true);
    ui->comboModif->setCurrentIndex(conf->getTools_global_modification(_isPrst));
    ui->comboModif->blockSignals(false);
    ui->graphParamGlobal->setMinMax(conf->getTools_global_mini(_isPrst),
                                    conf->getTools_global_maxi(_isPrst));
    ui->spinVelMin->setValue(qMin(conf->getTools_global_miniVel(_isPrst), conf->getTools_global_maxiVel(_isPrst)));
    ui->spinVelMax->setValue(qMax(conf->getTools_global_maxiVel(_isPrst), conf->getTools_global_miniVel(_isPrst)));

    // Dessin
    this->indexMotifChanged(this->ui->comboMotif->currentIndex());
    ui->graphParamGlobal->setMinMaxX(conf->getTools_global_miniX(_isPrst),
                                     conf->getTools_global_maxiX(_isPrst));
    this->ui->graphParamGlobal->setValues(conf->getTools_global_courbe(_isPrst));

    // Zone du clavier
    ui->graphParamGlobal->setEtendueClavier(conf->getKeyboardType());
}
DialogParamGlobal::~DialogParamGlobal()
{
    delete ui;
}

void DialogParamGlobal::indexMotifChanged(int index)
{
    if (index == 3 || index == 4)
    {
        this->ui->label_6->setText(trUtf8("Raideur"));
        this->ui->doubleSpinRaideur->setEnabled(true);
    }
    else if (index == 5)
    {
        this->ui->label_6->setText(trUtf8("Répartition"));
        this->ui->doubleSpinRaideur->setEnabled(true);
    }
    else
    {
        this->ui->label_6->setText("-");
        this->ui->doubleSpinRaideur->setEnabled(false);
    }
    this->ui->graphParamGlobal->indexMotifChanged(index);
}
void DialogParamGlobal::raideurChanged(double value)
{
    this->ui->graphParamGlobal->raideurChanged(value);
}
void DialogParamGlobal::minChanged(double value)
{
    this->ui->graphParamGlobal->setMinMax(value, this->ui->doubleSpinMax->value());
}
void DialogParamGlobal::maxChanged(double value)
{
    this->ui->graphParamGlobal->setMinMax(this->ui->doubleSpinMin->value(), value);
}
void DialogParamGlobal::accept()
{
    // Sauvegarde des paramètres
    Config * conf = Config::getInstance();
    conf->setTools_global_courbe(_isPrst, this->ui->graphParamGlobal->getValues());
    conf->setTools_global_motif(_isPrst, this->ui->comboMotif->currentIndex());
    conf->setTools_global_raideur(_isPrst, this->ui->doubleSpinRaideur->value());
    conf->setTools_global_mini(_isPrst, this->ui->doubleSpinMin->value());
    conf->setTools_global_maxi(_isPrst, this->ui->doubleSpinMax->value());
    conf->setTools_global_modification(_isPrst, this->ui->comboModif->currentIndex());
    conf->setTools_global_parametre(_isPrst, this->ui->comboValeur->currentIndex());
    conf->setTools_global_miniX(_isPrst, this->ui->graphParamGlobal->getXmin());
    conf->setTools_global_maxiX(_isPrst, this->ui->graphParamGlobal->getXmax());
    conf->setTools_global_miniVel(_isPrst, this->ui->spinVelMin->value());
    conf->setTools_global_maxiVel(_isPrst, this->ui->spinVelMax->value());

    // Récupération et mise en forme des modificateurs
    QVector<double> dValues = this->ui->graphParamGlobal->getValues();
    double dMin = this->ui->doubleSpinMin->value();
    double dMax = this->ui->doubleSpinMax->value();
    for (int i = 0; i < dValues.size(); i++)
        dValues[i] = dValues.at(i) * (dMax - dMin) + dMin;
    emit(accepted(dValues, this->ui->comboModif->currentIndex(),
                  this->ui->comboValeur->itemData(this->ui->comboValeur->currentIndex()).toInt(),
                  this->ui->spinVelMin->value(),
                  this->ui->spinVelMax->value()));
    QDialog::accept();
}

/////////////////////////////////////////////////////
///////////////////// GRAPHIQUE /////////////////////
/////////////////////////////////////////////////////

GraphParamGlobal::GraphParamGlobal(QWidget * parent) : QCustomPlot(parent),
    forme(FORME_MANUELLE),
    flagEdit(false),
    limitEdit(0),
    nbPoints(140),
    raideurExp(50.0),
    yMin(0.), yMax(1.),
    xMin(0), xMax(140),
    labelCoord(NULL),
    previousX(-1)
{
    // Layer pour la position des octaves
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(0, 0, 0, 40));
    graphPen.setWidth(1);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsLine);
    QVector<double> x, y;
    x.resize(20);
    y.resize(20);
    y[0] = y[3] = y[4] = y[7] = y[8] = y[11] = y[12] = y[15] = y[16] = y[19] =  2;
    y[1] = y[2] = y[5] = y[6] = y[9] = y[10] = y[13] = y[14] = y[17] = y[18] = -1;
    for (int i = 0; i < 10; i++)
    {
        int note = 12 * (i + 1);
        double pos = (double)(note * this->nbPoints) / 127.;
        x[2*i] = x[2*i+1] = pos;
        QCPItemText *textLabel = new QCPItemText(this);
        this->addItem(textLabel);
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(pos, 0);
        textLabel->setText(Config::getInstance()->getKeyName(note));
        textLabel->setFont(QFont(font().family(), 8));
        textLabel->setColor(QColor(40, 40, 40));
    }
    this->graph(0)->setData(x, y);

    // Layers coloration zone sur laquelle s'étend le clavier
    this->addGraph();
    x.resize(2);
    y.resize(2);
    x[0] = -1;
    x[1] = this->nbPoints + 1;
    y[0] = y[1] = -2;
    graphPen.setWidth(0);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setData(x, y);
    this->graph(1)->setBrush(QBrush(QColor(255, 255, 0, 30)));
    this->addGraph();
    this->graph(2)->setPen(graphPen);
    this->graph(1)->setChannelFillGraph(this->graph(2));

    // Layer des valeurs
    this->addGraph();
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidth(2);
    this->graph(3)->setPen(graphPen);
    this->graph(3)->setLineStyle(QCPGraph::lsNone);
    this->graph(3)->setScatterStyle(QCPScatterStyle::ssDot);
    this->graph(3)->setAntialiased(true);
    this->graph(3)->setAntialiasedScatters(true);

    // Layer aperçu valeurs
    this->addGraph();
    graphPen.setColor(QColor(0, 0, 0));
    graphPen.setWidth(1);
    this->graph(4)->setPen(graphPen);
    this->graph(4)->setScatterStyle(QCPScatterStyle::ssPlus);
    labelCoord = new QCPItemText(this);
    this->addItem(labelCoord);
    labelCoord->position->setType(QCPItemPosition::ptPlotCoords);
    labelCoord->setText("");
    QFont fontLabel = QFont(font().family(), 9);
    fontLabel.setBold(true);
    labelCoord->setFont(fontLabel);
    labelCoord->setColor(QColor(0, 0, 0));

    // Axes
    this->xAxis->setRange(0, this->nbPoints);
    this->yAxis->setRange(0, 1);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));
    // Préparation des données
    this->dValues.resize(this->nbPoints);
    this->dValues.fill(0.5);
    // Filtre sur les événements
    this->installEventFilter(this);
    // Affichage
    this->replot();
}
GraphParamGlobal::~GraphParamGlobal()
{
}

// Méthodes publiques
void GraphParamGlobal::setEtendueClavier(int keyboardType)
{
    QVector<double> x, y;
    x.resize(2);
    y.resize(2);
    x[0] = 36. * this->nbPoints / 127.;
    y[0] = y[1] = 2;
    if (keyboardType == 1)
    {
        // Clavier 5 octaves
        x[1] = 96. * this->nbPoints / 127.;
        this->graph(2)->setData(x, y);
    }
    else if (keyboardType == 2)
    {
        // Clavier 6 octaves
        x[1] = 108. * this->nbPoints / 127.;
        this->graph(2)->setData(x, y);
    }
    this->replot();
}
void GraphParamGlobal::indexMotifChanged(int motif)
{
    this->forme = (TypeForme)motif;
    this->flagEdit = false;
    this->limitEdit = 0;
    this->previousX = -1;
    this->xMin = 0;
    this->xMax = this->nbPoints;
    this->writeMotif();
}
void GraphParamGlobal::raideurChanged(double value)
{
    this->raideurExp = value;
    this->writeMotif();
}
QVector<double> GraphParamGlobal::getValues()
{
    return this->dValues;
}
void GraphParamGlobal::setValues(QVector<double> val)
{
    for (int i = 0; i < qMin(this->nbPoints, val.size()); i++)
        dValues[i] = val.at(i);
}

// Méthodes privées
void GraphParamGlobal::mousePressed(QPoint pos)
{
    this->afficheCoord(-1, -1);
    if (this->forme == FORME_MANUELLE)
    {
        this->flagEdit = true;
        // Inscription du premier point
        this->write(pos);
    }
    else
    {
        // Modification min X
        this->limitEdit = -1;
        int x = this->xAxis->pixelToCoord(pos.x());
        if (x < xMax)
        {
            xMin = x;
            this->writeMotif();
        }
    }
}
void GraphParamGlobal::mouseRightPressed(QPoint pos)
{
    if (this->forme != FORME_MANUELLE)
    {
        this->afficheCoord(-1, -1);
        // Modification max X
        this->limitEdit = 1;
        int x = this->xAxis->pixelToCoord(pos.x());
        if (x > xMin)
        {
            xMax = x;
            this->writeMotif();
        }
    }
}
void GraphParamGlobal::mouseReleased(QPoint pos)
{
    if (this->limitEdit == -1 || this->flagEdit)
    {
        this->limitEdit = 0;
        this->flagEdit = false;
        this->previousX = -1;
        // Affichage coordonnées
        this->mouseMoved(pos);
    }
}
void GraphParamGlobal::mouseRightReleased(QPoint pos)
{
    if (this->limitEdit == 1)
    {
        this->limitEdit = 0;
        // Affichage coordonnées
        this->mouseMoved(pos);
    }
}
void GraphParamGlobal::mouseMoved(QPoint pos)
{
    if (this->flagEdit)
    {
        this->afficheCoord(-1, -1);
        this->write(pos);
    }
    else if (this->limitEdit == -1 && forme != FORME_ALEATOIRE)
    {
        // Modification min X
        int x = this->xAxis->pixelToCoord(pos.x());
        if (x < 0)
            x = 0;
        else if (x >= this->nbPoints)
            x = this->nbPoints - 1;
        if (x < xMax)
        {
            xMin = x;
            this->writeMotif();
        }
    }
    else if (this->limitEdit == 1 && forme != FORME_ALEATOIRE)
    {
        // Modification max X
        int x = this->xAxis->pixelToCoord(pos.x());
        if (x < 0)
            x = 0;
        else if (x >= this->nbPoints)
            x = this->nbPoints - 1;
        if (x > xMin)
        {
            xMax = x;
            this->writeMotif();
        }
    }
    else
    {
        // Conversion des coordonnées
        double x = this->xAxis->pixelToCoord(pos.x());
        double y = this->yAxis->pixelToCoord(pos.y());
        // Point le plus proche
        double distanceMin = -1;
        int posX = -1;
        for (int i = 0; i < this->dValues.size(); i++)
        {
            double distanceTmp = 0.05 * qAbs(x - i) + qAbs(y - this->dValues[i]);
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                posX = i;
            }
        }
        if (posX != -1)
            this->afficheCoord(posX, this->dValues[posX]);
        else
            this->afficheCoord(-1, -1);
    }
}
void GraphParamGlobal::mouseLeft()
{
    this->afficheCoord(-1, -1);
}

void GraphParamGlobal::writeMotif()
{
    // Ecriture début et fin
    if (this->forme == FORME_LINEAIRE_ASC || this->forme == FORME_EXP_ASC)
    {
        // Remplissage de 0 au début
        for (int i = 0; i < this->xMin; i++)
            this->dValues[i] = 0;
        // Remplissage de 1 à la fin
        for (int i = this->xMax; i < this->nbPoints; i++)
            this->dValues[i] = 1;
    }
    else if (this->forme == FORME_LINEAIRE_DESC || this->forme == FORME_EXP_DESC)
    {
        // Remplissage de 1 au début
        for (int i = 0; i < this->xMin; i++)
            this->dValues[i] = 1;
        // Remplissage de 0 à la fin
        for (int i = this->xMax; i < this->nbPoints; i++)
            this->dValues[i] = 0;
    }

    switch (this->forme)
    {
    case FORME_MANUELLE:
        break;
    case FORME_LINEAIRE_ASC:
        for (int i = this->xMin; i < this->xMax; i++)
            this->dValues[i] = (double)(i - this->xMin) / (this->xMax - this->xMin);
        break;
    case FORME_LINEAIRE_DESC:
        for (int i = this->xMin; i < this->xMax; i++)
            this->dValues[i] = 1.0 - (double)(i - this->xMin) / (this->xMax - this->xMin);
        break;
    case FORME_EXP_ASC:{
        double baseExp = 1. + 1.0 / (101.0 - this->raideurExp);
        double alpha = 1.0 / (pow(baseExp, this->xMax - this->xMin) - 1.0);
        for (int i = this->xMin; i < this->xMax; i++)
            this->dValues[i] = alpha * (pow(baseExp, i - this->xMin) - 1.0);
        }break;
    case FORME_EXP_DESC:{
        double baseExp = 1. + 1.0 / (101.0 - this->raideurExp);
        double alpha = 1.0 / (pow(baseExp, this->xMin - this->xMax) - 1.0);
        for (int i = this->xMin; i < this->xMax; i++)
            this->dValues[i] = 1.0 - alpha * (pow(baseExp, this->xMin - i) - 1.0);
        }break;
    case FORME_ALEATOIRE:
        for (int i = 0; i < this->nbPoints; i++)
        {
            double valTmp = (double)(qrand() % 2000) / 1000. - 1.;
            double exp = qExp((50. - this->raideurExp) / 15.);
            if (valTmp < 0)
                this->dValues[i] = -qPow(qAbs(valTmp), exp);
            else
                this->dValues[i] = qPow(qAbs(valTmp), exp);
            this->dValues[i] = (this->dValues[i] + 1.) / 2;
        }
        break;
    }

    // Affichage
    this->replot();
}
void GraphParamGlobal::write(QPoint pos)
{
    // Conversion coordonnées
    int x = (int)this->xAxis->pixelToCoord(pos.x());
    double y = this->yAxis->pixelToCoord(pos.y());
    if (x < 0)
        x = 0;
    else if (x > this->nbPoints-1)
        x = this->nbPoints-1;
    if (y < 0)
        y = 0;
    else if (y > 1)
        y = 1;

    // Modification valeur
    if (this->previousX >= 0 && this->previousX != x)
    {
        if (this->previousX < x)
        {
            for (int i = this->previousX + 1; i < x; i++)
                this->dValues[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
        else
        {
            for (int i = this->previousX - 1; i > x; i--)
                this->dValues[i] = this->previousY +
                    (y - previousY) * (i - this->previousX) / (x - this->previousX);
        }
    }
    this->dValues[x] = y;

    // Mémorisation du point
    this->previousX = x;
    this->previousY = y;

    // Affichage
    this->replot();
}
void GraphParamGlobal::replot()
{
    QVector<double> x(this->nbPoints);
    for (int i = 0; i < this->nbPoints; i++)
        x[i] = i;
    this->graph(3)->setData(x, this->dValues);
    // Affichage
    QCustomPlot::replot();
}
void GraphParamGlobal::afficheCoord(double x, double y)
{
    QVector<double> xVector, yVector;
    if (x >= - 0.5)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = x;
        yVector[0] = y;
        // Affichage texte
        if (y >= 0.5)
            labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        labelCoord->setText(Config::getInstance()->getKeyName(qRound((double)x * 128. / this->nbPoints)) + ":" +
                            QString::number(yMin + (yMax - yMin) * y, 'f', 2));
        // Ajustement position
        QFontMetrics fm(labelCoord->font());
        double distX = this->xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 2);
        if (x < distX)
            x = distX;
        else if (x > this->nbPoints - distX)
            x = this->nbPoints - distX;
        double distY = 1. - this->yAxis->pixelToCoord(2);
        if (y >= 0.5)
            y -= distY;
        else
            y += distY;
        labelCoord->position->setCoords(x, y);
    }
    else
    {
        xVector.resize(0);
        yVector.resize(0);
        labelCoord->setText("");
    }
    this->graph(4)->setData(xVector, yVector);
    this->replot();
}
