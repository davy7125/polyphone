#include "dialog_visualizer.h"
#include "ui_dialog_visualizer.h"
#include "config.h"

DialogVisualizer::DialogVisualizer(Pile_sf2 *sf2, EltID id, QWidget *parent) :
    QDialog(parent),
    _sf2(sf2),
    ui(new Ui::DialogVisualizer),
    _initialID(id)
{
    ui->setupUi(this);
}

DialogVisualizer::~DialogVisualizer()
{
    delete ui;
}

void DialogVisualizer::on_comboParameter_currentIndexChanged(int index)
{

}

void DialogVisualizer::on_checkLog_stateChanged(int arg1)
{

}


///////////////
// GRAPHIQUE //
///////////////

GraphVisualizer::GraphVisualizer(QWidget *parent) :
    QCustomPlot(parent)
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
        x[2*i] = x[2*i+1] = note;
        QCPItemText *textLabel = new QCPItemText(this);
        this->addItem(textLabel);
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(note, 0);
        textLabel->setText(QString::number(note));
        textLabel->setFont(QFont(font().family(), 8));
        textLabel->setColor(QColor(40, 40, 40));
    }
    this->graph(0)->setData(x, y);

    // Layers coloration zone sur laquelle s'étend le clavier
    this->addGraph();
    x.resize(2);
    y.resize(2);
    x[0] = -1;
    x[1] = 128;
    y[0] = y[1] = -2;
    graphPen.setWidth(0);
    this->graph(1)->setPen(graphPen);
    this->graph(1)->setData(x, y);
    this->graph(1)->setBrush(QBrush(QColor(255, 255, 0, 30)));
    this->addGraph();
    this->graph(2)->setPen(graphPen);
    this->graph(1)->setChannelFillGraph(this->graph(2));
}

void GraphVisualizer::mouseMoved(QPoint pos)
{

}

void GraphVisualizer::mouseLeft()
{

}
