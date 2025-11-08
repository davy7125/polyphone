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

#include "graphicsfourier.h"
#include <QMouseEvent>
#include <QPainterPath>
#include "sound.h"
#include "sampleutils.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "frequency_peaks/toolfrequencypeaks.h"
#include "graphicswavepainter.h"
#include "utils.h"
#include <QMenu>
#include <QFileDialog>
#include <QPainter>

GraphicsFourier::GraphicsFourier(QWidget * parent) : QWidget(parent),
    _wavePainter(new GraphicsWavePainter()),
    _menu(nullptr),
    _toolFrequencyPeak(new ToolFrequencyPeaks()),
    _paintForPng(false)
{
    // Préparation du menu contextuel
    _menu = new QMenu(this);
    _menu->setStyleSheet(ContextManager::theme()->getMenuTheme());
    QAction * action1 = _menu->addAction(tr("Export graph") + "...");
    connect(action1, SIGNAL(triggered()), this, SLOT(exportPng()));
    QAction * action2 = _menu->addAction(tr("Show peak frequencies") + "...");
    connect(action2, SIGNAL(triggered()), this, SLOT(exportPeaks()));

    // Wave configuration
    QColor color = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
    color.setAlpha(150);
    _wavePainter->setWaveColor(color);
    _wavePainter->setOnlyPositiveValues(true);
    _wavePainter->addMeanCurve(false);
}

GraphicsFourier::~GraphicsFourier()
{
    delete _toolFrequencyPeak;
    delete _wavePainter;
}

void GraphicsFourier::setCurrentIds(IdList ids)
{
    _currentIds = ids;

    if (_currentIds.count() == 1)
    {
        // Sample rate
        this->_dwSmplRate = SoundfontManager::getInstance()->get(_currentIds[0], champ_dwSampleRate).dwValue;

        // Load data
        _fData = SoundfontManager::getInstance()->getDataFloat(_currentIds[0]);

        // Menu
        _menu->actions()[0]->setEnabled(true);
    }
    else
    {
        this->_dwSmplRate = -1;
        _fData.clear();
        _peaks.clear();
        _wavePainter->clearData();
        this->update();

        // Menu
        _menu->actions()[0]->setEnabled(false);
    }
}

void GraphicsFourier::setPos(quint32 posStart, quint32 posEnd, bool withRepaint)
{
    if (_currentIds.count() != 1)
        return;

    _peaks.clear();
    _key = -1;
    _delta = 0;

    if (_fData.isEmpty())
        _wavePainter->clearData();
    else
    {
        QVector<float> vectFourier;
        int posMaxFourier;
        _peaks = computePeaks(_fData, _dwSmplRate, posStart, posEnd, vectFourier, posMaxFourier, &_key, &_delta, &_score);

        // Display the Fourier transform?
        if (posMaxFourier == -1)
            _wavePainter->clearData();
        else
            this->dispFourier(vectFourier, posMaxFourier);
    }

    if (withRepaint)
        this->update();
}

