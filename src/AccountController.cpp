#include "AccountController.h"
#include "AccountWidget.h"
#include "AppController.h"
#include "include/cpp_joinstr.h"
#include "screens/Coins.h"
#include "screens/Pools.h"
#include "screens/Receive.h"
#include "screens/Send.h"
#include "screens/Settings.h"
#include "screens/modals/CreatePool.h"
#include "screens/modals/SelectCoins.h"
#include <cstdint>
#include <include/cxx.h>
#include <optional>
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qlogging.h>
#include <qsystemtrayicon.h>
#include <qtimer.h>
#include <string>
#include <utility>

AccountController::AccountController(const QString &account,
                                     AccountWidget *widget) {
    m_widget = widget;
    init(account);
}

void AccountController::init(const QString &account) {
    if (m_init)
        return;

    // init the wallet
    auto wallet = new_account(account.toStdString());

    m_wallet = std::make_optional(std::move(wallet));

    // init the timer that poll notifications
    m_notif_timer = new QTimer;
    connect(m_notif_timer, &QTimer::timeout, this, &AccountController::poll);
    m_notif_timer->start(100); // poll every 100ms
    // init the timer that poll coins
    m_coins_timer = new QTimer;
    connect(m_coins_timer, &QTimer::timeout, this,
            &AccountController::pollCoins);
    m_coins_timer->start(1000);

    m_init = true;
}

void AccountController::loadPanel(const QString &name) {
    auto *panel = m_panels.value(name);
    if (m_widget != nullptr) {
        m_widget->loadPanel(panel);
    }
}

void AccountController::insertPanel(qontrol::Panel *panel) {
    m_panels.insert(panel->name(), panel);
}

auto AccountController::screen(const QString &screen)
    -> std::optional<qontrol::Screen *> {
    auto *panel = m_panels.value(screen);
    if (panel != nullptr) {
        return std::optional(panel->screen());
    }
    return std::nullopt;
}

void AccountController::poll() {
    if (!m_first_poll) {
        m_first_poll = true;
        pollPools();
    }
    pollNotifications();
}

void AccountController::pollCoins() {
    if (m_wallet.has_value()) {
        m_wallet.value()->generate_coins();
        auto rcoins = m_wallet.value()->spendable_coins();
        emit this->updateCoins(rcoins);
    }
}

void AccountController::pollPools() {
    qDebug() << "AppController::pollPools()";
    if (m_wallet.has_value()) {
        auto rpools = m_wallet.value()->pools();
        if (rpools->is_ok()) {
            auto pools = QList<RustPool>();
            for (const auto &p : rpools->value()) {
                pools.append(p);
            }
            emit this->updatePools(pools, rpools->relay().c_str());
        }
    }
}

void AccountController::pollAddresses() {
    qDebug() << "AppController::pollAddresses()";
}

void AccountController::pollNotifications() {
    if (m_wallet.has_value()) {
        auto poll = m_wallet.value()->try_recv();
        while (poll->is_ok()) {
            auto signal = poll->signal();
            if (!signal->is_err()) {
                auto s = signal->unwrap();
                if (s == SignalFlag::CoinUpdate) {
                } else if (s == SignalFlag::PoolUpdate) {
                    pollPools();
                } else if (s == SignalFlag::AddressTipChanged) {
                    pollAddresses();
                } else if (s == SignalFlag::Stopped) {
                    emit stopped();
                } else {
                    auto signalStr = QString(
                        signal_flag_to_string(signal->unwrap()).c_str());
                    qDebug() << "AppController::pollNotification() signal: "
                             << signalStr;
                    AppController::get()->osInfo("Info", signalStr);
                }
            } else if (signal->is_err()) {
                auto error = QString(std::string(signal->error()).c_str());
                qDebug() << "AppController::pollNotification() error: "
                         << error;
                AppController::get()->osCritical("Error", error, 5000);
            } else {
                qDebug() << "AppController::pollNotification() empty: ";
            }
            poll = m_wallet.value()->try_recv();
        }
    }
}

void AccountController::loadPanels() {
    auto *cScreen = new screen::Coins(this);
    auto *coins = new qontrol::Panel(cScreen, "coins");
    this->insertPanel(coins);

    auto *pScreen = new screen::Pools(this);
    auto *pools = new qontrol::Panel(pScreen, "pools");
    this->insertPanel(pools);

    auto *sScreen = new screen::Send(this);
    auto *send = new qontrol::Panel(sScreen, "send");
    this->insertPanel(send);

    auto *rScreen = new screen::Receive(this);
    auto *receive = new qontrol::Panel(rScreen, "receive");
    this->insertPanel(receive);

    auto *ssScreen = new screen::Settings(this);
    auto *settings = new qontrol::Panel(ssScreen, "settings");
    this->insertPanel(settings);

    this->loadPanel("settings");
}

void AccountController::coinsClicked() {
    this->loadPanel("coins");
}

void AccountController::poolsClicked() {
    this->loadPanel("pools");
}

void AccountController::sendClicked() {
    this->loadPanel("send");
}

void AccountController::receiveClicked() {
    this->loadPanel("receive");
}

void AccountController::settingsClicked() {
    this->loadPanel("settings");
}

auto AccountController::coins() -> screen::Coins * {
    auto *screen = dynamic_cast<screen::Coins *>(this->screen("coins").value());
    return screen;
}

