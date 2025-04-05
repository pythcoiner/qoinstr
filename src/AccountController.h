#pragma once

#include "AccountWidget.h"
#include "Screen.h"
#include "payloads/Config.h"
#include "payloads/Receive.h"
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

class AccountWidget;

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

class AccountController: public QObject {
    Q_OBJECT 
public:
    AccountController(const QString &account, AccountWidget *widget);
    void init(const QString &account);
    auto screen(const QString &screen) -> std::optional<qontrol::Screen*>;
    void loadPanels();

    auto coins() -> screen::Coins*;
    auto pools() -> screen::Pools*;
    auto send() -> screen::Send*;
    auto receive() -> screen::Receive*;
    auto settings() -> screen::Settings*;
    auto relay() -> QString;

signals:
    // Backend => GUI
    void updateCoins(payload::Coins*);
    void updatePools(QList<payload::Relay*>* pools);
    void newAddress(payload::Address*);
    void loadConfig(payload::Config);

public slots:
    void loadPanel(const QString &name);
    void insertPanel(qontrol::Panel *panel);

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
    QPointer<qontrol::Panel> m_current_panel;
    QHash<QString, qontrol::Panel*> m_panels;
    AccountWidget *m_widget;
    std::optional<rust::Box<Account>> m_wallet = std::nullopt;
    QTimer *m_timer = nullptr;
};
