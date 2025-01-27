#include "Send.h"
#include "AppController.h"
#include "common.h"
#include <Qontrol>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

namespace screen {

auto Output::widget(Send *screen, int id) -> QWidget* {
    if (m_widget != nullptr) {
        return m_widget;
    }
    auto *addr = new QLabel("Address:");
    addr->setFixedWidth(LABEL_WIDTH);

    m_address = new QLineEdit;
    m_address->setFixedWidth(2 * INPUT_WIDTH);

    auto *delBtn = new QPushButton(" - ");

    auto *addrRow = (new qontrol::Row)
        ->push(addr)
        ->push(m_address)
        ->pushSpacer(10)
        ->push(delBtn)
        ->pushSpacer()
        ;

    auto *label = new QLabel("Label:");
    label->setFixedWidth(LABEL_WIDTH);

    m_label = new QLineEdit;
    m_label->setFixedWidth(2 * INPUT_WIDTH );

    auto *labelRow = (new qontrol::Row)
        ->push(label)
        ->push(m_label)
        ->pushSpacer()
        ;

    auto *amount = new QLabel("Amount:");
    amount->setFixedWidth(LABEL_WIDTH);

    m_amount = new QLineEdit;
    m_amount->setFixedWidth(120);

    auto *btc = new QLabel("BTC");
    btc->setFixedWidth(80);

    auto *fee = new QLabel("Fees:");
    fee->setFixedWidth(75);

    m_fees = new QLineEdit;
    m_fees->setFixedWidth(120);

    auto *sats = new QLabel("sats/vb");
    sats->setFixedWidth(120);

    auto *lastRow = (new qontrol::Row)
        ->push(amount)
        ->push(m_amount)
        ->pushSpacer(5)
        ->push(btc)
        ->push(fee)
        ->push(m_fees)
        ->pushSpacer(5)
        ->push(sats)
        ->pushSpacer()
        ;

    auto *col = (new qontrol::Column)
        ->pushSpacer(V_SPACER)
        ->push(addrRow)
        ->pushSpacer(V_SPACER)
        ->push(labelRow)
        ->pushSpacer( V_SPACER)
        ->push(lastRow)
        ->pushSpacer(2 * V_SPACER)
        ;

    QObject::connect(delBtn, &QPushButton::clicked, screen, [screen, id]() {
        screen->deleteOutput(id);
    });

    m_widget = col;
    return col;
}

auto Output::output() -> payload::Output {

}

Send::Send() {
    this->init();
    this->doConnect();
    this->view();
}

void Send::init() {
    m_column = (new qontrol::Column);
    this->addOutput();

    m_send_btn = new QPushButton("Send");
    m_add_btn = new QPushButton("+ Add");
    connect(m_add_btn, &QPushButton::clicked, this, &Send::addOutput);

}

void Send::addOutput() {
    auto *output = new Output;
    m_outputs.insert(m_output_id, output);
    m_column->push(output->widget(this, m_output_id));
    m_output_id++;
}

void Send::doConnect() {}

void Send::view() {
    auto *oldColumn = m_column;

    m_column = new qontrol::Column;
    
    auto keys = QList<int>();
    for (auto id : m_outputs.keys()) {
        keys.push_back(id);
    }
    std::sort(keys.begin(), keys.end());
    for (auto id : keys) {
        auto *output = m_outputs.value(id);
        m_column->push(output->widget(this, id));
    }
    delete oldColumn;

    auto *lastRow = (new qontrol::Row)
        ->push(m_send_btn)
        ->pushSpacer(400)
        ->push(m_add_btn)
        ->pushSpacer()
        ;

    auto *col = (new qontrol::Column)
        ->push(m_column)
        ->pushSpacer(10)
        ->push(lastRow)
        ->pushSpacer()
        ;

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(col);
    delete oldWidget;
    delete this->layout();
    this->setLayout(m_main_widget->layout());
}

void Send::deleteOutput(int id) {
    if (m_outputs.size() < 2) return;
    auto *output = m_outputs.take(id);
    delete output;
    this->view();
}

} // namespace screen
