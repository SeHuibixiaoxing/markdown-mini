#include "qwebdata.h"
#include <QFile>
#include <QDataStream>

QWebData::QWebData()
{

}

void QWebData::save(QString filePath, QString fileName, int rank)
{
    QFile file(filePath + fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(this->data.at(rank));
}

void QWebData::insertData(const QByteArray& data)
{
    this->data.append(data);
}

void QWebData::insertData(const QString& data)
{
    this->data.append(data.toUtf8());
}

QDataStream &operator<<(QDataStream& dataStream, const QWebData &webFile)
{
    dataStream << webFile.dataType << webFile.data;
    return dataStream;
}

QDataStream &operator>>(QDataStream& dataStream, QWebData &webFile)
{
    dataStream >> webFile.dataType >> webFile.data;
    return dataStream;
}
