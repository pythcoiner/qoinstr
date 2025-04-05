#include "Pools.h"

#include "AccountController.h"
#include <algorithm>

namespace payload {

    auto Pool::dummy() -> Pool* {
        auto *pool = new Pool();
        pool->id = "aabbccddee";
        pool->denomination = 1000000;
        pool->fees = 5;
        pool->timeout = QDateTime::currentDateTime().addSecs(3600);
        pool->current_peers = 3;
        pool->total_peers = 5;
        return pool;
    }

    auto Pool::fromRust(rust::Box<RustPool> rpool) -> Pool* {
        auto *pool = new Pool();
        pool->id = QString(rpool->id().c_str());
        pool->denomination = rpool->denomination_sat();
        pool->fees = rpool->fees();
        pool->timeout = QDateTime::fromSecsSinceEpoch(rpool->timeout());
        pool->current_peers = 0;
        pool->total_peers = rpool->peers();
        return pool;
    }

    auto Pool::operator==(const Pool& other) const -> bool {
        return id == other.id &&
               denomination == other.denomination &&
               fees == other.fees &&
               timeout == other.timeout &&
               current_peers == other.current_peers &&
               total_peers == other.total_peers;
    }

} // namespace payload (Pool)

namespace payload {

    auto Relay::dummy() -> Relay* {
        auto *relay = new Relay();
        relay->url = "wss://relay.nostr";
        for (int i = 0; i < 5; i++) {
            relay->pools.push_back(payload::Pool::dummy());
        }
        return relay;
    }

    auto Relay::dummyRelays() -> QList<Relay*>* { 
        auto *list = new QList<Relay*>;
        list->push_back(Relay::dummy());
        list->push_back(Relay::dummy());
        list->push_back(Relay::dummy());
        return list;
    }

    auto Relay::fromRust(rust::Box<RustPools> rpools, const QString &relay_url) -> QList<Relay*>* {
        auto *relay = new Relay();
        relay->url = relay_url;

        for (size_t i = 0; i < rpools->count() ; i++) {
            auto *pool = Pool::fromRust(rpools->get(i));
            relay->pools.push_back(pool);
        }
        auto *relays = new QList<Relay*>;
        relays->push_back(relay);
        return relays;
    }

    auto Relay::operator==(const Relay& other) const -> bool {
        if (url != other.url || pools.size() != other.pools.size()) {
            return false;
        }

        for (int i = 0; i < pools.size(); ++i) {
            if (!(*pools[i] == *other.pools[i])) {
                return false;
            }
        }

        return true;
    }

    void Relay::sort() {
        std::ranges::sort(pools, [](const Pool* a, const Pool* b) {
            if (a->timeout == b->timeout) {
                return a->id < b->id; // Secondary: sort by id
            }
            return a->timeout < b->timeout; // Primary: sort by timeout
        });
    }

} // namespace payload (Relay)
