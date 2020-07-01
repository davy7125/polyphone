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

#include "graphiquefourier.h"
#include "sound.h"
#include "sampleutils.h"
#include "contextmanager.h"
#include "utils.h"
#include <QMenu>
#include <QFileDialog>
#include <QPainter>

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
    this->xAxis->setLabel(tr("Frequency (Hz)"));
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
    this->yAxis->setLabel(tr("Intensity"));
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

    // Préparation du menu contextuel
    _menu = new QMenu(this);
    QAction * action = _menu->addAction(tr("Export graph"));
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

void GraphiqueFourier::setBackgroundColor(QColor color)
{
    // Modification de la couleur de fond
    this->setBackground(color);
}

void GraphiqueFourier::setData(QByteArray baData, quint32 dwSmplRate)
{
    int length = baData.size() / 2;
    _fData.resize(length);
    qint16 * data = reinterpret_cast<qint16*>(baData.data());
    for (int i = 0; i < length; i++)
        _fData[i] = static_cast<float>(data[i]);

    this->dwSmplRate = dwSmplRate;
}

void GraphiqueFourier::setPos(quint32 posStart, quint32 posEnd, bool withReplot)
{
    QList<double> freq, factor;
    QList<int> pitch, corrections;
    setPos(posStart, posEnd, freq, factor, pitch, corrections, withReplot);
}

void GraphiqueFourier::setPos(quint32 posStart, quint32 posEnd, QList<double> &frequencies, QList<double> &factors,
                              QList<int> &pitch, QList<int> &deltas, bool withReplot)
{
    _peaks.clear();
    _delta = 0;
    _key = -1;

    if (posEnd < 20 + posStart)
    {
        // Take the full sample in that case
        posStart = 0;
        posEnd = static_cast<quint32>(_fData.size() - 1);
    }

    if (_fData.isEmpty())
    {
        graph(0)->data()->clear();
        replot();
        return;
    }

    // Détermination du régime permanent pour transformée de Fourier et corrélation (max 0.5 seconde)
    if (posStart == posEnd)
        SampleUtils::regimePermanent(_fData, dwSmplRate, posStart, posEnd);
    if (posEnd > static_cast<quint32>(0.5 * dwSmplRate) + posStart)
    {
        quint32 offset = (posEnd - posStart - dwSmplRate / 2) / 2;
        posStart += offset;
        posEnd -= offset;
    }

    // Extraction des données du régime permanent
    QVector<float> baData2 = _fData.mid(static_cast<int>(posStart), static_cast<int>(posEnd - posStart));

    // Corrélation du signal de 20 à 20000Hz
    quint32 dMin;
    QVector<float> vectCorrel = SampleUtils::correlation(baData2.constData(),
                                                         qMin(static_cast<quint32>(baData2.size()), 4000u),
                                                         dwSmplRate, 20, 20000, dMin);

    // Transformée de Fourier du signal
    QVector<float> vectFourier = SampleUtils::getFourierTransform(baData2);
    quint32 size = static_cast<quint32>(vectFourier.size()) * 2;

    // Recherche des corrélations minimales (= plus grandes similitudes) et intensités fréquencielles maximales
    QList<quint32> posMinCor = SampleUtils::findMins(vectCorrel, 20, 0.7f);
    QList<quint32> posMaxFFT = SampleUtils::findMax(vectFourier, 50, 0.05f);
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
    float freq = 0;
    float score = -1;
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
            float freqCorrel = (static_cast<float>(size - 1) / (posMinCor[i] + dMin) * dwSmplRate) / (size - 1);
            float scoreCorrel;
            if (vectCorrel[static_cast<int>(posMinCor[i])] <= 0)
                scoreCorrel = 1;
            else
                scoreCorrel = vectCorrel[static_cast<int>(posMinCor[0])] / vectCorrel[static_cast<int>(posMinCor[i])];

            // Fréquence et score Fourier
            float freqFourier = (static_cast<float>(posMaxFFT[numeroPic]) * dwSmplRate) / (size - 1) / coefFourier;
            float scoreFourier = vectFourier[static_cast<int>(posMaxFFT[numeroPic])] /
                    vectFourier[static_cast<int>(posMaxFFT[0])];

            // Score global
            float scoreGlobal = scoreCorrel * scoreFourier;
            if (scoreGlobal > score)
            {
                score = scoreGlobal;

                // Ajustement des scores en fonction de la hauteur de note
                float noteTmp = 12.0f * static_cast<float>(qLn(static_cast<double>(freqCorrel))) / 0.69314718056f - 36.3763f;
                if (noteTmp < 40 || posEnd - posStart < 4096)
                    scoreFourier = 0;
                else if (noteTmp < 90)
                {
                    noteTmp = (noteTmp - 40) / 50;
                    scoreFourier *= noteTmp;
                    scoreCorrel *=  1.f - noteTmp;
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
        freq = static_cast<float>(posMaxFFT[0]) * dwSmplRate / (size - 1);

    // Numéro de la note correspondant à cette fréquence
    double note3 = 12 * qLn(static_cast<double>(freq)) / 0.69314718056 - 36.3763;

    // Note la plus proche
    int note = qRound(note3);

    // Affichage
    if (note >= 0 && note <= 128)
    {
        _key = note;
        _delta = Utils::round32((note3 - static_cast<double>(note)) * 100.);

        for (int i = 0; i < posMaxFFT.size(); i++)
        {
            // intensité
            factors << static_cast<double>(vectFourier[static_cast<int>(posMaxFFT[i])] /
                       vectFourier[static_cast<int>(posMaxFFT[0])]);

            // fréquence
            double freq = static_cast<double>(posMaxFFT[i] * dwSmplRate) / (size - 1);
            frequencies << freq;

            // note la plus proche
            double note = 12. * qLn(freq) / 0.69314718056 - 36.3763;
            if (note < 0)
                note = 0;
            else if (note > 128)
                note = 128;
            int note2 = Utils::round32(note);
            int delta = Utils::round32((static_cast<double>(note2) - note) * 100.);
            pitch << note2;
            deltas << delta;

            // Prepare text
            _peaks << Peak(factors.last(), freq, note2, delta);
        }
    }
    else
    {
        _key = -1;
        _delta = 0;
    }

    if (withReplot)
        this->replot();
}

void GraphiqueFourier::dispFourier(QVector<float> vectFourier, float posMaxFourier)
{
    this->graph(0)->data()->clear();
    qint32 size_x = (vectFourier.size() * 40000) / static_cast<signed>(this->dwSmplRate);
    QVector<double> x(size_x), y(size_x);
    for (qint32 i = 0; i < size_x; i++)
    {
        x[i] = (20000. * i) / (size_x - 1); // Conversion Hz
        if (i < vectFourier.size())
            y[i] = static_cast<double>(vectFourier[i]) /
                    static_cast<double>(vectFourier[static_cast<int>(posMaxFourier)]); // normalisation entre 0 et 1
        else
            y[i] = 0;
    }
    this->graph(0)->setData(x, y, true);
}

void GraphiqueFourier::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // Affichage menu contextuel
        _menu->exec(QCursor::pos());
    }
}

