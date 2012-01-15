/**
    @file	matrix_raytracer.hpp	template implementation of matrix class raytracer bindings
    @author Andreas Poesch (itdevel@mechaos.de)
    @since  2010-01-06
    @brief  lapack-enhanced matrix functions

    All matrix functions that use lapack in any way (directly or subcall) are to be defined here to easily disable
    lapack usage

    $Id: matrix_lapack.hpp 72 2011-05-07 11:13:42Z mechaot $

**/

#ifndef MATRIX_LAPACK_HPP
#define MATRIX_LAPACK_HPP

#if SIMALI_USE_LAPACK


/**
  @brief    perform LU decomposition
  @return   returns a matrix composed of U (upper right triangle) and L (lower right triangle, with 1's omitted)
  @note     function is only available if LAPACK is used

  Enhanced by LAPACK function DGETRF
  **/
template<typename type> Matrix<type> Matrix<type>::LU()
{
    using namespace lapack;

    if ((rows < 1) || (cols < 1)) {
        EX_THROW("LU factoization of empty matrix.");
    }
    int min = (rows < cols) ? rows : cols;      //the smaller dimension

    Matrix<integer>  P( 1,min );            // permutation array
    Matrix<type>     A( (*this) );          // target array, we don't want to destroy (*this)


//    /* Subroutine */ int dgetrf_(integer *m, integer *n, doublereal *a, integer *lda, integer *ipiv, integer *info);

    integer info;
    integer M = A.rows;
    integer N = A.cols;

    dgetrf_( &M, &N, A.pData, &M, P.pData, &info );         //LAPACK
    if (info != 0) {
        CONSOLE(1,QString("LU factorization failed"));
        EX_THROW("LU factorization failed");
    }

    return A;
}

/**
    @brief	Calculate Determinante of matrix
    @return determinante value
    @note     function is only available if LAPACK is used

    Determinante is calculated using LAPACK function DGETRF
**/
template<typename type> type Matrix<type>::Det()
{
    using namespace lapack;

    type det = 0;

    if ( (pData == NULL) || (cols != rows) ) {
        EX_THROW( "Deteminant is only defined for square matrices");
    }
    if ((cols == 0) || (rows == 0)) {
        EX_THROW("Determinant calculation of empty matrix skipped.")
        return 0;
    }
    if (rows != cols) {
        EX_THROW(QString("Error: Cannot calculate determinant of non-square (%1 x %2) matrix.").arg(cols).arg(rows));
    }

    int min = rows;      //the smaller dimension

    Matrix<integer>  P(1,min);
    Matrix<type> A( (*this) );
/*
    int signum = (type) this->UL(&U);     //start with the correct signum
    det = U.Trace() * signum;
*/



//    /* Subroutine */ int dgetrf_(integer *m, integer *n, doublereal *a, integer *
//        lda, integer *ipiv, integer *info);

    integer info;
    integer M = A.rows;
    integer N = A.cols;

    dgetrf_( &M, &N, A.pData, &M, P.pData, &info );


    int sig = 1;    //calculate "signum" as integer because it's faster than double
    for(int i = 0; i < M; i++) {
        if (P.pData[i] != (i+1)) {      //if not "interchanged with self" there is a change of sign; remember that lapack counts starting by 1
            sig = -sig;
        }
    }
    det = (type) sig * A.Trace();

    if (info != 0) {
        CONSOLE(1,QString("WARNING Calculation of determinant failed: Det = %1").arg(det));
        if (det)    //seems that a det of 0.0 will possibly show an error because lapack thinks it's numerically unstable
            CONSOLE(1,QString("WARNING Calculation of determinant failed: Det = %1").arg(det));
    }

    return det;
}


/**
  @brief    perform QT decomposition
  @param    R the upper triangle output matrix will be stored in an object pointed to by R
  @param    Q the rotation matrix will be stored here
  @return   returns 0 on success, non-zero otherwise
  @note     function is only available if LAPACK is used
  @bug      untested, never called

  Enhanced by LAPACK functions DGEQRF and DORGQR
  **/
