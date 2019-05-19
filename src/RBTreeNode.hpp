#ifndef __RBTREE_NODE_HPP_INCLUDED
#define __RBTREE_NODE_HPP_INCLUDED

#include <memory>
#include <utility>

template <typename T>
//class RBTreeNode :
//public std::enable_shared_from_this<RBTreeNode<T>> {
class RBTreeNode {
  //friend class RBTreeNodeIterator<const T>;
  //template <typename Compare>
  //friend class RBTree<T, Compare>;

  using pNode = std::shared_ptr<RBTreeNode<T>>;
  using cNode = std::shared_ptr<const RBTreeNode<T>>;
  enum color_t {RED, BLACK};

  //RBTreeNode(const RBTreeNode &other) : _value(other._value),
  //_left(other._left?std::make_shared(*other._left):nullptr),
  //_right(other._right?std::make_shared(*other._right):nullptr),
  //_color(other._color) {}
  //
  //pNode copy_from_this() const {
  //  pNode cpy = make_shared(*this);


  //  if (_left) _left->parent = std::shared_from_this();
  //  if (_right) _left->parent = std::shared_from_this();
  //}

  //void update

public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  //RBTreeNode() = default;

  RBTreeNode() {}
  RBTreeNode(const value_type &value) : _value(value) {}
  template <typename std::enable_if<
    std::is_nothrow_move_constructible<value_type>::value>::type* = nullptr>
  RBTreeNode(value_type &&value) noexcept : _value(std::move(value)) {}

  RBTreeNode(const RBTreeNode &other) = delete;
  //RBTreeNode(RBTreeNode &&other) noexcept {this->swap(other);}
  //RBTreeNode &operator=(const RBTreeNode &other) {
  //  RBTreeNode cpy(other);
  //  this->swap(cpy);
  //  return *this;
  //}

  //RBTreeNode &operator=(RBTreeNode &&other) noexcept {
  //  this->swap(other); return *this;}
  ~RBTreeNode() noexcept {}

  //pNode getLeft() noexcept {return _left;}
  //cNode getLeft() const noexcept {return _left;}
  //pNode getRight() noexcept {return _right;}
  //cNode getRight() const noexcept {return _right;}

  //pNode getPrev() noexcept {return _prev;}
  //pNode getNext() noexcept {return _next;}
  
  reference value() noexcept {return _value;}
  const_reference value() const noexcept {return _value;}

  pNode &left() noexcept {return _left;}
  cNode left() const noexcept {return _left;}
  pNode &right() noexcept {return _right;}
  cNode right() const noexcept {return _right;}
  pNode &parent() noexcept {return _parent;}
  cNode parent() const noexcept {return _parent;}

  pNode &prev() noexcept {return _prev;}
  cNode prev() const noexcept {return _prev;}
  pNode &next() noexcept {return _next;}
  cNode next() const noexcept {return _next;}

  color_t &color() noexcept {return _color;}
  color_t color() const noexcept {return _color;}

  void swap_value(RBTreeNode &other) noexcept {
    using std::swap;
    swap(_value, other._value);
  }

private:
  value_type _value;

  pNode _left;
  pNode _right;

  pNode _parent;
  pNode _prev;
  pNode _next;

  color_t _color;
};

// non swappable
template <typename T>
void swap(RBTreeNode<T> &lhs, RBTreeNode<T> &rhs);

#endif // __RBTREE_NODE_HPP_INCLUDED
