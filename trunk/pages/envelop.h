#ifndef ENVELOP_H
#define ENVELOP_H

#include <QMap>
#include <QSize>
class QGraphicsLineItem;
class QGraphicsScene;

class Envelop
{
public:
    enum ValueType {
        DELAY,
        ATTACK,
        HOLD,
        DECAY,
        SUSTAIN,
        RELEASE,
        KEYNUM_TO_HOLD,
        KEYNUM_TO_DECAY
    };

    Envelop(QGraphicsScene * scene, Envelop * parent = NULL);
    ~Envelop();

    void set(ValueType type, double value, bool defined = true);
    bool isDefined(ValueType type);
    double get(ValueType type);

    static void computeLimits();

    void replot(QSize size, double releaseTime, double totalTime);

private:
    QMap<ValueType, double> _values;
    QMap<ValueType, bool> _defined;
    QList<QGraphicsLineItem *> _lines;
    QGraphicsScene * _scene;
    Envelop * _parentEnvelop;

    static QList<Envelop*> s_envelops;
    static double s_releaseTime;
    static double s_totalTime;
};

#endif // ENVELOP_H
