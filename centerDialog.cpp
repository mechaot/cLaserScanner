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
        }
    }
    if (ui->comboCameras->count() < 1) {
        ui->comboCameras->addItem("No camera found!");
        ui->comboCameras->setEnabled(false);
        ui->buttonCamera->setEnabled(false);
    } else { //cams found, activate the connect button
        ui->buttonCamera->setEnabled(true);
    }
}

/**
  @brief    try to connect to the selected opencv camera

  we try to set the full-HD resolution
  **/
void CenterDialog::connectCamera(void)
{
    QString s(ui->comboCameras->currentText());
    if (s.startsWith("cv: ")) {
        s = s.remove(0,4);
        bool ok;
        int camIdx = s.toInt(&ok);
        if (!ok) {
            QMessageBox::critical(this,"Camera connect failed", "Could not read selected camera");
        }
        m_iCamera = camIdx;
        m_cvCapture = cvCaptureFromCAM(m_iCamera);

        cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_WIDTH, 1920);
        cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_HEIGHT, 1080);
    }
}









#endif // CENTERDIALOG_CPP
