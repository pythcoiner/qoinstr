#include "payloads/Receive.h"
#include <QList>
#include <cstdint>
#include <include/cpp_joinstr.h>
#include <include/cxx.h>
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
    Coin(const Coin &other);
    static auto dummy() -> Coin *;
    static auto fromRust(rust::Box<RustCoin> coin) -> Coin *;

    auto operator==(const Coin &other) const -> bool;
};

struct Coins {
    uint64_t confirmed_balance = 0;
    uint64_t unconfirmed_balance = 0;
    uint64_t confirmed_coins = 0;
    uint64_t unconfirmed_coins = 0;
    QList<Coin *> coins;

    Coins() = default;
    static auto dummy() -> Coins *;
    static auto fromRust(rust::Box<RustCoins> coins) -> Coins *;

    void update();
    auto operator==(const Coins &other) const -> bool;
};

} // namespace payload
