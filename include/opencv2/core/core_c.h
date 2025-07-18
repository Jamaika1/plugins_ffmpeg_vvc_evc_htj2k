/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#ifndef OPENCV_CORE_C_H
#define OPENCV_CORE_C_H

#include "opencv2/core/types_c.h"

#ifdef __cplusplus
/* disable MSVC warning C4190 / clang-cl -Wreturn-type-c-linkage:
       'function' has C-linkage specified, but returns UDT 'typename'
       which is incompatible with C

   It is OK to disable it because we only extend few plain structures with
   C++ constructors for simpler interoperability with C++ API of the library
*/
#  if defined(__clang__)
     // handle clang on Linux and clang-cl (i. e. clang on Windows) first
#    pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"
#  elif defined(_MSC_VER)
     // then handle MSVC
#    pragma warning(disable:4190)
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup core_c
    @{
*/

/****************************************************************************************\
*          Array allocation, deallocation, initialization and access to elements         *
\****************************************************************************************/

/** `malloc` wrapper.
   If there is no enough memory, the function
   (as well as other OpenCV functions that call cvAlloc)
   raises an error. */
CVAPI(void*)  cvAlloc( size_t size );

/** `free` wrapper.
   Here and further all the memory releasing functions
   (that all call cvFree) take double pointer in order to
   to clear pointer to the data after releasing it.
   Passing pointer to NULL pointer is Ok: nothing happens in this case
*/
CVAPI(void)   cvFree_( void* ptr );
#define cvFree(ptr) (cvFree_(*(ptr)), *(ptr)=0)

/** @brief Creates an image header but does not allocate the image data.

@param size Image width and height
@param depth Image depth (see cvCreateImage )
@param channels Number of channels (see cvCreateImage )
 */
CVAPI(IplImage*)  cvCreateImageHeader( CvSize size, int depth, int channels );

/** @brief Initializes an image header that was previously allocated.

The returned IplImage\* points to the initialized header.
@param image Image header to initialize
@param size Image width and height
@param depth Image depth (see cvCreateImage )
@param channels Number of channels (see cvCreateImage )
@param origin Top-left IPL_ORIGIN_TL or bottom-left IPL_ORIGIN_BL
@param align Alignment for image rows, typically 4 or 8 bytes
 */
CVAPI(IplImage*) cvInitImageHeader( IplImage* image, CvSize size, int depth,
                                   int channels, int origin CV_DEFAULT(0),
                                   int align CV_DEFAULT(4));

/** @brief Creates an image header and allocates the image data.

This function call is equivalent to the following code:
@code
    header = cvCreateImageHeader(size, depth, channels);
    cvCreateData(header);
@endcode
@param size Image width and height
@param depth Bit depth of image elements. See IplImage for valid depths.
@param channels Number of channels per pixel. See IplImage for details. This function only creates
images with interleaved channels.
 */
CVAPI(IplImage*)  cvCreateImage( CvSize size, int depth, int channels );

/** @brief Deallocates an image header.

This call is an analogue of :
@code
    if(image )
    {
        iplDeallocate(*image, IPL_IMAGE_HEADER | IPL_IMAGE_ROI);
        *image = 0;
    }
@endcode
but it does not use IPL functions by default (see the CV_TURN_ON_IPL_COMPATIBILITY macro).
@param image Double pointer to the image header
 */
CVAPI(void)  cvReleaseImageHeader( IplImage** image );

/** @brief Deallocates the image header and the image data.

This call is a shortened form of :
@code
    if(*image )
    {
        cvReleaseData(*image);
        cvReleaseImageHeader(image);
    }
@endcode
@param image Double pointer to the image header
*/
CVAPI(void)  cvReleaseImage( IplImage** image );

/** Creates a copy of IPL image (widthStep may differ) */
CVAPI(IplImage*) cvCloneImage( const IplImage* image );

/** @brief Sets the channel of interest in an IplImage.

If the ROI is set to NULL and the coi is *not* 0, the ROI is allocated. Most OpenCV functions do
*not* support the COI setting, so to process an individual image/matrix channel one may copy (via
cvCopy or cvSplit) the channel to a separate image/matrix, process it and then copy the result
back (via cvCopy or cvMerge) if needed.
@param image A pointer to the image header
@param coi The channel of interest. 0 - all channels are selected, 1 - first channel is selected,
etc. Note that the channel indices become 1-based.
 */
CVAPI(void)  cvSetImageCOI( IplImage* image, int coi );

/** @brief Returns the index of the channel of interest.

Returns the channel of interest of in an IplImage. Returned values correspond to the coi in
cvSetImageCOI.
@param image A pointer to the image header
 */
CVAPI(int)  cvGetImageCOI( const IplImage* image );

/** @brief Sets an image Region Of Interest (ROI) for a given rectangle.

If the original image ROI was NULL and the rect is not the whole image, the ROI structure is
allocated.

Most OpenCV functions support the use of ROI and treat the image rectangle as a separate image. For
example, all of the pixel coordinates are counted from the top-left (or bottom-left) corner of the
ROI, not the original image.
@param image A pointer to the image header
@param rect The ROI rectangle
 */
CVAPI(void)  cvSetImageROI( IplImage* image, CvRect rect );

/** @brief Resets the image ROI to include the entire image and releases the ROI structure.

This produces a similar result to the following, but in addition it releases the ROI structure. :
@code
    cvSetImageROI(image, cvRect(0, 0, image->width, image->height ));
    cvSetImageCOI(image, 0);
@endcode
@param image A pointer to the image header
 */
CVAPI(void)  cvResetImageROI( IplImage* image );

/** @brief Returns the image ROI.

If there is no ROI set, cvRect(0,0,image-\>width,image-\>height) is returned.
@param image A pointer to the image header
 */
CVAPI(CvRect) cvGetImageROI( const IplImage* image );

/** @brief Creates a matrix header but does not allocate the matrix data.

The function allocates a new matrix header and returns a pointer to it. The matrix data can then be
allocated using cvCreateData or set explicitly to user-allocated data via cvSetData.
@param rows Number of rows in the matrix
@param cols Number of columns in the matrix
@param type Type of the matrix elements, see cvCreateMat
 */
CVAPI(CvMat*)  cvCreateMatHeader( int rows, int cols, int type );

/** @brief Initializes a pre-allocated matrix header.

This function is often used to process raw data with OpenCV matrix functions. For example, the
following code computes the matrix product of two matrices, stored as ordinary arrays:
@code
    double a[] = { 1, 2, 3, 4,
                   5, 6, 7, 8,
                   9, 10, 11, 12 };

    double b[] = { 1, 5, 9,
                   2, 6, 10,
                   3, 7, 11,
                   4, 8, 12 };

    double c[9];
    CvMat Ma, Mb, Mc ;

    cvInitMatHeader(&Ma, 3, 4, CV_64FC1, a);
    cvInitMatHeader(&Mb, 4, 3, CV_64FC1, b);
    cvInitMatHeader(&Mc, 3, 3, CV_64FC1, c);

    cvMatMulAdd(&Ma, &Mb, 0, &Mc);
    // the c array now contains the product of a (3x4) and b (4x3)
@endcode
@param mat A pointer to the matrix header to be initialized
@param rows Number of rows in the matrix
@param cols Number of columns in the matrix
@param type Type of the matrix elements, see cvCreateMat .
@param data Optional: data pointer assigned to the matrix header
@param step Optional: full row width in bytes of the assigned data. By default, the minimal
possible step is used which assumes there are no gaps between subsequent rows of the matrix.
 */
CVAPI(CvMat*) cvInitMatHeader( CvMat* mat, int rows, int cols,
                              int type, void* data CV_DEFAULT(NULL),
                              int step CV_DEFAULT(CV_AUTOSTEP) );

/** @brief Creates a matrix header and allocates the matrix data.

The function call is equivalent to the following code:
@code
    CvMat* mat = cvCreateMatHeader(rows, cols, type);
    cvCreateData(mat);
@endcode
@param rows Number of rows in the matrix
@param cols Number of columns in the matrix
@param type The type of the matrix elements in the form
CV_\<bit depth\>\<S|U|F\>C\<number of channels\> , where S=signed, U=unsigned, F=float. For
example, CV _ 8UC1 means the elements are 8-bit unsigned and the there is 1 channel, and CV _
32SC2 means the elements are 32-bit signed and there are 2 channels.
 */
CVAPI(CvMat*)  cvCreateMat( int rows, int cols, int type );

/** @brief Deallocates a matrix.

The function decrements the matrix data reference counter and deallocates matrix header. If the data
reference counter is 0, it also deallocates the data. :
@code
    if(*mat )
        cvDecRefData(*mat);
    cvFree((void**)mat);
@endcode
@param mat Double pointer to the matrix
 */
CVAPI(void)  cvReleaseMat( CvMat** mat );

/** @brief Decrements an array data reference counter.

The function decrements the data reference counter in a CvMat or CvMatND if the reference counter

pointer is not NULL. If the counter reaches zero, the data is deallocated. In the current
implementation the reference counter is not NULL only if the data was allocated using the
cvCreateData function. The counter will be NULL in other cases such as: external data was assigned
to the header using cvSetData, header is part of a larger matrix or image, or the header was
converted from an image or n-dimensional matrix header.
@param arr Pointer to an array header
 */
CV_INLINE  void  cvDecRefData( CvArr* arr )
{
    if( CV_IS_MAT( arr ))
    {
        CvMat* mat = (CvMat*)arr;
        mat->data.ptr = NULL;
        if( mat->refcount != NULL && --*mat->refcount == 0 )
            cvFree( &mat->refcount );
        mat->refcount = NULL;
    }
    else if( CV_IS_MATND( arr ))
    {
        CvMatND* mat = (CvMatND*)arr;
        mat->data.ptr = NULL;
        if( mat->refcount != NULL && --*mat->refcount == 0 )
            cvFree( &mat->refcount );
        mat->refcount = NULL;
    }
}

/** @brief Increments array data reference counter.

The function increments CvMat or CvMatND data reference counter and returns the new counter value if
the reference counter pointer is not NULL, otherwise it returns zero.
@param arr Array header
 */
CV_INLINE  int  cvIncRefData( CvArr* arr )
{
    int refcount = 0;
    if( CV_IS_MAT( arr ))
    {
        CvMat* mat = (CvMat*)arr;
        if( mat->refcount != NULL )
            refcount = ++*mat->refcount;
    }
    else if( CV_IS_MATND( arr ))
    {
        CvMatND* mat = (CvMatND*)arr;
        if( mat->refcount != NULL )
            refcount = ++*mat->refcount;
    }
    return refcount;
}


/** Creates an exact copy of the input matrix (except, may be, step value) */
CVAPI(CvMat*) cvCloneMat( const CvMat* mat );


/** @brief Returns matrix header corresponding to the rectangular sub-array of input image or matrix.

The function returns header, corresponding to a specified rectangle of the input array. In other

words, it allows the user to treat a rectangular part of input array as a stand-alone array. ROI is
taken into account by the function so the sub-array of ROI is actually extracted.
@param arr Input array
@param submat Pointer to the resultant sub-array header
@param rect Zero-based coordinates of the rectangle of interest
 */
CVAPI(CvMat*) cvGetSubRect( const CvArr* arr, CvMat* submat, CvRect rect );
#define cvGetSubArr cvGetSubRect

/** @brief Returns array row or row span.

The function returns the header, corresponding to a specified row/row span of the input array.
cvGetRow(arr, submat, row) is a shortcut for cvGetRows(arr, submat, row, row+1).
@param arr Input array
@param submat Pointer to the resulting sub-array header
@param start_row Zero-based index of the starting row (inclusive) of the span
@param end_row Zero-based index of the ending row (exclusive) of the span
@param delta_row Index step in the row span. That is, the function extracts every delta_row -th
row from start_row and up to (but not including) end_row .
 */
CVAPI(CvMat*) cvGetRows( const CvArr* arr, CvMat* submat,
                        int start_row, int end_row,
                        int delta_row CV_DEFAULT(1));

/** @overload
@param arr Input array
@param submat Pointer to the resulting sub-array header
@param row Zero-based index of the selected row
*/
CV_INLINE  CvMat*  cvGetRow( const CvArr* arr, CvMat* submat, int row )
{
    return cvGetRows( arr, submat, row, row + 1, 1 );
}


/** @brief Returns one of more array columns.

The function returns the header, corresponding to a specified column span of the input array. That

is, no data is copied. Therefore, any modifications of the submatrix will affect the original array.
If you need to copy the columns, use cvCloneMat. cvGetCol(arr, submat, col) is a shortcut for
cvGetCols(arr, submat, col, col+1).
@param arr Input array
@param submat Pointer to the resulting sub-array header
@param start_col Zero-based index of the starting column (inclusive) of the span
@param end_col Zero-based index of the ending column (exclusive) of the span
 */
CVAPI(CvMat*) cvGetCols( const CvArr* arr, CvMat* submat,
                        int start_col, int end_col );

/** @overload
@param arr Input array
@param submat Pointer to the resulting sub-array header
@param col Zero-based index of the selected column
*/
CV_INLINE  CvMat*  cvGetCol( const CvArr* arr, CvMat* submat, int col )
{
    return cvGetCols( arr, submat, col, col + 1 );
}

/** low-level scalar <-> raw data conversion functions */
CVAPI(void) cvScalarToRawData( const CvScalar* scalar, void* data, int type,
                              int extend_to_12 CV_DEFAULT(0) );

CVAPI(void) cvRawDataToScalar( const void* data, int type, CvScalar* scalar );

/** @brief Creates a new matrix header but does not allocate the matrix data.

The function allocates a header for a multi-dimensional dense array. The array data can further be
allocated using cvCreateData or set explicitly to user-allocated data via cvSetData.
@param dims Number of array dimensions
@param sizes Array of dimension sizes
@param type Type of array elements, see cvCreateMat
 */
CVAPI(CvMatND*)  cvCreateMatNDHeader( int dims, const int* sizes, int type );

/** @brief Creates the header and allocates the data for a multi-dimensional dense array.

This function call is equivalent to the following code:
@code
    CvMatND* mat = cvCreateMatNDHeader(dims, sizes, type);
    cvCreateData(mat);
@endcode
@param dims Number of array dimensions. This must not exceed CV_MAX_DIM (32 by default, but can be
changed at build time).
@param sizes Array of dimension sizes.
@param type Type of array elements, see cvCreateMat .
 */
CVAPI(CvMatND*)  cvCreateMatND( int dims, const int* sizes, int type );

/** @brief Initializes a pre-allocated multi-dimensional array header.

@param mat A pointer to the array header to be initialized
@param dims The number of array dimensions
@param sizes An array of dimension sizes
@param type Type of array elements, see cvCreateMat
@param data Optional data pointer assigned to the matrix header
 */
CVAPI(CvMatND*)  cvInitMatNDHeader( CvMatND* mat, int dims, const int* sizes,
                                    int type, void* data CV_DEFAULT(NULL) );

/** @brief Deallocates a multi-dimensional array.

The function decrements the array data reference counter and releases the array header. If the
reference counter reaches 0, it also deallocates the data. :
@code
    if(*mat )
        cvDecRefData(*mat);
    cvFree((void**)mat);
@endcode
@param mat Double pointer to the array
 */
CV_INLINE  void  cvReleaseMatND( CvMatND** mat )
{
    cvReleaseMat( (CvMat**)mat );
}

/** Creates a copy of CvMatND (except, may be, steps) */
CVAPI(CvMatND*) cvCloneMatND( const CvMatND* mat );

/** @brief Creates sparse array.

The function allocates a multi-dimensional sparse array. Initially the array contain no elements,
that is PtrND and other related functions will return 0 for every index.
@param dims Number of array dimensions. In contrast to the dense matrix, the number of dimensions is
practically unlimited (up to \f$2^{16}\f$ ).
@param sizes Array of dimension sizes
@param type Type of array elements. The same as for CvMat
 */
CVAPI(CvSparseMat*)  cvCreateSparseMat( int dims, const int* sizes, int type );

/** @brief Deallocates sparse array.

The function releases the sparse array and clears the array pointer upon exit.
@param mat Double pointer to the array
 */
CVAPI(void)  cvReleaseSparseMat( CvSparseMat** mat );

/** Creates a copy of CvSparseMat (except, may be, zero items) */
CVAPI(CvSparseMat*) cvCloneSparseMat( const CvSparseMat* mat );

/** @brief Initializes sparse array elements iterator.

The function initializes iterator of sparse array elements and returns pointer to the first element,
or NULL if the array is empty.
@param mat Input array
@param mat_iterator Initialized iterator
 */
CVAPI(CvSparseNode*) cvInitSparseMatIterator( const CvSparseMat* mat,
                                              CvSparseMatIterator* mat_iterator );

/** @brief Returns the next sparse matrix element

The function moves iterator to the next sparse matrix element and returns pointer to it. In the
current version there is no any particular order of the elements, because they are stored in the
hash table. The sample below demonstrates how to iterate through the sparse matrix:
@code
    // print all the non-zero sparse matrix elements and compute their sum
    double sum = 0;
    int i, dims = cvGetDims(sparsemat);
    CvSparseMatIterator it;
    CvSparseNode* node = cvInitSparseMatIterator(sparsemat, &it);

    for(; node != 0; node = cvGetNextSparseNode(&it))
    {
        int* idx = CV_NODE_IDX(array, node);
        float val = *(float*)CV_NODE_VAL(array, node);
        printf("M");
        for(i = 0; i < dims; i++ )
            printf("[%d]", idx[i]);
        printf("=%g\n", val);

        sum += val;
    }

    printf("nTotal sum = %g\n", sum);
@endcode
@param mat_iterator Sparse array iterator
 */
CV_INLINE CvSparseNode* cvGetNextSparseNode( CvSparseMatIterator* mat_iterator )
{
    if( mat_iterator->node->next )
        return mat_iterator->node = mat_iterator->node->next;
    else
    {
        int idx;
        for( idx = ++mat_iterator->curidx; idx < mat_iterator->mat->hashsize; idx++ )
        {
            CvSparseNode* node = (CvSparseNode*)mat_iterator->mat->hashtable[idx];
            if( node )
            {
                mat_iterator->curidx = idx;
                return mat_iterator->node = node;
            }
        }
        return NULL;
    }
}


#define CV_MAX_ARR 10

/** matrix iterator: used for n-ary operations on dense arrays */
typedef struct CvNArrayIterator
{
    int count; /**< number of arrays */
    int dims; /**< number of dimensions to iterate */
    CvSize size; /**< maximal common linear size: { width = size, height = 1 } */
    uchar* ptr[CV_MAX_ARR]; /**< pointers to the array slices */
    int stack[CV_MAX_DIM]; /**< for internal use */
    CvMatND* hdr[CV_MAX_ARR]; /**< pointers to the headers of the
                                 matrices that are processed */
}
CvNArrayIterator;

#define CV_NO_DEPTH_CHECK     1
#define CV_NO_CN_CHECK        2
#define CV_NO_SIZE_CHECK      4

/** initializes iterator that traverses through several arrays simultaneously
   (the function together with cvNextArraySlice is used for
    N-ari element-wise operations) */
CVAPI(int) cvInitNArrayIterator( int count, CvArr** arrs,
                                 const CvArr* mask, CvMatND* stubs,
                                 CvNArrayIterator* array_iterator,
                                 int flags CV_DEFAULT(0) );

/** returns zero value if iteration is finished, non-zero (slice length) otherwise */
CVAPI(int) cvNextNArraySlice( CvNArrayIterator* array_iterator );


/** @brief Returns type of array elements.

The function returns type of the array elements. In the case of IplImage the type is converted to
CvMat-like representation. For example, if the image has been created as:
@code
    IplImage* img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
@endcode
The code cvGetElemType(img) will return CV_8UC3.
@param arr Input array
 */
CVAPI(int) cvGetElemType( const CvArr* arr );

/** @brief Return number of array dimensions

The function returns the array dimensionality and the array of dimension sizes. In the case of
IplImage or CvMat it always returns 2 regardless of number of image/matrix rows. For example, the
following code calculates total number of array elements:
@code
    int sizes[CV_MAX_DIM];
    int i, total = 1;
    int dims = cvGetDims(arr, size);
    for(i = 0; i < dims; i++ )
        total *= sizes[i];
@endcode
@param arr Input array
@param sizes Optional output vector of the array dimension sizes. For 2d arrays the number of rows
(height) goes first, number of columns (width) next.
 */
CVAPI(int) cvGetDims( const CvArr* arr, int* sizes CV_DEFAULT(NULL) );


/** @brief Returns array size along the specified dimension.

@param arr Input array
@param index Zero-based dimension index (for matrices 0 means number of rows, 1 means number of
columns; for images 0 means height, 1 means width)
 */
CVAPI(int) cvGetDimSize( const CvArr* arr, int index );


/** @brief Return pointer to a particular array element.

The functions return a pointer to a specific array element. Number of array dimension should match
to the number of indices passed to the function except for cvPtr1D function that can be used for
sequential access to 1D, 2D or nD dense arrays.

The functions can be used for sparse arrays as well - if the requested node does not exist they
create it and set it to zero.

All these as well as other functions accessing array elements ( cvGetND , cvGetRealND , cvSet
, cvSetND , cvSetRealND ) raise an error in case if the element index is out of range.
@param arr Input array
@param idx0 The first zero-based component of the element index
@param type Optional output parameter: type of matrix elements
 */
CVAPI(uchar*) cvPtr1D( const CvArr* arr, int idx0, int* type CV_DEFAULT(NULL));
/** @overload */
CVAPI(uchar*) cvPtr2D( const CvArr* arr, int idx0, int idx1, int* type CV_DEFAULT(NULL) );
/** @overload */
CVAPI(uchar*) cvPtr3D( const CvArr* arr, int idx0, int idx1, int idx2,
                      int* type CV_DEFAULT(NULL));
/** @overload
@param arr Input array
@param idx Array of the element indices
@param type Optional output parameter: type of matrix elements
@param create_node Optional input parameter for sparse matrices. Non-zero value of the parameter
means that the requested element is created if it does not exist already.
@param precalc_hashval Optional input parameter for sparse matrices. If the pointer is not NULL,
the function does not recalculate the node hash value, but takes it from the specified location.
It is useful for speeding up pair-wise operations (TODO: provide an example)
*/
CVAPI(uchar*) cvPtrND( const CvArr* arr, const int* idx, int* type CV_DEFAULT(NULL),
                      int create_node CV_DEFAULT(1),
                      unsigned* precalc_hashval CV_DEFAULT(NULL));

/** @brief Return a specific array element.

The functions return a specific array element. In the case of a sparse array the functions return 0
if the requested node does not exist (no new node is created by the functions).
@param arr Input array
@param idx0 The first zero-based component of the element index
 */
CVAPI(CvScalar) cvGet1D( const CvArr* arr, int idx0 );
/** @overload */
CVAPI(CvScalar) cvGet2D( const CvArr* arr, int idx0, int idx1 );
/** @overload */
CVAPI(CvScalar) cvGet3D( const CvArr* arr, int idx0, int idx1, int idx2 );
/** @overload
@param arr Input array
@param idx Array of the element indices
*/
CVAPI(CvScalar) cvGetND( const CvArr* arr, const int* idx );

/** @brief Return a specific element of single-channel 1D, 2D, 3D or nD array.

Returns a specific element of a single-channel array. If the array has multiple channels, a runtime
error is raised. Note that Get?D functions can be used safely for both single-channel and
multiple-channel arrays though they are a bit slower.

In the case of a sparse array the functions return 0 if the requested node does not exist (no new
node is created by the functions).
@param arr Input array. Must have a single channel.
@param idx0 The first zero-based component of the element index
 */
CVAPI(double) cvGetReal1D( const CvArr* arr, int idx0 );
/** @overload */
CVAPI(double) cvGetReal2D( const CvArr* arr, int idx0, int idx1 );
/** @overload */
CVAPI(double) cvGetReal3D( const CvArr* arr, int idx0, int idx1, int idx2 );
/** @overload
@param arr Input array. Must have a single channel.
@param idx Array of the element indices
*/
CVAPI(double) cvGetRealND( const CvArr* arr, const int* idx );

/** @brief Change the particular array element.

The functions assign the new value to a particular array element. In the case of a sparse array the
functions create the node if it does not exist yet.
@param arr Input array
@param idx0 The first zero-based component of the element index
@param value The assigned value
 */
CVAPI(void) cvSet1D( CvArr* arr, int idx0, CvScalar value );
/** @overload */
CVAPI(void) cvSet2D( CvArr* arr, int idx0, int idx1, CvScalar value );
/** @overload */
CVAPI(void) cvSet3D( CvArr* arr, int idx0, int idx1, int idx2, CvScalar value );
/** @overload
@param arr Input array
@param idx Array of the element indices
@param value The assigned value
*/
CVAPI(void) cvSetND( CvArr* arr, const int* idx, CvScalar value );

/** @brief Change a specific array element.

The functions assign a new value to a specific element of a single-channel array. If the array has
multiple channels, a runtime error is raised. Note that the Set\*D function can be used safely for
both single-channel and multiple-channel arrays, though they are a bit slower.

In the case of a sparse array the functions create the node if it does not yet exist.
@param arr Input array
@param idx0 The first zero-based component of the element index
@param value The assigned value
 */
CVAPI(void) cvSetReal1D( CvArr* arr, int idx0, double value );
/** @overload */
CVAPI(void) cvSetReal2D( CvArr* arr, int idx0, int idx1, double value );
/** @overload */
CVAPI(void) cvSetReal3D( CvArr* arr, int idx0,
                        int idx1, int idx2, double value );
/** @overload
@param arr Input array
@param idx Array of the element indices
@param value The assigned value
*/
CVAPI(void) cvSetRealND( CvArr* arr, const int* idx, double value );

/** clears element of ND dense array,
   in case of sparse arrays it deletes the specified node */
CVAPI(void) cvClearND( CvArr* arr, const int* idx );

/** @brief Returns matrix header for arbitrary array.

The function returns a matrix header for the input array that can be a matrix - CvMat, an image -
IplImage, or a multi-dimensional dense array - CvMatND (the third option is allowed only if
allowND != 0) . In the case of matrix the function simply returns the input pointer. In the case of
IplImage\* or CvMatND it initializes the header structure with parameters of the current image ROI
and returns &header. Because COI is not supported by CvMat, it is returned separately.

The function provides an easy way to handle both types of arrays - IplImage and CvMat using the same
code. Input array must have non-zero data pointer, otherwise the function will report an error.

@note If the input array is IplImage with planar data layout and COI set, the function returns the
pointer to the selected plane and COI == 0. This feature allows user to process IplImage structures
with planar data layout, even though OpenCV does not support such images.
@param arr Input array
@param header Pointer to CvMat structure used as a temporary buffer
@param coi Optional output parameter for storing COI
@param allowND If non-zero, the function accepts multi-dimensional dense arrays (CvMatND\*) and
returns 2D matrix (if CvMatND has two dimensions) or 1D matrix (when CvMatND has 1 dimension or
more than 2 dimensions). The CvMatND array must be continuous.
@sa cvGetImage, cvarrToMat.
 */
CVAPI(CvMat*) cvGetMat( const CvArr* arr, CvMat* header,
                       int* coi CV_DEFAULT(NULL),
                       int allowND CV_DEFAULT(0));

/** @brief Returns image header for arbitrary array.

The function returns the image header for the input array that can be a matrix (CvMat) or image
(IplImage). In the case of an image the function simply returns the input pointer. In the case of
CvMat it initializes an image_header structure with the parameters of the input matrix. Note that
if we transform IplImage to CvMat using cvGetMat and then transform CvMat back to IplImage using
this function, we will get different headers if the ROI is set in the original image.
@param arr Input array
@param image_header Pointer to IplImage structure used as a temporary buffer
 */
CVAPI(IplImage*) cvGetImage( const CvArr* arr, IplImage* image_header );


#define cvReshapeND( arr, header, new_cn, new_dims, new_sizes )   \
      cvReshapeMatND( (arr), sizeof(*(header)), (header),         \
                      (new_cn), (new_dims), (new_sizes))

/** @brief Changes shape of matrix/image without copying data.

The function initializes the CvMat header so that it points to the same data as the original array
but has a different shape - different number of channels, different number of rows, or both.

The following example code creates one image buffer and two image headers, the first is for a
320x240x3 image and the second is for a 960x240x1 image:
@code
    IplImage* color_img = cvCreateImage(cvSize(320,240), IPL_DEPTH_8U, 3);
    CvMat gray_mat_hdr;
    IplImage gray_img_hdr, *gray_img;
    cvReshape(color_img, &gray_mat_hdr, 1);
    gray_img = cvGetImage(&gray_mat_hdr, &gray_img_hdr);
@endcode
And the next example converts a 3x3 matrix to a single 1x9 vector:
@code
    CvMat* mat = cvCreateMat(3, 3, CV_32F);
    CvMat row_header, *row;
    row = cvReshape(mat, &row_header, 0, 1);
@endcode
@param arr Input array
@param header Output header to be filled
@param new_cn New number of channels. 'new_cn = 0' means that the number of channels remains
unchanged.
@param new_rows New number of rows. 'new_rows = 0' means that the number of rows remains
unchanged unless it needs to be changed according to new_cn value.
*/
CVAPI(CvMat*) cvReshape( const CvArr* arr, CvMat* header,
                        int new_cn, int new_rows CV_DEFAULT(0) );

/** @brief Allocates array data

The function allocates image, matrix or multi-dimensional dense array data. Note that in the case of
matrix types OpenCV allocation functions are used. In the case of IplImage they are used unless
CV_TURN_ON_IPL_COMPATIBILITY() has been called before. In the latter case IPL functions are used
to allocate the data.
@param arr Array header
 */
CVAPI(void)  cvCreateData( CvArr* arr );

/** @brief Releases array data.

The function releases the array data. In the case of CvMat or CvMatND it simply calls
cvDecRefData(), that is the function can not deallocate external data. See also the note to
cvCreateData .
@param arr Array header
 */
CVAPI(void)  cvReleaseData( CvArr* arr );

/** @brief Assigns user data to the array header.

The function assigns user data to the array header. Header should be initialized before using
cvCreateMatHeader, cvCreateImageHeader, cvCreateMatNDHeader, cvInitMatHeader,
cvInitImageHeader or cvInitMatNDHeader.
@param arr Array header
@param data User data
@param step Full row length in bytes
 */
CVAPI(void)  cvSetData( CvArr* arr, void* data, int step );

/** @brief Retrieves low-level information about the array.

The function fills output variables with low-level information about the array data. All output

parameters are optional, so some of the pointers may be set to NULL. If the array is IplImage with
ROI set, the parameters of ROI are returned.

The following example shows how to get access to array elements. It computes absolute values of the
array elements :
@code
    float* data;
    int step;
    CvSize size;

    cvGetRawData(array, (uchar**)&data, &step, &size);
    step /= sizeof(data[0]);

    for(int y = 0; y < size.height; y++, data += step )
        for(int x = 0; x < size.width; x++ )
            data[x] = (float)fabs(data[x]);
@endcode
@param arr Array header
@param data Output pointer to the whole image origin or ROI origin if ROI is set
@param step Output full row length in bytes
@param roi_size Output ROI size
 */
CVAPI(void) cvGetRawData( const CvArr* arr, uchar** data,
                         int* step CV_DEFAULT(NULL),
                         CvSize* roi_size CV_DEFAULT(NULL));

/** @brief Returns size of matrix or image ROI.

The function returns number of rows (CvSize::height) and number of columns (CvSize::width) of the
input matrix or image. In the case of image the size of ROI is returned.
@param arr array header
 */
CVAPI(CvSize) cvGetSize( const CvArr* arr );

/** @brief Copies one array to another.

The function copies selected elements from an input array to an output array:

\f[\texttt{dst} (I)= \texttt{src} (I)  \quad \text{if} \quad \texttt{mask} (I)  \ne 0.\f]

If any of the passed arrays is of IplImage type, then its ROI and COI fields are used. Both arrays
must have the same type, the same number of dimensions, and the same size. The function can also
copy sparse arrays (mask is not supported in this case).
@param src The source array
@param dst The destination array
@param mask Operation mask, 8-bit single channel array; specifies elements of the destination array
to be changed
 */
CVAPI(void)  cvCopy( const CvArr* src, CvArr* dst,
                     const CvArr* mask CV_DEFAULT(NULL) );

/** @brief Sets every element of an array to a given value.

The function copies the scalar value to every selected element of the destination array:
\f[\texttt{arr} (I)= \texttt{value} \quad \text{if} \quad \texttt{mask} (I)  \ne 0\f]
If array arr is of IplImage type, then is ROI used, but COI must not be set.
@param arr The destination array
@param value Fill value
@param mask Operation mask, 8-bit single channel array; specifies elements of the destination
array to be changed
 */
CVAPI(void)  cvSet( CvArr* arr, CvScalar value,
                    const CvArr* mask CV_DEFAULT(NULL) );

/** @brief Clears the array.

The function clears the array. In the case of dense arrays (CvMat, CvMatND or IplImage),
cvZero(array) is equivalent to cvSet(array,cvScalarAll(0),0). In the case of sparse arrays all the
elements are removed.
@param arr Array to be cleared
 */
CVAPI(void)  cvSetZero( CvArr* arr );
#define cvZero  cvSetZero


/** Splits a multi-channel array into the set of single-channel arrays or
   extracts particular [color] plane */
CVAPI(void)  cvSplit( const CvArr* src, CvArr* dst0, CvArr* dst1,
                      CvArr* dst2, CvArr* dst3 );

/** Merges a set of single-channel arrays into the single multi-channel array
   or inserts one particular [color] plane to the array */
CVAPI(void)  cvMerge( const CvArr* src0, const CvArr* src1,
                      const CvArr* src2, const CvArr* src3,
                      CvArr* dst );

/** Copies several channels from input arrays to
   certain channels of output arrays */
CVAPI(void)  cvMixChannels( const CvArr** src, int src_count,
                            CvArr** dst, int dst_count,
                            const int* from_to, int pair_count );

/** @brief Converts one array to another with optional linear transformation.

The function has several different purposes, and thus has several different names. It copies one
array to another with optional scaling, which is performed first, and/or optional type conversion,
performed after:

\f[\texttt{dst} (I) =  \texttt{scale} \texttt{src} (I) + ( \texttt{shift} _0, \texttt{shift} _1,...)\f]

All the channels of multi-channel arrays are processed independently.

The type of conversion is done with rounding and saturation, that is if the result of scaling +
conversion can not be represented exactly by a value of the destination array element type, it is
set to the nearest representable value on the real axis.
@param src Source array
@param dst Destination array
@param scale Scale factor
@param shift Value added to the scaled source array elements
 */
CVAPI(void)  cvConvertScale( const CvArr* src, CvArr* dst,
                             double scale CV_DEFAULT(1),
                             double shift CV_DEFAULT(0) );
#define cvCvtScale cvConvertScale
#define cvScale  cvConvertScale
#define cvConvert( src, dst )  cvConvertScale( (src), (dst), 1, 0 )


/** Performs linear transformation on every source array element,
   stores absolute value of the result:
   dst(x,y,c) = abs(scale*src(x,y,c)+shift).
   destination array must have 8u type.
   In other cases one may use cvConvertScale + cvAbsDiffS */
CVAPI(void)  cvConvertScaleAbs( const CvArr* src, CvArr* dst,
                                double scale CV_DEFAULT(1),
                                double shift CV_DEFAULT(0) );
#define cvCvtScaleAbs  cvConvertScaleAbs


/** checks termination criteria validity and
   sets eps to default_eps (if it is not set),
   max_iter to default_max_iters (if it is not set)
*/
CVAPI(CvTermCriteria) cvCheckTermCriteria( CvTermCriteria criteria,
                                           double default_eps,
                                           int default_max_iters );

/****************************************************************************************\
*                   Arithmetic, logic and comparison operations                          *
\****************************************************************************************/

/** dst(mask) = src1(mask) + src2(mask) */
CVAPI(void)  cvAdd( const CvArr* src1, const CvArr* src2, CvArr* dst,
                    const CvArr* mask CV_DEFAULT(NULL));

/** dst(mask) = src(mask) + value */
CVAPI(void)  cvAddS( const CvArr* src, CvScalar value, CvArr* dst,
                     const CvArr* mask CV_DEFAULT(NULL));

/** dst(mask) = src1(mask) - src2(mask) */
CVAPI(void)  cvSub( const CvArr* src1, const CvArr* src2, CvArr* dst,
                    const CvArr* mask CV_DEFAULT(NULL));

/** dst(mask) = src(mask) - value = src(mask) + (-value) */
CV_INLINE  void  cvSubS( const CvArr* src, CvScalar value, CvArr* dst,
                         const CvArr* mask CV_DEFAULT(NULL))
{
    cvAddS( src, cvScalar( -value.val[0], -value.val[1], -value.val[2], -value.val[3]),
            dst, mask );
}

/** dst(mask) = value - src(mask) */
CVAPI(void)  cvSubRS( const CvArr* src, CvScalar value, CvArr* dst,
                      const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src1(idx) * src2(idx) * scale
   (scaled element-wise multiplication of 2 arrays) */
CVAPI(void)  cvMul( const CvArr* src1, const CvArr* src2,
                    CvArr* dst, double scale CV_DEFAULT(1) );

/** element-wise division/inversion with scaling:
    dst(idx) = src1(idx) * scale / src2(idx)
    or dst(idx) = scale / src2(idx) if src1 == 0 */
CVAPI(void)  cvDiv( const CvArr* src1, const CvArr* src2,
                    CvArr* dst, double scale CV_DEFAULT(1));

/** dst = src1 * scale + src2 */
CVAPI(void)  cvScaleAdd( const CvArr* src1, CvScalar scale,
                         const CvArr* src2, CvArr* dst );

/** @brief Calculates the dot product of two arrays in Euclidean metrics.

The function calculates and returns the Euclidean dot product of two arrays.

\f[src1  \bullet src2 =  \sum _I ( \texttt{src1} (I)  \texttt{src2} (I))\f]

In the case of multiple channel arrays, the results for all channels are accumulated. In particular,
cvDotProduct(a,a) where a is a complex vector, will return \f$||\texttt{a}||^2\f$. The function can
process multi-dimensional arrays, row by row, layer by layer, and so on.
@param src1 The first source array
@param src2 The second source array
 */
CVAPI(double)  cvDotProduct( const CvArr* src1, const CvArr* src2 );

/** dst(idx) = src1(idx) & src2(idx) */
CVAPI(void) cvAnd( const CvArr* src1, const CvArr* src2,
                  CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src(idx) & value */
CVAPI(void) cvAndS( const CvArr* src, CvScalar value,
                   CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src1(idx) | src2(idx) */
CVAPI(void) cvOr( const CvArr* src1, const CvArr* src2,
                 CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src(idx) | value */
CVAPI(void) cvOrS( const CvArr* src, CvScalar value,
                  CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src1(idx) ^ src2(idx) */
CVAPI(void) cvXor( const CvArr* src1, const CvArr* src2,
                  CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = src(idx) ^ value */
CVAPI(void) cvXorS( const CvArr* src, CvScalar value,
                   CvArr* dst, const CvArr* mask CV_DEFAULT(NULL));

/** dst(idx) = ~src(idx) */
CVAPI(void) cvNot( const CvArr* src, CvArr* dst );

#define CV_CMP_EQ   0
#define CV_CMP_GT   1
#define CV_CMP_GE   2
#define CV_CMP_LT   3
#define CV_CMP_LE   4
#define CV_CMP_NE   5

/** The comparison operation support single-channel arrays only.
   Destination image should be 8uC1 or 8sC1 */

/** dst(idx) = src1(idx) _cmp_op_ value */
CVAPI(void) cvCmpS( const CvArr* src, double value, CvArr* dst, int cmp_op );

/****************************************************************************************\
*                                Math operations                                         *
\****************************************************************************************/

/** Does powering: dst(idx) = src(idx)^power */
CVAPI(void)  cvPow( const CvArr* src, CvArr* dst, double power );

/** Does exponention: dst(idx) = exp(src(idx)).
   Overflow is not handled yet. Underflow is handled.
   Maximal relative error is ~7e-6 for single-precision input */
CVAPI(void)  cvExp( const CvArr* src, CvArr* dst );


#define  CV_CHECK_RANGE    1
#define  CV_CHECK_QUIET    2
/** Checks array values for NaNs, Infs or simply for too large numbers
   (if CV_CHECK_RANGE is set). If CV_CHECK_QUIET is set,
   no runtime errors is raised (function returns zero value in case of "bad" values).
   */
CVAPI(int)  cvCheckArr( const CvArr* arr, int flags CV_DEFAULT(0),
                        double min_val CV_DEFAULT(0), double max_val CV_DEFAULT(0));
#define cvCheckArray cvCheckArr

#define CV_RAND_UNI      0
#define CV_RAND_NORMAL   1

#define CV_SORT_EVERY_ROW 0
#define CV_SORT_EVERY_COLUMN 1
#define CV_SORT_ASCENDING 0
#define CV_SORT_DESCENDING 16

/** Finds all real and complex roots of a polynomial equation */
CVAPI(void) cvSolvePoly(const CvMat* coeffs, CvMat *roots2,
      int maxiter CV_DEFAULT(20), int fig CV_DEFAULT(100));

/****************************************************************************************\
*                                Matrix operations                                       *
\****************************************************************************************/

/** @brief Calculates the cross product of two 3D vectors.

The function calculates the cross product of two 3D vectors:
\f[\texttt{dst} =  \texttt{src1} \times \texttt{src2}\f]
or:
\f[\begin{array}{l} \texttt{dst} _1 =  \texttt{src1} _2  \texttt{src2} _3 -  \texttt{src1} _3  \texttt{src2} _2 \\ \texttt{dst} _2 =  \texttt{src1} _3  \texttt{src2} _1 -  \texttt{src1} _1  \texttt{src2} _3 \\ \texttt{dst} _3 =  \texttt{src1} _1  \texttt{src2} _2 -  \texttt{src1} _2  \texttt{src2} _1 \end{array}\f]
@param src1 The first source vector
@param src2 The second source vector
@param dst The destination vector
 */
CVAPI(void)  cvCrossProduct( const CvArr* src1, const CvArr* src2, CvArr* dst );

/** Matrix transform: dst = A*B + C, C is optional */
#define cvMatMulAdd( src1, src2, src3, dst ) cvGEMM( (src1), (src2), 1., (src3), 1., (dst), 0 )
#define cvMatMul( src1, src2, dst )  cvMatMulAdd( (src1), (src2), NULL, (dst))

#define CV_GEMM_A_T 1
#define CV_GEMM_B_T 2
#define CV_GEMM_C_T 4
/** Extended matrix transform:
   dst = alpha*op(A)*op(B) + beta*op(C), where op(X) is X or X^T */
CVAPI(void)  cvGEMM( const CvArr* src1, const CvArr* src2, double alpha,
                     const CvArr* src3, double beta, CvArr* dst,
                     int tABC CV_DEFAULT(0));
#define cvMatMulAddEx cvGEMM

/** Transforms each element of source array and stores
   resultant vectors in destination array */
CVAPI(void)  cvTransform( const CvArr* src, CvArr* dst,
                          const CvMat* transmat,
                          const CvMat* shiftvec CV_DEFAULT(NULL));
#define cvMatMulAddS cvTransform

/** Does perspective transform on every element of input array */
CVAPI(void)  cvPerspectiveTransform( const CvArr* src, CvArr* dst,
                                     const CvMat* mat );

/** Calculates (A-delta)*(A-delta)^T (order=0) or (A-delta)^T*(A-delta) (order=1) */
CVAPI(void) cvMulTransposed( const CvArr* src, CvArr* dst, int order,
                             const CvArr* delta CV_DEFAULT(NULL),
                             double scale CV_DEFAULT(1.) );

/** Transposes matrix. Square matrices can be transposed in-place */
CVAPI(void)  cvTranspose( const CvArr* src, CvArr* dst );
#define cvT cvTranspose

/** Completes the symmetric matrix from the lower (LtoR=0) or from the upper (LtoR!=0) part */
CVAPI(void)  cvCompleteSymm( CvMat* matrix, int LtoR CV_DEFAULT(0) );

/** Mirror array data around horizontal (flip=0),
   vertical (flip=1) or both(flip=-1) axises:
   cvFlip(src) flips images vertically and sequences horizontally (inplace) */
CVAPI(void)  cvFlip( const CvArr* src, CvArr* dst CV_DEFAULT(NULL),
                     int flip_mode CV_DEFAULT(0));
#define cvMirror cvFlip


#define CV_SVD_MODIFY_A   1
#define CV_SVD_U_T        2
#define CV_SVD_V_T        4

/** Performs Singular Value Decomposition of a matrix */
CVAPI(void)   cvSVD( CvArr* A, CvArr* W, CvArr* U CV_DEFAULT(NULL),
                     CvArr* V CV_DEFAULT(NULL), int flags CV_DEFAULT(0));

/** Performs Singular Value Back Substitution (solves A*X = B):
   flags must be the same as in cvSVD */
CVAPI(void)   cvSVBkSb( const CvArr* W, const CvArr* U,
                        const CvArr* V, const CvArr* B,
                        CvArr* X, int flags );

#define CV_LU  0
#define CV_SVD 1
#define CV_SVD_SYM 2
#define CV_CHOLESKY 3
#define CV_QR  4
#define CV_NORMAL 16

/** Inverts matrix */
CVAPI(double)  cvInvert( const CvArr* src, CvArr* dst,
                         int method CV_DEFAULT(CV_LU));
#define cvInv cvInvert

/** Solves linear system (src1)*(dst) = (src2)
   (returns 0 if src1 is a singular and CV_LU method is used) */
CVAPI(int)  cvSolve( const CvArr* src1, const CvArr* src2, CvArr* dst,
                     int method CV_DEFAULT(CV_LU));

/** Calculates determinant of input matrix */
CVAPI(double) cvDet( const CvArr* mat );

/** Calculates trace of the matrix (sum of elements on the main diagonal) */
CVAPI(CvScalar) cvTrace( const CvArr* mat );

/** Finds eigen values and vectors of a symmetric matrix */
CVAPI(void)  cvEigenVV( CvArr* mat, CvArr* evects, CvArr* evals,
                        double eps CV_DEFAULT(0),
                        int lowindex CV_DEFAULT(-1),
                        int highindex CV_DEFAULT(-1));

///* Finds selected eigen values and vectors of a symmetric matrix */
//CVAPI(void)  cvSelectedEigenVV( CvArr* mat, CvArr* evects, CvArr* evals,
//                                int lowindex, int highindex );

/** Makes an identity matrix (mat_ij = i == j) */
CVAPI(void)  cvSetIdentity( CvArr* mat, CvScalar value CV_DEFAULT(cvRealScalar(1)) );

/** Fills matrix with given range of numbers */
CVAPI(CvArr*)  cvRange( CvArr* mat, double start, double end );

/**   @anchor core_c_CovarFlags
@name Flags for cvCalcCovarMatrix
@see cvCalcCovarMatrix
  @{
*/

/** flag for cvCalcCovarMatrix, transpose([v1-avg, v2-avg,...]) * [v1-avg,v2-avg,...] */
#define CV_COVAR_SCRAMBLED 0

/** flag for cvCalcCovarMatrix, [v1-avg, v2-avg,...] * transpose([v1-avg,v2-avg,...]) */
#define CV_COVAR_NORMAL    1

/** flag for cvCalcCovarMatrix, do not calc average (i.e. mean vector) - use the input vector instead
   (useful for calculating covariance matrix by parts) */
#define CV_COVAR_USE_AVG   2

/** flag for cvCalcCovarMatrix, scale the covariance matrix coefficients by number of the vectors */
#define CV_COVAR_SCALE     4

/** flag for cvCalcCovarMatrix, all the input vectors are stored in a single matrix, as its rows */
#define CV_COVAR_ROWS      8

/** flag for cvCalcCovarMatrix, all the input vectors are stored in a single matrix, as its columns */
#define CV_COVAR_COLS     16

/** @} */

/** Calculates covariation matrix for a set of vectors
@see @ref core_c_CovarFlags "flags"
*/
CVAPI(void)  cvCalcCovarMatrix( const CvArr** vects, int count,
                                CvArr* cov_mat, CvArr* avg, int flags );

#define CV_PCA_DATA_AS_ROW 0
#define CV_PCA_DATA_AS_COL 1
#define CV_PCA_USE_AVG 2
CVAPI(void)  cvCalcPCA( const CvArr* data, CvArr* mean,
                        CvArr* eigenvals, CvArr* eigenvects, int flags );

CVAPI(void)  cvProjectPCA( const CvArr* data, const CvArr* mean,
                           const CvArr* eigenvects, CvArr* result );

CVAPI(void)  cvBackProjectPCA( const CvArr* proj, const CvArr* mean,
                               const CvArr* eigenvects, CvArr* result );

/** Calculates Mahalanobis(weighted) distance */
CVAPI(double)  cvMahalanobis( const CvArr* vec1, const CvArr* vec2, const CvArr* mat );
#define cvMahalonobis  cvMahalanobis

/****************************************************************************************\
*                                    Array Statistics                                    *
\****************************************************************************************/

/** Finds sum of array elements */
CVAPI(CvScalar)  cvSum( const CvArr* arr );

/** Calculates number of non-zero pixels */
CVAPI(int)  cvCountNonZero( const CvArr* arr );

/** Calculates mean value of array elements */
CVAPI(CvScalar)  cvAvg( const CvArr* arr, const CvArr* mask CV_DEFAULT(NULL) );

/** Calculates mean and standard deviation of pixel values */
CVAPI(void)  cvAvgSdv( const CvArr* arr, CvScalar* mean, CvScalar* std_dev,
                       const CvArr* mask CV_DEFAULT(NULL) );

/** Finds global minimum, maximum and their positions */
CVAPI(void)  cvMinMaxLoc( const CvArr* arr, double* min_val, double* max_val,
                          CvPoint* min_loc CV_DEFAULT(NULL),
                          CvPoint* max_loc CV_DEFAULT(NULL),
                          const CvArr* mask CV_DEFAULT(NULL) );

/** @anchor core_c_NormFlags
  @name Flags for cvNorm and cvNormalize
  @{
*/
#define CV_C            1
#define CV_L1           2
#define CV_L2           4
#define CV_NORM_MASK    7
#define CV_RELATIVE     8
#define CV_DIFF         16
#define CV_MINMAX       32

#define CV_DIFF_C       (CV_DIFF | CV_C)
#define CV_DIFF_L1      (CV_DIFF | CV_L1)
#define CV_DIFF_L2      (CV_DIFF | CV_L2)
#define CV_RELATIVE_C   (CV_RELATIVE | CV_C)
#define CV_RELATIVE_L1  (CV_RELATIVE | CV_L1)
#define CV_RELATIVE_L2  (CV_RELATIVE | CV_L2)
/** @} */

/** Finds norm, difference norm or relative difference norm for an array (or two arrays)
@see ref core_c_NormFlags "flags"
*/
CVAPI(double)  cvNorm( const CvArr* arr1, const CvArr* arr2 CV_DEFAULT(NULL),
                       int norm_type CV_DEFAULT(CV_L2),
                       const CvArr* mask CV_DEFAULT(NULL) );

/** @see ref core_c_NormFlags "flags" */
CVAPI(void)  cvNormalize( const CvArr* src, CvArr* dst,
                          double a CV_DEFAULT(1.), double b CV_DEFAULT(0.),
                          int norm_type CV_DEFAULT(CV_L2),
                          const CvArr* mask CV_DEFAULT(NULL) );

/** @anchor core_c_ReduceFlags
  @name Flags for cvReduce
  @{
*/
#define CV_REDUCE_SUM 0
#define CV_REDUCE_AVG 1
#define CV_REDUCE_MAX 2
#define CV_REDUCE_MIN 3
/** @} */

/** @see @ref core_c_ReduceFlags "flags" */
CVAPI(void)  cvReduce( const CvArr* src, CvArr* dst, int dim CV_DEFAULT(-1),
                       int op CV_DEFAULT(CV_REDUCE_SUM) );

/****************************************************************************************\
*                      Discrete Linear Transforms and Related Functions                  *
\****************************************************************************************/

/** @anchor core_c_DftFlags
  @name Flags for cvDFT, cvDCT and cvMulSpectrums
  @{
  */
#define CV_DXT_FORWARD  0
#define CV_DXT_INVERSE  1
#define CV_DXT_SCALE    2 /**< divide result by size of array */
#define CV_DXT_INV_SCALE (CV_DXT_INVERSE + CV_DXT_SCALE)
#define CV_DXT_INVERSE_SCALE CV_DXT_INV_SCALE
#define CV_DXT_ROWS     4 /**< transform each row individually */
#define CV_DXT_MUL_CONJ 8 /**< conjugate the second argument of cvMulSpectrums */
/** @} */

/** Discrete Fourier Transform:
    complex->complex,
    real->ccs (forward),
    ccs->real (inverse)
@see core_c_DftFlags "flags"
*/
CVAPI(void)  cvDFT( const CvArr* src, CvArr* dst, int flags,
                    int nonzero_rows CV_DEFAULT(0) );
#define cvFFT cvDFT

/** Multiply results of DFTs: DFT(X)*DFT(Y) or DFT(X)*conj(DFT(Y))
@see core_c_DftFlags "flags"
*/
CVAPI(void)  cvMulSpectrums( const CvArr* src1, const CvArr* src2,
                             CvArr* dst, int flags );

/** Finds optimal DFT vector size >= size0 */
CVAPI(int)  cvGetOptimalDFTSize( int size0 );

/** Discrete Cosine Transform
@see core_c_DftFlags "flags"
*/
CVAPI(void)  cvDCT( const CvArr* src, CvArr* dst, int flags );

/****************************************************************************************\
*                              Dynamic data structures                                   *
\****************************************************************************************/

/** Calculates length of sequence slice (with support of negative indices). */
CVAPI(int) cvSliceLength( CvSlice slice, const CvSeq* seq );


/** Creates new memory storage.
   block_size == 0 means that default,
   somewhat optimal size, is used (currently, it is 64K) */
CVAPI(CvMemStorage*)  cvCreateMemStorage( int block_size CV_DEFAULT(0));


/** Creates a memory storage that will borrow memory blocks from parent storage */
CVAPI(CvMemStorage*)  cvCreateChildMemStorage( CvMemStorage* parent );


/** Releases memory storage. All the children of a parent must be released before
   the parent. A child storage returns all the blocks to parent when it is released */
CVAPI(void)  cvReleaseMemStorage( CvMemStorage** storage );


/** Clears memory storage. This is the only way(!!!) (besides cvRestoreMemStoragePos)
   to reuse memory allocated for the storage - cvClearSeq,cvClearSet ...
   do not free any memory.
   A child storage returns all the blocks to the parent when it is cleared */
CVAPI(void)  cvClearMemStorage( CvMemStorage* storage );

/** Remember a storage "free memory" position */
CVAPI(void)  cvSaveMemStoragePos( const CvMemStorage* storage, CvMemStoragePos* pos );

/** Restore a storage "free memory" position */
CVAPI(void)  cvRestoreMemStoragePos( CvMemStorage* storage, CvMemStoragePos* pos );

/** Allocates continuous buffer of the specified size in the storage */
CVAPI(void*) cvMemStorageAlloc( CvMemStorage* storage, size_t size );

/** Allocates string in memory storage */
//CVAPI(CvString) cvMemStorageAllocString( CvMemStorage* storage, const char* ptr,
//                                         int len CV_DEFAULT(-1) );

/** Creates new empty sequence that will reside in the specified storage */
CVAPI(CvSeq*)  cvCreateSeq( int seq_flags, size_t header_size,
                            size_t elem_size, CvMemStorage* storage );

/** Changes default size (granularity) of sequence blocks.
   The default size is ~1Kbyte */
CVAPI(void)  cvSetSeqBlockSize( CvSeq* seq, int delta_elems );


/** Adds new element to the end of sequence. Returns pointer to the element */
CVAPI(schar*)  cvSeqPush( CvSeq* seq, const void* element CV_DEFAULT(NULL));


/** Adds new element to the beginning of sequence. Returns pointer to it */
CVAPI(schar*)  cvSeqPushFront( CvSeq* seq, const void* element CV_DEFAULT(NULL));


/** Removes the last element from sequence and optionally saves it */
CVAPI(void)  cvSeqPop( CvSeq* seq, void* element CV_DEFAULT(NULL));


/** Removes the first element from sequence and optioanally saves it */
CVAPI(void)  cvSeqPopFront( CvSeq* seq, void* element CV_DEFAULT(NULL));


#define CV_FRONT 1
#define CV_BACK 0
/** Adds several new elements to the end of sequence */
CVAPI(void)  cvSeqPushMulti( CvSeq* seq, const void* elements,
                             int count, int in_front CV_DEFAULT(0) );

/** Removes several elements from the end of sequence and optionally saves them */
CVAPI(void)  cvSeqPopMulti( CvSeq* seq, void* elements,
                            int count, int in_front CV_DEFAULT(0) );

/** Inserts a new element in the middle of sequence.
   cvSeqInsert(seq,0,elem) == cvSeqPushFront(seq,elem) */
CVAPI(schar*)  cvSeqInsert( CvSeq* seq, int before_index,
                            const void* element CV_DEFAULT(NULL));

/** Removes specified sequence element */
CVAPI(void)  cvSeqRemove( CvSeq* seq, int index );


/** Removes all the elements from the sequence. The freed memory
   can be reused later only by the same sequence unless cvClearMemStorage
   or cvRestoreMemStoragePos is called */
CVAPI(void)  cvClearSeq( CvSeq* seq );


/** Retrieves pointer to specified sequence element.
   Negative indices are supported and mean counting from the end
   (e.g -1 means the last sequence element) */
CVAPI(schar*)  cvGetSeqElem( const CvSeq* seq, int index );

/** Calculates index of the specified sequence element.
   Returns -1 if element does not belong to the sequence */
CVAPI(int)  cvSeqElemIdx( const CvSeq* seq, const void* element,
                         CvSeqBlock** block CV_DEFAULT(NULL) );

/** Initializes sequence writer. The new elements will be added to the end of sequence */
CVAPI(void)  cvStartAppendToSeq( CvSeq* seq, CvSeqWriter* writer );


/** Combination of cvCreateSeq and cvStartAppendToSeq */
CVAPI(void)  cvStartWriteSeq( int seq_flags, int header_size,
                              int elem_size, CvMemStorage* storage,
                              CvSeqWriter* writer );

/** Closes sequence writer, updates sequence header and returns pointer
   to the resultant sequence
   (which may be useful if the sequence was created using cvStartWriteSeq))
*/
CVAPI(CvSeq*)  cvEndWriteSeq( CvSeqWriter* writer );


/** Updates sequence header. May be useful to get access to some of previously
   written elements via cvGetSeqElem or sequence reader */
CVAPI(void)   cvFlushSeqWriter( CvSeqWriter* writer );


/** Initializes sequence reader.
   The sequence can be read in forward or backward direction */
CVAPI(void) cvStartReadSeq( const CvSeq* seq, CvSeqReader* reader,
                           int reverse CV_DEFAULT(0) );


/** Returns current sequence reader position (currently observed sequence element) */
CVAPI(int)  cvGetSeqReaderPos( CvSeqReader* reader );


/** Changes sequence reader position. It may seek to an absolute or
   to relative to the current position */
CVAPI(void)   cvSetSeqReaderPos( CvSeqReader* reader, int index,
                                 int is_relative CV_DEFAULT(0));

/** Copies sequence content to a continuous piece of memory */
CVAPI(void*)  cvCvtSeqToArray( const CvSeq* seq, void* elements,
                               CvSlice slice CV_DEFAULT(CV_WHOLE_SEQ) );

/** Creates sequence header for array.
   After that all the operations on sequences that do not alter the content
   can be applied to the resultant sequence */
CVAPI(CvSeq*) cvMakeSeqHeaderForArray( int seq_type, int header_size,
                                       int elem_size, void* elements, int total,
                                       CvSeq* seq, CvSeqBlock* block );

/** Extracts sequence slice (with or without copying sequence elements) */
CVAPI(CvSeq*) cvSeqSlice( const CvSeq* seq, CvSlice slice,
                         CvMemStorage* storage CV_DEFAULT(NULL),
                         int copy_data CV_DEFAULT(0));

CV_INLINE CvSeq* cvCloneSeq( const CvSeq* seq, CvMemStorage* storage CV_DEFAULT(NULL))
{
    return cvSeqSlice( seq, CV_WHOLE_SEQ, storage, 1 );
}

/** Removes sequence slice */
CVAPI(void)  cvSeqRemoveSlice( CvSeq* seq, CvSlice slice );

/** Inserts a sequence or array into another sequence */
CVAPI(void)  cvSeqInsertSlice( CvSeq* seq, int before_index, const CvArr* from_arr );

/** a < b ? -1 : a > b ? 1 : 0 */
typedef int (CV_CDECL* CvCmpFunc)(const void* a, const void* b, void* userdata );

/** Sorts sequence in-place given element comparison function */
CVAPI(void) cvSeqSort( CvSeq* seq, CvCmpFunc func, void* userdata CV_DEFAULT(NULL) );

/** Finds element in a [sorted] sequence */
CVAPI(schar*) cvSeqSearch( CvSeq* seq, const void* elem, CvCmpFunc func,
                           int is_sorted, int* elem_idx,
                           void* userdata CV_DEFAULT(NULL) );

/** Reverses order of sequence elements in-place */
CVAPI(void) cvSeqInvert( CvSeq* seq );

/** Splits sequence into one or more equivalence classes using the specified criteria */
CVAPI(int)  cvSeqPartition( const CvSeq* seq, CvMemStorage* storage,
                            CvSeq** labels, CvCmpFunc is_equal, void* userdata );

/************ Internal sequence functions ************/
CVAPI(void)  cvChangeSeqBlock( void* reader, int direction );
CVAPI(void)  cvCreateSeqBlock( CvSeqWriter* writer );


/** Creates a new set */
CVAPI(CvSet*)  cvCreateSet( int set_flags, int header_size,
                            int elem_size, CvMemStorage* storage );

/** Adds new element to the set and returns pointer to it */
CVAPI(int)  cvSetAdd( CvSet* set_header, CvSetElem* elem CV_DEFAULT(NULL),
                      CvSetElem** inserted_elem CV_DEFAULT(NULL) );

/** Fast variant of cvSetAdd */
CV_INLINE  CvSetElem* cvSetNew( CvSet* set_header )
{
    CvSetElem* elem = set_header->free_elems;
    if( elem )
    {
        set_header->free_elems = elem->next_free;
        elem->flags = elem->flags & CV_SET_ELEM_IDX_MASK;
        set_header->active_count++;
    }
    else
        cvSetAdd( set_header, NULL, &elem );
    return elem;
}

/** Removes set element given its pointer */
CV_INLINE  void cvSetRemoveByPtr( CvSet* set_header, void* elem )
{
    CvSetElem* _elem = (CvSetElem*)elem;
    assert( _elem->flags >= 0 /*&& (elem->flags & CV_SET_ELEM_IDX_MASK) < set_header->total*/ );
    _elem->next_free = set_header->free_elems;
    _elem->flags = (_elem->flags & CV_SET_ELEM_IDX_MASK) | CV_SET_ELEM_FREE_FLAG;
    set_header->free_elems = _elem;
    set_header->active_count--;
}

/** Removes element from the set by its index  */
CVAPI(void)   cvSetRemove( CvSet* set_header, int index );

/** Returns a set element by index. If the element doesn't belong to the set,
   NULL is returned */
CV_INLINE CvSetElem* cvGetSetElem( const CvSet* set_header, int idx )
{
    CvSetElem* elem = (CvSetElem*)(void *)cvGetSeqElem( (CvSeq*)set_header, idx );
    return elem && CV_IS_SET_ELEM( elem ) ? elem : 0;
}

/** Removes all the elements from the set */
CVAPI(void)  cvClearSet( CvSet* set_header );

/** Creates new graph */
CVAPI(CvGraph*)  cvCreateGraph( int graph_flags, int header_size,
                                int vtx_size, int edge_size,
                                CvMemStorage* storage );

/** Adds new vertex to the graph */
CVAPI(int)  cvGraphAddVtx( CvGraph* graph, const CvGraphVtx* vtx CV_DEFAULT(NULL),
                           CvGraphVtx** inserted_vtx CV_DEFAULT(NULL) );


/** Removes vertex from the graph together with all incident edges */
CVAPI(int)  cvGraphRemoveVtx( CvGraph* graph, int index );
CVAPI(int)  cvGraphRemoveVtxByPtr( CvGraph* graph, CvGraphVtx* vtx );


/** Link two vertices specified by indices or pointers if they
   are not connected or return pointer to already existing edge
   connecting the vertices.
   Functions return 1 if a new edge was created, 0 otherwise */
CVAPI(int)  cvGraphAddEdge( CvGraph* graph,
                            int start_idx, int end_idx,
                            const CvGraphEdge* edge CV_DEFAULT(NULL),
                            CvGraphEdge** inserted_edge CV_DEFAULT(NULL) );

CVAPI(int)  cvGraphAddEdgeByPtr( CvGraph* graph,
                               CvGraphVtx* start_vtx, CvGraphVtx* end_vtx,
                               const CvGraphEdge* edge CV_DEFAULT(NULL),
                               CvGraphEdge** inserted_edge CV_DEFAULT(NULL) );

/** Remove edge connecting two vertices */
CVAPI(void)  cvGraphRemoveEdge( CvGraph* graph, int start_idx, int end_idx );
CVAPI(void)  cvGraphRemoveEdgeByPtr( CvGraph* graph, CvGraphVtx* start_vtx,
                                     CvGraphVtx* end_vtx );

/** Find edge connecting two vertices */
CVAPI(CvGraphEdge*)  cvFindGraphEdge( const CvGraph* graph, int start_idx, int end_idx );
CVAPI(CvGraphEdge*)  cvFindGraphEdgeByPtr( const CvGraph* graph,
                                           const CvGraphVtx* start_vtx,
                                           const CvGraphVtx* end_vtx );
#define cvGraphFindEdge cvFindGraphEdge
#define cvGraphFindEdgeByPtr cvFindGraphEdgeByPtr

/** Remove all vertices and edges from the graph */
CVAPI(void)  cvClearGraph( CvGraph* graph );


/** Count number of edges incident to the vertex */
CVAPI(int)  cvGraphVtxDegree( const CvGraph* graph, int vtx_idx );
CVAPI(int)  cvGraphVtxDegreeByPtr( const CvGraph* graph, const CvGraphVtx* vtx );


/** Retrieves graph vertex by given index */
#define cvGetGraphVtx( graph, idx ) (CvGraphVtx*)cvGetSetElem((CvSet*)(graph), (idx))

/** Retrieves index of a graph vertex given its pointer */
#define cvGraphVtxIdx( graph, vtx ) ((vtx)->flags & CV_SET_ELEM_IDX_MASK)

/** Retrieves index of a graph edge given its pointer */
#define cvGraphEdgeIdx( graph, edge ) ((edge)->flags & CV_SET_ELEM_IDX_MASK)

#define cvGraphGetVtxCount( graph ) ((graph)->active_count)
#define cvGraphGetEdgeCount( graph ) ((graph)->edges->active_count)

#define  CV_GRAPH_VERTEX        1
#define  CV_GRAPH_TREE_EDGE     2
#define  CV_GRAPH_BACK_EDGE     4
#define  CV_GRAPH_FORWARD_EDGE  8
#define  CV_GRAPH_CROSS_EDGE    16
#define  CV_GRAPH_ANY_EDGE      30
#define  CV_GRAPH_NEW_TREE      32
#define  CV_GRAPH_BACKTRACKING  64
#define  CV_GRAPH_OVER          -1

#define  CV_GRAPH_ALL_ITEMS    -1

/** flags for graph vertices and edges */
#define  CV_GRAPH_ITEM_VISITED_FLAG  (1 << 30)
#define  CV_IS_GRAPH_VERTEX_VISITED(vtx) \
    (((CvGraphVtx*)(vtx))->flags & CV_GRAPH_ITEM_VISITED_FLAG)
#define  CV_IS_GRAPH_EDGE_VISITED(edge) \
    (((CvGraphEdge*)(edge))->flags & CV_GRAPH_ITEM_VISITED_FLAG)
#define  CV_GRAPH_SEARCH_TREE_NODE_FLAG   (1 << 29)
#define  CV_GRAPH_FORWARD_EDGE_FLAG       (1 << 28)

typedef struct CvGraphScanner
{
    CvGraphVtx* vtx;       /* current graph vertex (or current edge origin) */
    CvGraphVtx* dst;       /* current graph edge destination vertex */
    CvGraphEdge* edge;     /* current edge */

    CvGraph* graph;        /* the graph */
    CvSeq*   stack;        /* the graph vertex stack */
    int      index;        /* the lower bound of certainly visited vertices */
    int      mask;         /* event mask */
}
CvGraphScanner;

/** Creates new graph scanner. */
CVAPI(CvGraphScanner*)  cvCreateGraphScanner( CvGraph* graph,
                                             CvGraphVtx* vtx CV_DEFAULT(NULL),
                                             int mask CV_DEFAULT(CV_GRAPH_ALL_ITEMS));

/** Releases graph scanner. */
CVAPI(void) cvReleaseGraphScanner( CvGraphScanner** scanner );

/** Get next graph element */
CVAPI(int)  cvNextGraphItem( CvGraphScanner* scanner );

/** Creates a copy of graph */
CVAPI(CvGraph*) cvCloneGraph( const CvGraph* graph, CvMemStorage* storage );


/** Does look-up transformation. Elements of the source array
   (that should be 8uC1 or 8sC1) are used as indexes in lutarr 256-element table */
CVAPI(void) cvLUT( const CvArr* src, CvArr* dst, const CvArr* lut );


/******************* Iteration through the sequence tree *****************/
typedef struct CvTreeNodeIterator
{
    const void* node;
    int level;
    int max_level;
}
CvTreeNodeIterator;

CVAPI(void) cvInitTreeNodeIterator( CvTreeNodeIterator* tree_iterator,
                                   const void* first, int max_level );
CVAPI(void*) cvNextTreeNode( CvTreeNodeIterator* tree_iterator );
CVAPI(void*) cvPrevTreeNode( CvTreeNodeIterator* tree_iterator );

/** Inserts sequence into tree with specified "parent" sequence.
   If parent is equal to frame (e.g. the most external contour),
   then added contour will have null pointer to parent. */
CVAPI(void) cvInsertNodeIntoTree( void* node, void* parent, void* frame );

/** Removes contour from tree (together with the contour children). */
CVAPI(void) cvRemoveNodeFromTree( void* node, void* frame );

/** Gathers pointers to all the sequences,
   accessible from the `first`, to the single sequence */
CVAPI(CvSeq*) cvTreeToNodeSeq( const void* first, int header_size,
                              CvMemStorage* storage );

/****************************************************************************************\
*                                    System functions                                    *
\****************************************************************************************/

typedef IplImage* (CV_STDCALL* Cv_iplCreateImageHeader)
                            (int,int,int,char*,char*,int,int,int,int,int,
                            IplROI*,IplImage*,void*,IplTileInfo*);
typedef void (CV_STDCALL* Cv_iplAllocateImageData)(IplImage*,int,int);
typedef void (CV_STDCALL* Cv_iplDeallocate)(IplImage*,int);
typedef IplROI* (CV_STDCALL* Cv_iplCreateROI)(int,int,int,int,int);
typedef IplImage* (CV_STDCALL* Cv_iplCloneImage)(const IplImage*);

/** @brief Makes OpenCV use IPL functions for allocating IplImage and IplROI structures.

Normally, the function is not called directly. Instead, a simple macro
CV_TURN_ON_IPL_COMPATIBILITY() is used that calls cvSetIPLAllocators and passes there pointers
to IPL allocation functions. :
@code
    ...
    CV_TURN_ON_IPL_COMPATIBILITY()
    ...
@endcode
@param create_header pointer to a function, creating IPL image header.
@param allocate_data pointer to a function, allocating IPL image data.
@param deallocate pointer to a function, deallocating IPL image.
@param create_roi pointer to a function, creating IPL image ROI (i.e. Region of Interest).
@param clone_image pointer to a function, cloning an IPL image.
 */
CVAPI(void) cvSetIPLAllocators( Cv_iplCreateImageHeader create_header,
                               Cv_iplAllocateImageData allocate_data,
                               Cv_iplDeallocate deallocate,
                               Cv_iplCreateROI create_roi,
                               Cv_iplCloneImage clone_image );

#define CV_TURN_ON_IPL_COMPATIBILITY()                                  \
    cvSetIPLAllocators( iplCreateImageHeader, iplAllocateImage,         \
                        iplDeallocate, iplCreateROI, iplCloneImage )


/** @brief Releases an object.

 The function finds the type of a given object and calls release with the double pointer.
 @param struct_ptr Double pointer to the object
 */
CVAPI(void) cvRelease( void** struct_ptr );

/** @brief Makes a clone of an object.

The function finds the type of a given object and calls clone with the passed object. Of course, if
you know the object type, for example, struct_ptr is CvMat\*, it is faster to call the specific
function, like cvCloneMat.
@param struct_ptr The object to clone
 */
CVAPI(void*) cvClone( const void* struct_ptr );

/*********************************** Multi-Threading ************************************/

/** retrieve/set the number of threads used in OpenMP implementations */
CVAPI(int)  cvGetNumThreads( void );
CVAPI(void) cvSetNumThreads( int threads CV_DEFAULT(0) );
/** get index of the thread being executed */
CVAPI(int)  cvGetThreadNum( void );


/********************************** Error Handling **************************************/

/** Get current OpenCV error status */
CVAPI(int) cvGetErrStatus( void );

/** Sets error status silently */
CVAPI(void) cvSetErrStatus( int status );

#define CV_ErrModeLeaf     0   /* Print error and exit program */
#define CV_ErrModeParent   1   /* Print error and continue */
#define CV_ErrModeSilent   2   /* Don't print and continue */

/** Retrieves current error processing mode */
CVAPI(int) cvGetErrMode( void );

/** Sets error processing mode, returns previously used mode */
CVAPI(int) cvSetErrMode( int mode );

/** Retrieves detailed information about the last error occurred */
CVAPI(int) cvGetErrInfo( const char** errcode_desc, const char** description,
                        const char** filename, int* line );

/** Maps IPP error codes to the counterparts from OpenCV */
CVAPI(int) cvErrorFromIppStatus( int ipp_status );

#define OPENCV_ERROR(status,func,context)                           \
cv::error((status),(func),(context),__FILE__,__LINE__)

#define OPENCV_ASSERT(expr,func,context)                            \
{if (! (expr))                                      \
{OPENCV_ERROR(CV_StsInternal,(func),(context));}}

#define OPENCV_CALL( Func )                                         \
{                                                                   \
Func;                                                           \
}

/**
 CV_CHECK macro checks error status after CV (or IPL)
 function call. If error detected, control will be transferred to the exit
 label.
 */
#define CV_CHECK()                                                  \
{                                                                   \
    if( cvGetErrStatus() < 0 )                                      \
        CV_ERROR( CV_StsBackTrace, "Inner function failed." );      \
}


/**
 CV_CALL macro calls CV (or IPL) function, checks error status and
 signals a error if the function failed. Useful in "parent node"
 error processing mode
 */
#define CV_CALL( Func )                                             \
{                                                                   \
    Func;                                                           \
    CV_CHECK();                                                     \
}


/** Runtime assertion macro */
#define CV_ASSERT( Condition )                                          \
{                                                                       \
    if( !(Condition) )                                                  \
        CV_ERROR( CV_StsInternal, "Assertion: " #Condition " failed" ); \
}

#define __CV_BEGIN__       {
#define __CV_END__         goto exit; exit: ; }
#define __CV_EXIT__        goto exit

/** @} core_c */

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

#include "opencv2/core/utility.hpp"

namespace cv
{

//! @addtogroup core_c_glue
//! @{

/////////////////////////////////////////// glue ///////////////////////////////////////////

//! converts array (CvMat or IplImage) to cv::Mat
CV_EXPORTS Mat cvarrToMat(const CvArr* arr, bool copyData=false,
                          bool allowND=true, int coiMode=0,
                          AutoBuffer<double>* buf=0);


//! extracts Channel of Interest from CvMat or IplImage and makes cv::Mat out of it.
CV_EXPORTS void extractImageCOI(const CvArr* arr, OutputArray coiimg, int coi=-1);


////// specialized implementations of DefaultDeleter::operator() for classic OpenCV types //////

template<> struct DefaultDeleter<CvMat>{ CV_EXPORTS void operator ()(CvMat* obj) const; };
template<> struct DefaultDeleter<IplImage>{ CV_EXPORTS void operator ()(IplImage* obj) const; };
template<> struct DefaultDeleter<CvMatND>{ CV_EXPORTS void operator ()(CvMatND* obj) const; };
template<> struct DefaultDeleter<CvSparseMat>{ CV_EXPORTS void operator ()(CvSparseMat* obj) const; };
template<> struct DefaultDeleter<CvMemStorage>{ CV_EXPORTS void operator ()(CvMemStorage* obj) const; };

////////////// convenient wrappers for operating old-style dynamic structures //////////////

template<typename _Tp> class SeqIterator;

typedef Ptr<CvMemStorage> MemStorage;

/*!
 Template Sequence Class derived from CvSeq

 The class provides more convenient access to sequence elements,
 STL-style operations and iterators.

 \note The class is targeted for simple data types,
    i.e. no constructors or destructors
    are called for the sequence elements.
*/
template<typename _Tp> class Seq
{
public:
    typedef SeqIterator<_Tp> iterator;
    typedef SeqIterator<_Tp> const_iterator;

    //! the default constructor
    Seq();
    //! the constructor for wrapping CvSeq structure. The real element type in CvSeq should match _Tp.
    Seq(const CvSeq* seq);
    //! creates the empty sequence that resides in the specified storage
    Seq(MemStorage& storage, int headerSize = sizeof(CvSeq));
    //! returns read-write reference to the specified element
    _Tp& operator [](int idx);
    //! returns read-only reference to the specified element
    const _Tp& operator[](int idx) const;
    //! returns iterator pointing to the beginning of the sequence
    SeqIterator<_Tp> begin() const;
    //! returns iterator pointing to the element following the last sequence element
    SeqIterator<_Tp> end() const;
    //! returns the number of elements in the sequence
    size_t size() const;
    //! returns the type of sequence elements (CV_8UC1 ... CV_64FC(CV_CN_MAX) ...)
    int type() const;
    //! returns the depth of sequence elements (CV_8U ... CV_64F)
    int depth() const;
    //! returns the number of channels in each sequence element
    int channels() const;
    //! returns the size of each sequence element
    size_t elemSize() const;
    //! returns index of the specified sequence element
    size_t index(const _Tp& elem) const;
    //! appends the specified element to the end of the sequence
    void push_back(const _Tp& elem);
    //! appends the specified element to the front of the sequence
    void push_front(const _Tp& elem);
    //! appends zero or more elements to the end of the sequence
    void push_back(const _Tp* elems, size_t count);
    //! appends zero or more elements to the front of the sequence
    void push_front(const _Tp* elems, size_t count);
    //! inserts the specified element to the specified position
    void insert(int idx, const _Tp& elem);
    //! inserts zero or more elements to the specified position
    void insert(int idx, const _Tp* elems, size_t count);
    //! removes element at the specified position
    void remove(int idx);
    //! removes the specified subsequence
    void remove(const Range& r);

    //! returns reference to the first sequence element
    _Tp& front();
    //! returns read-only reference to the first sequence element
    const _Tp& front() const;
    //! returns reference to the last sequence element
    _Tp& back();
    //! returns read-only reference to the last sequence element
    const _Tp& back() const;
    //! returns true iff the sequence contains no elements
    bool empty() const;

    //! removes all the elements from the sequence
    void clear();
    //! removes the first element from the sequence
    void pop_front();
    //! removes the last element from the sequence
    void pop_back();
    //! removes zero or more elements from the beginning of the sequence
    void pop_front(_Tp* elems, size_t count);
    //! removes zero or more elements from the end of the sequence
    void pop_back(_Tp* elems, size_t count);

    //! copies the whole sequence or the sequence slice to the specified vector
    void copyTo(std::vector<_Tp>& vec, const Range& range=Range::all()) const;
    //! returns the vector containing all the sequence elements
    operator std::vector<_Tp>() const;

    CvSeq* seq;
};


/*!
 STL-style Sequence Iterator inherited from the CvSeqReader structure
*/
template<typename _Tp> class SeqIterator : public CvSeqReader
{
public:
    //! the default constructor
    SeqIterator();
    //! the constructor setting the iterator to the beginning or to the end of the sequence
    SeqIterator(const Seq<_Tp>& seq, bool seekEnd=false);
    //! positions the iterator within the sequence
    void seek(size_t pos);
    //! reports the current iterator position
    size_t tell() const;
    //! returns reference to the current sequence element
    _Tp& operator *();
    //! returns read-only reference to the current sequence element
    const _Tp& operator *() const;
    //! moves iterator to the next sequence element
    SeqIterator& operator ++();
    //! moves iterator to the next sequence element
    SeqIterator operator ++(int) const;
    //! moves iterator to the previous sequence element
    SeqIterator& operator --();
    //! moves iterator to the previous sequence element
    SeqIterator operator --(int) const;

    //! moves iterator forward by the specified offset (possibly negative)
    SeqIterator& operator +=(int);
    //! moves iterator backward by the specified offset (possibly negative)
    SeqIterator& operator -=(int);

    // this is index of the current element module seq->total*2
    // (to distinguish between 0 and seq->total)
    int index;
};



// bridge C++ => C Seq API
CV_EXPORTS schar*  seqPush( CvSeq* seq, const void* element=0);
CV_EXPORTS schar*  seqPushFront( CvSeq* seq, const void* element=0);
CV_EXPORTS void  seqPop( CvSeq* seq, void* element=0);
CV_EXPORTS void  seqPopFront( CvSeq* seq, void* element=0);
CV_EXPORTS void  seqPopMulti( CvSeq* seq, void* elements,
                              int count, int in_front=0 );
CV_EXPORTS void  seqRemove( CvSeq* seq, int index );
CV_EXPORTS void  clearSeq( CvSeq* seq );
CV_EXPORTS schar*  getSeqElem( const CvSeq* seq, int index );
CV_EXPORTS void  seqRemoveSlice( CvSeq* seq, CvSlice slice );
CV_EXPORTS void  seqInsertSlice( CvSeq* seq, int before_index, const CvArr* from_arr );

template<typename _Tp> inline Seq<_Tp>::Seq() : seq(0) {}
template<typename _Tp> inline Seq<_Tp>::Seq( const CvSeq* _seq ) : seq((CvSeq*)_seq)
{
    CV_Assert(!_seq || _seq->elem_size == sizeof(_Tp));
}

template<typename _Tp> inline Seq<_Tp>::Seq( MemStorage& storage,
                                             int headerSize )
{
    CV_Assert(headerSize >= (int)sizeof(CvSeq));
    seq = cvCreateSeq(DataType<_Tp>::type, headerSize, sizeof(_Tp), storage);
}

template<typename _Tp> inline _Tp& Seq<_Tp>::operator [](int idx)
{ return *(_Tp*)getSeqElem(seq, idx); }

template<typename _Tp> inline const _Tp& Seq<_Tp>::operator [](int idx) const
{ return *(_Tp*)getSeqElem(seq, idx); }

template<typename _Tp> inline SeqIterator<_Tp> Seq<_Tp>::begin() const
{ return SeqIterator<_Tp>(*this); }

template<typename _Tp> inline SeqIterator<_Tp> Seq<_Tp>::end() const
{ return SeqIterator<_Tp>(*this, true); }

template<typename _Tp> inline size_t Seq<_Tp>::size() const
{ return seq ? seq->total : 0; }

template<typename _Tp> inline int Seq<_Tp>::type() const
{ return seq ? CV_MAT_TYPE(seq->flags) : 0; }

template<typename _Tp> inline int Seq<_Tp>::depth() const
{ return seq ? CV_MAT_DEPTH(seq->flags) : 0; }

template<typename _Tp> inline int Seq<_Tp>::channels() const
{ return seq ? CV_MAT_CN(seq->flags) : 0; }

template<typename _Tp> inline size_t Seq<_Tp>::elemSize() const
{ return seq ? seq->elem_size : 0; }

template<typename _Tp> inline size_t Seq<_Tp>::index(const _Tp& elem) const
{ return cvSeqElemIdx(seq, &elem); }

template<typename _Tp> inline void Seq<_Tp>::push_back(const _Tp& elem)
{ cvSeqPush(seq, &elem); }

template<typename _Tp> inline void Seq<_Tp>::push_front(const _Tp& elem)
{ cvSeqPushFront(seq, &elem); }

template<typename _Tp> inline void Seq<_Tp>::push_back(const _Tp* elem, size_t count)
{ cvSeqPushMulti(seq, elem, (int)count, 0); }

template<typename _Tp> inline void Seq<_Tp>::push_front(const _Tp* elem, size_t count)
{ cvSeqPushMulti(seq, elem, (int)count, 1); }

template<typename _Tp> inline _Tp& Seq<_Tp>::back()
{ return *(_Tp*)getSeqElem(seq, -1); }

template<typename _Tp> inline const _Tp& Seq<_Tp>::back() const
{ return *(const _Tp*)getSeqElem(seq, -1); }

template<typename _Tp> inline _Tp& Seq<_Tp>::front()
{ return *(_Tp*)getSeqElem(seq, 0); }

template<typename _Tp> inline const _Tp& Seq<_Tp>::front() const
{ return *(const _Tp*)getSeqElem(seq, 0); }

template<typename _Tp> inline bool Seq<_Tp>::empty() const
{ return !seq || seq->total == 0; }

template<typename _Tp> inline void Seq<_Tp>::clear()
{ if(seq) clearSeq(seq); }

template<typename _Tp> inline void Seq<_Tp>::pop_back()
{ seqPop(seq); }

template<typename _Tp> inline void Seq<_Tp>::pop_front()
{ seqPopFront(seq); }

template<typename _Tp> inline void Seq<_Tp>::pop_back(_Tp* elem, size_t count)
{ seqPopMulti(seq, elem, (int)count, 0); }

template<typename _Tp> inline void Seq<_Tp>::pop_front(_Tp* elem, size_t count)
{ seqPopMulti(seq, elem, (int)count, 1); }

template<typename _Tp> inline void Seq<_Tp>::insert(int idx, const _Tp& elem)
{ seqInsert(seq, idx, &elem); }

template<typename _Tp> inline void Seq<_Tp>::insert(int idx, const _Tp* elems, size_t count)
{
    CvMat m = cvMat(1, count, DataType<_Tp>::type, elems);
    seqInsertSlice(seq, idx, &m);
}

template<typename _Tp> inline void Seq<_Tp>::remove(int idx)
{ seqRemove(seq, idx); }

template<typename _Tp> inline void Seq<_Tp>::remove(const Range& r)
{ seqRemoveSlice(seq, cvSlice(r.start, r.end)); }

template<typename _Tp> inline void Seq<_Tp>::copyTo(std::vector<_Tp>& vec, const Range& range) const
{
    size_t len = !seq ? 0 : range == Range::all() ? seq->total : range.end - range.start;
    vec.resize(len);
    if( seq && len )
        cvCvtSeqToArray(seq, &vec[0], cvSlice(range));
}

template<typename _Tp> inline Seq<_Tp>::operator std::vector<_Tp>() const
{
    std::vector<_Tp> vec;
    copyTo(vec);
    return vec;
}

template<typename _Tp> inline SeqIterator<_Tp>::SeqIterator()
{ memset(this, 0, sizeof(*this)); }

template<typename _Tp> inline SeqIterator<_Tp>::SeqIterator(const Seq<_Tp>& _seq, bool seekEnd)
{
    cvStartReadSeq(_seq.seq, this);
    index = seekEnd ? _seq.seq->total : 0;
}

template<typename _Tp> inline void SeqIterator<_Tp>::seek(size_t pos)
{
    cvSetSeqReaderPos(this, (int)pos, false);
    index = pos;
}

template<typename _Tp> inline size_t SeqIterator<_Tp>::tell() const
{ return index; }

template<typename _Tp> inline _Tp& SeqIterator<_Tp>::operator *()
{ return *(_Tp*)ptr; }

template<typename _Tp> inline const _Tp& SeqIterator<_Tp>::operator *() const
{ return *(const _Tp*)ptr; }

template<typename _Tp> inline SeqIterator<_Tp>& SeqIterator<_Tp>::operator ++()
{
    CV_NEXT_SEQ_ELEM(sizeof(_Tp), *this);
    if( ++index >= seq->total*2 )
        index = 0;
    return *this;
}

template<typename _Tp> inline SeqIterator<_Tp> SeqIterator<_Tp>::operator ++(int) const
{
    SeqIterator<_Tp> it = *this;
    ++*this;
    return it;
}

template<typename _Tp> inline SeqIterator<_Tp>& SeqIterator<_Tp>::operator --()
{
    CV_PREV_SEQ_ELEM(sizeof(_Tp), *this);
    if( --index < 0 )
        index = seq->total*2-1;
    return *this;
}

template<typename _Tp> inline SeqIterator<_Tp> SeqIterator<_Tp>::operator --(int) const
{
    SeqIterator<_Tp> it = *this;
    --*this;
    return it;
}

template<typename _Tp> inline SeqIterator<_Tp>& SeqIterator<_Tp>::operator +=(int delta)
{
    cvSetSeqReaderPos(this, delta, 1);
    index += delta;
    int n = seq->total*2;
    if( index < 0 )
        index += n;
    if( index >= n )
        index -= n;
    return *this;
}

template<typename _Tp> inline SeqIterator<_Tp>& SeqIterator<_Tp>::operator -=(int delta)
{
    return (*this += -delta);
}

template<typename _Tp> inline ptrdiff_t operator - (const SeqIterator<_Tp>& a,
                                                    const SeqIterator<_Tp>& b)
{
    ptrdiff_t delta = a.index - b.index, n = a.seq->total;
    if( delta > n || delta < -n )
        delta += delta < 0 ? n : -n;
    return delta;
}

template<typename _Tp> inline bool operator == (const SeqIterator<_Tp>& a,
                                                const SeqIterator<_Tp>& b)
{
    return a.seq == b.seq && a.index == b.index;
}

template<typename _Tp> inline bool operator != (const SeqIterator<_Tp>& a,
                                                const SeqIterator<_Tp>& b)
{
    return !(a == b);
}

//! @}

} // cv

#endif

#endif
