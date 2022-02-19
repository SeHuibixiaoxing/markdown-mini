#include "qfiledata.h"
#include <QDataStream>

QFileData::QFileData()
{
    fileName = "";
    lastEditeTime = QDateTime::currentDateTime();
    filePath = "";
}

QDataStream &operator<<(QDataStream& dataStream, const QFileData &fielData)
{
    dataStream << fielData.fileName << fielData.filePath << fielData.lastEditeTime;
    return dataStream;
}

QDataStream &operator>>(QDataStream& dataStream, QFileData &fielData)
{
    dataStream >> fielData.fileName >> fielData.filePath >> fielData.lastEditeTime;
    return dataStream;
}
