#ifndef CENTERDIALOG_CPP
#define CENTERDIALOG_CPP

#include "centerDialog.h"
#include "ui_centerdialog.h"
#include <QtGui>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

CenterDialog::CenterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::centerDialog)
{
    m_cvCapture = NULL;         //initialize: nothing connected
    m_threadCam = NULL;         //no camera thread yet
    m_iCamera = -1;             //invalid --> no cam connected
    ui->setupUi(this);
    setLayout(ui->pageLayout);
    //QMessageBox::critical(this, "CenterDialog", "Testpoint");
    ui->buttonCamera->setEnabled(false);
    connect(ui->buttonCamera, SIGNAL(clicked(bool)), this, SLOT(connectCamera(bool)));
    connect(ui->buttonCameraFind, SIGNAL(clicked()), this, SLOT(findCameras()));

    findCameras();
}


/**
  @brief    destructur  clean up
  **/
CenterDialog::~CenterDialog()
{
    connectCamera(false);
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
            if ((dummy) && (dummy->height * dummy->width > 1)) { //means: image is ok
                ui->comboCameras->addItem( QString("cv: %1").arg(i) );
            }
            cvReleaseCapture(&capture);
        }
        ui->comboCameras->addItem("cv: 0");
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
void CenterDialog::connectCamera(bool connect)
{
    if (connect) {
        QString s(ui->comboCameras->currentText());
        if (s.startsWith("cv: ")) {
            s = s.remove(0,4);
            bool ok;
            int camIdx = s.toInt(&ok);
            if (!ok) {
                QMessageBox::critical(this,"Camera connect failed", "Could not read selected camera. Trying first one!");
            }
            m_iCamera = camIdx;
            //m_cvCapture = cvCaptureFromCAM(m_iCamera);
            //m_cvCapture = cvCreateCameraCapture(-1);
            m_cvCapture = cvCreateCameraCapture(camIdx);

            //cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_WIDTH, 1920);
            //cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_HEIGHT, 1080);

            double w = cvGetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_WIDTH);
            double h = cvGetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_HEIGHT);

            QString s = QString("Cam %3: %1 x %2").arg(w).arg(h).arg(m_iCamera);
            ui->labelCamProps->setText(s);

            IplImage *m_iplImage = cvQueryFrame(m_cvCapture);
            ui->cameraWidget->setImage(m_iplImage);
            ui->cameraWidget->update();

           // return;
            if (m_threadCam ) {
                DEBUG(10, "Warning: deleting a running cameraThread");
                m_threadCam->sendTerminationRequest();
                m_threadCam->wait(10000);
                if (!m_threadCam->isFinished()) {
                    m_threadCam->terminate();
                }
                delete m_threadCam;
                m_threadCam = 0;
            }

            m_threadCam = new CameraThread(this);
            m_threadCam->setCameraWidget(ui->cameraWidget);
            m_threadCam->setCvCamera(m_iCamera, m_cvCapture);
            m_threadCam->start();
        } else {
            QMessageBox::critical(this,"Camera connect failed", "Could not get camera ID. Try rescanning for cameras.");
        }
   } else { // disconnect
        if (m_threadCam ) {
            m_threadCam->sendTerminationRequest();
            m_threadCam->wait(10000);
            if (!m_threadCam->isFinished()) {
                m_threadCam->terminate();
                DEBUG(10, "Warning camera didn't want to terminate itself. Thread killed.");
            }
            delete m_threadCam;
            m_threadCam = 0;
        }
        if (m_cvCapture) {
            cvReleaseCapture(&m_cvCapture);
            m_cvCapture = 0;
        }
        if (m_iCamera >= 0) {
            m_iCamera = -1;
        }
    }
}









#endif // CENTERDIALOG_CPP
