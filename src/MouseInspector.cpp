#include "MouseInspector.h"

bool MouseInspector::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QPoint globalPos = QCursor::pos();
        QWidget *widget = QApplication::widgetAt(globalPos);
        if (widget) {
            qDebug() << "Mouse click on widget:"
                     << widget->metaObject()->className()
                     << "ObjectName:" << widget->objectName()
                     << "Pos:" << globalPos;
        } else {
            qDebug() << "Mouse click on: No widget found at" << globalPos;
        }
    }
    return QObject::eventFilter(obj, event); // Let other events through
}
