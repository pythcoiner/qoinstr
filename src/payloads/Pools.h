#pragma once

#include <QList>
#include <cstdint>
#include <include/cpp_joinstr.h>
#include <include/cxx.h>
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

    static auto dummy() -> Pool*;
    static auto fromRust(rust::Box<RustPool> rpool) -> Pool*;
    auto operator==(const Pool& other) const -> bool;
};

struct Relay {
    QString url;
    QList<Pool*> pools;

    Relay() = default;
    static auto dummy() -> Relay*;
    static auto dummyRelays() -> QList<Relay*>*;
    static auto fromRust(rust::Box<RustPools> rpools) -> QList<Relay*>*;
    auto operator==(const Relay& other) const -> bool;
    void sort();
};


} // namespace payload
