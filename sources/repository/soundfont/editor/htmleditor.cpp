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

#include "htmleditor.h"
#include "ui_htmleditor.h"
#include "soundfontdescriptiondata.h"
#include "contextmanager.h"
#include "editordialoginserttable.h"
#include "editordialoginsertlink.h"
#include "editordialoginsertimage.h"
#include "editordialoginsertvideo.h"
#include <QMenu>
#include <QTextList>
#include <QTextTable>

HtmlEditor::HtmlEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HtmlEditor)
{
    ui->setupUi(this);

    // Colors
    QString buttonBackground = ContextManager::theme()->getColor(ThemeManager::BUTTON_BACKGROUND).name();
    QString buttonText = ContextManager::theme()->getColor(ThemeManager::BUTTON_TEXT).name();
    QString border = this->palette().dark().color().name();
    QString highlight = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    bool isDark = ContextManager::theme()->isDark(ThemeManager::BUTTON_BACKGROUND, ThemeManager::BUTTON_TEXT);

    // Style
    ui->frame->setStyleSheet("QFrame{background-color:" + buttonBackground + "}");
    ui->textEdit->setStyleSheet("QTextEdit{border: 1px solid " + border + ";border-top:0}");
    ui->plainTextEdit->setStyleSheet("QPlainTextEdit{border: 1px solid " + border + ";border-top:0" +
                                     ";background-color:" + (isDark ? buttonBackground : buttonText) +
                                     ";color:" + (isDark ? buttonText : buttonBackground) + "}");

    // Icons
    ui->pushHtml->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/code.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushStyle->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/paragraph.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushBold->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/bold.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushItalic->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/italic.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushUnderline->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/underline.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushStrikeThrough->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/strikethrough.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushHr->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/hr.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushUl->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/list-ul.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushOl->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/list-ol.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushAlign->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/align-left.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushImage->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/image.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushVideo->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/video.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushTable->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/table.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushLink->setIcon(ContextManager::theme()->getColoredSvg(":/html_editor/link.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    // Menu "style"
    QMenu * menu = new QMenu(ui->pushStyle);
    menu->setStyleSheet("QMenu::item{padding:5px 10px 5px 10px}QMenu::item:selected{background-color:" + highlight + ";color:" + highlightedText + "}");
    ui->pushStyle->setMenu(menu);

    menu->addAction("Normal text", this, SLOT(setStyleNormal()));

    QAction * action = menu->addAction("Quote", this, SLOT(setStyleQuote()));
    QFont font = this->font();
    font.setItalic(true);
    action->setFont(font);

    action = menu->addAction("Code", this, SLOT(setStyleCode()));
    font = this->font();
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setFamily("monospace");
    action->setFont(font);

    action = menu->addAction("Header 1", this, SLOT(setStyleHeader1()));
    font = this->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 8);
    action->setFont(font);

    action = menu->addAction("Header 2", this, SLOT(setStyleHeader2()));
    font.setPointSize(font.pointSize() - 2);
    action->setFont(font);

    action = menu->addAction("Header 3", this, SLOT(setStyleHeader3()));
    font.setPointSize(font.pointSize() - 2);
    action->setFont(font);

    action = menu->addAction("Header 4", this, SLOT(setStyleHeader4()));
    font.setPointSize(font.pointSize() - 2);
    action->setFont(font);

    // Menu "align"
    menu = new QMenu(ui->pushAlign);
    menu->setStyleSheet("QMenu::item{padding:5px 10px 5px 10px}QMenu::item:selected{background-color:" + highlight + ";color:" + highlightedText + "}");
    ui->pushAlign->setMenu(menu);
    menu->addAction("Align text to the left", this, SLOT(setAlignLeft()));
    menu->addAction("Center text", this, SLOT(setAlignCenter()));
    menu->addAction("Align text to the right", this, SLOT(setAlignRight()));
    menu->addAction("Justify text", this, SLOT(setAlignJustify()));

    // Menu "table"
    menu = new QMenu(ui->pushTable);
    menu->setStyleSheet("QMenu::item{padding:5px 10px 5px 10px}QMenu::item:selected{background-color:" + highlight + ";color:" + highlightedText + "}");
    ui->pushTable->setMenu(menu);
    menu->addAction("Insert table", this, SLOT(insertTable()));
    menu->addSeparator();
    menu->addAction("Add row above", this, SLOT(addRowAbove()));
    menu->addAction("Add row below", this, SLOT(addRowBelow()));
    menu->addAction("Add column left", this, SLOT(addColumnLeft()));
    menu->addAction("Add column right", this, SLOT(addColumnRight()));
    menu->addSeparator();
    menu->addAction("Delete row", this, SLOT(deleteRow()));
    menu->addAction("Delete column", this, SLOT(deleteColumn()));
    menu->addAction("Delete table", this, SLOT(deleteTable()));

    // Menu "link"
    menu = new QMenu(ui->pushLink);
    menu->setStyleSheet("QMenu::item{padding:5px 10px 5px 10px}QMenu::item:selected{background-color:" + highlight + ";color:" + highlightedText + "}");
    ui->pushLink->setMenu(menu);
    menu->addAction("Insert link", this, SLOT(link()));
    menu->addAction("Unlink", this, SLOT(unlink()));
}

HtmlEditor::~HtmlEditor()
{
    delete ui;
}

void HtmlEditor::initialize(SoundfontDescriptionData * data)
{
    // Preparation of the text
    ui->plainTextEdit->setPlainText(data->getText());

    // Formatted text mode
    ui->pushHtml->blockSignals(true);
    ui->pushHtml->setChecked(false);
    ui->pushHtml->blockSignals(false);

    // Display formatted text
    on_pushHtml_clicked(false);
}

void HtmlEditor::on_pushHtml_clicked(bool checked)
{
    if (checked)
    {
        // Get the html
        QString text = ui->textEdit->toHtml();

        // Load it in the plain text editor
        ui->plainTextEdit->setPlainText(text);

        // Switch the view
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        // Get the plain text
        QString text = ui->plainTextEdit->toPlainText();

        // Load it in the html editor
        ui->textEdit->setHtml(text, false);

        // Switch the view
        ui->stackedWidget->setCurrentIndex(0);
    }

    // Enable / disable buttons
    foreach (QWidget * widget, ui->frame->findChildren<QWidget*>())
        if (widget != ui->pushHtml)
            widget->setEnabled(!checked);
}

void HtmlEditor::on_textEdit_cursorPositionChanged()
{
    // Current text format
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextCharFormat currentFormat = cursor.charFormat();

    ui->frame->blockSignals(true);
    ui->pushBold->setChecked(currentFormat.fontWeight() == QFont::Bold);
    ui->pushItalic->setChecked(currentFormat.fontItalic());
    ui->pushUnderline->setChecked(currentFormat.fontUnderline());
    ui->pushStrikeThrough->setChecked(currentFormat.fontStrikeOut());

    // Current list format
    if (cursor.currentList())
    {
        switch (cursor.currentList()->format().style())
        {
        case QTextListFormat::ListDisc:
        case QTextListFormat::ListCircle:
        case QTextListFormat::ListSquare:
            ui->pushOl->setChecked(false);
            ui->pushUl->setChecked(true);
            break;
        case QTextListFormat::ListDecimal:
        case QTextListFormat::ListLowerAlpha:
        case QTextListFormat::ListUpperAlpha:
        case QTextListFormat::ListLowerRoman:
        case QTextListFormat::ListUpperRoman:
            ui->pushOl->setChecked(true);
            ui->pushUl->setChecked(false);
            break;
        case QTextListFormat::ListStyleUndefined:
            ui->pushOl->setChecked(false);
            ui->pushUl->setChecked(false);
            break;
        }
    }
    else
    {
        ui->pushOl->setChecked(false);
        ui->pushUl->setChecked(false);
    }
    ui->frame->blockSignals(false);
}

void HtmlEditor::setStyleNormal()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_NORMAL);
}

