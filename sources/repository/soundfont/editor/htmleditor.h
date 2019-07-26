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

#ifndef HTMLEDITOR_H
#define HTMLEDITOR_H

#include <QWidget>
class SoundfontDescriptionData;

namespace Ui {
class HtmlEditor;
}

class HtmlEditor : public QWidget
{
    Q_OBJECT

public:
    explicit HtmlEditor(QWidget *parent = nullptr);
    ~HtmlEditor();

    // Initialize the description
    void initialize(SoundfontDescriptionData *data);

private slots:
    void on_pushHtml_clicked(bool checked);
    void on_textEdit_cursorPositionChanged();

    void setStyleNormal();
    void setStyleQuote();
    void setStyleCode();
    void setStyleHeader1();
    void setStyleHeader2();
    void setStyleHeader3();
    void setStyleHeader4();

    void on_pushBold_clicked(bool checked);
    void on_pushItalic_clicked(bool checked);
    void on_pushUnderline_clicked(bool checked);
    void on_pushStrikeThrough_clicked(bool checked);

    void on_pushUl_clicked(bool checked);
    void on_pushOl_clicked(bool checked);
    void setAlignLeft();
    void setAlignCenter();
    void setAlignRight();
    void setAlignJustify();

    void on_pushImage_clicked();
    void insertImage(QString url);
    void on_pushVideo_clicked();
    void insertYouTubeVideo(QString videoId);
    void insertTable();
    void insertTable(int rows, int columns);
    void addRowAbove();
    void addRowBelow();
    void addColumnLeft();
    void addColumnRight();
    void deleteRow();
    void deleteColumn();
    void deleteTable();
    void link();
    void link(QString text, QString link);
    void unlink();

    void on_pushHr_clicked();

private:
    Ui::HtmlEditor *ui;
};

#endif // HTMLEDITOR_H
