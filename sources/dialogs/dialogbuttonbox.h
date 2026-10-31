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

#ifndef DIALOGBUTTONBOX_H
#define DIALOGBUTTONBOX_H

#include <QDialogButtonBox>

/// Button box with keyboard shortcuts:
/// * Ctrl+Enter clicks the validation button (accept or yes role), if there is only one,
/// * Escape clicks the cancel / close button (reject role), if there is only one,
///   so that the result is the same as with the mouse (otherwise the dialog is only rejected)
class DialogButtonBox : public QDialogButtonBox
{
    Q_OBJECT

public:
    explicit DialogButtonBox(QWidget * parent = nullptr);

protected:
    bool event(QEvent * event) override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void onValidationShortcut();

private:
    /// Return the only visible button having one of the roles, nullptr if there are none or several
    QAbstractButton * getSingleButton(QList<ButtonRole> roles);
};

#endif // DIALOGBUTTONBOX_H
