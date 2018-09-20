#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include "idlist.h"
class AbstractInput;

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();

    /// Initialize the editor with a parser that can extract data and build a soundfont
    void initialize(AbstractInput * input);

    /// Index of the soundfont created
    int getSf2Index() { return _sf2Index; }

    /// Notify that a change has been made somewhere
    void update(QString editingSource);

signals:
    void tabTitleChanged(QString title);

public slots:

private slots:
    void inputProcessed();
    void onSelectionChanged(IdList ids);
    void displayOptionChanged(int displayOption);

private:
    void updateTitle();

    Ui::Editor *ui;
    int _sf2Index;
};

#endif // EDITOR_H