QList<Peak> GraphicsFourier::computePeaks(QVector<float> fData, quint32 dwSmplRate,
                                           quint32 posStart, quint32 posEnd,
                                           QVector<float> &vectFourier, int &posMaxFourier,
                                           int *key, int *correction, float * reliance)
{
    // If there is no loop, determine a stable portion for the Fourier transform and the autocorrelation
    if (posStart == posEnd)
        SampleUtils::regimePermanent(fData, dwSmplRate, posStart, posEnd);

    // Limit the portion to 0.5 second
    if (posEnd > dwSmplRate / 2 + posStart)
    {
        quint32 offset = (posEnd - dwSmplRate / 2 - posStart) / 2;
        posStart += offset;
        posEnd -= offset;
    }

    // Extraction data from the stable portion
    QVector<float> baData2 = fData.mid(static_cast<int>(posStart), static_cast<int>(posEnd - posStart));

    // Duplicate existing data in case the length is less then 0.5s
    while (baData2.size() < 0.5 * dwSmplRate)
        baData2 += fData.mid(static_cast<int>(posStart), static_cast<int>(posEnd - posStart));

    // Corrélation du signal de 20 à 20000Hz
    quint32 dMin;
    QVector<float> vectCorrel = SampleUtils::correlation(baData2.constData(),
                                                         qMin(static_cast<quint32>(baData2.size()), 4000u),
                                                         dwSmplRate, 20, 20000, dMin);

    // Transformée de Fourier du signal
    vectFourier = SampleUtils::getFourierTransform(baData2);
    quint32 size = static_cast<quint32>(vectFourier.size()) * 2;

    // Recherche des corrélations minimales (= plus grandes similitudes) et intensités fréquencielles maximales
    QList<quint32> posMinCor = SampleUtils::findMins(vectCorrel, 20, 0.7f);
    QList<quint32> posMaxFFT = SampleUtils::findMax(vectFourier, 50, 0.05f);
    if (posMaxFFT.isEmpty())
    {
        posMaxFourier = -1;
        return QList<Peak>();
    }
    else
        posMaxFourier = posMaxFFT[0];

    if (posMinCor.isEmpty())
        return QList<Peak>();

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
            if (!rep)
                numeroPic++;
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
            numeroPic = 0;
            while (!rep && numeroPic < posMaxFFT.size())
            {
                rep = iMin < posMaxFFT[numeroPic] && posMaxFFT[numeroPic] < iMax;
                if (!rep)
                    numeroPic++;
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
    {
        freq = static_cast<float>(posMaxFFT[0]) * dwSmplRate / (size - 1);
        score = 0;
    }

    // Numéro de la note correspondant à cette fréquence
    double note3 = 12 * qLn(static_cast<double>(freq)) / 0.69314718056 - 36.3763;

    // Note la plus proche
    int note = qRound(note3);

    // Affichage
    QList<Peak> result;
    if (note >= 0 && note <= 128)
    {
        // Closest key with the associated correction
        if (key != nullptr)
            *key = note;
        if (correction != nullptr)
            *correction = Utils::round32((note3 - static_cast<double>(note)) * 100.);
        if (reliance != nullptr)
            *reliance = score;

        // Peaks
        for (int i = 0; i < posMaxFFT.size(); i++)
        {
            // intensité
            double factor = static_cast<double>(vectFourier[static_cast<int>(posMaxFFT[i])] /
                            vectFourier[static_cast<int>(posMaxFFT[0])]);

            // fréquence
            double freq = static_cast<double>(posMaxFFT[i] * dwSmplRate) / (size - 1);

            // note la plus proche
            double note = 12. * qLn(freq) / 0.69314718056 - 36.3763;
            if (note < 0)
                note = 0;
            else if (note > 128)
                note = 128;
            int note2 = Utils::round32(note);
            int delta = Utils::round32((static_cast<double>(note2) - note) * 100.);

            // Prepare text
            result << Peak(factor, freq, note2, delta);
        }
    }
    else
    {
        if (reliance != nullptr)
            *reliance = -1.0f;
    }

    return result;
}

void GraphicsFourier::dispFourier(QVector<float> vectFourier, float posMaxFourier)
{
    qint32 size_x = (vectFourier.size() * 40000) / static_cast<signed>(this->_dwSmplRate);
    QVector<float> y(size_x);
    for (qint32 i = 0; i < size_x; i++)
    {
        if (i < vectFourier.size())
            y[i] = vectFourier[i] / vectFourier[static_cast<int>(posMaxFourier)]; // normalisation entre 0 et 1
        else
            y[i] = 0;
    }
    _wavePainter->setData(y);
}

void GraphicsFourier::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // Affichage menu contextuel
        _menu->exec(QCursor::pos());
    }
}

