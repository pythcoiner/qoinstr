#include "AppController.h"
#include "include/cpp_joinstr.h"
#include "payloads/Receive.h"
#include "screens/Coins.h"
#include "screens/Pools.h"
#include "screens/Receive.h"
#include "screens/Send.h"
#include "screens/Settings.h"
#include "screens/modals/CreatePool.h"
#include <algorithm>
#include <cstdint>
#include <include/cxx.h>
#include <optional>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qsystemtrayicon.h>
#include <qtimer.h>
#include <string>

void AppController::initState() {
    qDebug() << "AppController::initState()";

    // init the wallet
    auto mnemonicStr = 
        rust::String("rigid brother public ship curve holiday scrub plate furnace emerge weather derive");
    auto mnemonic = mnemonic_from_string(mnemonicStr);
    if (mnemonic->is_err()) {
        auto errStr = std::string(mnemonic->error());
        qDebug() << "Invalid mnemonic: "<< errStr;
        return;
    }
    auto network = Network::Regtest;
    auto electrum = rust::String("192.168.1.21");
    uint16_t port = 50003;
    auto relay = rust::String("ws://192.168.1.21:8100");
    // look for pools up to yesterday
    uint64_t back = 60 * 60 * 24;

    auto wallet = 
        new_wallet(std::move(mnemonic), network, electrum, port, relay, back);

    auto addr = wallet->recv_addr_at(0);
    qDebug() << std::string(addr);
    m_wallet = std::make_optional(std::move(wallet));

    // init the timer that trigger polls
    m_timer =  new QTimer;
    connect(m_timer, &QTimer::timeout, this, &AppController::poll);
    m_timer->start(100); // poll every 100ms

    m_tray_icon = new QSystemTrayIcon;
    m_tray_icon->setIcon(QIcon::fromTheme("dialog-information")); // required on Linux!
    m_tray_icon->setVisible(true);

}

void AppController::poll() {
    pollNotifications();
}

void AppController::pollCoins() {
    qDebug() << "AppController::pollCoins()";
    if (m_wallet.has_value()) {
        auto rcoins = m_wallet.value()->spendable_coins();
        if (rcoins->is_ok()) {
            auto *coins =  payload::Coins::fromRust(std::move(rcoins));
            emit this->updateCoins(coins);
        }
    }
}

void AppController::pollPools() {
    qDebug() << "AppController::pollPools()";
    if (m_wallet.has_value()) {
        auto rpools = m_wallet.value()->pools();
        if (rpools->is_ok()) {
            auto *pools =  payload::Relay::fromRust(std::move(rpools));
            emit this->updatePools(pools);
        }
    }
}

void AppController::pollAddresses() {
    qDebug() << "AppController::pollAddresses()";

}

void AppController::pollNotifications() {
    if (m_wallet.has_value()) {
        auto poll = m_wallet.value()->try_recv();
        while (poll->is_ok()) {
            auto signal = poll->boxed();
            if (!signal->is_err()) {
                auto s = signal->unwrap();
                if ( s == SignalFlag::CoinUpdate) {
                    pollCoins();
                } else if (s == SignalFlag::PoolUpdate) {
                    pollPools();
                } else if (s == SignalFlag::AddressTipChanged) {
                    pollAddresses();
                } else {
                    auto signalStr = QString(signal_flag_to_string(signal->unwrap()).c_str());
                    qDebug() << "AppController::pollNotification() signal: " << signalStr;
                    osInfo("Info", signalStr);
                }
            } else if (signal->is_err()) {
                auto error = QString(std::string(signal->error()).c_str());
                qDebug() << "AppController::pollNotification() error: " << error;
                osCritical("Error", error, 5000);
            } else {
                qDebug() << "AppController::pollNotification() empty: ";
            }
            poll = m_wallet.value()->try_recv();
        }
    }
}

void AppController::loadPanels() {
    auto *coins = new qontrol::Panel(new screen::Coins(), "coins");
    this->insertPanel(coins);

    auto *pools = new qontrol::Panel(new screen::Pools(), "pools");
    this->insertPanel(pools);

    auto *send = new qontrol::Panel(new screen::Send(), "send");
    this->insertPanel(send);

    auto *receive = new qontrol::Panel(new screen::Receive(), "receive");
    this->insertPanel(receive);

    auto *settings = new qontrol::Panel(new screen::Settings(), "settings");
    this->insertPanel(settings);

    this->loadPanel("coins");
}

void AppController::coinsClicked() {
    this->loadPanel("coins");
}

void AppController::poolsClicked() {
    this->loadPanel("pools");
}

void AppController::sendClicked() {
    this->loadPanel("send");
}

void AppController::receiveClicked() {
    this->loadPanel("receive");
}

void AppController::settingsClicked() {
    this->loadPanel("settings");
}

auto AppController::get() -> AppController* {
    auto *ctrl = Controller::get();
    auto *controller = dynamic_cast<AppController*>(ctrl);
    return controller;
}

auto AppController::coins() -> screen::Coins* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Coins*>(ctrl->screen("coins").value());
    return screen;
}

auto AppController::pools() -> screen::Pools* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Pools*>(ctrl->screen("pools").value());
    return screen;
}

auto AppController::send() -> screen::Send* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Send*>(ctrl->screen("send").value());
    return screen;
}

auto AppController::receive() -> screen::Receive* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Receive*>(ctrl->screen("receive").value());
    return screen;
}

auto AppController::settings() -> screen::Settings* {
    auto *ctrl = AppController::get();
    auto *screen = dynamic_cast<screen::Settings*>(ctrl->screen("settings").value());
    return screen;
}

auto AppController::relay() -> QString {
    auto *ctrl = AppController::get();
    if (ctrl->m_wallet.has_value()) {
        return QString(ctrl->m_wallet.value()->relay().c_str());
    }
    return QString("Unknow relay");

}

void AppController::init() {
    if (Controller::isInit()) {
        qFatal() << "Controller have already been initiated!";
    }
    Controller::init(new AppController);
}

auto AppController::window() -> QWidget* {
    return Controller::window();
}

void AppController::actionCreatePool(payload::Coin coin) { // NOLINT
    auto *modal = new modal::CreatePool(coin);
    AppController::execModal(modal);
}

void AppController::actionCreateNewAddress() {
    if (m_wallet.has_value()) {
        auto raddr = m_wallet.value()->new_addr();
        auto *addr = payload::Address::fromRust(std::move(raddr));

        emit newAddress(addr);
    }
}

void AppController::cmdCreatePool(
    // TODO: pass by value
    const QString &outpoint,
    uint64_t denomination,
    uint32_t fees,
    uint64_t max_duration,
    size_t peers
) {
    if (m_wallet.has_value()) {
        m_wallet.value()->create_dummy_pool(denomination, peers, max_duration, fees);
    }
}

void AppController::listpools() {
    qDebug() << "AppController::listpools()";

    auto *modal = new qontrol::Modal("Error", "Toto blblalbla.........");
    AppController::execModal(modal);
}

void AppController::osMessage(QString title, QString msg, int delay) { // NOLINT 
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::NoIcon, delay);
}
void AppController::osInfo(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::NoIcon, delay);
}
void AppController::osWarning(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::NoIcon, delay);
}
void AppController::osCritical(QString title, QString msg, int delay) { // NOLINT
    m_tray_icon->showMessage(title, msg, QSystemTrayIcon::NoIcon, delay);
}
