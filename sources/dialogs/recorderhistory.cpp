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

#include "recorderhistory.h"
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>
#include <QVBoxLayout>
#include "contextmanager.h"
#include "elidedlabel.h"

// Dimensions (pixels)
static const int LINE_HEIGHT = 26;
static const int ICON_SIZE = 16;

RecorderHistory::RecorderHistory(QWidget * parent) : QFrame(parent),
    _selectedIndex(-1),
    _isFirstFileBusy(false),
    _isImportAvailable(false)
{
    ThemeManager * theme = ContextManager::theme();
    this->setStyleSheet("QFrame{background-color:" + theme->getColor(ThemeManager::LIST_BACKGROUND).name() + ";color:" +
                        theme->getColor(ThemeManager::LIST_TEXT).name() + "}" +
                        "QFrame[alternate=\"true\"],QFrame[alternate=\"true\"] QLabel{background-color:" +
                        theme->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND).name() + "}" +
                        "QFrame[selected=\"true\"],QFrame[selected=\"true\"] QWidget{background-color:" +
                        theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                        theme->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}" +
                        "QToolButton{border:0;background:transparent}" +
                        "QLabel#labelEmpty{color:" + ThemeManager::mix(theme->getColor(ThemeManager::LIST_BACKGROUND),
                                                                       theme->getColor(ThemeManager::LIST_TEXT), 0.6).name() + "}");

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    for (int i = 0; i < LINE_COUNT; i++)
    {
        Line &line = _lines[i];
        line.widget = new QFrame(this);
        line.widget->setFixedHeight(LINE_HEIGHT);
        // Alternative background from the first line, which then stands out from the area above (background of the lists)
        line.widget->setProperty("alternate", i % 2 == 0);
        line.widget->installEventFilter(this);

        line.labelName = new ElidedLabel(line.widget);
        line.labelName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        line.labelName->setMinimumWidth(100);
        line.labelDetail = new QLabel(line.widget);
        line.labelDetail->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QHBoxLayout * lineLayout = new QHBoxLayout(line.widget);
        lineLayout->setContentsMargins(6, 0, 4, 0);
        lineLayout->setSpacing(6);
        lineLayout->addWidget(line.labelName, 1);
        lineLayout->addWidget(line.labelDetail);
        layout->addWidget(line.widget);
    }

    _buttons = new QWidget(this);
    QHBoxLayout * buttonLayout = new QHBoxLayout(_buttons);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(2);
    _buttonListen = createButton(":/icons/recorder_play.svg", tr("Listen to the recording"));
    _buttonFolder = createButton(":/icons/folder.svg", tr("Open the folder of the recording"));
    _buttonDelete = createButton(":/icons/trash.svg", tr("Delete the recording"));
    _buttonImport = createButton(":/icons/download.svg", "");
    connect(_buttonListen, &QToolButton::clicked, this, [this]() { emit listenRequested(_files.value(_selectedIndex)); });
    connect(_buttonFolder, &QToolButton::clicked, this, [this]() { emit folderRequested(_files.value(_selectedIndex)); });
    connect(_buttonDelete, &QToolButton::clicked, this, [this]() { emit deleteRequested(_files.value(_selectedIndex)); });
    connect(_buttonImport, &QToolButton::clicked, this, [this]() { emit importRequested(_files.value(_selectedIndex)); });
    _buttons->hide();

    // Above the empty lines, which remain so that the size of the window doesn't change
    _labelEmpty = new QLabel(this);
    _labelEmpty->setObjectName("labelEmpty");
    _labelEmpty->setAlignment(Qt::AlignCenter);
    _labelEmpty->setWordWrap(true);
    _labelEmpty->setContentsMargins(20, 0, 20, 0);

    updateButtons();
}

QToolButton * RecorderHistory::createButton(QString svgPath, QString toolTip)
{
    QToolButton * button = new QToolButton(_buttons);
    button->setIcon(ContextManager::theme()->getBannerIcon(svgPath, ICON_SIZE));
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolTip(toolTip);
    _buttons->layout()->addWidget(button);
    return button;
}

