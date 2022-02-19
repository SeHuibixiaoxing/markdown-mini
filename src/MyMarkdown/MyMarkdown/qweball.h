#ifndef QWEBALL_H
#define QWEBALL_H

#include <QDebug>
#include "qwebdata.h"
#include <QByteArray>
#include <QDataStream>
#include <QTcpServer>
#include <QFile>
#include <QTcpSocket>
#include <QDir>
#include <QWidget>

class QWebAll : QWidget
{
public:
    QTcpSocket tcpSocketAll;
    QTcpServer tcpServerAll;
    static QString profilePath;
    static QString filePath;
    QWebAll();
    void dealAllSignal();
public slots:
    void dealAll();
};

#endif // QWEBALL_H
