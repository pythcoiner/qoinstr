#pragma once

#include "payloads/Receive.h"
#include "payloads/Config.h"
#include "screens/Coins.h"
#include <QObject>
#include <Qontrol>
#include <cmath>
#include <cstdint>
#include <include/cpp_joinstr.h>
#include <optional>
#include <qdialog.h>
#include <qsystemtrayicon.h>
#include <qtimer.h>

namespace payload {
class Relay;
}

namespace screen {
class Coins;
class Pools;
class Send;
class Receive;
class Settings;
} // namespace screen

class AppController : public qontrol::Controller {
    Q_OBJECT 
public:
    void loadPanels() override;
    static void init();
    static auto get() -> AppController*;
    static auto window() -> QWidget*;

    static auto coins() -> screen::Coins*;
    static auto pools() -> screen::Pools*;
    static auto send() -> screen::Send*;
    static auto receive() -> screen::Receive*;
    static auto settings() -> screen::Settings*;
    static auto relay() -> QString;

signals:
    // Backend => GUI
    void updateCoins(payload::Coins*);
    void updatePools(QList<payload::Relay*>* pools);
    void newAddress(payload::Address*);
    void loadConfig(payload::Config);

public slots:
    void initState();
    void poll();
    void pollCoins();
    void pollPools();
    void pollAddresses();
    void pollNotifications();

    // Main buttons actions
    void coinsClicked();
    void poolsClicked();
    void sendClicked();
    void receiveClicked();
    void settingsClicked();

    // OS Notifications
    void osMessage(QString title, QString msg, int delay=3000);
    void osInfo(QString title, QString msg, int delay=3000);
    void osWarning(QString title, QString msg, int delay=3000);
    void osCritical(QString title, QString msg, int delay=3000);

    // User Actions handlers
    void actionCreatePool(payload::Coin coin);
    void actionCreateNewAddress();

    // Wallet Commands
    void cmdCreatePool(
        const QString &outpoint,
        uint64_t denomination,
        uint32_t fees,
        uint64_t max_duration,
        size_t peers
    );
    void cmdSaveConfig(payload::Config payload);
    void cmdLoadConfig();

    
    void listpools();

private:
    std::optional<rust::Box<Account>> m_wallet = std::nullopt;
    QTimer *m_timer = nullptr;
    payload::Coins *m_coins = nullptr;
    QSystemTrayIcon *m_tray_icon;
    
};
