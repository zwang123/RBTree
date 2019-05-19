#ifndef __RBTREE_HPP_INCLUDED
#define __RBTREE_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <type_traits>
#include <RBTreeNode.hpp>
#include <RBTreeIterator.hpp>

template <typename T, typename Compare = std::less<T>, typename Enable = void>
class RBTree;

template <typename T, typename Compare>
class RBTree<T, Compare, 
      typename std::enable_if<std::is_assignable<T&, T>::value
      && !std::is_reference<T>::value>::type> {

  using Node = RBTreeNode<T>;
  using pNode = std::shared_ptr<Node>;
  using cNode = std::shared_ptr<const Node>;

public:

  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = RBTreeIterator<const T>;
  using const_iterator = RBTreeIterator<const T>;
  using difference_type = typename iterator::difference_type;
  using size_type = std::size_t;
  
  RBTree() {}
  RBTree(const RBTree &other) 
    : _root(copy_node(other._root)) {
    traverse_build();
  }
  RBTree(RBTree &&other) noexcept {this->swap(other);}
  RBTree &operator=(const RBTree &other) {
    RBTree cpy(other);
    this->swap(cpy);
    return *this;
  }
  RBTree &operator=(RBTree &&other) noexcept {
    this->swap(other); return *this;}
  ~RBTree() noexcept {}
  
  iterator root() noexcept {return _root;}
  const_iterator root() const noexcept {return _root;}

  iterator begin() noexcept {return _begin;}
  const_iterator cbegin() const noexcept {return _begin;}
  iterator end() noexcept {return _end;}
  const_iterator cend() const noexcept {return _end;}

  size_type size() const noexcept {return _size;}
  bool empty() const noexcept {return _begin == _end;}

  void swap(RBTree &other) noexcept {
    using std::swap;
    swap(_root, other._root);
    swap(_begin, other._begin);
    swap(_end, other._end);
    swap(_size, other._size);
  }

private:
  pNode _root;
  iterator _begin;
  iterator _end;
  size_type _size;


  // TODO build prev and next pointers
  void traverse_build();

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

template <typename T, typename Compare>
void swap(RBTree<T, Compare> lhs, RBTree<T, Compare> rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_HPP_INCLUDED
