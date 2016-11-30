/*
 * StlAlloc.h
 *
 *  Created on: Nov 30, 2016
 *      Author: noam
 */


#ifndef _STL_ALLOC_POCO_H
#define _STL_ALLOC_POCO_H

#include <sstream>
#include <exception>
//#include <Poco/MemoryPool.h>
#include "MemoryPool_st.h"


/// Add a description to the exception object.
/// This class is useful iff the caller has meaningful message at the point of throwing the exception.
class bad_alloc_descr : public std::exception
{
  public:
    bad_alloc_descr(const char* msg, const char* fname, int lineNum) throw()
    {
      _name =  msg;
      _filename = fname;
      _lineNum = lineNum;
    }

     bad_alloc_descr(const char* msg) throw()
    {
      _name =  msg;
      _lineNum = 0;
    }

    virtual ~bad_alloc_descr() throw(){}

    /// \return the exception reason.
    /// the pointer is valid as long as the exception object is in scope.
    virtual const char* what() const throw()
    {
      std::stringstream t;
      t << _name << " " << _filename << " " << _lineNum <<".";
      return t.str().c_str();
    }

    int line() const { return _lineNum;}
    const std::string name() const{ return _name;}
    const std::string filename() const { return _filename; }

  private:
    std::string _name;
    std::string _filename;
    int _lineNum;
};


/** \brief custom allocator for STL containers.
 *
 * This allocator uses preallocated memory in class Poco::MemoryPool (MT safe)
 *    or MemoryPool_st (MT UNSAFE)
 *
 * \code
 * // a map with this allocator
 * typedef  StlAlloc< std::pair<MyKey,RuntimeEvent *>, MAX_ALARMS, 1000  >  MapAlloc_t;
 *
 * // allocator of 500 int32_t
 * typedef  StlAlloc< int32_t, 500  >  IntAlloc_t;
 * std::set<int, std::less<int>, IntAlloc_t> mySet;
 * mySet.insert(44);
 *
 * \endcode
 *
 * A more complete example:
 * \code
 *
  class Key{
  public:
	Key(int i){a=i;}
	int a;
	bool operator<(const Key& other)const{
		return (this->a < other.a);
	}
};
  class ThisTypeIsNotUsed{};

  void custom_key_value(){
	const int slots = 30;
	std::map<Key, Value, std::less<Key>, StlAlloc<ThisTypeIsNotUsed, slots> > aMap;

	Key k4(4);
	aMap[k4] = 40;
	aMap[55] = 550;

	for(auto& kv: aMap){
		cout << kv.first << ":" << kv.second << "; ";
	}
	cout << endl;
}
\endcode
 * \warning the user MUST use try{}catch(bad_alloc& ex){}  !
 * \author Noam Cohen
 */
template <typename T,int numSlots>
class StlAlloc
{
public:
  StlAlloc() {
	  //_InternalAlloc = new Poco::MemoryPool(sizeof(T), numSlots, numSlots);
	  _InternalAlloc = new MemoryPool_st(sizeof(T), numSlots, numSlots);
  }

  ~StlAlloc(){
	  delete _InternalAlloc;
  }

   // Typedefs
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T         value_type;


    /// Allocate raw memory
    T* allocate( size_t n, const void* = NULL ) {
		int nBytes = n * sizeof(T);
		if(nBytes != sizeof(T)){
		  throw bad_alloc_descr(__func__, __FILE__, __LINE__);
		}
		T* p = (T*)_InternalAlloc->get();
		if( p == NULL ){
			throw bad_alloc_descr(__func__, __FILE__, __LINE__);
		}
        return p;
    }

    /// Free raw memory.
    void deallocate( void* p, size_t ) {
        // The standard states that p must not be NULL. However, some
        // STL implementations fail this requirement, so the check must
        // be made here.
        if( p == NULL )
            return;
        _InternalAlloc->release(reinterpret_cast<T*>(p));
    }

    template <typename U> struct rebind  {
	typedef StlAlloc<U , numSlots> other;
    };

    size_t max_size() const   {
        return 1;
    }

      // In-place construction
    void construct( T* p, const T& c ) {
        // placement new operator
        new( reinterpret_cast<void*>(p) ) T(c);
    }

    // In-place destruction
    void destroy( pointer p )  {
        // call destructor directly
        (p)->~T();
    }

private:
   //Poco::MemoryPool* _InternalAlloc;
   MemoryPool_st* _InternalAlloc;
};

#endif

