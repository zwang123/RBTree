#ifndef __RBTREE_NODE_POINTER_HPP_INCLUDED
#define __RBTREE_NODE_POINTER_HPP_INCLUDED

#include <memory>
#include <type_traits>
#include <utility>
#ifndef NDEBUG
#include <iostream>
#endif

template <typename>
class RBTreeNode;
template <typename>
class RBTreeNodePointer;

//#include <RBTreeNode.hpp>

// a weak_ptr with some shared_ptr functions
template <typename T>
class RBTreeNodePointer 
: public std::conditional<std::is_const<T>::value,
std::weak_ptr<const RBTreeNode<typename std::remove_const<T>::type>>,
std::weak_ptr<RBTreeNode<T>>>::type {
public:
  using element_type = typename std::conditional<std::is_const<T>::value,
    const RBTreeNode<typename std::remove_const<T>::type>,
    RBTreeNode<T>>::type;

  // use ctors of std::weak_ptr
  template <typename... Args>
  RBTreeNodePointer(Args && ... args) noexcept
  : std::weak_ptr<element_type>(std::forward<Args>(args) ...) {}

  // conversion between NodePointers (e.g. nonconst to const)
  template <typename Y>
  RBTreeNodePointer(const RBTreeNodePointer<Y> & other) noexcept
  : std::weak_ptr<element_type>(
      static_cast<std::weak_ptr<
      typename RBTreeNodePointer<Y>::element_type>>(other)) {}

  element_type &operator*() const noexcept {return *this->lock();}
  std::shared_ptr<element_type> operator->() const noexcept 
  {return this->lock();}
  
  // NodePointers to bool
  explicit operator bool() const noexcept 
  {return static_cast<bool>(this->lock());}
};

template <typename T, typename U>
bool operator==(const RBTreeNodePointer<T> &lhs, 
                const RBTreeNodePointer<U> &rhs) noexcept
{
  return lhs.lock() == rhs.lock();
}

template <typename T, typename U>
bool operator==(const std::shared_ptr<T> &lhs, 
                const RBTreeNodePointer<U> &rhs) noexcept
{
  return lhs == rhs.lock();
}

template <typename T, typename U>
bool operator==(const RBTreeNodePointer<T> &lhs, 
                const std::shared_ptr<U> &rhs) noexcept
{
  return lhs.lock() == rhs;
}

template <typename T, typename U>
bool operator!=(const RBTreeNodePointer<T> &lhs, 
                const RBTreeNodePointer<U> &rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T, typename U>
bool operator!=(const std::shared_ptr<T> &lhs, 
                const RBTreeNodePointer<U> &rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T, typename U>
bool operator!=(const RBTreeNodePointer<T> &lhs, 
                const std::shared_ptr<U> &rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T>
void swap(RBTreeNodePointer<T> &lhs, RBTreeNodePointer<T> &rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_NODE_POINTER_HPP_INCLUDED
