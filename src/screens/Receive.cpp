#include "Receive.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include "common.h"
#include "payloads/Receive.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

namespace screen {

Receive::Receive() {
    this->init();
    this->doConnect();
    this->view();
}

void Receive::init() {
    m_addresses = payload::Address::dummyAddresses();
}

void Receive::doConnect() {
    // connect(m_btn, &QPushButton::clicked, AppController::get(), &AppController::onNext, UNIQUE);
}

auto Receive::addressWidget(const payload::Address *address) -> QWidget* {
    auto *addr = new QLabel("Address:");
    addr->setFixedWidth(LABEL_WIDTH);

    auto *addrStr = new QLabel(address->address);
    addrStr->setFixedWidth(3 * INPUT_WIDTH );

    auto *qrBtn = new QPushButton("QR Code");
    auto *copyBtn = new QPushButton("Copy");

    auto *label = new QLabel("Label:");
    label->setFixedWidth(LABEL_WIDTH);

    auto *labelInput = new QLineEdit;
    labelInput->setFixedWidth(3 * INPUT_WIDTH );
    m_label_inputs.insert(address->address, labelInput);

    auto *saveBtn = new QPushButton("Save");

    auto *addrRow = (new qontrol::Row)
        ->push(addr)
        ->push(addrStr)
        ->pushSpacer(10)
        ->push(qrBtn)
        ->pushSpacer(10)
        ->push(copyBtn)
        ->pushSpacer()
        ;

    auto *labelRow = (new qontrol::Row)
        ->push(label)
        ->push(labelInput)
        ->pushSpacer(10)
        ->push(saveBtn)
        ->pushSpacer()
        ;

    auto *col = (new qontrol::Column)
        ->pushSpacer(V_SPACER)
        ->push(addrRow)
        ->pushSpacer(V_SPACER)
        ->push(labelRow)
        ->pushSpacer(2 * V_SPACER)
        ;

    auto *controller = AppController::get();
    connect(qrBtn, &QPushButton::clicked, controller, 
            [address, controller]() {
                controller->showAddressQr(address->address, address->index, address->change);
            });

    connect(copyBtn, &QPushButton::clicked, controller, 
            [address, controller]() {controller->copyAddress(address->address);});

    connect(saveBtn, &QPushButton::clicked, controller, 
            [this, address, controller]() {
                auto *label = m_label_inputs.value(address->address);
                if (label != nullptr) {
                    controller->saveAddressLabel(address->address, label->text());
                }
            });

    return col;
}

void Receive::view() {
    auto *col = new qontrol::Column;
    m_label_inputs.clear();

    for (const auto *addr : *m_addresses) {
        col->push(addressWidget(addr));
    }

    auto *generateBtn = new QPushButton("Generate new address");
    auto *row = (new qontrol::Row)
        ->push(generateBtn)
        ->pushSpacer()
        ;

    col->push(row)
        ->pushSpacer()
        ;

    connect(generateBtn, &QPushButton::clicked, AppController::get(), &AppController::generateAddress);

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(col);
    delete oldWidget;
    this->setLayout(m_main_widget->layout());
}

} // namespace screen

