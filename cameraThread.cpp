#include "cameraThread.h"
#include "QtException.h"
#include <opencv.hpp>
#include <QTime>
#include "settings.h"
#include <QMessageBox>
#include <QApplication>

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
    m_iLinePowerThreshold = 0;
    m_iPointPowerThreshold = 0;
    //channel 0: position of the maximum found (cvSplit to display this only)
    //channel 1: energy of the maximum found (0 for "copies"; i.e. we triple lines to fill spaces)
    //channel 2: zero
    //note: size is transposed with respect to camera resolution because laser scanner is vertical
    m_scanData = NULL;
    m_pointCloud = NULL;

    m_dScaleX = 1.;
    m_dScaleY = 1.;
    m_dScaleZ = 2.5;
    m_dOffsetX = 0;
    m_dOffsetY = 0;
    m_dOffsetZ = 0;

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
    if (m_scanData)
        cvReleaseImage(&m_scanData);
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
  @brief    set scale for triangulation
  @param    scale new scale factor
  **/
void CameraThread::setScaleX(double scale)
{
    if (scale <= 0.0) {
        DEBUG(1, "Warning: nonpositive scale factor!");
    }
    m_dScaleX = scale;
}

/**
  @brief    set scale for triangulation
  @param    scale new scale factor
  **/
void CameraThread::setScaleY(double scale)
{
    if (scale <= 0.0) {
        DEBUG(1, "Warning: nonpositive scale factor!");
    }
    m_dScaleY = scale;
}

/**
  @brief    set scale for triangulation
  @param    scale new scale factor
  **/
void CameraThread::setScaleZ(double scale)
{
    if (scale <= 0.0) {
        DEBUG(1, "Warning: nonpositive scale factor!");
    }
    m_dScaleZ = scale;
}


/**
  @brief    set offset for triangulation
  @param    scale new offset
  **/
void CameraThread::setOffsetX(double offset)
{
    m_dOffsetX = offset;
}

/**
  @brief    set scaoffsetle for triangulation
  @param    scale new offset
  **/
void CameraThread::setOffsetY(double offset)
{
    m_dOffsetY = offset;
}

/**
  @brief    set offset for triangulation
  @param    scale new offset
  **/
void CameraThread::setOffsetZ(double offset)
{
    m_dOffsetZ = offset;
}

/**
  @brief    tell us if we shall digitize
  @parm     digi    do or not to do
  **/
