/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOG_HELP_H
#define DIALOG_HELP_H

#include <QDialog>
#include <QUrl>
#include <QSplitter>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>

class DialogHelp : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogHelp(QWidget * parent);
    ~DialogHelp();
    
private slots:
    void onTextSourceChanged(const QUrl &arg1, bool store = true);
    void onTreeClicked(QTreeWidgetItem* item, QTreeWidgetItem*);
    void onPreviousClicked();
    void onNextClicked();
    void verticalSliderMoved(int value);

private:
    void createUi();
    void fillTree();
    QTreeWidgetItem * createItem(QString name, QString htmlPage, QTreeWidgetItem * parent = NULL);
    QTreeWidgetItem * findItem(QTreeWidgetItem *item, QString url);
    void updateNextPreviousStates();

    QSplitter * _splitter;
    QTreeWidget * _tree;
    QLabel * _label;
    QTextBrowser * _textBrowser;
    QPushButton * _buttonLeft, * _buttonRight;

    QStringList _urlPrevious, _urlNext;
    QList<int> _posPrevious, _posNext;
    int _valTmp;
};

#endif // DIALOG_HELP_H
