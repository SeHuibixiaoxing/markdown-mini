#ifndef QBUTTONLIST_H
#define QBUTTONLIST_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include "qpushbuttonfile.h"
#include <QString>
#include <QWidget>
#include "mainwidget.h"

class QPushButtonFile;
class MainWidget;

class QButtonList : public QScrollArea
{
private:
    QVBoxLayout vBoxLayout;
    QWidget* widget;
    void initFile();
    void initWeb();
public:
    QButtonList();
    void addPushButtonFile(QPushButtonFile*);
    void removePushButtonFile(QPushButtonFile*);
    void renamePushButtonFile(QPushButtonFile*, QString&);
};

#endif // QBUTTONLIST_H
