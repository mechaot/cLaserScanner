/**
    @file	matrix_statics.hpp	template implementation of matrix class static member functions


    $Id: matrix_statics.hpp 73 2011-05-31 13:14:31Z mechaot $

**/

#ifndef MATRIX_STATICS_HPP
#define MATRIX_STATICS_HPP


/**
  @brief    conveniently create an identity matrix
  @param    c   number of columns desired
  @param    r   number of rows desired (if negative = default), r will be set to c
  @return   new matrix of type filled with an eye pattern
  **/
template<typename type> Matrix<type> Matrix<type>::eye(int c, int r /* = -1*/)      // create an eye-matrix of size r x c
{
    if (r < 0) {
        r = c;
    }
    Matrix<type> e(c,r);
    for(int y = 0; y < r; y++) {
        for(int x = 0; x < c; x++) {
            if (x == y)
                e.pData[y * c + x] = 1;
            else
                e.pData[y * c + x] = 0;
        }
    }
    return e;
}

/**
  @brief    conveniently create an zeroed matrix
  @param    c   number of columns desired
  @param    r   number of rows desired (if negative = default), r will be set to c
  @return   new matrix of type filled with zeros
  **/
template<typename type> Matrix<type> Matrix<type>::zero(int c, int r)     // create a zero-matrix of size r x c
{
    if (r < 0) {
        r = c;
    }
    Matrix<type> e(c,r);
    for(int y = 0; y < r; y++) {
        for(int x = 0; x < c; x++) {
                e.pData[y * c + x] = 0;
        }
    }
    return e;
}

/**
  @brief    create a 4x4 rotation matrix for rotation around X-axis by some angle (in rad)
  @param    angle   angle around axis (in rad)
  @return   rotation matrix

  Matrix is 4x4 because for use with homogeneous 3D coordinates
  **/
template<typename type> Matrix<type> Matrix<type>::rotX(double angle)
{
    Matrix<type> rot = Matrix<type>::eye(4);
    rot.pData[5] = cos(angle);
    rot.pData[10] = rot.pData[5];

    rot.pData[9] = sin(angle);
    rot.pData[6] = -rot.pData[9];

    return rot;
}

/**
  @brief    create a 4x4 rotation matrix for rotation around Y-axis by some angle (in rad)
  @param    angle   angle around axis (in rad)
  @return   rotation matrix

  Note that for y-rotation the signum of the sinus has other signum!

  Matrix is 4x4 because for use with homogeneous 3D coordinates
  **/
template<typename type> Matrix<type> Matrix<type>::rotY(double angle)
{
    Matrix<type> rot = Matrix<type>::eye(4);
    rot.pData[0] = cos(angle);
    rot.pData[10] = rot.pData[0];

    rot.pData[2] = sin(angle);
    rot.pData[8] = -rot.pData[8];

    return rot;
}

/**
  @brief    create a 4x4 rotation matrix for rotation around Z-axis by some angle (in rad)
  @param    angle   angle around axis (in rad)
  @return   rotation matrix

  Matrix is 4x4 because for use with homogeneous 3D coordinates

  **/
template<typename type> Matrix<type> Matrix<type>::rotZ(double angle)
{
    Matrix<type> rot = Matrix<type>::eye(4);
    rot.pData[0] = cos(angle);
    rot.pData[5] = rot.pData[0];

    rot.pData[4] = sin(angle);
    rot.pData[1] = -rot.pData[4];

    return rot;
}

/**
  @brief    create 4x4 homogeneous rotation matrix with yaw pitch and roll set to parameters (in rad)
  @param    yaw     matrix yaw angle (in rad)
  @param    pitch   matrix pitch angle (in rad)
  @param    roll    matrix roll angle (in rad)
  @return   rotation matrix

  Matrix is 4x4 because for use with homogeneous 3D coordinates

  **/
template<typename type> Matrix<type> Matrix<type>::rotation(double yaw, double pitch, double roll)
{
//    Matrix<type> rot = Matrix<type>::rotX(yaw);
//    rot *= Matrix<type>::rotY(pitch);
//    rot *= Matrix<type>::rotZ(roll);

    double sx = sin(yaw);
    double sy = sin(pitch);
    double sz = sin(roll);
    double cx = cos(yaw);
    double cy = cos(pitch);
    double cz = cos(roll);

    Matrix<type> rot(4,4);
    rot.pData[0] = cy * cz;
    rot.pData[1] = -cx * sz + sx * sy * cz;
    rot.pData[2] = sx * sz + cx * sy * cz;
    rot.pData[3] = 0;

    rot.pData[4] = cy * sz;
    rot.pData[5] = cx * cz - sx * sy * sz;
    rot.pData[6] = -sx * cz + cx * sy * sz;
    rot.pData[7] = 0;

    rot.pData[8] = -sy;
    rot.pData[9] = sx * cy;
    rot.pData[10] = cx * cy;
    rot.pData[11] = 0;

    rot.pData[12] = rot.pData[13] = rot.pData[14] = 0;
    rot.pData[15] = 1;

    return rot;
}

/**
  @brief    create 4x4 homogeneous rotation matrix after rodrigues formula
  @param    x     matrix yaw angle
  @param    y   matrix pitch angle
  @param    z    matrix roll angle
  @return   rotation matrix

  @bug
  **/
template<typename type> Matrix<type> Matrix<type>::rodrigues(double x, double y, double z)
{
    Matrix<type> rot = Matrix<type>::zero(4);
    rot.pData[1] = -z;
    rot.pData[2] = y;
    rot.pData[6] = -x;

    rot.pData[4] = z;
    rot.pData[8] = -y;
    rot.pData[9] = x;

    rot.pData[15] = 1;
    return rot;
}

/**
  @brief    create a 3x3 rotation matrix for rotation around Z-axis by some angle (in rad)
  @param    x   x-movement
  @param    y   y-movement
  @param    z   z-movement
  @return   translation matrix
  **/
template<typename type> Matrix<type> Matrix<type>::trans(double x, double y, double z)
{
    Matrix<type> trans = Matrix<type>::eye(4);
    trans.pData[ 3] = x;
    trans.pData[ 7] = y;
    trans.pData[11] = z;
    return trans;
}


/**
  @brief    create a 3x4 transformation identity matrix
  @return   matrix (1 0 0 0; 0 1 0 0; 0 0 1 0)
  **/
template<typename type> Matrix<type> Matrix<type>::transIdentity()
{
    return Matrix<type>::eye(4,4);
}



/**
  @brief    create scale matrix
  @param    sX   x scale
  @param    sY   y scale
  @param    sZ   z scale
  @return   scale matrix
  **/
template<typename type> Matrix<type> Matrix<type>::scale(double sX, double sY, double sZ)
{
    Matrix<type> trans = Matrix<type>::eye(4);
    trans.pData[ 0] = sX;
    trans.pData[ 5] = sY;
    trans.pData[10] = sZ;

    return trans;
}

#endif // MATRIX_STATICS_HPP
