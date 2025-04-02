#pragma once

#include "include/cpp_joinstr.h"
#include <QList>
#include <cstdint>
#include <qcontainerfwd.h>

namespace payload {

struct Config {
    QString electrum_url;
    QString electrum_port;
    QString nostr_relay;
    QString nostr_back;

    static auto fromRust(rust::Box<RustConfig> rconf) -> Config;
};

} // namespace payload
