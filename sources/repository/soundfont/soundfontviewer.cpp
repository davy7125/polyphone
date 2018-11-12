#include "soundfontviewer.h"
#include "ui_soundfontviewer.h"
#include "contextmanager.h"
#include "repositorymanager.h"

SoundfontViewer::SoundfontViewer(QWidget *parent) :
    QWidget(parent),
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
    ui->scrollMainArea->setStyleSheet("QWidget#scrollMainArea{ background: " + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
                                      ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + "; }");
}

SoundfontViewer::~SoundfontViewer()
{
    delete ui;
}

void SoundfontViewer::initialize(int soundfontId)
{
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

    // Ask for the details
}
