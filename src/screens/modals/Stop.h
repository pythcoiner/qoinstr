#pragma once

#include "Modal.h"

namespace modal {

class Stop : public qontrol::Modal {
    Q_OBJECT
public:
    Stop();
    void stop();

public slots:
    void onStopped();

private:
    int m_count = 0;
    int m_stopped = 0;
};

} // namespace modal
