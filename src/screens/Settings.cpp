#include "Settings.h"
#include "AppController.h"
#include "Row.h"
#include <qlabel.h>
#include <qpushbutton.h>

namespace screen {
    Settings::Settings() {
        this->init();
        this->doConnect();
        this->view();
    }

    void Settings::init() {
        m_btn = new QPushButton("Settings", this);
        m_row = (new qontrol::Row(this))
            ->pushSpacer()
            ->push(m_btn)
            ->pushSpacer()
            ;
    }

    void Settings::doConnect() {
        connect(m_btn, &QPushButton::clicked, AppController::get(), &AppController::listpools, qontrol::UNIQUE);
    }

    void Settings::view() {
        auto *mainLayout = (new qontrol::Column(this))
            ->pushSpacer()
            ->push(m_row)
            ->pushSpacer()
            ;

        this->setLayout(mainLayout->layout());
    }
}

