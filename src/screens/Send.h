#pragma once

#include "payloads/Send.h"
#include <Qontrol>
#include <cstdint>
#include <qabstractbutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AccountController;

namespace payload {
class Coin;
}

namespace screen {
class Send;

struct Coin {
    Coin() = default;
    Coin(const Coin &other);
    Coin(const payload::Coin &other);
    QString outpoint;
    QString label;
    uint64_t value = 0;

    auto operator==(const Coin &other) const -> bool;
};

class Input {
public:
    Input(Send *screen, int id);
    auto widget() -> QWidget *;
    void setDeletable(bool deletable);
    void setOutpoint(const QString &outpoint);
    void setLabel(const QString &label);
    void setAmount(uint64_t amount);
    auto coin() -> Coin;

private:
    QLineEdit *m_outpoint = nullptr;
    QLineEdit *m_label = nullptr;
    QLineEdit *m_amount = nullptr;
    QPushButton *m_delete = nullptr;
    QWidget *m_delete_spacer = nullptr;
    QWidget *m_widget = nullptr;
    uint64_t m_value = 0;
};

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

signals:

public slots:
    void outputSetMax(int id);
    void deleteInput(int id);
    void deleteOutput(int id);
    void addInput(const screen::Coin &coin);
    void addOutput();
    void clearInputs();
    void clearOutputs();
    void updateRadio();
    void setBroadcastable(bool broadcastable);
    void addCoins();
    void onCoinsSelected(const QList<screen::Coin> &coins);

protected:
    void init() override;
    void doConnect() override;
    void view() override;
    auto inputsView() -> QWidget *;
    auto outputsView() -> QWidget *;
    auto selectedCoins() -> QList<Coin>;

    auto isTransactionReady() -> bool {
        // TODO: call rust  tx "validator"
        //   - check parsing of all fields
        //   - chek that transaction hvae valis structure:
        //     - inputs != 0
        //     - sum(outputs) < sum(inputs)
        return false;
    }

    auto prepareTransaction() -> QString {
        // TODO: prepare PSBT on rust side
        // - add a change output if needed
        // - sanitize the transaction
        // - craft PSBT
        // - populate PSBT w/ signing infos
        // -> return PSBT string
        return QString();
    };

private:
    AccountController *m_controller = nullptr;
    int m_input_id = 0;
    int m_output_id = 0;
    QHash<int, Output *> m_outputs;
    QHash<int, Input *> m_inputs;
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
