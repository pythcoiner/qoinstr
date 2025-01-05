#pragma once

#include <QList>
#include <cstdint>
#include <qlist.h>

namespace payload {

struct Address {
    QString address;
    QString label;
    bool change = false;
    uint64_t index = 0;

    static auto dummy() -> Address* {
        auto *address = new Address;
        address->address = "bc1qk3sr4edee6xugc3lu550j2z03rulmmfny2s43t";
        return address;
    }

    static auto dummyAddresses() -> QList<Address*>* {
        auto *addresses = new QList<Address*>;
        for (int i = 0; i < 2; i++) {
            addresses->push_back(Address::dummy());
        }
        return addresses;
    }
};

} // namespace payload
