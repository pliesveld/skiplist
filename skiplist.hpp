#ifndef __skiplist_hpp_
#define __skiplist_hpp_
#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>

#include "rand.hpp"
/*
	A c++11x implementation of Skiplist.  
	@author Patrick Liesveld

	See William Pugh, Skip Lists: A Probabilistic Alternative to Balanced Trees, CACM, 33(6):668-676, June 1990.
	More information: http://xlinux.nist.gov/dads/HTML/skiplist.html
*/



/*
	trait expects a constexpr function nProb() 
  that returns the probabilitiy of increasing the height
	of a new node inserted into the skiplist.
*/

template<typename T>
struct _skip_node_height
{
	constexpr double   nProb() { return T::nProp(); }
};

struct null_skiplist_traits
{ };

/*
	default height probability
 */
template<>
struct _skip_node_height<null_skiplist_traits>
{
	constexpr double nProb() { return 0.5; }
};

template<typename _Key,typename _Tp>
class _skip_node_data;

/*
	A node in a skiplist has a level,a constant number of internal pointers.
	Nodes are stored in a sorted order.  Two consecutive nodes of the same height 
  will have level pointer of the first node point to the second node.  If the first
  node has a higher level, each pointer points to a node that is greater than the second.
*/
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

	template<typename _Arg1, typename _Arg2>
	_skip_node_data(int level,_Arg1 &&k, _Arg2 &&v) 
  : _skip_node_base<_Key,_Tp>(level), 
		key(std::forward<_Arg1>(k)), elem(std::forward<_Arg2>(v)) { };


	_Key key;
	_Tp elem;
public:
};


template<typename _Key, typename _Tp>
struct _skip_list_iterator
{
	typedef _skip_list_iterator<_Key,_Tp> _Self;
	typedef _skip_node_data<_Key,_Tp>     _Node;
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

template<typename _Key, typename _Tp>
struct _skip_list_const_iterator
{
	typedef _skip_list_const_iterator<_Key,_Tp>   _Self;
	typedef const _skip_node_data<_Key,_Tp>       _Node;
	typedef _skip_list_iterator<_Key,_Tp>         iterator;

	typedef _Tp                         value_type;
	typedef const _Tp*                  pointer;
	typedef const _Tp&                  reference;
	typedef ptrdiff_t                   difference_type;
	typedef std::forward_iterator_tag   iterator_category;

	_skip_list_const_iterator() : m_node() { }

	explicit
  _skip_list_const_iterator(_skip_node_data<_Key,_Tp>* n)
	: m_node(n) { }

	_skip_list_const_iterator(const iterator& __iter)
	: m_node(__iter.m_node) { }

	reference
	operator*() const
	{ return static_cast<_Node*>(this->m_node)->elem; }
	
	pointer
	operator->() const
	{ return std::__addressof(static_cast<_Node*>
		(this->m_node)->elem); }
	
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

	bool
	operator==(const _Self& __rhs) const
	{ return m_node == __rhs.m_node; }

	bool
	operator!=(const _Self& __rhs) const
	{ return m_node != __rhs.m_node; }

	_Self
	it_next() const
	{
		if(this->m_node)
			return _skip_list_const_iterator(m_node->forward[0]);
		else
			return _skip_list_const_iterator(0);
	}

	const _Node *m_node;
};
	
	
template<typename _Key, typename _Tp>
inline bool
operator==(const _skip_list_iterator<_Key,_Tp> &__x,
           const _skip_list_const_iterator<_Key,_Tp> &__y)
{ return __x.m_node == __y.m_node; }

template<typename _Key,typename _Tp>
inline bool
operator!=(const _skip_list_iterator<_Key,_Tp> &__x,
           const _skip_list_const_iterator<_Key,_Tp> &__y)
{ return __x.m_node != __y.m_node; }


/*
	Skiplist base class.
	
	template arguments:
	@_Key: type of the key.  unique.  must have comparison and equality operators.
	@_Tp:  type of the value.
	@_Maxlevel:  maximum height of the skiplist.
	@_NodeProperty: traits class used to configure the node height probability
	@_Compare:  comparison function to use for @_Key
	@_Alloc:    allocator to use for nodes

*/

template<typename _Key,typename _Tp, intmax_t _Maxlevel, typename _NodeProperty,
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


	static const _skip_node_height<_NodeProperty>	node_property;
protected:


/* rebinds for allocator-aware support */
//	typedef typename _Alloc::value_type       _Alloc_value_type;
//	typedef typename _Alloc::template rebind<value_type>::other _Pair_alloc_type;
	typedef typename _Alloc::template rebind<_skip_node_data<_Key,_Tp> >::other _Node_alloc_type;

	struct _skip_list_impl
	: public _Node_alloc_type
	{
		_skip_node_base<key_type,mapped_type> m_head;

		_skip_list_impl()
			: _Node_alloc_type(), m_head(_Maxlevel)
		{ }

		_skip_list_impl(const _Node_alloc_type& __a)
			: _Node_alloc_type(__a), m_head(_Maxlevel)
		{ }
	};

	_skip_list_impl m_impl;

	struct _level_gen<_Maxlevel>              level_gen;
public:

