#ifndef HEIGHTMAPWIDGET_H
#define HEIGHTMAPWIDGET_H

#include <QWidget>
#include <opencv.hpp>

class HeightmapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeightmapWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    //void setImagePart(const IplImage *img, const QRect &rect);
    void setImage(const QImage &img);
    void setImage(const IplImage *img);

protected:
    QImage  m_image;            ///< display image data
    cv::Mat m_heightMap;        ///< heightmap data
    double  m_scaleX;           ///< display scale x
    double  m_scaleY;           ///< display scale y

    virtual void paintEvent(QPaintEvent *event);
};

#endif // HEIGHTMAPWIDGET_H
