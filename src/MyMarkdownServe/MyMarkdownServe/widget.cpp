#include "widget.h"
#include <QDebug>
#include <QHostInfo>
#include <QDir>
#include <QSqlQuery>
#include <QDir>
#include <QSqlError>
#include <QStringList>
#include <QTextStream>
#include <QProcess>

QString Widget::cssFilePath = QDir::currentPath() + QString("/css/");
QString Widget::filePath = QDir::currentPath() + QString("/");


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    //数据库
    dataBase = QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setDatabaseName("mymarkdown");
    dataBase.setHostName("127.0.0.1");
    dataBase.setUserName("root");
    dataBase.setPassword("password");
    dataBase.open();

    QSqlQuery query(dataBase);


    query.exec("drop table profile");
    query.exec("drop table markdown");

    QStringList dataTable = dataBase.tables();


    if(!dataTable.contains("profile"))
        query.exec("create table profile(fileName text, fileData mediumblob)");
    else
        qDebug() << "已有profile表" << endl;

    if(!dataTable.contains("markdown"))
        query.exec("create table markdown(fileName blob, fileData mediumblob)");
    else
        qDebug() << "已有markdown表" << endl;

    //网络
    tcpServerProfile.listen(QHostAddress::LocalHost, 10000);
    tcpServerMarkdown.listen(QHostAddress::Any, 10001);
    tcpServerPdf.listen(QHostAddress::Any, 14003);
    tcpServerAll.listen(QHostAddress::Any, 12005);

    connect(&tcpServerProfile, &QTcpServer::newConnection, this, &Widget::connectProfile);
    connect(&tcpServerMarkdown, &QTcpServer::newConnection, this, &Widget::connectMarkdown);
    connect(&tcpServerPdf, &QTcpServer::newConnection, this, &Widget::connectPdf);
    connect(&tcpServerAll, &QTcpServer::newConnection, this, &Widget::connectAll);

}

Widget::~Widget()
{
}

void Widget::connectProfile()
{
    this->tcpSocketProfile = this->tcpServerProfile.nextPendingConnection();
    connect(tcpSocketProfile, &QTcpSocket::readyRead, this, &Widget::dealProfile);
}

void Widget::dealProfile()
{
    QDataStream inData(this->tcpSocketProfile);

    QWebData webData;

    inData >> webData;

  /*  QFile file(QDir::currentPath() + "/profile");
    file.open(QIODevice::ReadWrite | QFile::Truncate);
    QDataStream dataStream(&file);
    dataStream.writeRawData(webData.data[0], webData.data[0].size());
    file.close();*/

    QSqlQuery query(dataBase);
    query.prepare("select * from profile");
    query.bindValue(":fileName", "profile");
    query.exec();

    int cnt = 0;

    while(query.next())
    {
        ++ cnt;
    }

    if(cnt == 0)
    {
        query.prepare("insert into profile (fileName, fileData) values (:fileName, :fileData)");
        query.bindValue(":fileName", "profile");
        query.bindValue(":fileData", webData.data[0]);
        query.exec();
    }
    else
    {
        query.prepare("update profile set fileData = :fileData where fileName = :fileName");
        query.bindValue(":fileName", "profile");
        query.bindValue(":fileData", webData.data[0]);
        query.exec();
    }


}

void Widget::connectMarkdown()
{
    this->tcpSocketMarkdown = this->tcpServerMarkdown.nextPendingConnection();
    connect(tcpSocketMarkdown, &QTcpSocket::readyRead, this, &Widget::dealMarkdown);
}

