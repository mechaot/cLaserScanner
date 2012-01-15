/**
	@file		matrix.hpp
    @author	Andreas Poesch (itdevel@mechaos.de)
	@since	2007
	@warning 	ALPHA VERSION, not all functions may be numerically stable or working properly
	
	Simple Math Library (SiMaLi)
		
    Matrix template class definition
	2D only version
	tested with type "double" only
	
    rows*cols may not exceed 2^31-1


    $Id: matrix.h 76 2011-11-16 16:47:33Z mechaot $

**/


// Matrix class header file
// unsing templates the declaration and implementation of the template class
// MUST be in the same file (no separation header-file <-> implementation-file)
// and the file is to be #included into other files...
// all data is stored in one block and coordinates are virtual by index calculation
// no images bigger than 2G!

#pragma once

#ifndef MATRIX_H
#define MATRIX_H

#include <opencv.hpp>   //opencv 2.2 include-syntax


#include "clapack.h"


#include "QtException.h"

class QString;
class QChar;

// forward declaration, needed for friend in & out ops
template<typename type> class Matrix;

// typedef Matrix<double> DMatrix;
// typedef Matrix<float>  FMatrix;

#define SIMALI_RAYTRACER_FUNCTIONS          1      ///< define this to non-zero value to enable raytracer binding
#define SIMALI_USE_BV_FUNCTIONS             1      ///< define this to non-zero value to enable image processing functions
#define SIMALI_USE_LAPACK                   1      ///< define this to non-zero value to enable the lapack enhanced functions, remember to update .pro-file accordingly if neccessary

#if SIMALI_RAYTRACER_FUNCTIONS
    #include "RTvect3D.h"
    #include "RTvect3Dh.h"
    #include "RTcolor.h"
#endif //SIMALI_RAYTRACER_FUNCTIONS


#define _DEBUG_MATRIX    1         ///<0: no debugging, 1: some debugging, 2: exhaustive debugging

#ifndef RTCONST_H   // defines PI also
    const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;
#endif // RTCONST_H
static const double MAT_ZERO = 1e-12;                     //values below that can be considered zero if necessary

/*** Patterns for fill-function ***/
#define MATRIX_PATTERN_ZEROS		0			//fill matrix with zeros
#define MATRIX_PATTERN_EYE			1			//create identity matrix with ones on diag
#define MATRIX_PATTERN_UPCOUNTER	2			// pData[i] = i+1;
#define MATRIX_PATTERN_ONES			3			//create  matrix containing ones
#define MATRIX_PATTERN_HILBERT		4			//create a close-to-singular hilbert matrix
#define MATRIX_PATTERN_M1			5			//create a non-singular matrix containing a defined pattern

#ifndef QT_VERSION
    #error not using QT
#endif

//friends to enable streaming data in and out, not implemented yet
// template<typename type> istream& operator>>(istream& is, Matrix<type>& b);
// template<typename type> ostream& operator<<(ostream& os, const Matrix<type>& b);

template<typename type> Matrix<type> operator+(type t, const Matrix<type>& M);
template<typename type> Matrix<type> operator*(type t, const Matrix<type>& M);

enum MatrixLocation {
    HOST,                   //valid matrix data is located on host
    GPU,                    //valid matrix data is located on GPU
    BOTH                    //valid matrix data is located on GPU and CPU (useful for non-destructive use with CPU and GPU algorithms)
};


/**
  @brief    helper function to determine if some element is close to zero
  @param    d   element to check for
  @return   true if abs(d) < MAT_ZERO, false otherwise
  **/
template<typename type> static inline bool MAT_IS_ZERO(const type& d)
{
    if (d >= 0.0) {
        return d <= MAT_ZERO;
    } else {
        return d > (-MAT_ZERO);
    }
}

/**
  @brief    helper function to set any element to zero that has an absolute value below MAT_ZERO
  @param    d   pointer to element to manipulate
  **/
template<typename type> static inline void MAT_ROUND_ZERO(type *d)
{
    if (M_IS_ZERO(*d))
        *d = 0.0;
}

