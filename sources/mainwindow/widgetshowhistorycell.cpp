#include "widgetshowhistorycell.h"
#include "ui_widgetshowhistorycell.h"
#include <QDateTime>
#include <QFile>
#include "contextmanager.h"

WidgetShowHistoryCell::Icons * WidgetShowHistoryCell::s_icons = NULL;

WidgetShowHistoryCell::WidgetShowHistoryCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetShowHistoryCell)
{
    ui->setupUi(this);

    // File icons
    if (s_icons == NULL)
        s_icons = new Icons();

    ui->iconFile->setPixmap(s_icons->_fileIcon);

    // The style changes when the cell is activated
    _activeStyleSheet = "QLabel{color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";
}

WidgetShowHistoryCell::Icons::Icons()
{
    _fileIcon = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::LIST_TEXT);
    _fileIconActive = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::HIGHLIGHTED_TEXT);
}

WidgetShowHistoryCell::~WidgetShowHistoryCell()
{
    delete ui;
}

void WidgetShowHistoryCell::setLink(QString filePath)
{
    _link = filePath;
    ui->labelLink->setText(filePath);
}

QString WidgetShowHistoryCell::getLink()
{
    return _link;
}

void WidgetShowHistoryCell::setDateTime(QDateTime dateTime)
{
    if (dateTime.isValid())
        ui->labelDateTime->setText(dateTime.toString("yyyy/MM/dd hh:mm"));
    else
        ui->labelDateTime->setText("-");
}

void WidgetShowHistoryCell::setActive(bool isActive)
{
    if (isActive && this->styleSheet() == "")
    {
        this->setStyleSheet(_activeStyleSheet);
        ui->iconFile->setPixmap(s_icons->_fileIconActive);
    }
    else if (!isActive && this->styleSheet() != "")
    {
        this->setStyleSheet("");
        ui->iconFile->setPixmap(s_icons->_fileIcon);
    }
}
