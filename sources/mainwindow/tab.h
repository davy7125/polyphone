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

#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include "imidilistener.h"
class AbstractInputParser;

class Tab : public QWidget, public IMidiListener
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr);
    ~Tab();

    /// Initialize the tab with a parser that can extract data and build a soundfont
    void initialize(AbstractInputParser * input);
    void initializeWithSoundfontIndex(int indexSf2);

    /// Index of the soundfont created
    int getSf2Index() { return _sf2Index; }

    /// Notify that a change has been made somewhere
    void update(QString editingSource);

signals:
    void tabTitleChanged(QString title);
    void filePathChanged(QString filePath);
    void recorderDisplayChanged(bool isDisplayed);
    void keyboardDisplayChanged(bool isDisplayed);

protected:
    virtual void tabInitializing(QString filename) = 0;
    virtual void tabInError(QString errorMessage) = 0;
    virtual void tabInitialized(int indexSf2) = 0;
    virtual void tabUpdate(QString editingSource) = 0;

private slots:
    void inputProcessed();

private:
    void updateTitleAndPath();
    int _sf2Index;
};

#endif // TAB_H
