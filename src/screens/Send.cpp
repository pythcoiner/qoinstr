#include "Send.h"
#include "AccountController.h"
#include "AppController.h"
#include "Row.h"
#include "common.h"
#include <Qontrol>
#include <algorithm>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

namespace screen {

Output::Output(Send *screen, int id) {
    m_address = new QLineEdit;
    m_address->setFixedWidth(300);
    m_address->setPlaceholderText("Address: bc1.....");

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

auto Output::widget() -> QWidget * {
    return m_widget;
}

void Output::setDeletable(bool deletable) {
    m_delete->setVisible(deletable);
    m_delete_spacer->setVisible(deletable);
}

void Output::enableMax(bool max) {
    m_max->setChecked(false);
    m_max->setVisible(max);
    m_max_label->setVisible(max);
}

auto Output::isMax() -> bool {
    return m_max->isChecked();
}

RadioElement::RadioElement(Send *parent, const QString &label) {
    m_button = new QRadioButton(parent);
    m_value = new QLineEdit;
    m_value->setFixedWidth(100);
    m_label = new QLabel(label);
    QObject::connect(m_button, &QAbstractButton::toggled, parent,
                     [parent] { parent->updateRadio(); });

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
}

Send::Send(AccountController *ctrl) {
    m_controller = ctrl;
    this->init();
    this->doConnect();
    this->view();
    this->updateRadio();
    this->setBroadcastable(false);
}

void Send::init() {
    m_column = (new qontrol::Column);
    this->addOutput();

    m_add_btn = new QPushButton("+ Add an Output");
    connect(m_add_btn, &QPushButton::clicked, this, &Send::addOutput);

    m_sign_btn = new QPushButton("Sign");
    m_broadcast_button = new QPushButton("Broadcast");
    m_clear_btn = new QPushButton("Clear");
    m_export_btn = new QPushButton("Export");

    m_fee_sats = new RadioElement(this, "sats");
    m_fee_sats_vb = new RadioElement(this, "sats/vb");
    m_fee_blocks = new RadioElement(this, "blocks");
    m_fee_group = new QButtonGroup;
    m_fee_group->addButton(m_fee_sats->button());
    m_fee_group->addButton(m_fee_sats_vb->button());
    m_fee_group->addButton(m_fee_blocks->button());

    m_fee_sats_vb->button()->setChecked(true);
}

void Send::doConnect() {
}

void Send::view() {
    auto *oldColumn = m_column;

    m_column = new qontrol::Column;

    auto keys = QList<int>();
    for (auto id : m_outputs.keys()) {
        keys.push_back(id);
    }
    std::ranges::sort(keys);
    for (auto id : keys) {
        auto *output = m_outputs.value(id);
        m_column->push(output->widget());
    }
    delete oldColumn;

    auto *addOutputRow = (new qontrol::Row)
                             ->pushSpacer()
                             ->push(m_add_btn)
                             ->pushSpacer();

    auto *lastRow = (new qontrol::Row)
                        ->pushSpacer()
                        ->push(m_clear_btn)
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

    auto *col = (new qontrol::Column)
                    ->push(m_column)
                    ->push(addOutputRow)
                    ->pushSpacer(20)
                    ->push(feeRow)
                    ->pushSpacer(20)
                    ->push(lastRow)
                    ->pushSpacer();

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(col);
    delete oldWidget;
    delete this->layout();
    this->setLayout(m_main_widget->layout());
}

void Send::addOutput() {
    auto *output = new Output(this, m_output_id);
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
    m_column->push(output->widget());
    m_output_id++;
}

void Send::deleteOutput(int id) {
    auto *output = m_outputs.take(id);
    if (output->isMax()) {
        for (auto *outp : m_outputs) {
            outp->enableMax(true);
        }
    }
    delete output;
    if (m_outputs.size() == 1) {
        auto *outp = m_outputs.value(m_outputs.keys().first());
        outp->setDeletable(false);
        if (!outp->isMax()) {
            outp->enableMax(true);
        }
    }
    this->view();
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

} // namespace screen
