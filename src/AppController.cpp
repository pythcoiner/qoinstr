#include "AppController.h"
#include "MainWindow.h"
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

    // DEBUG:
    addAccount("main");
    addAccount("main2");

    // TODO: load existing wallet that have open == true

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
