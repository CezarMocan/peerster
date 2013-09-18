#include <QEvent>
#include <QKeyEvent>

#include "multilineedit.h"

bool ReturnKeyFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            emit returnKeyPressedSignal();
            return true;
        }
        else {
            return QObject::eventFilter(obj, event);
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
}

MultiLineEdit::MultiLineEdit(QWidget *parent) : QTextEdit(parent) {
    this->returnKeyFilter = new ReturnKeyFilter();
    this->installEventFilter(this->returnKeyFilter);
    connect(returnKeyFilter, SIGNAL(returnKeyPressedSignal()),
        this, SLOT(returnKeyPressedSlot()));
}

void MultiLineEdit::returnKeyPressedSlot() {
    emit returnSignal();
}
