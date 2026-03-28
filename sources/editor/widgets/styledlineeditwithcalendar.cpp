#include "styledlineeditwithcalendar.h"
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QEvent>
#include <QApplication>

CalendarPopup::CalendarPopup(QWidget * parent) : QFrame(parent, Qt::FramelessWindowHint)
{
    Q_UNUSED(parent)
    setFrameShape(QFrame::Box);

    _calendar = new QCalendarWidget(this);
    _calendar->setWeekdayTextFormat(Qt::Saturday, _calendar->weekdayTextFormat(Qt::Monday));
    _calendar->setWeekdayTextFormat(Qt::Sunday, _calendar->weekdayTextFormat(Qt::Monday));
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(_calendar);

    connect(_calendar, &QCalendarWidget::clicked, this, &CalendarPopup::dateSelected);
}

void CalendarPopup::setDate(QDate date)
{
    _calendar->setSelectedDate(date);
}

StyledLineEditWithCalendar::StyledLineEditWithCalendar(QWidget * parent) : StyledLineEdit(parent)
{
    _popup = new CalendarPopup(this->parentWidget());
    connect(_popup, &CalendarPopup::dateSelected, this, &StyledLineEditWithCalendar::onDateSelected);
    connect(qApp, &QApplication::focusChanged, this, &StyledLineEditWithCalendar::onFocusChanged);
    connect(this, &QLineEdit::textEdited, this, &StyledLineEditWithCalendar::setInitialText);
}

void StyledLineEditWithCalendar::onFocusChanged(QWidget * old, QWidget * now)
{
    Q_UNUSED(old)
    if (now != nullptr && (now == this || _popup->isAncestorOf(now) || this->isAncestorOf(now)))
    {
        if (!_popup->isVisible())
        {
            QPoint globalPos = this->mapToGlobal(this->rect().bottomLeft());
            QPoint parentPos = _popup->parentWidget()->mapFromGlobal(globalPos);
            _popup->move(parentPos);
            _popup->raise();
            _popup->show();
        }
    }
    else
        _popup->hide();
}

void StyledLineEditWithCalendar::onDateSelected(const QDate &date)
{
    setInitialText(date.toString("yyyy-MM-dd"));
    emit(editingFinished());
}

void StyledLineEditWithCalendar::setInitialText(const QString &text)
{
    _initialText = text;
    _currentDate = QDate(); // Invalid date

    // Try to find the datetime with the current locale or with the EN locale
    QDateTime dateTime = this->locale().toDateTime(_initialText, "dddd d MMMM yyyy, hh:mm:ss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(_initialText, "dddd d MMMM yyyy, hh:mm:ss");
    if (!dateTime.isValid())
        dateTime = QDateTime::fromString(_initialText, "yyyy-MM-ddThh:mm:ss");
    if (dateTime.isValid())
        _currentDate = dateTime.date();

    // Maybe a date?
    if (!_currentDate.isValid())
        _currentDate = QDate::fromString(_initialText, "yyyy-MM-dd");

    this->setTextToElide(_currentDate.isValid() ? this->locale().toString(_currentDate, QLocale::LongFormat) : _initialText);

    if (_currentDate.isValid())
        _popup->setDate(_currentDate);
}
