/*
 * MemPool_st.cpp
 *
 *  Created on: Nov 27, 2016
 *      Author: noam
 */




//
// MemoryPool_st.cpp
//
// $Id: //poco/1.4/Foundation/src/MemoryPool_st.cpp#1 $
//
// Library: Foundation
// Package: Core
// Module:  MemoryPool_st
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "MemoryPool_st.h"
#include "Poco/Exception.h"



MemoryPool_st::MemoryPool_st(std::size_t blockSize, int preAlloc, int maxAlloc):
	_blockSize(blockSize),
	_maxAlloc(maxAlloc),
	_allocated(preAlloc)
{
	poco_assert (maxAlloc == 0 || maxAlloc >= preAlloc);
	poco_assert (preAlloc >= 0 && maxAlloc >= 0);

	int r = BLOCK_RESERVE;
	if (preAlloc > r)
		r = preAlloc;
	if (maxAlloc > 0 && maxAlloc < r)
		r = maxAlloc;
	_blocks.reserve(r);
	for (int i = 0; i < preAlloc; ++i)
	{
		_blocks.push_back(new char[_blockSize]);
	}
}


MemoryPool_st::~MemoryPool_st()
{
	for (BlockVec::iterator it = _blocks.begin(); it != _blocks.end(); ++it)
	{
		delete [] *it;
	}
}


void* MemoryPool_st::get()
{
	//FastMutex::ScopedLock lock(_mutex);

	if (_blocks.empty())
	{
		if (_maxAlloc == 0 || _allocated < _maxAlloc)
		{
			++_allocated;
			return new char[_blockSize];
		}
		else throw Poco::OutOfMemoryException("MemoryPool_st exhausted");
	}
	else
	{
		char* ptr = _blocks.back();
		_blocks.pop_back();
		return ptr;
	}
}


void MemoryPool_st::release(void* ptr)
{
	//FastMutex::ScopedLock lock(_mutex);

	_blocks.push_back(reinterpret_cast<char*>(ptr));
}