QRect GraphicsFourier::paintGrid(QPainter &painter, QRect rect)
{
    QFont font = this->font();
    QFontMetrics fm(font);
    QPen penGrid = QPen(QColor(140, 140, 140), 1, Qt::DotLine);
    QPen penSubGrid = QPen(QColor(220, 220, 220), 1, Qt::DotLine);
    QColor textColor = Qt::black;

    painter.setFont(font);
    int margin = 5;
    int verticalLegendWidth = fm.horizontalAdvance("0.8");
    int textHeight = fm.height();

    // x[i] = (20000. * i) / (size_x - 1); // Conversion Hz

    QRect gridRect = rect;
    gridRect.setLeft(rect.left() + verticalLegendWidth + textHeight + 3 * margin);
    gridRect.setBottom(rect.bottom() - 2 * textHeight - 3 * margin);
    gridRect.setRight(rect.right() - margin);
    gridRect.setTop(rect.top() + 2 * margin + textHeight);

    // Title
    painter.setPen(textColor);
    painter.drawText(
        QRect(gridRect.left(), rect.top() + margin, gridRect.width(), textHeight),
        Qt::AlignCenter | Qt::AlignTop, _name);

    // Labels
    painter.drawText(
        QRect(gridRect.left(), rect.bottom() - textHeight - margin, gridRect.width(), textHeight),
        Qt::AlignCenter | Qt::AlignTop, tr("Frequency (Hz)"));
    painter.save();
    painter.rotate(-90);
    painter.drawText(
        QRect(-gridRect.bottom(), margin, gridRect.height(), textHeight),
        Qt::AlignCenter | Qt::AlignTop, tr("Intensity"));
    painter.restore();

    // Horizontal lines
    for (int i = 0; i <= 20; i++)
    {
        painter.setPen(i % 4 == 0 ? penGrid : penSubGrid);
        float posY = gridRect.top() + gridRect.height() * (static_cast<float>(i) / 20.);
        painter.drawLine(QPointF(gridRect.left(), posY), QPointF(gridRect.right(), posY));

        if (i % 4 == 0)
        {
            painter.setPen(textColor);
            painter.drawText(
                QRect(rect.left() + 2 * margin + textHeight, posY - textHeight / 2, verticalLegendWidth, textHeight),
                Qt::AlignRight | Qt::AlignVCenter, QString::number(1.0f - static_cast<float>(i) / 20.0f));
        }
    }

    // Vertical lines
    for (int i = 0; i <= 40; i++)
    {
        painter.setPen(i % 4 == 0 ? penGrid : penSubGrid);
        float posX = gridRect.left() + gridRect.width() * (static_cast<float>(i) / 40.);
        painter.drawLine(QPointF(posX, gridRect.top()), QPointF(posX, gridRect.bottom()));

        if (i % 4 == 0 && i < 40)
        {
            painter.setPen(textColor);
            painter.drawText(
                QRect(posX - 50, rect.bottom() - 2 * fm.height() - 2 * margin, 100, fm.height()),
                Qt::AlignCenter | Qt::AlignTop, QString::number(20000 * i / 40));
        }
    }

    return gridRect;
}

