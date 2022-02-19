#include "qautosave.h"

QAutoSave::QAutoSave(MainWidget* mainWidget, QObject *parent) : QObject(parent)
{
    this->widget = mainWidget;
}

QAutoSave::~QAutoSave()
{
}


