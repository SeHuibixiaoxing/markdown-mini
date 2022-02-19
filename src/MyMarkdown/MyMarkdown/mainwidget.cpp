#include "mainwidget.h"
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QList>
#include <QProcess>
#include "qfiledata.h"
#include <QPlainTextEdit>
#include <QTextCursor>
#include <Python.h>
#include <QIODevice>
#include <QUrl>
#include <QDataStream>
#include <QThread>
#include <sys/stat.h>
#include <QFileDialog>

QString MainWidget::filePath = QDir::currentPath() + QString("/document/");
QString MainWidget::profilePath = QDir::currentPath() + QString("/profile");
QString MainWidget::cssFilePath = QDir::currentPath() + QString("/css/");
QString MainWidget::htmlFilePath = QDir::currentPath() + QString("/html/");
QString MainWidget::pdfFilePath = QDir::currentPath() + QString("/pdf/");

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    initWeb();
   // initUI();
}

MainWidget::~MainWidget()
{
    //存入配置文件
    saveProfile();
}

/*
 *网络部分
 */
void MainWidget::initWeb()
{
    tcpSocketProfile.abort();
    tcpSocketMarkdown.abort();
    tcpSocketPdf.abort();
    tcpSocketAll.abort();

    tcpSocketProfile.connectToHost("127.0.0.1", 10000);
    tcpSocketMarkdown.connectToHost("127.0.0.1", 10001);
    tcpSocketPdf.connectToHost("127.0.0.1", 14003);
    tcpSocketAll.connectToHost("127.0.0.1", 12005);

    connect(&tcpSocketAll, &QTcpSocket::readyRead, this, &MainWidget::dealAll);
    connect(&tcpSocketMarkdown, &QTcpSocket::readyRead, this, &MainWidget::refreshFileView);
    connect(&tcpSocketPdf, &QTcpSocket::readyRead, this, &MainWidget::dealPdf);

    //从服务端下载全部所需文件到本地
    dealAllSignal();
}

void MainWidget::dealProfile()
{
    QFile profile(profilePath);
    profile.open(QIODevice::ReadOnly);

    QWebData x;
    x.dataType = QWebData::uploadProfile;
    QByteArray ba;
    ba = profile.readAll();
    x.insertData(ba);

    QByteArray byteArray;
    QDataStream xStream(&byteArray, QIODevice::ReadWrite);
    xStream << x;

    profile.close();

    tcpSocketProfile.write(byteArray);
    tcpSocketProfile.waitForBytesWritten();
}

void MainWidget::dealMarkdown(QFileData fileData)
{
    QFile file(fileData.filePath + fileData.fileName);
    file.open(QIODevice::ReadWrite);

    QWebData x;
    x.dataType = QWebData::uploadMarkdown;
    x.insertData(fileData.fileName);
    x.insertData(file.readAll());

    QByteArray byteArray;
    QDataStream xStream(&byteArray, QIODevice::ReadWrite);
    xStream << x;

    file.close();

    tcpSocketMarkdown.write(byteArray);
    tcpSocketMarkdown.waitForBytesWritten();
}

void MainWidget::dealAllSignal()
{
    //发送信号
    QWebData signalWebData;
    signalWebData.dataType = QWebData::downloadAll;
    QByteArray signalByteArray;
    QDataStream signalDataStream(&signalByteArray, QIODevice::ReadWrite);
    signalDataStream << signalWebData;
    tcpSocketAll.write(signalByteArray);
    tcpSocketAll.waitForBytesWritten();

}

void MainWidget::dealAll()
{
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

    file.close();

    for(int i = 1;i < webData.data.size();i += 2)
    {
        QString fileName = webData.data[i];
        QFile file(filePath + fileName);
        file.open(QIODevice::ReadWrite | QFile::Truncate | QFile::Text);
        QTextStream fileTextStream(&file);
        fileTextStream << webData.data[i + 1];
       // fileDataStream.writeRawData(webData.data[i + 1].toUtf8(), webData.data[i + 1].toUtf8().size());

        file.close();
    }

    initUI();
}

/*
 *  UI部分
 */
