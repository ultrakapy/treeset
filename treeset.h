#ifndef TREESET_HH
#define TREESET_HH

#include <memory>
#include <limits>
#include <initializer_list>
#include <ostream>
#include <stack>
#include <cassert>
#include <functional>
#include <type_traits>

/***************** Begin TreeSet declaration  ****************/

template <typename T, typename Compare = std::less<T>>
class TreeSetIter; //! Forward declaration of class TreeSetIter

/*!
TreeSet is an ordered-set data type that internally uses a binary search tree to
store and retrieve its values.
*/
template <typename T, typename Compare = std::less<T>>
class TreeSet {
  /*!
    Node is the internal (and private) tree representation used by the TreeSet.
  */
  struct node {
    T value;
    std::shared_ptr<node> left;
    std::shared_ptr<node> right;

    //! node constructor that sets the value of the node
    node(const T &value) : value(value) { };

    //! node Copy-Contructor to make a deep copy of the tree node
    node(const std::shared_ptr<node> &other);

    //! Replaces one of the node's children with a new node (or empty smart-ptr)
    void replace_child(const std::shared_ptr<node> &old_child,
                       const std::shared_ptr<node> &new_child);
  };
  using sp_node = std::shared_ptr<node>;

  //! The root node of the binary search tree.
  sp_node _root;

  //! Stores the size of the tree so that it can be returned in constant time.
  int _size;

  //! Comparator used for the items in the TreeSet
  Compare _cmp;

  /*! Verifies that the node n holds a value between minval & maxval, and then
    recursively checks the children of n with the same function, updating minval
    and/or maxval appropriately. The function prints all identified issues to cerr
    (not stopping after the first issue is found), and it returns false if any
    issues are identified along the way. This way the function can be used with
    assert() everywhere the tree is changed.
    Note: We only perform sanity check if T has std::numeric_limits.
    Assumes minval & maxval are valid for the TreeSet's comparator.
  */
  bool sanity_check(const sp_node &n, const T &minval, const T &maxval) const;

  /*! Only perform sanity check if T has std::numeric_limits.
    Use this TreeSet's Compare fn to determine minval & maxval to use for check
  */
  bool sanity_check(const sp_node &n) const;

  /*! Merge two subtrees of a binary search tree into one while maintaining the
    binary search tree invariant. Assumes small node is left subtree and big node
    is right subtree.
  */
  sp_node merge(const sp_node &small, sp_node &big);

public:
  //! As a friend, TreeSetIter has access to all private members of TreeSet
  friend class TreeSetIter<T, Compare>;
  
  //! Provide "standard" name for iterator type
  using iterator = TreeSetIter<T, Compare>;

  //! Constructor initializes an empty set. Note: sp_node() creates nullptr.
  TreeSet() : _root(nullptr), _size(0), _cmp(Compare{}) { };

  //! Initializer-list constructor
  TreeSet(const std::initializer_list<T> &list);

  //! Copy-constructor
  TreeSet(const TreeSet<T, Compare> &other);

  //! Copy-assignment operator
  TreeSet<T, Compare>& operator=(const TreeSet<T, Compare> &other);

  //! Move-constructor
  TreeSet(TreeSet<T, Compare> &&other);

  //! Move-assignment operator
  TreeSet<T, Compare>& operator=(TreeSet<T, Compare> &&other);

  //! Destructor is explicity defaulted since we are using only smart pointers
  ~TreeSet() = default;

  //! Return an iterator to the first value in the TreeSet
  TreeSetIter<T, Compare> begin() const;
  
  //! Return an iterator "past the end" of the TreeSet. Use empty node pointer.
  TreeSetIter<T, Compare> end() const;

  //! Returns true if the rhs set contains the same values as this set.
  bool operator==(const TreeSet<T, Compare> &rhs);

  //! Inverse of ==
  bool operator!=(const TreeSet<T, Compare> &rhs) { return !(*this == rhs); }

