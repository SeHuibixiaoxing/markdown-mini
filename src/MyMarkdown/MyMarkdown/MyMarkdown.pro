QT       += core gui network
QT       += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwidget.cpp \
    qbuttonlist.cpp \
    qfiledata.cpp \
    qmathdialog.cpp \
    qpushbuttonfile.cpp \
    qwebdata.cpp

HEADERS += \
    mainwidget.h \
    qbuttonlist.h \
    qfiledata.h \
    qmathdialog.h \
    qpushbuttonfile.h \
    qwebdata.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    tohtml.py

INCLUDEPATH += -I /usr/include/python3.6
LIBS += -L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu/
