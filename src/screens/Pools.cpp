#include "Pools.h"
#include "AccountController.h"
#include "AppController.h"
#include "Row.h"
#include "common.h"
#include "include/cpp_joinstr.h"
#include "widgets/Collapsible.h"
#include <Qontrol>
#include <cstdint>
#include <qlabel.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qtablewidget.h>

namespace screen {

Pools::Pools(AccountController *ctrl) {
    m_controller = ctrl;
    this->init();
    this->doConnect();
    this->view();
}

void Pools::recvPayload(const QList<RustPool> &pools, const QString &relay) {
    emit poolsUpdated();

    m_state = QPair<QList<RustPool>, QString>(pools, relay);
    this->view();
}

void Pools::init() {
}

void Pools::doConnect() {
    auto *ctrl = m_controller;
    connect(ctrl, &AccountController::updatePools, this, &Pools::recvPayload);
    connect(this, &Pools::poolsUpdated, ctrl, []() {
        AppController::get()->osInfo("Pools updated",
                                     "List of pools have been updated");
    });
}

auto remainingTime(const QDateTime &timeout) -> QString {
    // TODO:
    return "< 1 hour";
}

auto peersCount(uint8_t peers, uint8_t total) -> QString {
    // TODO:
    return "0/0";
}

void insertPool(QTableWidget *table, const RustPool &pool, int index) {
    auto idStr = pool.id;
    auto *id = new QTableWidgetItem(QString(idStr.c_str()));
    id->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, id);

    auto *denomination = new QTableWidgetItem(
        toBitcoin(pool.denomination, false));
    denomination->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 1, denomination);

    auto *fees = new QTableWidgetItem(QString::number(pool.fees));
    fees->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 2, fees);

    auto timeout = QDateTime::fromMSecsSinceEpoch(pool.timeout);
    auto *remains = new QTableWidgetItem(remainingTime(timeout));
    remains->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 3, remains);

    auto *peers = new QTableWidgetItem(
        peersCount(pool.current_peers, pool.total_peers));
    peers->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 4, peers);

    auto poolId = pool.id;
    // auto *controller = ctrl;

    auto *join = new QPushButton("Join");
    // QObject::connect(join, &QPushButton::clicked, controller,
    //         [controller, poolId]() {controller->joinPoolById(poolId);});

    auto *details = new QPushButton("Details");
    // QObject::connect(details, &QPushButton::clicked, controller,
    //         [controller, poolId]() {controller->poolDetails(poolId);});
    auto *row = (new qontrol::Row(table))
                    ->pushSpacer()
                    ->push(join)
                    ->pushSpacer()
                    ->push(details)
                    ->pushSpacer();
    table->setCellWidget(index, 5, row);
}

void Pools::insertRelay(qontrol::Column *col) {
    auto *collapsible = new qontrol::widgets::Collapsible(m_state.second, col);

    m_collapsibles->push_back(collapsible);

    int rowCount = m_state.first.size() + 1;
    const int c_table_width = 6;
    auto *table = new QTableWidget(rowCount, c_table_width);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    table->setMinimumHeight((rowCount + 1) * 30);
    auto headers = QStringList{"Id",    "Denomination", "Fee", "Remaining time",
                               "Peers", "Action"};
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (const auto &pool : m_state.first) {
        insertPool(table, pool, index);
        index++;
    }

    // merge cells of the last row
    table->setSpan(index, 0, 6, c_table_width);

    auto *create = new QPushButton("Create pool");
    auto *controller = m_controller;
    auto relay = m_state.second;
    connect(create, &QPushButton::clicked, controller, [relay, controller]() {
        controller->actionCreatePoolForRelay(relay);
    });
    auto *row = (new qontrol::Row(table))
                    ->pushSpacer()
                    ->push(create)
                    ->pushSpacer();
    table->setCellWidget(index, 0, row);

    table->resizeColumnsToContents();
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    collapsible->pushInner(table);
    collapsible->pushInner(table);

    collapsible->setCollapsed(true);
    col->push(collapsible)->pushSpacer(15);

    m_tables->push_back(table);
}

void Pools::view() {
    auto *col = new qontrol::Column;

    auto *oldTables = m_tables;
    m_tables = new QList<QTableWidget *>;
    if (oldTables != nullptr) {
        for (auto *item : *oldTables) {
            delete item;
        }
    }
    delete oldTables;

    auto *oldCollapsibles = m_collapsibles;
    m_collapsibles = new QList<qontrol::widgets::Collapsible *>;
    if (oldCollapsibles != nullptr) {
        for (auto *item : *oldCollapsibles) {
            delete item;
        }
    }
    delete oldCollapsibles;

    insertRelay(col);

    col->pushSpacer();

    auto *boxed = margin(col);

    auto *old = this->layout();
    delete old;

    this->setLayout(boxed->layout());
}

} // namespace screen
