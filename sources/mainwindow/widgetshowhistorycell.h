#ifndef WIDGETSHOWHISTORYCELL_H
#define WIDGETSHOWHISTORYCELL_H

#include <QWidget>

namespace Ui {
class WidgetShowHistoryCell;
}

class WidgetShowHistoryCell : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetShowHistoryCell(QWidget *parent = 0);
    ~WidgetShowHistoryCell();

    void setLink(QString filePath);
    QString getLink();
    void setDateTime(QDateTime dateTime);
    void setActive(bool isActive);

private:
    class Icons
    {
    public:
        Icons();

        QPixmap _fileIcon;
        QPixmap _fileIconActive;
    };

    Ui::WidgetShowHistoryCell *ui;
    QString _link;
    QString _activeStyleSheet;
    static Icons * s_icons;
};

#endif // WIDGETSHOWHISTORYCELL_H
