#include "configsectionrepository.h"
#include "ui_configsectionrepository.h"
#include "contextmanager.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFileDialog>
#include "utils.h"

ConfigSectionRepository::ConfigSectionRepository(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigSectionRepository)
{
    ui->setupUi(this);
    ui->pushDirectory->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-open.svg", QSize(16, 16), ThemeManager::WINDOW_TEXT));

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
    // User identifier and password
    ui->lineUser->setText(ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString());
    int passwordLength = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password_length", 0).toInt();
    QString fake;
    fake.resize(passwordLength);
    ui->linePassword->setText(fake); // Same number of characters
    _fakePassword = true;

    // Force the initialization of the user state
    onConnectionStateChanged(UserManager::getInstance()->getConnectionState());
    if (_currentState == UserManager::DISCONNECTED)
        ui->labelMessage->setText("");

    // Download directory
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (defaultDir.isEmpty())
        defaultDir = QDir::homePath();
    ui->lineDownloadDirectory->blockSignals(true);
    ui->lineDownloadDirectory->setText(ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "directory", defaultDir).toString());
    ui->lineDownloadDirectory->blockSignals(false);
}

void ConfigSectionRepository::onConnectionStateChanged(UserManager::ConnectionState connectionState)
{
    _currentState = connectionState;
    QString messageContent;
    QColor messageColor;
    int currentIndex = 0;
    QString buttonText;

    switch (_currentState)
    {
    case UserManager::DISCONNECTED:
        messageContent = "";
        messageColor = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
                                         ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT), 0.5);
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
        buttonText = trUtf8("Se déconnecter");
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
    case UserManager::FAILED:
        // Save the user name and the encrypted password
        ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "username", ui->lineUser->text());
        if (!_fakePassword) // If it didn't change, keep the old encrypted password
        {
            QString password = ui->linePassword->text();
            ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "password", Utils::rsaEncrypt(password));
            ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "password_length", ui->linePassword->text().size());
        }

        // Try to log
        ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "auto_connect", true);
        UserManager::getInstance()->login();
        break;
    case UserManager::PENDING:
    case UserManager::CONNECTED:
    case UserManager::CONNECTED_PREMIUM:
    case UserManager::BANNED:
        // Disconnect the user or stop the connection
        ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "auto_connect", false);
        UserManager::getInstance()->logout();
        break;
    }
}

void ConfigSectionRepository::on_lineDownloadDirectory_editingFinished()
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "directory", ui->lineDownloadDirectory->text());
}

void ConfigSectionRepository::on_lineUser_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    if (_currentState == UserManager::DISCONNECTED)
        ui->labelMessage->setText(trUtf8("Cliquez sur %0 pour sauvegarder les paramètres.").arg("<b>" + ui->pushConnect->text() + "</b>"));
}

void ConfigSectionRepository::on_linePassword_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    if (_currentState == UserManager::DISCONNECTED)
        ui->labelMessage->setText(trUtf8("Cliquez sur %0 pour sauvegarder les paramètres.").arg("<b>" + ui->pushConnect->text() + "</b>"));
    _fakePassword = false;
}

void ConfigSectionRepository::on_pushDirectory_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisissez un répertoire de destination"),
                                                     ui->lineDownloadDirectory->text());
    if (!qDir.isEmpty())
    {
        ui->lineDownloadDirectory->setText(qDir);
        on_lineDownloadDirectory_editingFinished();
    }
}

void ConfigSectionRepository::on_linePassword_returnPressed()
{
    if (!_fakePassword && (_currentState == UserManager::DISCONNECTED || _currentState == UserManager::FAILED))
        this->on_pushConnect_clicked();
}
