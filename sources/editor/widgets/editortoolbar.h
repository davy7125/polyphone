#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <QToolBar>
#include "idlist.h"
#include "page.h"
class ToolMenu;
class StyledAction;

class EditorToolBar : public QToolBar
{
    Q_OBJECT

public:
    EditorToolBar(QWidget *parent = NULL);
    void setSf2Index(int sf2index);
    void updateActions();
    void disable();
    void setDisplayOptions(QList<Page::DisplayOption> displayOptions);

public slots:
    void onSelectionChanged(IdList ids);
    void disableDisplayOption(QList<int> disabledOptions);
    void selectDisplayOption(int displayOption);

signals:
    void displayOptionChanged(int displayOption);

private slots:
    void onDisplayActionClicked();
    void onUndo();
    void onRedo();

private:
    StyledAction * _actionAddSample;
    StyledAction * _actionAddInstrument;
    StyledAction * _actionAddPreset;
    StyledAction * _actionToolBox;
    StyledAction * _actionUndo;
    StyledAction * _actionRedo;
    StyledAction * _actionSave;
    StyledAction * _actionShowRecorder;
    StyledAction * _actionShowKeyboard;

    int _sf2Index;
    QAction * _displayActionSeparator;
    QList<StyledAction *> _displayActions;
    bool _updatingDisplayOptions;
    ToolMenu * _toolMenu;
};

#endif // EDITORTOOLBAR_H
