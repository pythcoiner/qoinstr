#pragma once

#include <QList>
#include <cstdint>
#include <optional>
#include <qdatetime.h>
#include <qlist.h>

namespace payload {

struct Pool {
    QString id;
    uint64_t denomination = 0;
    uint64_t fees = 0;
    QDateTime timeout;
    uint8_t current_peers = 0;
    uint8_t total_peers = 0;
    std::optional<QString> coin;

    static auto dummy() -> Pool* {
        auto *pool = new Pool();
        pool->id = "aabbccddee";
        pool->denomination = 1000000;
        pool->fees = 5;
        pool->timeout = QDateTime::currentDateTime().addSecs(3600);
        pool->current_peers = 3;
        pool->current_peers = 5;
        return pool;
    }
};

struct Relay {
    QString url;
    QList<Pool*> pools;

    Relay() = default;
    static auto dummy() -> Relay* {
        auto *relay = new Relay();
        relay->url = "wss://relay.nostr";
        for (int i = 0; i < 5; i++) {
            relay->pools.push_back(payload::Pool::dummy());
        }
        return relay;
    }

    static auto dummyRelays() -> QList<Relay*>* { 
        auto *list = new QList<Relay*>();
        list->push_back(Relay::dummy());
        list->push_back(Relay::dummy());
        list->push_back(Relay::dummy());
        return list;
    }
};


} // namespace payload
