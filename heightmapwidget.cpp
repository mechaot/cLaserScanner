#include "heightmapwidget.h"
#include "QtException.h"
#include <QPainter>
#include <stdint.h>

#ifndef UINT8
    typedef unsigned char UINT8;
#endif
#ifndef UINT32
    typedef unsigned int UINT32;
#endif


HeightmapWidget::HeightmapWidget(QWidget *parent) :
    QWidget(parent)
{
    m_scaleX = 0.0;
    m_scaleY = 0.0;
}


/**
  @brief    set whole content
  @param    img image to be set
  **/
void HeightmapWidget::setImage(const QImage &img)
{
    m_image = img;
    update();   //schedule paint event
}

/**
  @brief    set image from image matrix
  @param    img matrix class image
  **/
void HeightmapWidget::setImage(const IplImage *img)
{
      if (NULL == img) {
        DEBUG(10, "Image is NULL");
        return;
      }
      if ( (img->height != m_image.height()) || (img->width != m_image.width()) ) {	//check for image sizes
          m_image = QImage(img->width, img->height, QImage::Format_RGB32);
          m_image.fill( 0x000000ff);
          DEBUG(10, QString("Created m_image (%1x%2)").arg(m_image.width()).arg(m_image.height()));
      }

      UINT32 *pDstBase; // = (UINT32*) m_image.bits();
      int	lineWidthDst = m_image.bytesPerLine();

      if ((img->nChannels == 1) && (img->depth == IPL_DEPTH_8U)){
          //DEBUG(1,"Convert color image 8u1");
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
          //DEBUG(1,"Convert color image 8u3");
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
      } else if ((img->nChannels == 3) && (img->depth == IPL_DEPTH_64F)) {
          //DEBUG(1,QString("Convert color image 64F3 (%1, %2)").arg(img->width).arg(img->height));
          UINT8 r;
          UINT8 g;
          UINT8 b;

          UINT32 pixval;
          char* pix;
          double* pix_d;

          int y;
          /* //#pragma omp parallel for private(y,pixval,pix,pDstBase) */
          for(y=0; y < img->height; y++) {
              pDstBase = (UINT32*) m_image.scanLine(y);
              for(int x=0; x < img->width; x++) {
                  pix = img->imageData + y*img->widthStep + x*img->nChannels * sizeof(double);
                  pix_d = (double*) pix;
                  r = (UINT8) *(pix_d+0);
                  g = (UINT8) *(pix_d+1);
                  b = (UINT8) *(pix_d+2);

                  pixval = 0x00000000 | (((UINT32)(r)) << 16) | (((UINT32)(g)) << 8) | r;
                  //if (pixval != 0xff000000)  DEBUG(1,QString::number(pixval,16));
                  //pixval = x*y;
                  *(pDstBase + x) = pixval;
              }
          }
      } else {
          DEBUG(10, "Invalid format of IplImage");
          //QMessageBox::critical(this, "Image format error", "Invalid format of IplImage");
      }
      update();
}

/**
  @brief    do all that needs to be done for displaying purposes
  **/
void HeightmapWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if (!m_image.isNull()) {    //only print something if we have content
        //get painter

        //draw image content first

        //DEBUG(10,QString("Draw %1 %2 %3 %4").arg(this->rect().left()).arg(this->rect().top()).arg(this->rect().right()).arg(this->rect().bottom()));
        painter.drawImage(this->rect(), m_image);

        //calculate current scale
        m_scaleX = float(this->rect().width()) / float(m_image.width());
        m_scaleY = float(this->rect().height()) / float(m_image.height());


    } else {    //no content
        DEBUG(10, "Empty m_image");
        painter.fillRect(this->rect(), QColor(0xff000000));  //blacken
    }
}