void GraphicsFourier::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    if (_peaks.empty())
        return;

    // Helpful elements
    QRect rect = this->rect();
    int marginRight = 1;
    int marginTop = 1;
    int tuneWidth = qMin(160, rect.width() - marginRight);
    int tuneCellPadding = 5;
    int tickHalfHeight = 3;

    QColor highlightColor = _paintForPng ? QColor(240, 0, 0) : ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor highlightTextColor = _paintForPng ? Qt::white : ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor backgroundColor = _paintForPng ? Qt::white : ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND);
    QColor textColor = _paintForPng ? Qt::black : ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);

    QFont fontInfo = QFont(font().family(), 7);
    QFont fontInfoSmall = QFont(font().family(), 6);
    QFont fontMain = fontInfo;
    fontMain.setBold(true);
    QFontMetrics fm(fontMain);
    int fontHeight = fm.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Full rect
    if (_paintForPng)
    {
        // White background color
        painter.fillRect(rect, backgroundColor);
        _wavePainter->setWaveColor(highlightColor);

        // Grid with legend
        rect = this->paintGrid(painter, rect);
    }

    // Fourier transform
    _wavePainter->paint(&painter, rect, 0, 0, 1);

    // Text to display
    QString keyName = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_key));
    if (ContextManager::keyName()->getNameMiddleC() == KeyNameManager::MIDDLE_C_60)
        keyName += " (" + ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_key), true, false, false, true) + ")";
    int textWidth = fm.horizontalAdvance(keyName);

    // TUNING

    // Left and right scales, delta
    textColor.setAlpha(100);
    painter.setPen(textColor);
    painter.setFont(fontMain);
    {
        int tuneCellMargin = fontHeight * 3 / 2;
        int x1 = rect.right() - marginRight - tuneWidth + fontHeight / 2 + tuneCellPadding;
        int x2 = rect.right() - marginRight - tuneWidth / 2 - textWidth / 2 - tuneCellMargin - tuneCellPadding;
        int x3 = rect.right() - marginRight - tuneWidth / 2 + textWidth / 2 + tuneCellMargin + tuneCellPadding;
        int x4 = rect.right() - marginRight - fontHeight / 2 - tuneCellPadding;

        // Horizontal line
        painter.drawLine(x1, rect.top() + marginTop + tuneCellPadding + fontHeight / 2, x2,
                         rect.top() + marginTop + tuneCellPadding + fontHeight / 2);
        painter.drawLine(x3, rect.top() + marginTop + tuneCellPadding + fontHeight / 2, x4,
                         rect.top() + marginTop + tuneCellPadding + fontHeight / 2);

        // Ticks
        for (int i = 0; i < 6; i++)
        {
            int x = x1 + (x2 - x1) * i / 5;
            painter.drawLine(x, rect.top() + marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             x, rect.top() + marginTop + tuneCellPadding + fontHeight / 2 + tickHalfHeight);
            x = x3 + (x4 - x3) * i / 5;
            painter.drawLine(x, rect.top() + marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             x, rect.top() + marginTop + tuneCellPadding + fontHeight / 2 + tickHalfHeight);
        }

        // Delta
        if (_delta != 0)
        {
            // Text position
            int pos = _delta > 0 ?
                        (50 * x3 + (x4 - x3) * _delta) / 50 :
                        (50 * x2 + (x2 - x1) * _delta) / 50;
            QString txt = QString::number(_delta < 0 ? -_delta : _delta);
            int textWidth = fm.horizontalAdvance(txt);

            painter.setBrush(highlightColor);
            painter.setPen(highlightColor);
            painter.drawEllipse(QPoint(pos, rect.top() + marginTop + tuneCellPadding + fontHeight / 2),
                                fontHeight / 2 + tuneCellPadding, fontHeight / 2 + tuneCellPadding);
            painter.fillRect(pos, rect.top() + marginTop + tuneCellPadding + fontHeight / 2 - tickHalfHeight,
                             rect.right() - marginRight - tuneWidth / 2 - pos, 2 * tickHalfHeight,
                             highlightColor);
            painter.setPen(highlightTextColor);
            painter.drawText(QRect(pos - textWidth / 2, rect.top() + marginTop + tuneCellPadding, textWidth, fontHeight),
                             Qt::AlignCenter, txt);
        }
    }

    // Write key name
    {
        QPainterPath path;
        textColor.setAlpha(255);
        path.addRoundedRect(QRect(rect.right() - marginRight - tuneWidth / 2 - textWidth / 2 - tuneCellPadding,
                                   rect.top() + marginTop, textWidth + 2 * tuneCellPadding, fontHeight + 2 * tuneCellPadding), 3, 3);
        painter.fillPath(path, _delta == 0 ? highlightColor : textColor);
        painter.setPen(_delta == 0 ? highlightTextColor : backgroundColor);
        painter.drawText(QRect(rect.right() - marginRight - tuneWidth / 2 - textWidth / 2 - tuneCellPadding,
                               rect.top() + marginTop + tuneCellPadding,
                               textWidth + 2 * tuneCellPadding, fontHeight), Qt::AlignCenter, keyName);
    }

    // MAIN FREQUENCY PEAKS

    textColor.setAlpha(_paintForPng ? 255 : 100);
    painter.setPen(textColor);
    {
        // Compute the text
        QList<QStringList> lines;
        int peakNumber = 0;
        int posY = rect.top() + 2 * tuneCellPadding + fontHeight * 3 / 2;
        while (posY + fontHeight < rect.bottom() && peakNumber < _peaks.count())
        {
            QStringList line;
            line << QLocale::system().toString(_peaks[peakNumber]._factor, 'f', 2)
                 << QLocale::system().toString(_peaks[peakNumber]._frequency, 'f', 2) + " " + tr("Hz", "unit for Herz")
                 << ContextManager::keyName()->getKeyName(static_cast<unsigned int>(_peaks[peakNumber]._key))
                 << (_peaks[peakNumber]._correction > 0 ? "+" : "") + QString::number(_peaks[peakNumber]._correction);
            lines << line;

            peakNumber++;
            posY += fontHeight;
        }
        QString lastTextElement = "/ 100";

        // Compute the column widths
        int columnWidth[5];
        QFontMetrics fm(fontInfo);
        for (int col = 0; col < 4; col++)
        {
            columnWidth[col] = 0;
            for (peakNumber = 0; peakNumber < lines.count(); peakNumber++)
            {
                int w = fm.horizontalAdvance(lines[peakNumber][col]);
                if (w > columnWidth[col])
                    columnWidth[col] = w;
            }
        }
        columnWidth[4] = QFontMetrics(fontInfoSmall).horizontalAdvance(lastTextElement);

        // Margins
        columnWidth[1] += 10;
        columnWidth[2] += 10;
        columnWidth[3] += 10;
        columnWidth[4] += 2;

        // Write the text
        posY = rect.top() + 2 * tuneCellPadding + fontHeight * 3 / 2;
        for (peakNumber = 0; peakNumber < lines.count(); peakNumber++)
        {
            painter.setFont(fontInfo);
            painter.drawText(QRect(0, posY, rect.right() - columnWidth[1] - columnWidth[2] - columnWidth[3] - columnWidth[4] - marginRight, fontHeight),
                             Qt::AlignRight | Qt::AlignVCenter, lines[peakNumber][0]);
            painter.drawText(QRect(0, posY, rect.right() - columnWidth[2] - columnWidth[3] - columnWidth[4] - marginRight, fontHeight),
                             Qt::AlignRight | Qt::AlignVCenter, lines[peakNumber][1]);
            painter.drawText(QRect(0, posY, rect.right() - columnWidth[3] - columnWidth[4] - marginRight, fontHeight),
                             Qt::AlignRight | Qt::AlignVCenter, lines[peakNumber][2]);
            painter.drawText(QRect(0, posY, rect.right() - columnWidth[4] - marginRight, fontHeight),
                             Qt::AlignRight | Qt::AlignVCenter, lines[peakNumber][3]);
            painter.setFont(fontInfoSmall);
            painter.drawText(QRect(0, posY, rect.right() - marginRight, fontHeight),
                             Qt::AlignRight | Qt::AlignVCenter, lastTextElement);
            posY += fontHeight;
        }
    }

    if (_paintForPng)
    {
        // Restore parameters
        QColor color = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
        color.setAlpha(150);
        _wavePainter->setWaveColor(color);
    }
}

void GraphicsFourier::exportPng()
{
    QString defaultFile = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_FREQUENCIES) + "/" +
            _name.replace(QRegularExpression(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_") + ".png";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export a graph"),
                                                    defaultFile, tr("Png file") + " (*.png)");
    if (!fileName.isEmpty())
    {
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_FREQUENCIES, fileName);
        exportPng(fileName);
    }
}

void GraphicsFourier::exportPng(QString fileName)
{
    // Increase the size of this widget
    _paintForPng = true;
    QSize minSize = this->minimumSize();
    this->setMinimumSize(1200, 400);

    // Create the png file
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QPixmap pix = this->grab();
        pix.save(&file, "PNG");
        file.close();
    }

    // Restore this widget size
    _paintForPng = false;
    this->setMinimumSize(minSize);
}

void GraphicsFourier::exportPeaks()
{
    _toolFrequencyPeak->setIds(_currentIds);
    _toolFrequencyPeak->run();
}

void GraphicsFourier::getEstimation(int &pitch, int &correction, float &score)
{
    pitch = _key;
    correction = -_delta;
    score = _score;
}
