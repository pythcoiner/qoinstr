#include "Stop.h"
#include "AppController.h"
#include "Column.h"
#include "Row.h"
#include <qlabel.h>
#include <qtimer.h>

namespace modal {

Stop::Stop() {
    m_count = AppController::get()->accounts();
    setWindowTitle("Stopping");
    setFixedSize(300,150);
    auto *label = new QLabel("Stopping the service...");
    auto *col = (new qontrol::Column)
        ->pushSpacer()
        ->push(label)
        ->pushSpacer()
        ;
    auto *row = (new qontrol::Row)
        ->pushSpacer()
        ->push(col)
        ->pushSpacer()
        ;
    setMainWidget(row);

}

void Stop::onStopped() {
    m_stopped++;
    if (m_stopped >= m_count) {
        close();
    }
}

void Stop::stop() {
    QTimer::singleShot(50, this, [this]() {
        AppController::get()->stop(this);
        if (m_count == 0) {
            close();
        }
    });
}

} // namespace modal
