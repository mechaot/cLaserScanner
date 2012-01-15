/**
    @file	matrix_operators.hpp	template implementation of matrix class operators


    $Id: matrix_operators.hpp 76 2011-11-16 16:47:33Z mechaot $

**/

#pragma once

#ifndef MATRIX_OPERATORS_HPP
#define MATRIX_OPERATORS_HPP

#include <QString>

/**
	@brief	Add  matrix to this one and return value as a new matrix.
	
	Enable the expresseion	C = A + B;
**/
template<typename type> Matrix<type> Matrix<type>::operator+ (const Matrix<type> &other) const
{
	Matrix<type> result( cols, rows);

        if ( !DimMatch(other) ) {
            EX_THROW( "Matrix dimension mismatch" );
	}
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		result.pData[i] = this->pData[i] + other.pData[i];
	}
	return result;
}

/**
	@brief	Add  scalar one and return value as a new matrix.
	
	Enable the expresseion	C = A + b;
**/
template<typename type> Matrix<type> Matrix<type>::operator+ (const type val) const
{
	Matrix<type> result( cols, rows);
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		result.pData[i] = this->pData[i] + val;
	}
	return result;
}

/**
	@brief	Add another matrix to this one and return reference to this
	
    Enable the expresseion	C += A;
**/
template<typename type> Matrix<type>& Matrix<type>::operator+= (const Matrix<type> &other)
{
	if ( !DimMatch(other) )
	{
        EX_THROW( "Matrix dimension mismatch" );
	}
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
            this->pData[i] += other.pData[i];
	}
	return *this;
}

/**
	@brief	Add a scalar to every object in matrix
	@param	val	value of the scalar
	
    Enable the expresseion	C += a;
**/
template<typename type> Matrix<type>& Matrix<type>::operator+= (const type val)
{
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		this->pData[i] += val;
	}
	return *this;
}

/**
	@brief	Add  matrix to this one and return value as a new matrix.
	
    Enable the expresseion	C = A - B;
**/
template<typename type> Matrix<type> Matrix<type>::operator- (const Matrix<type> &other) const
{
	Matrix<type> result( cols, rows);

	if ( !DimMatch(other) )	{
        EX_THROW("Matrix dimension mismatch");
	}
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		result.pData[i] = this->pData[i] - other.pData[i];
	}
	return result;
}

/**
	@brief	Add  scalar one and return value as a new matrix.
	
    Enable the expresseion	C = A - b;
**/
template<typename type> Matrix<type> Matrix<type>::operator- (const type val) const
{
	Matrix<type> result( cols, rows);

	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		result.pData[i] = this->pData[i] - val;
	}
	return result;
}


/**
	@brief	Add another matrix to this one and return reference to this
	
    Enable the expresseion	C -= A ;
**/
template<typename type> Matrix<type>& Matrix<type>::operator-= (const Matrix<type> &other)
{
	if ( !DimMatch(other) )
	{
        EX_THROW("Matrix dimension mismatch");
	}
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		this->pData[i] -= other.pData[i];
	}
	return *this;
}

/**
	@brief	Substract a scalar from every entry in matrix
	@param	val	value of the scalar
	
    Enable the expresseion	C -= b;
**/
template<typename type> Matrix<type>& Matrix<type>::operator-= (const type val)
{
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		this->pData[i] -= val;
	}
	return *this;
}



/**
	@brief	Multiply matrix with scalar returning value as a new matrix
	
	Enable the expresseion	A *= 2.3;
**/
template<typename type> Matrix<type> Matrix<type>::operator* (const type val) const
{
	Matrix<type> result( cols, rows);

	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		result.pData[i] = this->pData[i] * val;
	}
	return result;
}

/**
	@brief	Right multiply other matrix
	
	Enable the expresseion	this *= other;
**/
template<typename type> Matrix<type>& Matrix<type>::operator*= (const Matrix<type> &other)
{
	if (this->cols != other.rows) {						//due to mathematical definition of matrix product
        EX_THROW(QString("Mismatch in argument dimensionality (%1 x %2) * (%3 x %4) requested").arg(rows).arg(cols).arg(other.rows).arg(other.cols));
	}

	int result_cols = other.cols;
	int result_rows = this->rows;

	Matrix<type> result(result_cols, result_rows);

	type accu;
	for(int y = 0; y < result_rows; y++) {
		for(int x = 0; x < other.cols; x++) {
			accu = 0.0f;
			for(int i = 0; i < this->cols; i++) {
				accu += this->pData[ y * this->cols + i ] * other.pData[ i * other.cols + x ];
			}
            //cout << ".";
			result.pData[y * result_cols + x] = accu;
		}
	}
	*this = result;
	return *this;
}


