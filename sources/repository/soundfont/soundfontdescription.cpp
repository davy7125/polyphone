#include "soundfontdescription.h"
#include "ui_soundfontdescription.h"
#include "soundfontdescriptiondata.h"
#include "contextmanager.h"
#include <QWebFrame>

QString SoundfontDescription::s_css = "";
QString SoundfontDescription::s_js = "";

SoundfontDescription::SoundfontDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDescription)
{
    ui->setupUi(this);
    ui->webView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

SoundfontDescription::~SoundfontDescription()
{
    delete ui;
}

void SoundfontDescription::display(SoundfontDescriptionData * data)
{
    // Set the content
    QString content = "<html><head>"
                      "<meta http-equiv='Content-Type' content='text/html;charset=utf-8'/>"
                      "<style>%0</style>"
                      "<script type='text/javascript'>%1</script>"
                      "</head>"
                      "<body onresize='updateTitle()'><div id='description'>%2</div></body></html>";
    ui->webView->setHtml(content
                         .arg(getCss())
                         .arg(getJs())
                         .arg(getBody(data))
                         );
}

QString SoundfontDescription::getCss()
{
    if (s_css == "")
    {
        s_css = "body { color:%0; padding: 0; margin:0; font-family:%2; text-align: justify;}"
                "h1, h2, h3, h4, h5, h5, h6 { color: %1; font-weight: bold; } "
                "a {color:%1; text-decoration: none; }"
                "img { max-width: 100%; }"
                "table { border-collapse: collapse; }"
                "th, td { text-align: left; border: 1px solid %3; padding: 0 5px; }";
        s_css = s_css
                .arg(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name())
                .arg(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name())
                .arg(QApplication::font().family())
                .arg(ThemeManager::mix(
                         ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                         ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                         0.5).name());
    }

    return s_css;
}

QString SoundfontDescription::getJs()
{
    if (s_js == "")
    {
        s_js = "var updateTitle = function() {"
               "  document.title = document.getElementById('description').offsetHeight;"
               "};"
               "window.onload = function() {"
               "  div = document.getElementById('description');"
               "  div.addEventListener('onresize', updateTitle());"
               "  updateTitle();"
               "};";
    }

    return s_js;
}

QString SoundfontDescription::getBody(SoundfontDescriptionData * data)
{
    QString str = data->getText();
    if (str == "")
        str = "<p>" + trUtf8("Pas de description.") + "</p>";
    return str;
}

void SoundfontDescription::on_webView_titleChanged(const QString &title)
{
    if (title.isEmpty())
        return;

    // Automatically resize the QWebView
    bool ok = false;
    int height = title.toInt(&ok);
    if (ok)
        ui->webView->setMinimumHeight(height);
}
