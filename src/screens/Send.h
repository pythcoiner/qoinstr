#pragma once

#include "include/cpp_joinstr.h"
#include <Qontrol>
#include <cstdint>
#include <optional>
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

class InputW {
public:
    InputW(const RustCoin &coin, Send *screen, int id);
    auto widget() -> QWidget *;
    void setDeletable(bool deletable);
    void setOutpoint(const QString &outpoint);
    void setLabel(const QString &label);
    void setAmount(uint64_t amount);
    auto coin() -> RustCoin;
    auto label() -> QString;

private:
    QLineEdit *m_outpoint = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_amount = nullptr;
    QPushButton *m_delete = nullptr;
    QWidget *m_delete_spacer = nullptr;
    QWidget *m_widget = nullptr;
    RustCoin m_coin;
};

class OutputW {
public:
    OutputW(Send *screen, int id);
    auto widget() -> QWidget *;
    void setDeletable(bool deletable);
    void enableMax(bool max);
    auto isMax() -> bool;
    auto address() -> QString;
    auto amount() -> std::optional<uint64_t>;
    auto label() -> QString;

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
    auto text() -> QString;

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

signals:

public slots:
    void outputSetMax(int id);
    void deleteInput(int id);
    void deleteOutput(int id);
    void addInput(const RustCoin &coin);
    void addOutput();
    void clearInputs();
    void clearOutputs();
    void updateRadio();
    void setBroadcastable(bool broadcastable);
    void addCoins();
    void onCoinsSelected(const QList<RustCoin> &coins);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
    auto inputsView() -> QWidget *;
    auto outputsView() -> QWidget *;
    auto selectedCoins() -> QList<RustCoin>;
    auto txTemplate() -> std::optional<TransactionTemplate>;

    auto isTransactionReady() -> bool {
        // TODO: call rust  tx "validator"
        //   - check parsing of all fields
        //   - chek that transaction hvae valis structure:
        //     - inputs != 0
        //     - sum(outputs) < sum(inputs)
        return false;
    }

    auto prepareTransaction() -> std::optional<QString>;

private:
    AccountController *m_controller = nullptr;
    int m_input_id = 0;
    int m_output_id = 0;
    QHash<int, OutputW *> m_outputs;
    QHash<int, InputW *> m_inputs;
    qontrol::Column *m_outputs_column = nullptr;
    qontrol::Column *m_inputs_column = nullptr;

    QWidget *m_main_widget = nullptr;
    QWidget *m_inputs_frame = nullptr;
    QWidget *m_outputs_frame = nullptr;

    RadioElement *m_fee_sats_vb = nullptr;
    RadioElement *m_fee_sats = nullptr;
    RadioElement *m_fee_blocks = nullptr;
    QButtonGroup *m_fee_group = nullptr;

    QPushButton *m_add_input_btn = nullptr;

    QPushButton *m_clear_inputs_btn = nullptr;
    QPushButton *m_auto_inputs_btn = nullptr;

    QPushButton *m_add_output_btn = nullptr;

    QPushButton *m_export_btn = nullptr;
    QPushButton *m_clear_outputs_btn = nullptr;
    QPushButton *m_sign_btn = nullptr;
    QPushButton *m_broadcast_button = nullptr;

    bool m_broadcastable = false;

    auto output() -> QWidget *;
};

} // namespace screen
