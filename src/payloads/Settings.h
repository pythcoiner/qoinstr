#pragma once

#include <QList>
#include <cstdint>
#include <qcontainerfwd.h>

namespace payload {

enum class Network : uint8_t {
    Bitcoin,
    Testnet,
    Signet,
    Regtest
};

struct Electrum {
    QString url;
    uint16_t port = 0;
    Network network = Network::Bitcoin;
    bool validate_domain = true;
    bool connected = false;
};

struct Relay {
    QString url;
    bool connected;
};

struct Settings {
    Electrum electrum;
    QList<Relay> relays;
    Network network = Network::Bitcoin;
};

} // namespace payload
