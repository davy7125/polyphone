/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#ifndef DIALOGTIMBRECREATION_H
#define DIALOGTIMBRECREATION_H

#include <QDialog>
#include "timbremodel.h"
class QRadioButton;
class QComboBox;
class QLabel;

// Creation of a computed note at a free key: the root keys used are chosen automatically or by hand
class DialogTimbreCreation : public QDialog
{
    Q_OBJECT

public:
    DialogTimbreCreation(const TimbreModel &model, int key, QWidget * parent = nullptr);

    // Root keys chosen, -1 if automatic
    int getClosestKey();
    int getSecondKey();

    // Automatic choice: the closest root key (the higher one if two are at the same distance), the second one being on
    // the other side of the key if possible, otherwise the next one on the same side (as the tool "create missing samples")
    static void getAutomaticKeys(const QList<int> &rootKeys, int key, int &closest, int &second);

private slots:
    void updateWarning();
    void selectManual();

private:
    int _key;
    QList<int> _rootKeys;
    QRadioButton * _radioAutomatic, * _radioManual;
    QComboBox * _comboClosest, * _comboSecond;
    QLabel * _labelWarning;
};

#endif // DIALOGTIMBRECREATION_H
