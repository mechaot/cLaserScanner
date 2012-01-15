#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T14:58:03
#
#-------------------------------------------------

QT       += core gui

TARGET = cLaserScanner
TEMPLATE = app

INCLUDEPATH += . opencv opencv/opencv2 QtException SiMaLi
DEPENDPATH += . opencv opencv/opencv2 QtException SiMaLi

include(QtException/QtException.pri)
include(SiMaLi/SiMaLi.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    cameraWidget.cpp \
    centerDialog.cpp \
    heightmapwidget.cpp

HEADERS  += mainwindow.h \
    cameraWidget.h \
    centerDialog.h \
    heightmapwidget.h

FORMS    += mainwindow.ui \
    centerdialog.ui


LIBS += -L$$PWD/opencv/lib -lopencv_highgui220 -lopencv_core220
