# Trie (aka. Prefix Tree)

An C++ implementation of a trie with some interesting features:

- Follows the same conventions as that of the associative containers in the STL.
  + *Note:* It does not currently have all of the functionality implemented.
- Allow for any sequence container to be used as the key, not just strings.
- Support for propogation inserts:
  + Non-overwriting: Insert the value on every **free** node along the path.
  + Overwriting: Insert the value on **every** node along the path.
- Special support for `char` `key_type`.
  + `const char*` sequences are handled through an overload, only present
	when `char` is the key.
  + Accomplished with the use of `std::enable_if`.


### Template Parameters

With the standard associative containers, the first template argument
is the `key_type` and the second is the `mapped_type`, together forming
`value_type` which is defined as `std::pair<key_type,
mapped_type>`, which you would pass to functions such as
`insert()`. With a trie, since they are especially designed for indexing
on sequences of elements, it is an unnecessary restriction to require
only one type of sequence be used for indexing, so as a result, the
`key_type` of the trie is the type which you wish to use for
individual links.


### The Usual `insert()` and `find()`

As mentioned above, this implementation follows closely in the
footsteps of the standard associative contains, only with a few
useful additions. As a result of the choice of `key_type` being only
the link to be used between nodes, you can use any sequence to
insert values into and retrieve values from the trie. For example, if
one was indexing `std::string`s, the `key_type` would be `char`. As a
result, any sequence can be used to insert and find values in the
trie, even `const char*` due to the special case being handled with
`std::enable_if`.

**Example:**

	trie<char, int> t;
	t.insert(std::make_pair("abc", 1));  // "abc" is of type const char*

	std::string key;
	std::cin >> key;
	auto i = t.find(key);  // We can also use a std::string as a key.

	// Usual way of checking for existance of a value.
	if (i != std::end(t))
	  std::cout << i->first << ": " << i->second << std::end;

After the insert, the trie would look like this:

    () --a--> () --b--> () --c--> (1)


### A Nice Addition: Range `insert()` and `find()`

In addition to be able to use a sequence container to index into the
data structure, you are also able to use an iterator range as the
index. This allows for much more power if you only want to index a
value by part of a range.

	trie<char, long int> t;
	std::string key = "hello my name is matt";
	auto spacePos = std::find(key.begin(), key.end(), ' ');

	t.insert(
	  std::make_pair(
	    std::make_pair(key.begin(), spacePos),
	    std::distance(key.begin(), spacePos)
	  )
	);

	// Just to show another sequence being used.
	std::vector<char> search{'h', 'e', 'l', 'l', 'o'};
	std::cout << t.find(search)->second << std::endl;


### Propogate Inserts

Another useful feature available is something I call *propogate
inserts*. What these inserts due is place the value being mapped
on every node along the path. There are two versions:

#### `propogate_insert`

With this version, if a value already exists at a node, it will not
modify it. So if you have the following trie:

	() --a--> () --b--> (1)

Inserting the pair ("abc", 4) into an empty trie would yield the
following: 

	(4) --a--> (4) --b--> (1) --c--> (4)


#### `propogate_insert_overwrite`

This version will overwrite all nodes along the path, so assuming the
same starting trie as for `propagate_insert`, we would end up with
the following:

	(4) --a--> (4) --b--> (4) --c--> (4)
