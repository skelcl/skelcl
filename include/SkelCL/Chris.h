///
/// \file Chris.h
///
///	\author Chris Cummins <chrisc.101@gmail.com>
///
/// This file is used to help segregate Chris' work from the main
/// SkelCL codebase, and contains code revelant to his work on
/// autotuning SkelCL.

#ifndef CHRIS_H_
#define CHRIS_H_

#define CHRIS_OPENCL_FLAG "-cl-fast-relaxed-math"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <CL/cl.h>
#pragma GCC diagnostic pop  //  -Weffc++

#include <pvsutil/Logger.h>

#include "detail/Program.h"

namespace chris {

///
/// Initialise "stuff".
///
void init();

///
/// Return the number of partitions to use for a given inputBuffer and
/// work group size.
///
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <typename Tin>
cl_uint get_num_partitions(Tin &inputBuffer, const cl_uint workGroupSize) {
  // TODO: Determine how many partitions to set programatically.
  int partitions = 1;

  LOG_DEBUG_INFO("chris::get_num_partitions() --> ", partitions);
  return static_cast<cl_uint>(partitions);
}
#pragma GCC diagnostic pop //  -Wunused-parameter

///
/// Returns a string of OpenCL options to build a given program for a
/// given device with.
///
const char *get_cl_flags(skelcl::detail::Program *const program,
                         cl::Device device);

///
/// Start a timer with name "name".
///
void startTimer(const char *name);

///
/// Stop timer with name "name" and return elapsed time in milliseconds.
///
int stopTimer(const char *name);

} // namespace chris

#endif // CHRIS_H_
