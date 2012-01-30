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

#define MODE_LIVE_NONE              0
#define MODE_LIVE_CAMERA            1
#define MODE_LIVE_PREPROCESSED      2
#define MODE_LIVE_CHESSBOARD        3
#define MODE_LIVE_CHESSBOARD_SAVE   4       ///< save current frame data


/**
  @class    CameraThread    threaded entity that captures camera frames, processes the image (find lasers) and propagates to gui widget
  **/
class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = 0);
    virtual ~CameraThread();

protected:
    void run();

signals:
    void pointPosition(int x, int y);
    void newScanData();
    
public slots:
    void sendTerminationRequest();
    void setCvCamera(int cvIndex, CvCapture* cvCapture);
    void setCameraWidget(CameraWidget* target);
    void setLiveViewMode(int mode);
    int liveViewMode();
    void setRoi(const QRect &roi, int roitype);
    void setRoiPoint(const QRect &roi);
    void setRoiLine(const QRect &roi);
    void digitize(bool digi);
    void loadInternalCalibration(const QString& fileName);
    void loadExternalCalibration(const QString& fileName);
    void saveInternalCalibration(const QString& fileName);
    void saveExternalCalibration(const QString& fileName);
    void clearHeightmap();
    void triangulatePointCloud();

private:
    int            captureFrame();
    void           setModeOfOperation(int mode);
    int            modeOfOperation();
    IplImage*      evaluateImage(IplImage *img, IplImage *debug = NULL);

private:
    int            m_iMode;                 ///< mode of operation
    bool           m_bTerminationRequest;   ///< internal: thread termination request
    int            m_iCamera;               ///< camera id
    int            m_iLiveViewMode;         ///< live view mode: what is to be sent to the widget
    CvCapture*     m_cvCapture;             ///< pointer to capture device struct
    CameraWidget*  m_camWidget;             ///< pointer to displaying widget
    IplImage*      m_iplImage;              ///< image from opencv camera
    QRect          m_roiLine;               ///< region of interest for line detection
    QRect          m_roiPoint;              ///< region of interest for point detection


    QPoint         m_posPoint;              ///< found laser point position

    cv::Mat        m_camIntrinsics;         ///< camera intrinsic parameters
    cv::Mat        m_camExtrinsics;         ///< camera extrinsic matrix

    bool           m_bDigitizing;           ///< state: are we digitizing for 3D?
public:
    IplImage*      m_scanData;              ///< scanned data
    IplImage*      m_pointCloud;            ///< double x,y,z point cloud data

};

#endif // CAMERATHREAD_H
