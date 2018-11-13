#include "filtertagcell.h"
#include "ui_filtertagcell.h"
#include "contextmanager.h"

FilterTagCell::FilterTagCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterTagCell)
{
    ui->setupUi(this);

    // Style of the widget
    QColor buttonBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor buttonText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor buttonHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    this->setStyleSheet("QPushButton,QLabel{background-color:" + buttonBackground.name() + "; color:" + buttonText.name() +
                        ";padding:5px;border:1px solid " + buttonBackground.name() + "}" +
                        "QPushButton:hover{ background-color:" + buttonHover.name() + ";border:1px solid " + buttonHover.name() + "}" +
                        "QLabel{border-top-left-radius:4px;border-bottom-left-radius:4px}" +
                        "QPushButton{border-top-right-radius:4px;border-bottom-right-radius:4px}");
    ui->pushDelete->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
}

FilterTagCell::~FilterTagCell()
{
    delete ui;
}

void FilterTagCell::on_pushDelete_clicked()
{
    emit(onRemoveClicked());
}

void FilterTagCell::setTagName(QString tagName)
{
    _tagName = tagName;
    ui->labelName->setTextToElide(tagName);
}

QString FilterTagCell::getTagName()
{
    return _tagName;
}
