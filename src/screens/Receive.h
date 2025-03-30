#pragma once

#include "include/cpp_joinstr.h"
#include "payloads/Receive.h"
#include <QHash>
#include <Qontrol>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace screen {

class Receive : public qontrol::Screen {
    Q_OBJECT
public:
    explicit Receive();

    void onUnload() override;
    void onNewAddress(payload::Address* addr);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    QList<payload::Address*> m_addresses;
    QList<QWidget*> m_widgets;
    QWidget *m_main_widget = nullptr;
    QPushButton *m_btn_generate = nullptr;
    QHash<QString, QLineEdit*> m_label_inputs;

    auto addressWidget(const payload::Address *address) -> QWidget*;
};

} // namespace screen
