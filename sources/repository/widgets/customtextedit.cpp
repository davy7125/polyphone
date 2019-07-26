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

#include "customtextedit.h"
#include "contextmanager.h"
#include "urlreaderimage.h"
#include <QTextList>
#include <QDebug>

CustomTextEdit::CustomTextEdit(QWidget *parent) : QTextBrowser(parent),
    _imageReader(new UrlReaderImage(""))
{
    // Color for the default images
    _colorReplacement["currentColor"] = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                0.7).name();

    this->setContentsMargins(0, 0, 0, 10);
    this->setAcceptRichText(false);
    this->setMinimumHeight(20);

    // Prepare a css for displaying html text
    _css = "body { color: %1; padding: 0px; margin: 0px; }"
           "p {text-align: justify; }"
           "h1, h2, h3, h4, h5, h6 { color: %2; font-weight: bold; } "
           "a {color: %2; text-decoration: none; }"
           "table, td, th { border-width: 1px; border-color: %3; }"
           "td, th { text-align: left; padding: 2px 5px; }"
           "blockquote { font-style: italic; background-color: %4; margin: 20px; font-size:small; }";
    _css = _css
            .arg(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name())
            .arg(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name())
            .arg(_colorReplacement["currentColor"])
            .arg(ThemeManager::mix( // Light color for quotes
                                    ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                    ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                    0.2).name());

    // Connections
    connect(reinterpret_cast<const QObject*>(this->document()->documentLayout()), SIGNAL(documentSizeChanged(QSizeF)),
            this, SLOT(documentSizeChanged(QSizeF)));
    connect(_imageReader, SIGNAL(downloadCompleted(QString)), this, SLOT(imageDownloaded(QString)), Qt::QueuedConnection);
}

CustomTextEdit::~CustomTextEdit()
{
    delete _imageReader;
}

void CustomTextEdit::setHtml(QString html, bool readOnly)
{
    // Reinit variables
    _readOnly = readOnly;
    _extractions.clear();
    if (readOnly)
    {
        this->setOpenExternalLinks(true);
        this->setTextInteractionFlags(Qt::TextBrowserInteraction);
        this->setReadOnly(true);
        this->setUndoRedoEnabled(false);
        this->viewport()->setCursor(Qt::ArrowCursor);
    }
    else
    {
        this->setOpenExternalLinks(false);
        this->setTextInteractionFlags(Qt::TextEditorInteraction);
        this->setReadOnly(false);
        this->setUndoRedoEnabled(true);
        this->viewport()->setCursor(Qt::IBeamCursor);
    }

    // Wrap the html code in a body associated with a css
    QString content = "<html><head>"
                      "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'/>"
                      "<style>%1</style>"
                      "</head>"
                      "<body>%2</body></html>";
    QTextEdit::setHtml(content.arg(_css).arg(processHtmlToInsert(html)));

    // Begin downloading the resources
    this->downloadOneImage();
}

