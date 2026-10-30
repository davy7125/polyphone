#include "styledlineeditwithcalendar.h"
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>

CalendarPopup::CalendarPopup(QWidget * parent) : QFrame(parent, Qt::FramelessWindowHint)
{
    Q_UNUSED(parent)
    setFrameShape(QFrame::Box);
    this->setAutoFillBackground(true);

    // Calendar
    _calendar = new QCalendarWidget(this);
    _calendar->setWeekdayTextFormat(Qt::Saturday, _calendar->weekdayTextFormat(Qt::Monday));
    _calendar->setWeekdayTextFormat(Qt::Sunday, _calendar->weekdayTextFormat(Qt::Monday));
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(_calendar);

    // Buttons
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(6, 0, 6, 6);
    QPushButton * cancelButton = new QPushButton(tr("&Cancel"), this);
    QPushButton * okButton = new QPushButton(tr("&Ok"), this);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    layout->addLayout(buttonLayout);

    // Connections
    connect(_calendar, &QCalendarWidget::activated, this, &CalendarPopup::dateSelected);
    connect(cancelButton, &QPushButton::clicked, this, &QWidget::hide);
    connect(okButton, &QPushButton::clicked, this, [this]{ emit dateSelected(getDate()); });
}

void CalendarPopup::focusDate()
{
    auto view = _calendar->findChild<QTableView *>();
    if (view)
        view->setFocus();
}

void CalendarPopup::setDate(QDate date)
{
    _calendar->setSelectedDate(date);
}

QDate CalendarPopup::getDate() const
{
    return _calendar->selectedDate();
}

StyledLineEditWithCalendar::StyledLineEditWithCalendar(QWidget * parent) : StyledLineEdit(parent)
{
    _popup = new CalendarPopup(this->parentWidget());
    _popup->hide(); // Otherwise displayed with its parent, until the next focus change
    connect(_popup, &CalendarPopup::dateSelected, this, &StyledLineEditWithCalendar::onDateSelected);
    connect(qApp, &QApplication::focusChanged, this, &StyledLineEditWithCalendar::onFocusChanged);
    connect(this, &QLineEdit::textEdited, this, &StyledLineEditWithCalendar::setInitialText);
    _popup->installEventFilter(this);
    this->installEventFilter(this);
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
        closePopup();
}

void StyledLineEditWithCalendar::onDateSelected(const QDate &date)
{
    if (_currentDate != date)
    {
        setInitialText(date.toString("yyyy-MM-dd"));
        emit(editingFinished());
    }
    closePopup();
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

bool StyledLineEditWithCalendar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Escape)
        {
            keyEvent->accept();
            closePopup();
            return true;
        }
        else if (obj == this && keyEvent->key() == Qt::Key_Down)
        {
            keyEvent->accept();
            QTimer::singleShot(0, _popup, &CalendarPopup::focusDate);
            return true;
        }
    }

    return StyledLineEdit::eventFilter(obj, event);
}

void StyledLineEditWithCalendar::closePopup()
{
    this->setInitialText(_initialText);
    QTimer::singleShot(0, _popup, &QWidget::hide);
    this->clearFocus();
}