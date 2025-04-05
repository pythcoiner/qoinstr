#pragma once

#include "../payloads/Coins.h"
#include "AccountController.h"
#include <Qontrol>
#include <cstdint>
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

signals:
    void coinsUpdated();

public slots:
    void recvPayload(payload::Coins *payload);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    payload::Coins *m_payload = nullptr;
    QWidget *m_main_widget = nullptr;
    QTableWidget *m_table = nullptr;
    QList<QString /*outpoint*/> m_selected_coins;
    uint64_t m_amount_selected = 0;
    AccountController *m_controller = nullptr;
};

} // namespace screen
