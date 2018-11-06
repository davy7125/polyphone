#include "fivestarswidget.h"
#include "ui_fivestarswidget.h"
#include "contextmanager.h"
#include <QPainter>

FiveStarsWidget::IconContainer * FiveStarsWidget::s_icons = NULL;

FiveStarsWidget::FiveStarsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FiveStarsWidget),
    _isActive(true),
    _score(0)
{
    ui->setupUi(this);

    if (s_icons == NULL)
        s_icons = new IconContainer();

    // Star list
    _stars.append(ui->star1);
    _stars.append(ui->star2);
    _stars.append(ui->star3);
    _stars.append(ui->star4);
    _stars.append(ui->star5);
}

FiveStarsWidget::~FiveStarsWidget()
{
    delete ui;
}

void FiveStarsWidget::setScore(double score)
{
    _score = score;
    updateStars();
}

void FiveStarsWidget::setActive(bool isActive)
{
    if ((isActive && !_isActive) || (!isActive && _isActive))
    {
        _isActive = isActive;
        updateStars();
    }
}

void FiveStarsWidget::updateStars()
{
    int starNumber = _stars.count();
    for (int i = 0; i < starNumber; i++)
    {
        // Compute the percentage for the star number i
        double percentage = _score / 10.0 * starNumber - i;
        if (percentage < 0)
            percentage = 0.0;
        else if (percentage > 1)
            percentage = 1;

        // Update the image
        QPixmap mixedStar(s_icons->_baseStar);
        QPainter painter(&mixedStar);
        painter.drawPixmap(0, 0, mixedStar.width() * percentage, mixedStar.height(),
                           _isActive ? s_icons->_selectedColoredStar : s_icons->_coloredStar,
                           0, 0, mixedStar.width() * percentage, mixedStar.height());
        _stars[i]->setPixmap(mixedStar);
    }
}

FiveStarsWidget::IconContainer::IconContainer()
{
    _coloredStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_BACKGROUND);
    _selectedColoredStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT);
    _baseStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::LIST_TEXT);
}
