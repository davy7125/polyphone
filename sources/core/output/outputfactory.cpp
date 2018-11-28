#include "outputfactory.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "abstractoutput.h"
#include "sf2/outputsf2.h"
#include "sf3/outputsf3.h"
#include "sfz/outputsfz.h"
#include "not_supported/outputnotsupported.h"
#include "empty/outputdummy.h"

AbstractOutput * OutputFactory::getOutput(QString fileName)
{
    AbstractOutput * output = nullptr;

    if (fileName.isEmpty())
    {
        // Do nothing
        output = new OutputDummy();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "sf2")
        {
            // Format sf2
            output = new OutputSf2();
        }
        else if (extension == "sf3")
        {
            // Format sf3
            output = new OutputSf3();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            output = new OutputSfz();
        }
        else
        {
            // Input "not supported format"
            output = new OutputNotSupported();
        }
    }
    output->initialize(fileName, SoundfontManager::getInstance());

    return output;
}

bool OutputFactory::save(int indexSf2, bool saveAs)
{
    // Check that the soundfont is valid
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, indexSf2);
    if (indexSf2 == -1 || !sm->isValid(id))
        return false;

    // Don't go further if the file is already saved
    if (!sm->isEdited(id.indexSf2) && !saveAs)
        return false;

    // Path of the file for saving the soundfont
    QString savePath;
    QString filePathInitial = sm->getQstr(id, champ_filenameInitial);
    QString filePathForData = sm->getQstr(id, champ_filenameForData);
    if (saveAs || filePathInitial != filePathForData || filePathInitial.isEmpty())
    {
        // Default path for selecting the destination
        QString defaultPath;
        if (filePathInitial.isEmpty())
        {
            // A new file is to be saved, the path is based on the internal name and the recent files
            QString currentName = sm->getQstr(id, champ_name);
            if (currentName.isEmpty())
                currentName = QObject::trUtf8("untitled");
            defaultPath = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT) + "/" + currentName + ".sf2";
        }
        else if (filePathInitial != filePathForData)
        {
            // The soundfont to be saved was imported => the path is based on the initial file with another extension
            QFileInfo fi(filePathInitial);
            defaultPath = fi.absolutePath() + "/" + fi.completeBaseName() + ".sf2";
        }
        else
            defaultPath = filePathInitial;

        // Dialog for choosing a destination
        savePath = QFileDialog::getSaveFileName(QApplication::activeWindow(), QObject::trUtf8("Save a soundfont"),
                                                defaultPath, QObject::trUtf8("Sf2 files") + " (*.sf2)");
        if (savePath.isNull())
            return false;
    }
    else
        savePath = filePathInitial;

    AbstractOutput * output = OutputFactory::getOutput(savePath);
    output->setOption("override", true);
    output->process(id.indexSf2, false);

    bool success = output->isSuccess();
    if (success)
    {
        // Possibly delete a temporary file
        if (filePathInitial != filePathForData && !filePathForData.isEmpty())
            QFile::remove(filePathForData);

        // New recent file
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SOUNDFONT, savePath);
    }
    else
        QMessageBox::warning(QApplication::activeWindow(), QObject::trUtf8("Warning"), output->getError());
    delete output;

    return success;
}
