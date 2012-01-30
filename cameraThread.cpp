#include "cameraThread.h"
#include "QtException.h"
#include <opencv.hpp>
#include <QTime>
#include "settings.h"

#define USE_OPENMP      1       ///< use openmp multiprocessin library to speed up things

#if USE_OPENMP
    #include <omp.h>
#endif

CameraThread::CameraThread(QObject *parent) :
    QThread(parent)
{
    m_iCamera = -1;
    m_cvCapture = NULL;
    m_camWidget = NULL;
    m_iplImage = NULL;
    m_iLiveViewMode = MODE_LIVE_PREPROCESSED;
    m_posPoint.setX(-1); m_posPoint.setY(-1);
    m_bDigitizing = false;

    //channel 0: position of the maximum found (cvSplit to display this only)
    //channel 1: energy of the maximum found (0 for "copies"; i.e. we triple lines to fill spaces)
    //channel 2: zero
    //note: size is transposed with respect to camera resolution because laser scanner is vertical
    m_scanData = cvCreateImage( cvSize(CAMERA_RESOLUTION_Y, CAMERA_RESOLUTION_X), IPL_DEPTH_64F, 3 );

    cvZero(m_scanData);

    //load config file for calibration filenames
    loadInternalCalibration("internal.xml");
    loadExternalCalibration("external.xml");
}

/**
  @brief    cleaning up destructor
  **/
CameraThread::~CameraThread()
{
    if (m_bDigitizing)
        digitize(false);
    if (m_iplImage)
        cvReleaseImage(&m_iplImage);
}

/**
  @brief    tell the thread to terminate itself in a clean way
  **/
void CameraThread::sendTerminationRequest()
{
    m_bTerminationRequest = true;
}

/**
  @brief    note the telling name ;)
  @param    filename to load parameters from
  **/
void CameraThread::loadInternalCalibration(const QString& fileName)
{
    //m_camIntrinsics = cvLoad()
}

/**
  @brief    note the telling name ;)
  @param    filename to load parameters from
  **/
void CameraThread::loadExternalCalibration(const QString& fileName)
{

}

/**
  @brief    note the telling name ;)
  @param    filename to save parameters to
  **/
void CameraThread::saveInternalCalibration(const QString& fileName)
{

}

/**
  @brief    note the telling name ;)
  @param    filename to save parameters to
  **/
void CameraThread::saveExternalCalibration(const QString& fileName)
{

}


/**
  @brief    tell us if we shall digitize
  @parm     digi    do or not to do
  **/
void CameraThread::digitize(bool digi)
{
    m_bDigitizing = digi;
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
    if (m_bDigitizing) {
        EX_THROW("Don't change the ROI while digitizing!");
    }
    m_roiPoint = roi;
}

/**
  @brief    slot to set roi for line
  **/
