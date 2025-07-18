/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2010-2013, Advanced Micro Devices, Inc., all rights reserved.
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
// In no event shall the OpenCV Foundation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "../precomp.hpp"

#if defined(HAVE_OPENCL)

#include "opencv2/core.hpp" // CV_Error
#include "opencv2/core/utils/configuration.private.hpp"

#if defined(HAVE_OPENCL_STATIC)
#if defined __APPLE__
#define CL_SILENCE_DEPRECATION
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
CV_SUPPRESS_DEPRECATED_START
#include "opencl_core_static_impl.hpp"
CV_SUPPRESS_DEPRECATED_END
#include "opencv2/core/opencl/runtime/opencl_core.hpp"
#else

#include "opencv2/core/opencl/runtime/opencl_core.hpp"

#define OPENCL_FUNC_TO_CHECK_1_1 "clEnqueueReadBufferRect"
#define ERROR_MSG_CANT_LOAD "Failed to load OpenCL runtime\n"
#define ERROR_MSG_INVALID_VERSION "Failed to load OpenCL runtime (expected version 1.1+)\n"

static std::string getRuntimePath(const std::string & defaultPath)
{
    const std::string res = cv::utils::getConfigurationParameterString(
        "OPENCV_OPENCL_RUNTIME", defaultPath);
    if (res == "disabled")
        return std::string();
    else
        return res;
}

#if defined(__APPLE__)
#include <dlfcn.h>

