#include "Coins.h"
#include <algorithm>

namespace payload {

    Coin::Coin(const Coin& other) = default;

    auto Coin::dummy() -> Coin* {
        auto *coin = new Coin();
        coin->date = QDateTime::currentDateTime();
        coin->outpoint = "3b0e62c4136a43c0030bfaff5296658961fd18eced81abdd3a2ecdc7ccef6fa0:0";
        coin->address = *Address::dummy();
        coin->label = "label _____";
        coin->value = 10000000;
        return coin;
    }

    auto Coin::fromRust(rust::Box<RustCoin> coin) -> Coin* {
        auto *out = new Coin();
        out->date = QDateTime();
        out->outpoint = QString(coin->outpoint().c_str());
        out->address = *Address::none();
        out->label = "";
        out->value = coin->amount_sat();
        return out;
    }

    auto Coin::operator==(const Coin& other) const -> bool {
        return date == other.date &&
               outpoint == other.outpoint &&
               address == other.address &&  // assumes Address has operator==
               label == other.label &&
               value == other.value;
    }
} // namespace payload (Coin)

namespace payload {

    auto Coins::dummy() -> Coins* {
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

    auto Coins::fromRust(rust::Box<RustCoins> coins) -> Coins* {
        auto *out = new Coins;
        for (size_t i = 0; i<coins->count(); i++) {
            auto *c = Coin::fromRust(coins->get(i));
            out->coins.push_back(c);
        }
        out->update();
        return out;
    }

    void Coins::update() {
        // sort coins 
        std::ranges::sort(coins, [](const Coin* a, const Coin* b) {
            if (a->date == b->date) {
                return a->outpoint < b->outpoint;  // Secondary: lexicographic
            }
            return a->date < b->date;  // Primary: earlier date first
        });

        confirmed_balance = 0;
        confirmed_coins = 0;

        for (const Coin* coin : coins) {
            confirmed_balance += coin->value;
            confirmed_coins += 1;
        }

        // All coins assumed confirmed
        unconfirmed_balance = 0;
        unconfirmed_coins = 0;
    }

    auto Coins::operator==(const Coins& other) const -> bool {
        if (confirmed_balance != other.confirmed_balance ||
            unconfirmed_balance != other.unconfirmed_balance ||
            confirmed_coins != other.confirmed_coins ||
            unconfirmed_coins != other.unconfirmed_coins ||
            coins.size() != other.coins.size()) {
            return false;
        }

        for (size_t i = 0; i < coins.size(); ++i) {
            if (!(*coins[i] == *other.coins[i])) {
                return false;
            }
        }

        return true;
    }

} // namespace payload (Coins)