/**
	@brief	Multiply scalar to every entry in matrix
	@param	val	value of the scalar
	
	Enable the expresseion	C = A * k;
**/
template<typename type> Matrix<type>& Matrix<type>::operator*= (const type val)
{
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		this->pData[i] *= val;
	}
	return *this;
}



/**
	@brief	Divide every matrix element by scalar
	@param	val	value of the scalar
	
	Enable the expresseion	C = A * k;
**/
template<typename type> Matrix<type>& Matrix<type>::operator/= (const type val)
{
	int size = cols * rows;
	for(int i = 0; i < size; i++) {
		this->pData[i] /= val;
	}
	return *this;
}

/**
	@brief	Right multiply other matrix
	
	Enable the expresseion	C = this * other;
**/
template<typename type> Matrix<type> Matrix<type>::operator* (const Matrix<type> &other) const
{
	if (this->cols != other.rows) {						//due to mathematical definition of matrix product
        EX_THROW( QString("Mismatch in argument dimensionality *this: %1 x %2; other: %3 x %4" ).arg(rows).arg(cols).arg(other.rows).arg(other.cols));
	}

	int result_cols = other.cols;
	int result_rows = this->rows;

	Matrix<type> result(result_cols, result_rows);

	type accu;
	for(int y = 0; y < result_rows; y++) {
		for(int x = 0; x < other.cols; x++) {
			accu = 0.0f;
			for(int i = 0; i < this->cols; i++) {
				accu += this->pData[ y * this->cols + i ] * other.pData[ i * other.cols + x ];
			}
            //cout << ".";
			result.pData[y * result_cols + x] = accu;
		}
	}

	return result;
}

/**
	@brief	Matrix assigning, copy constructor
	
	Enable the expresseion	A *= 2.3;
**/
template<typename type> Matrix<type>& Matrix<type>::operator= (const Matrix<type> &other)
{
	if ( !DimMatch(other) ) {
		this->Clear();
		this->Create(other.cols, other.rows);
	}
//    if (typeid(*this) == typeid(other)) {       // matrices of same type: speed up using memcopy
        memcpy(this->pData, other.pData, cols * rows * sizeof(type));
//    } else {                                    // matrices of different type: slow version type-converting each element
//        for (int i = 0; i < (cols*rows); i++) {
//            pData[i] = other.pDate[i];
//        }
//    }

	return *this;
}

/**
	@brief	Matrix comparison
	
	Enable the expresseion	A *= 2.3;
**/
template<typename type> bool Matrix<type>::operator== (const Matrix<type> &other)
{
	if ( !DimMatch(other) ) {
		return false;
	}
	int size = cols * rows;
	for (int i = 0; i < size; i++) {
		if (this->pData[i] != other.pData[i])
			return false;
	}

	return true;
}


/**
    @brief provide convenient accees to matrix elements
    @param	p   position in pData
    @return reference to matrix element
**/
template<typename type> inline type& Matrix<type>::operator[] (int idx)
{
    return pData[idx];
}

/**
    @brief provide convenient accees to matrix elements
    @param	p   position in pData
    @return matrix element
**/
template<typename type> inline type Matrix<type>::operator[] (int idx) const
{
    return pData[idx];
}


/**
    @brief provide convenient accees to matrix elements
    @param	p   position in pData
    @return reference to matrix element
**/
template<typename type> inline type& Matrix<type>::operator() (int p)
{
    return pData[p];
}

/**
    @brief provide convenient accees to matrix elements
    @param	p   position in pData
    @return matrix element
**/
template<typename type> inline type Matrix<type>::operator() (int p) const
{
    return pData[p];
}

/**
	@brief provide convenient accees to matrix elements
	@param	c	column number
	@param	r	row number
	@return reference to matrix element
**/
template<typename type> inline type& Matrix<type>::operator() (int c, int r)
{
    return pData[r * cols + c];
}

/**
    @brief provide convenient accees to matrix elements
    @param	c	column number
    @param	r	row number
    @return matrix element
**/
template<typename type> inline type Matrix<type>::operator() (int c, int r) const
{
    return pData[r * cols + c];
}


#endif //MATRIX_OPERATORS_HPP

