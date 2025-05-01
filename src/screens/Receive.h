#pragma once

#include "include/cpp_joinstr.h"
#include <QHash>
#include <Qontrol>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {

class Receive : public qontrol::Screen {
    Q_OBJECT
public:
    Receive(AccountController *ctrl);

    void onUnload() override;
    void onNewAddress(const RustAddress &addr);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

private:
    AccountController *m_controller;
    QList<RustAddress> m_addresses;
    QList<QWidget *> m_widgets;
    QWidget *m_main_widget = nullptr;
    QPushButton *m_btn_generate = nullptr;
    QHash<QString, QLineEdit *> m_label_inputs;

    auto addressWidget(const RustAddress &address) -> QWidget *;
};

} // namespace screen
