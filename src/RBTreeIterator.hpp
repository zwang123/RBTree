#ifndef __RBTREE_ITERATOR_HPP_INCLUDED
#define __RBTREE_ITERATOR_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <type_traits>
template <typename, typename>
class RBTreeIterator;
#include <RBTreeDeclare.hpp>
#include <RBTreeNode.hpp>
#include <RBTreeNodePointer.hpp>

template <typename T, typename Enable = void>
class RBTreeIterator;

template <typename T>
class RBTreeIterator<T,
      typename std::enable_if<std::is_const<T>::value>::type> {
public:
///////////////////////////////////////////////////////////////////////////////
// iterator traits
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename std::remove_const<T>::type;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

///////////////////////////////////////////////////////////////////////////////
// friends
  template <typename T1, typename T2, typename T3>
  friend class RBTree;
  template <typename T1, typename T2>
  friend bool operator==(const RBTreeIterator<T1> &, 
                         const RBTreeIterator<T2> &) noexcept;
#ifndef NDEBUG
  friend void testInsertion();
#endif

private:
  using wNode = RBTreeNodePointer<T>;
  using sNode = std::shared_ptr<const RBTreeNode<value_type>>;

  template <typename Y>
  RBTreeIterator(const std::shared_ptr<Y> &ptr) noexcept : _ptr(ptr) {}
  template <typename Y>
  RBTreeIterator(const RBTreeNodePointer<Y> &ptr) noexcept : _ptr(ptr) {}

  sNode lock() const noexcept {return _ptr.lock();}

public:
  constexpr RBTreeIterator() noexcept {}

  reference operator*() const noexcept {return lock()->value();}
  pointer operator->() const noexcept {return &lock()->value();}

  RBTreeIterator &operator++() {_ptr = lock()->next(); return *this;}
  RBTreeIterator &operator--() {_ptr = lock()->prev(); return *this;}
  RBTreeIterator operator++(int) {
    RBTreeIterator other(*this); ++*this; return other;}
  RBTreeIterator operator--(int) {
    RBTreeIterator other(*this); --*this; return other;}

  void swap(RBTreeIterator &other) noexcept {
    using std::swap;
    swap(_ptr, other._ptr);
  }

private:
  wNode _ptr;
};

template <typename T, typename U>
bool operator==(const RBTreeIterator<T> &lhs, 
                const RBTreeIterator<U> &rhs) noexcept
{
  return lhs.lock() == rhs.lock();
}

template <typename T, typename U>
bool operator!=(const RBTreeIterator<T> &lhs, 
                const RBTreeIterator<U> &rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T>
void swap(RBTreeIterator<T> &lhs, RBTreeIterator<T> &rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_ITERATOR_HPP_INCLUDED
