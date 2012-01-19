#include "QtException.h"
#include "cameraWidget.h"
#include "QtGui"
#include "QtCore"

typedef unsigned char UINT8;
typedef unsigned int UINT32;

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
void CameraWidget::setImage(const QImage &img)
{
    m_image = img;
    update();   //schedule paint event
}

/**
  @brief    set image from image matrix
  @param    img matrix class image
  **/
void CameraWidget::setImage(const IplImage *img)
{
      if (NULL == img) {
        DEBUG(10, "Image is NULL");
        return;
      }
      if ( (img->height != m_image.height()) || (img->width != m_image.width()) ) {	//check for image sizes
          m_image = QImage(img->width, img->height, QImage::Format_RGB32);
          m_image.fill( 0xff00ffff);
          DEBUG(10, QString("Created m_image (%1x%2)").arg(m_image.width()).arg(m_image.height()));
      }

      UINT32 *pDstBase; // = (UINT32*) m_image.bits();
      int	lineWidthDst = m_image.bytesPerLine();

      if ((img->nChannels == 1) && (img->depth == IPL_DEPTH_8U)){
          UINT32 pixval;


          for(int y=0; y < img->height; y++) {
              for(int x=0; x < img->width; x++) {
                  pixval = *((UINT8*) (img->imageData + y*img->widthStep + x));
                  pixval = 0xff000000 | (pixval << 16) | (pixval << 8) | pixval;
                  //*((UINT32*)  (pDstBase + (x * sizeof(UINT32)))) = pixval;

              }
              pDstBase += lineWidthDst;
          }
      } else if ((img->nChannels == 3) && (img->depth == IPL_DEPTH_8U)) {
          //DEBUG(50,"Convert color image");
          UINT32 pixval;
          char* pix;

          int y;
          /* //#pragma omp parallel for private(y,pixval,pix,pDstBase) */
          for(y=0; y < img->height; y++) {
              pDstBase = (UINT32*) m_image.scanLine(y);
              for(int x=0; x < img->width; x++) {
                  pix = img->imageData + y*img->widthStep + x*img->nChannels;
                  pixval = 0xFF000000 | (((*(pix+2)) & 0xff)<< 16) | (((*(pix+1)) & 0xff) << 8) | ((*pix) & 0xff);
                  *(pDstBase + x) = pixval;
              }
          }
      } else {
          DEBUG(10, "Invalid format of IplImage");
          QMessageBox::critical(this, "Image format error", "Invalid format of IplImage");
      }
      update();
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
        DEBUG(10,"Invalid roi type");
    }
    update();
}

/**
  @brief    get some roi
  @param    roitype which roi to retrieve
  **/
QRect CameraWidget::roi(int roitype)
{
    if (roitype == ROI_TYPE_LINE) {
        return m_roiLine;
    } else if (roitype == ROI_TYPE_POINT) {
        return m_roiPoint;
    } else {
        DEBUG(10, "Invalid roi type requested");
        return QRect();
    }
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
    if (!m_image.isNull()) {    //only print something if we have content
        //get painter

        //draw image content first

        DEBUG(10,QString("Draw %1 %2 %3 %4").arg(this->rect().left()).arg(this->rect().top()).arg(this->rect().right()).arg(this->rect().bottom()));
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
        DEBUG(10, "Empty m_image");
        painter.fillRect(this->rect(), QColor(0xff000000));  //blacken
    }
}
