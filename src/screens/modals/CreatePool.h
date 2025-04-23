#pragma once

#include "AccountController.h"
#include "screens/Coins.h"
#include <cstdint>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qvalidator.h>

namespace modal {

class CreatePool : public qontrol::Modal {
    Q_OBJECT
public:
    CreatePool(const payload::Coin &coin, AccountController *ctrl);

signals:
    void createPool(const QString &outpoint, uint64_t denomination, uint32_t fees, uint64_t max_duration, size_t peers);

public slots:
    void onCreatePool();
    void process(bool force_denom = false);

private:
    auto fetch() -> bool;

private:
    payload::Coin m_coin;
    QString m_relay;
    AccountController *m_controller = nullptr;

    // Processed values
    uint64_t m_denomination = 0;
    uint32_t m_fees = 0;
    uint64_t m_max_duration = 0;
    size_t m_peers = 0;

    // Widgets
    QLineEdit *m_w_denomination = nullptr;
    QLineEdit *m_w_fee = nullptr;
    QLineEdit *m_w_peers = nullptr;
    QLineEdit *m_w_timeout = nullptr;
    QPushButton *m_create_btn = nullptr;

    // Input validators
    QIntValidator *m_fees_validator = nullptr;
    QIntValidator *m_peers_validator = nullptr;
    QIntValidator *m_timeout_validator = nullptr;
    QDoubleValidator *m_denom_validator = nullptr;
};

} // namespace modal
