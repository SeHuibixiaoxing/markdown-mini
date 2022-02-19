#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QWebEngineView>
#include <QString>
#include "qpushbuttonfile.h"
#include "qfilelist.h"
#include "qbuttonlist.h"
#include "qwebdata.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <qmathdialog.h>

class QPushButtonFile;
class QButtonList;

class MainWidget : public QWidget
{
    Q_OBJECT
/*
 *网络部分
 */
public:
    QTcpServer tcpServerProfile;
    QTcpServer tcpServerMarkdown;
    QTcpServer tcpServerPdf;
    QTcpServer tcpServerAll;

    QTcpSocket tcpSocketProfile;
    QTcpSocket tcpSocketMarkdown;
    QTcpSocket tcpSocketPdf;
    QTcpSocket tcpSocketAll;

    QTimer *timer;

    void initWeb();
    void refreshTime();

public slots:
    void dealProfile();
    void dealMarkdown(QFileData fileData);
    void dealPdf();
    void dealAllSignal();
    void dealAll();
    void exportFileLocation();
    void dealPdfSignal(QString markdownPath);
/*
 *  UI部分
 */
private:
    //整个窗口布局
    QHBoxLayout* mainHBox;
    QWidget* leftWidget;
    QWidget* rightWidget;

    //左边文件浏览部分布局
    QVBoxLayout* leftVBox;
    QButtonList* buttonList;
    QWidget* pushButtonWidget;
    QHBoxLayout* pushButtonHBox;
    QSize* leftPushButtonSize;
    QPushButton* newPushButton;
    QPushButton* exportPushButton;
    QPushButton* renamePushButton;
    QPushButton* deletePushButton;

    //右边编辑及预览区布局
    QToolBar* toolBarWidget;
    QWidget* rightBottomWidget;
    QVBoxLayout* rightVBox;
    QHBoxLayout* rightBottomHBox;
    QPlainTextEdit* textWidget;
    QWebEngineView* webViewWidget;

public:
    static QString filePath;//point to folder
    static QString profilePath;//point to file
    static QString cssFilePath;//point to folder
    static QString htmlFilePath;//point to folder
    static QString pdfFilePath;//point to folder

    QString markdownToHtml(QString markdownName);
    QString markdownToPdf(QString markdownName);

    void saveProfile();
    void saveFile();
    void initUI();
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

signals:
    void bold();
    void italics();
    void strikethrough();
    void h1();
    void h2();
    void h3();
    void h4();
    void h5();
    void h6();
    void horizontalLine();
    void quote();
    void html();
    void link();
    void image();
    void code();
    void math();

public slots:
    void actionBold();
    void actionItalics();
    void actionStrikethrough();
    void actionH1();
    void actionH2();
    void actionH3();
    void actionH4();
    void actionH5();
    void actionH6();
    void actionHorizontalLine();
    void actionQuote();
    void actionHtml();
    void actionLink();
    void actionImage();
    void actionCode();
    void actionMath();

    void refreshFileView();
//本地操作：
    void newFileLocation();
    void renameFileLocation();
    void deleteFileLocation();
    void initFile();

 //服务器操作：


};
#endif // MAINWIDGET_H
