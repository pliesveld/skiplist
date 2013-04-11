#ifndef __skiplist_tcc_
#define __skiplist_tcc_


template<typename _Key,typename _Tp, intmax_t _Maxlevel, typename _NodeProperty, typename _Compare, typename _Alloc >

_skip_node_data<_Key,_Tp>* _skip_list_base<_Key,_Tp,_Maxlevel,_NodeProperty,_Compare,_Alloc>::_impl_search(const _Key &key)
{
	node_ptr x = (node_ptr) &m_impl.m_head;
	for(int i = level - 1; i >= 0 && x; --i)
	{
		while(x->forward[i] &&
			m_keycomp(x->forward[i]->key, key))
			x = x->forward[i];
	}
	if(!x || !x->forward[0])
		return NULL;

	x = x->forward[0];
	if(x->key == key)
		return x;

	return NULL;
}



template<typename _Key,typename _Tp, intmax_t _Maxlevel, typename _NodeProperty, typename _Compare, typename _Alloc >
void _skip_list_base<_Key,_Tp,_Maxlevel,_NodeProperty,_Compare,_Alloc>::insert(const _Key &k, const _Tp &v)
{
	node_ptr update[_Maxlevel];
	node_ptr x = (node_ptr) &m_impl.m_head;

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

	int newLevel = level_gen.randomLevel();

	if( newLevel > level )
	{
		for(int i = level; i < newLevel;++i)
			update[i] = (_skip_node_data<_Key,_Tp>*) &m_impl.m_head;
		level = newLevel;
	}
	x = new node_type(k,v,newLevel);
	
	for(int i = 0; i < newLevel;++i)
	{
		x->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = x;
	}
}


template<typename _Key,typename _Tp, intmax_t _Maxlevel, typename _NodeProperty, typename _Compare, typename _Alloc >
size_t _skip_list_base<_Key,_Tp,_Maxlevel,_NodeProperty,_Compare,_Alloc>::erase(const _Key &k)
{
	node_ptr update[_Maxlevel];
	node_ptr x = (node_ptr) &m_impl.m_head;

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

		while( level > 1 && m_impl.m_head.forward[level] == NULL)
			--level;
		return 1;
	}
	return 0;
}

template<typename _Key,typename _Tp, intmax_t _Maxlevel, typename _NodeProperty, typename _Compare, typename _Alloc >
void _skip_list_base<_Key,_Tp,_Maxlevel,_NodeProperty,_Compare,_Alloc>::_impl_clear()
{
	node_ptr x = (node_ptr) &m_impl.m_head;
	node_ptr x_next = m_impl.m_head.forward[0];
	level = 0;
	for(int i(_Maxlevel-1);i > 0;--i)
		m_impl.m_head.forward[i] = 0;

	if(x_next == NULL)
		return;
	x = x_next;

	for(;x_next != NULL;x = x_next)
	{
		x_next = x->forward[0];
		delete x;
	}
}



#endif


