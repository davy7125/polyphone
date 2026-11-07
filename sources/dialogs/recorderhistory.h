/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef RECORDERHISTORY_H
#define RECORDERHISTORY_H

#include <QFrame>
#include <QStringList>
class ElidedLabel;
class QLabel;
class QToolButton;

// Last files of the recorder, from the most recent one. The buttons using a file are displayed on the line selected with
// a click.
class RecorderHistory : public QFrame
{
    Q_OBJECT

public:
    static const int LINE_COUNT = 5;

    explicit RecorderHistory(QWidget * parent = nullptr);

    // Files with a text on the right side of each one (at most LINE_COUNT), the first one being selected
    void setFiles(const QStringList &files, const QStringList &details);

    // Message displayed as long as there is no file
    void setEmptyMessage(QString message);

    // Update the text of the first line, for a file being recorded
    void setFirstDetail(QString detail);

    // The first file is being recorded: it can't be used
    void setFirstFileBusy(bool isBusy);

    // File being played (empty if none): the play button of its line stops it, and selecting another line plays its
    // file instead
    void setPlayingFile(QString file);

    // The button for importing a file is only there for the audio files and available in a soundfont editor
    void setImportDisplayed(bool isDisplayed);
    void setImportAvailable(bool isAvailable);

signals:
    void listenRequested(QString file);
    void folderRequested(QString file);
    void deleteRequested(QString file);
    void importRequested(QString file);

protected:
    bool eventFilter(QObject * watched, QEvent * event) override;
    void resizeEvent(QResizeEvent * event) override;

private:
    struct Line
    {
        QWidget * widget;
        ElidedLabel * labelName;
        QLabel * labelDetail;
    };

    QToolButton * createButton(QString svgPath, QString toolTip);
    void select(int index);
    bool isUsable(int index);
    void updateButtons();

    Line _lines[LINE_COUNT];
    QLabel * _labelEmpty;
    QWidget * _buttons; // On the line selected
    QToolButton * _buttonListen;
    QToolButton * _buttonFolder;
    QToolButton * _buttonDelete;
    QToolButton * _buttonImport;
    QStringList _files;
    QString _playingFile;
    int _selectedIndex;
    bool _isFirstFileBusy;
    bool _isImportAvailable;
};

#endif // RECORDERHISTORY_H
