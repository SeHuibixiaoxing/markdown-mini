#ifndef QFILEDATA_H
#define QFILEDATA_H

#include <QDateTime>
#include <QString>

class QFileData
{
public:
    QFileData();
    QString fileName;
    QDateTime lastEditeTime;
    QString filePath;
    friend QDataStream &operator<<(QDataStream& dataStream, const QFileData &fielData);
    friend QDataStream &operator>>(QDataStream& dataStream, QFileData &fileData);
};

#endif // QFILEDATA_H
