#include "common.h"
#include "Column.h"
#include "Row.h"

auto margin(QWidget *widget) -> QWidget* {
    auto *col = ( new Column)
        ->pushSpacer(TOP_MARGIN)
        ->push(widget)
        ->pushSpacer(BOTTOM_MARGIN)
        ;
    auto *row = (new Row)
        ->pushSpacer(LEFT_MARGIN)
        ->push(col)
        ->pushSpacer(RIGHT_MARGIN)
        ;

    return row;
}

auto toBitcoin(uint64_t sats, bool with_unit) -> QString {
    double bitcoinValue = static_cast<double>(sats) / SATS;
    auto btcStr = QString::number(bitcoinValue);
    if (with_unit) {
        return btcStr + " BTC";
    }
    return btcStr;
}

auto coinsCount(uint64_t count) -> QString {
    auto coinsStr = QString::number(count);
    return coinsStr + " coins";
}