void CameraThread::digitize(bool digi)
{
    m_bDigitizing = digi;
    if (digi) {
        if (m_scanData && m_scanData->width == m_roiLine.height() && m_scanData->height == m_roiPoint.width()) {
            //nothing in the setup has changed, we can continue scanning
            return;
        }
        if (m_scanData) {
            cvReleaseImage(&m_scanData);
        }
        m_scanData = cvCreateImage( cvSize(m_roiLine.height(), m_roiPoint.width()), IPL_DEPTH_64F, 3 );
        cvZero(m_scanData);
    }
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
  @brief    get current live view state
  @return   m_iLiveViewMode
  **/
int CameraThread::liveViewMode()
{
    return m_iLiveViewMode;
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
  @brief set power threshold for point detection
  @param    value new threshold value

  Pixel values below threshold are not detected as line: so there may be "empty" spaces
  **/
void CameraThread::setPowerThresholdPoint(int value)
{
    m_iPointPowerThreshold = value;
}



/**
  @brief set power threshold for line detection
  @param    value new threshold value

  Pixel values below threshold are not detected as line: so there may be "empty" spaces
  **/
void CameraThread::setPowerThresholdLine(int value)
{
    m_iLinePowerThreshold = value;
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
IplImage *CameraThread::evaluateImage(IplImage *img, IplImage *debug /*= NULL*/)
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
        if (max >= m_iPointPowerThreshold) {
            m_posPoint.setX(maxloc.x + m_roiPoint.left());
            m_posPoint.setY(maxloc.y + m_roiPoint.top());
        } else {
            m_posPoint.setX(-1);
            m_posPoint.setY(-1);
        }

        emit pointPosition(m_posPoint.x(), m_posPoint.y());
    }
    if (m_roiLine.width() > 0) {
        cv::Rect lRect( m_roiLine.left(), m_roiLine.top(), m_roiLine.width(), m_roiLine.height() );
        //DEBUG(1, QString("Evaluate Point %1 %2 %3 %4").arg(m_roiLine.left()).arg(m_roiLine.top()).arg(m_roiLine.right()).arg(m_roiLine.bottom()));

        // sub-image
        cvSetImageROI(img,lRect);
        IplImage *lineImage = cvCreateImage( cvSize(lRect.width, lRect.height), IPL_DEPTH_32F, 1 );
        cvConvertScale(img,lineImage);
        cvResetImageROI(img);

        cvSmooth(lineImage, lineImage, CV_GAUSSIAN, 17,5);
        //cvCvtColor(m_iplImage, gray, CV_RGB2GRAY);
        //double lineFilterCoeffs[] = { -3, -2, -1, -1, 0, 1, 2, 5, 7, 11, 7, 5, 2, 1, 0, -1, -1, -2, -3};
        //CvMat lineFilter;
        //cvInitMatHeader( &lineFilter, 1, 19, CV_64FC1, lineFilterCoeffs);

        //cvFilter2D( lineImage ,lineImage, &lineFilter, cvPoint(-1,-1));
        //cvSmooth(grayF,grayF, CV_GAUSSIAN, 15, 1);

        float power;
        int   xpos;

        int slider_x = m_posPoint.x() - m_roiPoint.left();
        for(int y = 0; y < lineImage->height; y++) { //for every row search max
            float *data = (float*) (lineImage->imageData + y * lineImage->widthStep);
            power = 0;
            xpos = 0;
            for(int x = 0; x < lineImage->width; x++) {
                //data += lineImage->nChannels;
                ++data;
                if (*data > power) {
                    power = *data;
                    xpos = x;
                }
            }
            if (power < m_iLinePowerThreshold)
                continue;
            if (debug) {
                cvDrawCircle(debug, cvPoint(xpos + lRect.x,y+lRect.y), 1, cvScalar(0xff,0x00,0xff,0x00), 1);
            } else {
                cvDrawCircle(img, cvPoint(xpos + lRect.x,y+lRect.y), 1, cvScalar(0xff), 1);
            }
            double h = 255.0 - (double(xpos) * 255.0 / m_roiLine.width());
            if (m_bDigitizing && slider_x >= 0 && slider_x < m_scanData->height && xpos >= 0 && xpos < m_scanData->width) {
                double *data = (double*) (m_scanData->imageData + slider_x*m_scanData->widthStep + m_scanData->nChannels*y*sizeof(double)); //
                if (power >= *(data+1)) {   //if stronger/better than old value; then overwrite it
                    *data = h ; //todo: make it different from that
                    *(data+1) =  power;
                }
                //also do the line above, if applicable
                if (slider_x >= 1) { //if line above exists
                    data = (double*) (m_scanData->imageData + (slider_x-1)*m_scanData->widthStep + m_scanData->nChannels*y*sizeof(double)); //
                    if(*(data+1) != 0.0) {   //if line above is unset
                        *data = h;
                        *(data+1) = power / 3.; //lower power for neighbour
                    }
                }
                if (slider_x >= 2) { //if line above exists
                    data = (double*) (m_scanData->imageData + (slider_x-2)*m_scanData->widthStep + m_scanData->nChannels*y*sizeof(double)); //
                    if(*(data+1) != 0.0) {   //if line above is unset
                        *data = h;
                        *(data+1) = power / 5.; //lower power for neighbour
                    }
                }
                //also do the line below if applicable
                if ((slider_x+1) <  m_scanData->height) { //if line below exists
                    data = (double*) (m_scanData->imageData + (slider_x+1)*m_scanData->widthStep + m_scanData->nChannels*y*sizeof(double)); //
                    if(*(data+1) != 0.0) {   //if line below is unset
                        *data = h;
                        *(data+1) = power / 3.;
                    }
                }
                //also do the line below if applicable
                if ((slider_x+2) <  m_scanData->height) { //if line below exists
                    data = (double*) (m_scanData->imageData + (slider_x+2)*m_scanData->widthStep + m_scanData->nChannels*y*sizeof(double)); //
                    if(*(data+1) != 0.0) {   //if line below is unset
                        *data = h;
                        *(data+1) = power / 5.;
                    }
                }
            }
        }
        if (m_bDigitizing) {
            emit newScanData();
        }
        cvReleaseImage( &lineImage);
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

        //cvConvertScale(g,g,0.2);
        //cvSub(gray, b, gray);

        //cvSmooth(grayF,grayF, CV_GAUSSIAN, 3, 3);

        if ((m_iLiveViewMode == MODE_LIVE_CHESSBOARD) || (m_iLiveViewMode == MODE_LIVE_CHESSBOARD_SAVE)) {
            IplImage* gray = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), m_iplImage->depth, 1);
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
            cvReleaseImage(&gray);
            // Draw it
            cvDrawChessboardCorners( m_iplImage, cvSize(CALIBRATION_CHESSBOARD_WIDTH, CALIBRATION_CHESSBOARD_HEIGHT), corners, corner_count, found );
            delete [] corners;

            m_camWidget->setImage(m_iplImage);
        } else {
            IplImage* gray = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), IPL_DEPTH_8U, 1);
            IplImage *grayF32 = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), IPL_DEPTH_32F, 1);

            //truncate to zero; remove negatives
            float *data;
            float value;
            for(int y = 0; y < grayF32->height; y++) { //for every row search max
                data = (float*) (grayF32->imageData + y * grayF32->widthStep);
                for(int x = 0; x < grayF32->width; x++) {
                    //value = 0;
                    //value +=  *((unsigned char*)(gray->imageData + y*gray->widthStep + x*gray->nChannels ));
                    value = ((float) ( *((unsigned char*)m_iplImage->imageData + y*m_iplImage->widthStep + x*m_iplImage->nChannels + 2) ));
                    value -= ((float) ( *((unsigned char*)m_iplImage->imageData + y*m_iplImage->widthStep + x*m_iplImage->nChannels + 1) )) * 0.25;
                    value -= ((float) ( *((unsigned char*)m_iplImage->imageData + y*m_iplImage->widthStep + x*m_iplImage->nChannels + 0) )) * 0.25;

                    *data = (value > 0.0) ? value : 0.0;
                    ++data;
                }
            }

            IplImage* debug = cvCreateImage(cvSize(m_iplImage->width, m_iplImage->height), IPL_DEPTH_8U, 3);
            cvCvtScale(grayF32, gray);
            cvCvtColor(gray, debug, CV_GRAY2BGR);    //better send a (formally) color image to the camera widget

            grayF32 = evaluateImage(grayF32,debug);

            if (m_camWidget) {
                if (MODE_LIVE_PREPROCESSED == m_iLiveViewMode) {
                    m_camWidget->setImage(debug);
                } else if (MODE_LIVE_CAMERA == m_iLiveViewMode) {
                    m_camWidget->setImage(m_iplImage);
                //else do nothing (MODE_LIVE_NONE == m_iLiveViewMode)
                }
            }
            cvReleaseImage(&debug);
            cvReleaseImage(&grayF32);
            cvReleaseImage(&gray);
        }

        //cvReleaseImage(&m_iplImage);  //this one is auto-cleared
        //delete [] lineFilterCoeffs;
    }



    DEBUG(10,"Exiting thread.");
}