template<typename type> int Matrix<type>::QR(Matrix<type> *Q, Matrix<type> *R)
{
    using namespace lapack;

    Matrix<type> A(*this);

    int lda = rows;

    int min_dim = rows > cols ? cols : rows;
    double* tau = new double[min_dim];
    ASSERT(tau);

    int info;
    int work_size = -1;              //-1 for best work-size query
    double* work = new double[1];
    ASSERT(work);

    //workspace size query
    dgeqrf_(&rows, &cols, A.pData, &lda, tau, work, &work_size, &info);     //lapack QR

    work_size = work[0];    //now we know the best work size
    delete [] work;
    work = NULL;

    work = new double[work_size];
    ASSERT(work);

    //qr decomposition (stage 1)
    dgeqrf_(&rows, &cols, A.pData, &lda, tau, work, &work_size, &info);     //lapack QR
    if (info < 0) {
        EX_THROW(QString("QR decomposition stage 1: Error in argument # %1: invalid value (matrix non-symmetric?)").arg(-info));
    }
    if (R) {
        *R = A.upperTriangular();
    }

    //calculate rotation part explicitely
    if (Q) {
        //stage 2
        dorgqr_(&A.rows, &A.cols, &min_dim, A.pData, &A.rows, tau, work, &work_size, &info);    //lapack reconstruct Q
        if (info < 0) {
            EX_THROW(QString("QR decomposition stage 2: Error in argument # %1: invalid value (matrix non-symmetric?)").arg(-info));
        }

        *Q = A;
    }

    delete [] tau;
    delete [] work;

    return 0;
}

/**
    @brief	calcualate inverse matrix
    @note   Calculation is done via lapack
    @note the value for local variable 'nb' is taken out of the fortran source of ialenv_ and hard coded to 64

    @throw	throws exception if matrix is singular (non-invertible)

**/
template<typename type> Matrix<type> Matrix<type>::Inv() const
{
    using namespace lapack;

    if ((rows < 1) || (cols < 1)) {
        EX_THROW("Inv(): LU factoization of empty matrix.");
    }
    integer min = (rows < cols) ? rows : cols;      //the smaller dimension

    Matrix<integer>  P( 1,min );            // permutation array
    Matrix<type>     A( (*this) );          // target array, we don't want to destroy (*this)


//    /* Subroutine */ int dgetrf_(integer *m, integer *n, doublereal *a, integer *lda, integer *ipiv, integer *info);

    integer info;
    integer M = A.rows;
    integer N = A.cols;

    dgetrf_( &M, &N, A.pData, &M, P.pData, &info );         //LAPACK
    if (info != 0) {
        CONSOLE(2,"LU factorization failed. Cannot determine Inverse");
        EX_THROW("LU factorization failed. Cannot determine Inverse");
    }

    if ( MAT_IS_ZERO( A.Trace() ) ) {
        CONSOLE(2,"Warning: Matrix is almost singlar. Results may be inaccurate.")
    }


    //integer ispec = 1;
    //character name[] = "DGETRI\0";
    //character opts[] = "UTN";
    //integer dummy = -1;
    //integer nb = ialenv_(&ispec, name, &min, &dummy, &dummy, &dummy); //should normally evaluate to 64

    integer nb = 64; // taken out of fortran source code of ialenv_ ;


    integer lwork = min * nb;
    Matrix<type> workspace(1, lwork);

    ///* Subroutine */ int dgetri_(integer *n, doublereal *a, integer *lda, integer *ipiv, doublereal *work, integer *lwork, integer *info);
    dgetri_ (&min, A.pData, &min, P.pData, workspace.pData, &lwork, &info );
    if (info != 0) {
        EX_THROW("Matrix inversion failed.");
    }

    return A;
}

/**
    @brief	calcualate moore-penrose pseude-inverse matrix
    @bug		algorithm is numberically pretty unstable
**/
template<typename type> Matrix<type> Matrix<type>::Pinv() const
{

    Matrix<type> A(*this);
    Matrix<type> tmp = A.T() * A;	//if A.isReal -> adjugierte Matrix == A.T();
    tmp = tmp.Inv();
    return  tmp * A.T();
}


#else
template<typename type> Matrix<type> Matrix<type>::LU()
{
    EX_THROW(1,"Lapack not used: function is not available in this configuration");
    return *this;
}

template<typename type> type Matrix<type>::Det()
{
    EX_THROW(1,"Lapack not used: function is not available in this configuration");
    return 0;
}

template<typename type> int Matrix<type>::QR(Matrix<type> *Q, Matrix<type> *R)
{
    EX_THROW(1,"Lapack not used: function is not available in this configuration");
    return 0;
}

template<typename type> Matrix<type> Matrix<type>::Inv() const
{
    EX_THROW(1,"Lapack not used: function is not available in this configuration");
    return *this;
}
#endif // SIMALI_USE_LAPACK

#endif // MATRIX_LAPACK_HPP
