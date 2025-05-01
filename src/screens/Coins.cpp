#include "Coins.h"
#include "AccountController.h"
#include "AppController.h"
#include "Row.h"
#include "common.h"
#include <cstdint>
#include <optional>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qtablewidget.h>

namespace screen {

Coins::Coins(AccountController *ctrl) {
    m_controller = ctrl;
    this->init();
    this->doConnect();
    this->view();
}

void Coins::init() {
}

void Coins::recvPayload(const CoinState &coins) {
    if (m_state == coins) {
        // if state content is unchanged, do not update
        return;
    }
    m_state = coins;
    this->view();
    emit coinsUpdated();
}

void Coins::doConnect() {
    auto *ctrl = m_controller;
    connect(ctrl, &AccountController::updateCoins, this, &Coins::recvPayload);
    connect(this, &Coins::coinsUpdated, []() {
        AppController::get()->osInfo("Coins updated",
                                     "List of coins have been updated");
    });
}

auto balanceRow(const QString &label_str, uint64_t balance,
                uint64_t coins_count) -> QWidget * {
    auto *label = new QLabel(label_str);
    label->setFixedWidth(LABEL_WIDTH);

    auto priceStr = toBitcoin(balance);
    auto *price = new QLabel(priceStr);
    price->setFixedWidth(PRICE_WIDTH);

    auto coinStr = coinsCount(coins_count);
    auto *coins = new QLabel(coinStr);
    coins->setFixedWidth(PRICE_WIDTH);

    auto *row = (new qontrol::Row)
                    ->push(label)
                    ->push(price)
                    ->push(coins)
                    ->pushSpacer();

    return row;
}

void insertCoin(QTableWidget *table, const RustCoin &coin, int index) {
    QString blockHeight;
    if (coin.confirmed) {
        blockHeight = QString::number(coin.height);
    } else {
        blockHeight = "";
    }
    auto *value = new QTableWidgetItem(toBitcoin(coin.value, false));
    value->setTextAlignment(Qt::AlignCenter);
    // TODO: populate depth on rust side
    auto *depth = new QTableWidgetItem(QString::number(0));
    depth->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, new QTableWidgetItem(blockHeight));
    auto op = coin.outpoint;
    table->setItem(index, 1, new QTableWidgetItem(QString(op.c_str())));
    auto addr = coin.address.address;
    table->setItem(index, 2, new QTableWidgetItem(QString(addr.c_str())));
    auto label = coin.label;
    table->setItem(index, 3, new QTableWidgetItem(QString(label.c_str())));
    table->setItem(index, 4, value);
    table->setItem(index, 5, depth);
}

void Coins::view() {

    auto *oldCR = m_confirmed_row;
    m_confirmed_row = balanceRow("Confirmed:", m_state.confirmed_balance,
                                 m_state.confirmed_coins);
    delete oldCR;

    auto *oldUR = m_unconfirmed_row;
    m_unconfirmed_row = balanceRow("Unconfirmed:", m_state.unconfirmed_balance,
                                   m_state.unconfirmed_coins);
    delete oldUR;

    int rowCount = m_state.coins.size();
    auto *table = new QTableWidget(rowCount, 6);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto headers = QStringList{"Block Height", "OutPoint", "Address",
                               "Label",        "Value",    "Depth"};
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (const auto &coin : m_state.coins) {
        insertCoin(table, coin, index);
        index++;
    }
    auto *oldTable = m_table;
    m_table = table;
    delete oldTable;
    table->resizeColumnsToContents();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto *mainLayout = (new qontrol::Column(this))
                           ->push(m_confirmed_row)
                           ->pushSpacer(V_SPACER)
                           ->push(m_unconfirmed_row)
                           ->pushSpacer(20)
                           ->push(table);

    auto *boxed = margin(mainLayout);
    delete m_main_widget;
    m_main_widget = boxed;

    auto *old = this->layout();
    delete old;
    this->setLayout(boxed->layout());
}

auto Coins::getCoins() -> std::optional<QList<RustCoin>> {
    auto coins = QList<RustCoin>();
    for (const auto &coin : m_state.coins) {
        coins.append(coin);
    }
    return std::make_optional(coins);
}
} // namespace screen
