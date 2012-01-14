#include "cameraWidget.h"
#include "QtGui"
#include "QtCore"

CameraWidget::CameraWidget(QWidget *parent) :
    QWidget(parent)
{
    //gimme speed
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    m_scaleX = 1.0;
    m_scaleY = 1.0;


    m_penPoint = QPen(QColor(0xff00ff00));
    m_penPoint.setStyle(Qt::DotLine);
    m_penLine = QPen(QColor(0xffff0000));
    m_penLine.setStyle(Qt::DotLine);

    m_posPoint.setX(-1);    //make invalid
}

/**
  @brief    set whole content
  @param    img image to be set
  **/
void CameraWidget::setImage(QImage &img)
{
    m_image = img;
    update();   //schedule paint event
}

/**
  @brief    set coordinates of one of the rois (in image coords)
  @param    roi     roi coordinates
  @param    roitype type of roi to set {ROI_TYPE_POINT, ROI_TYPE_LINE}
  **/
void CameraWidget::setRoi(QRect &roi, int roitype)
{
    if (roitype == ROI_TYPE_LINE) {
        m_roiLine = roi;
    } else if (roitype == ROI_TYPE_POINT) {
        m_roiPoint = roi;
    } else {
        qDebug("Invalid roi type");
    }
    update();
}

/**
  @brief    tell the widget about the detected laser pos for displaying
  @param    pos position of detected laser

  pos with negative coordinates indicate not found
  **/
void CameraWidget::tellLaserPos(QPointF &pos)
{
    m_posPoint = pos;
    update();
}

/**
  @brief    tell the widget about the current detected laser line for displaying
  @param    line    array of line coords
  **/
void CameraWidget::tellLaserLine(float *line)
{
    //todo
    update();
}

/**
  @brief    things to be done when mouse key is pressed; e.g. tell position
  @param[in] event  recieved event
  **/
void CameraWidget::mousePressEvent(QMouseEvent *event)
{
    //QWidget::mousePressEvent(event);
    float px = float(event->x()) / m_scaleX;
    float py = float(event->y()) / m_scaleY;

    emit tellMousePosition( int(px+0.5), int(py+0.5));
}

/**
  @brief    things to be done when mouse key released
  @param[in] event  recieved event
  **/
void CameraWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //QWidget::mouseReleaseEvent(event);
}

/**
  @brief    things to be done when mouse moved while key is pressed; e.g. tell position
  @param[in] event  recieved event
  **/
void CameraWidget::mouseMoveEvent(QMouseEvent *event)
{
    //QWidget::mouseMoveEvent(event);
    float px = event->x() / m_scaleX;
    float py = event->y() / m_scaleY;

    emit tellMousePosition( int(px+0.5), int(py+0.5));
}

/**
  @brief    do all that needs to be done for displaying purposes
  **/
void CameraWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
/*    if (!m_image.isNull()) {    //only print something if we have content
        //get painter

        //draw image content first
        painter.drawImage(this->rect(), m_image);

        //calculate current scale
        m_scaleX = float(this->rect().width()) / float(m_image.width());
        m_scaleY = float(this->rect().height()) / float(m_image.height());

        // roi for point
        QRect scaledRoiPoint(m_roiPoint.x() * m_scaleX, m_roiPoint.y() * m_scaleY,
                              m_roiPoint.width() * m_scaleX, m_roiPoint.height() * m_scaleY);
        painter.setPen(m_penPoint);
        painter.drawRect(scaledRoiPoint);

        // circle around point position, if valid
        if (m_posPoint.x() >= 0 && m_posPoint.y() >= 0) {
            QPointF pos(m_posPoint.x() * m_scaleX, m_posPoint.y() * m_scaleY);
            painter.drawEllipse(pos, 10, 10);
        }

        //roi for line
        QRect scaledRoiLine(m_roiLine.x() * m_scaleX, m_roiLine.y() * m_scaleY,
                            m_roiLine.width() * m_scaleX, m_roiLine.height() * m_scaleY);
        painter.setPen(m_penLine);
        painter.drawRect(scaledRoiLine);

        //line: todo
    } else {    //no content
        painter.fillRect(this->rect(), QColor(0xff000000));  //blacken
    }*/
    //painter.fillRect(this->rect(), QColor(0xff000000));  //blacken
}
