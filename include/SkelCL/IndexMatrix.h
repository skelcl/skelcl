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
///  IndexMatrix.h
///
///	\author Michel Steuwer <michel.steuwer@uni-muenster.de>
///
#ifndef INDEX_MATRIX_H_
#define INDEX_MATRIX_H_

#include <vector>

#include "Index.h"
#include "Matrix.h"
#include "Vector.h"

#include "detail/DeviceList.h"

namespace skelcl {

///
/// \brief The IndexMatrix class is a special implementation of a Matrix with
///        IndexPoints as Elements. The implementation guaranties that no data
///        is transfered when using this version of the Matrix. OpenCL
///        functionality (global ids) is used to make the IndexPoints available
///        on the device. This implementation requires, that the Matrix is const
///        and can, therefore, not be modified after creation.
///
typedef const Matrix<IndexPoint> IndexMatrix;

template<>
class Matrix<IndexPoint> {
public:
	typedef IndexPoint value_type;
	typedef typename skelcl::MatrixSize size_type;
	typedef std::vector<IndexPoint> host_buffer_type;
	typedef typename host_buffer_type::const_iterator const_iterator;
	typedef typename host_buffer_type::iterator iterator;

	  struct coordinate {
	    typedef size_type::size_type index_type;

	    index_type rowIndex;
	    index_type columnIndex;
	  };

	Matrix() = delete;

	Matrix(const size_type size,
			const detail::Distribution<Matrix<IndexPoint>>& distribution =
					detail::Distribution<Matrix<IndexPoint>>());
	Matrix(const Matrix<IndexPoint>& rhs) = delete;
	Matrix(Matrix<IndexPoint> && rhs) = delete;
	Matrix<IndexPoint>& operator=(const Matrix<IndexPoint>&) = delete;
	Matrix<IndexPoint>& operator=(Matrix<IndexPoint> && rhs) = delete;
	~Matrix();

	IndexPoint operator()(int row, int col);

	// matrix interface

//	const_iterator begin() const;
//	const_iterator end() const;
	size_type size() const;
	detail::Sizes sizes() const;
	value_type operator[](size_type n) const;
	value_type at(size_type n) const;
	value_type front() const;
	value_type back() const;

	///
	/// \brief Returns a pointer to the current distribution of the vector.
	/// \return A pointer to the current distribution of the vector, of nullptr
	///         if no distribution is set
	///
	detail::Distribution<Matrix<IndexPoint>>& distribution() const;

	///
	/// \brief Changes the distribution of the vector
	///
	/// Changing the distribution might lead to data transfer between the host and
	/// the devices.
	///
	/// \param distribution The new distribution to be set. After this call
	///                     distribution is the new selected distribution of the
	///                     vector
	///
	template<typename U>
	void setDistribution(
			const detail::Distribution<Matrix<U>>& distribution) const;

	template<typename U>
	void setDistribution(const std::unique_ptr<detail::Distribution<Matrix<U>>>&
	newDistribution) const;

	void
	setDistribution(std::unique_ptr<detail::Distribution<Matrix<IndexPoint>>>&&
			newDistribution) const;

	static std::string deviceFunctions();

	//
			const detail::DeviceBuffer& deviceBuffer(const detail::Device& device) const;

			std::vector<IndexPoint>& hostBuffer() const;

			void dataOnDeviceModified() const;

			void dataOnHostModified() const;


		private:
	///
	/// \brief Formates information about the current instance into a string,
	///        used for Debug purposes
	///
	/// \return A formated string with information about the current instance
	///
			std::string getInfo() const;

	///
	/// \brief Formates even more information about the current instance into a
	///        string, as compared to getInfo, used for Debug purposes
	///
	/// \return A formated string with information about the current instance,
	///         contains all information from getInfo and more.
	///
			std::string getDebugInfo() const;

			value_type _maxIndex;
			mutable
			std::unique_ptr<detail::Distribution<Matrix<IndexPoint>>> _distribution;
		};

		} // namespace skelcl

#include "detail/IndexMatrixDef.h"

#endif // INDEX_MATRIX_H_