void MainWidget::initUI()
{
    //初始化常量
    leftPushButtonSize = new QSize(40, 40);             //左下方按钮默认大小

    //布局
    mainHBox = new QHBoxLayout(this);
    leftWidget = new QWidget(this);
    rightWidget = new QWidget(this);
    this->setLayout(mainHBox);
    mainHBox->addWidget(leftWidget);
    mainHBox->addWidget(rightWidget);
    mainHBox->setMargin(0);
    mainHBox->setSpacing(0);
    mainHBox->setAlignment(Qt::AlignLeft);
    this->resize(1200,800);

    leftVBox = new QVBoxLayout(leftWidget);
    buttonList = new QButtonList();
    buttonList->setParent(leftWidget);

    buttonList->setFixedWidth(250);
    leftVBox->addWidget(buttonList);
    leftWidget->setFixedWidth(250);
    leftVBox->setContentsMargins(0,0,0,0);
    leftVBox->setSpacing(0);
    leftWidget->setLayout(leftVBox);


    pushButtonWidget = new QWidget(leftWidget);
    pushButtonHBox = new QHBoxLayout(pushButtonWidget);

    newPushButton = new QPushButton("new", pushButtonWidget);
    exportPushButton = new QPushButton("export", pushButtonWidget);
    renamePushButton = new QPushButton("rename", pushButtonWidget);
    deletePushButton = new QPushButton("delete", pushButtonWidget);

    newPushButton->setFixedSize(*leftPushButtonSize);
    exportPushButton->setFixedSize(*leftPushButtonSize);
    renamePushButton->setFixedSize(*leftPushButtonSize);
    deletePushButton->setFixedSize(*leftPushButtonSize);

    pushButtonHBox->setAlignment(Qt::AlignLeft);
    pushButtonHBox->setMargin(0);
    pushButtonHBox->setSpacing(0);
    pushButtonHBox->addWidget(newPushButton);
    pushButtonHBox->addWidget(exportPushButton);
    pushButtonHBox->addWidget(renamePushButton);
    pushButtonHBox->addWidget(deletePushButton);


    pushButtonWidget->setFixedWidth(250);
    pushButtonWidget->setLayout(pushButtonHBox);

    leftVBox->addWidget(pushButtonWidget);

    toolBarWidget = new QToolBar(rightWidget);
    rightBottomWidget = new QWidget(rightWidget);
    rightBottomHBox = new QHBoxLayout(rightBottomWidget);
    textWidget = new QPlainTextEdit(rightBottomWidget);
    webViewWidget = new QWebEngineView(rightBottomWidget);
    rightVBox = new QVBoxLayout(rightWidget);
    rightBottomHBox->addWidget(textWidget, 1);
    rightBottomHBox->addWidget(webViewWidget, 1);
    rightBottomHBox->setMargin(0);
    rightBottomHBox->setSpacing(0);

    rightBottomWidget->setLayout(rightBottomHBox);
    rightVBox->addWidget(toolBarWidget);
    rightVBox->addWidget(rightBottomWidget);
    rightVBox->setContentsMargins(0, 0, 0, 0);
    rightVBox->setSpacing(0);
    rightVBox->setAlignment(Qt::AlignTop);
    rightWidget->setLayout(rightVBox);

    //
    toolBarWidget->addAction("B", this, &MainWidget::bold);
    toolBarWidget->addAction("I", this, &MainWidget::italics);
    toolBarWidget->addAction("-", this, &MainWidget::strikethrough);
    toolBarWidget->addSeparator();
    toolBarWidget->addAction("h1", this, &MainWidget::h1);
    toolBarWidget->addAction("h2", this, &MainWidget::h2);
    toolBarWidget->addAction("h3", this, &MainWidget::h3);
    toolBarWidget->addAction("h4", this, &MainWidget::h4);
    toolBarWidget->addAction("h5", this, &MainWidget::h5);
    toolBarWidget->addAction("h6", this, &MainWidget::h6);
    toolBarWidget->addSeparator();
    toolBarWidget->addAction("水平线", this, &MainWidget::horizontalLine);
    toolBarWidget->addAction("引用", this, &MainWidget::quote);
    toolBarWidget->addAction("Html", this, &MainWidget::html);
    toolBarWidget->addSeparator();
    toolBarWidget->addAction("链接", this, &MainWidget::link);
    toolBarWidget->addAction("图片", this, &MainWidget::image);
    toolBarWidget->addAction("代码", this, &MainWidget::code);
    toolBarWidget->addAction("数学公式", this, &MainWidget::math);

    //初始化文件按钮
    QPushButtonFile::activePushButtonFile = nullptr;
    QPushButtonFile::plainTextEdit = this->textWidget;

    textWidget->setReadOnly(true);
    exportPushButton->setDisabled(true);
    renamePushButton->setDisabled(true);
    deletePushButton->setDisabled(true);
    newPushButton->setDisabled(true);

    //连接信号与槽
    connect(this->newPushButton, &QPushButton::clicked, this, &MainWidget::newFileLocation);
    //connect(this->textWidget, &QPlainTextEdit::textChanged, this, &MainWidget::saveFile);
    connect(this->renamePushButton, &QPushButton::clicked, this, &MainWidget::renameFileLocation);
    connect(this->deletePushButton, &QPushButton::clicked, this, &MainWidget::deleteFileLocation);
    //connect(this->textWidget, &QPlainTextEdit::textChanged, this, &MainWidget::refreshFileView);
    connect(this->exportPushButton, &QPushButton::clicked, this, &MainWidget::exportFileLocation);
    connect(this->textWidget, &QPlainTextEdit::textChanged, this, &MainWidget::refreshTime);

    connect(this, &MainWidget::bold, this, &MainWidget::actionBold);
    connect(this, &MainWidget::italics, this, &MainWidget::actionItalics);
    connect(this, &MainWidget::strikethrough, this, &MainWidget::actionStrikethrough);
    connect(this, &MainWidget::h1, this, &MainWidget::actionH1);
    connect(this, &MainWidget::h2, this, &MainWidget::actionH2);
    connect(this, &MainWidget::h3, this, &MainWidget::actionH3);
    connect(this, &MainWidget::h4, this, &MainWidget::actionH4);
    connect(this, &MainWidget::h5, this, &MainWidget::actionH5);
    connect(this, &MainWidget::h6, this, &MainWidget::actionH6);
    connect(this, &MainWidget::horizontalLine, this, &MainWidget::actionHorizontalLine);
    connect(this, &MainWidget::quote, this, &MainWidget::actionQuote);
    connect(this, &MainWidget::html, this, &MainWidget::actionHtml);
    connect(this, &MainWidget::link, this, &MainWidget::actionLink);
    connect(this, &MainWidget::image, this, &MainWidget::actionImage);
    connect(this, &MainWidget::code, this, &MainWidget::actionCode);
    connect(this, &MainWidget::math, this, &MainWidget::actionMath);

    //初始化文件
    initFile();

    //定时器启动
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWidget::saveFile);
    timer->start(500);
}