void GraphiqueFourier::paintEvent(QPaintEvent * event)
{
    // Draw the graph
    QCustomPlot::paintEvent(event);
    if (_peaks.empty())
        return;

    // Helpful elements
    QSize size = this->size();
    int marginRight = 1;
    int marginTop = 1;
    int tuneWidth = qMin(150, size.width() - marginRight);
    int tuneCellPadding = 5;
    int tickHalfHeight = 3;

    QColor highlightColor = this->palette().color(QPalette::Highlight);
    QColor highlightTextColor = this->palette().color(QPalette::HighlightedText);
    QColor backgroundColor = this->palette().color(QPalette::Window);
    QColor textColor = this->palette().color(QPalette::WindowText);

    QFont fontInfo = QFont(font().family(), 7);
    QFont fontInfoSmall = QFont(font().family(), 6);
    QFont fontMain = fontInfo;
    fontMain.setBold(true);
    QFontMetrics fm(fontMain);
    int fontHeight = fm.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Text to display
    QString keyName = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_key));
    if (ContextManager::keyName()->getNameMiddleC() == KeyNameManager::MIDDLE_C_60)
        keyName += " (" + ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_key), true, false, false, true) + ")";
    int textWidth = fm.width(keyName);

    // TUNING

    // Left and right scales, delta
    textColor.setAlpha(100);
    painter.setPen(textColor);
    painter.setFont(fontMain);
    {
        int tuneCellMargin = fontHeight * 3 / 2;
        int x1 = size.width() - marginRight - tuneWidth;
        int x2 = size.width() - marginRight - tuneWidth / 2 - textWidth / 2 - tuneCellMargin - tuneCellPadding;
        int x3 = size.width() - marginRight - tuneWidth / 2 + textWidth / 2 + tuneCellMargin + tuneCellPadding;
        int x4 = size.width() - marginRight;

        // Horizontal line
        painter.drawLine(x1, marginTop + tuneCellPadding + fontHeight / 2, x2,
                         marginTop + tuneCellPadding + fontHeight / 2);
        painter.drawLine(x3, marginTop + tuneCellPadding + fontHeight / 2, x4,
                         marginTop + tuneCellPadding + fontHeight / 2);

        // Ticks
        for (int i = 0; i < 6; i++)
        {
            int x = x1 + (x2 - x1) * i / 5;
            painter.drawLine(x, marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             x, marginTop + tuneCellPadding + fontHeight / 2 + tickHalfHeight);
            x = x3 + (x4 - x3) * i / 5;
            painter.drawLine(x, marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             x, marginTop + tuneCellPadding + fontHeight / 2 + tickHalfHeight);
        }

        // Delta
        if (_delta != 0)
        {
            // Text position
            int pos = _delta > 0 ?
                        (50 * x3 + (x4 - x3) * _delta) / 50 :
                        (50 * x2 + (x2 - x1) * _delta) / 50;
            QString txt = QString::number(_delta < 0 ? -_delta : _delta);
            int textWidth = fm.width(txt);

            painter.setBrush(highlightColor);
            painter.setPen(highlightColor);
            painter.drawEllipse(QPoint(pos, marginTop + tuneCellPadding + fontHeight / 2),
                                fontHeight / 2 + tuneCellPadding, fontHeight / 2 + tuneCellPadding);
            painter.fillRect(pos, marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             size.width() - marginRight - tuneWidth / 2 - pos, 2 * tickHalfHeight,
                             highlightColor);
            painter.setPen(highlightTextColor);
            painter.drawText(QRect(pos - textWidth / 2, marginTop + tuneCellPadding, textWidth, fontHeight),
                             Qt::AlignCenter, txt);
        }
    }

    // Write key name
    {
        QPainterPath path;
        textColor.setAlpha(255);
        path.addRoundedRect(QRectF(size.width() - marginRight - tuneWidth / 2 - textWidth / 2 - tuneCellPadding,
                                   marginTop, textWidth + 2 * tuneCellPadding, fontHeight + 2 * tuneCellPadding), 3, 3);
        painter.fillPath(path, _delta == 0 ? highlightColor : textColor);
        painter.setPen(_delta == 0 ? highlightTextColor : backgroundColor);
        painter.drawText(QRect(size.width() - marginRight - tuneWidth, marginTop + tuneCellPadding,
                               tuneWidth - marginRight, fontHeight), Qt::AlignCenter, keyName);
    }

    // MAIN FREQUENCY PEAKS

    textColor.setAlpha(100);
    painter.setPen(textColor);
    {
        int peakNumber = 0;
        int posY = 2 * tuneCellPadding + fontHeight * 3 / 2;
        while (posY + fontHeight < size.height() && peakNumber < _peaks.count())
        {
            painter.setFont(fontInfo);
            painter.drawText(QRect(0, posY, size.width() - 133 - marginRight, fontHeight),
                             Qt::AlignRight,
                             QString::number(_peaks[peakNumber]._intensity, 'f', 2));
            painter.drawText(QRect(0, posY, size.width() - 74 - marginRight, fontHeight),
                             Qt::AlignRight,
                             QString::number(_peaks[peakNumber]._frequency, 'f', 2) + " " + tr("Hz", "unit for Herz"));
            painter.drawText(QRect(0, posY, size.width() - 45 - marginRight, fontHeight),
                             Qt::AlignRight,
                             ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_peaks[peakNumber]._key)));
            painter.drawText(QRect(0, posY, size.width() - 20 - marginRight, fontHeight),
                             Qt::AlignRight,
                             (_peaks[peakNumber]._delta > 0 ? "+" : "") + QString::number(_peaks[peakNumber]._delta));
            painter.setFont(fontInfoSmall);
            painter.drawText(QRect(0, posY, size.width() - marginRight, fontHeight),
                             Qt::AlignRight, "/ 100");
            peakNumber++;
            posY += fontHeight;
        }
    }
}

void GraphiqueFourier::exportPng()
{
    QString defaultFile = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_FREQUENCIES) + "/" +
            _name.replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".png";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export a graph"),
                                                    defaultFile, tr("Png file") + " (*.png)");
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
        QPixmap pix = this->grab();
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
    pitch = _key;
    correction = -_delta;
}
