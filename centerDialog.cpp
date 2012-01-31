#ifndef CENTERDIALOG_CPP
#define CENTERDIALOG_CPP

#include "centerDialog.h"
//#include "ui_centerdialog.h"
#include <QtGui>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "QtException.h"
#include "settings.h"

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
    connect(ui->cameraWidget, SIGNAL(finishedSettingRoi(bool)), ui->buttonRoiLine, SLOT(setChecked(bool)));
    connect(ui->cameraWidget, SIGNAL(finishedSettingRoi(bool)), ui->buttonRoiPoint, SLOT(setChecked(bool)));
    connect(ui->buttonDigitize, SIGNAL(toggled(bool)), this, SLOT(digitize(bool)));
    connect(ui->buttonCalibrateRt, SIGNAL(clicked()), this, SLOT(calibrateExternalParameters()));
    connect(ui->sliderLinePowerThreshold, SIGNAL(valueChanged(int)), ui->spinLinePowerThreshold, SLOT(setValue(int)) );
    connect(ui->spinLinePowerThreshold, SIGNAL(valueChanged(int)), ui->sliderLinePowerThreshold, SLOT(setValue(int)));
    ui->spinLinePowerThreshold->setValue(ui->sliderLinePowerThreshold->value());
    connect(ui->sliderPointPowerThreshold, SIGNAL(valueChanged(int)), ui->spinPointPowerThreshold, SLOT(setValue(int)) );
    connect(ui->spinPointPowerThreshold, SIGNAL(valueChanged(int)), ui->sliderPointPowerThreshold, SLOT(setValue(int)));
    ui->spinPointPowerThreshold->setValue(ui->sliderPointPowerThreshold->value());

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

  We are evil ;-) and catch any exception without interaction; --> just make the app to shut up
  **/
CenterDialog::~CenterDialog()
{
    try {
        connectCamera(false);
        delete ui;
    } catch(std::exception e) {
        DEBUG(1,QString("App Closing caused exception (%1). Catch and shut up!").arg(e.what()));
    }
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

            cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_WIDTH, CAMERA_RESOLUTION_X);
            cvSetCaptureProperty(m_cvCapture, CV_CAP_PROP_FRAME_HEIGHT, CAMERA_RESOLUTION_Y);

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
            this->connect(m_threadCam, SIGNAL(newScanData()), this, SLOT(updateHeightmapWidget()));
            this->connect(ui->buttonHeightmapClear, SIGNAL(clicked()), m_threadCam, SLOT(clearHeightmap()));
            this->connect(ui->button3D, SIGNAL(clicked()), m_threadCam, SLOT(triangulatePointCloud()));
            this->connect(ui->sliderLinePowerThreshold, SIGNAL(valueChanged(int)), m_threadCam, SLOT(setPowerThresholdLine(int)));

            m_threadCam->setPowerThresholdLine(ui->sliderLinePowerThreshold->value());
            this->connect(ui->sliderPointPowerThreshold, SIGNAL(valueChanged(int)), m_threadCam, SLOT(setPowerThresholdPoint(int)));
            m_threadCam->setPowerThresholdPoint(ui->sliderPointPowerThreshold->value());

            m_threadCam->setScaleX(ui->spinScaleX->value());
            this->connect(ui->spinScaleX, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setScaleX(double)));
            m_threadCam->setScaleY(ui->spinScaleY->value());
            this->connect(ui->spinScaleY, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setScaleY(double)));
            m_threadCam->setScaleZ(ui->spinScaleZ->value());
            this->connect(ui->spinScaleZ, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setScaleZ(double)));

            m_threadCam->setOffsetX(ui->spinOffsetX->value());
            this->connect(ui->spinOffsetX, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setOffsetX(double)));
            m_threadCam->setOffsetY(ui->spinOffsetY->value());
            this->connect(ui->spinOffsetY, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setOffsetY(double)));
            m_threadCam->setOffsetZ(ui->spinOffsetZ->value());
            this->connect(ui->spinOffsetZ, SIGNAL(valueChanged(double)), m_threadCam, SLOT(setOffsetZ(double)));
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
        ui->buttonCalibrateRt->setEnabled(false); //only enable for chessboard mode
        if(0 == mode.compare("Preprocessed Image", Qt::CaseInsensitive)) {
            m_threadCam->setLiveViewMode(MODE_LIVE_PREPROCESSED);
        } else if (0 == mode.compare("Camera Image", Qt::CaseInsensitive)) {
            m_threadCam->setLiveViewMode(MODE_LIVE_CAMERA);
        } else if (0 == mode.compare("Chessboard Detection", Qt::CaseInsensitive)) {
            m_threadCam->setLiveViewMode(MODE_LIVE_CHESSBOARD);
            ui->buttonCalibrateRt->setEnabled(true);
        } else {
            m_threadCam->setLiveViewMode(MODE_LIVE_NONE);
        }
    }
}


/**
  @brief    request external calibratio

  just update state machinge of camera thread/inform the thread
  **/
void CenterDialog::calibrateExternalParameters()
{
    if(m_threadCam) {
        m_threadCam->setLiveViewMode(MODE_LIVE_CHESSBOARD_SAVE);

        QProgressDialog progress("Calibrating external parameters from checkerboard.", "Abort", 0, -1, this);
        progress.setMinimumDuration(1);
        progress.setValue(0);
        while(m_threadCam->liveViewMode() == MODE_LIVE_CHESSBOARD_SAVE) {
            qApp->processEvents();
            if (progress.wasCanceled()) {
                break;
            }
        }
        if (m_threadCam->liveViewMode() == MODE_LIVE_CHESSBOARD)
            QMessageBox::information(this, "External Calibration","Parameters successfully saved.");
        else
            QMessageBox::warning(this, "External Calibration","External calibration failed!");
    }
}

/**
  @brief tell the heightmap widget to reload its content from image processing thread

  **/
void CenterDialog::updateHeightmapWidget()
{
    if (m_threadCam) {
        ui->heightmapWidget->setImage(m_threadCam->m_scanData);
        ui->heightmapWidget->update();
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
        geo.setWidth( CAMERA_RESOLUTION_X );
        geo.setHeight( CAMERA_RESOLUTION_Y );
        ui->cameraWidget->setGeometry(geo);
        ui->cameraWidget->setMinimumSize(geo.size());

        //ui->scrollCameraWidget->horizontalScrollBar()->setRange(0, CAMERA_RESOLUTION_X - ui->scrollCameraWidget->geometry().width());
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
  @brief        display coords within text label
  @param    rect coords
  **/
void CenterDialog::displayRoiPointCoords(const QRect &rect)
{
    ui->labelRoiPoint->setText(QString("[%1, %2 ... %3, %4]").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom()));
}

/**
  @brief display coords within text label
  @param    rect coords
  **/
void CenterDialog::displayRoiLineCoords(const QRect &rect)
{
    ui->labelRoiLine->setText(QString("[%1, %2 ... %3, %4]").arg(rect.left()).arg(rect.top()).arg(rect.right()).arg(rect.bottom()));
}

/**
  @brief    start/stop digitizing 3D
  @param    digi    true: start; false: stop
  **/
void CenterDialog::digitize(bool digi)
{
    ui->buttonRoiLine->setDisabled(digi);
    ui->buttonRoiPoint->setDisabled(digi);
    if (m_threadCam)
        m_threadCam->digitize(digi);
}


#endif // CENTERDIALOG_CPP