auto AccountController::pools() -> screen::Pools * {
    auto *screen = dynamic_cast<screen::Pools *>(this->screen("pools").value());
    return screen;
}

auto AccountController::send() -> screen::Send * {
    auto *screen = dynamic_cast<screen::Send *>(this->screen("send").value());
    return screen;
}

auto AccountController::receive() -> screen::Receive * {
    auto *screen = dynamic_cast<screen::Receive *>(
        this->screen("receive").value());
    return screen;
}

auto AccountController::settings() -> screen::Settings * {
    auto *screen = dynamic_cast<screen::Settings *>(
        this->screen("settings").value());
    return screen;
}

auto AccountController::relay() -> QString {
    if (m_wallet.has_value()) {
        return QString(m_wallet.value()->relay().c_str());
    }
    return QString("Unknow relay");
}

void AccountController::actionCreatePoolForRelay(const QString &relay_url) {

    auto coins = selectableCoins();
    if (!coins.isEmpty()) {
        auto *modal = new modal::SelectCoins(coins, relay_url);
        connect(modal, &modal::SelectCoins::coinSelectedForCreatePool, this,
                &AccountController::actionCreatePool, qontrol::UNIQUE);
        AppController::execModal(modal);
    }
}

void AccountController::actionCreateNewAddress() {
    if (m_wallet.has_value()) {
        auto raddr = m_wallet.value()->new_addr();

        emit newAddress(raddr);
    }
}

void AccountController::cmdCreatePool(const rust::String &outpoint,
                                      uint64_t denomination, uint32_t fees,
                                      uint64_t max_duration, size_t peers) {
    if (m_wallet.has_value()) {
        // FIXME: its a hack in order to wait the fee calculation to be fixed
        m_wallet.value()->create_pool(outpoint, denomination, 1, max_duration,
                                      peers);
    }
}

void AccountController::cmdSaveConfig(
    payload::Config payload) { // NOLINT(performance-unnecessary-value-param)
    qDebug() << "AppController::cmdSaveConfig()";
    if (m_wallet.has_value()) {
        m_wallet.value()->set_electrum(payload.electrum_url.toStdString(),
                                       payload.electrum_port.toStdString());
        m_wallet.value()->set_nostr(payload.nostr_relay.toStdString(),
                                    payload.nostr_back.toStdString());
        m_wallet.value()->set_look_ahead(payload.look_ahead.toStdString());
    }
}

void AccountController::cmdLoadConfig() {
    qDebug() << "AppController::cmdLoadConfig()";
    if (m_wallet.has_value()) {
        auto rconf = m_wallet.value()->get_config();
        auto config = payload::Config::fromRust(std::move(rconf));
        emit loadConfig(config);
    }
}

void AccountController::listpools() {
    qDebug() << "AppController::listpools()";

    auto *modal = new qontrol::Modal("Error", "Toto blblalbla.........");
    AppController::execModal(modal);
}

void AccountController::stop() {
    if (m_wallet.has_value()) {
        m_wallet.value()->stop();
    }
    emit stopped();
}

void AccountController::actionCreatePool(const RustCoin &coin,
                                         const QString &relay_url) {
    auto *modal = new modal::CreatePool(coin, relay_url, this);
    AppController::execModal(modal);
}

auto AccountController::cmdPrepareTx(TransactionTemplate tx_template)
    -> std::optional<QString /* PSBT */> {
    if (!m_wallet.has_value()) {
        return std::nullopt;
    }
    auto psbt = m_wallet.value()->prepare_transaction(std::move(tx_template));
    if (psbt->is_ok()) {
        auto psbtStr = psbt->value();
        return QString(psbtStr.c_str());
    }
    auto error = psbt->error();
    qDebug() << "AccountController::cmdPrepareTx() Fail to prepare "
                "transaction(): "
             << error.c_str();
    return std::nullopt;
}

void AccountController::cmdJoinPool(const RustCoin &coin,
                                    const QString &pool_id,
                                    const QString &relay_url) {
    auto outpoint = coin.outpoint;
    auto poolId = rust::String(pool_id.toStdString());
    if (m_wallet.has_value()) {
        m_wallet.value()->join_pool(outpoint, poolId);
    }
};

void AccountController::actionJoinPool(const QString &relay_url,
                                       const QString &pool_id) {
    auto coins = selectableCoins();
    if (!coins.isEmpty()) {
        auto *modal = new modal::SelectCoins(coins, relay_url, pool_id);
        connect(modal, &modal::SelectCoins::coinSelectedForJoinPool, this,
                &AccountController::cmdJoinPool, qontrol::UNIQUE);
        AppController::execModal(modal);
    }
};

auto AccountController::selectableCoins() -> QList<RustCoin> {
    auto optCoins = this->coins()->getCoins();
    QList<RustCoin> coins;
    if (optCoins.has_value() && !optCoins.value().isEmpty()) {
        for (const auto &c : optCoins.value()) {
            auto mc = RustCoin(c);
            coins.append(mc);
        }
    } else {
        auto *modal = new qontrol::Modal("No coins!",
                                         "There is no coins to select!");
        AppController::execModal(modal);
    }
    return coins;
};

auto AccountController::simulateTx(TransactionTemplate tx_template)
    -> TransactionSimulation {
    if (m_wallet.has_value()) {
        return m_wallet.value()->simulate_transaction(std::move(tx_template));
    }
    qDebug() << "AccountController::simulateTx() m_wallet is None!";
};