void MainWidget::initFile()
{
    //读取配置文件
    QFileInfo profileInfo(profilePath);
    QFile profile(MainWidget::profilePath);
    profile.open(QIODevice::ReadOnly);
    QDataStream dataStream(&profile);
    while(!dataStream.atEnd())
    {
       QFileData fileData;
       dataStream >> fileData;
       QPushButtonFile* pushButtonFile = new QPushButtonFile(this, fileData.filePath, fileData.fileName, fileData.lastEditeTime);
       buttonList->addPushButtonFile(pushButtonFile);
    }
    profile.close();

    if(QPushButtonFile::pushButtonFileList.size() > 0)
    {
        QPushButtonFile::pushButtonFileList[0]->open();
        textWidget->setReadOnly(false);
        exportPushButton->setDisabled(false);
        renamePushButton->setDisabled(false);
        deletePushButton->setDisabled(false);
        dealMarkdown(QPushButtonFile::activePushButtonFile->getFileData());
    }
    newPushButton->setDisabled(false);
}

void MainWidget::actionBold()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("****");
        textWidget->moveCursor(QTextCursor::Left);
        textWidget->moveCursor(QTextCursor::Left);
    }
    else
    {
        textWidget->insertPlainText("**" + textCursor.selectedText() + "**");
    }
}


