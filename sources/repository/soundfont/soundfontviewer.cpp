#include "soundfontviewer.h"
#include "ui_soundfontviewer.h"
#include "contextmanager.h"
#include "repositorymanager.h"
#include "detailsmanager.h"
#include "soundfontfilter.h"
#include "soundfontdetails.h"

SoundfontViewer::SoundfontViewer(QWidget *parent) : QWidget(parent),
    ui(new Ui::SoundfontViewer)
{
    ui->setupUi(this);

    // Style
    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" +
                                 ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                 ";border-radius:5px;padding:5px}");
    ui->frameTitle->setStyleSheet("QFrame{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                  ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");
    ui->stackedWidget->setStyleSheet("QStackedWidget{border:1px solid " +
                                     this->palette().dark().color().name() +
                                     ";border-top:0;border-right:0;border-bottom:0}");
    ui->iconComment->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/comment.svg", QSize(24, 24), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
    ui->iconDownload->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/download.svg", QSize(24, 24), ThemeManager::ColorType::HIGHLIGHTED_TEXT));
    ui->iconAuthor->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconDate->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconLicense->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/copyright.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconWebsite->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->scrollMainArea->setStyleSheet("QWidget#scrollMainArea{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                      ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    ui->pageFailed->setStyleSheet("QWidget#pageFailed{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                  ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    ui->pageWait->setStyleSheet("QWidget#pageWait{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                  ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
    QString styleSheetTitle = "QLabel { background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
            "; color: " + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
            ";border-radius: 2px; padding: 3px 5px; }";
    ui->labelDescription->setStyleSheet(styleSheetTitle);
    ui->labelDownload->setStyleSheet(styleSheetTitle);
    ui->labelComment->setStyleSheet(styleSheetTitle);

    QColor buttonBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor buttonText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor buttonBackgroundHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    QString tmp = QString("QPushButton{background-color:%1; color:%2;border:0px;padding:5px;border-radius:4px;}") +
            "QPushButton:hover{ background-color:%3;}QLabel#labelAuthor{color:%1;}";
    ui->scrollArea->setStyleSheet(tmp.arg(buttonBackground.name()).arg(buttonText.name()).arg(buttonBackgroundHover.name()));
}

SoundfontViewer::~SoundfontViewer()
{
    delete ui;
}

void SoundfontViewer::initialize(int soundfontId)
{
    ui->spinner->startAnimation();
    ui->stackedWidget->setCurrentIndex(0);
    _soundfontId = soundfontId;

    SoundfontInformation * soundfontInfo = RepositoryManager::getInstance()->getSoundfontInformation(soundfontId);
    if (soundfontInfo == nullptr)
        return; // Should not be possible

    // Title
    ui->labelTitle->setTextToElide(soundfontInfo->getTitle());

    // Numbers
    ui->labelCommentNumber->setText(QString::number(soundfontInfo->getCommentNumber()));
    ui->labelDownloadNumber->setText(QString::number(soundfontInfo->getDownloadNumber()));
    ui->widgetStars->setScore(soundfontInfo->getRating());

    // Author
    QString author = soundfontInfo->getAuthor();
    ui->labelAuthor->setText("<a style=\"text-decoration:none;color:" +
                             ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() +
                             "\" href=\"" + author + "\">" + author + "</a>");

    // Date
    ui->labelDate->setText(soundfontInfo->getDateTime().toString(Qt::SystemLocaleShortDate));

    // License
    ui->labelLicense->setText("<a style=\"text-decoration:none;color:" +
                              ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() +
                              "\" href=\"" +
                              RepositoryManager::getInstance()->getLicenseLink(soundfontInfo->getLicense()) +
                              "\">" + RepositoryManager::getInstance()->getLicenseLabel(soundfontInfo->getLicense()) + "</a>");

    // Website
    if (soundfontInfo->getWebsite() == "")
    {
        ui->labelWebsite->hide();
        ui->iconWebsite->hide();
    }
    else
    {
        ui->labelWebsite->show();
        ui->iconWebsite->show();
        ui->labelWebsite->setText("<a href='" + soundfontInfo->getWebsite() + "' style='text-decoration:none;color:" +
                                  ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name()+
                                  "'>" + soundfontInfo->getWebsite() + "</a>");
    }

    // Category
    connect(ui->attributeCategory, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
    ui->attributeCategory->addCategory(soundfontInfo->getCategoryId());

    // Properties
    SoundfontInformation::Property property = SoundfontInformation::SAMPLE_SOURCE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeSampleSource, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeSampleSource->addProperty(property, value);
    }
    else
    {
        ui->attributeSampleSource->hide();
        ui->labelSampleSource->hide();
    }

    property = SoundfontInformation::TIMBRE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeTimbre, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeTimbre->addProperty(property, value);
    }
    else
    {
        ui->attributeTimbre->hide();
        ui->labelTimbre->hide();
    }

    property = SoundfontInformation::ARTICULATION;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeArticulation, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeArticulation->addProperty(property, value);
    }
    else
    {
        ui->attributeArticulation->hide();
        ui->labelArticulation->hide();
    }

    property = SoundfontInformation::GENRE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeGenre, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeGenre->addProperty(property, value);
    }
    else
    {
        ui->attributeGenre->hide();
        ui->labelGenre->hide();
    }

    property = SoundfontInformation::MIDI_STANDARD;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeMidiStandard, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeMidiStandard->addProperty(property, value);
    }
    else
    {
        ui->attributeMidiStandard->hide();
        ui->labelMidiStandard->hide();
    }

    // Tags
    if (soundfontInfo->getTags().isEmpty())
    {
        ui->labelTags->hide();
        ui->attributeTag->hide();
    }
    else
    {
        connect(ui->attributeTag, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString tag, soundfontInfo->getTags())
            ui->attributeTag->addTag(tag);
    }

    // Ask for the details
    DetailsManager * dm = DetailsManager::getInstance();
    connect(dm, SIGNAL(detailsReady(int)), this, SLOT(onDetailsReady(int)));
    connect(dm, SIGNAL(detailsFailed(int,QString)), this, SLOT(onDetailsFailed(int,QString)));
    dm->askForSoundfontDetails(_soundfontId);
}

void SoundfontViewer::on_labelAuthor_linkActivated(const QString &link)
{
    SoundfontFilter * filter = new SoundfontFilter();
    filter->setSearchText(QString("Author:\"%1\"").arg(link));
    emit(itemClicked(filter));
}

void SoundfontViewer::onDetailsReady(int soundfontId)
{
    if (soundfontId != _soundfontId)
        return;

    // Get the details
    SoundfontDetails * sd = DetailsManager::getInstance()->getDetails(_soundfontId);
    if (sd != nullptr)
    {
        ui->widgetDescription->display(sd->getDescription());
        ui->widgetDownload->display(sd->getDownloads());
        ui->widgetComment->display(sd->getComments());
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
        ui->stackedWidget->setCurrentIndex(2);
}

void SoundfontViewer::onDetailsFailed(int soundfontId, QString error)
{
    Q_UNUSED(error)

    if (soundfontId != _soundfontId)
        return;

    ui->stackedWidget->setCurrentIndex(2);
}

void SoundfontViewer::on_pushRetry_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    DetailsManager::getInstance()->askForSoundfontDetails(_soundfontId);
}
