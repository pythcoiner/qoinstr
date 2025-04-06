#include "InputLine.h"
#include "../screens/common.h"
#include "Row.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>

namespace widget {

InputLine::InputLine(const QString &label, const QString &unit, QWidget *parent) {
    m_label = new QLabel(label);
    m_input = new QLineEdit();
    this->push(m_label)
        ->pushSpacer(H_SPACER)
        ->push(m_input)
        ;
    if (!unit.isEmpty()) {
        m_unit = new QLabel(unit);
        this->pushSpacer(H_SPACER)
            ->push(m_unit);
    }
    this->pushSpacer();
    this->setParent(parent);
}

auto InputLine::label() -> QLabel* {
    return m_label;
}
auto InputLine::input() -> QLineEdit* {
    return m_input;
}
} // namespace widget
