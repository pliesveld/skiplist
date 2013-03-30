#ifndef __skiplist_hpp_
#define __skiplist_hpp_
#include <cstdint>
#include <vector>

#include "rand.hpp"

template<typename KEY,typename T>
class SkipNode;

template<typename KEY,typename T>
class SkipHead
{
protected:
public:
	SkipHead(int Level) : forward(Level,nullptr) { };
	typedef SkipNode<KEY,T> * node_type;
	std::vector<node_type> forward;
public:
};

template<typename KEY,typename T>
class SkipNode : public SkipHead<KEY,T>
{
public:
	typedef KEY key_type;
	typedef T mapped_type;
	SkipNode(const KEY &k, const T &v, int level) : SkipHead<KEY,T>(level), key(k), elem(v) { };
//private:
	typedef SkipNode<KEY,T> * node_type;
	KEY key;
	T elem;
public:
};

template<typename KEY,typename T, intmax_t MAXLEVEL = 10>
class SkipList
{
	typedef KEY key_type;
	typedef T mapped_type;
	typedef SkipNode<KEY,T> * node_ptr;
	typedef SkipNode<KEY,T> node_type;
public:
	// operator[]

	SkipList() : header(MAXLEVEL), level(0) { }

	T search(const KEY &key)
	{
		node_ptr x = (SkipNode<KEY,T>*) &header;
		for(int i = level - 1; i >= 0 && x; --i)
		{
			while(x && x->forward[i] 
					&& x->forward[i]->key < key)
				x = x->forward[i];
		}
		if(!x || !x->forward[0])
			throw 1;

		x = x->forward[0];

		if(x->key == key)
			return x->elem;

		throw 1;
	}

	void Insert(const KEY &k, const T &v)
	{
		node_ptr update[MAXLEVEL];
		node_ptr x = (SkipNode<KEY,T>*) &header;


		for(int i = level - 1;i >= 0;--i)
		{
			while(x->forward[i] && x->forward[i]->key < k)
				x = x->forward[i];
			update[i] = x;
		}
		
		if( x->forward[0] && x->forward[0]->key == k )
		{
			x->forward[0]->elem = v;
			return;
		}

		const std::ratio<1,2> r;
		int newLevel = randomLevel(MAXLEVEL,r);

		if( newLevel > level )
		{
			for(int i = level; i < newLevel;++i)
				update[i] = (SkipNode<KEY,T>*) &header;
			level = newLevel;
		}
		x = new node_type(k,v,newLevel);
		
		for(int i = 0; i < newLevel;++i)
		{
			x->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = x;
		}
/*
	local update[1..MaxLevel]
	x := list->header
	for i := list->level downto 1 do
		while x->forward[i]->key < searchKey do
			x:= x->forward[i]
		update[i] := x
	
	x:= x->forward[1]
	
	if x->key == searchKey 
		x->value := newValue
	else
		newLevel := randomLevel()
		if newLevel > list->level
			for i:= list->level + 1 to newLevel do
				update[i] := list->header
			list->level := newLevel
		x := makeNode(newLevel, searchKey, value)
		for i := 1 to newLevel do
			x->forward[i] := update[i]->forward[i]
			update[i]->forward[i] := x
*/
	}

	size_t Delete(const KEY &k)
	{
		node_ptr update[MAXLEVEL];
		node_ptr x = (SkipNode<KEY,T>*) &header;

		for(int i = level - 1;i >= 0;--i)
		{
			while(x->forward[i] && x->forward[i]->key < k)
				x = x->forward[i];
			update[i] = x;
		}
	
		if( x->forward[0] == NULL)
			return 0;

		x = x->forward[0];

		if( x->key == k)
		{
			for(int i(0); i < level;++i)
			{
				if( update[i]->forward[i] != x )
					break;
				update[i]->forward[i] = x->forward[i];
			}
	
			delete x;

			while( level > 1 && header.forward[level] == NULL)
				--level;

			return 1;
		}

		return 0;

	}

private:
	SkipHead<key_type,mapped_type> header;
	int level;
};


#endif
