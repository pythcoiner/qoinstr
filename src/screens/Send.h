#pragma once

#include "payloads/Send.h"
#include <Qontrol>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

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

// payload::Output {
//     QString address;
//     uint64_t amount = 0;
//     QString label;
//     bool max = false;
// };

class Send : public Screen {
    Q_OBJECT
public:
    explicit Send();

public slots:
    void deleteOutput(int id);
    void addOutput();

protected:
    void init() override;
    void doConnect() override;
    void view() override;
private:
    int m_output_id = 0;
    QHash<int, Output*> m_outputs;
    Column *m_column = nullptr;
    QWidget *m_main_widget = nullptr;
    QPushButton *m_add_btn = nullptr;
    QPushButton *m_send_btn = nullptr;

    auto output() -> QWidget*;
};

} // namespace screen