void CameraThread::setRoiLine(const QRect &roi)
{
    if (m_bDigitizing) {
        EX_THROW("Don't change the ROI while digitizing!");
    }
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

  cool thing: parallelize some tasks on multiple cores


**/
IplImage *CameraThread::evaluateImage(IplImage *img)
{
    //QTime tic = QTime::currentTime();

    if (m_roiPoint.width() > 0) {   //takes about 1-2 ms for reasonable roi
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
        m_posPoint.setX(maxloc.x + m_roiPoint.left());
        m_posPoint.setY(maxloc.y + m_roiPoint.top());

        emit pointPosition(m_posPoint.x(), m_posPoint.y() );
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

        float maxval;
        int   xpos;

        for(int y = 0; y < lRect.height; y++) { //for every row search max
            float *data = (float*) (lineImage->imageData + y * lineImage->widthStep);
            maxval = *data;
            xpos = 0;
            for(int x = 1; x < lRect.width; x++) {
                ++data;
                if (*data > maxval) {
                    maxval = *data;
                    xpos = x;
                }
            }
            cvDrawCircle(img, cvPoint(xpos /*+ lRect.x*/,y+lRect.y), 2, cvScalar(0xaf), 1);

            if (m_posPoint.x() >= 0 && m_posPoint.x() < m_scanData->height && xpos >= 0 && xpos < m_scanData->width) {
                if(m_bDigitizing) {
                    double *data = (double*) (m_scanData->imageData + m_posPoint.x()*m_scanData->widthStep + img->nChannels*(y+lRect.height)*sizeof(double)); //
                    //if (maxval >= *(data+1)) {   //if stronger/better than old value; then overwrite it
                        *data =  xpos ; //todo: make it different from that
                        *(data+1) =  maxval;
                    //}
                    /*
                    //also do the line above, if applicable
                    data = (double*) (m_scanData->imageData + (m_posPoint.x()-1)*m_scanData->widthStep + y*sizeof(double)); //
                    if (m_posPoint.x() >= 1) { //if line above exists
                        if(*(data+1) != 0.0) {   //if line above is unset
                            *data = xpos;
                            *(data+1) = maxval;
                        }
                    }
                    //also do the line below if applicable
                    data = (double*) (m_scanData->imageData + (m_posPoint.x()+1)*m_scanData->widthStep + y*sizeof(double)); //
                    if ((m_posPoint.x()+1) <  m_scanData->height) { //if line below exists
                        if(*(data+1) != 0.0) {   //if line below is unset
                            *data = xpos;
                            *(data+1) = maxval;
                        }
                    }
                    */
                }
                //*(data+2) = 0;

                //points.append( QPointF(xpos,y) );
            }
        }
        if (m_bDigitizing) {
            emit newScanData();
        }

        cvReleaseImage( &temp );
        cvReleaseImage( &lineImage);

        //emit linePosition(points);
    }
    //QTime toc = QTime::currentTime();
    //DEBUG(1, QString("Took: %1 ms").arg( tic.msecsTo(toc)));
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


        //cvConvertScale(g,g,0.2);
        //cvSub(gray, b, gray);

        //cvSmooth(grayF,grayF, CV_GAUSSIAN, 3, 3);

        if ((m_iLiveViewMode == MODE_LIVE_CHESSBOARD) || (m_iLiveViewMode == MODE_LIVE_CHESSBOARD_SAVE)) {
            int corner_count;
            CvPoint2D32f* corners = new CvPoint2D32f[ CALIBRATION_CHESSBOARD_WIDTH *  CALIBRATION_CHESSBOARD_HEIGHT];
            int found = cvFindChessboardCorners(m_iplImage, cvSize(CALIBRATION_CHESSBOARD_WIDTH, CALIBRATION_CHESSBOARD_HEIGHT), corners, &corner_count);
            if (found) {
                // Get subpixel accuracy on those corners
                cvCvtColor( m_iplImage, gray, CV_BGR2GRAY );
                cvFindCornerSubPix( gray, corners, corner_count, cvSize( 5, 5 ),
                    cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));


                if (m_iLiveViewMode == MODE_LIVE_CHESSBOARD_SAVE) { //save request; save it and go to normal mode
                    //cvSave( "Chessbaord.xml", external_matrix);
                    DEBUG(1, "External Calibration Saved");
                    m_iLiveViewMode = MODE_LIVE_CHESSBOARD;
                }
            }

            // Draw it
            cvDrawChessboardCorners( m_iplImage, cvSize(CALIBRATION_CHESSBOARD_WIDTH, CALIBRATION_CHESSBOARD_HEIGHT), corners, corner_count, found );
            delete [] corners;

            m_camWidget->setImage(m_iplImage);
        } else {
            cvSplit(m_iplImage, NULL, NULL, gray, NULL);
            IplImage *grayF = cvCreateImage(cvSize(gray->width, gray->height), IPL_DEPTH_32F, 1);
            cvConvertScale(gray,grayF,1.);
            grayF = evaluateImage(grayF);

            cvConvertScale(grayF,gray);

            if (m_camWidget) {
                if (MODE_LIVE_PREPROCESSED == m_iLiveViewMode) {
                    IplImage* temp = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), m_iplImage->depth, 3);
                    cvCvtColor(gray, temp, CV_GRAY2BGR);    //better send a (formally) color image to the camera widget
                    m_camWidget->setImage(temp);
                    cvReleaseImage(&temp);
                } else if (MODE_LIVE_CAMERA == m_iLiveViewMode) {
                    m_camWidget->setImage(m_iplImage);
                //else do nothing (MODE_LIVE_NONE == m_iLiveViewMode)
                }
            }
            cvReleaseImage(&grayF);
        }

        //cvReleaseImage(&m_iplImage);  //this one is auto-cleared
        cvReleaseImage(&gray);

        //delete [] lineFilterCoeffs;
    }
    DEBUG(10,"Exiting thread.");
}


