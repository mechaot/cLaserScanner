#include "QtException.h"
#include "cameraWidget.h"
#include "QtGui"
#include "QtCore"
#include "settings.h"
#include <QtWidgets/QMessageBox>

#ifndef UINT8
    typedef unsigned char UINT8;
#endif
#ifndef UINT32
    typedef unsigned int UINT32;
#endif

CameraWidget::CameraWidget(QWidget *parent) :
    QWidget(parent)
{
    //gimme speed
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    m_scaleX = 1.0;
    m_scaleY = 1.0;

    m_cursorX = -1;
    m_cursorY = -1;

    m_iRoiSettingMode = ROI_TYPE_NONE;

    m_penPoint = QPen(QColor(0xff00ff00));
    m_penPoint.setStyle(Qt::DotLine);
    m_penPoint.setWidth(2);
    m_penLine = QPen(QColor(0xffff0000));
    m_penLine.setStyle(Qt::DotLine);
    m_penLine.setWidth(2);
    m_penCursor = QPen(QColor(0xff00ffff));
    m_penCursor.setStyle(Qt::DashDotLine);

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
      } else if ((img->nChannels == 3) && (img->depth == IPL_DEPTH_32F)) {
          //DEBUG(50,"Convert color image");
          UINT32 pixval;
          float* pix;

          int y;
          /* //#pragma omp parallel for private(y,pixval,pix,pDstBase) */
          for(y=0; y < img->height; y++) {
              pDstBase = (UINT32*) m_image.scanLine(y);
              for(int x=0; x < img->width; x++) {
                  pix = (float*) (img->imageData + y*img->widthStep + x*img->nChannels);
                  pixval = (((UINT32)(*(pix+2)) &0xff) << 16) + (((UINT32)(*(pix+1)) &0xff) << 8) + ( (UINT32)(*pix) & 0xff );
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
  @brief    begin setting roi via mouse tracking
  **/
void CameraWidget::startSettingRoiPoint()
{
    m_iRoiSettingMode = ROI_TYPE_POINT;
}


/**
  @brief    begin setting roi via mouse tracking
  **/
void CameraWidget::startSettingRoiLine()
{
    m_iRoiSettingMode = ROI_TYPE_LINE;
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
void CameraWidget::tellLaserPos(int x, int y)
{
    m_posPoint.setX(x);
    m_posPoint.setY(y);
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
  @brief    give size hint

  necessary for scroll bar
  **/
QSize CameraWidget::sizeHint()
{
    if (m_image.isNull())
        return this->rect().size();
    else
        return m_image.size();
}

/**
  @brief    externally enable cursor clearing
  **/
void CameraWidget::clearCursor()
{
    m_cursorX = -1;
    m_cursorY = -1;

    emit tellMousePosition( m_cursorX, m_cursorY );
}

/**
  @brief    things to be done when mouse key is pressed; e.g. tell position
  @param[in] event  recieved event
  **/
void CameraWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        //QWidget::mousePressEvent(event);
        float px = float(event->x()) / m_scaleX;
        float py = float(event->y()) / m_scaleY;

        if (m_iRoiSettingMode == ROI_TYPE_POINT) {  //we are in roi setting mode
            m_roiPoint.setTopLeft(QPoint(int(px),int(py)));
            m_cursorX = -1; //no cursor while setting roi
            m_cursorY = -1;
        }
        else if (m_iRoiSettingMode == ROI_TYPE_LINE) {
            m_roiLine.setTopLeft(QPoint(int(px),int(py)));
            m_cursorX = -1;
            m_cursorY = -1;
        } else {
            m_cursorX = int(px);
            m_cursorY = int(py);
        }

    }
    if (event->buttons() & Qt::RightButton) {   //right mouse button clears cursor
        m_cursorX = -1;
        m_cursorY = -1;
    }

    emit tellMousePosition( m_cursorX, m_cursorY );
}

/**
  @brief    things to be done when mouse key released
  @param[in] event  recieved event
  **/
void CameraWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() & Qt::LeftButton) {
        float px = float(event->x()) / m_scaleX;
        float py = float(event->y()) / m_scaleY;

        if (m_iRoiSettingMode == ROI_TYPE_POINT) {
            m_roiPoint.setBottomRight(QPoint(int(px),int(py)));
            m_roiPoint = m_roiPoint.normalized();
            if (m_roiPoint.top() < 0)
                m_roiPoint.setTop(0);
            if (m_roiPoint.left() < 0)
                m_roiPoint.setLeft(0);
            if (m_roiPoint.right() >= CAMERA_RESOLUTION_X)
                m_roiPoint.setRight(CAMERA_RESOLUTION_X-1);
            if (m_roiPoint.bottom() >= CAMERA_RESOLUTION_Y)
                m_roiPoint.setBottom(CAMERA_RESOLUTION_Y-1);
            emit roiChangedPoint(m_roiPoint);
            emit finishedSettingRoi(false);
        }
        else if (m_iRoiSettingMode == ROI_TYPE_LINE) {
            m_roiLine.setBottomRight(QPoint(int(px),int(py)));
            m_roiLine = m_roiLine.normalized();
            if (m_roiLine.top() < 0)
                m_roiLine.setTop(0);
            if (m_roiLine.left() < 0)
                m_roiLine.setLeft(0);
            if (m_roiLine.right() >= CAMERA_RESOLUTION_X)
                m_roiLine.setRight(CAMERA_RESOLUTION_X-1);
            if (m_roiLine.bottom() >= CAMERA_RESOLUTION_Y)
                m_roiLine.setBottom(CAMERA_RESOLUTION_Y-1);
            emit roiChangedLine(m_roiLine);
            emit finishedSettingRoi(false);
        }
        m_iRoiSettingMode = ROI_TYPE_NONE;
    }
}

/**
  @brief    things to be done when mouse moved while key is pressed; e.g. tell position
  @param[in] event  recieved event
  **/
void CameraWidget::mouseMoveEvent(QMouseEvent *event)
{
    //QWidget::mouseMoveEvent(event);
    if (event->buttons() & Qt::LeftButton) {
        float px = event->x() / m_scaleX;
        float py = event->y() / m_scaleY;

        if (m_iRoiSettingMode == ROI_TYPE_POINT) {
            m_roiPoint.setBottomRight(QPoint(int(px+0.5),int(py+0.5)));
            m_cursorX = -1;
            m_cursorY = -1;
        }
        else if (m_iRoiSettingMode == ROI_TYPE_LINE) {
            m_roiLine.setBottomRight(QPoint(int(px+0.5),int(py+0.5)));
            m_cursorX = -1;
            m_cursorY = -1;
        } else {
            m_cursorX = int(px);
            m_cursorY = int(py);
        }
        emit tellMousePosition( m_cursorX, m_cursorY );
    }
}

/**
  @brief    do all that needs to be done for displaying purposes
  **/
void CameraWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    if (!m_image.isNull()) {    //only print something if we have content
        //get painter

        //draw image content first

        //DEBUG(10,QString("Draw %1 %2 %3 %4").arg(this->rect().left()).arg(this->rect().top()).arg(this->rect().right()).arg(this->rect().bottom()));
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

        //clicked cursor cross
        if (m_cursorX > 0 && m_cursorY > 0) {
            painter.setPen(m_penCursor);

            painter.drawLine(this->rect().left(), m_cursorY*m_scaleY, this->rect().right(), m_cursorY*m_scaleY);
            painter.drawLine(m_cursorX*m_scaleX, this->rect().top(), m_cursorX*m_scaleX, this->rect().bottom());
        }
        //line: todo
    } else {    //no content
        DEBUG(10, "Empty m_image");
        painter.fillRect(this->rect(), QColor(0xff000000));  //blacken
    }
}
