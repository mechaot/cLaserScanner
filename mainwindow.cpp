#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centerDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CenterDialog *dlg = new CenterDialog(this);
    setCentralWidget( dlg );
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}


