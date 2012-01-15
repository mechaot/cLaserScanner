#include <QtGui/QApplication>
#include "QtException.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    DEBUG_INIT("debug.txt");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret = a.exec();

    DEBUG_CLOSE();

    return ret;
}
