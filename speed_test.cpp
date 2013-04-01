// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------

#define _HAS_ITERATOR_DEBUGGING 0

#include "skiplist.hpp"
#include <cassert>

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <chrono>

#include <vector>
#include <time.h>

#include <random>
#include <map>

using std::cout;
using std::endl;
using std::forward_list;

using std::vector;
using std::map;

void ClockStart();
void ClockEnd();

vector<int>* CreateCommonAccessPattern(int size)
{
	ClockStart();
	vector<int> *idx_vec = new vector<int>(size*3);
	
	std::random_device rd;
	std::uniform_int_distribution<> uniform_dist(0,size-1);


	for(int i(0);i < size*3;++i)
		idx_vec->push_back(uniform_dist(rd));

	std::cout << "Create Random Accessing array ";
	ClockEnd();

	return idx_vec;
}

typedef std::chrono::high_resolution_clock my_clock;
my_clock::time_point t;

void ClockStart()
{	
	t = my_clock::now();
}
void ClockEnd()
{ 
	std::chrono::milliseconds ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(my_clock::now() - t);
	cout << ms_duration.count()
	<< " ms\n";
}


template<typename KEY = int, typename VALUE = int, intmax_t MLEVEL = 10, intmax_t TRIALS = 10000>
class AccessExperiment
{
	public:
		
		forward_list<VALUE> 				fwd_keys;
		skip_list<KEY,VALUE,MLEVEL> skiplist_keys;
		map<KEY,VALUE>							map_keys;

		vector<int> *access_idx;

	int size;
	explicit
	AccessExperiment(int _size = 10000)
		: access_idx(CreateCommonAccessPattern(_size)), size(_size)
	{
		ClockStart();

		for(int i(size - 1);i > 0;--i)
			skiplist_keys.insert(i,i);
		std::cout << "Inserting keys into SkipList ";
		ClockEnd();
		ClockStart();

		for(int i(size - 1);i > 0;--i)
			fwd_keys.push_front(i);
		std::cout << "Inserting keys into forward_list ";
		ClockEnd();
		ClockStart();
		for(int i(size - 1);i > 0;--i)
			map_keys.insert(std::make_pair(i,i));

		std::cout << "Inserting keys into map ";
		ClockEnd();
	}

	virtual ~AccessExperiment() { delete access_idx; }


	void clock_access_fl()
	{
		unsigned int j = 0;
		ClockStart();
		do
		{
			forward_list<int>::iterator it;
			for(it = fwd_keys.begin();it != fwd_keys.end();++it)
			{
				if( *it == (*access_idx)[j] )
					break;
			}
			
		} while (++j < access_idx->size());
		cout << "  forward_list (" << size << "): ";
		ClockEnd();
	}
	void clock_access_sl()
	{
		unsigned int j = 0;
		ClockStart();

		do
		{
			skip_list<int,int>::iterator it;
			it = skiplist_keys.search((*access_idx)[j]);
		} while (++j < access_idx->size());
		cout << "  skip_list (" << size << "): ";
		ClockEnd();
	}
	void clock_access_map()
	{
		unsigned int j = 0;
		ClockStart();

		do
		{
			map<int,int>::iterator it;
			it = map_keys.find((*access_idx)[j]);
		} while (++j < access_idx->size());
		cout << "  map (" << size << "): ";
		ClockEnd();
	}

};



int main()
{

	AccessExperiment<> ex;
	std::cout << "Clocking random access times:\n";
	ex.clock_access_fl();
	ex.clock_access_sl();
	ex.clock_access_map();
}