void MainWidget::actionItalics()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("**");
        textWidget->moveCursor(QTextCursor::Left);
    }
    else
    {
        textWidget->insertPlainText("*" + textCursor.selectedText() + "*");
    }
}
void MainWidget::actionStrikethrough()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("~~~~");
        textWidget->moveCursor(QTextCursor::Left);
        textWidget->moveCursor(QTextCursor::Left);
    }
    else
    {
        textWidget->insertPlainText("~~" + textCursor.selectedText() + "~~");
    }
}
void MainWidget::actionH1()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("#");
    }
    else
    {
        textWidget->insertPlainText("#" + textCursor.selectedText());
    }
}
void MainWidget::actionH2()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("##");
    }
    else
    {
        textWidget->insertPlainText("##" + textCursor.selectedText());
    }
}
void MainWidget::actionH3()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("###");
    }
    else
    {
        textWidget->insertPlainText("###" + textCursor.selectedText());
    }
}
void MainWidget::actionH4()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("####");
    }
    else
    {
        textWidget->insertPlainText("####" + textCursor.selectedText());
    }
}
void MainWidget::actionH5()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("#####");
    }
    else
    {
        textWidget->insertPlainText("#####" + textCursor.selectedText());
    }
}
void MainWidget::actionH6()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("######");
    }
    else
    {
        textWidget->insertPlainText("######" + textCursor.selectedText());
    }
}
void MainWidget::actionHorizontalLine()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("---\n\n\n---");
        textWidget->moveCursor(QTextCursor::Up);
    }
    else
    {
        textWidget->insertPlainText("---\n\n" + textCursor.selectedText() + "\n---");
        textWidget->moveCursor(QTextCursor::Up);
    }
}
void MainWidget::actionQuote()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("> ");
    }
    else
    {
        textWidget->insertPlainText("> " + textCursor.selectedText());
    }
}
void MainWidget::actionHtml()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("<html>\n\n</html>");
        textWidget->moveCursor(QTextCursor::Up);
    }
    else
    {
        textWidget->insertPlainText("<html>\n" + textCursor.selectedText() + "\n</html>");
        textWidget->moveCursor(QTextCursor::Up);
    }
}
void MainWidget::actionLink()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("[link]()");
        textWidget->moveCursor(QTextCursor::Left);
    }
    else
    {
        textWidget->insertPlainText("[link](" + textCursor.selectedText() + ")");
    }
}
void MainWidget::actionImage()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("![]()");
        textWidget->moveCursor(QTextCursor::Left);
    }
    else
    {
        textWidget->insertPlainText("![](" + textCursor.selectedText() + ")");
    }
}
void MainWidget::actionCode()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    const QTextCursor& textCursor = this->textWidget->textCursor();
    if(!textCursor.hasSelection())
    {
        textWidget->insertPlainText("```\n\n```");
        textWidget->moveCursor(QTextCursor::Up);
    }
    else
    {
        textWidget->insertPlainText("```\n" + textCursor.selectedText() + "\n```\n");
    }
}
void MainWidget::actionMath()
{
    if(QPushButtonFile::pushButtonFileList.size() == 0) return;
    QString insert = QMathDialog::creat();
    textWidget->insertPlainText("$" + insert + "$");
}