/**
  @brief    set the size-unchanged heightmap to all zeros
  **/
void CameraThread::clearHeightmap()
{
    if (m_scanData)
        cvZero(m_scanData);
    emit newScanData();
}


/**
  @brief    calculate a 3D point cloud from heightmap and calibration data

  stored in m_pointCloud
  **/
void CameraThread::triangulatePointCloud()
{
    if (m_pointCloud)
        cvReleaseImage(&m_pointCloud);

    m_pointCloud = cvCreateImage(cvSize(m_scanData->width, m_scanData->height), IPL_DEPTH_64F, 3);

    QTemporaryFile file("temp_XXXXXX.xyz");
    file.setAutoRemove(false);
    file.open();

    //cvSmooth(m_pointCloud, m_pointCloud, CV_GAUSSIAN, 7,7);
    double *data = (double*) m_pointCloud->imageData;
    double z;

    if (m_dScaleX == 0. || m_dScaleY == 0. || m_dScaleZ == 0.) {
        QMessageBox::critical(QApplication::activeWindow(), "Error", "Error: One of the scale factors is zero. Division by zero! Cannot triangulate. Aborting.");
        return;
    }

    for (int y = 0; y < m_pointCloud->height; y++) {
        data = (double*) (m_pointCloud->imageData + y * m_scanData->widthStep);
        for (int x = 0; x < m_pointCloud->width; x++) { //linear "triangulation"
            *data = x;
            ++data;
            *data = y;
            ++data;
            z = *((double*) (m_scanData->imageData + y * m_scanData->widthStep + x * m_scanData->nChannels * sizeof(double))  );
            *data = z;
            ++data;
            if (z != 0)
                file.write( QString("%1 %2 %3 0. 0. 1.\n").arg(((double)x - m_dOffsetX)/m_dScaleX ).arg(((double)y - m_dOffsetY)/m_dScaleY ).arg(((double)z - m_dOffsetZ)/(-m_dScaleZ)).toLatin1() );
        }
    }
    file.flush();
    QFileInfo info(file);
    QString app("C:\\Program Files (x86)\\VCG\\MeshLab\\meshlab.exe");
    QString path("C:\\Program Files (x86)\\VCG\\MeshLab");
    QStringList args;
    args.append(info.absoluteFilePath());
    DEBUG(1, QString("Temp-File: %1").arg(info.absoluteFilePath()));
    file.close();
    QProcess::startDetached(app, args, path);
}


