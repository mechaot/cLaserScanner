#include "cameraThread.h"
#include "QtException.h"

cameraThread::cameraThread(QObject *parent) :
    QThread(parent)
{
    m_iCamera = -1;
    m_cvCapture = NULL;
    m_camWidget = NULL;
}

/**
  @brief    tell the thread to terminate itself in a clean way
  **/
void cameraThread::sendTerminationRequest()
{
    m_bTerminationRequest = true;
}


/**
  @brief set mode of operation
  @param    mode mode to be set

  unchecked
  **/
void cameraThread::setModeOfOperation(int mode)
{
    m_iMode = mode;
}

/**
  @brief    get mode of operation
  @return   mode of operation
  **/
int cameraThread::modeOfOperation()
{
    return m_iMode;
}

/**
  @brief    get selected camera
  @param[in]    cvIndex     numeric index of cv camera
  @param[in]    cvCapture   pointer to connected camera
  **/
void cameraThread::setCvCamera(int cvIndex, CvCapture *cvCapture)
{
    m_iCamera = cvIndex;
    m_cvCapture = cvCapture;
}

/**
  @brief    set the widget for output
  @param    target  widget of type CameraWidget to take the generated output
  **/
void cameraThread::setCameraWidget(CameraWidget *target)
{
    m_camWidget = target;
}

/**
  @brief    thread's main routine
  **/
void cameraThread::run()
{
    m_bTerminationRequest = false;  //initially we don't want to kill us
    while (m_bTerminationRequest) {
        if(!m_cvCapture)     {
            DEBUG(10, "m_cvCapture not ready. Terminating thread.");
            break;
        }
        m_iplImage = cvQueryFrame(m_cvCapture);
        if(m_iplImage->width * m_iplImage->height < 1) {
            DEBUG(20, "image invalid, retrying");
            continue;
        }



        if ( (m_iMode & MODE_IMAGE_OUT) && (m_camWidget) ) {
            //m_camWidget->setImage(m_image);
        }

    }
}
