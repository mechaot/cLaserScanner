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

INCLUDEPATH += . opencv opencv/opencv2 omp QtException SiMaLi
DEPENDPATH += . opencv opencv/opencv2 omp QtException SiMaLi


SOURCES += main.cpp\
        mainwindow.cpp \
    cameraWidget.cpp \
    centerDialog.cpp \
    heightmapwidget.cpp \
    CameraThread.cpp

HEADERS  += mainwindow.h \
    cameraWidget.h \
    centerDialog.h \
    heightmapwidget.h \
    CameraThread.h

FORMS    += mainwindow.ui \
    centerdialog.ui

QMAKE_CXXFLAGS += -m32 -fopenmp


LIBS += -L$$PWD/opencv/lib -lopencv_highgui231 -lopencv_core231 -lopencv_imgproc231 -Lomp -lgomp