QString CustomTextEdit::toHtml()
{
    QString html = QTextEdit::toHtml();

    // Extract the body and trim the result
    QRegExp rx_body("<body[^>]*>(.*)</body>");
    if (rx_body.indexIn(html, 0) != -1)
        html = rx_body.cap(1).trimmed();

    // 1. Clean useless things
    html = html.replace(QRegExp(" *margin-top:[^;]+; *"), "")
            .replace(QRegExp(" *margin-bottom:[^;]+; *"), "")
            .replace(QRegExp(" *margin-right:[^;]+; *"), "")
            .replace(QRegExp(" *margin-left: *\\d{1,3}px; *"), "") // Keep magic numbers > 999
            .replace(QRegExp(" *padding-[^:]*:[^;]+; *"), "")
            .replace(QRegExp(" *border-color:[^;]+; *"), "")
            .replace(QRegExp(" *background-color:[^;]+; *"), "")
            .replace(QRegExp(" *color:[^;]+; *"), "")
            .replace(QRegExp(" *font-family:[^;]+; *"), "")
            .replace(QRegExp(" *-qt-list-indent:[^;]+; *"), "")
            .replace(QRegExp(" *-qt-block-indent:[^;]+; *"), "")
            .replace(QRegExp(" *text-indent:[^;]+; *"), "")
            .replace(QRegExp(" *cellspacing=\"[^\"]*\""), "")
            .replace(QRegExp(" *cellpadding=\"[^\"]*\""), "")
            .replace(QRegExp(" *border=\"[^\"]*\""), "")
            .replace(QRegExp(" *style=\"\""), "");

    //qDebug() << "BEFORE >>" << html;

    // 2. Recursively move font sizes "xx-large", "x-large", ... from <span> to their parent <p>
    int pos = -1;
    html = moveSpecialFontSizes(html, pos, 0);

    // 3. Browse all tags sequentially to continue the process
    _history.clear();
    _history << "body";
    QRegExp rx_tag("<([^<>]*)>");
    pos = 0;
    while ((pos = rx_tag.indexIn(html, pos)) != -1)
    {
        QString tagContent = rx_tag.cap(1);
        QString tagName = tagContent.split(" ").first();

        if (tagName.startsWith("/"))
        {
            // Closing tag
            if (_history.last() != tagName.right(tagName.length() - 1))
            {
                // Problem
                qWarning() << "cannot close" << _history.last() << "with" << tagName;
                return html;
            }

            // Remove an element in the history
            _history.removeLast();
        }
        else
        {
            // Process a tag
            bool autoClosing = tagContent.endsWith("/");
            QString updatedTagContent = processTag(tagContent.trimmed());

            if (updatedTagContent != tagContent)
            {
                if (updatedTagContent.isEmpty())
                {
                    // Remove the opening tag
                    html = html.replace(pos, tagContent.length() + 2, "");

                    // Remove the corresponding closing tag
                    if (!autoClosing)
                        html = replaceTagEnd(html, tagName, "", pos + 1);
                }
                else
                {
                    // Replace the opening tag
                    html = html.replace(pos + 1, tagContent.length(), updatedTagContent);

                    if (!autoClosing)
                    {
                        // Possibly replace the closing tag
                        QString updatedTagName = updatedTagContent.split(" ").first();
                        if (updatedTagName != tagName)
                            html = replaceTagEnd(html, tagName, updatedTagName, pos + 1);

                        // Update the history
                        _history << updatedTagName;
                    }
                }
            }
            else
            {
                // Update the history
                if (!autoClosing)
                    _history << tagName;
            }
        }

        pos++;
    }

    // 4. Restore frames and images
    QRegExp rx_img("<img ([^<>]*)/>");
    pos = 0;
    while ((pos = rx_img.indexIn(html, pos)) != -1)
    {
        QString imgContent = rx_img.cap(1);

        // Get the source
        QRegExp rx_src("src=\"(\\d+)\"");
        if (rx_src.indexIn(imgContent) != -1)
        {
            // Element id
            bool ok;
            int id = rx_src.cap(1).toInt(&ok);
            if (!ok)
            {
                qWarning() << "Couldn't read element id in" << imgContent;
                continue;
            }

            // Replace the content of the img
            if (_extractions.contains(id))
                html = html.replace(pos, rx_img.matchedLength(), _extractions.take(id));
            else
                qWarning() << "Couldn't find id" << id << "in the extractions";
        }
        else
            qWarning() << "couldn't find an id in" << imgContent;

        pos++;
    }

    // 5. Final cleaning
    html = html.replace(QRegExp(" *>"), ">");

    //qDebug() << "AFTER >>" << html;

    return html;
}

void CustomTextEdit::updateTextFormat(QTextCharFormat &format)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
}

void CustomTextEdit::insertHtml(QString html)
{
    html = processHtmlToInsert(html);
    this->textCursor().insertHtml(html);
    downloadOneImage();
}

void CustomTextEdit::setCurrentBlockType(BlockType type)
{
    QTextBlockFormat format;
    format.setLeftMargin(static_cast<int>(type)); // Trick => will be resolved after the reload

    // Selection is the current selection (comprising one or more blocks) or the current block
    this->textCursor().mergeBlockFormat(format);

    // After a block change, html is read, processed, then reinjected in the text editor
    // Typical use: h1, h2, h3 cannot be inserted with a QTextBlockFormat so the trick is to change the left margin to the magical number "BLOCK_TYPE_H1"
    // Then we merge the text block change so that the left margin is correctly applied on the html inside the text editor
    // Then the html is read and processed, blocks H1 appears
    // Finally it is reinjected in the text editor
    this->setHtml(this->toHtml(), false);
}

void CustomTextEdit::setAlignment(Qt::Alignment alignment)
{
    QTextBlockFormat format;
    format.setAlignment(alignment);

    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::BlockUnderCursor);
    cursor.mergeBlockFormat(format);
}

