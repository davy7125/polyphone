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

#include "dialog_visualizer.h"
#include "ui_dialog_visualizer.h"
#include "config.h"
#include "page.h"

DialogVisualizer::DialogVisualizer(Pile_sf2 *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    _sf2(sf2),
    ui(new Ui::DialogVisualizer),
    _initialID(id)
{
    // Initialisation liste des champs
    QList<Champ> listeDesChamps;
    listeDesChamps << champ_initialAttenuation
                   << champ_pan
                   << champ_coarseTune
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
    _isPrst = id.typeElement == elementPrst || id.typeElement == elementPrstInst;

    Config * conf = Config::getInstance();

    // Chargement de la dernière configuration
    this->ui->comboParameter->blockSignals(true);
    for (int i = 0; i < listeDesChamps.size(); i++)
    {
        ui->comboParameter->addItem(Page::getGenName(listeDesChamps.at(i), 1 + _isPrst));
        ui->comboParameter->setItemData(i, (int)listeDesChamps.at(i));
    }
    this->ui->comboParameter->setCurrentIndex(conf->getTools_visualizer_parameter(_isPrst));
    this->ui->comboParameter->blockSignals(false);
    this->ui->checkLog->blockSignals(true);
    this->ui->checkLog->setChecked(conf->getTools_visualizer_logScale(_isPrst));
    this->ui->checkLog->blockSignals(false);

    // Dessin
    this->on_checkLog_stateChanged(conf->getTools_visualizer_logScale(_isPrst));
    this->on_comboParameter_currentIndexChanged(ui->comboParameter->currentIndex());

    // Légendes
    this->ui->widgetLegendDefined->plot(QCPScatterStyle::ssCross, QColor(100, 130, 250), 5, 2, false);
    this->ui->widgetLegendDefault->plot(QCPScatterStyle::ssCross, QColor(100, 130, 250), 5, 1, false);
    this->ui->widgetLegendMoyenne->plot(QCPScatterStyle::ssCircle, QColor(30, 250, 80), 7, 2, true);
}

DialogVisualizer::~DialogVisualizer()
{
    delete ui;
}

void DialogVisualizer::on_comboParameter_currentIndexChanged(int index)
{
    // Mémorisation du paramètre
    Config::getInstance()->setTools_visualizer_parameter(_isPrst, index);

    // Création des données
    QVector<QList<double> > vectListPoints, vectListPointsDef;
    vectListPoints.resize(128);
    vectListPointsDef.resize(128);
    Champ champ = (Champ)this->ui->comboParameter->itemData(index).toInt();

    // Valeur par défaut
    EltID id = _initialID;
    if (_isPrst)
        id.typeElement = elementPrst;
    else
        id.typeElement = elementInst;
    bool globDefined = _sf2->isSet(id, champ);
    double globVal;
    if (globDefined)
        globVal = this->getValue(id, champ);
    else
        globVal = this->getDefaultValue(champ);
    if (_isPrst)
        id.typeElement = elementPrstInst;
    else
        id.typeElement = elementInstSmpl;
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            rangesType keyRange = _sf2->get(id, champ_keyRange).rValue;
            if (_sf2->isSet(id, champ))
            {
                // Champ renseigné dans la division
                double val = this->getValue(id, champ);
                for (int key = keyRange.byLo; key <= keyRange.byHi; key++)
                {
                    if (key >= 0 && key < 128)
                        vectListPoints[key] << val;
                }
            }
            else if (globDefined)
            {
                // Champ renseigné globalement
                for (int key = keyRange.byLo; key <= keyRange.byHi; key++)
                {
                    if (key >= 0 && key < 128)
                        vectListPoints[key] << globVal;
                }
            }
            else
            {
                // Valeur par défaut du champ
                for (int key = keyRange.byLo; key <= keyRange.byHi; key++)
                {
                    if (key >= 0 && key < 128)
                        vectListPointsDef[key] << globVal;
                }
            }
        }
    }

    // Envoi des données au graphique
    this->ui->graphVisualizer->setData(vectListPoints, vectListPointsDef);
}

void DialogVisualizer::on_checkLog_stateChanged(int arg1)
{
    // Mémorisation du paramètre
    Config::getInstance()->setTools_visualizer_logScale(_isPrst, arg1);

    // Modification de l'échelle
    this->ui->graphVisualizer->setIsLog(arg1);
    this->ui->graphVisualizer->setScale();
}

