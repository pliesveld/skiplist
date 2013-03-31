#include "skiplist.hpp"
#include <cassert>


void test_singlenode()
{
//	SkipNode<int,int> n;
}

void test_list_one_level()
{
	skip_list<int,int> l;

	int k = 3;
	int v = 7;

	l.insert(k,v);
	
	skip_list<int,int>::iterator it;

	assert( (it = l.search(k)) != l.end() && *it == v);
}

void test_list_one_level_replace()
{
	skip_list<int,int> l;

	int k = 3;
	int v = 7;

	l.insert(k,v);
	v = 10;
	l.insert(k,v);

	skip_list<int,int>::iterator it;
	assert( (it = l.search(k)) != l.end() && *it == v);
}


void test_list_two_nodes()
{
	skip_list<int,int> l;
	l.insert(2,2);
	l.insert(3,3);

	skip_list<int,int>::iterator it;

	assert( (it = l.search(2)) != l.end() && *it == 2);
	assert( (it = l.search(3)) != l.end() && *it == 3);


	skip_list<int,int> l2;
	l2.insert(3,3);
	l2.insert(2,2);

	assert( (it = l2.search(2)) != l2.end() && *it == 2);
	assert( (it = l2.search(3)) != l2.end() && *it == 3);

}

void test_list_two_nodes2()
{
	skip_list<int,int> l;

	int k[2] {1,2};
	int v[2] {3,4};

	for(int i(0);i < sizeof(k)/sizeof(int);++i)
		l.insert(k[i],v[i]);


	skip_list<int,int>::iterator it;
	for(int i(0);i < sizeof(k)/sizeof(int);++i)
		assert( (it = l.search(k[i])) != l.end() && *it == v[i]);

}


void test_list_ten_nodes()
{
	skip_list<int,int> l;

	for(int i(0); i < 10;++i)
		l.insert( i, i );


	skip_list<int,int>::iterator it;
	for(int i(0); i < 10;++i)
		assert( (it = l.search(i)) != l.end() && *it == i);
}


void test_delete()
{
	skip_list<int,int> l;
	skip_list<int,int> l2;
	skip_list<int,int> l3;

	for(int i(0); i < 10;++i)
	{
		l.insert( i, i );
		l2.insert( i, i );
		l3.insert( i, i );
	}


	skip_list<int,int>::iterator it;

	assert(l.erase(0) == 1);
	assert(l2.erase(9) == 1);
	assert(l3.erase(5) == 1);



	for(int i(1); i < 10;++i)
		assert( (it = l.search(i)) != l.end() && *it == i);

	for(int i(0); i < 9;++i)
		assert( (it = l2.search(i)) != l2.end() && *it == i);

	for(int i(0); i < 10;++i)
		if( i != 5)
			assert( (it = l3.search(i)) != l3.end() && *it == i);


	assert( l.search(0) == l.end());
	assert( l2.search(9) == l.end());
	assert( l3.search(5) == l.end());

}



void test_iterator_interface()
{

	skip_list<int,int> l;

	skip_list<int,int>::iterator it;

	it = l.begin();

	assert(l.begin() == l.end());
	assert(it == l.end());
	

	for(int i = 1;i <= 3;++i)
		l.insert(i,3*i);

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