void HtmlEditor::setStyleQuote()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_QUOTE);
}

void HtmlEditor::setStyleCode()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_CODE);
}

void HtmlEditor::setStyleHeader1()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_H1);
}

void HtmlEditor::setStyleHeader2()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_H2);
}

void HtmlEditor::setStyleHeader3()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_H3);
}

void HtmlEditor::setStyleHeader4()
{
    ui->textEdit->setCurrentBlockType(CustomTextEdit::BLOCK_TYPE_H4);
}

void HtmlEditor::on_pushBold_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    ui->textEdit->updateTextFormat(format);
}

void HtmlEditor::on_pushItalic_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontItalic(checked);
    ui->textEdit->updateTextFormat(format);
}

void HtmlEditor::on_pushUnderline_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontUnderline(checked);
    ui->textEdit->updateTextFormat(format);
}

void HtmlEditor::on_pushStrikeThrough_clicked(bool checked)
{
    QTextCharFormat format;
    format.setFontStrikeOut(checked);
    ui->textEdit->updateTextFormat(format);
}

void HtmlEditor::on_pushUl_clicked(bool checked)
{
    ui->textEdit->setList(checked ? QTextListFormat::ListDisc : QTextListFormat::ListStyleUndefined);
    on_textEdit_cursorPositionChanged();
}

