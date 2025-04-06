#include "AppController.h"
#include "MainWindow.h"
#include "common.h"
#include "include/cpp_joinstr.h"
#include "screens/modals/CreateAccount.h"
#include <qlogging.h>
#include <qsystemtrayicon.h>

AppController::AppController() = default;

void AppController::init() {
    if (Controller::isInit()) {
        qFatal() << "Controller have already been initialized!";
    }
    Controller::init(new AppController);
}

auto AppController::get() -> AppController* {
    auto *ctrl = Controller::get();
    auto *controller = dynamic_cast<AppController*>(ctrl);
    return controller;
}

void AppController::initState() {
    init_rust_logger(LogLevel::Info);

    m_tray_icon = new QSystemTrayIcon;
    m_tray_icon->setIcon(QIcon::fromTheme("dialog-information")); // required on Linux!
    m_tray_icon->setVisible(true);

    listAccounts();

    connect(this, &AppController::accountCreated, this, &AppController::onAccountCreated, qontrol::UNIQUE);

    // // DEBUG:
    // addAccount("main");
    // addAccount("main2");

}

void AppController::addAccount(const QString &name) { // NOLINT(readability-convert-member-functions-to-static)
    auto *window = AppController::window();
    auto *win = dynamic_cast<MainWindow*>(window);
    if (!win->accountExists(name)) {
        auto *acc = new AccountWidget(name);
        win->insertAccount(acc, name);

    } else {
        qCritical() << "Account " << name << " already exists!";
    }
}

void AppController::removeAccount(const QString &account) { // NOLINT(readability-convert-member-functions-to-static)
    auto *win = dynamic_cast<MainWindow*>(window());
    win->removeAccount(account);
}

void AppController::listAccounts() {

    auto raccounts = list_configs();
    auto accounts = QList<QString>();
    for (auto acc : raccounts) {
        accounts.append(acc.c_str());
    }
    emit accountList(accounts);
}

void AppController::onCreateAccount() {
    auto *modal = new modal::CreateAccount();
    AppController::execModal(modal);
}

void AppController::createAccount(const QString &name, const QString &mnemonic, Network network) {
    auto config = config_from_file(name.toStdString());
    config->set_account(name.toStdString());
    config->set_mnemonic(mnemonic.toStdString());
    config->set_network(network);
    config->to_file();

    emit accountCreated(name);
}

void AppController::onAccountCreated(const QString &name) {
    qDebug() << "AppController::onAccountCreated()";
    addAccount(name);
}

void AppController::osMessage(QString title, QString msg, int delay) { // NOLINT 
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::NoIcon, delay);
}
void AppController::osInfo(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::Information, delay);
}
void AppController::osWarning(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::Warning, delay);
}
void AppController::osCritical(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::Critical, delay);
}
