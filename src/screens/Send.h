#pragma once

#include "payloads/Send.h"
#include <Qontrol>
#include <qabstractbutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace screen {
class Send;

class Output {
public:
    Output(Send *screen, int id);
    auto widget() -> QWidget *;
    void setDeletable(bool deletable);
    void enableMax(bool max);
    auto isMax() -> bool;

private:
    QLineEdit *m_address = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_amount = nullptr;
    QPushButton *m_delete = nullptr;
    QWidget *m_delete_spacer = nullptr;
    QCheckBox *m_max = nullptr;
    QLabel *m_max_label = nullptr;
    QWidget *m_widget = nullptr;
};

class RadioElement {
public:
    RadioElement(Send *parent, const QString &label);
    auto widget() -> qontrol::Row *;
    void update();
    auto button() -> QAbstractButton *;

private:
    QRadioButton *m_button = nullptr;
    QLineEdit *m_value = nullptr;
    QLabel *m_label = nullptr;
    qontrol::Row *m_widget;
};

class Send : public qontrol::Screen {
    Q_OBJECT
public:
    Send(AccountController *ctrl);

public slots:
    void outputSetMax(int id);
    void deleteOutput(int id);
    void addOutput();
    void updateRadio();
    void setBroadcastable(bool broadcastable);

protected:
    void init() override;
    void doConnect() override;
    void view() override;

private:
    AccountController *m_controller = nullptr;
    int m_output_id = 0;
    QHash<int, Output *> m_outputs;
    qontrol::Column *m_column = nullptr;
    QWidget *m_main_widget = nullptr;

    RadioElement *m_fee_sats_vb = nullptr;
    RadioElement *m_fee_sats = nullptr;
    RadioElement *m_fee_blocks = nullptr;
    QButtonGroup *m_fee_group = nullptr;

    QPushButton *m_add_btn = nullptr;

    QPushButton *m_export_btn = nullptr;
    QPushButton *m_clear_btn = nullptr;
    QPushButton *m_sign_btn = nullptr;
    QPushButton *m_broadcast_button = nullptr;

    bool m_broadcastable = false;

    auto output() -> QWidget *;
};

} // namespace screen
