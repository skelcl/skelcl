//
// OpenCLInfo.cpp - Print OpenCL platform and device information.
//
// Author: Chris Cummins <chrisc.101@gmail.com>
//
// Based on opencl_test.cpp by Kyle Lutz. License:
//---------------------------------------------------------------------------//
// Copyright (c) 2013-2014 Kyle Lutz <kyle.r.lutz@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See http://kylelutz.github.com/compute for more information.
//---------------------------------------------------------------------------//

#include <iostream>

#include <CL/cl.h>

// Check that return code was success, else print message and exit.
inline void checkErr(cl_int ret, const char *msg) {
  if (ret != CL_SUCCESS) {
    std::cerr << "Error[" << ret << "]: " << msg << std::endl;
    exit(EXIT_FAILURE);
  }
}

// Lookup device property info.
template<typename T>
void queryDeviceProperty(const cl_device_id device,
                         const cl_device_info name,
                         const size_t length,
                         T *const value) {
  cl_uint ret = clGetDeviceInfo(device, name, length, value, NULL);
  checkErr(ret, "Failed to query device property");
}

// Get and return device info.
template<typename T>
T getDeviceInfo(const cl_device_id device,
                const cl_device_info name) {
  // Get the property value.
  T value;
  queryDeviceProperty(device, name, sizeof(T), &value);
  return value;
}

// Get and return device info (specialised for strings).
template<>
char *getDeviceInfo<char *>(const cl_device_id device,
                            const cl_device_info name) {
  // Get the property length.
  size_t length = 0;
  cl_uint ret = clGetDeviceInfo(device, name, 0, NULL, &length);
  checkErr(ret, "Failed to query device property length");

  // Allocate memory for property value.
  char *const buff = new char[length];
  queryDeviceProperty(device, name, length, buff);
  return buff;
}

// Print a name/value pair.
#define PRINT_DEVICE_INFO(name, value)                          \
  std::cout << "      " << name << " : " << value << std::endl;

// Print device info.
template<typename T>
void printDeviceInfo(const char *msg,
                     const cl_device_id device,
                     const cl_device_info name) {
  T info = getDeviceInfo<T>(device, name);
  PRINT_DEVICE_INFO(msg, info);
}

// Print device info (specialised for strings).
template<>
void printDeviceInfo<char *>(const char *msg,
                             const cl_device_id device,
                             const cl_device_info name) {
  char *const info = getDeviceInfo<char *>(device, name);
  PRINT_DEVICE_INFO(msg, info);
  delete[] info;
}

#undef PRINT_DEVICE_INFO

// The opencl_test example displays the opencl platforms and devices found
// on the system using the opencl api directly. if this test fails to compile
// and/or run, there is a problem with the opencl implementation found on the
// system.
int main()
{
  cl_int ret;
  cl_uint num_platforms = 0;

  // Query number of opencl platforms
  ret = clGetPlatformIDs(0, NULL, &num_platforms);
  checkErr(ret, "Failed to query platforms");

  // Check that at least one platform was found
  if (num_platforms == 0) {
    std::cerr << "Found 0 OpenCL platforms" << std::endl;
    return 0;
  }

  // Get platform IDs
  cl_platform_id *const platforms = new cl_platform_id[num_platforms];
  clGetPlatformIDs(num_platforms, platforms, NULL);

  // Iterate through each platform and query its devices
  for(cl_uint i = 0; i < num_platforms; i++) {
    const cl_platform_id platform = platforms[i];

    // Print number of devices found
    std::cout << "Platform " << i << ":" << std::endl << std::endl;

    // Query number of opencl devices
    cl_uint num_devices = 0;
    ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    checkErr(ret, "Failed to lookup devices for platform");

    // Get device ids for the platform
    cl_device_id *const devices = new cl_device_id[num_devices];
    ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    checkErr(ret, "Failed to query platform devices");

    // Iterate through each device on the platform and print info.
    for(cl_uint j = 0; j < num_devices; j++) {
      const cl_device_id device = devices[j];

      char *const name = getDeviceInfo<char *>(device, CL_DEVICE_NAME);
      std::cout << "  Device " << j << ": " << name << std::endl;
      delete[] name;

      // Print device properties.
      printDeviceInfo<char *> ("Device Version      ", device, CL_DEVICE_VERSION);
      printDeviceInfo<char *> ("OpenCL C Version    ", device, CL_DEVICE_OPENCL_C_VERSION);
      printDeviceInfo<cl_uint>("Compute Units       ", device, CL_DEVICE_MAX_COMPUTE_UNITS);
      printDeviceInfo<cl_uint>("Clock Frequency     ", device, CL_DEVICE_MAX_CLOCK_FREQUENCY);
      printDeviceInfo<size_t> ("Global Memory Size  ", device, CL_DEVICE_GLOBAL_MEM_SIZE);
      printDeviceInfo<size_t> ("Local Memory Size   ", device, CL_DEVICE_LOCAL_MEM_SIZE);
      printDeviceInfo<size_t> ("Max Work Group Size ", device, CL_DEVICE_MAX_WORK_GROUP_SIZE);
      printDeviceInfo<cl_uint>("Work Item Dimensions", device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);

      // Work work item sizes.
      cl_uint dimen = getDeviceInfo<cl_uint>(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
      size_t *const sizes = new size_t[dimen];
      cl_uint ret = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, dimen, sizes, NULL);
      std::cout << "      " << "Max Work Item Sizes  : [";
      for (cl_uint d = 0; d < dimen; d++) {
        if (d) std::cout << ", "; // Print separator.
        std::cout << sizes[d];
      }
      std::cout << "]" << std::endl;
      delete[] sizes;
    }

    delete[] devices;
  }

  delete[] platforms;

  return 0;
}
