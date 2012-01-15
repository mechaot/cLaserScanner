#ifndef MATRIX_BV_HPP
#define MATRIX_BV_HPP

#include <opencv.hpp>
#include <cstring>
#include <string.h>

#if SIMALI_RAYTRACER_FUNCTIONS
    #include "RTcolor.h"
#endif


#if SIMALI_RAYTRACER_FUNCTIONS

/**
  @brief    conversion to opencvmatrix for color-matrix
  @param    mattype desired destination type for cv::Mat
  @return   cv::Mat with data from this matrix

  @note remember to have the template specialization BEFORE the generic version
  **/
//template<> cv::Mat Matrix<RTcolor>::toMat(int mattype /* = cv::CV_64FC3*/)
/*{
    cv::Mat mat(rows, cols, mattype);
    if (CV_64FC3 == mattype) {
        double *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((double*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = pData[y * cols + x].r;
                    *(ptr++) = pData[y * cols + x].g;
                    *(ptr++) = pData[y * cols + x].b;
            }
        }
    }
    if (CV_64FC1 == mattype) {
        double *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((double*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = pData[y * cols + x].intensity();
            }
        }
    }
    if (CV_8UC3 == mattype) {
        unsigned char *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((unsigned char*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = (unsigned char) pData[y * cols + x].r;
                    *(ptr++) = (unsigned char) pData[y * cols + x].g;
                    *(ptr++) = (unsigned char) pData[y * cols + x].b;
            }
        }
    }
    if (CV_8UC1 == mattype) {
        unsigned char *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((unsigned char*)mat.ptr(y));
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = (unsigned char) pData[y * cols + x].intensity();
            }
        }
    }
    return mat;
}
*/
#endif //SIMALI_RAYTRACER_FUNCTIONS


/**
  @brief    convert this to matrix to an opencv matrix
  @param    mattype desired destination type for cv::Mat
  @return   cv::Mat with data from this matrix
  **/
template<typename type> cv::Mat Matrix<type>::toMat(int mattype /* = CV_64FC3*/)
{
    cv::Mat mat(rows, cols, mattype);
    if (CV_64FC3 == mattype) {
        double *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((double*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = pData[y * cols + x];
                    *(ptr++) = pData[y * cols + x];
                    *(ptr++) = pData[y * cols + x];
            }
        }
    }
    if (CV_64FC1 == mattype) {
        double *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((double*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = pData[y * cols + x];
            }
        }
    }
    if (CV_8UC3 == mattype) {
        unsigned char *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((unsigned char*)mat.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = (unsigned char) pData[y * cols + x];
                    *(ptr++) = (unsigned char) pData[y * cols + x];
                    *(ptr++) = (unsigned char) pData[y * cols + x];
            }
        }
    }
    if (CV_8UC1 == mattype) {
        unsigned char *ptr;
        for(int y = 0; y < rows; y++) {
            ptr = ((unsigned char*)mat.ptr(y));
            for(int x = 0; x < cols; x++) {
                    *(ptr++) = (unsigned char) pData[y * cols + x];
            }
        }
    }
    return mat;
}

/**
    @brief  load matrix from an image file; note todo
    @param  fileName    name of image file to lead from
    @param  flags       -1 (default): load as it is
                        0: load grayscale (forcefully)
                        1: load color (forcefully)
  **/
template<typename type> Matrix<type>& Matrix<type>::LoadFromImageFile(const QString &fileName, int flags /* = -1 */)
{
    const char* fName = fileName.toAscii().constData();
    std::string s;
    s.append(fName);
    cv::Mat image = cv::imread(s, flags);
    if(image.cols * image.rows < 1) {
        DEBUG(1,QString("Error: cannot load image '%1': size null").arg(fileName));
        Clear();
        return *this;
    }
    if(image.empty()) {
        DEBUG(1,QString("Error: cannot load image '%1': size null").arg(fileName));
        Clear();
        return *this;
    }
    if (image.channels() == 1) {
        Create(image.cols, image.rows);
        type* dst = pData;
        double *src;
        for(int y = 0; y < rows; y++) {
            src = ((double*)image.ptr(y)) ;
            for(int x = 0; x < cols; x++) {
                    *(dst) = *(src);
                    ++dst;
                    ++src;
            }
        }
    } else if (image.channels() == 3) {
        Create(image.cols * 3, image.rows);
        type* dst = pData;
        double *src;
        for(int y = 0; y < rows; y++) {
            src = ((double*)image.ptr(y)) ;
            for(int x = 0; x < 3*cols; x++) {
                    *(dst) = *(src); ++dst; ++src;
                    //*(dst) = *(src); ++dst; ++src;
                    //*(dst) = *(src); ++dst; ++src;
            }
        }
    } else {
        EX_THROW("Can only load 1 or 3 channel images")
    }
    return (*this);
}

/**
  @brief    save matrix to image file
    @todo   test
    @warning non-tested
  **/
template<typename type> int Matrix<type>::SaveToImageFile(const QString &fileName, bool normalize /* = true */)
{
    if(fileName.isEmpty()) {
        CONSOLE(1, "Error saving Image, no fileName given");
        return -1;
    }

    cv::Mat image = toMat(CV_64FC3);
    const char* fName = fileName.toAscii().constData();
    bool ok =  cv::imwrite(fName, image);

    return ok ? 0 : -1;
}





#endif // MATRIX_BV_HPP
