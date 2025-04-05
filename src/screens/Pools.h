#pragma once

#include "../payloads/Pools.h"
#include "AccountController.h"
#include <Qontrol>
#include <qhash.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {

class Pools : public qontrol::Screen {
    Q_OBJECT
public:
    Pools(AccountController *ctrl);
signals:
    void poolsUpdated();

public slots:
    void recvPayload(QList<payload::Relay*> *payload);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

private:
    AccountController *m_controller = nullptr;
    QList<payload::Relay*> *m_payload = nullptr;
    QWidget *m_main_widget = nullptr;
    QList<QTableWidget*> *m_tables = nullptr;
    QList<qontrol::widgets::Collapsible*> *m_collapsibles = nullptr;

    void insertRelay(qontrol::Column *col, const payload::Relay *relay);
};

} // namespace screen
