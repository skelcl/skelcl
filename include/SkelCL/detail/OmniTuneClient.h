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

namespace omnitune {

void init();

namespace stencil {

void getLocalSize(cl_uint *const local);

}  // namespace stenicl

}  // namespace omnitune

#endif  /* OMNITUNE_CLIENT_H_ */