void CustomTextEdit::setList(QTextListFormat::Style style)
{
    // Possibly remove the list
    QTextCursor cursor = this->textCursor();
    if (style == QTextListFormat::ListStyleUndefined)
    {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(0);
        cursor.mergeBlockFormat(bfmt);
        setTextCursor(cursor);
        return;
    }

    // Or create a list
    QTextListFormat listFormat;
    listFormat.setStyle(style);
    if (style == QTextListFormat::ListDecimal)
        listFormat.setNumberSuffix(".");
    cursor.createList(listFormat);
}

QString CustomTextEdit::processHtmlToInsert(QString html)
{
    // Remove unnecessary data
    html = html.replace("background-color: initial;", "");

    // Remove the space between cells in tables
    html = html.replace("<table>", "<table cellspacing=0>");

    // Extract frames and images
    QRegExp rx_img1("<img([^<>]*)/>"); // Autoclosing FIRST (because we are creating similar elements)
    int pos = 0;
    while ((pos = rx_img1.indexIn(html, pos)) != -1)
    {
        html = html.replace(pos, rx_img1.matchedLength(), extract("img", rx_img1.cap(1), ""));
        pos ++;
    }

    QRegExp rx_img2("<img([^<>]*)>([^<>]*)</img>"); // Not autoclosing SECOND
    pos = 0;
    while ((pos = rx_img2.indexIn(html, pos)) != -1)
    {
        html = html.replace(pos, rx_img2.matchedLength(), extract("img", rx_img2.cap(1), rx_img2.cap(2)));
        pos ++;
    }

    QRegExp rx_frame("<iframe([^<>]*)>([^<>]*)</iframe>"); // Frame LAST
    pos = 0;
    while ((pos = rx_frame.indexIn(html, pos)) != -1)
    {
        html = html.replace(pos, rx_frame.matchedLength(), extract("iframe", rx_frame.cap(1), rx_frame.cap(2)));
        pos ++;
    }

    return html;
}

QString CustomTextEdit::extract(QString tagName, QString content1, QString content2)
{
    // Store the frame
    int id = _extractions.count();
    _extractions[id] = "<" + tagName + content1 + ">" + content2 + "</" + tagName + ">";

    QPixmap defaultImage;
    QString imageUrl = "";
    QString link;
    if (tagName == "img")
    {
        // Default image
        defaultImage = ContextManager::theme()->getColoredSvg(":/html_editor/image.svg", QSize(150, 150), _colorReplacement);

        // Get the real source
        QRegExp rx_src("src=\"([^\"]*)\"");
        if (rx_src.indexIn(content1) != -1)
            imageUrl = rx_src.cap(1);
    }
    else
    {
        // Youtube frame?
        QRegExp rx_youtube("youtube\\.com/embed/([0-9a-zA-Z-_]*)\"");
        if (rx_youtube.indexIn(content1) != -1)
        {
            defaultImage = ContextManager::theme()->getColoredSvg(":/html_editor/video.svg", QSize(150, 150), _colorReplacement);
            imageUrl = "https://img.youtube.com/vi/" + rx_youtube.cap(1) + "/hqdefault.jpg"; // Thumbnail corresponding to the video
            link = "https://www.youtube.com/watch?v=" + rx_youtube.cap(1);
        }
        else
        {
            // Something else in the frame
            defaultImage = ContextManager::theme()->getColoredSvg(":/html_editor/piece.svg", QSize(150, 150), _colorReplacement);
        }
    }

    // Create an image instead
    QString replacement = "<img src='" + QString::number(id) + "' />";
    if (_readOnly && !link.isEmpty())
    {
        // Wrap the image in <a>, it will be clickable
        replacement = "<a href=\"" + link + "\">" + replacement + "</a>";
    }

    // Load the default image
    this->document()->addResource(QTextDocument::ImageResource, QString::number(id), defaultImage);

    // Possibly add a query to get the real image
    if (!imageUrl.isEmpty())
        _imageUrlToDownload[id] = imageUrl;

    return replacement;
}

