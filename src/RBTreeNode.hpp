#ifndef __RBTREE_NODE_HPP_INCLUDED
#define __RBTREE_NODE_HPP_INCLUDED

#include <memory>
#include <utility>
template <typename>
class RBTreeNode;
#include <RBTreeNodePointer.hpp>

template <typename T>
class RBTreeNode :
public std::enable_shared_from_this<RBTreeNode<T>> {
  using pNode = std::shared_ptr<RBTreeNode<T>>;
  using cNode = std::shared_ptr<const RBTreeNode<T>>;
  using wNode = RBTreeNodePointer<T>;
  using cwNode = RBTreeNodePointer<const T>;
  enum color_t {RED, BLACK};

public:
///////////////////////////////////////////////////////////////////////////////
// member types
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;

///////////////////////////////////////////////////////////////////////////////
// ctor/dtor
  RBTreeNode() : _value() {}
  RBTreeNode(const value_type &value) : _value(value) {}
  template <typename std::enable_if<
    std::is_nothrow_move_constructible<value_type>::value>::type* = nullptr>
  RBTreeNode(value_type &&value) noexcept : _value(std::move(value)) {}
  RBTreeNode(const RBTreeNode &other) = delete;
  ~RBTreeNode() noexcept {}

///////////////////////////////////////////////////////////////////////////////
// query/modifier
  reference value() noexcept {return _value;}
  const_reference value() const noexcept {return _value;}

  //void swap_value(RBTreeNode &other) noexcept {
  //  using std::swap;
  //  swap(_value, other._value);
  //}

  color_t &color() noexcept {return _color;}
  color_t color() const noexcept {return _color;}

  void set_red() noexcept {_color = RED;}
  void set_black() noexcept {_color = BLACK;}
  bool is_red() const noexcept {return _color == RED;}
  bool is_black() const noexcept {return _color == BLACK;}

  bool leaf_child_count() const noexcept {return !_left + !_right;}
  bool is_root() const noexcept {return !_parent;}
  bool is_end() const noexcept {return !_next;}

///////////////////////////////////////////////////////////////////////////////
// node operations
  pNode &left() noexcept {return _left;}
  cNode left() const noexcept {return _left;}
  pNode &right() noexcept {return _right;}
  cNode right() const noexcept {return _right;}

  wNode &parent() noexcept {return _parent;}
  cwNode parent() const noexcept {return _parent;}
  // must not be root
  pNode &sibling() 
    // pNode::get()?
  {return parent()->left() == this->shared_from_this() ? 
    parent()->right() : parent()->left();}
  cNode sibling() const
  {return parent()->left() == this->shared_from_this() ? 
    parent()->right() : parent()->left();}
  pNode &pointer_to_this()
  {return parent()->left() == this->shared_from_this() ? 
    parent()->left() : parent()->right();}
  
  // parent must not be root
  pNode &uncle() {return parent()->sibling();}
  cNode uncle() const {return parent()->sibling();}
  wNode &grandparent() {return parent()->parent();}
  cwNode grandparent() const {return parent()->parent();}

  wNode &prev() noexcept {return _prev;}
  cwNode prev() const noexcept {return _prev;}
  wNode &next() noexcept {return _next;}
  cwNode next() const noexcept {return _next;}

private:
  value_type _value;

  pNode _left;
  pNode _right;

  wNode _parent;
  wNode _prev;
  wNode _next;

  color_t _color = RED;
};

// non swappable
template <typename T>
void swap(RBTreeNode<T> &lhs, RBTreeNode<T> &rhs);

#endif // __RBTREE_NODE_HPP_INCLUDED
