#include "Coins.h"
#include "AccountController.h"
#include "AppController.h"
#include "Row.h"
#include "common.h"
#include <cstdint>
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
    m_payload = new payload::Coins();
}

void Coins::recvPayload(payload::Coins *payload) {
    if (*m_payload == *payload) {
        // if payload content is unchanged, do not update
        return;
    }
    auto *old = m_payload;
    m_payload = payload;
    delete old;
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

void insertCoin(AccountController *ctrl, QTableWidget *table,
                const payload::Coin *coin, int index) {
    QString date;
    if (coin->date.has_value()) {
        date = coin->date.value().toString();
    } else {
        date = "";
    }
    auto *value = new QTableWidgetItem(toBitcoin(coin->value, false));
    value->setTextAlignment(Qt::AlignCenter);
    auto *depth = new QTableWidgetItem(QString::number(coin->depth));
    depth->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, new QTableWidgetItem(date));
    table->setItem(index, 1, new QTableWidgetItem(coin->outpoint));
    table->setItem(index, 2, new QTableWidgetItem(coin->address.address));
    table->setItem(index, 3, new QTableWidgetItem(coin->label));
    table->setItem(index, 4, value);
    table->setItem(index, 5, depth);

    auto outpoint = coin->outpoint;
    auto *controller = ctrl;

    auto *join = new QPushButton("Join");
    // QObject::connect(join, &QPushButton::clicked, controller,
    //         [controller, outpoint]() {controller->joinPool(outpoint);});

    auto ccoin = payload::Coin(*coin);
    auto *create = new QPushButton("Create");
    QObject::connect(
        create, &QPushButton::clicked, controller,
        [controller, ccoin]() { controller->actionCreatePool(ccoin); });
    auto *row = (new qontrol::Row(table))
                    ->pushSpacer()
                    ->push(join)
                    ->pushSpacer()
                    ->push(create)
                    ->pushSpacer();
    table->setCellWidget(index, 6, row);
}

void Coins::view() {

    auto *oldCR = m_confirmed_row;
    m_confirmed_row = balanceRow("Confirmed:", m_payload->confirmed_balance,
                                 m_payload->confirmed_coins);
    delete oldCR;

    auto *oldUR = m_unconfirmed_row;
    m_unconfirmed_row = balanceRow(
        "Unconfirmed:", m_payload->unconfirmed_balance,
        m_payload->unconfirmed_coins);
    delete oldUR;

    int rowCount = m_payload->coins.size();
    auto *table = new QTableWidget(rowCount, 7);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto headers = QStringList{"Date",  "OutPoint", "Address", "Label",
                               "Value", "Depth",    "Pool"};
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (auto *coin : m_payload->coins) {
        insertCoin(m_controller, table, coin, index);
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

auto Coins::getCoins() -> std::optional<QList<payload::Coin>> {
    return m_payload->getCoins();
}
} // namespace screen
