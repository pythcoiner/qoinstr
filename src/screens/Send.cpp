#include "Send.h"
#include "AccountController.h"
#include "AppController.h"
#include "common.h"
#include <Qontrol>
#include <algorithm>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

namespace screen {

Output::Output(Send *screen, int id) {
    m_address = new QLineEdit;
    m_address->setFixedWidth(2 * INPUT_WIDTH);
    m_address->setPlaceholderText("Address: bc1.....");

    m_delete = new QPushButton();
    QIcon closeIcon = m_delete->style()->standardIcon(QStyle::SP_DialogCloseButton);
    m_delete->setIcon(closeIcon);
    m_delete->setFixedWidth(25);
    m_delete->setFixedHeight(25);

    m_delete_spacer = new QWidget;
    m_delete_spacer->setFixedWidth(V_SPACER);

    m_amount = new QLineEdit;
    m_amount->setFixedWidth(120);
    m_amount->setPlaceholderText("0.002 BTC");

    m_label = new QLineEdit;
    m_label->setFixedWidth(2 * INPUT_WIDTH );
    m_label->setPlaceholderText("Label");

    m_max = new QCheckBox;

    m_max_label = new QLabel("MAX");
    m_max_label->setFixedWidth(120);

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
        ->pushSpacer()
        ;

    auto *labelRow = (new qontrol::Row)
        ->push(m_label)
        ->pushSpacer()
        ;

    auto *col = (new qontrol::Column)
        ->pushSpacer(V_SPACER)
        ->push(addrRow)
        ->pushSpacer(V_SPACER)
        ->push(labelRow)
        ->pushSpacer(2 * V_SPACER)
        ;

    QObject::connect(m_delete, &QPushButton::clicked, screen, [screen, id]() {
        screen->deleteOutput(id);
    });
    QObject::connect(m_max, &QCheckBox::checkStateChanged, screen, [screen, id]() {
        screen->outputSetMax(id);
    });

    m_widget = col;

}

auto Output::widget() -> QWidget* {
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

Send::Send(AccountController *ctrl) {
    m_controller = ctrl;
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

void Send::doConnect() {}

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

void Send::addOutput() {
    auto *output = new Output(this, m_output_id);
    if (m_outputs.size() == 0) {
        output->setDeletable(false);
    } else {
        for (auto &out : m_outputs) {
            out->setDeletable(true);
        }
    }
    m_outputs.insert(m_output_id, output);
    m_column->push(output->widget());
    m_output_id++;
}

void Send::deleteOutput(int id) {
    auto *output = m_outputs.take(id);
    delete output;
    if (m_outputs.size() == 1) {
        m_outputs.value(m_outputs.keys().first())->setDeletable(false);
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

} // namespace screen
