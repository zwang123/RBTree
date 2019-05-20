#ifndef __RBTREE_HPP_INCLUDED
#define __RBTREE_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
template <typename, typename, typename>
class RBTree;
#include <RBTreeIterator.hpp>
#include <RBTreeNode.hpp>
#include <RBTreeNodePointer.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

template <typename T, typename Compare = std::less<T>, typename Enable = void>
class RBTree;

template <typename T, typename Compare>
class RBTree<T, Compare, 
      typename std::enable_if<std::is_assignable<T&, T>::value
      && !std::is_reference<T>::value>::type> {

  using Node = RBTreeNode<T>;
  using pNode = std::shared_ptr<Node>;
  using cNode = std::shared_ptr<const Node>;
  using wNode = RBTreeNodePointer<T>;

public:

///////////////////////////////////////////////////////////////////////////////
// member types
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = RBTreeIterator<const T>;
  using const_iterator = RBTreeIterator<const T>;
  using difference_type = typename iterator::difference_type;
  using size_type = std::size_t;
  
///////////////////////////////////////////////////////////////////////////////
// ctor
  RBTree() {}
  RBTree(const RBTree &other) 
    : _root(copy_node(other._root)) {
    build_prev_next(_root);
  }
  RBTree(RBTree &&other) noexcept {this->swap(other);}

///////////////////////////////////////////////////////////////////////////////
// dtor
  ~RBTree() noexcept {}

///////////////////////////////////////////////////////////////////////////////
// operator=
  RBTree &operator=(const RBTree &other) {
    RBTree cpy(other);
    this->swap(cpy);
    return *this;
  }
  RBTree &operator=(RBTree &&other) noexcept {
    this->swap(other); return *this;}
  
///////////////////////////////////////////////////////////////////////////////
// iterators
  iterator root() noexcept {return _root;}
  const_iterator root() const noexcept {return _root;}

  iterator begin() noexcept {return _begin;}
  const_iterator cbegin() const noexcept {return _begin;}
  // end is always nullptr
  constexpr iterator end() noexcept {return iterator();}
  constexpr const_iterator cend() const noexcept {return const_iterator();}

///////////////////////////////////////////////////////////////////////////////
// capacity
  bool empty() const noexcept {return cbegin() == cend();}
  size_type size() const noexcept {return _size;}

///////////////////////////////////////////////////////////////////////////////
// modifiers
  void clear() noexcept;
  // dummy function, TODO
  std::pair<iterator, bool> insert(const_reference value) {
    if (empty()) {
      _begin = _root = std::make_shared<Node>(value);
    } else {
      _begin->left() = std::make_shared<Node>(value);
      _begin->left()->parent() = _begin;
      _begin->prev() = _begin->left();
      _begin->left()->next() = _begin;
      _begin = _begin->left();
    }
    ++_size;
    return {begin(), true};
  }

  void swap(RBTree &other) noexcept {
    using std::swap;
    swap(_root, other._root);
    swap(_begin, other._begin);
    swap(_size, other._size);
  }
  
///////////////////////////////////////////////////////////////////////////////
// lookup

private:
  pNode _root; // store above root instead of root?
  wNode _begin; // TODO should be weak_ptr
  size_type _size;


  // build prev and next pointers, and size and begin
  void build_prev_next(pNode curr) {
    if (curr == nullptr) return;

    ++_size;
    if (_begin == curr->parent()) _begin = curr;

    // assume all prev and next starts at nullptr
    if (curr->right()) {
      if (curr->next()) curr->next()->prev() = curr->right();
      curr->right()->next() = curr->next();
      curr->right()->prev() = curr;
      curr->next() = curr->right();
    }
    if (curr->left()) {
      if (curr->prev()) curr->prev()->next() = curr->left();
      curr->left()->prev() = curr->prev();
      curr->left()->next() = curr;
      curr->prev() = curr->left();
    }
    build_prev_next(curr->left());
    build_prev_next(curr->right());
  }

  static pNode copy_node(cNode src) {
    if (src == nullptr) return nullptr;
    pNode dest = std::make_shared<Node>(src->value());
    dest->color() = src->color();
    dest->left() = copy_node(src->left());
    dest->right() = copy_node(src->right());
    if (dest->left()) dest->left()->parent() = dest;
    if (dest->right()) dest->right()->parent() = dest;
    return dest;
  }
};

///////////////////////////////////////////////////////////////////////////////
// non-member functions
template <typename T, typename Compare>
void swap(RBTree<T, Compare> lhs, RBTree<T, Compare> rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_HPP_INCLUDED
