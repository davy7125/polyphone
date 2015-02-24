/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#include "rangeeditor.h"
#include "ui_rangeeditor.h"
#include "config.h"
#include "graphicssimpletextitem.h"
#include "rectangleitem.h"

RangeEditor::RangeEditor(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::RangeEditor),
    _scene(new QGraphicsScene(-0.5, -0.5, 128, 128))
{
    ui->setupUi(this);

    // Configuration
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setRenderHint(QPainter::Antialiasing, true);

    // Preparation graphique
    this->setScene(_scene);
    this->initGridAndAxes();
}

RangeEditor::~RangeEditor()
{
    delete ui;
}

void RangeEditor::initGridAndAxes()
{
    QColor lineColor(220, 220, 220);
    QColor textColor(80, 80, 80);

    // Vertical lines
    QPen penVerticalLines(lineColor, 1);
    penVerticalLines.setCosmetic(true);
    for (int note = 12; note <= 120; note += 12)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(note, -0.5, note, 127.5);
        line->setPen(penVerticalLines);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignHCenter + Qt::AlignBottom, line);
        text->setBrush(textColor);
        text->setText(Config::getInstance()->getKeyName(note));
        text->setPos(note, 127.5);
    }

    // Horizontal lines
    QPen penHorizontalLines(lineColor, 1, Qt::DotLine);
    penHorizontalLines.setCosmetic(true);
    for (int vel = 10; vel <= 120; vel += 10)
    {
        QGraphicsLineItem * line = new QGraphicsLineItem(-0.5, 127 - vel, 127.5, 127 - vel);
        line->setPen(penHorizontalLines);
        _scene->addItem(line);
        GraphicsSimpleTextItem * text = new GraphicsSimpleTextItem(Qt::AlignLeft + Qt::AlignVCenter, line);
        text->setBrush(textColor);
        text->setText(QString::number(vel));
        text->setPos(-0.5, 127 - vel);
    }
}

void RangeEditor::display(EltID id)
{
    QList<QRectF> listRect;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else if (id.typeElement == elementPrst)
        id.typeElement = elementPrstInst;

    int count = _sf2->count(id);
    for (int i = 0; i < count; i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            int minKey = 0, maxKey = 127;
            int minVel = 0, maxVel = 127;
            if (_sf2->isSet(id, champ_keyRange))
            {
                rangesType range = _sf2->get(id, champ_keyRange).rValue;
                minKey = range.byLo;
                maxKey = range.byHi;
            }
            if (_sf2->isSet(id, champ_velRange))
            {
                rangesType range = _sf2->get(id, champ_velRange).rValue;
                minVel = range.byLo;
                maxVel = range.byHi;
            }
            listRect << QRectF(-0.5 + minKey, 126.5 - maxVel, maxKey - minKey + 1, maxVel - minVel + 1);
        }
    }

    display(listRect);
}

void RangeEditor::display(QList<QRectF> rectangles)
{
    // Style
    QPen penRectangle(QColor(250, 0, 0, 120), 1);
    penRectangle.setCosmetic(true);
    QBrush brushRectangle(QColor(250, 0, 0, 60));

    // Clear previous rectangles
    foreach (QGraphicsRectItem * item, _rectangles)
        _scene->removeItem(item);
    _rectangles.clear();

    // Add new rectangles
    foreach (QRectF rectangle, rectangles)
    {
        RectangleItem * item = new RectangleItem(rectangle);
        _scene->addItem(item);
        _rectangles << item;
        item->setPen(penRectangle);
        item->setBrush(brushRectangle);
    }
}

void RangeEditor::resizeEvent(QResizeEvent * event)
{
    fitInView(_scene->sceneRect());
    QGraphicsView::resizeEvent(event);
}