void MainWidget::newFileLocation()
{
    bool tag = false;
    QString fileName = QInputDialog::getText(this, QString("新建"), QString("请输入新建markdown文件名："), QLineEdit::Normal, 0, &tag);
    if(!tag) ;
    else if(fileName.isEmpty())
    {
        QMessageBox::warning(this, QString("警告"), QString("文件名不能为空"));
    }
    else if(fileName.contains('/'))
    {
        QMessageBox::warning(this, QString("警告"), QString("文件名不能含有'/'"));
    }
    else
    {
        QFileInfo fileInfo(filePath + fileName + QString(".md"));
        if(fileInfo.isFile())
        {
            QMessageBox::warning(this, QString("警告"), QString("已有该文件！"));
        }
        else
        {
            QPushButtonFile* pushButtonFile = new QPushButtonFile(this, filePath, fileName + QString(".md"));
            //创建文件
            QFile file(pushButtonFile->getFileData().filePath + pushButtonFile->getFileData().fileName);
            file.open(QIODevice::ReadWrite | QIODevice::Text);

            buttonList->addPushButtonFile(pushButtonFile);
            exportPushButton->setDisabled(false);
            renamePushButton->setDisabled(false);
            deletePushButton->setDisabled(false);
            if(QPushButtonFile::pushButtonFileList.size() == 1)
                pushButtonFile->open();
            dealMarkdown(pushButtonFile->getFileData());

            file.close();
        }
    }
}

void MainWidget::saveProfile()
{
   // 配置文件
   //存储所有的QPushButtonFile
   QFile profile(MainWidget::profilePath);
   profile.open(QIODevice::WriteOnly | QFile::Truncate);
   QDataStream dataStream(&profile);
   for(int i = 0;i < QPushButtonFile::pushButtonFileList.size();++ i)
        dataStream << QPushButtonFile::pushButtonFileList[i]->getFileData();
    profile.close();

   //存储到云端
   dealProfile();

}

void MainWidget::refreshTime()
{
    if(QPushButtonFile::activePushButtonFile == nullptr) return;
    QPushButtonFile* &tmp = QPushButtonFile::activePushButtonFile;

    if(tmp->freshTimes < 2) ++ tmp->freshTimes;

    //更新活动状态按钮的时间
    if(tmp->freshTimes == 2)
        QPushButtonFile::activePushButtonFile->setLastEditTime(QDateTime::currentDateTime());

}

