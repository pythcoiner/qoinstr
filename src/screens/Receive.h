#pragma once

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

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    QList<payload::Address*> *m_addresses = nullptr;
    QWidget *m_main_widget = nullptr;
    QHash<QString, QLineEdit*> m_label_inputs;

    auto addressWidget(const payload::Address *address) -> QWidget*;
};

} // namespace screen
