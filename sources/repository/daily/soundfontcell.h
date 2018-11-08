#ifndef SOUNDFONTCELL_H
#define SOUNDFONTCELL_H

#include <QWidget>
class SoundfontInformation;
class SoundfontFilter;

namespace Ui {
class SoundfontCell;
}

class SoundfontCell : public QWidget
{
    Q_OBJECT

public:
    SoundfontCell(SoundfontInformation* soundfontInfo, QWidget *parent = 0);
    ~SoundfontCell();
    void setActive(bool isActive);
    bool hasHeightForWidth() const;
    int heightForWidth(int width) const override;
    int getSoundfontId() { return _soundfontId; }

signals:
    void itemClicked(SoundfontFilter * filter);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void on_labelAuthor_linkActivated(const QString &link);

private:
    class IconContainer
    {
    public:
        IconContainer();

        QPixmap _userIcon;
        QPixmap _userIconSelected;
    };

    Ui::SoundfontCell *ui;
    QString _normalStyleSheet;
    QString _activeStyleSheet;
    bool _active;
    QString _authorTextNoColor;
    int _soundfontId;
    static IconContainer * s_icons;
};

#endif // SOUNDFONTCELL_H
