#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include <QToolBar>
#include "page.h"
class ToolMenu;
class StyledAction;

class EditorToolBar : public QToolBar
{
    Q_OBJECT

public:
    EditorToolBar(QWidget *parent = NULL);
    ~EditorToolBar();
    void setSf2Index(int sf2index);
    void updateActions();
    void disable();
    void setDisplayOptions(QList<Page::DisplayOption> displayOptions);

    static void updateRecorderButtonsState(bool isChecked);
    static void updateKeyboardButtonsState(bool isChecked);

public slots:
    void onSelectionChanged(IdList ids);
    void disableDisplayOption(QList<int> disabledOptions);
    void selectDisplayOption(int displayOption);

signals:
    void displayOptionChanged(int displayOption);
    void recorderDisplayChanged(bool isDisplayed);
    void keyboardDisplayChanged(bool isDisplayed);
    void selectionChanged(IdList id);

private slots:
    void onNewSmplClicked();
    void onNewInstClicked();
    void onNewPrstClicked();
    void onSaveClicked();
    void onDisplayActionClicked();
    void onRecorderActionClicked();
    void onKeyboardActionClicked();
    void onUndo();
    void onRedo();

private:
    IdList addSmpl(QString path, EltID id, int *replace);
    QString getName(QString name, int maxCharacters, int suffixNumber);

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
    IdList _currentSelection;

    static QList<EditorToolBar *> s_instances;
    static bool s_recorderOpen;
    static bool s_keyboardOpen;
};

#endif // EDITORTOOLBAR_H
