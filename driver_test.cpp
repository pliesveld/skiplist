#include "skiplist.hpp"
#include <cassert>


bool test_singlenode()
{
//	SkipNode<int,int> n;
}

bool test_list_one_level()
{
	SkipList<int,int> l;

	int k = 3;
	int v = 7;

	l.Insert(k,v);

	assert( l.search(k) == v);
}

bool test_list_one_level_replace()
{
	SkipList<int,int> l;

	int k = 3;
	int v = 7;

	l.Insert(k,v);
	v = 10;
	l.Insert(k,v);

	assert( l.search(k) == v);
}


void test_list_two_nodes()
{
	SkipList<int,int> l;
	l.Insert(2,2);
	l.Insert(3,3);

	assert( l.search(2) == 2);
	assert( l.search(3) == 3);


	SkipList<int,int> l2;
	l2.Insert(3,3);
	l2.Insert(2,2);
	assert( l2.search(2) == 2);
	assert( l2.search(3) == 3);

}

void test_list_two_nodes2()
{
	SkipList<int,int> l;

	int k[2] {1,2};
	int v[2] {3,4};

	for(int i(0);i < sizeof(k)/sizeof(int);++i)
		l.Insert(k[i],v[i]);

	for(int i(0);i < sizeof(k)/sizeof(int);++i)
		assert( l.search(k[i]) == v[i] );

}


void test_list_ten_nodes()
{
	SkipList<int,int> l;

	for(int i(0); i < 10;++i)
		l.Insert( i, i );

	for(int i(0); i < 10;++i)
		assert( l.search(i) == i);
}


void test_delete()
{
	SkipList<int,int> l;
	SkipList<int,int> l2;
	SkipList<int,int> l3;

	for(int i(0); i < 10;++i)
	{
		l.Insert( i, i );
		l2.Insert( i, i );
		l3.Insert( i, i );
	}

	assert(l.Delete(0) == 1);
	assert(l2.Delete(9) == 1);
	assert(l3.Delete(5) == 1);

	for(int i(1); i < 10;++i)
		assert( l.search(i) == i);

	for(int i(0); i < 9;++i)
		assert( l2.search(i) == i);

	for(int i(0); i < 10;++i)
		if( i != 5)
			assert( l3.search(i) == i);
	try {
		l.search(0);
	}
	catch(...)
	{
		assert(true);
	}

	try {
		l2.search(9);
	}
	catch(...)
	{
		assert(true);
	}
	try {
		l3.search(5);
	}
	catch(...)
	{
		assert(true);
		return;
	}
	throw 4;
}



void test_iterator_interface()
{

	SkipList<int,int> l;

	SkipList<int,int>::iterator it;

	it = l.begin();

	assert(l.begin() == l.end());
	assert(it == l.end());
	

	for(int i = 1;i <= 3;++i)
		l.Insert(i,3*i);

	int cnt(0);
	for(it = l.begin();it != l.end();++it)
		cnt += *it;

	assert(cnt == 3 + 6 + 9);
}

int main()
{
	test_list_one_level();
	test_list_one_level_replace();

	test_list_two_nodes();
	test_list_two_nodes2();

	test_list_ten_nodes();

	test_delete();
	test_iterator_interface();
	return 0;
}
