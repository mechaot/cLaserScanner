/**
    @file	matrix_raytracer.hpp	template implementation of matrix class raytracer bindings


    $Id: matrix_raytracer.hpp 72 2011-05-07 11:13:42Z mechaot $

**/

#ifndef MATRIX_RAYTRACER_HPP
#define MATRIX_RAYTRACER_HPP

#if SIMALI_RAYTRACER_FUNCTIONS

#include "RTvect3D.h"
#include "RTvect3Dh.h"


/**
  @brief    constructor from vector
  @param    v   vector containing data source

  A 3x1 matrix (1 col, 3 rows) is created
**/
template<typename type> Matrix<type>::Matrix(const RTvect3D &v)
{
    pData = NULL;
    pGPU = NULL;
    rows = 0;
    cols = 0;

    Create(1, 3);

    pData[0] = v.x;
    pData[1] = v.y;
    pData[2] = v.z;
}


/**
  @brief    constructor from vector
  @param    v   homogeneous vector containing data source

  A 3x1 matrix (1 col, 3 rows) is created
**/
template<typename type> Matrix<type>::Matrix(const RTvect3Dh &v)
{
    pData = NULL;
    pGPU = NULL;
    rows = 0;
    cols = 0;

    Create(1, 4);

    pData[0] = v.x;
    pData[1] = v.y;
    pData[2] = v.z;
    pData[3] = v.w;
}

/**
  @brief    convert matrix contents to a RT vector
  @param    ok  pointer to bool, if non-NULL there will be stated if conversion was successful
  @return   vector
**/
template<typename type> RTvect3D Matrix<type>::toRTvect3D(bool *ok /* = NULL*/) const
{
    if ((rows * cols) == 3) {   //from integers there is only two possibilties to get 3: one is 1 one is 3, or the other way round
        if (ok) {
            *ok = true;
        }
        return RTvect3D(pData[0], pData[1], pData[2]);
    }
    else {
        if (ok) {
            *ok = false;
        }
        EX_THROW("Matrix form invalid!")
        return RTvect3D(0,0,0);
    }
}

/**
  @brief    convert matrix contents to a RT vector
  @param    ok  pointer to bool, if non-NULL there will be stated if conversion was successful
  @return   vector
**/
template<typename type> RTvect3Dh Matrix<type>::toRTvect3Dh(bool *ok /* = NULL*/) const
{
    if ((rows == 1) && (cols == 4)) {
        if (ok) {
            *ok = true;
        }
        return RTvect3Dh(pData[0], pData[1], pData[2], pData[3]);
    }
    else {
        if (ok) {
            *ok = false;
        }
        return RTvect3Dh(0,0,0,0);
    }
}

/**
    @brief	Matrix assigning, copy constructor
    @param  v   vector containing data
**/
template<typename type> Matrix<type>& Matrix<type>::operator= (const RTvect3D &v)
{
    Clear();
    Create(1, 3);
    pData[0] = v.x;
    pData[1] = v.y;
    pData[2] = v.z;
}

/**
    @brief	Matrix assigning, copy constructor
    @param  v   vector containing data
**/
template<typename type> Matrix<type>& Matrix<type>::operator= (const RTvect3Dh &v)
{
    Clear();
    Create(1, 4);
    pData[0] = v.x;
    pData[1] = v.y;
    pData[2] = v.z;
    pData[3] = v.w;
}

/**
  @brief    right-multiply (column) vector to matrix
  @param    v   vector to multiply
  @throw    throws an exception if matrix dimension does not fit

  Generates a new vector containing the result.

  **/
/*template<typename type> Matrix<type> Matrix<type>::operator* (const RTvect3D &v) const
{
    if ( cols != 3 ) {
        EX_THROW(QString("Invalid Matrix dimension <%1 x %2>!").arg(rows).arg(cols));
    }

    Matrix<type> res(rows,1);
    for(int i = 0; i < rows; i++) {
        res.pData[i] = this->pData[i * cols + 0] * v.x
                     + this->pData[i * cols + 1] * v.y
                     + this->pData[i * cols + 2] * v.z;
    }

    return res;
}*/

/**
  @brief    right-multiply (column) vector to matrix
  @param    v   vector to multiply
  @throw    throws an exception if matrix dimension does not fit

  Generates a new vector containing the result.

  **/
template<typename type> RTvect3D Matrix<type>::operator*(const RTvect3D &v) const
{
    RTvect3D res;
    if ((rows == 4) && (cols == 4)) {
        double w;
        w     = pData[12] * v.x + pData[13] * v.y + pData[14] * v.z + pData[15];
        if(0.0 == w) {
            DEBUG(1,"Invalid transformation matrix");
            return RTvect3D(0,0,0);
        }
        res.x = (pData[0] * v.x + pData[1] * v.y + pData[2] * v.z + pData[3]) / w;        //v.w = 1 (homogeneous coordinates)
        res.y = (pData[4] * v.x + pData[5] * v.y + pData[6] * v.z + pData[7]) / w;
        res.z = (pData[8] * v.x + pData[9] * v.y + pData[10] * v.z + pData[11]) / w;
    } else if ((rows == 3) && (cols == 3)) {
        res.x = pData[0] * v.x + pData[1] * v.y + pData[2] * v.z;
        res.y = pData[3] * v.x + pData[4] * v.y + pData[5] * v.z;
        res.z = pData[6] * v.x + pData[7] * v.y + pData[8] * v.z;
    } else {
        EX_THROW("Invalid matrix dimensions")
    }
    return res;
}


#endif //SIMALI_RAYTRACER_FUNCTIONS


#endif // MATRIX_RAYTRACER_HPP
