#ifndef __skiplist_hpp_
#define __skiplist_hpp_
#include <cstdint>
#include <cstddef>
#include <utility>
//#include <vector>

#include "rand.hpp"

template<typename KEY,typename T>
class SkipNode;


template<typename KEY,typename T>
class SkipHead
{
public:
	SkipNode<KEY,T> **forward;

	SkipHead(int Level) : 
		forward(new SkipNode<KEY,T>*[Level]) 
		{  
			for(int i(0); i < Level; ++i)
				forward[i] = nullptr;
		};
	virtual ~SkipHead() { delete [] forward; }
	typedef SkipNode<KEY,T> * node_type;

};

template<typename KEY,typename T>
class SkipNode : public SkipHead<KEY,T>
{
public:
	typedef KEY key_type;
	typedef T mapped_type;
	//typedef SkipNode<KEY,T> * node_ptr;

	SkipNode(const KEY &k, const T &v, int level) : SkipHead<KEY,T>(level), key(k), elem(v) { };
	KEY key;
	T elem;
public:
};


template<typename KEY, typename T>
struct SkipListIterator
{
	typedef SkipListIterator<KEY,T>     _Self;
	typedef SkipNode<KEY,T>             _Node;
	typedef T                           value_type;
	typedef T*                          pointer;
	typedef T&                          reference;
	typedef ptrdiff_t                   difference_type;
	typedef std::forward_iterator_tag   iterator_category;
	
	
	SkipListIterator()
  : m_node() { }

  SkipListIterator(SkipNode<KEY,T>* n)
	: m_node(n) { }

	reference
	operator*() const
	{ return static_cast<_Node*>(this->m_node)->elem; }

	pointer
	operator->() const
	{ return std::__addressof(static_cast<_Node *>(this->m_node)->elem); }

	_Self&
	operator++()
	{
		m_node = m_node->forward[0];
		return *this;
	}

	_Self
	operator++(int)
	{
		_Self tmp(*this);
		m_node = m_node->forward[0];
		return tmp;
	}

	bool operator==(const _Self& rhs) const
	{ return m_node == rhs.m_node; }

	bool operator!=(const _Self&rhs) const
	{ return m_node != rhs.m_node; }

	_Self
	it_next() const
	{
		if(this->m_node)
			return SkipListIterator(m_node->forward[0]);
		else
			return SkipListIterator(0);
	}
	
	
	_Node *m_node;

};


template<typename KEY,typename T, intmax_t MAXLEVEL = 10,
         typename COMPARE = std::less<KEY> >
class SkipListBase
{
	typedef KEY                     key_type;
	typedef T                       mapped_type;
	typedef std::pair<const KEY, T> value_type;
	typedef SkipNode<KEY,T> *       node_ptr;
	typedef SkipNode<KEY,T>         node_type;
public:

	SkipListBase() : header(MAXLEVEL), level(0) { }

	T search(const KEY &key)
	{
		
		node_ptr x = (SkipNode<KEY,T>*) &header;
		for(int i = level - 1; i >= 0 && x; --i)
		{
			while(x->forward[i] && x->forward[i]->key < key)
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
		//node_ptr x = (node_ptr) &header;

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
		//node_ptr x = (SkipNode<KEY,T>*) &header;

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

protected:
	SkipHead<key_type,mapped_type> header;
	int level;
};

template<typename KEY,typename T, intmax_t MAXLEVEL = 10,
         typename COMPARE = std::less<KEY> >
class SkipList : public SkipListBase<KEY,T,MAXLEVEL,COMPARE>
{
public:
	typedef SkipListIterator<KEY,T> iterator;

	iterator
	begin()
	{ return iterator(this->header.forward[0]); }
	
	iterator
	end()
	{ return iterator(0); }
	
};

#endif
