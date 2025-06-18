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
}

auto remainingTime(const QDateTime &timeout) -> QString {
    // TODO:
    return "< 1 hour";
}

auto peersCount(uint8_t peers, uint8_t total, PoolRole role, PoolStatus status)
    -> QString {
    if (role == PoolRole::Initiator || role == PoolRole::Peer) {
        if (status == PoolStatus::RegisterInputs ||
            status == PoolStatus::RegisterOutputs) {
            auto count = QString::number(peers) + "/" + QString::number(total);
            return count;
        }
        return "-";
    }
    return "?";
}

void insertPool(QTableWidget *table, const RustPool &pool, int index,
                AccountController *ctrl) {
    auto idStr = pool.id;
    auto statusStr = pool_status_to_string(pool.status);
    auto roleStr = pool_role_to_string(pool.role);
    auto *id = new QTableWidgetItem(QString(idStr.c_str()));
    id->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, id);

    auto *status = new QTableWidgetItem(statusStr.c_str());
    status->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 1, status);

    auto *role = new QTableWidgetItem(roleStr.c_str());
    role->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 2, role);

    auto *denomination = new QTableWidgetItem(
        toBitcoin(pool.denomination, false));
    denomination->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 3, denomination);

    auto *fees = new QTableWidgetItem(QString::number(pool.fees));
    fees->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 4, fees);

    auto timeout = QDateTime::fromMSecsSinceEpoch(pool.timeout);
    auto *remains = new QTableWidgetItem(remainingTime(timeout));
    remains->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 5, remains);

    auto *peers = new QTableWidgetItem(peersCount(
        pool.current_peers, pool.total_peers, pool.role, pool.status));
    peers->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 6, peers);

    auto poolId = QString::fromStdString(std::string(pool.id));
    auto relayUrl = QString::fromStdString(std::string(pool.relay));

    auto *join = new QPushButton("Join");

    bool joinVisible = (pool.status == PoolStatus::Available) &&
                       (pool.role == PoolRole::None);
    bool detailsVisible = pool.role != PoolRole::None;

    QObject::connect(join, &QPushButton::clicked, [ctrl, poolId, relayUrl]() {
        ctrl->actionJoinPool(relayUrl, poolId);
    });

    auto *details = new QPushButton("Details");
    // QObject::connect(details, &QPushButton::clicked, controller,
    //         [controller, poolId]() {controller->poolDetails(poolId);});

    join->setVisible(joinVisible);
    details->setVisible(detailsVisible);

    auto *row = (new qontrol::Row(table))
                    ->pushSpacer()
                    ->push(join)
                    ->pushSpacer()
                    ->push(details)
                    ->pushSpacer();
    table->setCellWidget(index, 7, row);
}

void Pools::insertRelay(qontrol::Column *col, AccountController *ctrl) {
    auto *collapsible = new qontrol::widgets::Collapsible(m_state.second, col);

    m_collapsibles->push_back(collapsible);

    int rowCount = m_state.first.size() + 1;
    const int c_table_width = 8;
    auto *table = new QTableWidget(rowCount, c_table_width);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    table->setMinimumHeight((rowCount + 1) * 30);
    auto headers = QStringList{"Id",  "Status",         "Role",  "Denomination",
                               "Fee", "Remaining time", "Peers", "Action"};
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (const auto &pool : m_state.first) {
        insertPool(table, pool, index, ctrl);
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

    insertRelay(col, m_controller);

    col->pushSpacer();

    auto *boxed = margin(col);

    auto *old = this->layout();
    delete old;

    this->setLayout(boxed->layout());
}

} // namespace screen
