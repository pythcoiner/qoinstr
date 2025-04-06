#pragma once

#include "AccountController.h"
#include "screens/Coins.h"
#include "widgets/ComboLine.h"
#include "widgets/InputLine.h"
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>

namespace modal {

class CreateAccount : public qontrol::Modal {
    Q_OBJECT
public:
    CreateAccount();

signals:
    void createAccount(
        QString name,
        QString mnemonic,
        Network network
    );


public slots:
    void onCreateAccount();
    void onGenerateMnemonic();

private:
    // Widgets
    widget::InputLine *m_name = nullptr;
    widget::InputLine *m_mnemonic = nullptr;
    widget::ComboLine *m_network = nullptr;
    QPushButton *m_create_btn = nullptr;
    QPushButton *m_generate_btn = nullptr;

};

} // namespace modal
