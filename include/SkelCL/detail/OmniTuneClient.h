///
/// \file OmniTuneClient.h
///
///
/// \author Chris Cummins <chrisc.101@gmail.com>
///
#ifndef OMNITUNE_CLIENT_H_
#define OMNITUNE_CLIENT_H_

// GLib DBus C++ interface.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include <CL/cl.h>
#include <glibmm.h>
#include <giomm.h>
#include <giomm/dbusproxy.h>
#pragma GCC diagnostic pop

#include "SkelCL/detail/Device.h"

#define OMNILOG(...) LOG_INFO("OMNITUNE: ", __VA_ARGS__)

namespace omnitune {

void init();

namespace stencil {

void getLocalSize(const cl::Kernel &kernel,
                  const cl::Device &device,
                  const std::string &source,
                  cl_uint *const local);

}  // namespace stenicl

}  // namespace omnitune

#endif  /* OMNITUNE_CLIENT_H_ */
