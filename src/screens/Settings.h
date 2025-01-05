#pragma once

#include <Qontrol>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace screen {
    class Settings : public Screen {
        Q_OBJECT
    public:
        explicit Settings();

    protected:
        void init() override;
        void doConnect() override;
        void view() override;
    private:
        QWidget *m_row = nullptr;
        QPushButton *m_btn = nullptr;
    };
}
