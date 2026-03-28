#ifndef STYLEDLINEEDITWITHCALENDAR_H
#define STYLEDLINEEDITWITHCALENDAR_H

#include <QLineEdit>
#include <QFrame>
#include <QDate>
#include "styledlineedit.h"
class QCalendarWidget;

class CalendarPopup : public QFrame
{
    Q_OBJECT

public:
    explicit CalendarPopup(QWidget * parent = nullptr);
    void setDate(QDate date);

signals:
    void dateSelected(const QDate &date);

private:
    QCalendarWidget * _calendar;
};

class StyledLineEditWithCalendar : public StyledLineEdit
{
    Q_OBJECT

public:
    explicit StyledLineEditWithCalendar(QWidget *parent = nullptr);
    QString getInitialText() { return _initialText; }

public slots:
    void setInitialText(const QString &text);

private slots:
    void onDateSelected(const QDate &date);
    void onFocusChanged(QWidget * old, QWidget * now);

private:
    CalendarPopup * _popup;
    QString _initialText;
    QDate _currentDate;
};

#endif // STYLEDLINEEDITWITHCALENDAR_H
