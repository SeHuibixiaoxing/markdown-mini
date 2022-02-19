#ifndef QAUTOSAVE_H
#define QAUTOSAVE_H

#include <QObject>
#include "mainwidget.h"

class QAutoSave : public QObject
{
    Q_OBJECT
public:
    MainWidget* widget;
    explicit QAutoSave(MainWidget* , QObject *parent = nullptr);
    ~QAutoSave();
    void handleTimeout();
    virtual void timerEvent(QTimerEvent *event) override;;
signals:

private:
    int m_nTimerID;

};

#endif // QAUTOSAVE_H
