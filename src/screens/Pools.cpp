#include "Pools.h"
#include "AppController.h"
#include "Row.h"
#include <Qontrol>
#include <cstdint>
#include <qlabel.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include "common.h"

namespace screen {

Pools::Pools() {
    this->init();
    this->doConnect();
    this->view();
}

void Pools::recvPayload(QList<payload::Relay*> *payload) {
    auto *old = m_payload;
    m_payload = payload;
    for (auto *relay : *old) {
        delete relay;
    }
    delete old;
    this->view();
}

void Pools::init() {
    m_payload = payload::Relay::dummyRelays();
}

void Pools::doConnect() {}

auto remainingTime(const QDateTime &timeout) -> QString {
    // TODO:
    return "< 1 hour";
}

auto peersCount(uint8_t peers, uint8_t total) -> QString {
    // TODO:
    return "0/0";
}

void insertPool(QTableWidget *table, const payload::Pool *pool, int index) {
    auto *id = 
        new QTableWidgetItem(pool->id);
    id->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, id);

    auto *denomination = 
        new QTableWidgetItem(toBitcoin(pool->denomination, false));
    denomination->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 1, denomination);

    auto *fees = 
        new QTableWidgetItem(QString::number(pool->fees));
    fees->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 2, fees);

    auto *remains = 
        new QTableWidgetItem(remainingTime(pool->timeout));
    remains->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 3, remains);

    auto *peers = 
        new QTableWidgetItem(peersCount(pool->current_peers, pool->total_peers));
    peers->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 4, peers);

    auto poolId = pool->id;
    auto *controller = AppController::get();

    auto *join = new QPushButton("Join");
    QObject::connect(join, &QPushButton::clicked, controller,
            [controller, poolId]() {controller->joinPoolById(poolId);});

    auto *details = new QPushButton("Details");
    QObject::connect(details, &QPushButton::clicked, controller,
            [controller, poolId]() {controller->poolDetails(poolId);});
    auto *row = (new Row(table))
        ->pushSpacer()
        ->push(join)
        ->pushSpacer()
        ->push(details)
        ->pushSpacer()
        ;
    table->setCellWidget(index, 5, row);
}

void Pools::insertRelay(Column *col, const payload::Relay *relay) {
    auto *collapsible = new Collapsible(relay->url, col);

    int rowCount = relay->pools.size() + 1;
    const int c_table_width = 6;
    auto *table = new QTableWidget(rowCount, c_table_width);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto headers = QStringList{
        "Id",
        "Denomination",
        "Fee",
        "Remaining time",
        "Peers",
        "Action"
    };
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (auto *pool : relay->pools) {
        insertPool(table, pool, index);
        index++;
    }

    // merge cells of the last row
    table->setSpan(index, 0, 6, c_table_width);

    auto *create = new QPushButton("Create pool");
    auto *controller = AppController::get();
    connect(create, &QPushButton::clicked, controller, 
            [relay, controller]() {controller->createPoolOnRelay(relay->url);});
    auto *row = (new Row(table))
        -> pushSpacer()
        ->push(create)
        -> pushSpacer()
        ;
    table->setCellWidget(index, 0, row);

    table->resizeColumnsToContents();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    collapsible->pushInner(table);
    collapsible->pushInner(table);

    collapsible->setCollapsed(false);
    col->push(collapsible)
        ->pushSpacer(15)
        ;

    m_tables->push_back(table);
}

void Pools::view() {
    auto *col = new Column;

    auto *oldTables = m_tables;
    m_tables = new QList<QTableWidget*>();
    delete oldTables;

    for (const auto *relay : *m_payload) {
        insertRelay(col, relay);
    }

    col->pushSpacer();

    auto *boxed = margin(col);
    this->setLayout(boxed->layout());
}

} // namespace screen