double DialogVisualizer::getValue(EltID id, Champ champ)
{
    genAmountType genVal = _sf2->get(id, champ).genValue;
    double dRet = 0;
    switch (champ)
    {
    case champ_initialAttenuation: case champ_pan: case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv: case champ_sustainModEnv:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        dRet = (double)genVal.shAmount / 10.;
        break;

    case champ_sampleModes: case champ_overridingRootKey: case champ_exclusiveClass:
    case champ_keynum: case champ_velocity:
        dRet = genVal.wAmount;
        break;

    case champ_scaleTuning: case champ_coarseTune: case champ_fineTune:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
        dRet = genVal.shAmount;
        break;

    case champ_initialFilterFc:
        if (_isPrst)
            dRet = qPow(2., (double)genVal.shAmount/1200);
        else
            dRet = qPow(2., (double)genVal.shAmount/1200) * 8.176;
        break;

    case champ_freqModLFO: case champ_freqVibLFO:
        if (_isPrst)
            dRet = qPow(2., (double)genVal.shAmount/1200);
        else
            dRet = qPow(2., (double)genVal.shAmount/1200) * 8.176;
        break;

    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        dRet = qPow(2., (double)genVal.shAmount/1200);
        break;

    default: break;
    }

    return dRet;
}
double DialogVisualizer::getDefaultValue(Champ champ)
{
    double dRet = 0;
    switch (champ)
    {
    case champ_sampleModes: case champ_overridingRootKey:
    case champ_keynum: case champ_velocity:
        dRet = -1;
        break;
    case champ_scaleTuning: case champ_coarseTune: case champ_fineTune:
    case champ_exclusiveClass: case champ_pan: case champ_initialFilterQ:
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
    case champ_initialAttenuation: case champ_sustainModEnv:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_sustainVolEnv: case champ_modLfoToVolume:
    case champ_chorusEffectsSend: case champ_reverbEffectsSend:
        dRet = 0;
        break;
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        if (_isPrst)
            dRet = 1;
        else
            dRet = 0.001;
        break;

    case champ_freqModLFO: case champ_freqVibLFO:
        if (_isPrst)
            dRet = 1;
        else
            dRet = 8.176;
        break;

    case champ_initialFilterFc:
        if (_isPrst)
            dRet = 1;
        else
            dRet = 19914;
        break;
    default: break;
    }

    return dRet;
}


///////////////
// GRAPHIQUE //
///////////////

const double GraphVisualizer::MIN_LOG = 0.0001;

GraphVisualizer::GraphVisualizer(QWidget *parent) :
    QCustomPlot(parent),
    labelCoord(NULL)
{
    // Layer pour la position des octaves
    this->addGraph();
    QPen graphPen;
    graphPen.setColor(QColor(0, 0, 0, 40));
    graphPen.setWidth(1);
    this->graph(0)->setPen(graphPen);
    this->graph(0)->setLineStyle(QCPGraph::lsLine);
    for (int i = 0; i < 10; i++)
    {
        int note = 12 * (i + 1);
        QCPItemText *textLabel = new QCPItemText(this);
        listTextOctave << textLabel;
        this->addItem(textLabel);
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(note, 0);
        textLabel->setText(Config::getInstance()->getKeyName(note));
        textLabel->setFont(QFont(font().family(), 8));
        textLabel->setColor(QColor(40, 40, 40));
    }

    // Layer des moyennes
    this->addGraph();
    graphPen.setColor(QColor(30, 250, 80));
    graphPen.setWidth(2);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setLineStyle(QCPGraph::lsNone);
    this->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    this->graph(1)->setAntialiasedScatters(true);

    // Layer des valeurs
    this->addGraph();
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidth(2);
    this->graph(2)->setPen(graphPen);
    this->graph(2)->setLineStyle(QCPGraph::lsNone);
    this->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->graph(2)->setAntialiasedScatters(false);

    // Layer des valeurs par défaut
    this->addGraph();
    graphPen.setColor(QColor(100, 130, 250));
    graphPen.setWidth(1);
    this->graph(3)->setPen(graphPen);
    this->graph(3)->setLineStyle(QCPGraph::lsNone);
    this->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->graph(3)->setAntialiasedScatters(false);

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

    // Message warning
    textWarning = new QCPItemText(this);
    this->addItem(textWarning);
    textWarning->setPositionAlignment(Qt::AlignTop | Qt::AlignLeft);
    textWarning->position->setType(QCPItemPosition::ptPlotCoords);
    textWarning->position->setCoords(0, 0);
    textWarning->setFont(QFont(font().family(), 10, 100));
    textWarning->setColor(QColor(255, 0, 0));

    // Axes
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Filtre sur les événements
    this->installEventFilter(this);
}
GraphVisualizer::~GraphVisualizer()
{
}

