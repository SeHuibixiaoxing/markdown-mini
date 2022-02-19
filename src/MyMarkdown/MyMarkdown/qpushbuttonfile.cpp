#include "qpushbuttonfile.h"
#include <QRgb>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QTextCursor>

QPushButtonFile* QPushButtonFile::activePushButtonFile = nullptr;
QPlainTextEdit* QPushButtonFile::plainTextEdit = nullptr;
QList<QPushButtonFile*> QPushButtonFile::pushButtonFileList;


QPushButtonFile::QPushButtonFile(MainWidget* x, QString filePath, QString fileName, QDateTime lastEditTime)
{
    this->mainWidget = x;
    //初始化
    fileData.fileName = fileName;
    fileData.filePath = filePath;
    fileData.lastEditeTime = lastEditTime;

    //设置按钮大小
    this->setFixedWidth(250);
    this->setFixedHeight(80);

    //设置名称、时间文字
    labelName.setText(fileName);
    labelTime.setText(fileData.lastEditeTime.toString("yy-MM-dd hh:mm:ss ddd"));

    //label size
    labelName.adjustSize();
    labelTime.adjustSize();

    //设置字体大小
    nameFont.setPointSize(15);
    timeFont.setPointSize(10);
    labelName.setFont(nameFont);
    labelTime.setFont(timeFont);

    //设置字体颜色
    nameColor.setColor(QPalette::WindowText, QColor::fromRgb(0, 0, 0));
    timeColor.setColor(QPalette::WindowText, QColor::fromRgb(123, 117, 111));
    labelName.setPalette(nameColor);
    labelTime.setPalette(timeColor);

    //加入到布局
    vBox.addWidget(&labelName, 0, Qt::AlignLeft);
    vBox.addWidget(&labelTime, 0, Qt::AlignRight);

    //布局应用到窗口
    this->setLayout(&vBox);

    //新建文件
    QFile file(filePath);
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    //
    pushButtonFileList.push_back(this);

    //
    this->setCheckable(true);
    this->setAutoExclusive(true);

    freshTimes = 0;

    connect(this, &QPushButton::clicked, this, &QPushButtonFile::open);
}

QPushButtonFile::~QPushButtonFile()
{

}

void QPushButtonFile:: setLastEditTime(QDateTime lastEditTime)
{
    this->fileData.lastEditeTime = lastEditTime;
    refreshText();
}

void QPushButtonFile::refreshText()
{
    labelName.setText(fileData.fileName);
    labelTime.setText(fileData.lastEditeTime.toString("yy-MM-dd hh:mm:ss ddd"));
}

void QPushButtonFile::open()
{
    if(plainTextEdit == nullptr)
    {
        qDebug() << "QPushButtonFile类中open槽函数异常，原因为QPushButtonFile对象中的plainTextEdit为nullptr" << endl;
        return;
    }

    //读取文件
    QFile file(fileData.filePath + fileData.fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    //把活动状态切换到当前文件，更新之前活动状态按钮的时间
   // if(QPushButtonFile::activePushButtonFile != nullptr)
    //{
      //  QPushButtonFile::activePushButtonFile->setLastEditTime();
    //}

    if(activePushButtonFile != nullptr)
        activePushButtonFile->freshTimes = 0;
    QPushButtonFile::activePushButtonFile = this;

    //
    plainTextEdit->setReadOnly(false);
    plainTextEdit->setFocus();

    //将文件内容输出到plainTextEdit上,光标移动到开头
    plainTextEdit->setPlainText(QString(file.readAll()));
    plainTextEdit->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);

    //设置为按下状态
    for(int i = 0;i < pushButtonFileList.size();++ i)
    {
        if(pushButtonFileList[i] != this)
            pushButtonFileList[i]->setDown(false);
        else
            pushButtonFileList[i]->setDown(true);
    }

    this->mainWidget->saveFile();
}

void QPushButtonFile::rename(QString newName)
{
    QFile file(fileData.filePath + fileData.fileName);
    file.copy(newName);
    file.remove();
    labelName.setText(newName);
    fileData.fileName = newName;
}

QFileData QPushButtonFile::getFileData()
{
    return this->fileData;
}
