/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#include "soundfontdescription.h"
#include "ui_soundfontdescription.h"
#include "soundfontdescriptiondata.h"
#include "urlreaderimage.h"
#include "contextmanager.h"
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

QString SoundfontDescription::s_css = "";

SoundfontDescription::SoundfontDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDescription),
    _imageReader(new UrlReaderImage(""))
{
    ui->setupUi(this);

    // Text browser configuration
    ui->textBrowser->setContentsMargins(0, 0, 0, 0);
    connect(ui->textBrowser->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            this, SLOT(documentSizeChanged(QSizeF)));

    // Image reader
    connect(_imageReader, SIGNAL(downloadCompleted(QString)), this, SLOT(imageDownloaded(QString)), Qt::QueuedConnection);
}

SoundfontDescription::~SoundfontDescription()
{
    delete _imageReader;
    delete ui;
}

void SoundfontDescription::display(SoundfontDescriptionData * data)
{
    // Set the content
    QString content = "<html><head>"
                      "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'/>"
                      "<style>%1</style>"
                      "</head>"
                      "<body>%2</body></html>";
    ui->textBrowser->setHtml(content.arg(getCss()).arg(getBody(data)));

    // Download the resources
    this->downloadOneImage();
}

QString SoundfontDescription::getCss()
{
    if (s_css == "")
    {
        s_css = "body { color: %1; padding: 0px; margin: 0px; }"
                "p {text-align: justify; }"
                "h1, h2, h3, h4, h5, h5, h6 { color: %2; font-weight: bold; } "
                "a {color: %2; text-decoration: none; }"
                "img { max-width: 100%; }"
                "table { border-color: %3; border-width: 1px; }"
                "th, td { text-align: left; padding: 2px 5px; }";
        s_css = s_css
                .arg(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name())
                .arg(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name())
                .arg(ThemeManager::mix(
                         ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                         ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                         0.7).name());
    }

    return s_css;
}

QString SoundfontDescription::getBody(SoundfontDescriptionData * data)
{
    QString str = data->getText();
    if (str == "")
        str = "<p>" + trUtf8("No description.") + "</p>";
    else
    {
        // Remove unnecessary data
        str = str.replace(QRegExp("[\r\n\t]"), "");

        // Format tables
        str = str.replace("<table>", "<table cellspacing=0>");

        // Process possible iframes
        QRegExp rx("<iframe([^<>]*)>[^<>]*</iframe>");
        int pos = 0;
        while ((pos = rx.indexIn(str, pos)) != -1)
        {
            str.replace(pos, rx.matchedLength(), processFrame(rx.cap(1)));
            pos += rx.matchedLength();
        }

        // Links

    }

    return str;
}

QString SoundfontDescription::processFrame(QString frame)
{
    QString result = "";

    // Youtube content? Replace it with a clickable image
    // <iframe width="560" height="315"
    //   src="https://www.youtube.com/embed/u_Y-Af2R3qE"
    //   frameborder="0"
    //   allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture"
    //   allowfullscreen="">
    // </iframe>
    QRegExp rx("youtube\\.com/embed/([0-9a-zA-Z-_]*)\"");
    if (rx.indexIn(frame) != -1)
    {
        QString videoId = rx.cap(1);
        QString thumbnailUrl = "https://img.youtube.com/vi/" + videoId + "/hqdefault.jpg";
        result = "<a href='https://www.youtube/watch?v=" + videoId + "'>" +
                 "<img width='320' height='180' src='" + thumbnailUrl + "'></img></a>";

        // Temporary image
        ui->textBrowser->document()->addResource(QTextDocument::ImageResource, thumbnailUrl,
                                                 QPixmap(":/misc/youtube.jpg"));

        // Add a query to get the image
        _imagesToDownload << thumbnailUrl;
    }

    return result;
}

void SoundfontDescription::documentSizeChanged(QSizeF size)
{
    ui->textBrowser->setMinimumHeight(size.height());
}

void SoundfontDescription::downloadOneImage()
{
    if (_imagesToDownload.empty())
        return;

    _imageReader->setUrl(_imagesToDownload.takeFirst());
    _imageReader->download();
}

void SoundfontDescription::imageDownloaded(QString error)
{
    if (error.isEmpty())
    {
        // Update the image
        ui->textBrowser->document()->addResource(QTextDocument::ImageResource,
                                                 _imageReader->getUrl(),
                                                 _imageReader->getPixmap());
        ui->textBrowser->viewport()->update();
    }
    else
        qWarning() << "couldn't download" << _imageReader->getUrl() << ":" << error;

    // Download the next image
    this->downloadOneImage();
}
