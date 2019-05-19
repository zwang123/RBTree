#ifndef __RBTREE_NODE_HPP_INCLUDED
#define __RBTREE_NODE_HPP_INCLUDED

#include <memory>
#include <utility>
template <typename>
class RBTreeNode;
#include <RBTreeNodePointer.hpp>

template <typename T>
class RBTreeNode {
  using pNode = std::shared_ptr<RBTreeNode<T>>;
  using cNode = std::shared_ptr<const RBTreeNode<T>>;
  using wNode = RBTreeNodePointer<T>;
  //using wNode = pNode;
  using cwNode = RBTreeNodePointer<const T>;
  //using cwNode = cNode;
  enum color_t {RED, BLACK};

public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;

  RBTreeNode() {}
  RBTreeNode(const value_type &value) : _value(value) {}
  template <typename std::enable_if<
    std::is_nothrow_move_constructible<value_type>::value>::type* = nullptr>
  RBTreeNode(value_type &&value) noexcept : _value(std::move(value)) {}

  RBTreeNode(const RBTreeNode &other) = delete;
  ~RBTreeNode() noexcept {}

  reference value() noexcept {return _value;}
  const_reference value() const noexcept {return _value;}

  pNode &left() noexcept {return _left;}
  cNode left() const noexcept {return _left;}
  pNode &right() noexcept {return _right;}
  cNode right() const noexcept {return _right;}

  wNode &parent() noexcept {return _parent;}
  cwNode parent() const noexcept {return _parent;}

  wNode &prev() noexcept {return _prev;}
  cwNode prev() const noexcept {return _prev;}
  wNode &next() noexcept {return _next;}
  cwNode next() const noexcept {return _next;}

  color_t &color() noexcept {return _color;}
  color_t color() const noexcept {return _color;}

  //bool is_leaf() const noexcept {return !(_left || _right);}
  bool is_root() const noexcept {return _parent().lock() == wNode();}

  void swap_value(RBTreeNode &other) noexcept {
    using std::swap;
    swap(_value, other._value);
  }

private:
  value_type _value;

  pNode _left;
  pNode _right;

  wNode _parent;
  wNode _prev;
  wNode _next;

  color_t _color;
};

// non swappable
template <typename T>
void swap(RBTreeNode<T> &lhs, RBTreeNode<T> &rhs);

#endif // __RBTREE_NODE_HPP_INCLUDED
