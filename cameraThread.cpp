#include "CameraThread.h"
#include "QtException.h"
#include <opencv.hpp>


CameraThread::CameraThread(QObject *parent) :
    QThread(parent)
{
    m_iCamera = -1;
    m_cvCapture = NULL;
    m_camWidget = NULL;
    m_iplImage = NULL;
}

/**
  @brief    tell the thread to terminate itself in a clean way
  **/
void CameraThread::sendTerminationRequest()
{
    m_bTerminationRequest = true;
}


/**
  @brief set mode of operation
  @param    mode mode to be set

  unchecked
  **/
void CameraThread::setModeOfOperation(int mode)
{
    m_iMode = mode;
}

/**
  @brief    get mode of operation
  @return   mode of operation
  **/
int CameraThread::modeOfOperation()
{
    return m_iMode;
}

/**
  @brief    get selected camera
  @param[in]    cvIndex     numeric index of cv camera
  @param[in]    cvCapture   pointer to connected camera
  **/
void CameraThread::setCvCamera(int cvIndex, CvCapture *cvCapture)
{
    m_iCamera = cvIndex;
    m_cvCapture = cvCapture;
}

/**
  @brief    set the widget for output
  @param    target  widget of type CameraWidget to take the generated output
  **/
void CameraThread::setCameraWidget(CameraWidget *target)
{
    m_camWidget = target;

    //fetch roi
    m_roiLine = m_camWidget->roi(ROI_TYPE_LINE);
    m_roiPoint = m_camWidget->roi(ROI_TYPE_POINT);
}

/**
  @brief    set coordinates of one of the rois (in image coords)
  @param    roi     roi coordinates
  @param    roitype type of roi to set {ROI_TYPE_POINT, ROI_TYPE_LINE}
  **/
void CameraThread::setRoi(QRect &roi, int roitype)
{
    if (roitype == ROI_TYPE_LINE) {
        m_roiLine = roi;
    } else if (roitype == ROI_TYPE_POINT) {
        m_roiPoint = roi;
    } else {
        qDebug("Invalid roi type");
    }
    //if  (m_camWidget) { //forward roi
    //    m_camWidget->setRoi(roi, roitype);
    //}
}


/**
  @brief    heart 1 of the laser scanner: extract point and line
  @param    img     image to process

  @todo parallelize some tasks on multiple cores
**/
int CameraThread::evaluateImage(IplImage *img)
{

}

/**
  @brief    thread's main routine
  **/
void CameraThread::run()
{
    m_bTerminationRequest = false;  //initially we don't want to kill us
    while (!m_bTerminationRequest) {
        if(!m_cvCapture)     {
            DEBUG(10, "m_cvCapture not ready. Terminating thread.");
            break;
        }

        m_iplImage = cvQueryFrame(m_cvCapture);
        if(m_iplImage->width * m_iplImage->height < 1) {
            DEBUG(20, "image invalid, retrying");
            continue;
        }

        IplImage* hsv = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), m_iplImage->depth, 3);
        cvCvtColor(m_iplImage, hsv, CV_BGR2HSV);

        //if ( (m_iMode & MODE_IMAGE_OUT) && (m_camWidget) ) {
        if (m_camWidget) {
            m_camWidget->setImage(hsv);
        }

        evaluateImage(hsv);

        //cvReleaseImage(&m_iplImage);  //this one is auto-cleared
        cvReleaseImage(&hsv); //this one needs to be released explicitely --> or else we had a memleak
    }
    DEBUG(10,"Exiting thread.");
}