void HtmlEditor::on_pushOl_clicked(bool checked)
{
    ui->textEdit->setList(checked ? QTextListFormat::ListDecimal : QTextListFormat::ListStyleUndefined);
    on_textEdit_cursorPositionChanged();
}

void HtmlEditor::setAlignLeft()
{
    ui->textEdit->setAlignment(Qt::AlignLeft);
}

void HtmlEditor::setAlignCenter()
{
    ui->textEdit->setAlignment(Qt::AlignCenter);
}

void HtmlEditor::setAlignRight()
{
    ui->textEdit->setAlignment(Qt::AlignRight);
}

void HtmlEditor::setAlignJustify()
{
    ui->textEdit->setAlignment(Qt::AlignJustify);
}

void HtmlEditor::on_pushImage_clicked()
{
    EditorDialogInsertImage * dialog = new EditorDialogInsertImage(this);
    connect(dialog, SIGNAL(accepted(QString)), this, SLOT(insertImage(QString)));
    dialog->show();
}

void HtmlEditor::insertImage(QString url)
{
    QString img = "<img src=\"" + url + "\" />";
    ui->textEdit->insertHtml(img);
}

void HtmlEditor::on_pushVideo_clicked()
{
    EditorDialogInsertVideo * dialog = new EditorDialogInsertVideo(this);
    connect(dialog, SIGNAL(accepted(QString)), this, SLOT(insertYouTubeVideo(QString)));
    dialog->show();
}

void HtmlEditor::insertYouTubeVideo(QString videoId)
{
    QString frame = "<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/" +
            videoId + "\" frameborder=\"0\" allow=\"accelerometer; autoplay; encrypted-media; " +
            "gyroscope; picture-in-picture\" allowfullscreen></iframe>";
    ui->textEdit->insertHtml(frame);
}

void HtmlEditor::insertTable()
{
    EditorDialogInsertTable * dialog = new EditorDialogInsertTable(this);
    connect(dialog, SIGNAL(accepted(int,int)), this, SLOT(insertTable(int,int)));
    dialog->show();
}

void HtmlEditor::insertTable(int rows, int columns)
{
    QTextTableFormat format;
    format.setCellSpacing(0);
    format.setCellPadding(0);
    format.setHeaderRowCount(0);
    ui->textEdit->textCursor().insertTable(rows, columns, format);
}

void HtmlEditor::addRowAbove()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->insertRows(currentTable->cellAt(cursor).row(), 1);
}

void HtmlEditor::addRowBelow()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->insertRows(currentTable->cellAt(cursor).row() + 1, 1);
}

void HtmlEditor::addColumnLeft()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->insertColumns(currentTable->cellAt(cursor).column(), 1);
}

void HtmlEditor::addColumnRight()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->insertColumns(currentTable->cellAt(cursor).column() + 1, 1);
}

void HtmlEditor::deleteRow()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->removeRows(currentTable->cellAt(cursor).row(), 1);
}

void HtmlEditor::deleteColumn()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->removeColumns(currentTable->cellAt(cursor).column(), 1);
}

void HtmlEditor::deleteTable()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTable * currentTable = cursor.currentTable();
    if (currentTable != nullptr)
        currentTable->removeRows(0, currentTable->rows());
}

void HtmlEditor::link()
{
    QTextCursor cursor = ui->textEdit->textCursor();
    EditorDialogInsertLink * dialog = new EditorDialogInsertLink(ui->textEdit->getCurrentLinkText(), cursor.charFormat().anchorHref(), this);
    connect(dialog, SIGNAL(accepted(QString,QString)), this, SLOT(link(QString,QString)));
    dialog->show();
}

void HtmlEditor::link(QString text, QString link)
{
    if (text.isEmpty() && link.isEmpty())
        return;

    // If the text is empty, show the link
    if (text.isEmpty())
        text = link;

    if (link.isEmpty())
    {
        // Just insert the text
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.insertText(text);
    }
    else
    {
        // Insert a link
        ui->textEdit->setLink(text, link);
    }
}

void HtmlEditor::unlink()
{
    // Remove a link
    ui->textEdit->setLink("", "");
}

void HtmlEditor::on_pushHr_clicked()
{
    ui->textEdit->insertHtml("<hr />");
}
