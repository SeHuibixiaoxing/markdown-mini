#ifndef QWEBFILE_H
#define QWEBFILE_H

#include <QString>
#include <QList>


class QWebData
{
public:
    QWebData();
    //数据类型：
    //uploadProfile，uploadMarkdown, toHtml, toPdf, deleteFile, downloadAll
    enum type {uploadProfile, uploadMarkdown, toHtml, toPdf, deleteFile, downloadAll};
    type dataType;
    QList<QByteArray> data;
    void save(QString filePath, QString fileName, int rank = 0);
    void insertData(const QByteArray& data);
    void insertData(const QString& data);

    friend QDataStream &operator<<(QDataStream& dataStream, const QWebData &webFile);
    friend QDataStream &operator>>(QDataStream& dataStream, QWebData &webFile);
};

#endif // QWEBFILE_H
