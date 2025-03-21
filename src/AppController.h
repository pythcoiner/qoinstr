#pragma once

#include "screens/Coins.h"
#include <QObject>
#include <Qontrol>
#include <cmath>
#include <cstdint>
#include <include/cpp_joinstr.h>
#include <optional>
#include <qdialog.h>
#include <qtimer.h>
#include <qsystemtrayicon.h>

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

    // GUI => Backend

public slots:
    void initState();
    void poll();
    void pollCoins();
    void pollPools();
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

    // Wallet Commands
    void cmdCreatePool(
        const QString &outpoint,
        uint64_t denomination,
        uint32_t fees,
        uint64_t max_duration,
        size_t peers
    );

    
    void listpools();

private:
    std::optional<rust::Box<Wallet>> m_wallet = std::nullopt;
    QTimer *m_timer = nullptr;
    payload::Coins *m_coins = nullptr;
    QSystemTrayIcon *m_tray_icon;
    
};
