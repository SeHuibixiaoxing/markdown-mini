#ifndef QPUSHBUTTONFILE_H
#define QPUSHBUTTONFILE_H
#include <QPushButton>
#include <QString>
#include <QDateTime>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPalette>
#include <QList>
#include <QVBoxLayout>
#include "qfiledata.h"
#include "mainwidget.h"
#include <qbuttonlist.h>

class QPushButtonFile;
class MainWidget;

class QPushButtonFile : public QPushButton
{
private:
    QFileData fileData;
    QVBoxLayout vBox;
    QFont nameFont;
    QFont timeFont;
    QPalette nameColor;
    QPalette timeColor;
    QLabel labelName;
    QLabel labelTime;
public:
    int freshTimes;

    static QPushButtonFile* activePushButtonFile;
    static QPlainTextEdit* plainTextEdit;
    static QList<QPushButtonFile*> pushButtonFileList;
    MainWidget* mainWidget;

    QFileData getFileData();
    void setLastEditTime(QDateTime lastEditTime = QDateTime::currentDateTime());
    void refreshText();

    QPushButtonFile(MainWidget* x, QString filePath, QString fileName, QDateTime lastEditTime = QDateTime::currentDateTime());
    ~QPushButtonFile();

signals:

public slots:
    void open();
    void rename(QString newName);

};

#endif // QPUSHBUTTONFILE_H
