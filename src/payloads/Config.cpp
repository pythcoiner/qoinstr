#include "Config.h"

namespace payload {
auto Config::fromRust(rust::Box<RustConfig> rconf) -> Config {
    auto config = Config();
    config.electrum_url = QString(rconf->electrum_url().c_str());
    config.electrum_port = QString(rconf->electrum_port().c_str());
    config.nostr_relay = QString(rconf->nostr_url().c_str());
    config.nostr_back = QString(rconf->nostr_back().c_str());
    config.look_ahead = QString(rconf->look_ahead().c_str());
    return config;
}

} // namespace payload
