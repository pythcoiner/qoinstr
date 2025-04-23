#include "MouseInspector.h"
#include <MainWindow.h>
#include <QApplication>
#include <Qontrol>
#include <qlogging.h>

auto main(int argc, char *argv[]) -> int {
    QApplication app(argc, argv);

    MainWindow window;
    window.setMinimumSize(1300, 800);
    window.setWindowTitle("Qoinstr");
    window.show();

    // auto *inspector = new MouseInspector(&app);
    // app.installEventFilter(inspector);

    return QApplication::exec();
}
