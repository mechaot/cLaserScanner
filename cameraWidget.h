#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QPen>
#include <QRect>
#include <QPoint>
#include <QPointF>
//#include <opencv.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define ROI_TYPE_NONE 0
#define ROI_TYPE_POINT 1
#define ROI_TYPE_LINE 2

class CameraWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraWidget(QWidget *parent = 0);

private:
    QImage  m_image;
    QPointF m_posPoint;
    QRect   m_roiPoint;
    QRect   m_roiLine;
    float   m_scaleX;
    float   m_scaleY;

    int     m_cursorX;  //mouse clicked cursor position
    int     m_cursorY;  //mouse clicked cursor position

    QPen    m_penPoint;
    QPen    m_penLine;

    int     m_iRoiSettingMode;
signals:
    void tellMousePosition(int x, int y);
    void roiChangedPoint(const QRect&);
    void roiChangedLine(const QRect&);

public slots:
    void setImage(const QImage &img);
    void setImage(const IplImage *img);
    void setRoi(QRect &roi, int roitype);
    QRect roi(int roitype);
    void tellLaserPos(int x, int y);
    void tellLaserLine(float *line);

    void startSettingRoiPoint();
    void startSettingRoiLine();

    virtual QSize sizeHint();

    void clearCursor();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);
};

#endif // CAMERAWIDGET_H
