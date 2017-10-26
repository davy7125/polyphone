#include "envelop.h"
#include <QGraphicsLineItem>
#include <QGraphicsScene>

QList<Envelop*> s_envelops = QList<Envelop*>();
double Envelop::s_releaseTime = 5.0;
double Envelop::s_totalTime = 10.0;

Envelop::Envelop(QGraphicsScene * scene, Envelop *parent) :
    _scene(scene),
    _parentEnvelop(parent)
{
    // Default values
    _values[DELAY] = 0;
    _values[ATTACK] = 0;
    _values[HOLD] = 0;
    _values[DECAY] = 0;
    _values[SUSTAIN] = 0;
    _values[RELEASE] = 0;
    _values[KEYNUM_TO_HOLD] = 0;
    _values[KEYNUM_TO_DECAY] = 0;

    _defined[DELAY] = false;
    _defined[ATTACK] = false;
    _defined[HOLD] = false;
    _defined[DECAY] = false;
    _defined[SUSTAIN] = false;
    _defined[RELEASE] = false;
    _defined[KEYNUM_TO_HOLD] = false;
    _defined[KEYNUM_TO_DECAY] = false;

    // Create lines
    QPen pen;
    pen.setWidth(2);
    pen.setCosmetic(true);
    QGraphicsLineItem * line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
    line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
    line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
    line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
    line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
    line = new QGraphicsLineItem();
    line->setPen(pen);
    _scene->addItem(line);
    _lines << line;
}

Envelop::~Envelop()
{
    while (!_lines.isEmpty())
        delete _lines.takeFirst();
}

void Envelop::set(ValueType type, double value, bool defined)
{
    _values[type] = value;
    _defined[type] = defined;
}

bool Envelop::isDefined(ValueType type)
{
    return _defined[type];
}

double Envelop::get(ValueType type)
{
    return _defined[type] ? _values[type] : (_parentEnvelop == NULL ? 0 : _parentEnvelop->get(type));
}

void Envelop::replot(QSize size, double releaseTime, double totalTime)
{
    /*double count = 0;

    // First line: delay
    double length = get(DELAY);
    _lines[0]->setLine(0, s_max, length, s_max);
    count += length;

    // Second line: attack
    length = get(ATTACK);
    _lines[1]->setLine(count, s_max, count + length, 0);
    count += length;

    // Third line: hold
    length = get(HOLD);
    _lines[2]->setLine(count, 0, count + length, 0);
    count += length;

    // Fourth line: decay
    length = get(DECAY);
    double sustainLevel = get(SUSTAIN);
    _lines[3]->setLine(count, 0, count + length, sustainLevel);
    count += length;

    // Fifth line: sustain
    _lines[4]->setLine(count, sustainLevel, s_releasePosition, sustainLevel);

    // Sixth line: release
    length = get(RELEASE);
    _lines[5]->setLine(s_releasePosition, sustainLevel, s_releasePosition + length, s_max);*/
}

void Envelop::computeLimits()
{

}
