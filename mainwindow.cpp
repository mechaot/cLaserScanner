#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centerDialog.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CenterDialog *dlg = new CenterDialog();
    if (!dlg) {
        QMessageBox::critical(this, "Error on inititalization", "The central dialog could not be constructed");
    }
    setCentralWidget( dlg );

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
  @brief    show the common about/info dialog for this application
  **/
void MainWindow::about()
{
    QMessageBox::information(this, "About cLaserScanner", "<html><h1>cLaserScanner 0.01</h1><br/>(c)2012 <a href='http://www.imr.uni-hannover.de'>Institut f&uuml;r Mess und Regelungstechnik</a> der <a href='http://www.uni-hannover.de'>Leibniz Universit&auml; Hannover</a></html>");
}
