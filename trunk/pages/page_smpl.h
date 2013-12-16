/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#ifndef PAGE_SMPL_H
#define PAGE_SMPL_H

#include <QWidget>
#include "page.h"
#include "qcustomplot.h"


namespace Ui
{
    class Page_Smpl;
}

class Page_Smpl : public Page
{
    Q_OBJECT
public:
    explicit Page_Smpl(QWidget * parent = 0);
    ~Page_Smpl();

    // Méthodes publiques
    void afficher();
    void normalisation();
    void enleveBlanc();
    void enleveBlanc(EltID id);
    void enleveFin();
    void enleveFin(EltID id);
    void bouclage();
    void filtreMur();
    void reglerBalance();
    void transposer();
    void sifflements();
    bool isPlaying();

    void getPeakFrequencies(EltID id, QList<double> &frequencies, QList<double> &factors, QList<int> &keys, QList<int> &corrections);

signals:
    void noteChanged(int unused, int vel);

private slots:
    void lecture();
    void lecteurFinished();
    void applyEQ();
    void initEQ();
    void sifflements(int freq1, int freq2, double raideur);
    void setStartLoop();
    void setStartLoop(int val);
    void setEndLoop();
    void setEndLoop(int val);
    void setRootKey();
    void setRootKey(int val);
    void setTune();
    void setTune(int val);
    void setType(int index);
    void setLinkedSmpl(int index);
    void setRate(int index);
    void setLoopEnabled(int val);
    void setSinusEnabled(int val);
    void setGainSample(int val);
    void setStereo(int val);
    void selectionChanged();
    void updateColors();

private:
    Ui::Page_Smpl *ui;
    bool lectureEnCours;
    int preventStop;

    // Méthodes privées
    void setRateElt(EltID id, DWORD echFinal);
    EltID getRepercussionID(int num = 0);
};

class Graphique : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Graphique(QWidget *parent = 0);
    bool eventFilter(QObject* object, QEvent* event)
    {
        if (!filterEventEnabled)
            return false;
        if (event->type() == QEvent::MouseButtonPress && object == this)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            // Enregistrement situation
            xInit = this->xAxis2->pixelToCoord(mouseEvent->x());
            yInit = this->yAxis2->pixelToCoord(mouseEvent->y());
            zoomXinit = zoomX;
            zoomYinit = zoomY;
            posXinit = posX;
            posYinit = posY;
            modifiedFlag = false;
            if (mouseEvent->button() == Qt::LeftButton && !zoomFlag)
                dragFlag = true;
            else if (mouseEvent->button() == Qt::RightButton && !dragFlag)
                zoomFlag = true;
        }
        else if (event->type() == QEvent::MouseButtonRelease && object == this)
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                dragFlag = false;
                if (!modifiedFlag)
                {
                    // Modification start loop
                    int val = sizeX / zoomX * \
                            ((zoomX - 1)*posX + xInit);
                    if (this->spinStart && this->spinEnd)
                    {
                        if (this->spinEnd->value() > val)
                        {
                            this->spinEnd->setMinimum(val);
                            this->spinStart->setValue(val);
                            emit(startLoopChanged());
                        }
                    }
                    else
                        this->setStartLoop(val);
                }
                else
                    this->setCursor(Qt::ArrowCursor);
            }
            else if (mouseEvent->button() == Qt::RightButton)
            {
                zoomFlag = false;
                if (!modifiedFlag)
                {
                    // Modification end loop
                    int val = sizeX / zoomX * \
                            ((zoomX - 1)*posX + xInit);
                    if (this->spinStart && this->spinEnd)
                    {
                        if (this->spinStart->value() < val)
                        {
                            this->spinStart->setMaximum(val);
                            this->spinEnd->setValue(val);
                            emit(endLoopChanged());
                        }
                    }
                    else
                        this->setEndLoop(val);
                }
                else
                {
                    this->setZoomLine(-1, 0, 0, 0);
                    this->setCursor(Qt::ArrowCursor);
                }
            }
        }
        else if (event->type() == QEvent::MouseMove && object == this)
        {
            if (zoomFlag)
            {
                if (!modifiedFlag)
                {
                    modifiedFlag = true;
                    this->setCursor(Qt::ArrowCursor);
                }
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                this->zoom(mouseEvent->pos());
                // Ligne de zoom
                this->setZoomLine(xInit, yInit, \
                                  this->xAxis2->pixelToCoord(mouseEvent->x()), \
                                  this->yAxis2->pixelToCoord(mouseEvent->y()));
            }
            else if (dragFlag)
            {
                if (!modifiedFlag)
                {
                    modifiedFlag = true;
                    this->setCursor(Qt::SizeAllCursor);
                }
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                this->drag(mouseEvent->pos());
            }
        }
        return false;
    }
    // Méthodes publiques
    void clearAll();
    void setData(QByteArray baData);
    void linkSliderX(QScrollBar * qScrollX);
    void linkSpinBoxes(QSpinBox * spinStart, QSpinBox * spinEnd);
    void zoomDrag();
    void updateStyle();
    void displayMultipleSelection(bool isOn);

public slots:
    void setPosX(int posX);
    void setStartLoop(int pos, bool replot = true);
    void setEndLoop(int pos, bool replot = true);
    void setCurrentSample(int pos);

signals:
    void startLoopChanged();
    void endLoopChanged();

protected:
    void paintEvent(QPaintEvent *event);

private:
    // Attributs privés
    QPen penLecture;
    double sizeX;
    bool zoomFlag;
    bool dragFlag;
    bool modifiedFlag;
    double xInit, yInit;
    double zoomX, zoomY, posX, posY;
    double zoomXinit, zoomYinit, posXinit, posYinit;
    QPoint pointInit;
    bool bFromExt;
    QScrollBar * qScrollX;
    QSpinBox * spinStart;
    QSpinBox * spinEnd;
    int m_currentPos;
    bool filterEventEnabled;
    QCPItemText * textMultipleSelection;

    // Méthodes privées
    void zoom(QPoint point);
    void drag(QPoint point);
    void setZoomLine(double x1, double y1, double x2, double y2);
    void plotOverlay();
};

class GraphiqueFourier : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphiqueFourier(QWidget *parent = 0);
    void setBackgroundColor(QColor color);
    void setData(QByteArray baData, DWORD dwSmplRate);
    void setSampleName(QString name) { _name = name; }
    void setPos(qint32 posStart, qint32 posEnd);
    void setPos(qint32 posStart, qint32 posEnd, QList<double> &frequencies, QList<double> &factors,
                QList<int> &pitch, QList<int> &corrections);

protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void exportPng();

private:
    QByteArray baData;
    DWORD dwSmplRate;
    QString _name;
    QCPItemText * text1; // estimation
    QCPItemText * text2; // intensité
    QCPItemText * text3; // fréquence
    QCPItemText * text4; // note
    QCPItemText * text5; // correction
    QMenu * _menu;
    void resized();
    void exportPng(QString fileName);
};


#endif // PAGE_SMPL_H
