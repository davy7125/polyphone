#ifndef TABLEKEY_H
#define TABLEKEY_H

#include <QTableWidget>
#include <QSignalMapper>

class KeyMapper;

class TableKey : public QTableWidget
{
    Q_OBJECT

public:
    TableKey(QWidget * parent = NULL);
    void setMapper(KeyMapper * mapper);
    void setOctave(int octave);

signals:
    void combinaisonChanged(int numKey, QString combinaison);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void rowChanged(int row);

private:
    KeyMapper * _mapper;
    int _rootKey;
    QString getName(int note);
    QSignalMapper * _signalMapper;
};


#endif // TABLEKEY_H
