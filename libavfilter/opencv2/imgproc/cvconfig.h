#ifndef OPENCV_CVCONFIG_H_INCLUDED
#define OPENCV_CVCONFIG_H_INCLUDED

#define CL_TARGET_OPENCL_VERSION 120

/* OpenCV compiled as static or dynamic libs */
#define BUILD_SHARED_LIBS

/* OpenCV intrinsics optimized code */
#define CV_ENABLE_INTRINSICS

/* OpenCV additional optimized code */
//#define CV_DISABLE_OPTIMIZATION

#define CV_CPU_COMPILE_SSE
#define CV_CPU_COMPILE_SSE2
#define CV_CPU_COMPILE_SSE3
#define CV_CPU_COMPILE_SSSE3
#define CV_CPU_COMPILE_SSE4_1
#define CV_CPU_COMPILE_SSE4_2
#if defined(__AVX__)
#define CV_CPU_COMPILE_POPCNT
#define CV_CPU_COMPILE_AVX
#endif
#if defined(__AVX2__)
#define CV_CPU_COMPILE_FP16
#define CV_CPU_COMPILE_AVX2
#define CV_CPU_COMPILE_FMA3
#endif

#define HAVE_WIN32_ALIGNED_MALLOC 1
#define HAVE_MALLOC_H 1

/* Compile for 'real' NVIDIA GPU architectures */
//#define CUDA_ARCH_BIN "${OPENCV_CUDA_ARCH_BIN}"

/* NVIDIA GPU features are used */
//#define CUDA_ARCH_FEATURES "${OPENCV_CUDA_ARCH_FEATURES}"

/* Compile for 'virtual' NVIDIA PTX architectures */
//#define CUDA_ARCH_PTX "${OPENCV_CUDA_ARCH_PTX}"

/* AMD's Basic Linear Algebra Subprograms Library*/
//#define HAVE_CLAMDBLAS

/* AMD's OpenCL Fast Fourier Transform Library*/
//#define HAVE_CLAMDFFT

/* Clp support */
//#define HAVE_CLP

/* NVIDIA CUDA Runtime API*/
//#define HAVE_CUDA

/* NVIDIA CUDA Basic Linear Algebra Subprograms (BLAS) API*/
//#define HAVE_CUBLAS

/* NVIDIA CUDA Deep Neural Network (cuDNN) API*/
//#define HAVE_CUDNN

/* NVIDIA CUDA Fast Fourier Transform (FFT) API*/
//#define HAVE_CUFFT

/* DirectX */
#define HAVE_DIRECTX 1
//#define HAVE_DIRECTX_NV12
#define HAVE_D3D11 1
#define HAVE_D3D10 1
#define HAVE_D3D9 1

/* Eigen Matrix & Linear Algebra Library */
//#define HAVE_EIGEN

/* Geospatial Data Abstraction Library */
//#define HAVE_GDAL

/* Vulkan support */
//#define HAVE_VULKAN

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Intel Integrated Performance Primitives */
//#define HAVE_IPP
//#define HAVE_IPP_ICV
//#define HAVE_IPP_IW
//#define HAVE_IPP_IW_LL

/* JPEG-2000 codec */
//#define HAVE_OPENJPEG
//#define HAVE_JASPER

/* AVIF codec */
//#define HAVE_AVIF

/* IJG JPEG codec */
//#define HAVE_JPEG

/* GDCM DICOM codec */
//#define HAVE_GDCM

/* NVIDIA Video Decoding API*/
//#define HAVE_NVCUVID
//#define HAVE_NVCUVID_HEADER
//#define HAVE_DYNLINK_NVCUVID_HEADER

/* NVIDIA Video Encoding API*/
//#define HAVE_NVCUVENC

/* OpenCL Support */
#define HAVE_OPENCL 1
#define HAVE_OPENCL_STATIC 1
//#define HAVE_OPENCL_SVM 1

/* NVIDIA OpenCL D3D Extensions support */
//#define HAVE_OPENCL_D3D11_NV

/* OpenEXR codec */
//#define HAVE_OPENEXR

/* OpenGL support*/
//#define HAVE_OPENGL

/* PNG codec */
//#define HAVE_PNG

/* PNG codec */
//#define HAVE_SPNG

/* Posix threads (pthreads) */
//#define HAVE_PTHREAD

/* parallel_for with pthreads */
//#define HAVE_PTHREADS_PF

/* Intel Threading Building Blocks */
//#define HAVE_TBB

/* Ste||ar Group High Performance ParallelX */
//#define HAVE_HPX

/* TIFF codec */
//#define HAVE_TIFF

/* Define if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
//#define WORDS_BIGENDIAN

/* VA library (libva) */
//#define HAVE_VA

/* Intel VA-API/OpenCL */
//#define HAVE_VA_INTEL

/* Lapack */
//#define HAVE_LAPACK

/* Library was compiled with functions instrumentation */
//#define ENABLE_INSTRUMENTATION

/* OpenCV trace utilities */
//#define OPENCV_TRACE

/* Library QR-code decoding */
//#define HAVE_QUIRC

/* The font "uni" (WQY MicroHei) is available */
//#define HAVE_UNIFONT

#endif // OPENCV_CVCONFIG_H_INCLUDED