void Widget::dealMarkdown()
{
    int a = 1;
    a = a + 1;
    QDataStream inData(this->tcpSocketMarkdown);
    QWebData webData;
    inData >> webData;

    QSqlQuery query(dataBase);
   // qDebug() << webData.data[0] << endl << webData.data[1] << endl;

    query.prepare("select * from markdown where fileName = :fileName");
    query.bindValue(":fileName", webData.data[0]);
    query.exec();
    QByteArray tmp;
    while (query.next())
    {
        tmp = query.value(0).toByteArray();
    }

    if(tmp != webData.data[0])
    {
        query.prepare("insert into markdown (fileName, fileData) values (:fileName, :fileData)");
        query.bindValue(":fileName", webData.data[0]);
        query.bindValue(":fileData", webData.data[1]);
        query.exec();
    }
    else
    {
        query.prepare("update markdown set fileData = :fileData where fileName = :fileName");
        query.bindValue(":fileName", webData.data[0]);
        query.bindValue(":fileData", webData.data[1]);
        query.exec();
    }

    //将文件存到本地

    QFile file(filePath + "tmp.md");
    file.open(QIODevice::ReadWrite | QFile::Truncate | QFile::Text);
    QTextStream textStream(&file);
    textStream << QString(webData.data[1]);
    file.close();

    //转换
    QProcess::execute("pandoc --standalone --self-contained --css " + cssFilePath + "k.css" + " " + filePath + "tmp.md" + " --output " + filePath + "tmp.html" + " --mathml");

    //读取html
    QFile htmlFile(filePath + "tmp.html");
    htmlFile.open(QIODevice::ReadWrite);
    QDataStream htmlStream(&htmlFile);
    QByteArray html = htmlFile.readAll();

    htmlFile.close();

    htmlFile.remove();
    file.remove();

    //发送
    QWebData sendData;
    sendData.dataType = QWebData::uploadMarkdown;
    sendData.insertData(html);
    QByteArray sendByteArray;
    QDataStream sendStream(&sendByteArray, QIODevice::ReadWrite);
    sendStream << sendData;
    this->tcpSocketMarkdown->write(sendByteArray);
    this->tcpSocketMarkdown->waitForBytesWritten();
}


void Widget::connectPdf()
{
    qDebug() << "运行connectPdf了" << endl;
    this->tcpSocketPdf = this->tcpServerPdf.nextPendingConnection();
    connect(tcpSocketPdf, &QTcpSocket::readyRead, this, &Widget::dealPdf);
}

void Widget::dealPdf()
{
    qDebug() << "运行dealPdf了" << endl;

    QDataStream inData(this->tcpSocketPdf);

    QWebData webData;

    inData >> webData;

    //将文件保存到本地
    QFile file(filePath + "pdf/tmp.md");
    file.open(QIODevice::ReadWrite | QFile::Truncate | QFile::Text);
    QTextStream textStream(&file);
    textStream << QString(webData.data[1]);
    file.close();

    //转换
    QProcess::execute("pandoc --standalone --self-contained --css " + cssFilePath + "k.css" + " " + filePath + "pdf/tmp.md" + " --output " + filePath + "pdf/tmp.html" + " --mathml");
    QProcess::execute("wkhtmltopdf " + filePath + "pdf/tmp.html " + filePath + "pdf/tmp.pdf");

    //读取pdf
    QFile pdfFile(filePath + "pdf/tmp.pdf");
    pdfFile.open(QIODevice::ReadWrite);
    QDataStream pdfStream(&pdfFile);
    QByteArray pdf = pdfFile.readAll();

    pdfFile.close();

    pdfFile.remove();
    file.remove();
    QFile htmlFile(filePath + "pdf/tmp.html");
    htmlFile.remove();
    htmlFile.close();

    //发送
    QWebData sendData;
    sendData.dataType = QWebData::toPdf;
    sendData.insertData(webData.data[0]);
    sendData.insertData(pdf);
    QByteArray sendByteArray;
    QDataStream sendStream(&sendByteArray, QIODevice::ReadWrite);
    sendStream << sendData;
    this->tcpSocketPdf->write(sendByteArray);
    this->tcpSocketPdf->waitForBytesWritten();
}

void Widget::connectAll()
{
    this->tcpSocketAll = this->tcpServerAll.nextPendingConnection();
    connect(tcpSocketAll, &QTcpSocket::readyRead, this, &Widget::dealAll);
}

void Widget::dealAll()
{
    QString ss = this->tcpSocketAll->readAll();

    QWebData webData;
    webData.dataType = QWebData::downloadAll;

  /*  QFile file(QDir::currentPath() + "/profile");
    file.open(QIODevice::ReadWrite);

    QByteArray s = file.readAll();
    webData.insertData(s);

    qDebug() << s << endl;*/



    QSqlQuery query;

    query.exec("select * from profile");
    while(query.next())
    {
        webData.insertData(query.value(1).toByteArray());
    }

    query.exec("select * from markdown");
    while(query.next())
    {
        webData.insertData(query.value(0).toByteArray());
        webData.insertData(query.value(1).toByteArray());
    }

    QByteArray dataByteArray;
    QDataStream byteDataStream(&dataByteArray, QIODevice::ReadWrite);
    byteDataStream << webData;

   // file.close();

    tcpSocketAll->write(dataByteArray);
    tcpSocketAll->waitForBytesWritten();
}
