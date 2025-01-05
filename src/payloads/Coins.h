#include "payloads/Receive.h"
#include <QList>
#include <cstdint>
#include <optional>
#include <qdatetime.h>

namespace payload {

struct Coin {
    std::optional<QDateTime> date = std::nullopt;
    QString outpoint;
    Address address;
    QString label;
    uint64_t value = 0;
    uint16_t depth = 0;
    bool locked = false;

    Coin() = default;
    static auto dummy() -> Coin* {
        auto *coin = new Coin();
        coin->date = QDateTime::currentDateTime();
        coin->outpoint = "3b0e62c4136a43c0030bfaff5296658961fd18eced81abdd3a2ecdc7ccef6fa0:0";
        coin->address = *Address::dummy();
        coin->label = "label _____";
        coin->value = 10000000;
        return coin;
    }
};

struct Coins {
    uint64_t confirmed_balance = 0;
    uint64_t unconfirmed_balance = 0;
    uint64_t confirmed_coins = 0;
    uint64_t unconfirmed_coins = 0;
    QList<Coin*> coins;

    Coins() = default;
    static auto dummy() -> Coins* {
        auto *coins = new Coins();
        coins->confirmed_balance = 1000000000;
        coins->unconfirmed_balance = 10000000;
        coins->confirmed_coins = 123;
        coins->unconfirmed_coins = 4;
        for (int i = 0; i < 35; i++) {
            coins->coins.push_back(payload::Coin::dummy());
        }
        return coins;
    }
};


} // namespace payload
