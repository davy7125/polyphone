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

#include "graphiquefourier.h"
#include "sound.h"
#include "sampleutils.h"
#include "contextmanager.h"
#include <QMenu>
#include <QFileDialog>

GraphiqueFourier::GraphiqueFourier(QWidget * parent) : QCustomPlot(parent),
    _fixedTickerX(new QCPAxisTickerFixed()),
    _fixedTickerY(new QCPAxisTickerFixed()),
    _menu(nullptr)
{
    // Configuration du graphe
    this->addGraph();
    QPen graphPen;
    QColor color = this->palette().color(QPalette::WindowText);
    color.setAlpha(150);
    graphPen.setColor(color);
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);

    // Axes
    QPen penGrid = this->xAxis->grid()->pen();
    QPen penSubGrid = this->xAxis->grid()->subGridPen();
    penGrid.setColor(QColor(140, 140, 140));
    penSubGrid.setColor(QColor(220, 220, 220));

    this->xAxis->setRange(0, 20000);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->xAxis->setLabel(trUtf8("Frequency (Hz)"));
    this->xAxis->grid()->setSubGridVisible(true);
    this->xAxis->grid()->setPen(penGrid);
    this->xAxis->grid()->setSubGridPen(penSubGrid);
    this->xAxis->setTickLength(0);
    this->xAxis->setSubTickLength(0);
    this->xAxis->setBasePen(penGrid);
    _fixedTickerX->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    _fixedTickerX->setTickStep(2000);
    this->xAxis->setTicker(_fixedTickerX);

    this->yAxis->setRange(0, 1.05);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    this->yAxis->setLabel(trUtf8("Intensity"));
    this->yAxis->grid()->setSubGridVisible(true);
    this->yAxis->grid()->setPen(penGrid);
    this->yAxis->grid()->setSubGridPen(penSubGrid);
    this->yAxis->setTickLength(0);
    this->yAxis->setSubTickLength(0);
    this->yAxis->setBasePen(penGrid);
    _fixedTickerY->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    _fixedTickerY->setTickStep(0.2);
    this->yAxis->setTicker(_fixedTickerY);

    // Marges
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    // Texte
    text1 = new QCPItemText(this);
    text1->position->setType(QCPItemPosition::ptAxisRectRatio);
    text1->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text1->position->setCoords(1.0, 0);
    text1->setTextAlignment(Qt::AlignRight);
    text1->setFont(QFont(font().family(), 8, QFont::Bold));
    text1->setColor(color);

    text2 = new QCPItemText(this);
    text2->position->setType(QCPItemPosition::ptAxisRectRatio);
    text2->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text2->setTextAlignment(Qt::AlignRight);
    text2->setFont(QFont(font().family(), 7));
    color.setAlpha(75);
    text2->setColor(color);

    text3 = new QCPItemText(this);
    text3->position->setType(QCPItemPosition::ptAxisRectRatio);
    text3->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text3->setTextAlignment(Qt::AlignRight);
    text3->setFont(QFont(font().family(), 7));
    text3->setColor(this->palette().color(QPalette::Highlight));

    text4 = new QCPItemText(this);
    text4->position->setType(QCPItemPosition::ptAxisRectRatio);
    text4->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text4->setTextAlignment(Qt::AlignRight);
    text4->setFont(QFont(font().family(), 7));
    text4->setColor(this->palette().color(QPalette::Highlight));

    text5 = new QCPItemText(this);
    text5->position->setType(QCPItemPosition::ptAxisRectRatio);
    text5->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    text5->setTextAlignment(Qt::AlignRight);
    text5->setFont(QFont(font().family(), 7));
    text5->setColor(this->palette().color(QPalette::Highlight));

    // Préparation du menu contextuel
    _menu = new QMenu(this);
    QAction * action = _menu->addAction(trUtf8("Export graph"));
    connect(action, SIGNAL(triggered()), this, SLOT(exportPng()));

    this->plotLayout()->insertRow(0);
    this->plotLayout()->setRowStretchFactors(QList<double>() << 1 << 100);
    this->plotLayout()->setRowSpacing(0);
}

