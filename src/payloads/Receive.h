#pragma once

#include "include/cpp_joinstr.h"
#include <QList>
#include <cstdint>
#include <qlist.h>

namespace payload {

struct Address {
    QString address;
    QString label;
    bool change = false;
    uint64_t index = 0;

    static auto dummy() -> Address*;
    static auto none() -> Address*;
    static auto dummyAddresses() -> QList<Address*>*;
    static auto fromRust(rust::Box<RustAddress>) -> Address*;

    auto operator==(const Address& other) const -> bool;
};

} // namespace payload
