#include "Simulation.h"
#include "pvsutil/Logger.h"
#include "SkelCL/SkelCL.h"
#include "SkelCL/detail/DeviceProperties.h"

#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	skelcl::init(skelcl::nDevices(1).deviceType(skelcl::device_type::ANY));
	std::ofstream of("log.log");
	pvsutil::defaultLogger.setLoggingLevel(pvsutil::Logger::Severity::Info);
	pvsutil::defaultLogger.setOutput(of);
	Simulation sim(2048);
	sim.initialize();
	sim.run();
}
