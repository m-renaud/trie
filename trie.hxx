#ifndef MRR_TRIE_HXX_
#define MRR_TRIE_HXX_

#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

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

	using seq_type = typename std::conditional<
		std::is_same<Key, char>::value,
		std::string,
		std::vector<Key>
	>::type;
	using seq_iter_type = typename seq_type::const_iterator;

public:
	using key_type = Key;
	using mapped_type = T;
	using value_type = std::pair<seq_type, T>;
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


	//--------------------------------------------------
	// Modifiers

	//------------------------------
	// insert()
	template <typename Iter>
	std::pair<iterator,bool> insert(
		std::pair<std::pair<Iter, Iter>, T> const& x
	);

	template <typename Container>
	std::pair<iterator,bool> insert(
		std::pair<Container, T> const& x
	);

	auto insert(std::pair<const char*, T> const& x)
		-> typename std::enable_if<
			std::is_same<Key, char>::value,
			std::pair<iterator,bool>
		>::type
	{
		return insert(
			std::make_pair(
				std::string(x.first),
				x.second
			)
		);
	}


	//------------------------------
	// propogate_insert()
	template <typename Iter>
	std::pair<iterator,bool> propogate_insert(
		std::pair<std::pair<Iter, Iter>, T> const& x
	);

	template <typename Container>
	std::pair<iterator,bool> propogate_insert(
		std::pair<Container, T> const& x
	);

	auto propogate_insert(std::pair<const char*, T> const& x)
		-> typename std::enable_if<
			std::is_same<Key, char>::value,
			std::pair<iterator,bool>
		>::type
	{
		return propogate_insert(
			std::make_pair(
				std::string(x.first),
				x.second
			)
		);
	}


	//------------------------------
	// propogate_insert_overwrite()
	template <typename Iter>
	std::pair<iterator,bool> propogate_insert_overwrite(
		std::pair<std::pair<Iter, Iter>, T> const& x
	);

	template <typename Container>
	std::pair<iterator,bool> propogate_insert_overwrite(
		std::pair<Container, T> const& x
	);

	auto propogate_insert_overwrite(std::pair<const char*, T> const& x)
		-> typename std::enable_if<
			std::is_same<Key, char>::value,
			std::pair<iterator,bool>
		>::type
	{
		return propogate_insert_overwrite(
			std::make_pair(
				std::string(x.first),
				x.second
			)
		);
	}



	//------------------------------
	void clear()
	{
		root_.clear();
	}


	//--------------------------------------------------
	// Observers
	template <typename Iter>
	iterator find(Iter begin, Iter end);

	template <typename Container>
	iterator find(Container const& key);

	auto find(const char* const& str)
		-> typename std::enable_if<
			std::is_same<Key, char>::value,
			iterator
		>::type
	{
		return find(std::string(str));
	}

}; // class trie<Key,T>


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// mrr::trie<Key,T> member functions
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Iter>
auto trie<Key,T>::insert(std::pair<std::pair<Iter, Iter>, T> const& x)
	-> std::pair<iterator,bool>
{
	return insert(
		value_type(
			seq_type(x.first.first, x.first.second),
			x.second
		)
	);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Container>
auto trie<Key,T>::insert(
	std::pair<Container, T> const& x
)
	-> std::pair<iterator, bool>
{
	using std::begin;
	using std::end;

	node_type* cur = &root_;
	edge_list_iterator edge;

	// Go through each element in the key, creating a path in the tree
	for(auto const& elem : x.first)
		cur = &(cur->add_edge(elem));

	// If the key already exists, ignore the insert attempt
	if(cur->is_final())
	{
		return std::make_pair(
			iterator(cur, seq_type(begin(x.first), end(x.first))),
			false
		);
	}
	else
	{
		cur->set_value(x.second);
		cur->final(true);

		return std::make_pair(
			iterator(cur, seq_type(begin(x.first), end(x.first))),
			true
		);
	}

} // std::pair<iterator, bool> trie<Key,T>::insert()



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Iter>
auto trie<Key,T>::propogate_insert(
	std::pair<std::pair<Iter, Iter>, T> const& x
)
	-> std::pair<iterator,bool>
{
	return propogate_insert(
		value_type(
			seq_type(x.first.first, x.first.second),
			x.second
		)
	);
}



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Container>
auto trie<Key,T>::propogate_insert(
	std::pair<Container, T> const& x
)
	-> std::pair<iterator,bool>
{
	using std::begin;
	using std::end;

	node_type* cur = &root_;
	edge_list_iterator edge;

	if(!cur->is_final())
	{
		cur->set_value(x.second);
		cur->final(true);
	}

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

	return std::make_pair(
		iterator(cur, seq_type(begin(x.first), end(x.first))),
		!cur->is_final()
	);

}



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Iter>
auto trie<Key,T>::propogate_insert_overwrite(
	std::pair<std::pair<Iter, Iter>, T> const& x
)
	-> std::pair<iterator,bool>
{
	return propogate_insert_overwrite(
		value_type(
			seq_type(x.first.first, x.first.second),
			x.second
		)
	);
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Container>
auto trie<Key,T>::propogate_insert_overwrite(
	std::pair<Container, T> const& x
)
	-> std::pair<iterator,bool>
{
	using std::begin;
	using std::end;

	node_type* cur = &root_;
	edge_list_iterator edge;

	cur->set_value(x.second);
	cur->final(true);

	// Go through each element in the key
	for(auto const& elem : x.first)
	{
		// Move cur to node pointed to by elem
		cur = &(cur->add_edge(elem));

		cur->set_value(x.second);
		cur->final(true);
	}

	return std::make_pair(
		iterator(cur, seq_type(begin(x.first), end(x.first))),
		true
	);

}



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Iter>
auto trie<Key,T>::find(Iter first, Iter last) -> iterator
{
	return find(seq_type(first, last));
}


//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
template <typename Container>
auto trie<Key,T>::find(Container const& key) -> iterator
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
	return cur->is_final()
		? iterator(cur, seq_type(begin(key), end(key)))
		: end_;

} // iterator trie<Key,T>::find()



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie_node
{
	friend class trie<Key,T>;

	using node_type = trie_node<Key,T>;
	using edge_type = Key;
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

	node_type& add_edge(key_type const& x)
	{
		return edges_[x];
	}

	edge_list_iterator find_edge(key_type const& x)
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
	using seq_type = typename trie<Key,T>::seq_type;
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

	trie_iterator_base(node_type* node, seq_type const& key_seq)
		: node_(node), value_(std::make_pair(key_seq, node->value_))
	{
	}

	node_type* node_;
	std::pair<seq_type, T> value_;

}; // class trie_iterator_base<Key,T>



//m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
template <typename Key, typename T>
class trie_iterator
	: public trie_iterator_base<Key,T>
{
	friend class trie<Key,T>;

	using seq_type = typename trie_iterator_base<Key,T>::seq_type;
	using node_type = typename trie_iterator_base<Key,T>::node_type;
	using key_type = typename trie_iterator_base<Key,T>::key_type;
	using value_type = typename trie_iterator_base<Key,T>::value_type;

public:
	trie_iterator() = default;

	trie_iterator& operator =(trie_iterator const& iter)
	{
		this->node_ = iter.node_;
		this->value_ = iter.value_;
		return *this;
	}

	value_type& operator*()
	{
		return this->value_;
	}

	value_type* operator->()
	{
		return &this->value_;
	}

private:
	explicit trie_iterator(node_type* node, seq_type const& key_seq)
		: trie_iterator_base<Key,T>(node, key_seq)
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
