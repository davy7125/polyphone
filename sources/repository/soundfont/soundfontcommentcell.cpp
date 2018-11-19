#include "soundfontcommentcell.h"
#include "ui_soundfontcommentcell.h"
#include "soundfontcommentdata.h"
#include "contextmanager.h"

SoundfontCommentCell::SoundfontCommentCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontCommentCell)
{
    ui->setupUi(this);
    QColor titleColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                0.5);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = titleColor.name();
    ui->labelUserIcon->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), replacement));
    ui->labelDateIcon->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), replacement));
    ui->labelDate->setStyleSheet("QLabel{color:" + titleColor.name() + ";}");
    ui->labelUser->setStyleSheet("QLabel{color:" + titleColor.name() + ";}");

    ui->textComment->setContentsMargins(0, 0, 0, 0);
    ui->textComment->document()->setDocumentMargin(0);
}

SoundfontCommentCell::~SoundfontCommentCell()
{
    delete ui;
}

void SoundfontCommentCell::initialize(SoundfontCommentData * data, int level)
{
    // Info
    ui->labelUser->setText(data->getUserName());
    ui->labelDate->setText(data->getCreationTime().toString(Qt::SystemLocaleShortDate));
    ui->textComment->setText(data->getText());

    // Level
    int maxLevel = 5;
    for (int i = 0; i < qMin(level, maxLevel); i++)
    {
        QWidget * verticalBar = new QWidget(this);
        verticalBar->setFixedWidth(22);
        verticalBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        QColor color = ThemeManager::mix(
                    ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
                    ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                    (double)i / maxLevel * level);
        verticalBar->setStyleSheet("QWidget{margin-left: 6px; margin-right: 14px;background-color: " + color.name() + ";}");
        ui->leftBars->addWidget(verticalBar);
    }
}

void SoundfontCommentCell::resizeEvent(QResizeEvent *event)
{
    // Compute the height of the text browser
    int height = ui->textComment->document()->size().height();
    ui->textComment->setMinimumHeight(height);
    ui->textComment->setMaximumHeight(height);

    if (event != nullptr)
        QWidget::resizeEvent(event);
}