static void* AppleCLGetProcAddress(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle && !initialized)
    {
        cv::AutoLock lock(cv::getInitializationMutex());
        if (!initialized)
        {
            const char* defaultPath = "/System/Library/Frameworks/OpenCL.framework/Versions/Current/OpenCL";
            std::string path = getRuntimePath(defaultPath);
            if (!path.empty())
                handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
            if (handle == NULL)
            {
                if (path != NULL && path != defaultPath)
                    fprintf(stderr, ERROR_MSG_CANT_LOAD);
            }
            else if (dlsym(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
            {
                fprintf(stderr, ERROR_MSG_INVALID_VERSION);
                handle = NULL;
            }
            initialized = true;
        }
    }
    if (!handle)
        return NULL;
    return dlsym(handle, name);
}
#define CV_CL_GET_PROC_ADDRESS(name) AppleCLGetProcAddress(name)
#endif // __APPLE__

#if defined(_WIN32)
#include <windows.h>

static void* WinGetProcAddress(const char* name)
{
    static bool initialized = false;
    static HMODULE handle = NULL;
    if (!handle && !initialized)
    {
        cv::AutoLock lock(cv::getInitializationMutex());
        if (!initialized)
        {
            handle = GetModuleHandleA("OpenCL.dll");
            if (!handle)
            {
                const std::string defaultPath = "OpenCL.dll";
                const std::string path = getRuntimePath(defaultPath);
                if (!path.empty())
                    handle = LoadLibraryA(path.c_str());
                if (!handle)
                {
                    if (!path.empty() && path != defaultPath)
                        fprintf(stderr, ERROR_MSG_CANT_LOAD);
                }
                else if (GetProcAddress(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
                {
                    fprintf(stderr, ERROR_MSG_INVALID_VERSION);
                    FreeLibrary(handle);
                    handle = NULL;
                }
            }
            initialized = true;
        }
    }
    if (!handle)
        return NULL;
    return (void*)GetProcAddress(handle, name);
}
#define CV_CL_GET_PROC_ADDRESS(name) WinGetProcAddress(name)
#endif // _WIN32

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <dlfcn.h>
#include <stdio.h>

static void *GetHandle(const char *file)
{
    void *handle;

    handle = dlopen(file, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle)
        return NULL;

    if (dlsym(handle, OPENCL_FUNC_TO_CHECK_1_1) == NULL)
    {
        fprintf(stderr, ERROR_MSG_INVALID_VERSION);
        dlclose(handle);
        return NULL;
    }

    return handle;
}

#ifdef __ANDROID__

static const char *defaultAndroidPaths[] = {
    "libOpenCL.so",
    "/system/lib64/libOpenCL.so",
    "/system/vendor/lib64/libOpenCL.so",
    "/system/vendor/lib64/egl/libGLES_mali.so",
    "/system/vendor/lib64/libPVROCL.so",
    "/data/data/org.pocl.libs/files/lib64/libpocl.so",
    "/system/lib/libOpenCL.so",
    "/system/vendor/lib/libOpenCL.so",
    "/system/vendor/lib/egl/libGLES_mali.so",
    "/system/vendor/lib/libPVROCL.so",
    "/data/data/org.pocl.libs/files/lib/libpocl.so"
};

static void* GetProcAddress(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle && !initialized)
    {
        cv::AutoLock lock(cv::getInitializationMutex());
        if (!initialized)
        {
            bool foundOpenCL = false;
            for (unsigned int i = 0; i < (sizeof(defaultAndroidPaths)/sizeof(char*)); i++)
            {
                const std::string path = (i==0) ? getRuntimePath(defaultAndroidPaths[i]) : defaultAndroidPaths[i];
                if (!path.empty()) {
                    handle = GetHandle(path.c_str());
                    if (handle) {
                        foundOpenCL = true;
                        break;
                    }
                }
            }
            initialized = true;
            if (!foundOpenCL)
                fprintf(stderr, ERROR_MSG_CANT_LOAD);
        }
    }
    if (!handle)
        return NULL;
    return dlsym(handle, name);
}

#else // NOT __ANDROID__

static void* GetProcAddress(const char* name)
{
    static bool initialized = false;
    static void* handle = NULL;
    if (!handle && !initialized)
    {
        cv::AutoLock lock(cv::getInitializationMutex());
        if (!initialized)
        {
            const char* defaultPath = "libOpenCL.so";
            const std::string path = getRuntimePath(defaultPath);
            if (!path.empty())
            {
                handle = GetHandle(path.c_str());
                if (!handle)
                {
                    if (path == defaultPath)
                        handle = GetHandle("libOpenCL.so.1");
                    else
                        fprintf(stderr, ERROR_MSG_CANT_LOAD);
                }
            }
            initialized = true;
        }
    }
    if (!handle)
        return NULL;
    return dlsym(handle, name);
}
#endif // __ANDROID__

#define CV_CL_GET_PROC_ADDRESS(name) GetProcAddress(name)
#endif

#ifndef CV_CL_GET_PROC_ADDRESS
#ifdef __GNUC__
#warning("OPENCV: OpenCL dynamic library loader: check configuration")
#else
#pragma message("WARNING: OPENCV: OpenCL dynamic library loader: check configuration")
#endif
#define CV_CL_GET_PROC_ADDRESS(name) NULL
#endif

static void* opencl_check_fn(int ID);

#include "runtime_common.hpp"

#include "autogenerated/opencl_core_impl.hpp"

//
// BEGIN OF CUSTOM FUNCTIONS
//

#define CUSTOM_FUNCTION_ID 1000

#endif // HAVE_OPENCL_STATIC


#if defined HAVE_OPENCL_SVM && !defined(HAVE_OPENCL_STATIC)

#include "opencv2/core/opencl/runtime/opencl_svm_20.hpp"
#define SVM_FUNCTION_ID_START CUSTOM_FUNCTION_ID
#define SVM_FUNCTION_ID_END CUSTOM_FUNCTION_ID + 100

enum OPENCL_FN_SVM_ID
{
    OPENCL_FN_clSVMAlloc = SVM_FUNCTION_ID_START,
    OPENCL_FN_clSVMFree,
    OPENCL_FN_clSetKernelArgSVMPointer,
    OPENCL_FN_clSetKernelExecInfo,
    OPENCL_FN_clEnqueueSVMFree,
    OPENCL_FN_clEnqueueSVMMemcpy,
    OPENCL_FN_clEnqueueSVMMemFill,
    OPENCL_FN_clEnqueueSVMMap,
    OPENCL_FN_clEnqueueSVMUnmap,
};

opencl_fn4(OPENCL_FN_clSVMAlloc, void*, (cl_context p1, cl_svm_mem_flags p2, size_t p3, unsigned int p4))
void* (CL_API_CALL *clSVMAlloc)(cl_context context, cl_svm_mem_flags flags, size_t size, unsigned int alignment) =
        OPENCL_FN_clSVMAlloc_switch_fn;
static const struct DynamicFnEntry _clSVMAlloc_definition = { "clSVMAlloc", (void**)&clSVMAlloc};
opencl_fn2(OPENCL_FN_clSVMFree, void, (cl_context p1, void* p2))
void (CL_API_CALL *clSVMFree)(cl_context context, void* svm_pointer) =
        OPENCL_FN_clSVMFree_switch_fn;
static const struct DynamicFnEntry _clSVMFree_definition = { "clSVMFree", (void**)&clSVMFree};
opencl_fn3(OPENCL_FN_clSetKernelArgSVMPointer, cl_int, (cl_kernel p1, cl_uint p2, const void* p3))
cl_int (CL_API_CALL *clSetKernelArgSVMPointer)(cl_kernel kernel, cl_uint arg_index, const void* arg_value) =
        OPENCL_FN_clSetKernelArgSVMPointer_switch_fn;
static const struct DynamicFnEntry _clSetKernelArgSVMPointer_definition = { "clSetKernelArgSVMPointer", (void**)&clSetKernelArgSVMPointer};
opencl_fn8(OPENCL_FN_clEnqueueSVMMemcpy, cl_int, (cl_command_queue p1, cl_bool p2, void* p3, const void* p4, size_t p5, cl_uint p6, const cl_event* p7, cl_event* p8))
cl_int (CL_API_CALL *clEnqueueSVMMemcpy)(cl_command_queue command_queue, cl_bool blocking_copy, void* dst_ptr, const void* src_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        OPENCL_FN_clEnqueueSVMMemcpy_switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMemcpy_definition = { "clEnqueueSVMMemcpy", (void**)&clEnqueueSVMMemcpy};
opencl_fn8(OPENCL_FN_clEnqueueSVMMemFill, cl_int, (cl_command_queue p1, void* p2, const void* p3, size_t p4, size_t p5, cl_uint p6, const cl_event* p7, cl_event* p8))
cl_int (CL_API_CALL *clEnqueueSVMMemFill)(cl_command_queue command_queue, void* svm_ptr, const void* pattern, size_t pattern_size, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        OPENCL_FN_clEnqueueSVMMemFill_switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMemFill_definition = { "clEnqueueSVMMemFill", (void**)&clEnqueueSVMMemFill};
opencl_fn8(OPENCL_FN_clEnqueueSVMMap, cl_int, (cl_command_queue p1, cl_bool p2, cl_map_flags p3, void* p4, size_t p5, cl_uint p6, const cl_event* p7, cl_event* p8))
cl_int (CL_API_CALL *clEnqueueSVMMap)(cl_command_queue command_queue, cl_bool blocking_map, cl_map_flags map_flags, void* svm_ptr, size_t size, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        OPENCL_FN_clEnqueueSVMMap_switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMMap_definition = { "clEnqueueSVMMap", (void**)&clEnqueueSVMMap};
opencl_fn5(OPENCL_FN_clEnqueueSVMUnmap, cl_int, (cl_command_queue p1, void* p2, cl_uint p3, const cl_event* p4, cl_event* p5))
cl_int (CL_API_CALL *clEnqueueSVMUnmap)(cl_command_queue command_queue, void* svm_ptr, cl_uint num_events_in_wait_list, const cl_event* event_wait_list, cl_event* event) =
        OPENCL_FN_clEnqueueSVMUnmap_switch_fn;
static const struct DynamicFnEntry _clEnqueueSVMUnmap_definition = { "clEnqueueSVMUnmap", (void**)&clEnqueueSVMUnmap};

static const struct DynamicFnEntry* opencl_svm_fn_list[] = {
    &_clSVMAlloc_definition,
    &_clSVMFree_definition,
    &_clSetKernelArgSVMPointer_definition,
    NULL/*&_clSetKernelExecInfo_definition*/,
    NULL/*&_clEnqueueSVMFree_definition*/,
    &_clEnqueueSVMMemcpy_definition,
    &_clEnqueueSVMMemFill_definition,
    &_clEnqueueSVMMap_definition,
    &_clEnqueueSVMUnmap_definition,
};
#endif // HAVE_OPENCL_SVM

//
// END OF CUSTOM FUNCTIONS HERE
//

#if !defined(HAVE_OPENCL_STATIC)
static void* opencl_check_fn(int ID)
{
    const struct DynamicFnEntry* e = NULL;
    if (ID < CUSTOM_FUNCTION_ID)
    {
        CV_Assert(ID >= 0 && ID < (int)(sizeof(opencl_fn_list)/sizeof(opencl_fn_list[0])));
        e = opencl_fn_list[ID];
    }
#ifdef HAVE_OPENCL_SVM
    else if (ID >= SVM_FUNCTION_ID_START && ID < SVM_FUNCTION_ID_END)
    {
        ID = ID - SVM_FUNCTION_ID_START;
        CV_Assert(ID >= 0 && ID < (int)(sizeof(opencl_svm_fn_list)/sizeof(opencl_svm_fn_list[0])));
        e = opencl_svm_fn_list[ID];
    }
#endif
    else
    {
        CV_Error(cv::Error::StsBadArg, "Invalid function ID");
    }
    void* func = CV_CL_GET_PROC_ADDRESS(e->fnName);
    if (!func)
    {
        throw cv::Exception(cv::Error::OpenCLApiCallError,
                cv::format("OpenCL function is not available: [%s]", e->fnName),
                CV_Func, __FILE__, __LINE__);
    }
    *(e->ppFn) = func;
    return func;
}
#endif // HAVE_OPENCL_STATIC

#ifdef HAVE_OPENGL

#if defined(HAVE_OPENCL_STATIC)

#if defined __APPLE__
#include <OpenCL/cl_gl.h>
#else
#include <CL/cl_gl.h>
#endif
#ifdef cl_khr_gl_sharing
CV_SUPPRESS_DEPRECATED_START
#include "autogenerated/opencl_gl_static_impl.hpp"
CV_SUPPRESS_DEPRECATED_END
#include "opencv2/core/opencl/runtime/opencl_gl.hpp"
#endif

#else // HAVE_OPENCL_STATIC

#include "opencv2/core/opencl/runtime/opencl_gl.hpp"

#ifdef cl_khr_gl_sharing

static void* opencl_gl_check_fn(int ID);

#include "autogenerated/opencl_gl_impl.hpp"

static void* opencl_gl_check_fn(int ID)
{
    const struct DynamicFnEntry* e = NULL;
    CV_Assert(ID >= 0 && ID < (int)(sizeof(opencl_gl_fn_list)/sizeof(opencl_gl_fn_list[0])));
    e = opencl_gl_fn_list[ID];
    void* func = CV_CL_GET_PROC_ADDRESS(e->fnName);
    if (!func)
    {
        throw cv::Exception(cv::Error::OpenCLApiCallError,
                cv::format("OpenCL function is not available: [%s]", e->fnName),
                CV_Func, __FILE__, __LINE__);
    }
    *(e->ppFn) = func;
    return func;
}

#endif // cl_khr_gl_sharing

#endif // HAVE_OPENCL_STATIC

#endif // HAVE_OPENGL

#endif
