/*
 * MemPool_st.h
 *
 *  Created on: Nov 27, 2016
 *      Author: noam
 */

#ifndef SRC_COMMON_MEMPOOL_ST_H_
#define SRC_COMMON_MEMPOOL_ST_H_


//
// Library: Foundation
// Package: Core
// Module:  MemoryPool
//
// Definition of the MemoryPool class.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//



#include "Poco/Foundation.h"
//#include "Poco/Mutex.h"
#include <vector>
#include <cstddef>


class  MemoryPool_st
	/// A simple pool for fixed-size memory blocks.
	///
	/// The main purpose of this class is to speed-up
	/// memory allocations, as well as to reduce memory
	/// fragmentation in situations where the same blocks
	/// are allocated all over again, such as in server
	/// applications.
	///
	/// All allocated blocks are retained for future use.
	/// A limit on the number of blocks can be specified.
	/// Blocks can be preallocated.
	///
	/// This version has no locks.
	/// It is designed for SINGLE THREAD ACCESS ONLY

{
public:
	MemoryPool_st(std::size_t blockSize, int preAlloc = 0, int maxAlloc = 0);
		/// Creates a MemoryPool_st for blocks with the given blockSize.
		/// The number of blocks given in preAlloc are preallocated.

	~MemoryPool_st();

	void* get();
		/// Returns a memory block. If there are no more blocks
		/// in the pool, a new block will be allocated.
		///
		/// If maxAlloc blocks are already allocated, an
		/// OutOfMemoryException is thrown.

	void release(void* ptr);
		/// Releases a memory block and returns it to the pool.

	std::size_t blockSize() const;
		/// Returns the block size.

	int allocated() const;
		/// Returns the number of allocated blocks.

	int available() const;
		/// Returns the number of available blocks in the pool.

private:
	MemoryPool_st();
	MemoryPool_st(const MemoryPool_st&);
	MemoryPool_st& operator = (const MemoryPool_st&);

	enum
	{
		BLOCK_RESERVE = 128
	};

	typedef std::vector<char*> BlockVec;

	std::size_t _blockSize;
	int         _maxAlloc;
	int         _allocated;
	BlockVec    _blocks;
	//FastMutex   _mutex;
};


//
// inlines
//
inline std::size_t MemoryPool_st::blockSize() const
{
	return _blockSize;
}


inline int MemoryPool_st::allocated() const
{
	return _allocated;
}


inline int MemoryPool_st::available() const
{
	return (int) _blocks.size();
}



#endif /* SRC_COMMON_MEMPOOL_ST_H_ */