QString CustomTextEdit::moveSpecialFontSizes(QString html, int &pos, int pTagLength)
{
    // Keep a track of the initial position
    int initialPosition = pos;
    pos++;

    // Possible replacement of the current p, depending on the analyze
    QString replacement = "";

    // Find the next <p>, </p> or <span>
    QRegExp rx("<(p|span|/p)( [^<>]*|)>");
    while ((pos = rx.indexIn(html, pos)) != -1)
    {
        if (rx.cap(1) == "p")
        {
            // Process another <p>
            html = moveSpecialFontSizes(html, pos, rx.matchedLength());
        }
        else if (rx.cap(1) == "/p")
        {
            // End of the <p>, tag name can change
            if (replacement != "")
            {
                // Update the closing tag
                html = html.replace(pos, rx.matchedLength(), "</" + replacement + ">");

                // Update the opening tag
                replacement = html.mid(initialPosition, pTagLength).replace(QRegExp("<p"), "<" + replacement);
                html = html.replace(initialPosition, pTagLength, replacement);

                // Update the position
                pos += replacement.length() + 2 - pTagLength;
            }
            return html;
        }
        else if (rx.cap(1) == "span")
        {
            // Analyze the content of the span
            QMap<QString, QString> styleAttributes = getStyleAttributes(rx.cap(2));
            if (styleAttributes.contains("font-size"))
            {
                QString value = styleAttributes["font-size"];
                if (value == "xx-large")
                    value = "h1";
                else if (value == "x-large")
                    value = "h2";
                else if (value == "large")
                    value = "h3";
                else if (value == "medium")
                    value = "h4";
                else if (value == "small")
                    value = "blockquote";
                else
                    value = "";

                if (!value.isEmpty())
                {
                    if (replacement != "" && replacement != value)
                    {
                        qWarning() << "two possible replacements:" << replacement << value;
                    }
                    else
                    {
                        replacement = value;

                        // Remove the attributes "font-size" and "font-weight"
                        QString newTag = "<span " + rx.cap(2).replace(QRegExp(" *font-size:[^;\"']+; *"), "") + ">";
                        if (replacement == "blockquote")
                        {
                            if (!newTag.contains("font-style"))
                                newTag = newTag.replace("style=\"", "style=\"font-style:normal;");
                            else
                                newTag = newTag.replace(QRegExp(" *font-style:italic; *"), "");
                        }
                        else if (replacement.startsWith("h"))
                        {
                            if (!newTag.contains("font-weight"))
                                newTag = newTag.replace("style=\"", "style=\"font-weight:400;");
                            else
                                newTag = newTag.replace(QRegExp(" *font-weight:600; *"), "");
                        }
                        html = html.replace(pos, rx.matchedLength(), newTag.replace(QRegExp(" *style=\"\""), ""));
                    }
                }
            }
        }

        pos++;
    }

    // No </p> have been encountered, weird if we are not at the beginning
    if (initialPosition != -1)
        qWarning() << "moveSpecialFontSizes found no corresponding </p>";

    return html;
}

QString CustomTextEdit::processTag(QString tagContent)
{
    int separatorPos = tagContent.indexOf(" ");
    QString tagName = (separatorPos > 0 ? tagContent.left(separatorPos) : tagContent);
    QString attributes = (separatorPos > 0 ? tagContent.right(tagContent.length() - separatorPos - 1) : "");

    // Tag name can change
    if (tagName == "blockquote" || tagName == "p" || tagName == "h1" || tagName == "h2" || tagName == "h3" || tagName == "h4" || tagName == "pre")
        return processBlock(tagName, attributes);

    // Span can disappear
    if (tagName == "span")
        return processSpan(attributes);

    // Remove everything in <pre>
    if (tagName == "pre")
        return "pre";

    // Add target="_blank" for all <a> that are not redirecting to polyphone-soundfonts.com
    if (tagName == "a" && !tagContent.contains("polyphone-soundfonts.com"))
        return tagContent + " target=\"_blank\"";

    // Nothing to do
    return tagContent;
}

QString CustomTextEdit::processSpan(QString attributes)
{
    // Possibly remove the tag if there are no attributes
    if (attributes.isEmpty())
        return "";

    // Otherwise keep everything in the tag
    return "span " + attributes;
}

