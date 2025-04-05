#pragma once

#include "payloads/Send.h"
#include <Qontrol>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {
class Send;
class Output {
public:
    Output() = default;
    auto widget(Send *screen, int id) -> QWidget*;
    auto output() -> payload::Output;

private:
    QLineEdit *m_address = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_amount = nullptr;
    QLineEdit *m_fees = nullptr;
    QPushButton *m_remove = nullptr;
    QWidget *m_widget = nullptr;
};

class Send : public qontrol::Screen {
    Q_OBJECT
public:
    Send(AccountController *ctrl);

public slots:
    void deleteOutput(int id);
    void addOutput();

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    AccountController *m_controller = nullptr;
    int m_output_id = 0;
    QHash<int, Output*> m_outputs;
    qontrol::Column *m_column = nullptr;
    QWidget *m_main_widget = nullptr;
    QPushButton *m_add_btn = nullptr;
    QPushButton *m_send_btn = nullptr;

    auto output() -> QWidget*;
};

} // namespace screen
