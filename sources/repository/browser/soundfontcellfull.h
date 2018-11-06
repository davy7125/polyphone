#ifndef SOUNDFONTCELLFULL_H
#define SOUNDFONTCELLFULL_H

#include <QWidget>
class SoundfontInformation;
class SoundfontFilter;

namespace Ui {
class SoundfontCellFull;
}

class SoundfontCellFull : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontCellFull(SoundfontInformation* soundfontInfo, QWidget *parent = 0);
    ~SoundfontCellFull();
    void setActive(bool isActive);
    bool hasHeightForWidth() const;
    int heightForWidth(int width) const override;

signals:
    void itemClicked(SoundfontFilter * filter);

private slots:
    void on_labelAuthor_linkActivated(const QString &link);

private:
    class IconContainer
    {
    public:
        IconContainer();

        QPixmap _downloadIcon;
        QPixmap _downloadIconSelected;
        QPixmap _commentIcon;
        QPixmap _commentIconSelected;
        QPixmap _userIcon;
        QPixmap _userIconSelected;
        QPixmap _dateIcon;
        QPixmap _dateIconSelected;
        QPixmap _copyrightIcon;
        QPixmap _copyrightIconSelected;
    };

    void initIcons();

    QString _title;
    Ui::SoundfontCellFull *ui;
    QString _normalStyleSheet;
    QString _activeStyleSheet;
    bool _active;
    QString _authorTextNoColor;
    QString _licenseTextNoColor;
    static IconContainer * s_icons;
};

#endif // SOUNDFONTCELLFULL_H
