#ifndef SOUNDFONTDESCRIPTION_H
#define SOUNDFONTDESCRIPTION_H

#include <QWidget>
class SoundfontDescriptionData;
class QTextDocument;

namespace Ui {
class SoundfontDescription;
}

class SoundfontDescription : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontDescription(QWidget *parent = 0);
    ~SoundfontDescription();

    void display(SoundfontDescriptionData * data);

private slots:
    void on_webView_titleChanged(const QString &title);

private:
    static QString getCss();
    static QString getJs();
    QString getBody(SoundfontDescriptionData * data);

    Ui::SoundfontDescription *ui;
    static QString s_css;
    static QString s_js;
};

#endif // SOUNDFONTDESCRIPTION_H
