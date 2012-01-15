# -------------------------------------------------
# Project created by QtCreator 2010-02-10T10:38:52
# -------------------------------------------------
QT += core
TARGET = SiMaLi

#include(../opencv.pri)


DEPENDPATH += ../QtException
INCLUDEPATH += ../QtException
#include (../QtException/QtException.pri)

DEPENDPATH += ./SiMaLi
INCLUDEPATH += ./SiMaLi

TEMPLATE = lib
DEPENDPATH += . # #without this line including this file from Fringer.pro would lead to unknown file-error

HEADERS += matrix.h \
    matrix_operators.hpp \
    matrix.hpp \
    matrix_qtascii.hpp \
    matrix_raytracer.hpp \
    matrix_statics.hpp \
    matrix_bv.hpp \
    matrix_lapack.hpp


INCLUDEPATH += ../Raytracer

#lapack usage
DEPENDPATH += ./SiMaLi/lapack/include
INCLUDEPATH += ./SiMaLi/lapack/include
HEADERS +=    lapack/include/f2c.h \
            lapack/include/clapack.h

win32 {
LIBS += $$PWD/lapack/lib/lapack.a \
        $$PWD/lapack/lib/libgfortran.a \
        $$PWD/lapack/lib/tmglib.a \
        $$PWD/lapack/lib/blas.a
#    LIBS += -L$$PWD/lapack/lib -llapack -lgfortran -ltmg -lblas
}

unix {
    LIBS += -llapack -lgfortran  -lblas
}
#end lapack-things

win32 {
  QMAKE_CXXFLAGS += -Wno-deprecated \
    -O3  \
    -mfpmath=sse \
    -march=native  \ # optimize code for exactly the machine we're compiling on; code may not run on different machines; all thee give approx factor 4 rendering speed
    -mno-pe-aligned-commons #bugfix with old compiler and lapack
}

unix {
  QMAKE_CXXFLAGS += -Wno-deprecated \
    -O3  \
    -mfpmath=sse \
    -march=native   # optimize code for exactly the machine we're compiling on; code may not run on different machines; all thee give approx factor 4 rendering speed
}