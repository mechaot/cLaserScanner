/**
	@file	matrix.hpp	template implementation of matrix class


    $Id: matrix.hpp 73 2011-05-31 13:14:31Z mechaot $

**/

#pragma once

#ifndef MATRIX_HPP
#define MATRIX_HPP


/*! 
  @brief   Parameter less contstructor for Matrix class
  @details Creates an empty Matrix with all pointers set to NULL and all integers initialized
            to zero
 **/
template<typename type> Matrix<type>::Matrix()
{
    #if _DEBUG_MATRIX >= 2
	   cout << "Creating empty matrix. Instances: " << ++instances << "  " << endl;
	#endif
    pData = NULL;
    pGPU = NULL;
    rows = 0;
    cols = 0;
}



/*!
 @brief   Contstructor for two dimensional Matrix class by ascii string
 @details Creates a two dimensional Matrix by calling Create()
 @param   str string to load
 **/
template<typename type> Matrix<type>::Matrix(const char* str)
{
    #if _DEBUG_MATRIX >= 2
	   cout << "Creating matrix from string. Instances: " << ++instances << "  " << endl;
	#endif
    pData = NULL;
    pGPU = NULL;
    rows = 0;
	cols = 0;

	Load(str);	
}

/*!
  @brief   Destructor for Matrix class
  @details Ensures that all data pointers are deleted and memory is freed.
            Simply calls Clear() 
**/
template<typename type> Matrix<type>::~Matrix()
{
    #if _DEBUG_MATRIX >= 2
      cout << "Destructor of " << rows << " x " << cols << " matrix called. " << (--instances) << " instances left" << endl;
    #endif
    Clear();
}

/*! 
  @brief   Copy contstructor for Matrix class (will oftenly be called hiddenly)
  @details Creates a new Matrix with data areas copied from reference matrix
**/
template<typename type> Matrix<type>::Matrix(const Matrix<type>& Mat)
{
    #if _DEBUG_MATRIX >= 2
	   cout << "Copy-Constructing matrix. Instances: " << ++instances << "  " << endl;
	#endif
    pData = NULL;
    pGPU = NULL;
    rows = 0;
	cols = 0;

    Create(Mat.cols, Mat.rows);

    if (pData)	{
        memcpy(pData, Mat.pData, cols * rows * sizeof(type));
    }
}

/**
  @brief    create from opencv-matrix
  @param    img reference to opencv object
  @param    toGrayscale if set true then colored (=3 channel) img will be flattened to grayscale images, if false then
            (*this) will have 3*img.cols columns
            for grayscale input images this param is ignored

  @note     only grayscale 8-bit image matrices are supported yet
  **/
