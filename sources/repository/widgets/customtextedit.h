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

#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextBrowser>
class UrlReaderImage;

class CustomTextEdit : public QTextBrowser
{
    Q_OBJECT

public:
    enum BlockType
    {
        BLOCK_TYPE_NORMAL = 1000,
        BLOCK_TYPE_QUOTE,
        BLOCK_TYPE_CODE,
        BLOCK_TYPE_H1,
        BLOCK_TYPE_H2,
        BLOCK_TYPE_H3,
        BLOCK_TYPE_H4
    };

    CustomTextEdit(QWidget * parent = nullptr);
    ~CustomTextEdit();

    /// LOAD / SAVE HTML ///

    // Set html text
    void setHtml(QString html, bool readOnly = false);

    // Get the resulting html code
    QString toHtml();

    /// EDITING FUNCTIONS ///

    // Set the current text format
    void updateTextFormat(QTextCharFormat &format);

    // Insert html
    void insertHtml(QString html);

    // Set the type of the current block
    void setCurrentBlockType(BlockType type);

    // Change the alignment of the current block
    void setAlignment(Qt::Alignment alignment);

    // Create or remove a list (with style ListStyleUndefined)
    void setList(QTextListFormat::Style style);

    // Set a link
    void setLink(QString text, QString link);

    /// HELPERS ///

    // Return the text associated to a link under the cursor
    QString getCurrentLinkText();

protected:
    void resizeEvent(QResizeEvent * event);

private slots:
    void imageDownloaded(QString error);
    void documentSizeChanged(QSizeF size);

private:
    QString processHtmlToInsert(QString html);
    QString extract(QString tagName, QString content1, QString content2);
    QString moveSpecialFontSizes(QString html, int &pos, int pTagLength);
    QString processTag(QString tagContent);
    QString processBlock(QString tagName, QString attributes);
    QString processSpan(QString attributes);
    QString processPre(QString html, QString pre, int pos);
    QMap<QString, QString> getStyleAttributes(QString styleContent);
    QString replaceTagEnd(QString html, QString tagnameToFind, QString replacement, int startPos);

    void selectLinkText(QTextCursor * cursor);
    void downloadOneImage();
    void computeImageSize();

    bool _readOnly;
    QString _css;
    QStringList _history;
    QMap<QString, QString> _colorReplacement;
    QMap<int, QString> _extractions;
    QMap<int, QString> _imageUrlToDownload;
    UrlReaderImage * _imageReader;
    QMap<QString, QPixmap> _images;
};

#endif // CUSTOMTEXTEDIT_H
