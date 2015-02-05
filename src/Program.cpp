/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/
 
///
/// \file Program.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#undef  __CL_ENABLE_EXCEPTIONS

#include <stooling/SourceCode.h>

#include <pvsutil/Assert.h>
#include <pvsutil/Logger.h>

#include "SkelCL/Chris.h"
#include "SkelCL/detail/Program.h"

#include "SkelCL/detail/Device.h"
#include "SkelCL/detail/DeviceList.h"
#include "SkelCL/detail/Util.h"

namespace {

std::string binaryFilename(const std::string& hash,
                           const std::shared_ptr<skelcl::detail::Device>&
                                devicePtr)
{
  std::stringstream filename;
  // escape spaces in device name
  std::string devName = devicePtr->name();
  std::replace( devName.begin(), devName.end(), ' ', '_');
  filename << "." << hash
           << "-" << devName
           << "-" << devicePtr->id()
           << ".skelcl";
  return filename.str();
}

} // namespace

namespace skelcl {

namespace detail {

Program::Program(const std::string& source, const std::string& hash)
  : _source(source),
    _hash(hash),
    _clPrograms()
{
  LOG_DEBUG_INFO("Program instance created with source:\n", source,
                 "\n");
}

Program::Program(Program&& rhs)
  : _source(std::move(rhs._source)),
    _hash(std::move(rhs._hash)),
    _clPrograms(std::move(rhs._clPrograms))
{
}

Program& Program::operator=(Program&& rhs)
{
  _source      = std::move(rhs._source);
  _hash        = std::move(rhs._hash);
  _clPrograms  = std::move(rhs._clPrograms);
  return *this;
}

void Program::transferParameters(const std::string& from,
                                 unsigned           indexFrom,
                                 const std::string& to)
{
  _source.transferParameters(from, indexFrom, to);
  _source.fixKernelParameter(to);
}

void Program::transferArguments(const std::string& from,
                                unsigned           indexFrom,
                                const std::string& to)
{
  _source.transferArguments(from, indexFrom, to);
}

void Program::renameFunction(const std::string& from,
                             const std::string& to)
{
  _source.renameFunction(from, to);
}

void Program::renameType(const int i, const std::string& typeName)
{
  std::stringstream identifier;
  identifier << "SCL_TYPE_" << i;
  
  _source.redefineTypedef(identifier.str(), typeName);
}

bool Program::loadBinary()
{
  // if hash is empty no binary is loaded (maybe be more gentle and just return)
  ASSERT(!_hash.empty());

  for (auto& devicePtr : globalDeviceList) {
    std::ifstream binaryFile(binaryFilename(_hash, devicePtr),
                               std::ios_base::in
                             | std::ios_base::binary
                             | std::ios_base::ate);
    if (binaryFile.fail()) {
      _clPrograms.clear();
      return false;
    }

    // get the size of the file
		std::ifstream::pos_type size = binaryFile.tellg();
		// allocate memory
    std::unique_ptr<char[]> binary(new char[size]);
		// set position in file to the beginning
		binaryFile.seekg(0, std::ios::beg);
		// read the hole file
		binaryFile.read(binary.get(), size);
		// close it
		binaryFile.close();
		// push the binary on the vector
    cl::Program::Binaries binaries(1, std::make_pair(binary.get(), size));
    std::vector<cl::Device> devices{ devicePtr->clDevice() };
    _clPrograms.push_back( cl::Program( devicePtr->clContext(),
                                        devices, binaries ) );

    LOG_DEBUG_INFO("Load binary for device ", devicePtr->id(),
                   " from file ", binaryFilename(_hash, devicePtr));
  }
  ASSERT(_clPrograms.size() == globalDeviceList.size());
  return true;
}

void Program::build()
{
  bool createdProgramsFromSource = false;
  if (_clPrograms.empty()) {
    createProgramsFromSource();
    createdProgramsFromSource = true;
  }

  try {
    // build program for each device
    // TODO: how to build the program only for a subset of devices?
    for (auto& devicePtr : globalDeviceList) {
      // Select optimisation flags
      const char *flags = chris::get_cl_flags(this, devicePtr->clDevice());
      LOG_DEBUG_INFO("Compiling program for device ", devicePtr->id(),
                     " with flags: \"", flags, "\"");

      chris::startTimer("Program::build");
      _clPrograms[devicePtr->id()].build(
            std::vector<cl::Device>(1, devicePtr->clDevice()), flags );
      chris::stopTimer("Program::build");
    }

    if (createdProgramsFromSource) {
      saveBinary();
    }

  } catch (cl::Error& err) {
    if (err.err() == CL_BUILD_PROGRAM_FAILURE) {
      auto& devicePtr = globalDeviceList.front();
      auto  buildLog =
        _clPrograms[
          devicePtr->id()].getBuildInfo<CL_PROGRAM_BUILD_LOG>(
                                           devicePtr->clDevice() );
      LOG_ERROR(err, "\nBuild log:\n", buildLog);
      abort();
    } else {
      ABORT_WITH_ERROR(err);
    }
  }
}

cl::Kernel Program::kernel(const Device& device,
                           const std::string& name) const
{
  return cl::Kernel(_clPrograms[device.id()], name.c_str());
}

void Program::createProgramsFromSource()
{
  // insert programs into _clPrograms
  std::transform( globalDeviceList.begin(), globalDeviceList.end(),
                  std::back_inserter(_clPrograms),
      [this](DeviceList::const_reference devicePtr) -> cl::Program {
        std::stringstream ss;
        ss << "#define skelcl_get_device_id() " << devicePtr->id() << "\n";

        std::string s(ss.str());
        s.append(_source.code());

        LOG_DEBUG_INFO("Create cl::Program for device ", devicePtr->id(),
                       " with source:\n", s, "\n");

        return cl::Program(devicePtr->clContext(),
                           cl::Program::Sources(1, std::make_pair(s.c_str(),
                                                                  s.length()))
                          );
      });

}

void Program::saveBinary()
{
  if (_hash.empty()) return;
  // don't save binary, expect the user has explicitly requested so
  if (util::envVarValue("SKELCL_SAVE_BINARY") != "YES") return;

  for (auto& devicePtr : globalDeviceList) {
    try {
      auto size   = _clPrograms[
                      devicePtr->id()].getInfo<CL_PROGRAM_BINARY_SIZES>();
      ASSERT(size.size() == 1);

      std::unique_ptr<char[]> charPtr(new char[size.front()]);

      std::vector<char *> binary{ charPtr.get() };

      _clPrograms[devicePtr->id()].getInfo(CL_PROGRAM_BINARIES, &binary);

      std::ofstream outfile(binaryFilename(_hash, devicePtr),
                              std::ios_base::out
                            | std::ios_base::trunc
                            | std::ios_base::binary);
      outfile.write(binary.front(), static_cast<long>(size.front()));
      LOG_DEBUG_INFO("Saved binary for device ", devicePtr->id(),
                     " to file ", binaryFilename(_hash, devicePtr));
    } catch (cl::Error err) {
      ABORT_WITH_ERROR(err);
    }
  }
}

} // namespace detail

} // namespace skelcl
