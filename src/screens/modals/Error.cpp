#include "Error.h"
#include "Column.h"
#include "Row.h"

#include <Qontrol>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringliteral.h>
#include <qwidget.h>

namespace modal {

    Error::Error(const QString& msg) 
        : Error(msg, QStringLiteral("Error")){}

    Error::Error(const QString& msg, const QString& title) {
        this->setModal(true);
        this->setWindowTitle(title);
        this->setLayout(new QHBoxLayout);
        this->setFixedSize(300,160);
        auto *col = new qontrol::Column();
        auto *label = new QLabel(msg);
        auto  *btn = new QPushButton("Close");
        connect(btn, &QPushButton::clicked, this, &QDialog::close);
        auto *btnRow = (new qontrol::Row)
            ->pushSpacer()
            ->push(btn)
            ->pushSpacer()
            ;
        auto *labelRow = (new qontrol::Row)
            ->pushSpacer()
            ->push(label)
            ->pushSpacer()
            ;
        col->pushSpacer()
            ->push(labelRow)
            ->pushSpacer()
            ->push(btnRow)
            ->pushSpacer(10)
            ;
        this->layout()->addWidget(col);
        col->setParent(this);
    }

} // namespace modal
