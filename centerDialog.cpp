#ifndef CENTERDIALOG_CPP
#define CENTERDIALOG_CPP

#include "centerDialog.h"
//#include "ui_centerdialog.h"
#include <QtGui>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "QtException.h"

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
    connect(ui->comboLiveViewMode, SIGNAL(currentIndexChanged (QString)), this, SLOT(setLiveMode(QString)));
    connect(ui->radioZoomFit, SIGNAL(clicked()), this, SLOT(setZoomMode()));
    connect(ui->radioZoom100, SIGNAL(clicked()), this, SLOT(setZoomMode()));
    connect(ui->cameraWidget, SIGNAL(tellMousePosition(int,int)), this, SLOT(displayCursorPosition(int,int)));
    connect(ui->buttonClearCursor, SIGNAL(clicked()), ui->cameraWidget, SLOT(clearCursor()));
    connect(ui->buttonRoiLine, SIGNAL(clicked()), ui->cameraWidget, SLOT(startSettingRoiLine()));
    connect(ui->buttonRoiPoint, SIGNAL(clicked()), ui->cameraWidget, SLOT(startSettingRoiPoint()));
    connect(ui->cameraWidget, SIGNAL(roiChangedPoint(QRect)), this, SLOT(displayRoiPointCoords(QRect)));
    connect(ui->cameraWidget, SIGNAL(roiChangedLine(QRect)), this, SLOT(displayRoiLineCoords(QRect)));
    //ui->scrollCameraWidget->setWidget(ui->cameraWidget);

    //ui->cameraWidget->setAutoFillBackground(false);
    //ui->cameraWidget->show();
    //ui->scrollCameraWidget->setLayout(ui->layoutCameraWidget);
    findCameras();
    connectCamera(true);
    setZoomMode();
    ui->buttonCamera->setChecked(true);
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
    ui->comboCameras->setEnabled(true);
    ui->comboCameras->clear();

    for(int i = 0; i < MAX_OPENCV_CAMERA; i++) {
        CvCapture *capture = cvCaptureFromCAM(i);
        if (capture) {
            IplImage *dummy = cvQueryFrame(capture);
            if ((dummy) && (dummy->height * dummy->width > 1)) { //means: image is ok
                ui->comboCameras->addItem( QString("cv: %1").arg(i) );
            }
            cvReleaseCapture(&capture);
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

            cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_WIDTH, 1920);
            cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_HEIGHT, 1080);

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
            setLiveMode(ui->comboLiveViewMode->currentText());
            QTimer::singleShot(500, this,  SLOT(setZoomMode()));   //resize when camera thread is running; bad habit workaround
            m_threadCam->start();

            this->connect(m_threadCam, SIGNAL(pointPosition(int,int)), this, SLOT(displayPointPosition(int,int)));

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

/**
  @brief    slot to set live mode combo box
  @param    combo box string content

  Propagate setting
  **/
void CenterDialog::setLiveMode(const QString &mode)
{
    if(m_threadCam) {
        if(0 == mode.compare("Preprocessed Image", Qt::CaseInsensitive)) {
            m_threadCam->setLiveViewMode(MODE_LIVE_PREPROCESSED);
        } else if (0 == mode.compare("Camera Image", Qt::CaseInsensitive)) {
            m_threadCam->setLiveViewMode(MODE_LIVE_CAMERA);
        } else {
            m_threadCam->setLiveViewMode(MODE_LIVE_NONE);
        }
    }
}

/**
  @brief    set zoom mode of camera widget

  Reads user settings from gui
  **/
void CenterDialog::setZoomMode()
{
    if (ui->radioZoomFit->isChecked()) {
        QRect geo = ui->scrollCameraWidget->geometry();
        ui->scrollCameraWidget->setWidgetResizable(true);
        geo.moveTo(0,0);
        //ui->cameraWidget->setMinimumSize(geo.size());
        ui->cameraWidget->setGeometry(geo);
        ui->scrollCameraWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        ui->scrollCameraWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    } else {
        //DBG(QString("ScrollAreaWidget %1").arg((int) ui->scrollCameraWidget->widget()));

        QRect geo = ui->scrollCameraWidget->geometry();
        ui->scrollCameraWidget->setWidgetResizable(false);

        geo.moveTo(0,0);
        geo.setWidth( 1920 );
        geo.setHeight( 1080 );
        ui->cameraWidget->setGeometry(geo);
        ui->cameraWidget->setMinimumSize(geo.size());

        //ui->scrollCameraWidget->horizontalScrollBar()->setRange(0, 1920 - ui->scrollCameraWidget->geometry().width());
        ui->scrollCameraWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui->scrollCameraWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        ui->scrollCameraWidget->viewport()->setGeometry(geo);
        ui->scrollCameraWidget->viewport()->updateGeometry();
        ui->scrollCameraWidget->viewport()->update();
    }
    update();
}

/**
   @brief   display cursor position to gui
   @param   x x position
   @param   y y position
  **/
void CenterDialog::displayCursorPosition(int x, int y)
{
    if (x < 0 || y < 0) {
        ui->labelCursorPos->setText("-, -");
    } else  {
        QString s = QString("%1, %2").arg(x).arg(y);
        ui->labelCursorPos->setText(s);
    }
}

/**
  @brief    display laser line position
  @param    x       x coordinate
  @param    y       y coordinate
  **/
void CenterDialog::displayPointPosition(int x, int y)
{
    ui->labelLaserPoint->setText( QString("%1, %2").arg(x).arg(y) );
}

/**
  @param        display coords within text label
  @param    rect coords
  **/
void CenterDialog::displayRoiPointCoords(const QRect &rect)
{
    ui->labelRoiPoint->setText(QString("[%1, %2 ... %3, %4]").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom()));
}

/**
  @param        display coords within text label
  @param    rect coords
  **/
void CenterDialog::displayRoiLineCoords(const QRect &rect)
{
    ui->labelRoiLine->setText(QString("[%1, %2 ... %3, %4]").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom()));
}


#endif // CENTERDIALOG_CPP