	_skip_list_base() : m_impl(), level_gen(0.5),level(0)  { };

	_skip_list_base(const _Alloc& __a) : m_impl(__a), level_gen(node_property.nProb()), level(0)  { };


	_skip_list_base(_skip_list_base const &rhs) : m_impl(rhs.m_impl), level_gen(node_property.nProb()), level(rhs.level) 
	{
		node_ptr p = static_cast<node_ptr>(this->m_impl.m_head.forward[0]);
		while(p != NULL)
		{
			this->insert(p->key,p->elem);
			p = p->forward[0];
		}
	}


	~_skip_list_base() { _impl_erase_after((node_ptr)&m_impl.m_head); }

	_Node_alloc_type&
	_m_get_Node_allocator()
	{ return *static_cast<_Node_alloc_type*>(&this->m_impl); }

	_Node_alloc_type&
	_m_get_Node_allocator() const
	{ return *static_cast<const _Node_alloc_type*>(&this->m_impl); }

protected:
	node_ptr	
	_M_get_node()
	{	return _m_get_Node_allocator().allocate(1); }

	void _M_destroy_node(node_ptr __p)
	{
		_m_get_Node_allocator().destroy(__p);
		_M_put_node(__p);
	}

	void _M_put_node(node_ptr __p)
	{ 	_m_get_Node_allocator().deallocate(__p,1); }

	template<typename... _Args>
	node_ptr
	_M_create_node(_Args&&... __args)
	{
		node_ptr __node = this->_M_get_node();
		std::allocator_traits<_Node_alloc_type>::construct(
			_m_get_Node_allocator(),__node,std::forward<_Args>(__args)...);
		return __node;
	}
public:

	/*
		Erase nodes from the start pos to the end.
  */
	void     _impl_erase_after(node_ptr __pos);
	//void     _impl_erase_after(node_ptr __pos, node_ptr __end);
	
	/*
		Returns a pointer to node that contains the key.
		if no node is found, NULL is returned
	*/
	node_ptr _impl_search(const _Key &key);

  /*
		Inserts a new node into the skiplist.  If a node is found
		with key, its value is replaced with v.
  */
	template<typename _Arg1, typename _Arg2>
	void     insert(_Arg1 &&k,_Arg2 &&v);
	//pair<iterator,bool> insert(const _Key &k, const _Tp &v);

#if 0
	template<typename... _Args>
		std::pair<iterator,bool>
		emplace(_Args&&... _args)
	{
		return 
	}
#endif

	/*
		destroys and dellocates the node containing k.
		0 returned if no node is found.
  */
	size_t   erase(const _Key &k);

protected:
	_Compare m_keycomp;
	int level;
};

template<typename _Key,typename _Tp, intmax_t _Maxlevel = 10, typename _NodeProperty = null_skiplist_traits,
         typename _Compare = std::less<_Key>, typename _Alloc = std::allocator<_Tp> >
class skip_list : public _skip_list_base<_Key,_Tp,_Maxlevel,_NodeProperty,_Compare,_Alloc>
{
private:

	typedef _skip_list_base<_Key,_Tp, _Maxlevel, _NodeProperty, _Compare, _Alloc> 			_Base;
	typedef _skip_node_data<_Key, _Tp>																	_Node;
	typedef _skip_node_base<_Key, _Tp>																	_Node_base;
//typedef typename _Base::_Pair_alloc_type                            _Pair_alloc_type;

public:

/*
	STL Container comforming functions to implement.
*/
//mapped_type& operator[] (const key_type& k);
//mapped_type& operator[] (key_type&& k);
	//(*((this->insert(make_pair(x,mapped_type()))).first)).second
// r-value references
// std::initializer

	typedef _skip_list_iterator<_Key,_Tp>        iterator;
	typedef _skip_list_const_iterator<_Key,_Tp>  const_iterator;

	/* 
		Default constructor.  Initialize skiplist with zero elements.
  */
	explicit
	skip_list(const _Alloc& __al = _Alloc()) : _Base(__al) { }

  /*
		Copy Constructor
 	*/
	skip_list(skip_list const &rhs) : _Base(rhs) { }
	
	/*
		Move Constructor
   */

	/*
		assignment operator
 	*/
	skip_list& operator=(skip_list const &rhs)
	{ //note: what if skip_list has a different _Maxlevel
		if(this == &rhs)
			return *this;

		this->level = rhs.level;
		for(int i(0); i < _Maxlevel;++i)
		{
			this->m_impl.m_head.forward[i] = rhs.m_impl.m_head.forward[i];
			rhs.m_impl.m_head.forward[i] = NULL;
		}
		return *this;
	}

	iterator
	begin()
	{ return iterator(this->m_impl.m_head.forward[0]); }

	const_iterator
	begin() const
	{ return const_iterator(this->m_impl.m_head.forward[0]); }
	
	iterator
	end()
	{ return iterator(0); }

	const_iterator
	end() const
	{ return const_iterator(0); }

	bool
	empty() const
	{ return this->m_impl.m_head.forward[0] == NULL; }
	

	iterator search(const _Key &k)
	{
		return iterator(_Base::_impl_search(k));
	}
	
};



#include "skiplist.tcc"
#endif