QString CustomTextEdit::processBlock(QString tagName, QString attributes)
{
    // Get the style in the <p style="..."> tag
    QMap<QString, QString> styleAttributes = getStyleAttributes(attributes);

    // Possibly replace the tag name if the margin left is a magic number?
    QString replacement = "";
    if (styleAttributes.contains("margin-left"))
    {
        QString value = styleAttributes["margin-left"];
        if (value.count() > 2)
        {
            int number = value.left(value.length() - 2).toInt();
            switch (static_cast<BlockType>(number))
            {
            case BLOCK_TYPE_QUOTE:
                replacement = "blockquote";
                break;
            case BLOCK_TYPE_CODE:
                replacement = "pre";
                break;
            case BLOCK_TYPE_H1:
                replacement = "h1";
                break;
            case BLOCK_TYPE_H2:
                replacement = "h2";
                break;
            case BLOCK_TYPE_H3:
                replacement = "h3";
                break;
            case BLOCK_TYPE_H4:
                replacement = "h4";
                break;
            default: //case BLOCK_TYPE_NORMAL:
                replacement = "p";
                break;
            }
        }
    }
    if (!replacement.isEmpty())
        return replacement;

    // Otherwise keep everything in the tag
    return tagName + " " + attributes;
}

QMap<QString, QString> CustomTextEdit::getStyleAttributes(QString tag)
{
    QMap<QString, QString> result;

    // Extract the style from the tag
    QRegExp rx_style("style=[\"']([^\"']*)[\"']");
    QString styleContent;
    if (rx_style.indexIn(tag, 0) != -1)
        styleContent = rx_style.cap(1);

    QStringList parts = styleContent.split(";");
    foreach (QString part, parts)
    {
        QStringList subParts = part.split(":");
        if (subParts.length() == 2)
            result[subParts[0].trimmed()] = subParts[1].trimmed();
    }

    return result;
}

QString CustomTextEdit::replaceTagEnd(QString html, QString tagnameToFind, QString replacement, int startPos)
{
    QStringList openedTags;
    QRegExp rx_tag("<([^<>]*)>");
    int pos = startPos;
    while ((pos = rx_tag.indexIn(html, pos)) != -1)
    {
        QString tagContent = rx_tag.cap(1);
        QString tagname = tagContent.split(" ").first();

        if (tagname.startsWith("/"))
        {
            QString tagToClose = tagname.right(tagname.length() - 1);
            if (openedTags.length() > 0)
            {
                if (openedTags.last() != tagToClose)
                {
                    // Problem
                    qWarning() << "cannot close last tag" << openedTags.last() << "with" << tagToClose;
                    return html;
                }

                // Close the last opened tag
                openedTags.removeLast();
            }
            else
            {
                if (tagnameToFind != tagToClose)
                {
                    // Problem
                    qWarning() << "cannot close tag" << tagnameToFind << "with" << tagToClose;
                    return html;
                }

                if (replacement.isEmpty())
                {
                    // Remove the closing tag
                    return html.replace(pos, tagnameToFind.length() + 3, "");
                }
                else
                {
                    // Replace the closing tag by the new one
                    return html.replace(pos + 2, tagnameToFind.length(), replacement);
                }
            }
        }
        else if (!tagContent.endsWith("/")) // Not autoclosing
            openedTags << tagname;

        pos++;
    }

    // Problem
    qWarning() << "couldn't close tag" << tagnameToFind;

    return html;
}

void CustomTextEdit::setLink(QString text, QString link)
{
    // Extent the selection to the full link
    QTextCursor cursor = this->textCursor();
    selectLinkText(&cursor);

    // Possibly remove a link
    if (link.isEmpty())
    {
        // If there is a link, remove it
        if (!cursor.charFormat().anchorHref().isEmpty())
        {
            QTextCharFormat format;
            format.setAnchor(false);
            format.setForeground(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT));
            cursor.mergeCharFormat(format);
        }

        return;
    }

    // Save the original format to create an extra space with the existing char format for the block
    const QTextCharFormat originalFormat = cursor.charFormat();

    // Create a link
    QTextCharFormat format;
    format.setAnchor(true);
    format.setAnchorHref(link);
    format.setForeground(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND));

    // Insert the link
    cursor.beginEditBlock();
    cursor.insertText(text, format);

    // Insert a space after the link if at the end of the block so that
    // typing some text after the link does not carry link formatting
    if (cursor.atBlockEnd())
    {
        cursor.setPosition(cursor.selectionEnd());
        cursor.setCharFormat(originalFormat);
        cursor.insertText(QString(" "));
    }
    cursor.endEditBlock();
}

QString CustomTextEdit::getCurrentLinkText()
{
    QTextCursor cursor = this->textCursor();
    selectLinkText(&cursor);
    return cursor.selectedText();
}

