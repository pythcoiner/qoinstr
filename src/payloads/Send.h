#pragma once

#include <QList>
#include <cstdint>

namespace payload {

struct Output {
    QString address;
    uint64_t amount = 0;
    QString label;
    bool max = false;
};

struct Send {
    QList<Output> outputs;
    QList<QString /*coin*/> coins;
    bool max = false;
    uint64_t fee = 0;
};

} // namespace payload
