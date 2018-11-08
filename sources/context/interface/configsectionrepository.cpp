#include "configsectionrepository.h"
#include "ui_configsectionrepository.h"
#include "contextmanager.h"
#include <QDesktopServices>
#include <QUrl>

ConfigSectionRepository::ConfigSectionRepository(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionRepository)
{
    ui->setupUi(this);

    // Style for "Create an account"
    QString styleSheet = "QToolButton{border:none; background-color:" + ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND).name() + ";}";
    ui->buttonCreateAccount->setStyleSheet(styleSheet);
    ui->buttonCreateAccount->setIcon(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(48, 48), ThemeManager::WINDOW_TEXT));

    // Style for "Become Premium"
    ui->buttonBecomePremium->setStyleSheet(styleSheet);
    ui->buttonBecomePremium->setIcon(ContextManager::theme()->getColoredSvg(":/icons/crown.svg", QSize(48, 48), ThemeManager::HIGHLIGHTED_BACKGROUND));

    // Style for "Premium"
    ui->labelPremium->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/crown.svg", QSize(64, 64), ThemeManager::HIGHLIGHTED_BACKGROUND));

    // Style for "Ban"
    ui->labelBan1->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/skull.svg", QSize(48, 48), ThemeManager::WINDOW_TEXT));

    // Connection
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)),
            this, SLOT(onConnectionStateChanged(UserManager::ConnectionState)));
}

ConfigSectionRepository::~ConfigSectionRepository()
{
    delete ui;
}

void ConfigSectionRepository::initialize()
{

}

void ConfigSectionRepository::onConnectionStateChanged(UserManager::ConnectionState connectionState)
{
    _currentState = connectionState;
    QString messageContent;
    QColor messageColor;
    int currentIndex;
    QString buttonText;

    switch (connectionState)
    {
    case UserManager::DISCONNECTED:
        messageContent = "";
        currentIndex = 0;
        buttonText = trUtf8("Connexion");
        break;
    case UserManager::PENDING:
        messageContent = trUtf8("Connexion en cours...");
        messageColor = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                                         ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT), 0.5);
        currentIndex = 0;
        buttonText = trUtf8("Annuler");
        break;
    case UserManager::CONNECTED:
        messageContent = trUtf8("Connecté");
        messageColor = this->palette().color(QPalette::NoRole);
        currentIndex = 1;
        buttonText = trUtf8("Se déconnecter");
        break;
    case UserManager::CONNECTED_PREMIUM:
        messageContent = trUtf8("Connecté");
        messageColor = this->palette().color(QPalette::NoRole);
        currentIndex = 2;
        buttonText = trUtf8("Se déconnecter");
        break;
    case UserManager::BANNED:
        messageContent = "";
        currentIndex = 3;
        buttonText = trUtf8("Connexion");
        break;
    case UserManager::FAILED:
        messageContent = UserManager::getInstance()->error();
        messageColor = this->palette().color(QPalette::BrightText);
        currentIndex = 0;
        buttonText = trUtf8("Connexion");
        break;
    }

    ui->labelMessage->setText(messageContent);
    ui->labelMessage->setStyleSheet("QLabel{color:" + messageColor.name() + "}");
    ui->stackedWidget->setCurrentIndex(currentIndex);
    ui->pushConnect->setText(buttonText);
}

void ConfigSectionRepository::on_lineUser_editingFinished()
{

}

void ConfigSectionRepository::on_linePassword_editingFinished()
{

}

void ConfigSectionRepository::on_buttonCreateAccount_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/en/create-an-account"));
}

void ConfigSectionRepository::on_buttonBecomePremium_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/en/subscribe"));
}

void ConfigSectionRepository::on_pushConnect_clicked()
{
    switch (_currentState)
    {
    case UserManager::DISCONNECTED:
    case UserManager::BANNED:
    case UserManager::FAILED:
        UserManager::getInstance()->login();
        break;
    case UserManager::PENDING:
    case UserManager::CONNECTED:
    case UserManager::CONNECTED_PREMIUM:
        UserManager::getInstance()->logout();
        break;
    }
}
