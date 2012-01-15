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

class cameraThread : public QThread
{
    Q_OBJECT
public:
    explicit cameraThread(QObject *parent = 0);
    
protected:
    void run();

signals:
    
public slots:
    void sendTerminationRequest();
    void setCvCamera(int cvIndex, CvCapture* cvCapture);
    void setCameraWidget(CameraWidget* target);
    
private:
    int            captureFrame();
    void           setModeOfOperation(int mode);
    int            modeOfOperation();
    int            findLaserData();

private:
    int            m_iMode;                 ///< mode of operation
    bool           m_bTerminationRequest;   ///< internal: thread termination request
    int            m_iCamera;               ///< camera id
    CvCapture*     m_cvCapture;             ///< pointer to capture device struct
    CameraWidget*  m_camWidget;             ///< pointer to displaying widget
    IplImage*      m_iplImage;              ///< image as from opencv
    Matrix<double> m_matPoint;              ///< found laser point position
};

#endif // CAMERATHREAD_H
