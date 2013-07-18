#ifndef MRR_TRIE_HXX_
#define MRR_TRIE_HXX_

#include <iterator>
#include <map>
#include <utility>

//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace mrr {

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Prototypes
template <typename Key, typename T>
class trie_node;

template <typename Key, typename T>
class trie_iterator_base;

template <typename Key, typename T>
class trie_iterator;


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie
{
	friend class trie_iterator_base<Key,T>;

public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = std::pair<const Key, T>;
	using iterator = trie_iterator<Key, T>;

private:
	using node_type = trie_node<Key,T>;
	using edge_list_type = typename node_type::edge_list_type;
	using edge_list_iterator = typename edge_list_type::iterator;

private:
	node_type root_;
	iterator end_;

public:
	trie()
		: root_(), end_()
	{
	}

	// Iterators
	iterator end() const { return end_; }

	// Capacity
	bool empty() const   { return root_.empty(); }

	// Modifiers
	std::pair<iterator,bool> insert(value_type const& x);
	std::pair<iterator,bool> propogate_insert(value_type const& x);
	std::pair<iterator,bool> propogate_insert_overwrite(value_type const& x);
	void clear()         { root_.clear(); }

	// Observers
	iterator find(key_type const& key);

}; // class trie<Key,T>


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// mrr::trie<Key,T> member functions
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
auto trie<Key,T>::insert(value_type const& x) -> std::pair<iterator, bool>
{
	node_type* cur = &root_;
	edge_list_iterator edge;

	// Go through each element in the key, creating a path in the tree
	for(auto const& elem : x.first)
		cur = &(cur->add_edge(elem));

	// If the key already exists, ignore the insert attempt
	if(cur->is_final())
	{
		return std::make_pair(iterator(cur, x.first), false);
	}
	else
	{
		cur->set_value(x.second);
		cur->final(true);

		return std::make_pair(iterator(cur, x.first), true);
	}

} // std::pair<iterator, bool> trie<Key,T>::insert()


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
auto trie<Key,T>::propogate_insert(value_type const& x)
	-> std::pair<iterator,bool>
{
	node_type* cur = &root_;
	edge_list_iterator edge;

	// Go through each element in the key
	for(auto const& elem : x.first)
	{
		// Move cur to node pointed to by elem
		cur = &(cur->add_edge(elem));

		// If the current node is not final, make it final and set values
		if(!cur->is_final())
		{
			cur->set_value(x.second);
			cur->final(true);
		}
	}

	return std::make_pair(iterator(cur, x.first), !cur->is_final());

}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
auto trie<Key,T>::propogate_insert_overwrite(value_type const& x)
	-> std::pair<iterator,bool>
{
	node_type* cur = &root_;
	edge_list_iterator edge;

	// Go through each element in the key
	for(auto const& elem : x.first)
	{
		// Move cur to node pointed to by elem
		cur = &(cur->add_edge(elem));

		cur->set_value(x.second);
		cur->final(true);
	}

	return std::make_pair(iterator(cur, x.first), true);

}



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
auto trie<Key,T>::find(key_type const& key) -> iterator
{
	using std::begin;
	using std::end;

	// Start at the root
	node_type* cur = &root_;

	// Loop through the key
	for(auto const& elem : key)
	{
		// Follow the correct edge
		edge_list_iterator edge = cur->find_edge(elem);

		// If the edge is not found, return end_, if it is, follow edge
		if(edge == cur->edge_end())
			return end_;
		else
			cur = &(edge->second);
	}

	// If the node we end up at is final, return an iterator to it
	return cur->is_final() ? iterator(cur, key) : end_;

} // iterator trie<Key,T>::find()



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie_node
{
	friend class trie<Key,T>;

	using node_type = trie_node<Key,T>;
	using edge_type = typename Key::value_type;
	using edge_list_type = std::map<edge_type, node_type>;
	using edge_list_iterator = typename edge_list_type::iterator;
	using key_type = typename trie<Key,T>::key_type;
	using mapped_type = typename trie<Key,T>::mapped_type;
	using value_type = typename trie<Key,T>::value_type;

	edge_list_type edges_;
	bool final_state_;

public:
	trie_node()
		: edges_(), final_state_(false), parent_(nullptr), value_()
	{
	}

	void clear()
	{
		edges_.clear();
	}

	// Sub tree is empty if there are no edges and this is not a final state
	bool empty() const
	{
		return edges_.empty() && (final_state_ == false);
	}

	node_type& add_edge(typename key_type::value_type const& x)
	{
		return edges_[x];
	}

	edge_list_iterator find_edge(typename key_type::value_type const& x)
	{
		return edges_.find(x);
	}

	edge_list_iterator edge_end()
	{
		return end(edges_);
	}

	bool is_final() const
	{
		return final_state_;
	}

	void final(bool s)
	{
		final_state_ = s;
	}

	void set_value(mapped_type const& val)
	{
		value_ = val;
	}

	trie_node<Key,T>* parent_;
	mapped_type value_;

}; // class trie<Key,T>::trie_node



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie_iterator_base
	: std::iterator<std::forward_iterator_tag, T>
{
	friend class trie<Key,T>;

protected:
	using node_type = typename trie<Key,T>::node_type;
	using key_type = typename trie<Key,T>::key_type;
	using value_type = typename trie<Key,T>::value_type;


public:
	bool operator ==(trie_iterator_base const& iter) const
	{
		return node_ == iter.node_;
	}

	bool operator !=(trie_iterator_base const& iter) const
	{
		return ! (*this == iter);
	}

protected:
	trie_iterator_base()
		: node_(nullptr)
	{
	}

	trie_iterator_base(trie_iterator_base const& iter)
		: node_(iter.node_)
	{
	}

	trie_iterator_base(node_type* node, key_type const& key)
		: node_(node), value_(std::make_pair(key, node->value_))
	{
	}

	node_type* node_;
	value_type value_;

}; // class trie_iterator_base<Key,T>



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie_iterator
	: public trie_iterator_base<Key,T>
{
	friend class trie<Key,T>;

	using node_type = typename trie_iterator_base<Key,T>::node_type;
	using key_type = typename trie_iterator_base<Key,T>::key_type;
	using value_type = typename trie_iterator_base<Key,T>::value_type;

public:
	trie_iterator() = default;

	trie_iterator& operator =(trie_iterator const& iter)
	{
		this->node_ = iter.node_;
		return *this;
	}

	value_type& operator*()
	{
		return std::make_pair(this->key_, this->node_->value_);
	}

	value_type* operator->()
	{
		return &this->value_;
	}

private:
	explicit trie_iterator(node_type* node, key_type const& key)
		: trie_iterator_base<Key,T>(node, key)
	{
	}

}; // class trie<Key,T>::trie_iterator



template <typename Key, typename T>
inline auto end(trie<Key,T>& a) -> decltype(a.end())
{
	return a.end();
}



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

} // namespace mrr

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#endif // #ifndef MRR_TRIE_HXX_