// Méthodes publiques
void GraphVisualizer::setData(QVector<QList<double> > listPoints, QVector<QList<double> > listPointsDef)
{
    listX.clear();
    listY.clear();

    // Mise en forme des données, calcul de la moyenne
    QVector<double> vectValX, vectValY, vectDefValX, vectDefValY, vectMoyX, vectMoyY;
    int count;
    double somme;
    for (int i = 0; i < listPoints.size(); i++)
    {
        count = 0;
        somme = 0;
        for (int j = 0; j < listPoints.at(i).size(); j++)
        {
            count++;
            somme += listPoints.at(i).at(j);
            vectValX << i;
            vectValY << listPoints.at(i).at(j);
            listX << i;
            listY << listPoints.at(i).at(j);
        }
        for (int j = 0; j < listPointsDef.at(i).size(); j++)
        {
            count++;
            somme += listPointsDef.at(i).at(j);
            vectDefValX << i;
            vectDefValY << listPointsDef.at(i).at(j);
            listX << i;
            listY << listPointsDef.at(i).at(j);
        }
        if (count)
        {
            vectMoyX << i;
            vectMoyY << somme / count;
            listX << i;
            listY << somme / count;
        }
    }

    // Calcul des bornes
    if (listY.size())
    {
        xMinDonnees = listX.first();
        xMaxDonnees = listX.first();
        yMinDonnees = listY.first();
        yMaxDonnees = listY.first();
        yMin2Donnees = listY.first();
        for (int i = 1; i < listY.size(); i++)
        {
            xMinDonnees = qMin(xMinDonnees, listX.at(i));
            xMaxDonnees = qMax(xMaxDonnees, listX.at(i));
            yMinDonnees = qMin(yMinDonnees, listY.at(i));
            yMaxDonnees = qMax(yMaxDonnees, listY.at(i));
            if (yMin2Donnees < MIN_LOG)
                yMin2Donnees = listY.at(i);
            else
                if (listY.at(i) >= MIN_LOG && listY.at(i) < yMin2Donnees)
                    yMin2Donnees = listY.at(i);
        }
    }
    else
    {
        xMinDonnees = 0;
        xMaxDonnees = 127;
        yMinDonnees = 0;
        yMaxDonnees = 1;
    }
    if (yMin2Donnees < MIN_LOG)
        yMin2Donnees = yMaxDonnees;

    // Bornes sur x
    this->xAxis->setRange(xMinDonnees - 1, xMaxDonnees + 1);

    // Affichage
    this->graph(1)->setData(vectMoyX, vectMoyY);
    this->graph(2)->setData(vectValX, vectValY);
    this->graph(3)->setData(vectDefValX, vectDefValY);
    this->setScale();
}
void GraphVisualizer::setScale()
{
    double minY;
    double maxY = this->yMaxDonnees;

    if (isLog)
    {
        // Axe y
        this->yAxis->setScaleType(QCPAxis::stLogarithmic);

        // Bornes sur y
        minY = qMax(this->yMin2Donnees, MIN_LOG);
        maxY = qMax(maxY, MIN_LOG);
        if (minY == maxY)
        {
            minY /= 2;
            maxY *= 2;
        }
        else
        {
            double delta = maxY / minY;
            minY /= qPow(delta, 0.1);
            maxY *= qPow(delta, 0.05);
        }

        if (this->yMinDonnees < minY)
        {
            // Affichage avertissement
            textWarning->setText(trUtf8("Impossible d'afficher tous les points."));
            textWarning->position->setCoords(this->xAxis->range().lower, maxY);
        }
        else
            textWarning->setText("");
    }
    else
    {
        // Axe y
        this->yAxis->setScaleType(QCPAxis::stLinear);

        // Bornes sur y
        minY = this->yMinDonnees;
        if (minY == maxY)
        {
            minY -= 1;
            maxY += 1;
        }
        else
        {
            double delta = maxY - minY;
            minY -= delta * 0.1;
            maxY += delta * 0.05;
        }
        textWarning->setText("");
    }
    this->yAxis->setRange(minY, maxY);

    // Octaves
    QPointF pointTmp;
    for (int i = 0; i < listTextOctave.size(); i++)
    {
        pointTmp = listTextOctave.at(i)->position->coords();
        pointTmp.setY(minY);
        listTextOctave[i]->position->setCoords(pointTmp);
    }
    QVector<double> x, y;
    x.resize(20);
    y.resize(20);
    y[0] = y[3] = y[4] = y[7] = y[8] = y[11] = y[12] = y[15] = y[16] = y[19] = maxY;
    y[1] = y[2] = y[5] = y[6] = y[9] = y[10] = y[13] = y[14] = y[17] = y[18] = minY;
    for (int i = 0; i < 10; i++)
        x[2*i] = x[2*i+1] = 12 * (i + 1);
    this->graph(0)->setData(x, y);

    this->replot();
}
void GraphVisualizer::setIsLog(bool isLog)
{
    this->isLog = isLog;
}

