/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "extension_midi_dialog.h"
#include <QIcon>
#include <QGridLayout>
#include <QKeyEvent>
#include "contextmanager.h"

ExtensionMidiDialog::ExtensionMidiDialog(QString title, QString identifier, QWidget * gui) :
    QDialog(nullptr, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    _identifier(identifier)
{
    // Icon and title
    this->setWindowIcon(QIcon(":/misc/polyphone.png"));
    this->setWindowTitle(title);

    // Content
    QGridLayout * layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(gui);
    this->setLayout(layout);

    // Restore the geometry
    QByteArray geometry = ContextManager::configuration()->getExtensionValue(_identifier, "geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty())
        this->restoreGeometry(geometry);
}

ExtensionMidiDialog::~ExtensionMidiDialog()
{
    // Remove the GUI from the dialog
    this->layout()->takeAt(0);
}

void ExtensionMidiDialog::closeEvent(QCloseEvent *event)
{
    ContextManager::configuration()->setExtensionValue(_identifier, "geometry", this->saveGeometry());
    QDialog::closeEvent(event);
}

void ExtensionMidiDialog::keyPressEvent(QKeyEvent * event)
{
    // Prevent the escape key from closing the window
    if (event->key() != Qt::Key_Escape)
          QDialog::keyPressEvent(event);
}