GraphiqueFourier::~GraphiqueFourier()
{
    _fixedTickerX.clear();
    _fixedTickerY.clear();
}

// Méthodes publiques
void GraphiqueFourier::setBackgroundColor(QColor color)
{
    // Modification de la couleur de fond
    this->setBackground(color);
}
void GraphiqueFourier::setData(QByteArray baData, quint32 dwSmplRate)
{
    int length = baData.size() / 2;
    _fData.resize(length);
    qint16 * data = (qint16*)baData.data();
    for (int i = 0; i < length; i++)
        _fData[i] = (float)data[i];

    this->dwSmplRate = dwSmplRate;
}
void GraphiqueFourier::setPos(qint32 posStart, qint32 posEnd, bool withReplot)
{
    QList<double> freq, factor;
    QList<int> pitch, corrections;
    setPos(posStart, posEnd, freq, factor, pitch, corrections, withReplot);
}
void GraphiqueFourier::setPos(qint32 posStart, qint32 posEnd, QList<double> &frequencies, QList<double> &factors,
                              QList<int> &pitch, QList<int> &corrections, bool withReplot)
{
    text1->setText("");
    text2->setText("");
    text3->setText("");
    text4->setText("");
    text5->setText("");

    if (_fData.isEmpty())
    {
        graph(0)->data()->clear();
        replot();
        return;
    }

    // Détermination du régime permanent pour transformée de Fourier et corrélation (max 0.5 seconde)
    if (posStart == posEnd)
        SampleUtils::regimePermanent(_fData, dwSmplRate, posStart, posEnd);
    if (posEnd > (qint32)(posStart + dwSmplRate / 2))
    {
        qint32 offset = (posEnd - posStart - dwSmplRate / 2) / 2;
        posStart += offset;
        posEnd -= offset;
    }

    // Extraction des données du régime permanent
    QVector<float> baData2 = _fData.mid(posStart, (posEnd - posStart));

    // Corrélation du signal de 20 à 20000Hz
    qint32 dMin;
    QVector<float> vectCorrel = SampleUtils::correlation(baData2.mid(0, qMin(baData2.size(), 4000)), dwSmplRate, 20, 20000, dMin);

    // Transformée de Fourier du signal
    QVector<float> vectFourier = SampleUtils::getFourierTransform(baData2);
    unsigned long size = vectFourier.size() * 2;

    // Recherche des corrélations minimales (= plus grandes similitudes) et intensités fréquencielles maximales
    QList<int> posMinCor = SampleUtils::findMins(vectCorrel, 20, 0.7);
    QList<quint32> posMaxFFT = SampleUtils::findMax(vectFourier, 50, 0.05);
    if (posMaxFFT.isEmpty())
    {
        graph(0)->data()->clear();
        replot();
        return;
    }

    // Affichage transformée
    this->dispFourier(vectFourier, posMaxFFT[0]);

    if (posMinCor.isEmpty())
    {
        this->replot();
        return;
    }

    // Pour chaque minimum de corrélation (considéré comme la base), on cherche un pic de fréquence
    double freq = 0;
    double score = -1;
    for (int i = 0; i < posMinCor.size(); i++)
    {
        // Portion de Fourier contenant la corrélation
        quint32 iMin, iMax;
        int coefFourier = 1;
        iMin = (size - 1) / (posMinCor[i] + dMin + 1) - 1;
        if (iMin < 1) iMin = 1;
        if (posMinCor[0] + dMin - 1 <= 0)
            iMax = 0;
        else
            iMax = (size - 1) / (posMinCor[i] + dMin - 1) + 1;
        if (iMax > size / 2) iMax = size / 2;

        // Un pic s'y trouve-t-il ?
        bool rep = false;
        int numeroPic = 0;
        while (rep == false && numeroPic < posMaxFFT.size())
        {
            rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
            if (!rep) numeroPic++;
        }

        if (!rep)
        {
            // Recherche d'un pic à l'octave du dessus
            coefFourier = 2;
            iMin *= 2;
            iMax *= 2;
            if (iMin >= size / 2) iMin = size / 2;
            if (iMax >= size / 2) iMax = size / 2;

            // Un pic s'y trouve-t-il ?
            bool rep = false;
            numeroPic = 0;
            while (rep == false && numeroPic < posMaxFFT.size())
            {
                rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
                if (!rep) numeroPic++;
            }
        }

        if (rep)
        {
            // Fréquence et score corrélation
            double freqCorrel = (double)((double)(size - 1) / (posMinCor[i] + dMin) * (double)dwSmplRate) / (double)(size - 1);
            double scoreCorrel;
            if (vectCorrel[posMinCor[i]] == 0)
                scoreCorrel = 1;
            else
                scoreCorrel = vectCorrel[posMinCor[0]] / vectCorrel[posMinCor[i]];

            // Fréquence et score Fourier
            double freqFourier = (double)((double)posMaxFFT[numeroPic] * (double)dwSmplRate) / (double)(size - 1) / coefFourier;
            double scoreFourier = vectFourier[posMaxFFT[numeroPic]] / vectFourier[posMaxFFT[0]];

            // Score global
            double scoreGlobal = scoreCorrel * scoreFourier;
            if (scoreGlobal > score)
            {
                score = scoreGlobal;

                // Ajustement des scores en fonction de la hauteur de note
                double noteTmp = 12 * qLn(freqCorrel) / 0.69314718056 - 36.3763;
                if (noteTmp < 40 || posEnd - posStart < 4096)
                    scoreFourier = 0;
                else if (noteTmp < 90)
                {
                    noteTmp = (noteTmp - 40) / 50;
                    scoreFourier *= noteTmp;
                    scoreCorrel *=  1. - noteTmp;
                }
                else
                    scoreCorrel = 0;

                // Détermination de la fréquence globale et enregistrement
                freq = (scoreCorrel * freqCorrel + scoreFourier * freqFourier) / (scoreCorrel + scoreFourier);
            }
        }
    }

    // Si aucune note n'a été trouvée, on prend la note la plus probable d'après Fourier
    if (score < 0)
        freq = (double)((double)posMaxFFT[0] * (double)dwSmplRate) / (double)(size - 1);

    // Numéro de la note correspondant à cette fréquence
    double note3 = 12 * qLn(freq) / 0.69314718056 - 36.3763;

    // Note la plus proche
    int note = qRound(note3);
    int correction = qRound(((double)note - note3) * 100.);

    // Affichage
    QString qStr1 = "";
    QString qStr2 = "";
    QString qStr3 = "";
    QString qStr4 = "";
    QString qStr5 = "";
    if (note >= 0 && note <= 128)
    {
        qStr1 = trUtf8("key") + " " + ContextManager::keyName()->getKeyName(note) + ", " +
                trUtf8("correction") + " " + QString::number(correction) + " (" + trUtf8("estimation") + ")";
        _note = note;
        _correction = correction;

        for (int i = 0; i < posMaxFFT.size(); i++)
        {
            // intensité
            factors << vectFourier[posMaxFFT[i]] / vectFourier[posMaxFFT[0]];

            // fréquence
            double freq = (double)(posMaxFFT[i] * dwSmplRate) / (size - 1);
            frequencies << freq;

            // note la plus proche
            double note = 12 * qLn(freq) / 0.69314718056 - 36.3763;
            if (note < 0)
                note = 0;
            else if (note > 128)
                note = 128;
            int note2 = qRound(note);
            int correction = qRound(((double)note2 - note) * 100.);
            pitch << note2;
            corrections << correction;

            // Prepare text
            if (i < 8)
            {
                qStr2 += QString::number(factors.last(), 'f', 2) + "\n";
                qStr3 += QString::number(freq, 'f', 2) + " " + trUtf8("Hz", "unit for Herz") + "\n";
                qStr4 += ContextManager::keyName()->getKeyName(note2) + "\n";
                qStr5 += QString::number(correction) + "\n";
            }
        }
    }
    else
    {
        _note = -1;
        _correction = 0;
    }

    text1->setText(qStr1);
    text2->setText(qStr2);
    text3->setText(qStr3);
    text4->setText(qStr4);
    text5->setText(qStr5);

    if (withReplot)
        this->replot();
}

