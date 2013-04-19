#include "skiplist.hpp"
#include <cassert>
#include <cstdio>
using std::printf;


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

	for(size_t i(0);i < sizeof(k)/sizeof(int);++i)
		l.insert(k[i],v[i]);


	skip_list<int,int>::iterator it;
	for(size_t i(0);i < sizeof(k)/sizeof(int);++i)
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


void test_const_iterator_interface()
{
	skip_list<int,int> l;

	skip_list<int,int>::const_iterator it;

	l.insert(3,3);
	l.insert(4,4);
	l.insert(5,5);

	int i = 3;
	for(it = l.begin();it != l.end();++it)
	{
		assert(*it == i++);
	}
}

void test_copy_constructor()
{
	skip_list<int,int> l;
	skip_list<int,int>::const_iterator it;

	l.insert(3,3);
	l.insert(4,4);
	l.insert(5,5);

	skip_list<int,int> l2(l);
	int i = 3;
	for(it = l2.begin();it != l2.end();++it)
	{
		assert(*it == i++);
	}

}


void test_assignment_op()
{
	skip_list<int,int> l;
	skip_list<int,int> l2;


	skip_list<int,int>::const_iterator it;

	l.insert(3,3);
	l.insert(4,4);
	l.insert(5,5);

	l2 = l;
	int i = 3;
		for(it = l2.begin();it != l2.end();++it)
	{
		assert(*it == i++);
	}

	assert(l.empty());

}


enum Cntr
{
	CTOR = 0,
	DTOR = 1,
	LVALUE_CTOR = 2,
	RVALUE_CTOR = 3,
	LVALUE_ASSIGN = 4,
	RVALUE_ASSIGN = 5
};

char const *Cntr_str[] =
{
	"Constructor",
	"Destructor",
	"LValue-Ctor",
	"RValue-Ctor",
	"LValue-assignment",
	"RValue-assignment"
};

static int cnt[6];

void test_custom_types()
{
	for(int i = 0;i < 6;++i)
		cnt[i] = 0;

	struct MyData
	{
		MyData(int _i) : c(_i) 
		{ 
			++cnt[CTOR]; 
		}

		MyData(MyData const &rhs)
			: c(rhs.c), w(rhs.w), data(nullptr)
		{
			++cnt[LVALUE_CTOR];
			if(rhs.data)
			{
				data = new int(*rhs.data);
			}
		}

		MyData(MyData &&rhs)
			: c(rhs.c)
		{
			++cnt[RVALUE_CTOR];
			this->data = std::move(rhs.data);
			rhs.data = 0;
		}

		MyData& operator=(MyData const &rhs)
		{
			++cnt[LVALUE_ASSIGN];
			if(this == &rhs)
				return *this;

			if(rhs.data)
			{
				data = new int(*rhs.data);
			}
			c = rhs.c;
			return *this;
		}

		MyData& operator=(MyData &&rhs)
		{
			++cnt[RVALUE_ASSIGN];
			data = rhs.data;
			rhs.data = 0;
			c = rhs.c;
			return *this;
		}

		virtual ~MyData() 
		{
			++cnt[DTOR];
			if(data)
				delete data;
		}

		char a[3];
		int c;
		double w;
		int *data = nullptr;
	};

	{
		skip_list<int,MyData> l;
		l.insert(0,MyData(0));
		l.insert(1,MyData(1));
		l.insert(2,MyData(2));

		assert(cnt[CTOR] == 3);
		for(int i(0);i < 3;++i)
		{
			auto it = l.search(i);
			assert(it != l.end());
			assert(it->c == i);
		}


		l.insert(1,MyData(10));
		auto it = l.search(1);
		assert(it != l.end());
		assert(it->c == 10);

		MyData a_copy(11);
		l.insert(2,a_copy);
		a_copy.c = 9;
		l.insert(0,a_copy);

		for(int i(0);i < 3;++i)
		{
			auto it = l.search(i);
			assert(it != l.end());
			assert(it->c == i+9);
		}

		
		l.insert(4,a_copy);

	}

	printf("MyData Counters:\n");
	for(int i = 0;i < 6;++i)
		printf("\t%10s\t%d\n",Cntr_str[i],cnt[i]);
	
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
	test_const_iterator_interface();
	test_assignment_op();
	test_custom_types();
	return 0;
}
