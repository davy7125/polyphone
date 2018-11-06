#ifndef FIVESTARSWIDGET_H
#define FIVESTARSWIDGET_H

#include <QWidget>
class QLabel;

namespace Ui {
class FiveStarsWidget;
}

class FiveStarsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FiveStarsWidget(QWidget *parent = 0);
    ~FiveStarsWidget();

    // Set the score, between 0 and 10
    void setScore(double score);

    // Set true if the widget is selected
    void setActive(bool isActive);

private:
    class IconContainer
    {
    public:
        IconContainer();

        QPixmap _baseStar;
        QPixmap _coloredStar;
        QPixmap _selectedColoredStar;
    };

    void updateStars();

    Ui::FiveStarsWidget *ui;
    bool _isActive;
    double _score;
    QList<QLabel *> _stars;
    static IconContainer * s_icons;
};

#endif // FIVESTARSWIDGET_H
