#include "qweball.h"

QString QWebAll::profilePath = QDir::currentPath() + QString("/profile");
QString QWebAll::filePath = QDir::currentPath();


QWebAll::QWebAll()
{
    tcpSocketAll.abort();
    tcpSocketAll.connectToHost("127.0.0.1", 12005);

    connect(&tcpSocketAll, &QTcpSocket::readyRead, this, &QWebAll::dealAll);
}

void QWebAll::dealAllSignal()
{
    qDebug() << "运行dealAllSignal了" << endl;

    //发送信号
    QWebData signalWebData;
    signalWebData.dataType = QWebData::downloadAll;
    QByteArray signalByteArray;
    QDataStream signalDataStream(&signalByteArray, QIODevice::ReadWrite);
    signalDataStream << signalWebData;
    tcpSocketAll.write(signalByteArray);
    tcpSocketAll.waitForBytesWritten();

    qDebug() << "运行完dealAllSignal了" << endl;

}

void QWebAll::dealAll()
{
    qDebug() << "运行dealAll了" << endl;

    QDataStream inData(&(this->tcpSocketAll));

    QWebData webData;
    inData >> webData;

    QFile file(profilePath);
    file.open(QIODevice::ReadWrite | QFile::Truncate);
    if(webData.data.size() != 0)
    {
        QDataStream dataStream(&file);
        dataStream.writeRawData(webData.data[0], webData.data[0].size());
        //profileTextStream.writeRawData(webData.data[0], webData.data[0].size());
        //profileDataStream.writeRawData(webData.data[0].toUtf8(), webData.data[0].toUtf8().size());
    }

    for(int i = 1;i < webData.data.size();i += 2)
    {
        QString fileName = webData.data[i];
        QFile file(filePath + fileName);
        file.open(QIODevice::ReadWrite | QFile::Truncate | QFile::Text);
        QTextStream fileTextStream(&file);
        fileTextStream << webData.data[i + 1];
       // fileDataStream.writeRawData(webData.data[i + 1].toUtf8(), webData.data[i + 1].toUtf8().size());
    }

    qDebug() << "运行完dealAll了" << endl;

}
