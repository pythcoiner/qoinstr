#pragma once

#include "screens/Coins.h"
#include <cstdint>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qvalidator.h>

namespace modal {

class CreatePool : public QDialog {
    Q_OBJECT
public:
    CreatePool(const payload::Coin &coin);

signals:
    void createPool(
        const QString &outpoint,
        uint64_t denomination,
        uint32_t fees,
        uint64_t max_duration,
        size_t peers
    );

public slots:
    void onCreatePool();
    void process(bool force_denom = false);

private:
    auto fetch() -> bool;



private:
    payload::Coin m_coin;

    // Processed values
    uint64_t m_denomination = 0;
    uint32_t m_fees = 0;
    uint64_t m_max_duration = 0;
    size_t m_peers = 0;

    // Widgets
    QLineEdit *m_w_denomination;
    QLineEdit *m_w_fee;
    QLineEdit *m_w_peers;
    QLineEdit *m_w_timeout;
    QPushButton *m_create_btn;

    // Input validators
    QIntValidator *m_fees_validator;
    QIntValidator *m_peers_validator;
    QIntValidator *m_timeout_validator;
    QDoubleValidator *m_denom_validator;

};

} // namespace modal
