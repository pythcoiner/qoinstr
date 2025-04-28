#include "Pools.h"
#include "AccountController.h"
#include "AppController.h"
#include "Row.h"
#include "common.h"
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

auto relaysEquals(const QList<payload::Relay *> &a,
                  const QList<payload::Relay *> &b) -> bool {
    if (a.size() != b.size()) {
        return false;
    }

    for (int i = 0; i < a.size(); ++i) {
        const payload::Relay *lhs = a[i];
        const payload::Relay *rhs = b[i];

        if ((lhs == nullptr) || (rhs == nullptr)) {
            if (lhs != rhs)
                return false; // one is null, one is not
            continue;         // both are null, consider equal
        }

        if (!(*lhs == *rhs)) {
            return false; // value mismatch
        }
    }

    return true;
}

void Pools::recvPayload(QList<payload::Relay *> *payload) {
    if (relaysEquals(*m_payload, *payload)) {
        return;
    }
    emit poolsUpdated();

    auto *old = m_payload;
    m_payload = payload;
    for (auto *relay : *old) {
        delete relay;
    }
    delete old;
    this->view();
}

void Pools::init() {
    m_payload = new QList<payload::Relay *>;
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

void insertPool(AccountController *ctrl, QTableWidget *table,
                const payload::Pool *pool, int index) {
    auto *id = new QTableWidgetItem(pool->id);
    id->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 0, id);

    auto *denomination = new QTableWidgetItem(
        toBitcoin(pool->denomination, false));
    denomination->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 1, denomination);

    auto *fees = new QTableWidgetItem(QString::number(pool->fees));
    fees->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 2, fees);

    auto *remains = new QTableWidgetItem(remainingTime(pool->timeout));
    remains->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 3, remains);

    auto *peers = new QTableWidgetItem(
        peersCount(pool->current_peers, pool->total_peers));
    peers->setTextAlignment(Qt::AlignCenter);
    table->setItem(index, 4, peers);

    auto poolId = pool->id;
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

void Pools::insertRelay(qontrol::Column *col, const payload::Relay *relay) {
    auto *collapsible = new qontrol::widgets::Collapsible(relay->url, col);

    m_collapsibles->push_back(collapsible);

    int rowCount = relay->pools.size() + 1;
    const int c_table_width = 6;
    auto *table = new QTableWidget(rowCount, c_table_width);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    table->setMinimumHeight((rowCount + 1) * 30);
    auto headers = QStringList{"Id",    "Denomination", "Fee", "Remaining time",
                               "Peers", "Action"};
    table->setHorizontalHeaderLabels(headers);
    int index = 0;
    for (auto *pool : relay->pools) {
        insertPool(m_controller, table, pool, index);
        index++;
    }

    // merge cells of the last row
    table->setSpan(index, 0, 6, c_table_width);

    auto *create = new QPushButton("Create pool");
    auto *controller = m_controller;
    connect(create, &QPushButton::clicked, controller, [relay, controller]() {
        controller->actionCreatePoolForRelay(relay->url);
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

    for (const auto *relay : *m_payload) {
        insertRelay(col, relay);
    }

    col->pushSpacer();

    auto *boxed = margin(col);

    auto *old = this->layout();
    delete old;

    this->setLayout(boxed->layout());
}

} // namespace screen
