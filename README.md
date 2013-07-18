# Trie (aka. Prefix Tree) #

An C++ implementation of a Trie with some interesting features:

- Follows the same conventions as that of the associative containers in the STL.
  + *Note:* It does not currently have all of the functionality implemented.
- Allow for any sequence container to be used as the key, not just strings.
- Support for propogation inserts:
  + Non-overwriting: Insert the value on every **free** node along the path.
  + Overwriting: Insert the value on **every** node along the path.


# TODO #
* Implement ++ and -- for iterators
* Allow finding prefix and iterating over all subtrees.
  + This requires a special iterator that prevents moving up the tree.