void MainWidget::saveFile()
{
    if(QPushButtonFile::activePushButtonFile == nullptr) return;
    QPushButtonFile* &tmp = QPushButtonFile::activePushButtonFile;
    //读取文件
    QFile file(tmp->getFileData().filePath + tmp->getFileData().fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    QTextStream stream(&file);
    stream << textWidget->toPlainText();
    file.close();
    dealMarkdown(QPushButtonFile::activePushButtonFile->getFileData());
}

void MainWidget::renameFileLocation()
{
    if(QPushButtonFile::activePushButtonFile == nullptr)
        return;

    bool tag = false;
    QString fileName = QInputDialog::getText(this, QString("重命名"), QString("将该文件重命名为："), QLineEdit::Normal, 0, &tag);
    if(!tag) ;
    else if(fileName.isEmpty())
        QMessageBox::warning(this, QString("警告"), QString("文件名不能为空"));
    else if(fileName.contains('/'))
        QMessageBox::warning(this, QString("警告"), QString("文件名不能含有'/'"));
    else
    {
        QFileInfo fileInfo(filePath + fileName + QString(".md"));

        if(fileInfo.isFile())
        {
            QMessageBox::warning(this, QString("警告"), QString("已有该文件名！"));
        }
        else
        {
            QPushButtonFile::activePushButtonFile->rename(fileName + QString(".md"));
            QPushButtonFile::activePushButtonFile->refreshText();
            this->saveFile();
        }
    }
}

void MainWidget::deleteFileLocation()
{
    if(QPushButtonFile::activePushButtonFile == nullptr)
    {
        textWidget->setPlainText("");
        webViewWidget->setContent("");
        textWidget->setReadOnly(true);
        exportPushButton->setDisabled(true);
        renamePushButton->setDisabled(true);
        deletePushButton->setDisabled(true);
        return;
    }
    int pos = -1;
    for(int i = 0;i < QPushButtonFile::pushButtonFileList.size();++ i)
        if(QPushButtonFile::pushButtonFileList[i] == QPushButtonFile::activePushButtonFile)
        {
            QPushButtonFile::pushButtonFileList.removeAt(i);
            pos = i;
            break;
        }

    QFile file(QPushButtonFile::activePushButtonFile->getFileData().filePath + QPushButtonFile::activePushButtonFile->getFileData().fileName);
    file.remove();
    delete QPushButtonFile::activePushButtonFile;


    if(QPushButtonFile::pushButtonFileList.size() == 0)
    {
        QPushButtonFile::activePushButtonFile = nullptr;
        textWidget->setPlainText("");
        webViewWidget->setContent("");
        textWidget->setReadOnly(true);
        exportPushButton->setDisabled(true);
        renamePushButton->setDisabled(true);
        deletePushButton->setDisabled(true);
    }
    else
    {
        if(pos > QPushButtonFile::pushButtonFileList.size() - 1)
            pos = QPushButtonFile::pushButtonFileList.size() - 1;
        QPushButtonFile::activePushButtonFile = QPushButtonFile::pushButtonFileList[pos];
        QPushButtonFile::activePushButtonFile->open();
    }

    file.close();
}

QString MainWidget::markdownToHtml(QString markdownName)
{
    QProcess::execute("pandoc --standalone --self-contained --css " + cssFilePath + "k.css" + " " + filePath + markdownName + " --output " + htmlFilePath + markdownName.mid(0, markdownName.length() - 3) + ".html --mathml");
    QFile htmlFile(htmlFilePath + markdownName.mid(0, markdownName.length() - 3) + ".html");
    htmlFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QString re = QString(htmlFile.readAll());
    htmlFile.close();
    return re;
}

QString MainWidget::markdownToPdf(QString markdownName)
{
    QProcess::execute("pandoc --standalone --self-contained --css " + cssFilePath + "k.css" + " " + filePath + markdownName + " --output " + htmlFilePath + markdownName.mid(0, markdownName.length() - 3) + ".html --mathml");
    QProcess::execute("wkhtmltopdf " +
                      htmlFilePath + markdownName.mid(0, markdownName.length() - 3) + ".html "
                      + pdfFilePath + markdownName.mid(0, markdownName.length() - 3) + ".pdf");    
    return pdfFilePath + markdownName.mid(0, markdownName.length() - 3) + ".pdf";
}

void MainWidget::refreshFileView()
{
    if(QPushButtonFile::activePushButtonFile == nullptr)
    {
        this->webViewWidget->setContent("");
        return;
    }

    QDataStream inData(&(this->tcpSocketMarkdown));

    QWebData webData;
    inData >> webData;

    if(QPushButtonFile::activePushButtonFile != nullptr)
        this->webViewWidget->setContent(webData.data[0], "text/html;text/xml;charset=UTF-8", QUrl("http://www.w3.org/1998/Math/MathML"));
    /*QString a = markdownToPdf(QPushButtonFile::activePushButtonFile->getFileData().fileName);
    QUrl b(a);
    this->webViewWidget->setUrl(b);*/
}

void MainWidget::exportFileLocation()
{
    if(QPushButtonFile::activePushButtonFile == nullptr) return;
    QString name = QPushButtonFile::activePushButtonFile->getFileData().fileName;
    QString filePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出为PDF文件"), name.mid(0, name.size() - 3), tr("*.pdf"));
    dealPdfSignal(filePath);
}

void MainWidget::dealPdfSignal(QString pdfPath)
{
    qDebug() << "运行dealPdfSignal了" << endl;

    QWebData signalWebData;
    signalWebData.dataType = QWebData::toPdf;
    signalWebData.insertData(pdfPath);
    signalWebData.insertData(this->textWidget->toPlainText());
    QByteArray signalByteArray;
    QDataStream signalDataStream(&signalByteArray, QIODevice::ReadWrite);
    signalDataStream << signalWebData;
    tcpSocketPdf.write(signalByteArray);
    tcpSocketPdf.waitForBytesWritten();
}

void MainWidget::dealPdf()
{
    qDebug() << "运行dealPdf了" << endl;

    QDataStream inData(&(this->tcpSocketPdf));

    QWebData webData;
    inData >> webData;

    QString filePath = webData.data[0];

    QFile file(QString(webData.data[0]));
    file.open(QIODevice::ReadWrite | QFile::Truncate);

    QDataStream dataStream(&file);
    dataStream.writeRawData(webData.data[1], webData.data[1].size());
}