template<typename type> Matrix<type>::Matrix(const cv::Mat &img, bool toGrayscale /*= true*/)      // open-cv
{
    pData = NULL;
    pGPU = NULL;
    rows = 0;
    cols = 0;

    if ((1 == img.channels()) && (img.depth() == CV_8U)) {
        Create(img.cols, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        uchar* src = 0;
        CONSOLE(4,"Converting CV_8U (8-bit unsigned) with 1 channel");
        for(int y = 0; y < rows; y++) {		
            src = (uchar*) img.ptr(y);		//get pointer to row y of the cv::Mat-Image
            for(int x = 0; x < cols; x++) {  //walk alony x-direction
                *(dst++) =  *(src++);
            }
        }
    } else if ((1 == img.channels()) && (img.depth() == CV_64F)) {
        Create(img.cols, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        double* src = 0;
        CONSOLE(4,"Converting CV_F64 (floating point) with 1 channel");
        for(int y = 0; y < rows; y++) {
            src = (double*) img.ptr(y);
            for(int x = 0; x < cols; x++) {
                *(dst++) =  *(src++);
            }
        }
    } else if ((3 == img.channels()) && (img.depth() == CV_8U) && toGrayscale) {
        Create(img.cols, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        uchar* src = 0;
        int sum;
        CONSOLE(4,"Converting CV_8U (8-bit unsigned) with 3 channels; to grayscale");
        for(int y = 0; y < rows; y++) {
            src = (uchar*) img.ptr(y);
            for(int x = 0; x < cols; x++) {
                sum = 0;
                for(int c = 0; c < 3; c++) {    //mix channels to gray value = 1/3(r + g+ b)
                    sum += *(src++);
                }
                sum = (sum + 1) / 3;    //rounding with mathematic precision
                *(dst++) = (type) sum;
            }
        }
    } else if ((3 == img.channels()) && (img.depth() == CV_8U) && (!toGrayscale)) {
        Create(img.cols*3, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        uchar* src = 0;
        register type value;
        CONSOLE(4,"Converting CV_8U (8-bit unsigned) with 3 channels");
        for(int y = 0; y < rows; y++) {
            src = (uchar*) img.ptr(y);
            for(int x = 0; x < (cols*3); x++) {
                value =  *(src++);
                *(dst++) =  value;
            }
        }
    } else if ((3 == img.channels()) && (img.depth() == CV_64F) && toGrayscale) {
        Create(img.cols, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        double* src = 0;
        double sum;
        CONSOLE(4,"Converting CV_F64 (floating point) with 3 channels; to grayscale");
        for(int y = 0; y < rows; y++) {
            src = (double*) img.ptr(y);
            for(int x = 0; x < cols; x++) {
                sum = 0;
                for(int c = 0; c < 3; c++) {    //mix channels to gray image
                    sum += (*(src++));
                }
                sum /= 3.0;
                *(dst++) = (type) sum;
            }
        }
    } else if ((3 == img.channels()) && (img.depth() == CV_64F) && (!toGrayscale)) {
        Create(img.cols*3, img.rows);
        type *dst = pData;
        ASSERT_THROW(dst);

        double* src = 0;
        CONSOLE(4,"Converting CV_F64 (floating point) with 3 channels");
        for(int y = 0; y < rows; y++) {
            src = (double*) img.ptr(y);
            for(int x = 0; x < cols*3; x++) {
                *(dst++) = (type) (*(src++));
            }
        }
    } else {
        EX_THROW(QString("Invalid image type; channels: %1, depth: %2").arg(img.channels()).arg(img.depth()));
    }
}


/*!
 @brief   Contstructor for two dimensional Matrix class
 @param   c  cols of Matrix  (X)
 @param   r  rows of Matrix  (Y)

 Creates a two dimensional Matrix by calling Create()
**/
template<typename type> Matrix<type>::Matrix(int c, int r)
{
    #if _DEBUG_MATRIX >= 2
	   cout << "Creating empty matrix. Instances: " << ++instances << "  " << endl;
	#endif
    pData = NULL;
    pGPU = NULL;
    rows = 0;
	cols = 0;

    Create(c, r);
}

/**
  @brief    deep copy to new array
  @return   new matrix of same type and content
  **/
template<typename type> Matrix<type> Matrix<type>::Copy() const
{
    Matrix<type> ret(this->cols, this->rows);
    memcpy( ret.pData, pData, rows * cols* sizeof(type) );
    return ret;
}

/**
  @brief    shallow copy to new array
  @return   new matrix of same type and shared data pointer
  **/
template<typename type> Matrix<type> Matrix<type>::ShallowCopy() const
{
    Matrix<type> newmat;
    newmat.rows = rows;
    newmat.cols = cols;
    newmat.pGPU = pGPU;
    newmat.pData = pData;
    newmat.strName = strName;
    newmat.mLocation = mLocation;
    return newmat;
}

/**
  @brief    determine if matrix contains at least one valid element
  @return   false is matrix has no data or is of size 0
  **/
template<typename type> bool Matrix<type>::isEmpty() const
{
    if(NULL == pData)
        return true;
    if (rows <= 0)
        return true;
    if (cols <= 0)
        return true;
    return false;
}

/**
	@brief	check if this matrix is additively compatible to other matrix (i.e. contains same dimensionality)
	@param	other	other matrix to be added
**/
template<typename type> const bool Matrix<type>::DimMatch(const Matrix<type> &other) const		//are 2 matrices dimensionally equal (i.e. can they be added, substracted, etc...); i.e. same number of dimensions
{
	if ((cols == other.cols) && (rows == other.rows))
		return true;
	else
		return false;
}



/*!
  @brief   This function handles allocation of memory
  @param   rs   rows desired
  @param   cs   columns desired

  This function takes the dimension vector and its size and does all
            work for momory allocation and error checking
**/
template<typename type> int Matrix<type>::Create(const int cs, const int rs)
{
#if _DEBUG_MATRIX >= 2
        cout << __FUNCTION__ << "with " << cs << " cols and " << rs << " rosw " << endl;
#endif

#if _DEBUG_MATRIX >= 1
   if ( (rows * cols) || (pData) )
    {
       CONSOLE(10,QString("Warning. Resizing existant matrix (%1,%2).").arg(cs).arg(rs));
       //Clear();
    }
#endif
 
   if ( cs < 0 ||rs < 0 ) {
        throw "Cannot create Matrix with negative dimensions in Matrix<type>::Create(const int cs, const int rs)";
   }
  
   rows = rs;
   cols = cs;

   if (pData) {				//when resizing matrix, some data could already be allocated, discard this  memory
	   delete [] pData;
	   pData = NULL;
   }
    /*** data vector ***/
    if(rows * cols) {
		pData = (type*) malloc( rs * cs * sizeof(type) );    //allocate matrix
                ASSERT_THROW(pData);
	} else {
		#if _DEBUG_MATRIX >= 2
			cout << "Warning: empty matrix has pData pointing to NULL" <<endl;
		#endif
	}
    #if _DEBUG_MATRIX >= 2
        cout << rows << "x" << cols << " matrix created. "<< cols*rows <<" elements using "<< cols*rows*sizeof(type)<<" bytes in memory" <<endl;
    #endif
    /*** load scalar members ***/
    return 0;
}

/*!
  @brief   Clears matrix
  @details free all allocated memory areas
**/
template<typename type> void Matrix<type>::Clear()
{
    #if _DEBUG_MATRIX >= 2
	cout << "Clearing matrix. " << endl;
    #endif
    if (pData)
    {
       delete [] pData;
       pData = NULL;
    }
	rows = 0;
	cols = 0;
}

/**
  @brief    get matrix element count
  @return   rows * cols

  if rows or cols should contain a negative value (which is impossible normally) zero is returned
**/
template<typename type>int Matrix<type>::Size() const
{
    if (rows <= 0)
        return 0;
    if (cols <= 0)
        return 0;
    return rows * cols;
}

/**
  @brief    get pointer to data row
  @param    n   row number counting from 0
  @return   pointer to first element of row n of internal data structure pData
**/
template<typename type> type* Matrix<type>::Row(int n)
{
    type *ret = pData + (n*cols);
    return ret;
}

/*!
  @brief   Set all matrix elements to zero
  @details memset data area to zero in binary mode

	@warning	this method may be harmful with complex template types, consider another algorithm
**/
template<typename type> void Matrix<type>::SetZero()
{
    #if _DEBUG_MATRIX >= 2
      cout << __FUNCTION__ << endl;
    #endif
    if (pData)
    {
		memset(pData, 0, rows * cols * sizeof(type));

		// for(int i = 0; i < rows*cols; i++) pData[i] = 0;			//would be more safe for complex types but also slower
    }
}

/*!
  @brief   Set the value for a specific 1D data element
  @param   sx    index of data element to set
  @param   val   value that is to be stored
 
	Function checks if matrix is properly allocated and if coordinates are valid and therefore
	it is quite slow

**/
template<typename type> void Matrix<type>::Set(int sx, type val)
{
    if (((rows == 1) && (cols > 0)) || ((cols == 1) && (rows > 0)))		//if 1xn or nx1 vector and not null-vector
    {
       EX_THROW("Error: singleton set for non-vector");
    }
	if ( (sx >= (rows*cols)) || (sx < 0) ) {
#if _DEBUG_MATRIX >= 1
        CONSOLE(2,"Index out of range. Aborting");
#endif
	} else {
		pData[sx] = val;
	}
}

/*!
  @brief   Set the value for a specific 2D data element (pixel)
  @param   sx    first dimension coordinate of data element to set (X-Coordinate)
  @param   sy    second dimension coordinate of data element to set (Y-Coordinate)
  @param   val   value that is to be stored
 
	Function checks if matrix is properly allocated and if coordinates are valid and therefore
	it is quite slow
**/
template<typename type> void Matrix<type>::Set(int sx, int sy, type val)
{
    if ( (sx >= cols) || (sx < 0) || (sy >= rows) || (sy < 0))
    {
#if _DEBUG_MATRIX >= 1
       CONSOLE(2,"Index out of range. Aborting");
	   return;
#endif
    }
    pData[sy*cols + sx] = val;
}


/*!
  @brief   Retrieve value of a specific data element in a 1D matrix
  @return  value of the element
  @param   sx    position of the desired element
 
	Function checks if matrix is properly allocated and if coordinates are valid and therefore
	it is quite slow

**/
template<typename type> const type Matrix<type>::Get(int sx) const
{
    if (((rows == 1) && (cols > 0)) || ((cols == 1) && (rows > 0)))		//if 1xn or nx1 vector and not null-vector
    {
       EX_THROW("Error: singleton get for non-vector");
    }
	if ( (sx >= (rows*cols)) || (sx < 0) ) {
#if _DEBUG_MATRIX >= 1
        CONSOLE(2,"Index out of range. Aborting");
#endif
        EX_THROW("Get out of range");
	} else {
		return pData[sx];
	}
}

/*!
  @brief   Retrieve value of a specific data element in a 2D matrix
  @return  value of the element
  @param   sx    first dimension coordinate of the desired element
  @param   sy    second dimension coordinate of the desired element
 
	Function checks if matrix is properly allocated and if coordinates are valid and therefore
	it is quite slow

**/
template<typename type> const type Matrix<type>::Get(int sx, int sy) const
{
   if ( (sx >= cols) || (sx < 0) || (sy >= rows) || (sy < 0))
   {
       EX_THROW("Error: Get-indices out of range.");
       return 0;
    }
    return pData[sy*cols + sx];
}

/*!
  @brief   Retrieve value of the minimum-element within Matrix
  @return  value of the mimimum-element
  @param   idx    if not given (=NULL) this parameter is ignored
                   if given the address of an int the position of the first 
                   occurence of the minimum value is written to that address

    Lineary walks through data array from beginning to end
**/
template<typename type> type Matrix<type>::Min(int *idx) const
{
	if ((pData == NULL)  ||  (rows*cols == 0)) {
        EX_THROW("Min called for empty matrix");
	}
    int size = rows * cols;
    type m;
    m = pData[0];

    for (int i = 1; i < size; i++)
    {
        if ( pData[i] < m )
        {
             m = pData[i];
             if (idx) *idx = i;
        }
    }
    return m;
}

/*!
  @brief   Retrieve value of the maximum-element within Matrix
  @return  value of the maximum-element
  @param   idx    if not given (=NULL) this parameter is ignored
                   if given the address of an int the position of the first 
                   occurence of the maximum value is written to that address

Lineary walks through data array from beginning to end
**/
template<typename type> type Matrix<type>::Max(int *idx) const
{
	if ((pData == NULL)  ||  (rows*cols == 0)) {
        EX_THROW("Max called for empty matrix");
	}
    int size = rows * cols;
    type m;
    m = pData[0];

    for (int i = 1; i < size; i++)
    {
        if ( pData[i] > m )
        {
             m = pData[i];
             if (idx) *idx = i;
        }
    }
    return m;
}

/**
	@brief	sum up all matrix elements
	@param	overflow : defaults to NULL
			if valid: perform overflow checks,
				then it returns true on overflow detected
								false on no overflow detected
			if NULL: don't perform overflow check
				then output remains false
	@bug	overflow detection neither testet nor thought over

	@return	sum of all components

**/
template<typename type> type Matrix<type>::Sum(bool *overflow /* = NULL*/) const
{
	int sz = cols * rows;
	type ret = 0;
	if (overflow) {
		*overflow = false;
		for(int i = 0; i < sz; i++) {
			type temp = pData[i];
			if( (temp > 0) && (temp+ret < ret))
			{
				*overflow = true;
			}
			if( (temp < 0) && (temp-ret > ret))
			{
				*overflow = true;
			}
			ret += temp;
		}
	} else {
		for(int i = 0; i < sz; i++) {
			ret += pData[i];
		}
	}
	return ret;
}

/**
  @brief    calculate row minima
  @return   vertical 1xN vector containing row minima
**/
template<typename type> Matrix<type> Matrix<type>::RowMinima()     // vertical vector or row minima
{
    Matrix<type> mins = SubMatrix(0,0,1,rows);

    for(int c = 1; c < cols; c++) {
        for(int r = 0; r < rows; r++) {
            if (pData[r*cols + c] < mins.pData[r])
                mins.pData[r] = pData[r*cols + c];
        }
    }

    return mins;
}

/**
  @brief    calculate row maxima
  @return   vertical 1xN vector containing row maxima
**/
template<typename type> Matrix<type> Matrix<type>::RowMaxima()     // vertical vector of row maxima
{
    Matrix<type> maxs = SubMatrix(0,0,1,rows);

    for(int c = 1; c < cols; c++) {
        for(int r = 0; r < rows; r++) {
            if (pData[r*cols + c] > maxs.pData[r])
                maxs.pData[r] = pData[r*cols + c];
        }
    }

    return maxs;
}

/**
  @brief    calculate row sums
  @return   vertical 1xN vector containing row sums
**/
template<typename type> Matrix<type> Matrix<type>::RowSums()     // vertical vector of row sums
{
    Matrix<type> sums = SubMatrix(0,0,1,rows);

    for(int r = 0; r < rows; r++) {
        for(int c = 1; c < cols; c++) {
            sums.pData[r] += pData[r*cols + c];
        }
    }

    return sums;
}

/**
  @brief    sum of all elements of a specific row
  @param    row row of interest
  @return   sum(rowelements), 0 if row is out of range
  **/
template<typename type> type Matrix<type>::RowSum(int row)
{
    type sum = 0;
    if ( (row < 0) || (row >= rows) ) {
        EX_THROW("Row out of valid range");
    }
    for (int x = 0; x < cols; x++) {
        sum += pData[row * cols + x];
    }
    return sum;
}

/**
  @brief    sum of absolute of all elements of a specific row
  @param    row row of interest
  @return   sum(abs(rowelements)), 0 if row is out of range
  **/
template<typename type> type Matrix<type>::RowSumAbs(int row)
{
    type sum = 0;
    if ( (row < 0) || (row >= rows) ) {
        EX_THROW("Row out of valid range");
    }
    for (int x = 0; x < cols; x++) {
        sum += abs(pData[row * cols + x]);
    }
    return sum;
}


/**
  @brief    calculate col minima
  @return   horizontal 1xN vector containing column minima
**/
template<typename type> Matrix<type> Matrix<type>::ColMinima()
{
    Matrix<type> mins = SubMatrix(0,0,cols,1);

    for(int c = 0; c < cols; c++) {
        for(int r = 1; r < rows; r++) {
            if (pData[r*cols + c] < mins.pData[c])
                mins.pData[c] = pData[r*cols + c];
        }
    }

    return mins;
}

/**
  @brief    calculate col maxima
  @return   horizontal 1xN vector containing column maxima
**/
template<typename type> Matrix<type> Matrix<type>::ColMaxima()
{
    Matrix<type> maxs = SubMatrix(0,0,cols,1);

    for(int c = 0; c < cols; c++) {
        for(int r = 1; r < rows; r++) {
            if (pData[r*cols + c] > maxs.pData[c])
                maxs.pData[c] = pData[r*cols + c];
        }
    }

    return maxs;
}

/**
  @brief    calculate col sums
  @return   horizontal 1xN vector containing column sums
**/
template<typename type> Matrix<type> Matrix<type>::ColSums()
{
    Matrix<type> sums = SubMatrix(0,0,cols,1);

    for(int c = 0; c < cols; c++) {
        for(int r = 1; r < rows; r++) {
            sums.pData[c] += pData[r*cols + c];
        }
    }

    return sums;
}

/**
  @brief    sum of all elements of a specific col
  @param    col col of interest
  @return   sum(colelements), 0 if col is out of range
  **/
template<typename type> type Matrix<type>::ColSum(int col)
{
    type sum = 0;
    if ( (col < 0) || (col >= cols) ) {
        EX_THROW("Row out of valid range");
    }
    for (int y = 0; y < rows; y++) {
        sum += pData[y * cols + col];
    }
    return sum;
}

/**
  @brief    sum the absolute value of all elements of a specific col
  @param    col col of interest
  @return   sum(abs(colelements)), 0 if col is out of range
  **/
template<typename type> type Matrix<type>::ColSumAbs(int col)
{
    type sum = 0;
    if ( (col < 0) || (col >= cols) ) {
        EX_THROW("Row out of valid range");
    }
    for (int y = 0; y < rows; y++) {
        sum += abs(pData[y * cols + col]);
    }
    return sum;
}

/**
	@brief	output matrix to console
**/
template<typename type> void Matrix<type>::Print()
{
	if ((pData == NULL)  ||  (rows*cols == 0)) {
#if _DEBUG_MATRIX >= 1
        CONSOLE(1, QString("Matrix empty"));
#endif
		return;
	}
	

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
            std::cout << pData[y * cols + x] << "\t";
		}
        std::cout << endl;
	}
    std::cout << flush;
}



/**
	@brief	load a matrix element from text
	@param	col		column to pass the value to
	@param	row		row to pass the value to
	@param	str		string containing ascii-represantation of value
	@param	len		length of string, if Zero length will be determined by terminating NULL char in str
	@return	NULL if conversion failed
			pointer to data-cell if conversion succeeded

	@bug	as template specialization is evil (linker problems) it's non-specialized but works for type "double" (and maybe compatibles) only

	This function fits to have Load() be type-independend, this function may need specialization for different types
**/
template<typename type> inline type* Matrix<type>::LoadElementFromString(int col, int row, char* str, int len)
{
	double v = 0.0f;

	/*debug*/

    char temp = str[len];       //save char
    str[len] = 0;               //teminate for sscanf security

    if (0 == sscanf(str,"%lf",&v)) {
        EX_THROW("conversion failed");
    }

    str[len] = temp;            //restore char
	return  &(pData[row * cols + col] = v);
}


/**
  @brief    load matrix content from file
  @param    fileName        name of file to load from
  **/
template<typename type> Matrix<type>& Matrix<type>::fromFile(const char* fileName)
{
    FILE * pFile = NULL;
    pFile = fopen (fileName , "r");

    if (pFile == NULL) {
        DEBUG(1, QString("Error opening file %1").arg( fileName));
        Clear();
        return *this;
    }
    fseek( pFile, 0L, SEEK_END );
    long fileSize = ftell( pFile );
    fseek( pFile, 0L, SEEK_SET );

    char* str = new char[fileSize+2];

    long read = fread(str, 1, fileSize, pFile);
    fclose(pFile);

    str[fileSize] = 0;

    this->Load(str);
    delete[] str;
    return *this;
}

/**
	@brief	load RECTANGULAR matrix from string
	@param	str
	@return	reference to self

	The matrix is resized to fit the dimensions in the string, so size needs not to be known
**/
template<typename type> Matrix<type>& Matrix<type>::Load(const char* str)
{
	char *s = new char[strlen(str)+2];			//copy for working and manipulating;
    strncpy(s, str, strlen(str));				//safe copy function

	char rowsep[] = ";\n";			//these characters work as row separators (newline and ;)
	char whitespaces[] = " \t";		//these characters are allowed as whitespace characters in sequences of length 1+


	int mat_cols = 0;		//number of matrix columns
	int mat_rows = 0;		//number of matrix rows

	//count rows and cols in matrix and check if all rows are of equal element-count
	int words = 0;
	int words_this_row = 0;
	bool last_was_whitechar = true;					//so if the first char is a non-whitechar it's recognized as start of a word
	for (int i = 0; i < (int) strlen(str); i++) {			//count rows 
		if (s[i] == ',') {							//replace commas by points (to comply with german notation)
			s[i] = '.';
			continue;
		}
		if ( strchr(rowsep, s[i]) != NULL ) {		//found a row separator
			if (words_this_row != mat_cols) {
                EX_THROW(QString("Error invalid number of columns/elements in this row (%1)").arg(i));
			}
			++mat_rows;								//count rows
			words_this_row = 0;
			last_was_whitechar = true;				//rowseparator also acts as whitespace (can separate elements)
			continue;								//no need to test for whitespace any more, indeed this could cause harm so wrap-over
		}		

		if ( strchr(whitespaces, s[i]) == NULL ) {  //if no whitespace
			if (last_was_whitechar) {
				++words;
				++words_this_row;
				if(mat_rows == 0) {					//first row is used to determine number of columns
					++mat_cols;
				}
			} 
			last_was_whitechar = false;
		} else {									//if whitespace
			last_was_whitechar = true;
		}
	}

	//test for ending of last line
	if ((words_this_row == mat_cols) && (mat_cols)) {	//only increase row number if matrix has cols- an empty matrix of size(0,0) would gain a row else because words_this_row==mat_cols==0 would yield
		++mat_rows;			//last row had no terminating character, so ind row-counter
	} else if (words_this_row == 0) {
		;					//terminating character in last row but no element afterwards, do nothing
	} else {
        EX_THROW("Error parsing last line of matrix");
	}

	//
	
	//now the dimensions are clear!
    // cout << "Matrix has " << mat_rows << " rows " << mat_cols << " columns and " << words << " elements" << endl;

	//resize matrix appropriately
	Clear();
	Create(mat_cols,mat_rows);

	//use the same algorithm as before to parse the elements
	last_was_whitechar = true;						//so if the first char is a non-whitechar it's recognized as start of a word
	s[strlen(str)] = whitespaces[0];//VERY DIRTY HACK ;) ! we're mis-using the terminating NULL-Byte and fill it with a whitespace so algorithm is more simple and works for last element, too
	
	
	int index = 0;
	for (int i = 0; i < (int) strlen(str); i++) {			//iterate over words
		int len = 0;
		while ( (strchr(whitespaces, s[i]) == NULL) && (strchr(rowsep, s[i]) == NULL)) {  //if no whitespace and no separator --> part of word
			++len;	//
			++i;	//iterate over whole word
		}
		if (len > 0) { //if there was a word found
			int row = index / cols;
			int col = index - (row*cols);
			LoadElementFromString(col,row, &s[i-len], len);
			++index;
		}
	}
	delete [] s;
	return *this;
}


/**
  @brief    Transpose matrix in-place

  For square matrices this algorithm will be truely done in-place, for non-square matrices (*this) = this->T() will be invoked
  **/
template<typename type> void Matrix<type>::Transpose()   //in-place transpose
{
    if (cols == 1) {      //column vector, just reshape it
        cols = rows;
        rows = 1;
        return;
    }

    if (rows == 1) {    //row vector, just reshape it
        rows = cols;
        cols = 1;
        return;
    }

    if (rows == cols) {

        //matrix is
        type temp;
        for(int y = 0; y < rows; y++) {
            for(int x = y; x < cols; x++) {

                int s1 = y * cols + x;
                int s2 = x * rows + y;

                temp = pData[s1]; // was [y * cols + x] before
                pData[s1] = pData[s2];// pData[s2];
                pData[s2] = temp;
            }
        }
    } else {
        *this = T();
    }

}


/**
	@brief	Transpose matrix into a new matrix.
	
	Enable the expresseion	C = A.T() (math: C = A^T) ;
**/
template<typename type> Matrix<type> Matrix<type>::T()
{
	if ((pData == NULL)  ||  ((rows*cols) == 0)) {
#if _DEBUG_MATRIX >= 1
        EX_THROW(" called for empty matrix.");
#endif
		return *this;
	}
	Matrix<type> result( rows, cols );
	for(int y = 0; y < rows; y++) {
		for(int x = 0; x < cols; x++) {
			result.pData[x * rows + y] = pData[y * cols + x];
		}
	}
	return result;
}

/**
	@brief	Transpose matrix into a new matrix.
	
	Enable the expresseion	C = A.T() (math: C = A^T) ;
**/
template<typename type> void Matrix<type>::Reshape(int colsNew, int rowsNew)
{
	if ((pData == NULL)  ||  ((rows*cols) != (colsNew * rowsNew))) {
        EX_THROW("Reshaping invalid");
	}
	cols = colsNew;
	rows = rowsNew;
}


/**
	@brief	Returns true if type==Complex
	@bug	somehow the compiler accepts, but the linker complains

	let the vtable (RunTimeTypeIdentification) decide
**/
/*
template<> bool Matrix<Complex>::IsComplex()
{
	return true;
}
*/

/**
    @brief	Returns false if type!=Complex
	
	let the vtable (RunTimeTypeIdentification) decide
**/
template<typename type> bool Matrix<type>::IsComplex()
{
    EX_THROW("Not yet implemented");
	return false;
}


/**
	@brief	swap two cols in 2-dimensional matrix
	@param	a	col number of first col
	@param	b	col number of other col

	Range checking is applied, changing with itself is not checked for as execution does not lead to error (only time consumption)
**/
template<typename type> void Matrix<type>::SwapCols(int a, int b)
{
	if ((pData == NULL)  ||  (a >= cols) || (a < 0) || (b >= cols) || (b < 0) ) {
        EX_THROW("Error swapping cols: col numbers out of range");
	}
	type s;
	for(int y = 0; y < rows; y++) {	//swap elements along col(s)
		s				= pData[y*cols+a];
		pData[y*cols+a] = pData[y*cols+b];
		pData[y*cols+b] = s;
	}
}

/**
	@brief	swap two rows in 2-dimensional matrix
	@param	a	row number of first row
	@param	b	row number of other row

	Range checking is applied, changing with itself is not checked for as execution does not lead to error (only time consumption)
**/
template<typename type> void Matrix<type>::SwapRows(int a, int b)
{
	if ((pData == NULL)  ||  (a >= rows) || (a < 0) || (b >= rows) || (b < 0) ) {
        EX_THROW("Error swapping rows: row numbers out of range");
	}
	type s;
	for(int x = 0; x < cols; x++) {	//swap elements along row(s)
		s			    = pData[a*cols+x];
		pData[a*cols+x] = pData[b*cols+x];
		pData[b*cols+x] = s;
	}
}

/**
  @brief    mirror matrix
  @param    hor_notvert if true: mirror horizontally = swap rows; if false: mirror vertically = swap cols

  In-Place operation, not fully optimized for speed
  **/
template<typename type> void Matrix<type>::Mirror(bool hor_notvert)
{
    if (hor_notvert) {
        for(int y = 0; y < (rows/2); y++) { //oddsize matrices will not swap center line with itself; that's ok
            SwapRows(y, rows-y);
        }
    } else {
        for(int x = 0; x < (cols/2); x++) {
            SwapCols(x, cols-x);
        }
    }
}

/**
  @brief    rotate matrix
  @param    quarters    perform quarters*90° rotation clockwise
  **/
template<typename type> void Matrix<type>::Rotate(int quarters)
{
    if (quarters < 0) {                         //negative direction: turn as many FULL turns until we have a positive number
        int wholeturns = (-quarters / 4) + 1;
        quarters += wholeturns * 4;
    }
    quarters = quarters % 4;

    if (quarters == 0) {                        //full rotations: invariant
        return;
    }

    Matrix temp(*this);                         //copy this to temporary

    //left are 1, 2, 3
    switch(quarters) {
    case 1:
        rows = temp.cols;       //no need to reallocate, just swap dimensions, overall number of elements is kept
        cols = temp.rows;

        for(int y = 0; y < rows; y++) {
            for(int x = 0; x < cols; x++) {
                pData[y * cols + x] = temp.pDate[x * rows + (cols-y)]; //rows=temp.cols cols = temp.rows
            }
        }
        break;
    case 2:
        type swap;
        for(int y = 0; y < rows/2; y++) {
            for(int x = 0; x < cols; x++) {
                swap = pData[y*cols + x];
                pData[y*cols + x] = pData[(rows-y)*cols + (cols-x)];
                pData[(rows-y)*cols + (cols-x)] = swap;
            }
        }
        break;
    case 3:
        rows = temp.cols;       //no need to reallocate, just swap dimensions, overall number of elements is kept
        cols = temp.rows;

        for(int y = 0; y < rows; y++) {
            for(int x = 0; x < cols; x++) {
                pData[y * cols + x] = temp.pDate[(rows - x) * rows + y]; //rows=temp.cols cols = temp.rows
            }
        }
        break;
    default:
        EX_THROW("Invalid Rotation");
    }
}

/**
	@brief	calcualate inverse matrix
	@throw	throws exception if matrix is singular (non-invertible)
**/
template<typename type> Matrix<type> Matrix<type>::SubMatrix(int left, int upper, int width, int height)
{
	if ( ((left + width) > cols) || ((upper + height) > rows) ) {
        EX_THROW("Out of bounds");
		return Matrix<type>();
	}
	Matrix<type> ret(width, height);
	for(int y = 0; y < height; y++) {
		memcpy( &ret.pData[y*width], &pData[y*cols + left], width * sizeof(type) );		//it's faster to memcopy part of line than element-wise 
	}
	return ret;
}

/**
  @brief    place content of some matrix into this one
  @param    left    left offset for insertion (using position 0,0 of data matrix)
  @param    upper   top offset for insertion (using position 0,0 of data matrix)
  @param    height  number of rows to fill
  @param    width   nomber of cols to fill
  @return   0 on success

  Automatic index checking is performed, data matrix is accesses from upper-left and layed over current replacing appropriate positions
  **/
template<typename type> int Matrix<type>::setSubMatrix(int left, int top, int width, int height, const Matrix<type> &data)
{
    int r = 0;
    int c = 0;
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            c = x + left;
            r = y + top;

            if ( (x >= data.cols) || (y >= data.rows) )             //range checking for data
                continue;

            if((c < 0) || (c >= cols) || (r < 0) || (r >= rows))    //range checking for *this
                continue;

            pData[r * cols + c] = data.pData[y * data.cols + x];
        }
    }
    return 0;
}

/**
  @brief    get upper triangular matrix (set rest to zero)
  **/
template<typename type> Matrix<type> Matrix<type>::upperTriangular()
{
    Matrix<type> tri(this->cols, this->rows);

    type *src = pData;
    type *dst = tri.pData;
    for(int x = 0; x < cols; x++) {
        for(int y = 0; y < rows; y++) {
            if (x >= y) {
                *dst = *src;
            } else {
                *dst = 0;
            }
            ++src;
            ++dst;
        }
    }

    return tri;
}

/**
	@brief	get diagonal of matrix 
	@return	1xN - column-vector containing main diagonal elements

	This function also works for non-square rectangular matrices
**/
template<typename type> Matrix<type> Matrix<type>::Diag()
{
	int sz = ( (cols < rows) ? cols : rows);
	
	Matrix<type> res(1,sz);
	for(int i = 0; i < sz; i++) {
		res.pData[i] = pData[i * cols + i];
	}
	return res;		
}



/**
	@brief	calcualate adjugate matrix
    @note   function is very slow
**/
template<typename type> Matrix<type> Matrix<type>::Adjugate()
{
	if ( (pData == NULL) ) {
        EX_THROW("Adjugate of NULL-Matrix does not exist!");
	}
	Matrix<type> result(cols,rows);
	type	value;

	for(int y = 0; y < result.rows; y++) {
		for(int x = 0; x < result.cols; x++) {
			value = this->Minor(x,y).Det();
			result.pData[x*cols + y] = ( (x+y) % 2 ) ? -value : value;	//transpose and apply checkerboard-pattern of sign
		}
	}
	return result;
}

/**
    @brief	generate Minor-Matrix when eleminating one row and one column
    @param	strikeCol	number of column to eleminate
    @param	strikeRow	number of row to eleminate
	@return	returns a (rows-1)x(cols-1) - Matrix
**/
template<typename type> Matrix<type> Matrix<type>::Minor(int strikeCol, int strikeRow) const
{
	if ( (pData == NULL) || (cols < 1) || (rows < 1) || (strikeCol >= cols) || (strikeRow >= rows) ) {
        EX_THROW( "Matrix Minor algorithm failed integrity check");
	}
	Matrix<type> result(cols-1,rows-1);
	int sX, sY;
    for(int y = 0; y < result.rows; y++) {
		for(int x = 0; x < result.cols; x++) {
			sX = x;
			if (x >= strikeCol) ++sX;
			sY = y;
			if (y >= strikeRow) ++sY;
			result.pData[y*result.cols + x] = pData[sY * cols + sX];
		}
	}
	return result;
}

/**
    @brief	remove rows from matrix resulting in a smaller matrix with same number of columns
    @param	start       index of first row to eleminate
    @param	count       count of rows to eleminate (defaults to 1),
    @return	returns a (rows-count)x(cols) - Matrix

    if count is too big or negative "all right part" of matrix will be removed
    if start > rows nothing will be removed
**/
template<typename type> Matrix<type> Matrix<type>::RemoveRows(int start, int count /* = 1 */) const
{
    if ( (pData == NULL) || (cols < 1) || (rows < 1) ) {
        EX_THROW( "Matrix Minor algorithm failed integrity check");
    }
    if (count < 0)
        count = rows;   // negative: remove as much as possible

    if (start >= rows) {        //trying to remove non-existant rows
        CONSOLE(3,"start >= rows, not removing anything");
        return *this;
    }
    if ((start + count) > rows) {
        CONSOLE(3,"start + count >= rows, removing less ");
        count = rows - start;
    }

    Matrix<type> result(cols,rows-count);

    int srcY;
    for(int y = 0; y < result.rows; y++) {
        for(int x = 0; x < result.cols; x++) {
            srcY = y;
            if (y >= start)
                srcY += count;
            result.pData[y*result.cols + x] = pData[srcY*cols + x];
        }
    }
    return result;
}

/**
    @brief	remove cols from matrix resulting in a smaller matrix with same number of columns
    @param	start       index of first column to eleminate
    @param	count       count of columns to eleminate (defaults to 1),
    @return	returns a (rows)x(cols-count) - Matrix

    if count is too big or negative "all right part" of matrix will be removed
    if start > rows nothing will be removed
**/
template<typename type> Matrix<type> Matrix<type>::RemoveCols(int start, int count /* = 1 */) const
{
    if ( (pData == NULL) || (cols < 1) || (rows < 1) ) {
        EX_THROW( "Matrix Minor algorithm failed integrity check");
    }
    if (count < 0)
        count = cols;   // negative: remove as much as possible

    if (start >= cols) {        //trying to remove non-existant rows
        CONSOLE(3,"start >= cols, not removing anything");
        return *this;
    }
    if ((start + count) > cols) {
        CONSOLE(3,"start + count >= cols, removing less ");
        count = cols - start;
    }

    Matrix<type> result(cols-count,rows);

    int srcX;
    for(int y = 0; y < result.rows; y++) {
        for(int x = 0; x < result.cols; x++) {
            srcX = x;
            if (x >= start)
                srcX += count;
            result.pData[y*result.cols + x] = pData[y*cols + srcX];
        }
    }
    return result;
}

/**
  @brief    calculate trace of the matrix
  @return   trace

  The trace is the product of all main diagonal elements
  **/
template<typename type> type Matrix<type>::Trace() const
{
    type trace = pData[0];
    for(int i = 1; (i < cols) && (i < rows); i++) {
        trace *= pData[i * cols + i];
    }
    return trace;
}



/**
  @brief    calculate the norm of a matrix
  @param    kind    kind of norm to calculate (0: L\infty-norm, 1: L1-norm, 2: L2-norm, other: error)
  @return   norm value

  @bug  calculating nonsense
  **/
template<typename type> type Matrix<type>::Norm(int kind)
{
    if (NULL == pData) {
        EX_THROW("Matrix empty");
        return 0;
    }

    EX_THROW("Matrix Norm is formally invalid!!!! We are calculating total nonsense");

    type norm = 0;
    switch (kind) {
    case 0: {   //the \infty-norm, (max(abs(*this))
            norm = abs(pData[0]);
            for(int i = 1; i < rows*cols; i++) {
                type a = abs(pData[i]);
                if (a > norm) {
                    norm = a;
                }
            }
            break;
        }
    case 1: { //The 1-norm, or largest column sum of A, max(sum(abs(A)))
            norm = ColSumAbs(0);
            for(int c = 1; c < cols; c++) {
                type sum = ColSumAbs(c);
                if (sum > norm)
                    norm = sum;
            }
            break;
        }
    case 2: { //the L2-norm
            for(int i = 0; i < rows*cols; i++) {
                norm += pData[i]*pData[i];
            }
            norm = sqrt(norm);
            break;
        }
    default:
        EX_THROW("Desired matrix norm not known")
        break;
    }
    return norm;
}


/**
    @brief	Perform LU factorization
    @param  U   pointer to object that will recieve U-Matrix, trace(U) = det
    @param  L   pointer to object that will recieve L-Matrix, defaults to null
    @param  P   pointer to object what will recieve the P-Matrix which is a matrix of 0 and 1 elements keeping track of pivoting, if NULL there will be no pivot-tracking
    @return signum of the determinant (+-1) if valid results, zero otherwise
    @bug	thres is implemented hard-coded
	
	This function can be used as a start to get determinant and other information
**/
template<typename type> int Matrix<type>:: UL(Matrix<type>* U, Matrix<type> *L /* = NULL*/, Matrix<type> *P /* = NULL */)
{
   if ( (pData == NULL) || (cols != rows) ) {
		throw "Matrix LU-factorization is only possible for square matrices";
	}

    if (L) {
        L->Create(cols, cols);              //the L-Matrix
        L->Fill(MATRIX_PATTERN_EYE);
    }
    if (P) {
        P->Create(cols, cols);              //the P-matrix, keeps track of pivoting
        P->Fill(MATRIX_PATTERN_EYE);
    }
    (*U) = (*this);                     //the U-Matrix

    int sgn_det = 1;	//signum of determinante

	for(int y = 0; y < (rows-1); y++) {			//for all rows of matrix, but the last
		/** PIVOTING **/
		// check if there i a (close-to) zero at the current diagonal element
		//cout << "Element (" << y << "," << y << ") = " << U.pData[y*cols+y] << endl;
  //      type absolute = abs(U->pData[y*cols+y]);
 //		if ( absolute < thres ) { //we have a zero (or close), find the row to swap with: this is the row where the (remaining) column has maximum value
								//this is essential for numerical stability
			//cout << "Pivoting... " << endl;
			int pivot_row = y;			//
            type pivot_value = abs(U->pData[y*cols+y]);
			for(int ry = y; ry < rows; ry++) {	//for the rest of the column look for maximum element
                if ( abs(U->pData[ry*cols+y]) > pivot_value ) {
                    pivot_value = abs(U->pData[ry*cols+y]);
					pivot_row = ry;
				}
			}
			if (pivot_row == y) {			//no "better" pivot element found
            //	cout << "Warning " << __FUNCTION__ << ": no good pivot element found, algorithm may be unstable due to (close to) singular matrix" << endl;
                ; // do nothing, not swapping required

			} else {						//pivot element found, doing permutation
                U->SwapRows(y, pivot_row);
				type t;
                if (L) {
                    for(int i = 0; i < y; i++) { //L is to swap only elements below the diag
                        t = L->pData[y*cols+i];
                        L->pData[y*cols+i] = L->pData[pivot_row*cols+i];
                        L->pData[pivot_row*cols+i] = t;
                    }
                }
                if (P) {
                    P->SwapCols(y, pivot_row);	//cols is correct here! it's the permutation matrix
                }
                sgn_det = -sgn_det;
			}
			//cout << "Pivot swapping rows " << y << " <-> " << pivot_row;
//		}
		/** END PIVOTING **/

		/** ELIMINATING **/
		for(int r = y+1; r < rows; r++) {
			//cout << "Eleminating..." << endl;
            if (U->pData[y * cols + y] == 0.0) {
                EX_THROW("Cannot perform LU decomposition: Matrix is singular");
                return 0;
            }
            if (MAT_IS_ZERO(U->pData[y * cols + y])) {  //close to zero
                // cout << "Warning Matrix is close to singular. Result may be inappropriate." << endl;
            }

            type factor = U->pData[r*cols + y] / U->pData[y * cols + y];
            for(int k = 0; k < cols; k++) {
                U->pData[r*cols + k] -= factor * U->pData[y * cols + k];
			}
            if (L) {
                L->pData[r*cols + y] = factor;			//fill lower triangular matrix COLUMN with factor
            }
		}
		
		/** END ELIMINATING **/
		
		
	}
/*    cout << "LU factorization:\nU = " << endl;
    U->Print();
    if (L) {
        cout << endl << "L = " << endl;
        L->Print();
    }
	cout << endl << "P = " << endl;
    if (P) {
        P->Print();
        cout << endl << "P * L * U = " << endl;
        Matrix<type> K ( (*P) * (*L) * (*U) );
        K.Print();

        cout << endl;
    } else {
        cout << "N/A" << endl;
    }
*/
    return sgn_det;
}

template<typename type> void Matrix<type>::Fill(int pattern)
{
	if (MATRIX_PATTERN_ZEROS == pattern) {
		int size = rows * cols;
		for (int i = 0; i < size; i++) {
			pData[i] = 0;
		}
	} else if (MATRIX_PATTERN_EYE == pattern) {
		for(int y = 0; y < rows; y++) {
			for(int x = 0; x < cols; x++) {
				if (x == y)
					pData[y * cols + x] = 1;
				else
					pData[y * cols + x] = 0;
			}
		}
	} else if (MATRIX_PATTERN_UPCOUNTER == pattern) {
		int size = rows * cols;
		for (int i=0; i < size; i++) {
            pData[i] = i;
		}
	} else if (MATRIX_PATTERN_ONES == pattern) {
		int size = rows * cols;
		for (int i=0; i < size; i++) {
			pData[i] = 1;
		}
	}else if (MATRIX_PATTERN_HILBERT == pattern) {
		for(int y = 0; y < rows; y++) {
			for(int x = 0; x < cols; x++) {
				pData[y * cols + x] = 1.0f / (x + y + 1 );	//we are numbering from 0 so it's +1 not -1 as in wikipedia
			}
		}
	}else if (MATRIX_PATTERN_M1 == pattern) {
		for(int y = 0; y < rows; y++) {
			for(int x = 0; x < cols; x++) {
				pData[y * cols + x] = (x * (y*y+1) - 2) / (x+1);
			}
		}
	} else {
        EX_THROW("Fill mode not implmented");
	}
}

/**
  @brief    round close-to-zero-elements to zero, IN-PLACE
  @param    epsilon threshold value
  @return   number of rounded elements (including those who were actually zero)
  **/
template<typename type> Matrix<type>&  Matrix<type>::RoundZero(double epsilon, int *count /* = NULL*/)
{
    if (count)
        *count = 0;
    for(int i = 0; i < (rows*cols); i++) {
        if ( (pData[i] > epsilon) || (pData[i] < (-epsilon)) )
            continue;
        pData[i] = 0;
        if (count)
            ++(*count);
    }
    return *this;
}

#endif //MATRIX_HPP

