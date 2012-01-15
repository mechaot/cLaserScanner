#ifndef CENTERDIALOG_CPP
#define CENTERDIALOG_CPP

#include "centerDialog.h"
#include "ui_centerdialog.h"
#include <QtGui>

CenterDialog::CenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::centerDialog)
{
    m_cvCapture = NULL;         //initialize: nothing connected
    m_iCamera = -1;             //invalid --> no cam connected
    ui->setupUi(this);
    setLayout(ui->pageLayout);
    //QMessageBox::critical(this, "CenterDialog", "Testpoint");
    //connect(ui->buttonCamera, SIGNAL(clicked()), this, SLOT(connectCamera));
    connect(ui->buttonCameraFind, SIGNAL(clicked()), this, SLOT(findCameras()));
}


/**
  @brief    destructur  clean up
  **/
CenterDialog::~CenterDialog()
{
    delete ui;
}

/**
  @brief    try to detect opencv cameras
  **/
void CenterDialog::findCameras(void)
{
    ui->comboCameras->clear();
    ui->comboCameras->setEnabled(true);

    for(int i = 0; i < MAX_OPENCV_CAMERA; i++) {
        CvCapture *capture = cvCaptureFromCAM(i);
        if (capture) {
            IplImage *dummy = cvQueryFrame(capture);
            if (dummy->height * dummy->width > 1) { //means: image is ok
                ui->comboCameras->addItem( QString("cv: %1").arg(i) );
            }
        } else {
            ui->comboCameras->addItem("OpenCV error");
            ui->comboCameras->setEnabled(false);
        }
    }
    if (ui->comboCameras->count() < 1) {
        ui->comboCameras->addItem("No camera found!");
        ui->comboCameras->setEnabled(false);
    }
}


#endif // CENTERDIALOG_CPP
