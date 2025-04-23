#include "Receive.h"
#include "AccountController.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include "common.h"
#include "payloads/Receive.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qpushbutton.h>

namespace screen {

Receive::Receive(AccountController *ctrl) {
    m_controller = ctrl;
    this->init();
    this->doConnect();
    this->view();
}

void Receive::init() {
}

void Receive::doConnect() {
    connect(m_controller, &AccountController::newAddress, this, &Receive::onNewAddress);
}

void Receive::onUnload() {
    for (auto *item : m_addresses) {
        delete item;
    }
    for (auto *widget : m_widgets) {
        delete widget;
    }
    m_widgets.clear();
    m_addresses.clear();
    m_label_inputs.clear();
}

void Receive::onNewAddress(payload::Address *addr) {
    m_addresses.push_back(addr);
    view();
}

auto Receive::addressWidget(const payload::Address *address) -> QWidget * {
    auto *addr = new QLabel("Address:");
    addr->setFixedWidth(LABEL_WIDTH);

    auto *addrStr = new QLabel(address->address);
    addrStr->setFixedWidth(3 * INPUT_WIDTH);

    // auto *qrBtn = new QPushButton("QR Code");
    // m_widgets->push_back(qrBtn);
    auto *copyBtn = new QPushButton("Copy");

    auto *label = new QLabel("Label:");
    label->setFixedWidth(LABEL_WIDTH);

    auto *labelInput = new QLineEdit;
    labelInput->setFixedWidth(3 * INPUT_WIDTH);
    m_label_inputs.insert(address->address, labelInput);

    // auto *saveBtn = new QPushButton("Save");

    auto *addrRow = (new qontrol::Row)
                        ->push(addr)
                        ->push(addrStr)
                        ->pushSpacer(10)
                        // ->push(qrBtn)
                        // ->pushSpacer(10)
                        ->push(copyBtn)
                        ->pushSpacer();

    // auto *labelRow = (new qontrol::Row)
    //     ->push(label)
    //     ->push(labelInput)
    //     ->pushSpacer(10)
    //     ->push(saveBtn)
    //     ->pushSpacer()
    //     ;

    auto *col = (new qontrol::Column)
                    ->pushSpacer(V_SPACER)
                    ->push(addrRow)
                    // ->pushSpacer(V_SPACER)
                    // ->push(labelRow)
                    ->pushSpacer(2 * V_SPACER);
    m_widgets.push_back(col);

    auto *controller = m_controller;
    // connect(qrBtn, &QPushButton::clicked, controller,
    //         [address, controller]() {
    //             controller->showAddressQr(address->address, address->index, address->change);
    //         });
    //
    connect(copyBtn, &QPushButton::clicked, controller,
            [address]() { QApplication::clipboard()->setText(address->address); });

    // connect(saveBtn, &QPushButton::clicked, controller,
    //         [this, address, controller]() {
    //             auto *label = m_label_inputs.value(address->address);
    //             if (label != nullptr) {
    //                 controller->saveAddressLabel(address->address, label->text());
    //             }
    //         });

    return col;
}

void Receive::view() {
    auto *col = new qontrol::Column;
    m_label_inputs.clear();

    for (const auto *addr : m_addresses) {
        col->push(addressWidget(addr));
    }

    if (m_btn_generate == nullptr) {
        m_btn_generate = new QPushButton("Generate new address");
        connect(m_btn_generate, &QPushButton::clicked, m_controller, &AccountController::actionCreateNewAddress);
    }
    auto *row = (new qontrol::Row)->push(m_btn_generate)->pushSpacer();

    col->push(row)->pushSpacer();

    auto *oldWidget = m_main_widget;
    m_main_widget = margin(col);
    delete oldWidget;
    auto *oldLayout = this->layout();
    delete oldLayout;
    this->setLayout(m_main_widget->layout());
}

} // namespace screen