void CustomTextEdit::selectLinkText(QTextCursor *cursor)
{
    // If the cursor is on a link, select the text of the link
    if (cursor->charFormat().isAnchor())
    {
        QString href = cursor->charFormat().anchorHref();

        // Move anchor to start of link
        while (cursor->charFormat().anchorHref() == href)
        {
            if (cursor->atStart())
                break;
            cursor->setPosition(cursor->position() - 1, QTextCursor::MoveAnchor);
        }
        if (cursor->charFormat().anchorHref() != href)
            cursor->setPosition(cursor->position() + 1, QTextCursor::KeepAnchor);

        // Move selection to the end of the link
        while (cursor->charFormat().anchorHref() == href)
        {
            if (cursor->atEnd())
                break;
            cursor->setPosition(cursor->position() + 1, QTextCursor::KeepAnchor);
        }
        if (cursor->charFormat().anchorHref() != href)
            cursor->setPosition(cursor->position() - 1, QTextCursor::KeepAnchor);
    }
    else if (cursor->hasSelection())
    {
        // Nothing to do: use the currently selected text as the link text
    }
    else
    {
        // Select current word
        cursor->movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        cursor->movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    }
}

void CustomTextEdit::downloadOneImage()
{
    if (_imageUrlToDownload.empty())
        return;

    // Id of the image to download, and url
    int key = _imageUrlToDownload.keys().first();
    QString url = _imageUrlToDownload[key];

    // Maybe already downloaded?
    if (_images.contains(url))
    {
        // Update the image
        this->document()->addResource(QTextDocument::ImageResource, QString::number(key), _images[url]);
        this->viewport()->update();

        // Remove the image from the download list
        _imageUrlToDownload.remove(key);

        // Download the next image
        this->downloadOneImage();
    }
    else
    {
        _imageReader->setUrl(url);
        _imageReader->download();
    }
}

void CustomTextEdit::imageDownloaded(QString error)
{
    // Current image key
    int key = _imageUrlToDownload.keys().first();

    if (error.isEmpty())
    {
        // Store the image
        QPixmap image = _imageReader->getPixmap();
        _images[_imageUrlToDownload[key]] = image;

        // Update the image
        this->document()->addResource(QTextDocument::ImageResource, QString::number(key), image);
        this->viewport()->resize(this->viewport()->width(), static_cast<int>(this->document()->size().height()));
    }
    else
        qWarning() << "couldn't download" << _imageReader->getUrl() << ":" << error;

    // Remove the image from the download list
    _imageUrlToDownload.remove(key);

    // Download the next image
    this->downloadOneImage();
}

void CustomTextEdit::documentSizeChanged(QSizeF size)
{
    this->setMinimumHeight(static_cast<int>(size.height() + 4));
}

void CustomTextEdit::resizeEvent(QResizeEvent * event)
{
    QTextEdit::resizeEvent(event);
    computeImageSize();
}

void CustomTextEdit::computeImageSize()
{
    // Maximum width allowed and current position of the cursor
    int maxWidth = this->width() - 30;
    QTextCursor cursor = this->textCursor();
    int currentPosition = cursor.position();
    int currentAnchor = cursor.anchor();

    // Get the full text of the editor with no conversion
    QString html = QTextEdit::toHtml();

    // Browse all images
    QRegExp rx_img("<img ([^<>]*)/>");
    int pos = 0;
    while ((pos = rx_img.indexIn(html, pos)) != -1)
    {
        // Get the source of the image
        QString imgContent = rx_img.cap(1);
        QRegExp rx_src("src=\"([^\"]+)\"");

        if (rx_src.indexIn(imgContent) != -1)
        {
            // Get the initial width of the image
            QString src = rx_src.cap(1);
            int width = this->document()->resource(QTextDocument::ImageResource, QUrl(src)).value<QPixmap>().width();

            // Ideal width
            width = qMin(width, maxWidth);


            // Replace the img tag by the new one with the correct width
            QString replacement = (width > 0) ?
                        "<img src=\"" + src + "\" width=\"" + QString::number(width) + "\" />" :
                        "<img src=\"" + src + "\" />";
            html = html.replace(pos, rx_img.matchedLength(), replacement);
        }

        pos++;
    }

    // Select everything in the document
    cursor.select(QTextCursor::Document);

    // Replace by the modified text
    cursor.joinPreviousEditBlock();
    cursor.insertHtml(html);
    cursor.endEditBlock();

    // Restore the cursor position
    cursor.setPosition(currentPosition, QTextCursor::MoveAnchor);
    cursor.setPosition(currentAnchor, QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
}
