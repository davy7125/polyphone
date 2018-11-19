#include "toprightwidget.h"
#include "ui_toprightwidget.h"
#include "mainmenu.h"
#include "contextmanager.h"

TopRightWidget::TopRightWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopRightWidget)
{
    ui->setupUi(this);

    // Style
    if (ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT))
    {
        ui->toolButton->setIcon(ContextManager::theme()->getColoredSvg(":/icons/menu.svg", QSize(28, 28), ThemeManager::WINDOW_TEXT));
        ui->pushUser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(22, 22), ThemeManager::WINDOW_TEXT));

        // Red color for the warning icon
        _colorReplacement["currentColor"] = this->palette().color(QPalette::BrightText).name();
    }
    else
    {
        ui->toolButton->setIcon(ContextManager::theme()->getColoredSvg(":/icons/menu.svg", QSize(28, 28), ThemeManager::WINDOW_BACKGROUND));
        ui->pushUser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(22, 22), ThemeManager::WINDOW_BACKGROUND));

        // Red color for the warning icon
        _colorReplacement["currentColor"] = this->palette().color(QPalette::BrightText).lighter().name();
    }
    ui->toolButton->setStyleSheet("QToolButton::menu-indicator{width:0px;} QToolButton{margin: 3px;padding: 3px;background-color:#000}");

    // Add the main menu
    _menu = new MainMenu(ui->toolButton);
    _menu->setFullScreen(parent->windowState() & Qt::WindowFullScreen);
    ui->toolButton->setMenu(_menu);

    // Connections
    connect(_menu, SIGNAL(newClicked()), this, SIGNAL(newClicked()));
    connect(_menu, SIGNAL(openClicked()), this, SIGNAL(openClicked()));
    connect(_menu, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettingsClicked()));
    connect(_menu, SIGNAL(onlineHelpClicked()), this, SIGNAL(onlineHelpClicked()));
    connect(_menu, SIGNAL(aboutClicked()), this, SIGNAL(aboutClicked()));
    connect(_menu, SIGNAL(closeFileClicked()), this, SIGNAL(closeFileClicked()));
    connect(_menu, SIGNAL(closeClicked()), this, SIGNAL(closeClicked()));
    connect(_menu, SIGNAL(save()), this, SIGNAL(save()));
    connect(_menu, SIGNAL(saveAs()), this, SIGNAL(saveAs()));
    connect(_menu, SIGNAL(fullScreenTriggered()), this, SIGNAL(fullScreenTriggered()));
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)),
            this, SLOT(userStateChanged(UserManager::ConnectionState)));

    userStateChanged(UserManager::DISCONNECTED);
}

TopRightWidget::~TopRightWidget()
{
    delete ui;
}

void TopRightWidget::onEditorOpen(bool isOpen)
{
    _menu->onEditorOpen(isOpen);
}

void TopRightWidget::userStateChanged(UserManager::ConnectionState state)
{
    switch (state)
    {
    case UserManager::DISCONNECTED:
        // Hide everything
        ui->labelWarning->hide();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::PENDING:
        // Show the rotating spinner
        ui->labelWarning->hide();
        ui->pushUser->hide();
        ui->spinner->show();
        ui->spinner->startAnimation();
        break;
    case UserManager::CONNECTED:
        // Show a warning "not premium"
        ui->labelWarning->setToolTip(trUtf8("Souscrivez à un compte Premium pour bénéficier de toutes les fonctionnalités !"));
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::CONNECTED_PREMIUM:
        // Show the user button
        ui->labelWarning->hide();
        ui->pushUser->show();
        ui->spinner->hide();
        break;
    case UserManager::BANNED:
        // Show an error "banned account"
        ui->labelWarning->setToolTip(trUtf8("Ce compte a été banni"));
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/skull.svg", QSize(20, 20), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::FAILED:
        // Show the connection error
        ui->labelWarning->setToolTip(UserManager::getInstance()->error());
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    }
}

void TopRightWidget::on_pushUser_clicked()
{
    emit(userClicked());
}
