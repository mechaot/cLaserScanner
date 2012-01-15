#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T14:58:03
#
#-------------------------------------------------

QT       += core gui

include(QtException/QtException.pri)
include(SiMaLi/SiMaLi.pri)

TARGET = cLaserScanner
TEMPLATE = app

INCLUDEPATH += . opencv opencv/opencv2 QtException SiMaLi
DEPENDPATH += . opencv opencv/opencv2 QtException SiMaLi


SOURCES += main.cpp\
        mainwindow.cpp \
    cameraWidget.cpp \
    centerDialog.cpp \
    heightmapwidget.cpp \
    cameraThread.cpp

HEADERS  += mainwindow.h \
    cameraWidget.h \
    centerDialog.h \
    heightmapwidget.h \
    cameraThread.h

FORMS    += mainwindow.ui \
    centerdialog.ui


LIBS += -L$$PWD/opencv/lib -lopencv_highgui220 -lopencv_core220
