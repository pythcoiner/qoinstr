#pragma once

#include "../payloads/Pools.h"
#include <Qontrol>
#include <qhash.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace screen {

class Pools : public Screen {
    Q_OBJECT
public:
    explicit Pools();

public slots:
    void recvPayload(QList<payload::Relay*> *payload);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

private:
    QList<payload::Relay*> *m_payload = nullptr;
    QWidget *m_main_widget = nullptr;
    QList<QTableWidget*> *m_tables = nullptr;

    void insertRelay(Column *col, const payload::Relay *relay);
};

} // namespace screen
