#pragma once

#include "Row.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace widget {

class InputLine : public qontrol::Row {
    Q_OBJECT
public:
    InputLine(const QString &label, const QString &unit = "", QWidget *parent = nullptr);
    auto label() -> QLabel*;
    auto input() -> QLineEdit*;
private:
    QLabel *m_label = nullptr;
    QLineEdit *m_input = nullptr;
    QLabel *m_unit = nullptr;
};

} // namespace widget
