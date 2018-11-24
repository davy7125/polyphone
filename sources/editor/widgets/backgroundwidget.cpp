#include "backgroundwidget.h"
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include "contextmanager.h"

const int BackgroundWidget::SIZE = 600;
const int BackgroundWidget::OFFSET_X = 150;
const int BackgroundWidget::OFFSET_Y = 50;
const double BackgroundWidget::COLOR_RATIO = 0.2;

BackgroundWidget::BackgroundWidget(QWidget *parent) : QWidget(parent)
{
    // Color of the decoration
    QMap<QString, QString> replacements;
    replacements["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                                     ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                     COLOR_RATIO).name();
    _decoration = ContextManager::theme()->getColoredSvg(":/misc/decoration.svg", QSize(SIZE, SIZE), replacements);
}

void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(opt.rect, ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND));
    p.drawPixmap(opt.rect.left() - OFFSET_X, opt.rect.bottom() - _decoration.height() + OFFSET_Y, _decoration);
    QWidget::paintEvent(event);
}
