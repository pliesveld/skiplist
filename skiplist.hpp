#ifndef __skiplist_hpp_
#define __skiplist_hpp_
#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>

//#include <initializer_list>

#include "rand.hpp"

template<typename _Key,typename _Tp>
class _skip_node_data;

template<typename _Key,typename _Tp>
class _skip_node_base
{
public:
	_skip_node_data<_Key,_Tp> **forward;

	_skip_node_base(int _level) : 
	forward(new _skip_node_data<_Key,_Tp>*[_level]) 
	{  
		for(int i(0); i < _level; ++i)
			forward[i] = nullptr;
	};
	virtual ~_skip_node_base() { delete [] forward; }

};

template<typename _Key,typename _Tp>
class _skip_node_data : public _skip_node_base<_Key,_Tp>
{
public:
	typedef _Key key_type;
	typedef _Tp mapped_type;

	_skip_node_data(const _Key &k, const _Tp &v, int level) 
  : _skip_node_base<_Key,_Tp>(level), key(k), elem(v) { };

	_Key key;
	_Tp elem;
public:
};


template<typename _Key, typename _Tp>
struct _skip_list_iterator
{
	typedef _skip_list_iterator<_Key,_Tp>     _Self;
	typedef _skip_node_data<_Key,_Tp>             _Node;
	typedef _Tp                           value_type;
	typedef _Tp*                          pointer;
	typedef _Tp&                          reference;
	typedef ptrdiff_t                   difference_type;
	typedef std::forward_iterator_tag   iterator_category;
	
	
	_skip_list_iterator()
  : m_node() { }

  _skip_list_iterator(_skip_node_data<_Key,_Tp>* n)
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
			return _skip_list_iterator(m_node->forward[0]);
		else
			return _skip_list_iterator(0);
	}
	
	_Node *m_node;
};


template<typename _Key,typename _Tp, intmax_t _Maxlevel,
         typename _Compare, typename _Alloc >
class _skip_list_base
{
	typedef _Key                              key_type;
	typedef _Tp                               mapped_type;
	typedef std::pair<const _Key, _Tp>        value_type;

	typedef _skip_node_data<_Key,_Tp> *       node_ptr;
	typedef _skip_node_data<_Key,_Tp>         node_type;

  typedef _Compare                          key_compare;
	typedef _Alloc                            allocator_type;
protected:

/* NOT SURE WHERE I AM GOING WITH THIS */
	typedef typename _Alloc::value_type       _Alloc_value_type;
	typedef typename _Alloc::template rebind<value_type>::other _Pair_alloc_type;
	typedef typename _Alloc::template rebind<_skip_node_base<_Key,_Tp> >::other _Node_alloc_type;

/*
	class value_compare
  : public std::binary_function<value_type, value_type, bool>
  {
    friend class _skip_list_base<_Key,_Tp,_Maxlevel,_Compare,_Alloc>;
		protected:
		_Compare comp;

		value_compare(_Compare __c) : comp(__c) { }
		public:
		bool
		operator()(const value_type& __x, const value_type& __y) const
		{ return comp(__x.first, __y.first); }
	};


	value_compare value_comp;
*/
public:

	_skip_list_base() : header(_Maxlevel), level(0)  { }

	node_ptr _impl_search(const _Key &key)
	{
		node_ptr x = (node_ptr) &header;
		for(int i = level - 1; i >= 0 && x; --i)
		{
			while(x->forward[i] &&
				m_keycomp(x->forward[i]->key, key))
/*
				 x->forward[i]->key < key)
*/
				x = x->forward[i];
		}
		if(!x || !x->forward[0])
			return NULL;

		x = x->forward[0];
		if(x->key == key)
			return x;

		return NULL;
	}

	void insert(const _Key &k, const _Tp &v)
	{
		node_ptr update[_Maxlevel];
		node_ptr x = (node_ptr) &header;

		for(int i = level - 1;i >= 0;--i)
		{
			while(x->forward[i] &&
				m_keycomp(x->forward[i]->key, k))
				x = x->forward[i];
			update[i] = x;
		}
		
		if( x->forward[0] && x->forward[0]->key == k )
		{
			x->forward[0]->elem = v;
			return;
		}

		std::ratio<1,2> r;
		int newLevel = randomLevel(_Maxlevel,r);

		if( newLevel > level )
		{
			for(int i = level; i < newLevel;++i)
				update[i] = (_skip_node_data<_Key,_Tp>*) &header;
			level = newLevel;
		}
		x = new node_type(k,v,newLevel);
		
		for(int i = 0; i < newLevel;++i)
		{
			x->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = x;
		}
	}

	size_t erase(const _Key &k)
	{
		node_ptr update[_Maxlevel];
		node_ptr x = (node_ptr) &header;

		for(int i = level - 1;i >= 0;--i)
		{
			while(x->forward[i] &&
				m_keycomp(x->forward[i]->key, k))

				x = x->forward[i];
			update[i] = x;
		}

		if(x->forward[0] == NULL)
			return 0;

		x = x->forward[0];
	
		if(x->key == k)
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
	_Compare m_keycomp;
	_skip_node_base<key_type,mapped_type> header;
	int level;
};

template<typename _Key,typename _Tp, intmax_t _Maxlevel = 10,
         typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<_Tp> >
class skip_list : public _skip_list_base<_Key,_Tp,_Maxlevel,_Compare,_Alloc>
{
private:

	typedef _skip_list_base<_Key,_Tp, _Maxlevel, _Compare, _Alloc> 			_Base;
	typedef _skip_node_data<_Key, _Tp>																	_Node;
	typedef _skip_node_base<_Key, _Tp>																	_Node_base;
	typedef typename _Base::_Pair_alloc_type                            _Pair_alloc_type;
public:

/*
	STL Container comforming functions to implement.
*/
// void clear()
//mapped_type& operator[] (const key_type& k);
//mapped_type& operator[] (key_type&& k);
	//(*((this->insert(make_pair(x,mapped_type()))).first)).second



	typedef _skip_list_iterator<_Key,_Tp> iterator;

	iterator
	begin()
	{ return iterator(this->header.forward[0]); }
	
	iterator
	end()
	{ return iterator(0); }

	iterator search(const _Key &k)
	{
		return iterator(_Base::_impl_search(k));
	}
	
};

#endif