void GraphiqueFourier::dispFourier(QVector<float> vectFourier, float posMaxFourier)
{
    this->graph(0)->data()->clear();
    quint32 size_x = ((long int)vectFourier.size() * 40000) / this->dwSmplRate;
    QVector<double> x(size_x), y(size_x);
    for (unsigned long i = 0; i < size_x; i++)
    {
        x[i] = (double)i/(size_x - 1) * 20000; // Conversion Hz
        if (i < (unsigned)vectFourier.size())
            y[i] = vectFourier[i] / vectFourier[posMaxFourier]; // normalisation entre 0 et 1
        else
            y[i] = 0;
    }
    this->graph(0)->setData(x, y);
}

void GraphiqueFourier::resizeEvent(QResizeEvent * event)
{
    // Repositionnement du texte
    QSize size = this->size();
    double y = 1.0 + (double)(17.0 - size.height()) / size.height();
    text2->position->setCoords((double)(size.width() - 108) / size.width(), y);
    text3->position->setCoords((double)(size.width() - 45) / size.width(), y);
    text4->position->setCoords((double)(size.width() - 20) / size.width(), y);
    text5->position->setCoords((double)(size.width() - 1) / size.width(), y);

    QCustomPlot::resizeEvent(event);
}
void GraphiqueFourier::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // Affichage menu contextuel
        _menu->exec(QCursor::pos());
    }
}
void GraphiqueFourier::exportPng()
{
    QString defaultFile = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_FREQUENCIES) + "/" +
            _name.replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".png";
    QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Export a graph"),
                                                    defaultFile, trUtf8("Png file") + " (*.png)");
    if (!fileName.isEmpty())
    {
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_FREQUENCIES, fileName);
        exportPng(fileName);
    }
}
void GraphiqueFourier::exportPng(QString fileName)
{
    // Préparation
    this->yAxis->setRange(0, 1);
    QColor backgroundColor = this->mBackgroundBrush.color();
    this->setBackgroundColor(Qt::white);
    this->xAxis->setVisible(true);
    this->xAxis->setTicks(true);
    this->yAxis->setVisible(true);
    this->yAxis->setTicks(true);
    this->axisRect()->setAutoMargins(QCP::msAll);
    QCPTextElement * title = new QCPTextElement(this, _name);
    this->plotLayout()->addElement(0, 0, title);
    QPen graphPen;
    graphPen.setColor(QColor(240, 0, 0));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);

    // Redimensionnement
    QSize minSize = this->minimumSize();
    this->setMinimumSize(1200, 400);

    // Création fichier png
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QPixmap pix = QPixmap::grabWidget(this);
        pix.save(&file, "PNG");
        file.close();
    }

    // Restauration du style du départ
    this->yAxis->setRange(0, 1.05);
    this->setBackgroundColor(backgroundColor);
    this->xAxis->setVisible(false);
    this->xAxis->setTicks(false);
    this->yAxis->setVisible(false);
    this->yAxis->setTicks(false);
    this->axisRect()->setAutoMargins(QCP::msNone);
    this->axisRect()->setMargins(QMargins(0, 0, 0, 0));
    this->plotLayout()->remove(title);
    graphPen.setColor(this->palette().color(QPalette::WindowText));
    graphPen.setWidthF(1);
    this->graph(0)->setPen(graphPen);

    // Redimensionnement
    this->setMinimumSize(minSize);
}

void GraphiqueFourier::getEstimation(int &pitch, int &correction)
{
    pitch = _note;
    correction = _correction;
}
