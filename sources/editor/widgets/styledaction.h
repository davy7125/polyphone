#ifndef STYLEDACTION_H
#define STYLEDACTION_H

#include <QToolButton>

class StyledAction : public QToolButton
{
    Q_OBJECT

public:
    StyledAction(QString label, QString iconName, QWidget *parent);
    void disable(bool isDisabled);
    void setData(int data);
    int getData();

signals:
    void clicked();

protected:
    void leaveEvent(QEvent * event) override;
    void enterEvent(QEvent * event) override;
    QPixmap _icon;
    QPixmap _iconHover;
    QPixmap _iconDisabled;

private slots:
    void onToggled(bool isChecked);
    void onClicked(bool isClicked);

private:
    bool _isDisabled;
    static const QSize ICON_SIZE;
    QColor _checkedBackgroundColor;
    int _data;
};

#endif // STYLEDACTION_H
