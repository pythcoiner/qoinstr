#include "Send.h"
#include "AccountController.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include "common.h"
#include "include/cpp_joinstr.h"
#include "screens/modals/SelectCoins.h"
#include <Qontrol>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcontainerfwd.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

namespace screen {

InputW::InputW(const RustCoin &coin, Send *screen, int id) {
    m_coin = coin;
    m_outpoint = new QLineEdit();
    m_outpoint->setFixedWidth(200);
    m_outpoint->setEnabled(false);
    m_outpoint->setPlaceholderText("xxxx....yyyyy:z");

    m_delete = new QPushButton();
    QIcon closeIcon = m_delete->style()->standardIcon(
        QStyle::SP_DialogCloseButton);
    m_delete->setIcon(closeIcon);
    m_delete->setFixedWidth(m_outpoint->minimumSizeHint().height());
    m_delete->setFixedHeight(m_outpoint->minimumSizeHint().height());

    m_delete_spacer = new QWidget;
    m_delete_spacer->setFixedWidth(V_SPACER);

    m_amount = new QLineEdit;
    m_amount->setFixedWidth(95);
    m_amount->setEnabled(false);
    m_amount->setPlaceholderText("0.002 BTC");

    m_label = new QLineEdit;
    m_label->setFixedWidth(2 * INPUT_WIDTH);
    m_label->setPlaceholderText("Label");

    auto *outpointRow = (new qontrol::Row)
                            ->push(m_delete)
                            ->push(m_delete_spacer)
                            ->push(m_outpoint)
                            ->pushSpacer(H_SPACER)
                            ->push(m_amount)
                            ->pushSpacer();

    auto *labelRow = (new qontrol::Row)->push(m_label)->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->pushSpacer(V_SPACER)
                    ->push(outpointRow)
                    ->pushSpacer(V_SPACER)
                    ->push(labelRow)
                    ->pushSpacer(2 * V_SPACER);

    QObject::connect(m_delete, &QPushButton::clicked, screen,
                     [screen, id]() { screen->deleteInput(id); });

    m_widget = col;
}

auto InputW::widget() -> QWidget * {
    return m_widget;
}

void InputW::setDeletable(bool deletable) {
    m_delete->setVisible(deletable);
    m_delete_spacer->setVisible(deletable);
}

OutputW::OutputW(Send *screen, int id) {
    m_address = new QLineEdit;
    m_address->setFixedWidth(300);
    m_address->setPlaceholderText("Address: bc1.....");
    QObject::connect(m_address, &QLineEdit::editingFinished, screen,
                     &Send::process);

    m_delete = new QPushButton();
    QIcon closeIcon = m_delete->style()->standardIcon(
        QStyle::SP_DialogCloseButton);
    m_delete->setIcon(closeIcon);
    m_delete->setFixedWidth(m_address->minimumSizeHint().height());
    m_delete->setFixedHeight(m_address->minimumSizeHint().height());

    m_delete_spacer = new QWidget;
    m_delete_spacer->setFixedWidth(V_SPACER);

    m_amount = new QLineEdit;
    m_amount->setFixedWidth(95);
    m_amount->setPlaceholderText("0.002 BTC");
    QObject::connect(m_amount, &QLineEdit::editingFinished, screen,
                     &Send::process);

    m_label = new QLineEdit;
    m_label->setFixedWidth(2 * INPUT_WIDTH);
    m_label->setPlaceholderText("Label");

    m_max = new QCheckBox;
    m_max->setStyleSheet(R"(
      QCheckBox::indicator {
        width: 28px;
        height: 28px;
      }
    )");
    QObject::connect(m_max, &QCheckBox::toggled, screen, &Send::process,
                     qontrol::UNIQUE);

    m_max_label = new QLabel("MAX");
    QFont f = m_max_label->font();
    f.setPointSize(f.pointSize() + 4);
    m_max_label->setFont(f);

    auto *addrRow = (new qontrol::Row)
                        ->push(m_delete)
                        ->push(m_delete_spacer)
                        ->push(m_address)
                        ->pushSpacer(H_SPACER)
                        ->push(m_amount)
                        ->pushSpacer(H_SPACER)
                        ->push(m_max)
                        ->pushSpacer(H_SPACER)
                        ->push(m_max_label)
                        ->pushSpacer();

    auto *labelRow = (new qontrol::Row)->push(m_label)->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->pushSpacer(V_SPACER)
                    ->push(addrRow)
                    ->pushSpacer(V_SPACER)
                    ->push(labelRow)
                    ->pushSpacer(2 * V_SPACER);

    QObject::connect(m_delete, &QPushButton::clicked, screen,
                     [screen, id]() { screen->deleteOutput(id); });
    QObject::connect(m_max, &QCheckBox::checkStateChanged, screen,
                     [screen, id]() { screen->outputSetMax(id); });

    m_widget = col;
}

auto OutputW::widget() -> QWidget * {
    return m_widget;
}

void OutputW::setDeletable(bool deletable) {
    m_delete->setVisible(deletable);
    m_delete_spacer->setVisible(deletable);
}

void OutputW::enableMax(bool max) {
    m_max->setChecked(false);
    m_max->setVisible(max);
    m_max_label->setVisible(max);
}

auto OutputW::isMax() -> bool {
    return m_max->isChecked();
}

RadioElement::RadioElement(Send *parent, const QString &label) {
    m_button = new QRadioButton(parent);
    QObject::connect(m_button, &QAbstractButton::toggled, parent,
                     &Send::process);
    m_value = new QLineEdit;
    m_value->setFixedWidth(100);
    QObject::connect(m_value, &QLineEdit::editingFinished, parent,
                     &Send::process);
    m_label = new QLabel(label);

    auto *row = (new qontrol::Row)
                    ->push(m_button)
                    ->pushSpacer(V_SPACER)
                    ->push(m_value)
                    ->pushSpacer(V_SPACER)
                    ->push(m_label)
                    ->pushSpacer(4 * V_SPACER);
    m_widget = row;
}

auto RadioElement::button() -> QAbstractButton * {
    return m_button;
}

void RadioElement::update() {
    m_value->setEnabled(m_button->isChecked());
}

auto RadioElement::widget() -> qontrol::Row * {
    return m_widget;
}

void Send::updateRadio() {
    m_fee_sats_vb->update();
    m_fee_sats->update();
    m_fee_blocks->update();
    process();
}

Send::Send(AccountController *ctrl) {
    m_controller = ctrl;
    this->init();
    this->doConnect();
    this->addOutput();
    this->view();
    this->updateRadio();
    this->setBroadcastable(false);
}

void Send::init() {
    m_outputs_column = (new qontrol::Column);

    m_inputs_column = (new qontrol::Column);

    m_add_input_btn = new QPushButton("+ Add an Input");
    connect(m_add_input_btn, &QPushButton::clicked, this, &Send::addCoins);

    m_clear_inputs_btn = new QPushButton("Clear");
    connect(m_clear_inputs_btn, &QPushButton::clicked, this,
            &Send::clearInputs);

    m_auto_inputs_btn = new QPushButton("Auto");

    m_add_output_btn = new QPushButton("+ Add an Output");
    connect(m_add_output_btn, &QPushButton::clicked, this, &Send::addOutput);

    m_sign_btn = new QPushButton("Sign");
    m_sign_btn->setEnabled(false);
    m_broadcast_button = new QPushButton("Broadcast");
    m_clear_outputs_btn = new QPushButton("Clear");
    connect(m_clear_outputs_btn, &QPushButton::clicked, this,
            &Send::clearOutputs, qontrol::UNIQUE);
    m_export_btn = new QPushButton("Export");
    m_export_btn->setEnabled(false);

    m_fee_sats = new RadioElement(this, "sats");
    m_fee_sats_vb = new RadioElement(this, "sats/vb");
    m_fee_blocks = new RadioElement(this, "blocks");
    m_fee_blocks->setEnabled(false);
    m_fee_group = new QButtonGroup;
    m_fee_group->addButton(m_fee_sats->button());
    m_fee_group->addButton(m_fee_sats_vb->button());
    m_fee_group->addButton(m_fee_blocks->button());

    m_warning_label = new QLabel();
    m_warning_label->setVisible(false);

    m_fee_sats_vb->button()->setChecked(true);
}

void Send::doConnect() {
}

void Send::view() {
    auto *oldInputs = m_inputs_frame;
    auto *oldOutputs = m_outputs_frame;
    m_inputs_frame = frame(inputsView());
    m_outputs_frame = frame(outputsView());
    delete oldInputs;
    delete oldOutputs;

    auto *row = (new qontrol::Row)
                    ->push(m_inputs_frame)
                    ->pushSpacer(20)
                    ->push(m_outputs_frame);

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(row, 10);
    delete oldWidget;
    delete this->layout();
    this->setLayout(m_main_widget->layout());
}

auto Send::inputsView() -> QWidget * {
    auto *oldColumn = m_inputs_column;

    m_inputs_column = new qontrol::Column;

    auto keys = QList<int>();
    for (auto id : m_inputs.keys()) {
        keys.push_back(id);
    }
    std::ranges::sort(keys);
    for (auto id : keys) {
        auto *input = m_inputs.value(id);
        m_inputs_column->push(input->widget());
    }
    delete oldColumn;

    auto *addInputRow = (new qontrol::Row)
                            ->pushSpacer()
                            ->push(m_add_input_btn)
                            ->pushSpacer();

    auto *lastRow = (new qontrol::Row)
                        ->pushSpacer()
                        ->push(m_clear_inputs_btn)
                        ->pushSpacer()
                        ->push(m_auto_inputs_btn)
                        ->pushSpacer();

    auto *title = new QLabel("Inputs");
    auto font = title->font();
    font.setPointSize(15);
    title->setFont(font);

    auto *titleRow = (new qontrol::Row)
                         ->pushSpacer(15)
                         ->push(title)
                         ->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->push(titleRow)
                    ->pushSpacer(20)
                    ->push(m_inputs_column)
                    ->push(addInputRow)
                    ->pushSpacer(20)
                    ->push(lastRow)
                    ->pushSpacer();

    return col;
}

auto Send::outputsView() -> QWidget * {
    auto *oldColumn = m_outputs_column;

    m_outputs_column = new qontrol::Column;

    auto keys = QList<int>();
    for (auto id : m_outputs.keys()) {
        keys.push_back(id);
    }
    std::ranges::sort(keys);
    for (auto id : keys) {
        auto *output = m_outputs.value(id);
        m_outputs_column->push(output->widget());
    }
    delete oldColumn;

    auto *addOutputRow = (new qontrol::Row)
                             ->pushSpacer()
                             ->push(m_add_output_btn)
                             ->pushSpacer();

    auto *lastRow = (new qontrol::Row)
                        ->pushSpacer()
                        ->push(m_clear_outputs_btn)
                        ->pushSpacer()
                        ->push(m_export_btn)
                        ->pushSpacer()
                        ->push(m_sign_btn)
                        ->push(m_broadcast_button)
                        ->pushSpacer();

    auto *feeRow = (new qontrol::Row)
                       ->merge(m_fee_sats_vb->widget())
                       ->merge(m_fee_sats->widget())
                       ->merge(m_fee_blocks->widget())
                       ->pushSpacer();

    auto *warningRow = (new qontrol::Row)->push(m_warning_label)->pushSpacer();

    auto *title = new QLabel("Outputs");
    auto font = title->font();
    font.setPointSize(15);
    title->setFont(font);

    auto *titleRow = (new qontrol::Row)
                         ->pushSpacer(15)
                         ->push(title)
                         ->pushSpacer();

    auto *col = (new qontrol::Column)
                    ->push(titleRow)
                    ->pushSpacer(20)
                    ->push(m_outputs_column)
                    ->push(addOutputRow)
                    ->pushSpacer(20)
                    ->push(feeRow)
                    ->pushSpacer(20)
                    ->push(warningRow)
                    ->pushSpacer(20)
                    ->push(lastRow)
                    ->pushSpacer();

    return col;
}

void Send::addOutput() {
    auto *output = new OutputW(this, m_output_id);
    if (m_outputs.size() == 0) {
        output->setDeletable(false);
    } else {
        for (auto &out : m_outputs) {
            out->setDeletable(true);
        }
    }
    for (auto *outp : m_outputs) {
        if (outp->isMax()) {
            output->enableMax(false);
            break;
        }
    }
    m_outputs.insert(m_output_id, output);
    m_outputs_column->push(output->widget());
    m_output_id++;
    process();
    view();
}

void Send::deleteInput(int id) {
    auto *input = m_inputs.take(id);
    delete input->widget();
    delete input;
    process();
}

void Send::deleteOutput(int id) {
    auto *output = m_outputs.take(id);
    if (output->isMax()) {
        for (auto *outp : m_outputs) {
            outp->enableMax(true);
        }
    }
    delete output->widget();
    delete output;
    if (m_outputs.size() == 1) {
        auto *outp = m_outputs.value(m_outputs.keys().first());
        outp->setDeletable(false);
        if (!outp->isMax()) {
            outp->enableMax(true);
        }
    }
    process();
    view();
}

void Send::outputSetMax(int id) {
    if (m_outputs.value(id)->isMax()) {
        for (auto &key : m_outputs.keys()) {
            if (key != id) {
                m_outputs.value(key)->enableMax(false);
            }
        }
    } else {
        for (auto &key : m_outputs.keys()) {
            m_outputs.value(key)->enableMax(true);
        }
    }
    process();
}

void Send::setBroadcastable(bool broadcastable) {
    m_broadcastable = broadcastable;
    if (broadcastable) {
        m_sign_btn->setVisible(false);
        m_broadcast_button->setVisible(true);
    } else {
        m_sign_btn->setVisible(true);
        m_broadcast_button->setVisible(false);
    }
}

void Send::clearInputs() {
    for (auto *inp : m_inputs) {
        delete inp;
    }
    m_inputs.clear();
    process();
    view();
}

void Send::clearOutputs() {
    for (auto *outp : m_outputs) {
        delete outp;
    }
    m_outputs.clear();
    addOutput();
    process();
    view();
}

void Send::addCoins() {
    auto optCoins = dynamic_cast<screen::Coins *>(m_controller->coins())
                        ->getCoins();
    QList<RustCoin> coins;
    auto selected = selectedCoins();
    if (optCoins.has_value() && !optCoins.value().isEmpty()) {
        for (const auto &c : optCoins.value()) {
            auto mc = RustCoin(c);
            if (!selected.contains(mc)) {
                coins.append(mc);
            }
        }
        auto *modal = new modal::SelectCoins(coins);
        connect(modal, &modal::SelectCoins::coinsSelected, this,
                &Send::onCoinsSelected, qontrol::UNIQUE);
        AppController::execModal(modal);

    } else {
        auto *modal = new qontrol::Modal("No coins!",
                                         "There is no coins to select!");
        AppController::execModal(modal);
    }
}

void InputW::setOutpoint(const QString &outpoint) {
    m_outpoint->setText(outpoint);
}

void InputW::setLabel(const QString &label) {
    m_label->setText(label);
}

void InputW::setAmount(uint64_t amount) {
    m_coin.value = amount;
    double btcAmount = amount;
    btcAmount = btcAmount / SATS;
    m_amount->setText(QString::number(btcAmount) + " BTC");
}

void Send::addInput(const RustCoin &coin) {
    auto *input = new InputW(coin, this, m_input_id);
    auto op = coin.outpoint;
    input->setOutpoint(QString(op.c_str()));
    auto label = coin.label;
    input->setLabel(QString(label.c_str()));
    input->setAmount(coin.value);
    for (auto &out : m_inputs) {
        out->setDeletable(true);
    }
    m_inputs.insert(m_input_id, input);
    m_inputs_column->push(input->widget());
    m_input_id++;
    process();
    view();
}

void Send::onCoinsSelected(const QList<RustCoin> &coins) {
    for (const auto &coin : coins) {
        addInput(coin);
    }
}

auto InputW::coin() -> RustCoin {
    return m_coin;
}

auto Send::selectedCoins() -> QList<RustCoin> {
    auto output = QList<RustCoin>();
    for (auto *c : m_inputs) {
        output.append(c->coin());
    }
    return output;
}

auto RadioElement::text() -> QString {
    return m_value->text();
}

auto InputW::label() -> QString {
    return m_label->text();
}

auto OutputW::address() -> QString {
    return m_address->text();
}

auto OutputW::amount() -> std::optional<uint64_t> {
    auto amountStr = m_amount->text();
    bool ok = false;
    auto amountBtc = amountStr.toDouble(&ok);
    if (!ok) {
        qDebug() << "Output::amount() m_amount value is not a valid double";
        return std::nullopt;
    }
    return static_cast<uint64_t>(amountBtc * SATS);
}

auto OutputW::label() -> QString {
    return m_label->text();
}

auto Send::txTemplate() -> std::optional<TransactionTemplate> {
    auto txTemplate = TransactionTemplate();
    bool ok = false;

    // fees
    if (m_fee_sats->checked()) {
        auto fee = m_fee_sats->text().toInt(&ok);
        if (!ok) {
            qDebug() << "Send::txTemplate() m_fee_sats is not a valid int";
            return std::nullopt;
        }
        txTemplate.fee_sats = fee;

    } else if (m_fee_sats_vb->checked()) {
        auto fee = m_fee_sats_vb->text().toDouble(&ok);
        if (!ok) {
            qDebug()
                << "Send::txTemplate() m_fee_sats_vb is not a valid double";
            return std::nullopt;
        }
        txTemplate.fee_sats_vb = fee;
    }

    // TODO: handle block target fees

    // inputs
    for (auto *inp : m_inputs) {
        auto coin = inp->coin();
        coin.label = rust::String(inp->label().toStdString());
        txTemplate.inputs.push_back(coin);
    }

    // outputs
    for (auto *out : m_outputs) {
        auto output = Output();
        output.address = rust::String(out->address().toStdString());
        auto amount = out->amount();
        if (!amount.has_value()) {
            return std::nullopt;
        }
        output.amount = amount.value();
        output.label = rust::String(out->label().toStdString());
        output.max = out->isMax();
        txTemplate.outputs.push_back(output);
    }

    return txTemplate;
}

auto Send::prepareTransaction() -> std::optional<QString> {
    if (m_controller == nullptr) {
        return std::nullopt;
    }
    auto txTemp = txTemplate();
    if (txTemp.has_value()) {
        return m_controller->cmdPrepareTx(txTemp.value());
    }
    return std::nullopt;

    return QString();
};

void RadioElement::setEnabled(bool enabled) {
    m_button->setEnabled(enabled);
    m_value->setEnabled(enabled);
}

auto RadioElement::checked() -> bool {
    return m_button->isChecked();
}

void Send::setSpendable(bool spendable) {
    m_sign_btn->setEnabled(spendable);
    m_export_btn->setEnabled(spendable);
}

void Send::process() {
    qDebug() << "Send::progress()";
    auto txTemp = txTemplate();
    if (!txTemp.has_value()) {
        setSpendable(false);
        return;
    }
    auto simu = m_controller->simulateTx(txTemp.value());
    if (simu.error.empty()) {
        if (simu.spendable) {
            setSpendable(true);
            m_warning_label->setVisible(false);
        } else if (simu.has_change) {
            m_warning_label->setText(
                "A change output will be automatically added.");
            m_warning_label->setVisible(true);
            setSpendable(false);
        }
    } else {
        setSpendable(false);
        m_warning_label->setText(simu.error.c_str());
        m_warning_label->setVisible(true);
    }
}
} // namespace screen