  //! Computes the set-union of this set and the provided set s. Returns new set.
  TreeSet<T, Compare> plus(const TreeSet<T, Compare> &s) const;

  //! Computes the set-intersection of this set & provided set s. 
  TreeSet<T, Compare> intersect(const TreeSet<T, Compare> &s) const;

  //! Computes the set-difference of this set & provided set s.
  TreeSet<T, Compare> minus(const TreeSet<T, Compare> &s) const;

  //! Returns the number of elements in the set.
  int size() const { return _size; };

  //! Attempts to add a value to the set.
  bool add(const T &value);

  //! Attemps to remove value from the set.
  bool del(const T &value);

  //! Returns whether the value appears in the set or not.
  bool contains(const T &value) const;
};

/***************** End TreeSet declaration  ****************/





/***************** Begin TreeSetIter declaration & definition ****************/

/*! TreeSetIter provides iterator functionality for the TreeSet
*/
template <typename T, typename Compare>
class TreeSetIter {
  std::stack<typename TreeSet<T, Compare>::sp_node> _next_node_stack;
  TreeSet<T, Compare>::sp_node _current_node;

  //! Inorder traversal to leftmost node, adding visited nodes to stack.
  void inorder_traverse_to_leftmost_node(const TreeSet<T, Compare>::sp_node &n);

public:
  //! Default constructor
  TreeSetIter() { };

  //! Constructor
  TreeSetIter(const TreeSet<T, Compare>::sp_node &root_node) {
    inorder_traverse_to_leftmost_node(root_node);
  }
  
  //! Pre-increment operator returns a ref to the iterator that was incremented.
  TreeSetIter<T, Compare>& operator++();

  //! Post-increment operator returns a copy of the iterator before incremented.
  TreeSetIter<T, Compare> operator++(int);

  //! Dereference returns value of node being pointed to by iterator
  T operator*();

  //! Compares pointers of the tree nodes
  bool operator==(const TreeSetIter<T, Compare> &rhs) const;

  //! Inverse of ==
  bool operator!=(const TreeSetIter<T, Compare> &rhs) const {
    return !(*this == rhs);
  };
};

template <typename T, typename Compare> inline
void TreeSetIter<T, Compare>::inorder_traverse_to_leftmost_node(
const TreeSet<T, Compare>::sp_node &from_node) {
  // copy node shared_ptr to use for traversing the tree
  typename TreeSet<T, Compare>::sp_node n = from_node;
  
  while (n != nullptr) {
    _next_node_stack.push(n);
    n = n->left;
  }

  if (!_next_node_stack.empty()) {
    _current_node = _next_node_stack.top();
    _next_node_stack.pop();
  } else {
    _current_node = nullptr;
  }
}

template <typename T, typename Compare> inline
TreeSetIter<T, Compare>& TreeSetIter<T, Compare>::operator++() {
  if (_current_node != nullptr)
    inorder_traverse_to_leftmost_node(_current_node->right);

  return *this;
}

template <typename T, typename Compare> inline
TreeSetIter<T, Compare> TreeSetIter<T, Compare>::operator++(int) {
  TreeSetIter<T, Compare> it = *this;
  ++(*this);
  return it;
}

template <typename T, typename Compare> inline
T TreeSetIter<T, Compare>::operator*() {
  return _current_node->value;
}

template <typename T, typename Compare> inline
bool TreeSetIter<T, Compare>::operator==(const TreeSetIter<T, Compare> &rhs)
  const {
  return _current_node == rhs._current_node;
}

/***************** End TreeSetIter declaration & definition  ****************/





/***************** Begin TreeSet definition ****************/

