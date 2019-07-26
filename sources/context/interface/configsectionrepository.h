/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef CONFIGSECTIONREPOSITORY_H
#define CONFIGSECTIONREPOSITORY_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class ConfigSectionRepository;
}

class ConfigSectionRepository : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionRepository(QWidget *parent = nullptr);
    ~ConfigSectionRepository();
    void initialize();

private slots:
    void onConnectionStateChanged(UserManager::ConnectionState connectionState);

    void on_lineUser_textEdited(const QString &arg1);
    void on_linePassword_textChanged(const QString &arg1);
    void on_lineDownloadDirectory_editingFinished();

    void on_buttonCreateAccount_clicked();
    void on_buttonBecomePremium_clicked();
    void on_linePassword_returnPressed();
    void on_pushConnect_clicked();

    void on_pushDirectory_clicked();

private:
    Ui::ConfigSectionRepository *ui;
    UserManager::ConnectionState _currentState;
    bool _fakePassword;
};

#endif // CONFIGSECTIONREPOSITORY_H
