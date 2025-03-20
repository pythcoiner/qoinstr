#include "Receive.h"

namespace payload {


    auto Address::dummy() -> Address* {
        auto *address = new Address;
        address->address = "bc1qk3sr4edee6xugc3lu550j2z03rulmmfny2s43t";
        return address;
    }

    auto Address::none() -> Address* {
        auto *address = new Address;
        address->address = "";
        return address;
    }

    auto Address::dummyAddresses() -> QList<Address*>* {
        auto *addresses = new QList<Address*>;
        for (int i = 0; i < 2; i++) {
            addresses->push_back(Address::dummy());
        }
        return addresses;
    }

    auto Address::operator==(const Address& other) const -> bool {
        return address == other.address &&
               label == other.label &&
               change == other.change &&
               index == other.index;
    }

} // namespace payload