// Méthodes privées
void GraphVisualizer::mouseMoved(QPoint pos)
{
    int posX = pos.x();
    int posY = pos.y();
    // Point le plus proche
    double distanceMin = -1;
    int index = -1;
    double distanceTmp;
    double minY = yAxis->range().lower;
    for (int i = 0; i < this->listX.size(); i++)
    {
        if (listY.at(i) >= minY)
        {
            distanceTmp = 10 * qAbs(posX - xAxis->coordToPixel(listX.at(i))) + qAbs(posY - yAxis->coordToPixel(listY.at(i)));
            if (distanceMin == -1 || distanceTmp < distanceMin)
            {
                distanceMin = distanceTmp;
                index = i;
            }
        }
    }
    if (index != -1)
        this->afficheCoord(this->listX.at(index), this->listY.at(index));
    else
        this->afficheCoord(-1, -1);
}
void GraphVisualizer::mouseLeft()
{
    this->afficheCoord(-1, -1);
}
void GraphVisualizer::afficheCoord(double x, double y)
{
    QVector<double> xVector, yVector;
    if (x >= - 0.5)
    {
        // Coordonnées du point
        xVector.resize(1);
        yVector.resize(1);
        xVector[0] = x;
        yVector[0] = y;
        // Limites du graphique
        double xMin = this->xAxis->range().lower;
        double xMax = this->xAxis->range().upper;
        double yMin = this->yAxis->range().lower;
        double yMax = this->yAxis->range().upper;
        // Affichage texte
        if (y > (yMin + yMax) / 2)
            labelCoord->setPositionAlignment(Qt::AlignTop    | Qt::AlignHCenter);
        else
            labelCoord->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        char T[20];
        if (y == 0)
            sprintf(T, "%s:%.0f", Config::getInstance()->getKeyName(qRound(x)).toStdString().c_str(), y);
        else if (y < 1)
            sprintf(T, "%s:%.3f", Config::getInstance()->getKeyName(qRound(x)).toStdString().c_str(), y);
        else if (y < 10)
            sprintf(T, "%s:%.2f", Config::getInstance()->getKeyName(qRound(x)).toStdString().c_str(), y);
        else if (y < 100)
            sprintf(T, "%s:%.1f", Config::getInstance()->getKeyName(qRound(x)).toStdString().c_str(), y);
        else
            sprintf(T, "%s:%.0f", Config::getInstance()->getKeyName(qRound(x)).toStdString().c_str(), y);
        labelCoord->setText(T);
        // Ajustement position sur x
        QFontMetrics fm(labelCoord->font());
        double distX = this->xAxis->pixelToCoord(fm.width(labelCoord->text()) / 2 + 4) - xMin;
        if (x < xMin + distX)
            x = xMin + distX;
        else if (x > xMax - distX)
            x = xMax - distX;
        // Ajustement position sur y
        int pixelY = this->yAxis->coordToPixel(y);
        if (y > (yMin + yMax) / 2)
            pixelY += 3;
        else
            pixelY -= 3;
        y = yAxis->pixelToCoord(pixelY);
        // Positionnement du texte
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


//////////////
// LEGENDES //
//////////////

GraphLegend::GraphLegend(QWidget *parent) : QCustomPlot(parent)
{
    // Couche pour le symbole
    this->addGraph();

    // Ajout d'un point
    QVector<double> x, y;
    x << 0;
    y << 0;
    this->graph(0)->setData(x, y);

    // Axes
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->xAxis->setRange(-1, 1);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    this->yAxis->setRange(-1, 1);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Couleur de fond
    this->setBackground(parent->palette().background().color());
}
void GraphLegend::plot(QCPScatterStyle::ScatterShape style, QColor couleur, int size, int epaisseur, bool antiAliased)
{
    this->graph(0)->setScatterStyle(QCPScatterStyle(style, size));
    this->graph(0)->setAntialiasedScatters(antiAliased);
    QPen pen;
    pen.setColor(couleur);
    pen.setWidth(epaisseur);
    this->graph(0)->setPen(pen);

    this->replot();
}
