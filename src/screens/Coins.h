#pragma once

#include "AccountController.h"
#include <Qontrol>
#include <cstdint>
#include <optional>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {

class Coins : public qontrol::Screen {
    Q_OBJECT
public:
    Coins(AccountController *ctrl);
    auto getCoins() -> std::optional<QList<RustCoin>>;

signals:
    void coinsUpdated();

public slots:
    void recvPayload(const CoinState &coins);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

private:
    CoinState m_state;
    QWidget *m_main_widget = nullptr;
    QWidget *m_confirmed_row = nullptr;
    QWidget *m_unconfirmed_row = nullptr;
    QTableWidget *m_table = nullptr;
    QList<QString /*outpoint*/> m_selected_coins;
    uint64_t m_amount_selected = 0;
    AccountController *m_controller = nullptr;
};

} // namespace screen
