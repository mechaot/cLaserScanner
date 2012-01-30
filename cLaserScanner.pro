#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T14:58:03
#
#-------------------------------------------------

QT       += core gui

include(QtException/QtException.pri)

TARGET = cLaserScanner
TEMPLATE = app

INCLUDEPATH += . opencv opencv/opencv2  QtException
DEPENDPATH += . opencv opencv/opencv2  QtException

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
    cameraThread.h \
    settings.h

FORMS    += mainwindow.ui \
    centerdialog.ui

win32 {
    QMAKE_CXXFLAGS += -m32
    LIBS += -L$$PWD/opencv/lib -lopencv_highgui231 -lopencv_core231 -lopencv_imgproc231 -lopencv_calib3d231 -Lomp -lgomp
}

unix {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -L/usr/local/lib -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_calib3d -Lomp -lgomp
}


