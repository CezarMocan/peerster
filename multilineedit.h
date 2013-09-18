#ifndef MULTILINEEDIT_H
#define MULTILINEEDIT_H

#include <QTextEdit>

class ReturnKeyFilter : public QObject
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void returnKeyPressedSignal();
};

class MultiLineEdit : public QTextEdit
{
    Q_OBJECT

public:
    MultiLineEdit(QWidget *parent = 0);

signals:
    void returnSignal();

public slots:
    void returnKeyPressedSlot();

private:
    ReturnKeyFilter *returnKeyFilter;
};

#endif // MULTILINEEDIT_H
