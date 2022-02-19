#include "qbuttonlist.h"

QButtonList::QButtonList()
{
    widget = new QWidget(this);
    widget->setFixedWidth(300);
    widget->setLayout(&vBoxLayout);
    vBoxLayout.setAlignment(Qt::AlignTop);
    vBoxLayout.setMargin(0);
    vBoxLayout.setSpacing(0);
    this->setWidget(widget);
    this->setWidgetResizable(true);
    this->initFile();
}

void QButtonList::addPushButtonFile(QPushButtonFile* button)
{
    vBoxLayout.addWidget(button);
}

void QButtonList::removePushButtonFile(QPushButtonFile* button)
{
    vBoxLayout.removeWidget(button);
}

void QButtonList::renamePushButtonFile(QPushButtonFile* button, QString& newName)
{
    button->rename(newName);
}

void QButtonList::initFile()
{

}

void QButtonList::initWeb()
{

}
