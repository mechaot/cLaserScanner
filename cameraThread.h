#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <opencv.hpp>
#include <cameraWidget.h>

//modes are bitwire or'ed
#define MODE_NONE       0       ///< mode: do nothing
#define MODE_IMAGE_OUT  1       ///< mode: do copy image to output
#define MODE_POINT      2       ///< mode: do look for point within pointroi
#define MODE_LINE       4       ///< mode: do look for laser line within lineroi


/**
  @class    CameraThread    threaded entity that captures camera frames, processes the image (find lasers) and propagates to gui widget
  **/
class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = 0);
    
protected:
    void run();

signals:
    
public slots:
    void sendTerminationRequest();
    void setCvCamera(int cvIndex, CvCapture* cvCapture);
    void setCameraWidget(CameraWidget* target);
    void setRoi(QRect &roi, int roitype);

private:
    int            captureFrame();
    void           setModeOfOperation(int mode);
    int            modeOfOperation();
    int            evaluateImage(IplImage *img);

private:
    int            m_iMode;                 ///< mode of operation
    bool           m_bTerminationRequest;   ///< internal: thread termination request
    int            m_iCamera;               ///< camera id
    CvCapture*     m_cvCapture;             ///< pointer to capture device struct
    CameraWidget*  m_camWidget;             ///< pointer to displaying widget
    IplImage*      m_iplImage;              ///< image as from opencv
    //Matrix<double> m_matPoint;              ///< found laser point position
    QRect          m_roiLine;
    QRect          m_roiPoint;

};

#endif // CAMERATHREAD_H
