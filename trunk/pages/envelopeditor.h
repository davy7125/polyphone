#ifndef ENVELOPEDITOR_H
#define ENVELOPEDITOR_H

#include <QWidget>
#include "pile_sf2.h"
#include "graphicsviewenvelop.h"
#include "envelop.h"

namespace Ui {
class EnvelopEditor;
}

class EnvelopEditor : public QWidget
{
    Q_OBJECT

public:
    explicit EnvelopEditor(QWidget *parent = 0);
    ~EnvelopEditor();

    void init(Pile_sf2 * sf2);
    void display(QList<EltID> ids);

signals:
    void valueChanged();

private slots:
    void on_pushVolume_clicked();
    void on_pushModulation_clicked();
    void populate();
    void on_doubleSpinDelay_editingFinished();
    void on_doubleSpinAttack_editingFinished();
    void on_doubleSpinHold_editingFinished();
    void on_doubleSpinDecay_editingFinished();
    void on_doubleSpinSustain_editingFinished();
    void on_doubleSpinRelease_editingFinished();
    void on_spinKeyHold_editingFinished();
    void on_spinKeyDecay_editingFinished();
    void on_pushAttack_clicked();
    void on_pushDelay_clicked();
    void on_pushHold_clicked();
    void on_pushKeyHold_clicked();
    void on_pushSustain_clicked();
    void on_pushRelease_clicked();
    void on_pushDecay_clicked();
    void on_pushKeyDecay_clicked();

private:
    Ui::EnvelopEditor *ui;
    Pile_sf2 * _sf2;
    QList<EltID> _displayedElt;
    bool _isVolume;

    void enableEditor(bool isEnabled);
    void stopSignals(bool isStopped);
    void addEnvelop(EltID id, int index);
    double computeValue(EltID id, Champ champ, bool &isOverriden);
    void processEdit(Champ champ, double value);
    void processClear(Champ champ);
};

#endif // ENVELOPEDITOR_H