/**
 * @brief Matrix class for handling of (image) matrices
**/
template<typename type> class Matrix{

protected:
#if _DEBUG_MATRIX >= 2
    static	int instances = 0;
#endif

  public:	//everything public to enable fast access

    
    int rows;                       ///< rows in matrix (0: NULL-matrix; 1-col-vector)
    int cols;                       ///< cols in matrix (0: NULL-matrix; 1-row-vector)
    //int layers;                     ///< matrix layers (0: NULL-matrix; 1: 2D-matrix (image?) > 3: possibly color image
    type* pData;                    ///< pointer to data (HOST)
    type* pGPU;                     ///< pointer to data (GPU)
    QString strName;                ///< some string to name the matrix, just a "schmankerl"
    MatrixLocation mLocation;       ///< current matrix location

    #ifdef SIMALI_USE_CUDA
        #if SIMALI_USE_CUDA
            #error  CUDA currently not supported / implemented
        //what to do with operators for host + non-host mixed arguments? (upload both or download both / copy-upload non-host and then destroy gpu-version?)
        enum location{host_single_thread,host_multithread,gpu};        //where is the acutal matrix data located? desired type of operation


        MatrixLocation CurrentLocation();

        bool transferToGPU();           //upload matrix to do calculations on GPU, all subsequent calls will invoke CUDA functions; CPU data destroyed
        bool copyToGPU();               //copy to GPU leaving the CPU data intact
        bool transferToHost();          //download matrix to host, all subsequent calls will invoke CPU functions; deallocate GPU memory
        bool copyToHost();              //copy from GPU to host keeping GPU memory

        //IntersectionType intersect(RTline3D &line, RTvect3D *pos, RTvect3D *normal);  //calculate ray intersection for RTpolygon on GPU
        #endif
    #endif  //SIMALI_USE_CUDA

  public:
    int Create(const int cols, const int rows);          // Allocation function (used in constructors)
                                    // 
    /*** basic functions ***/
    Matrix();              			 // Constructor for empty matrix
    ~Matrix();             			 // Destructor for matrix 
    Matrix(const char* str);		 // Constructor by ascii string
    Matrix(const QString& str);		 // Constructor by ascii string
    Matrix(const Matrix<type>& Mat);    // Copy-constructor (deep copy)
    Matrix(const cv::Mat &img, bool toGrayscale = true);      // open-cv
   // cv::Mat toCvMat(int cvmattype);      // convert to cv::Mat with specified type
    Matrix(int c,int r);       			 // Constructor for two dimensional matrix (=2D image)
    Matrix<type> Copy() const;                //Copy to new array
    Matrix<type> ShallowCopy() const;                //Copy to new array with shared data pointer
    bool isEmpty() const;                     //is matrix filled with data?


    static Matrix<type> eye(int c, int r = -1);                      // create an eye-matrix of size r x c
    static Matrix<type> zero(int c, int r = -1);                     // create a zero-matrix of size r x c
    static Matrix<type> rotX(double angle);                          // create a 4x4 homogeneous rotation matrix for rotation around X-axis by some angle (in rad)
    static Matrix<type> rotY(double angle);                          // create a 4x4 homogeneous rotation matrix for rotation around Y-axis by some angle (in rad)
    static Matrix<type> rotZ(double angle);                          // create a 4x4 homogeneous rotation matrix for rotation around Z-axis by some angle (in rad)
    static Matrix<type> rotation(double yaw, double pitch, double roll); //create 4x4 homogeneous rotation matrix with yaw pitch and roll set to parameters (in rad)
    static Matrix<type> rodrigues(double x, double y, double z); //create 4x4 homogeneous rotation matrix with yaw pitch and roll set to parameters (in rad)
    static Matrix<type> trans(double x, double y, double z);         // create a 4x4 homogeneous translation matrix for rotation around Z-axis by some angle (in rad)
    static Matrix<type> transIdentity();            //
    static Matrix<type> scale(double sX, double sY, double sZ);

    int Size() const;                     // report size as number of elements rows*cols
    type *Row(int n);               // ret a pointer to the n-th data row

  /* QT supported human interaction functions */
    bool SaveRaw(const QString &fileName) const;
    Matrix<type>& LoadRaw(const QString &fileName);
    Matrix<type>& LoadFromGMDFile(const QString &fileName);
    Matrix<type>& LoadMultisource(const QString& dataorsource);
    Matrix<type>& LoadFromStringList(const QStringList& list);
    Matrix<type>& LoadFromString(const QString& str);
    Matrix<type>& LoadFromAsciiFile(const QString &fileName);
    Matrix<type>& LoadFromSdfFile(const QString &fileName);
    Matrix<type>& LoadFromRawTriangleVerticesFile(const QString &fileName);
    Matrix<type>& LoadFromBinaryTriangles(const QString &fileName);
    QString ToString(const QString lineSep = "\n") const;

  /* naming functions */
    void setName(const QString& name);
    QString name();
    bool hasName(const QString& name);

  /* old file functions */
    Matrix<type>& fromFile(const char* fileName);		//
    Matrix<type>& Load(const char* str);				//   M.load(" 1 3 4; 4 -3.5 2; -4 0 1 ");	colseparator: 1+ whitespaces; rowseparator whitespaces and ; xor \n
    inline type* LoadElementFromString(int col, int row, char* str, int len);

    const inline bool	DimMatch(const Matrix<type> &other) const;		//are 2 matrices dimensionally equal (i.e. can they be added, substracted, etc...); i.e. same number of dimensions
    //not existant in reality, as execution is left to right	const inline bool	DimMultLeft(const Matrix<type> &other) const;		//can a matrix be multiplied (e.g. vector \times matrix) right-sided (new = other * his )
	
    void Clear();                    // Clear up all pointers and set sizes to zero (called by destructor)
    void SetZero();					//Zero all elements NOT discarding/touching pointers
    void Fill(int pattern);			//fill with specific pattern

    Matrix<type>&  RoundZero(double epsilon = 1e-14, int *count = NULL); //set elements with abs(elements) < epsilon to 0.0; returns number of zero elements
	
    void Set(int sx, type val); 		// Set  value if vector
    void Set(int sx, int sy, type val); // Set  value
   
    const type Get(int sx) const;        			// Get pixel value from vector
    const type Get(int sx, int sy) const;        // Get pixel value
    
    type Min(int *idx = NULL) const;       // Get value of min( Matrix ) if idx != NULL (which is the default value) the idx of first occurence of min is returned here
    type Max(int *idx = NULL) const;       // Get value of max( Matrix ) if idx != NULL (which is the default value) the idx of first occurence of min is returned here
    type Sum(bool *overflow = NULL) const; // sum up all components of matrix

    Matrix<type> Diag();

    Matrix<type> RowMinima();     // vertical vector or row minima
    Matrix<type> RowMaxima();     // vertical vector of row maxima
    Matrix<type> RowSums();     // vertical vector of row sums
    type         RowSum(int row); //sum of all elements of a specific row
    type         RowSumAbs(int row); //sum of all elements of a specific row taking absolute values

    Matrix<type> ColMinima();     // horizontal vector of column minima
    Matrix<type> ColMaxima();     // horizontal vector of column maxima
    Matrix<type> ColSums();     // horizontal vector of column sums
    type         ColSum(int col); //sum of all elements of a specific column
    type         ColSumAbs(int col); //sum of all elements of a specific column taking absolute values

    Matrix<type> SubMatrix(int left, int upper, int width, int height);
    int setSubMatrix(int left, int top, int width, int height, const Matrix<type> &data);

    Matrix<type> upperTriangular();                         //get upper triangular matrix (set rest to zero)

    void SwapRows(int a, int b);	//swap two matrix rows
    void SwapCols(int a, int b);	//swap to columns
    void Mirror(bool hor_notvert);  //mirror matrix
    void Rotate(int quarters);      //rotate matrix by (quarters * 90°) in clockwise direction (negative values allowed)
    void Transpose();   //in-place transpose
    Matrix<type> T();	//transpose into new matrix
    void Reshape(int colsNew, int rowsNew);

// these functions MAY depend on kind of type (real=double or complex) so in further implementations this might be 
// repositioned to an other class "" derived from Matrix that explicitely deals with the intrinsic types (int, double) and 
// one "complex"-class
	// Conj();			//complex conjugate
	// IsReal();		//returns true if it is either of a non-complex type OR of complex type but with imag === 0
	bool IsComplex();		//returns true if it is of type Complex, not fully implemented
	// MakeComplex();	//return a matrix of type Complex
	// MakeReal(int method = CONVERT_METHOD_ABS);	//make real from Complex with different methods (absolute value, real value, imag value)

	// Matrix<type> SubMatrix(int offsetX, int offsetY, int width, int height, int border_fill_pattern = ZEROS/MIRROR/MIRROR_HALF/PERIODIC/ERROR)
	// svd
	// Matrix<type> & fromDiag(Matrix<type> diag);		
	// int rank()
	// int round(int decimals);	//round all values to accuracy
	// int roundZeros(int decimals);	//set values close to zero to exactly zero, do not round others
	// sgn() -> signum positive definite = +1, negative definite = -1, semidefinite: +- 0.5, indefinite: 0
	// Matrix<type> eig();	//eigenvalues as col-vector
	// Matrix<type> Rvectors(); //right hand eigenvectors
	// Matrix<type> Lvectors(); //left hand eigenvectors
	// double leastSquareScaleFactor();	//scale factor between 2 vectors, faster than leastSquareFit
	// Matrix<type> solve(b);		//linear solve A * x = b; returns x or empty matrix if non-solveable
	// Matrix<type> leastSquareFit(b);	//best solve for  b = A * x for overdetermined systems

//    Matrix<type> solve(Matrix<type> &b);


    Matrix<type> Minor(int strikeCol, int strikeRow) const;         //get Minor-Matrix of dimension (N-1 x M-1), i.e. the Matrix that upcomes when striking one col and one row (to get adjuct for ex.)
    Matrix<type> RemoveRows(int start, int count = 1) const;
    Matrix<type> RemoveCols(int start, int count = 1) const;
    Matrix<type> Adjugate();                                        //get adjugate matrix of dimension (N-1 x M-1)
    type Norm(int kind);

    int UL(Matrix<type>* U, Matrix<type> *L = NULL, Matrix<type> *P = NULL);	//perform LU algorithm, but with arguments swapped for NULL-allowance

	//setSingularityThreshold(type thres);	// this threshold defines when matrix is considered singular when applying LU-transform or Inversion/SVD


    /** LAPACK enhanced or otherwise "optimal" functions **/
    Matrix<type> LU();                                              //LU factorization
    int QR(Matrix<type> *Q, Matrix<type> *R);                       //QR factorization
    type Det();                                                     //get determinante
    Matrix<type> Inv() const;										//get inverse of Matrix
    Matrix<type> Pinv() const;                                      //get pseudoinverse of Matrix

    /** end enhanced functions **/
    type Trace() const;                                           //calculate trace of matrix which equals the product of main diagonal elements

    /*** operators 	***/
    // + += - -= = == * *= []  
    Matrix<type> operator+ (const Matrix<type> &other) const; //need to create new object
    Matrix<type> operator+ (const type val) const;			//need to create new object
    //enable the expression 2.0 + Matrix; // Matrix + 2.0 is enabled by means of a member of Matrix class
    friend Matrix<type> operator+  (type t, const Matrix<type>& M) { return operator+ (M, t); };
    Matrix<type>& operator+= (const Matrix<type> &other);	//
    Matrix<type>& operator+= (const type val);			//

    Matrix<type> operator- (const Matrix<type> &other) const; //need to create new object
    Matrix<type> operator- (const type val) const;			//need to create new object
    //Matrix<type> operator- <type> (type t, const Matrix<type>& M);

    Matrix<type>& operator-= (const Matrix<type> &other);	//
    Matrix<type>& operator-= (const type val);			//

    Matrix<type> operator* (const Matrix<type> &Mat) const;
    Matrix<type> operator* (const type val) const;			//
    friend Matrix<type> operator*  (type t, const Matrix<type>& M) { return operator* (M, t); };
    //enable 2 * matrix;
    Matrix<type>& operator*= (const Matrix<type> &other);			//
    Matrix<type>& operator*= (const type val);			//

    Matrix<type>& operator/= (const type val);

    Matrix<type>& operator= (const Matrix<type> &Mat);
    bool operator== (const Matrix<type> &Mat);

    inline type operator[](int idx) const;
    inline type& operator[](int idx);
    type operator() (int p) const;
    type& operator() (int p);
    type operator() (int c, int r) const;
    type& operator() (int c, int r);

    void Print();					// cout matrix


#if SIMALI_RAYTRACER_FUNCTIONS
    Matrix(const RTvect3D &v);
    Matrix(const RTvect3Dh &vh);
    RTvect3D toRTvect3D(bool *ok = NULL) const;
    RTvect3Dh toRTvect3Dh(bool *ok = NULL) const;
    Matrix<type>& operator= (const RTvect3D &v);
    Matrix<type>& operator= (const RTvect3Dh &v);

    //Matrix<type> operator*(const RTvect3D &v) const;
    RTvect3D operator*(const RTvect3D &v) const;
#endif //SIMALI_RAYTRACER_FUNCTIONS
  
#if SIMALI_USE_BV_FUNCTIONS
    cv::Mat toMat(int mattype = CV_64FC3);
    Matrix<type>& LoadFromImageFile(const QString &fileName, int flags  = -1 );
    int SaveToImageFile(const QString &fileName, bool normalize = true);
#endif //SIMALI_USE_BV_FUNCTIONS

};;


#include "matrix.hpp" 

#if SIMALI_USE_LAPACK
        #include "matrix_lapack.hpp"
#endif //SIMALI_USE_LAPACK

#include "matrix_statics.hpp"
#include "matrix_operators.hpp"
#include "matrix_qtascii.hpp"


#if SIMALI_RAYTRACER_FUNCTIONS
    template<> cv::Mat Matrix<RTcolor>::toMat(int mattype /* = cv::CV_64FC3*/);     //template specialization must be declared explicitely here!
    #include "matrix_raytracer.hpp"
#endif //SIMALI_RAYTRACER_FUNCTIONS

#if SIMALI_USE_BV_FUNCTIONS
    #include "matrix_bv.hpp"
#endif //SIMALI_USE_BV_FUNCTIONS


//we indeed DO INCLUDE *.hpp ON PURPOSE
//because templates CANNOT be PRECOMPILED into an .o-File!
//DO NOT attempt to BUILD matrix*.hpp
//matrix*.hpp cannot be compiled directly



#endif //MATRIX_H
