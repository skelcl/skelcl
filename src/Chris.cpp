#include "SkelCL/Chris.h"

#include <map>

#include <sys/time.h>

namespace chris {

//
// OpenCL compilation options. See:
//
//   https://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clBuildProgram.html
//
namespace ClFlags {
  // This option disables all optimizations. The default is
  // optimizations are enabled.
  const char *OPT_DISABLE = "-cl-opt-disable";
  // The following options control compiler behavior regarding
  // floating-point arithmetic. These options trade off between
  // performance and correctness and must be specifically
  // enabled. These options are not turned on by default since it
  // can result in incorrect output for programs which depend on
  // an exact implementation of IEEE 754 rules/specifications for
  // math functions.
  const char *MAD_ENABLE = "-cl-mad-enable";
  // Allow a * b + c to be replaced by a mad. The mad computes a *
  // b + c with reduced accuracy. For example, some OpenCL devices
  // implement mad as truncate the result of a * b before adding
  // it to c.
  const char *NO_SIGNED_ZEROS = "-cl-no-signed-zeros";
  // Allow optimizations for floating-point arithmetic that ignore
  // the signedness of zero. IEEE 754 arithmetic specifies the
  // distinct behavior of +0.0 and -0.0 values, which then
  // prohibits simplification of expressions such as x+0.0 or
  // 0.0*x (even with -clfinite-math only). This option implies
  // that the sign of a zero result isn't significant.
  const char *UNSAFE_MATH_OPTIMIZATIONS = "-cl-unsafe-math-optimizations";
  // Allow optimizations for floating-point arithmetic that (a)
  // assume that arguments and results are valid, (b) may violate
  // IEEE 754 standard and (c) may violate the OpenCL numerical
  // compliance requirements as defined in section 7.4 for
  // single-precision floating-point, section 9.3.9 for
  // double-precision floating-point, and edge case behavior in
  // section 7.5. This option includes the -cl-no-signed-zeros and
  // -cl-mad-enable options.
  const char *FINITE_MATH_ONLY = "-cl-finite-math-only";
  // Allow optimizations for floating-point arithmetic that assume that
  // arguments and results are not NaNs or ±∞. This option may violate
  // the OpenCL numerical compliance requirements defined in section 7.4
  // for single-precision floating-point, section 9.3.9 for
  // double-precision floating-point, and edge case behavior in section
  // 7.5.
  const char *FAST_RELAXED_MATH = "cl-fast-relaxed-math";
}  // ClFlags


void init() {
  LOG_INFO("Using Chris' modifications");
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
const char *get_cl_flags(skelcl::detail::Program *const program,
                         cl::Device device) {
  // TODO: Determine what flags to set programatically.
  return "";
}
#pragma GCC diagnostic pop //  -Wunused-parameter


///
/// TIMING
///
std::map<const char *, long> _timers;

long _walltime() {
  struct timeval time;
  gettimeofday(&time, NULL);

  return time.tv_sec * 1e3 + time.tv_usec * 1e-3;
}

void startTimer(const char *name) {
  LOG_INFO("Timer[", name , "] started");
  _timers[name] = _walltime();
}

int stopTimer(const char *name) {
  int elapsed = static_cast<int>(_walltime() - _timers[name]);
  LOG_INFO("Timer[", name , "] ", elapsed, " ms");
  return elapsed;
}

}  // chris
