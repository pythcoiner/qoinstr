#pragma once

#include <cstdint>
#include <qwidget.h>

const int MARGIN = 30;
const int LEFT_MARGIN = MARGIN;
const int TOP_MARGIN = MARGIN;
const int RIGHT_MARGIN = MARGIN;
const int BOTTOM_MARGIN = MARGIN;

const int LABEL_WIDTH = 120;
const int INPUT_WIDTH = 200;
const int PRICE_WIDTH = 200;

const int V_SPACER = 5;
const int H_SPACER = 5;

const int SATS = 100000000;

auto margin(QWidget *widget) -> QWidget *;
auto toBitcoin(uint64_t sats, bool with_unit = true) -> QString;
auto coinsCount(uint64_t count) -> QString;
