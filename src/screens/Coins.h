#pragma once

#include "../payloads/Coins.h"
#include <Qontrol>
#include <cstdint>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace screen {

class Coins : public Screen {
    Q_OBJECT
public:
    explicit Coins();

public slots:
    void recvPayload(payload::Coins *payload);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    payload::Coins *m_payload = nullptr;
    QWidget *m_main_widget = nullptr;
    QTableWidget *m_table = nullptr;
    QList<QString /*outpoint*/> m_selected_coins;
    uint64_t m_amount_selected = 0;
};

} // namespace screen
