#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QPen>
#include <QRect>
#include <QPoint>
#include <QPointF>
#include <matrix.h>
#include <opencv.hpp>

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

    QPen    m_penPoint;
    QPen    m_penLine;

signals:
    void tellMousePosition(int x, int y);

public slots:
    void setImage(QImage &img);
    void setImage(cv::Mat &img);
    void setRoi(QRect &roi, int roitype);
    void tellLaserPos(QPointF &pos);
    void tellLaserLine(float *line);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);
};

#endif // CAMERAWIDGET_H
