#include "CameraThread.h"
#include "QtException.h"
#include <opencv.hpp>
#include <QTime>


CameraThread::CameraThread(QObject *parent) :
    QThread(parent)
{
    m_iCamera = -1;
    m_cvCapture = NULL;
    m_camWidget = NULL;
    m_iplImage = NULL;
    m_iLiveViewMode = MODE_LIVE_PREPROCESSED;
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

    connect(this, SIGNAL(pointPosition(int,int)), m_camWidget, SLOT(tellLaserPos(int,int)));
    connect(m_camWidget, SIGNAL(roiChangedPoint(QRect)), this, SLOT(setRoiPoint(QRect)) );
    connect(m_camWidget, SIGNAL(roiChangedLine(QRect)), this, SLOT(setRoiLine(QRect)));
}

/**
  @brief    set live view mode
  @param    mode mode to apply

  determine which image should be sent to the widget
  **/
void CameraThread::setLiveViewMode(int mode)
{
    m_iLiveViewMode = mode;
}

/**
  @brief    slot to set roi for point
  **/
void CameraThread::setRoiPoint(const QRect &roi)
{
    m_roiPoint = roi;
}

/**
  @brief    slot to set roi for line
  **/
void CameraThread::setRoiLine(const QRect &roi)
{
    m_roiLine = roi;
}

/**
  @brief    set coordinates of one of the rois (in image coords)
  @param    roi     roi coordinates
  @param    roitype type of roi to set {ROI_TYPE_POINT, ROI_TYPE_LINE}
  **/
void CameraThread::setRoi(const QRect &roi, int roitype)
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

  ray: 5 pixels center; each wing 4 pix
**/
IplImage *CameraThread::evaluateImage(IplImage *img)
{

//    cv::Rect lRect( m_roiLine.left(), m_roiLine.top(), m_roiLine.right(), m_roiLine.bottom() );
    //    cv::Mat lineImage = cv::Mat(img, lRect);

    if (m_roiPoint.width() > 0) {   //takes about 1-2 ms for reasonable roi
        //QTime tic = QTime::currentTime();
        cv::Rect pRect( m_roiPoint.left(), m_roiPoint.top(), m_roiPoint.width(), m_roiPoint.height() );
        //DEBUG(1, QString("Evaluate Point %1 %2 %3 %4").arg(m_roiPoint.left()).arg(m_roiPoint.top()).arg(m_roiPoint.right()).arg(m_roiPoint.bottom()));

        cvSetImageROI(img,pRect);
        // sub-image
        IplImage *pointImage = cvCreateImage( cvSize(pRect.width, pRect.height), IPL_DEPTH_32F, 1 );
        //IplImage *temp = cvCreateImage( cvSize(pRect.width, pRect.height), IPL_DEPTH_32F, 1 );
        cvConvertScale(img,pointImage);
        cvResetImageROI(img); // release image ROI

        double min, max;
        CvPoint maxloc;
        cvSmooth( pointImage, pointImage, CV_GAUSSIAN, 31, 31);
        cvMinMaxLoc( pointImage, &min, &max, NULL, &maxloc);
        cvReleaseImage( &pointImage);
        //cvReleaseImage( &temp );
        //DEBUG(1, QString("Point: %1, %2").arg(maxloc.x).arg(maxloc.y));
        //QTime toc = QTime::currentTime();
        //DEBUG(1, QString("Took: %1 ms").arg( tic.msecsTo(toc)));

        emit pointPosition(maxloc.x + m_roiPoint.left(), maxloc.y + m_roiPoint.top());
    }
    if (m_roiLine.width() > 0) {
        cv::Rect lRect( m_roiLine.left(), m_roiLine.top(), m_roiLine.width(), m_roiLine.height() );
        //DEBUG(1, QString("Evaluate Point %1 %2 %3 %4").arg(m_roiLine.left()).arg(m_roiLine.top()).arg(m_roiLine.right()).arg(m_roiLine.bottom()));

        cvSetImageROI(img,lRect);
        // sub-image
        IplImage *lineImage = cvCreateImage( cvSize(lRect.width, lRect.height), IPL_DEPTH_32F, 1 );
        IplImage *temp = cvCreateImage( cvSize(lRect.width, lRect.height), IPL_DEPTH_8U, 1 );
        cvConvertScale(img,lineImage);
        cvResetImageROI(img);

        cvSmooth(lineImage, lineImage, CV_GAUSSIAN, 15,7);
        //cvCvtColor(m_iplImage, gray, CV_RGB2GRAY);
        //double lineFilterCoeffs[] = { -3, -2, -1, -1, 0, 1, 2, 5, 7, 11, 7, 5, 2, 1, 0, -1, -1, -2, -3};
        //CvMat lineFilter;
        //cvInitMatHeader( &lineFilter, 1, 19, CV_64FC1, lineFilterCoeffs);

        //cvFilter2D( lineImage ,lineImage, &lineFilter, cvPoint(-1,-1));
        //cvSmooth(grayF,grayF, CV_GAUSSIAN, 15, 1);

        double maxval;
        int   xpos;
        DEBUG(1, QString("Sizeof(float): %1").arg(sizeof(float)));
        for(int y = 0; y < lRect.height; y++) { //for every row search max
            float *data = (float*) (lineImage->imageData + y * lineImage->widthStep);
            maxval = *data;
            xpos = 0;
            for(int x = 1; x < lRect.width; x++)             {
                ++data;
                if (*data > maxval) {
                    maxval = *data;
                    xpos = x;
                }
            }

            cvDrawCircle(img, cvPoint(xpos,y), 3, cvScalar(0xff), 1);
        }


        cvReleaseImage( &temp );
        cvReleaseImage( &lineImage);
    }
    return img;
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

        IplImage* gray = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), m_iplImage->depth, 1);

        cvSplit(m_iplImage, NULL, NULL, gray, NULL);

        //cvConvertScale(g,g,0.2);
        //cvSub(gray, b, gray);
        IplImage *grayF = cvCreateImage(cvSize(gray->width, gray->height), IPL_DEPTH_32F, 1);

        cvConvertScale(gray,grayF,1.);
        //cvSmooth(grayF,grayF, CV_GAUSSIAN, 3, 3);

        grayF = evaluateImage(grayF);

        cvConvertScale(grayF,gray);
        IplImage* temp = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), m_iplImage->depth, 3);
        cvCvtColor(gray, temp, CV_GRAY2BGR);    //better send a (formally) color image to the camera widget


        if (m_camWidget) {
            if (MODE_LIVE_PREPROCESSED == m_iLiveViewMode) {
                m_camWidget->setImage(temp);
            } else if (MODE_LIVE_CAMERA == m_iLiveViewMode) {
                m_camWidget->setImage(m_iplImage);
            //else do nothing (MODE_LIVE_NONE == m_iLiveViewMode)
            }
        }



        //cvReleaseImage(&m_iplImage);  //this one is auto-cleared
        cvReleaseImage(&grayF);
        cvReleaseImage(&gray);
        cvReleaseImage(&temp); //this one needs to be released explicitely --> or else we had a memleak
        //delete [] lineFilterCoeffs;
    }
    DEBUG(10,"Exiting thread.");
}