template <typename T, typename Compare> inline
TreeSet<T, Compare>::TreeSet(const std::initializer_list<T> &list)
  : _root(nullptr), _size(0), _cmp(Compare{}) {
  for (T item : list) {
    add(item);
  }
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::TreeSet(const TreeSet<T, Compare> &other) {
  _size = other._size;
  // call node copy constructor which makes a deep copy
  if (other._size > 0) {
    _root = std::make_shared<node>(other._root);
  } else {
    _root = nullptr;
  }
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>& TreeSet<T, Compare>::operator=(const TreeSet<T, Compare>
                                                    &other) {
  if (this == &other) // detect and handle self-assignment
    return *this;

  // no need to set existing _root to nullptr. shared_ptr should cleanup itself
  _size = other._size;

  // call node copy constructor which makes a deep copy
  if (other.size() > 0) {
    _root = std::make_shared<node>(other._root);
  } else {
    _root = nullptr;
  }

  return *this;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::TreeSet(TreeSet<T, Compare> &&other)
  : _root(other._root), _size(other._size) {
  // no need to set other._root to nullptr. share_ptr should cleanup itself
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>& TreeSet<T, Compare>::operator=(TreeSet<T, Compare> &&other) {
  if (this == &other) // detect and handle self-assignment
    return *this;
  
  _size = other._size;
  
  if (other.size() > 0) {
    _root = other._root;
  } else {
    _root = nullptr;
  }
  
  // no need to set other._root to nullptr. share_ptr should cleanup itself
  
  return *this;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::iterator TreeSet<T, Compare>::begin() const {
  return TreeSetIter<T, Compare>{_root};
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::iterator TreeSet<T, Compare>::end() const {
  return TreeSetIter<T, Compare>{};
}

template <typename T, typename Compare> inline
bool TreeSet<T, Compare>::operator==(const TreeSet<T, Compare> &rhs) {
  auto this_it = begin();
  auto rhs_it = rhs.begin();
  
  while (this_it != end() && rhs_it != rhs.end()) {
    if (*this_it++ != *rhs_it++)
      return false;
  }

  return this_it == rhs_it; // both should equal end()
}

template <typename T, typename Compare> inline
TreeSet<T, Compare> TreeSet<T, Compare>::plus(const TreeSet<T, Compare> &s)
  const {
  TreeSet<T, Compare> new_set;
  
  for (auto this_it = begin(); this_it != end(); this_it++) {
    new_set.add(*this_it);
  }

  for (auto s_it = s.begin(); s_it != s.end(); s_it++) {
    new_set.add(*s_it);
  }

  return new_set;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare> TreeSet<T, Compare>::intersect(const TreeSet<T, Compare> &s)
  const {
  TreeSet<T, Compare> new_set;

  for (auto this_it = begin(); this_it != end(); this_it++) {
    for (auto s_it = s.begin(); s_it != s.end(); s_it++) {
      if (*this_it == *s_it)
        new_set.add(*this_it);
    }
  }

  return new_set;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare> TreeSet<T, Compare>::minus(const TreeSet<T, Compare> &s)
  const {
  TreeSet<T, Compare> new_set;

  for (auto this_it = begin(); this_it != end(); this_it++) {
    if (!s.contains(*this_it))
      new_set.add(*this_it);
  }

  return new_set;
}

/*! Outputs the contents of the set in this format: "[1,2,3]"
  Stream-output operator must not output a "\n" character, or any whitespace.
  An empty set would be output as: "[]" */
template <typename T, typename Compare>
ostream& operator<<(ostream &os, const TreeSet<T, Compare> &s) {
  os << "[";

  typename TreeSet<T, Compare>::iterator it = s.begin();
  while (it != s.end()) {
    os << *it++;
    
    if (it != s.end())
      os << ",";
  }

  os << "]";
  return os;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::node::node(const sp_node &other) {
  if (other == nullptr)
    return;

  value = other->value;

  if (other->left != nullptr)
    left = std::make_shared<node>(other->left);

  if (other->right != nullptr)
    right = std::make_shared<node>(other->right);
}

template <typename T, typename Compare> inline
void TreeSet<T, Compare>::node::replace_child(const sp_node &old_child,
                                              const sp_node &new_child) {
  if (left == old_child)
    left = new_child;
  else
    right = new_child;
}

template <typename T, typename Compare> inline bool
TreeSet<T, Compare>::sanity_check(const sp_node &n,
                                  const T &minval, const T &maxval) const {
  if (n == nullptr)
    return _cmp(minval, maxval);

  if (_cmp(n->value, minval) || _cmp(maxval, n->value)) {
    cerr << "node " << n->value << " has issues.";
    cerr << " minval: " << minval << ", maxval: " << maxval << endl;
  }

  return sanity_check(n->left, minval, n->value) &&
    sanity_check(n->right, n->value, maxval);
}

template <typename T, typename Compare> inline bool
TreeSet<T, Compare>::sanity_check(const sp_node &n) const {
  // Only perform sanity check if T has std::numeric_limits.
  // Use this TreeSet's Compare fn to determine minval & maxval to use for check
  if (std::numeric_limits<T>::is_specialized) {
    // Initial guess at min/max values
    T minval = std::numeric_limits<T>::min();
    T maxval = std::numeric_limits<T>::max();

    // If guess was wrong, swap values
    if (!_cmp(minval, maxval)) {
      std::swap(minval, maxval);
    }
    
    return sanity_check(n, minval, maxval);
  }

  // Intentionality not performing any sanity checks for other T and Compare types
  return true;
}

template <typename T, typename Compare> inline
TreeSet<T, Compare>::sp_node TreeSet<T, Compare>::merge(const sp_node &small,
                                                        sp_node &big) {
  if (small == nullptr)
    return big;

  if (big == nullptr)
    return small;

  sp_node n = big;
  while (n->left != nullptr) {
    n = n->left;
  }

  n->left = small;
  return big;
}

template <typename T, typename Compare> inline
bool TreeSet<T, Compare>::add(const T &value) {
  assert(sanity_check(_root));

  if (size() == 0) {
    _root = std::make_shared<node>(value);
    _size = 1;

    assert(sanity_check(_root));

    return true;
  }

  sp_node n = _root;
  
  while (n != nullptr) {
    if (value == n->value) { // value already exists
      return false;
    } else if (_cmp(value, n->value)) { // attempt add to left subtree
      if (n->left == nullptr) {
        n->left = std::make_shared<node>(value);
        _size++;
        return true;
      } else {
        n = n->left;
      }
    } else { // attempt add to right subtree
      if (n->right == nullptr) {
        n->right = std::make_shared<node>(value);
        _size++;
        return true;
      } else {
        n = n->right;
      }
    }
  }

  assert(sanity_check(_root));

  return false;
}

template <typename T, typename Compare> inline
bool TreeSet<T, Compare>::contains(const T &value) const {
  if (size() == 0)
    return false;

  sp_node n = _root;
  
  while (n != nullptr) {
    if (value == n->value) {
      return true;
    } else if (_cmp(value, n->value)) {
      n = n->left;
    } else {
      n = n->right;
    }
  }

  return false;
}

template <typename T, typename Compare> inline
bool TreeSet<T, Compare>::del(const T &value) {
  assert(sanity_check(_root));

  if (size() == 0)
    return false;

  sp_node n = _root;
  sp_node parent = nullptr;

  while (n != nullptr) {
    if (value == n->value) { // found value to delete
      if (parent == nullptr) { // we need to update the _root
        // so merge the _root's children into the new _root        
        _root = merge(n->left, n->right);
      } else { // no need to update the _root
        // we are deleting n, so first merge n's children
        sp_node merged_children = merge(n->left, n->right);

        // now replace n with its merged children
        parent->replace_child(n, merged_children);
      }
      
      _size--;
      return true;
    } else if (_cmp(value, n->value)) { // attempt delete from left subtree
      parent = n;
      n = n->left;
    } else { // attempt delete from right subtree
      parent = n;
      n = n->right;
    }
  }

  assert(sanity_check(_root));

  return false;
}

/***************** End TreeSet definition ****************/

#endif
