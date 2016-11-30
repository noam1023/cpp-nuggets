/*
 * stl_alloc_example.cpp
 * Sample code how to use std::map with a custom allocator
 *
 * NOTE: the size of the allocator block is about 40 bytes + sizeof(key) + sizeof(value)
 * These 40 bytes are the node and pair<k,v> data.
 * The type supplied to the StalAlloc<Type, num> is ignored .
 *
 * tested on g++ 5.4.0 on ubuntu 16.04
 *
 *  Created on: Nov 30, 2016
 *      Author: noam
 *
 *
 *      c++ stl_alloc_example.cpp MemoryPool_st.cpp -lPocoFoundation -o t --std=c++11
 */



#include <map>
#include <iostream>
#include "StlAlloc.h"

using namespace std;

class Value{
public:
	Value(){a=0;} // the map needs default ctor
	Value(int i){a=i;}
	int a;
	char buff[5000]; // make it big
};

std::ostream& operator << ( std::ostream& os, const Value& rhs ) {
    os << "V(" << rhs.a <<")";
    return os;
}


class Key{
public:
	Key(int i){a=i; b = 0;}
	int a;
	int b;
	char buff[3000];
	bool operator<(const Key& other)const{
		return (this->a < other.a);
	}
};

std::ostream& operator << ( std::ostream& os, const Key& rhs ) {
    os << "K(" << rhs.a <<"," << rhs.b<< ")";
    return os;
}

void basic_types(){
	const int slots = 30;
	std::map<char, int, std::less<char>, StlAlloc<int, slots> > aMap;

	aMap['a'] = 30;
	aMap['b'] = 40;
	aMap['c'] = 50;
	aMap.erase('3'); // not an error
	aMap.erase('a');
	for(auto& kv: aMap){
		cout << kv.first << ":" << kv.second << "; ";
	}
	cout << endl;
}
class ThisTypeIsNotUsed{};
void custom_value(){
	const int slots = 30;
	std::map<int, Value, std::less<int>, StlAlloc<ThisTypeIsNotUsed, slots> > aMap;

	aMap[3] = Value(30);
	aMap[4] = Value(40);
	aMap.erase(3);
	aMap[3] = Value(666);

	for(auto& kv: aMap){
		cout << kv.first << ":" << kv.second << "; ";
	}
	cout << endl;
}

void custom_key(){
	const int slots = 30;
	std::map<Key, int, std::less<Key>, StlAlloc<ThisTypeIsNotUsed, slots> > aMap;
	typedef std::map<Key, int >::iterator It;

	Key k3 = Key(3);
	Key k4(4); // different form of ctor. Less copies
	pair<It,bool> result = aMap.insert( make_pair( k3, 44 ));
	if(!result.second)
		cout << "insert failed";
	aMap[k3] = 30; // override value 44 with value 30
	aMap[k4] = 40;
	aMap[55] = 550;
	int n = aMap.erase(Key(4));
	cout << "erased "<< n << " elements"<< endl;
	for(auto& kv: aMap){
		cout << kv.first << ":" << kv.second << "; ";
	}
	cout << endl;
}


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

int main(){
	basic_types();
	custom_value();
	custom_key();
	custom_key_value();
	return 0;
}