void RecorderHistory::setFiles(const QStringList &files, const QStringList &details)
{
    _files = files.mid(0, LINE_COUNT);
    for (int i = 0; i < LINE_COUNT; i++)
    {
        _lines[i].labelName->setTextToElide(QFileInfo(_files.value(i)).fileName());
        _lines[i].labelName->setToolTip(QDir::toNativeSeparators(_files.value(i)));
        _lines[i].labelDetail->setText(details.value(i));
    }
    select(_files.isEmpty() ? -1 : 0);
    _labelEmpty->setVisible(_files.isEmpty());
}

void RecorderHistory::setEmptyMessage(QString message)
{
    _labelEmpty->setText(message);
}

void RecorderHistory::resizeEvent(QResizeEvent * event)
{
    QFrame::resizeEvent(event);
    _labelEmpty->setGeometry(this->rect());
}

void RecorderHistory::setFirstDetail(QString detail)
{
    _lines[0].labelDetail->setText(detail);
}

void RecorderHistory::setFirstFileBusy(bool isBusy)
{
    _isFirstFileBusy = isBusy;
    updateButtons();
}

void RecorderHistory::setPlayingFile(QString file)
{
    _playingFile = file;
    updateButtons();
}

void RecorderHistory::setImportDisplayed(bool isDisplayed)
{
    _buttonImport->setVisible(isDisplayed);
}

void RecorderHistory::setImportAvailable(bool isAvailable)
{
    _isImportAvailable = isAvailable;
    updateButtons();
}

void RecorderHistory::select(int index)
{
    // The selected line has the buttons, and a specific style
    for (int i = 0; i < LINE_COUNT; i++)
    {
        _lines[i].widget->setProperty("selected", i == index);
        _lines[i].widget->style()->unpolish(_lines[i].widget);
        _lines[i].widget->style()->polish(_lines[i].widget);
        foreach (QWidget * child, _lines[i].widget->findChildren<QWidget *>())
        {
            child->style()->unpolish(child);
            child->style()->polish(child);
        }
    }

    _selectedIndex = index;
    if (index >= 0)
    {
        _buttons->setParent(_lines[index].widget);
        _lines[index].widget->layout()->addWidget(_buttons);
        _buttons->show();
    }
    else
    {
        _buttons->hide();
        _buttons->setParent(this);
    }
    updateButtons();
}

bool RecorderHistory::isUsable(int index)
{
    return !(index == 0 && _isFirstFileBusy);
}

void RecorderHistory::updateButtons()
{
    bool isUsable = this->isUsable(_selectedIndex);
    bool isPlaying = !_playingFile.isEmpty() && _files.value(_selectedIndex) == _playingFile;
    _buttonListen->setEnabled(isUsable);
    _buttonListen->setIcon(ContextManager::theme()->getBannerIcon(isPlaying ? ":/icons/recorder_stop.svg" : ":/icons/recorder_play.svg", ICON_SIZE));
    _buttonListen->setToolTip(isPlaying ? tr("Stop listening") : tr("Listen to the recording"));
    _buttonDelete->setEnabled(isUsable);
    _buttonImport->setEnabled(isUsable && _isImportAvailable);
    _buttonImport->setToolTip(_isImportAvailable ? tr("Import the recording in the soundfont displayed") :
                                                   tr("Display a soundfont in the editor for importing the recording"));
}

bool RecorderHistory::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
    {
        for (int i = 0; i < _files.count(); i++)
        {
            if (watched == _lines[i].widget)
            {
                // While a file is played, another file selected is played instead
                bool playsAnotherFile = !_playingFile.isEmpty() && _files[i] != _playingFile && isUsable(i);
                select(i);
                if (playsAnotherFile)
                    emit listenRequested(_files[i]);
                return true;
            }
        }
    }
    return QFrame::eventFilter(watched, event);
}
