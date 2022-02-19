#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include "qwebdata.h"


class Widget : public QWidget
{
    Q_OBJECT
public:
    //数据库
    QSqlDatabase dataBase;

    //网络
private:
    QTcpServer tcpServerProfile;
    QTcpServer tcpServerMarkdown;
    QTcpServer tcpServerPdf;
    QTcpServer tcpServerAll;

    QTcpSocket* tcpSocketProfile;
    QTcpSocket* tcpSocketMarkdown;
    QTcpSocket* tcpSocketPdf;
    QTcpSocket* tcpSocketAll;
public:
    static QString cssFilePath;
    static QString filePath;

    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void connectProfile();
    void connectMarkdown();
    void connectPdf();
    void connectAll();


    void dealProfile();
    void dealMarkdown();
    void dealPdf();
    void dealAll();
};
#endif // WIDGET_H
