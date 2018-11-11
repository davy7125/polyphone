#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
class ColoredTabWidget;
class ConfigPanel;
class SoundfontFilter;
class SoundfontBrowser;
class Editor;
class UserArea;
class SoundfontViewer;

class WindowManager : public QObject
{
    Q_OBJECT

public:
    explicit WindowManager(ColoredTabWidget * tabWidget);
    ~WindowManager();

public slots:
    /// Open the configuration
    void openConfiguration();

    /// Open the soundfont browser
    void openSoundfont(QString fileName);
    void openNewSoundfont();

    /// Open the repository, initialized with a filter
    /// Note: this function delete the filter, no need to do it elsewhere
    /// The filter can be NULL
    void openRepository(SoundfontFilter * filter);

    /// Open a soundfont available on the repository
    void openRepositorySoundfont(int id);

    /// Open the user profile
    void openUser();

    /// The function is called when pages have to be updated
    void editingDone(QString source, QList<int> sf2Indexes);

    /// Get the current sf2 if an editor is displayed, or -1
    int getCurrentSf2();

    /// Close the current tab
    void closeCurrentTab();

signals:
    /// Emitted when the visibility of the recorder changes
    void recorderDisplayChanged(bool isDisplayed);

    /// Emitted when the visibility of the keyboard changes
    void keyboardDisplayChanged(bool isDisplayed);

    /// Emitted when the current tab changed
    void editorOpen(bool isOpen);

private slots:
    /// Called when the name or the filepath of a soundfont changed
    void onTabTitleChanged(QString title);
    void onFilePathChanged(QString filePath);

    /// Called when the user wants to close a tab
    void onTabCloseRequested(int tabIndex);

    /// Called every time the tab changes
    void onTabIndexChanged(int tabIndex);

private:
    ColoredTabWidget * _tabWidget;
    ConfigPanel * _configTab;
    SoundfontBrowser * _browserTab;
    UserArea * _userTab;
    QList<Editor*> _editors;
    QList<SoundfontViewer*> _viewers;
};

#endif // WINDOWMANAGER_H
