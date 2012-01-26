#include <QtGui/QApplication>
#include "mainwindow.h"
#include "QtException.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DEBUG_INIT("debug.txt");
    DBG("Debugging");
    MainWindow w;
    w.show();
    int ret = a.exec();
    DEBUG_CLOSE();

    return ret;
}
