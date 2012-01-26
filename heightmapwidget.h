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
    void setImage(const QImage &img);
    void setImage(const IplImage *img);

protected:
    QImage  m_image;
    double  m_scaleX;
    double  m_scaleY;

    virtual void paintEvent(QPaintEvent *event);
};

#endif // HEIGHTMAPWIDGET_H
