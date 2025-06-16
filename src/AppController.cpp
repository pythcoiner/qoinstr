#include "AppController.h"
#include "MainWindow.h"
#include "Modal.h"
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

auto AppController::get() -> AppController * {
    auto *ctrl = Controller::get();
    auto *controller = dynamic_cast<AppController *>(ctrl);
    return controller;
}

void AppController::initState() {
    auto logger = new_logger(LogLevel::Debug);
    logger->module("simple_nostr_client", LogLevel::Info);
    logger->module("simple_electrum_client", LogLevel::Info);
    logger->module("joinstr::electrum", LogLevel::Info);
    logger->module("cpp_joinstr::account", LogLevel::Info);
    logger->module("joinstr::nostr::sync", LogLevel::Info);
    // logger->module("joinstr::joinstr", LogLevel::Info);
    logger->init();

    m_tray_icon = new QSystemTrayIcon;
    m_tray_icon->setIcon(
        QIcon::fromTheme("dialog-information")); // required on Linux!
    m_tray_icon->setVisible(true);

    listAccounts();

    connect(this, &AppController::accountCreated, this,
            &AppController::onAccountCreated, qontrol::UNIQUE);
}

void AppController::addAccount(
    const QString
        &name) { // NOLINT(readability-convert-member-functions-to-static)
    auto *window = AppController::window();
    auto *win = dynamic_cast<MainWindow *>(window);
    if (!win->accountExists(name)) {
        auto *acc = new AccountWidget(name);
        win->insertAccount(acc, name);
        m_accounts.insert(name, acc->controller());

    } else {
        qCritical() << "Account " << name << " already exists!";
    }
}

void AppController::removeAccount(
    const QString
        &account) { // NOLINT(readability-convert-member-functions-to-static)
    auto *win = dynamic_cast<MainWindow *>(window());
    win->removeAccount(account);
    m_accounts.remove(account);
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

void AppController::createAccount(const QString &name, const QString &mnemonic,
                                  Network network) {
    if (!config_exists(name.toStdString())) {
        auto config = new_config(mnemonic.toStdString(), name.toStdString(),
                                 network);
        config->set_nostr_relay("ws://127.0.0.1:8181");
        config->set_nostr_back("10000");
        config->set_electrum_url("127.0.0.1");
        config->set_electrum_port("50001");
        config->to_file();

        emit accountCreated(name);
    } else {

        auto *modal = new qontrol::Modal(
            "Account already exists!",
            "An account file with the same name already exists!");
        AppController::execModal(modal);
    }
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

void AppController::osCritical(QString title, QString msg,
                               int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::Critical, delay);
}

auto AppController::accounts() -> int {
    return m_accounts.size();
}

void AppController::stop(modal::Stop *modal) {
    for (auto *account : m_accounts) {
        connect(account, &AccountController::stopped, modal,
                &modal::Stop::onStopped, qontrol::UNIQUE);
        account->stop();
    }
    m_tray_icon->hide();
    m_tray_icon->deleteLater();
}
