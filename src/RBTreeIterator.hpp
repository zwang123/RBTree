#ifndef __RBTREE_ITERATOR_HPP_INCLUDED
#define __RBTREE_ITERATOR_HPP_INCLUDED

#include <cstddef>
#include <type_traits>
//#include <utility>
#include <RBTreeNode.hpp>
#include <RBTreeDeclare.hpp>

template <typename T, typename Enable = void>
class RBTreeIterator;

template <typename T>
class RBTreeIterator<T,
      typename std::enable_if<std::is_const<T>::value>::type> {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  //using value_type = T;
  using difference_type = std::ptrdiff_t;
  //using pointer = const value_type*;
  //using reference = const value_type&;

  using value_type = typename std::remove_const<T>::type;
  using pointer = T*;
  using reference = T&;

  template <typename T1, typename T2, typename T3>
  friend class RBTree;

private:
  using cNode = std::shared_ptr<const RBTreeNode<value_type>>;
  template <typename Y>
  RBTreeIterator(const std::shared_ptr<Y> &ptr) noexcept : _ptr(ptr) {}
  //template <typename Y>
  //RBTreeIterator(std::shared_ptr<Y> &&ptr) noexcept : _ptr(std::move(ptr)) {}
  //template <typename Y>
  //RBTreeIterator &operator=(const std::shared_ptr<Y> &ptr) noexcept 
  //{_ptr = ptr;}
  //template <typename Y>
  //RBTreeIterator &operator=(std::shared_ptr<Y> &&ptr) noexcept 
  //{_ptr = std::move(ptr);}

//public:
//  using iterator_category = std::bidirectional_iterator_tag;
//  using value_type = RBTreeNode<typename std::remove_const<T>::type>;
//private:
//  using c_value_type = typename std::conditional<
//    std::is_const<T>::value,
//    const value_type, value_type>::type;
//public:
//  using difference_type = std::ptrdiff_t;
//  using pointer = std::shared_ptr<c_value_type>;
//  using reference = c_value_type&;
//
//  RBTreeIterator(pointer ptr = nullptr) noexcept : _ptr(ptr) {}
//  RBTreeIterator &operator=(pointer ptr) noexcept {_ptr = ptr; return *this;}
//  reference operator*() const noexcept {return *_ptr;}
//  pointer operator->() const noexcept {return _ptr;}

public:
  RBTreeIterator() noexcept {}
  reference operator*() const noexcept {return _ptr->value();}
  pointer operator->() const noexcept {return &_ptr->value();}

  RBTreeIterator &operator++() {_ptr = _ptr->next(); return *this;}
  RBTreeIterator &operator--() {_ptr = _ptr->prev(); return *this;}
  RBTreeIterator operator++(int) {
    RBTreeIterator other(*this); ++*this; return other;}
  RBTreeIterator operator--(int) {
    RBTreeIterator other(*this); --*this; return other;}


  void swap(RBTreeIterator &other) noexcept {
    using std::swap;
    swap(_ptr, other._ptr);
  }

  //template <typename T>
  //RBTreeIterator(T ptr) : _ptr(ptr) {}
private:
  //pointer _ptr;
  cNode _ptr;
};

template <typename T>
bool operator==(RBTreeIterator<T> lhs, RBTreeIterator<T> rhs) noexcept
{
  return lhs._ptr == rhs._ptr;
}

template <typename T>
bool operator!=(RBTreeIterator<T> lhs, RBTreeIterator<T> rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T>
void swap(RBTreeIterator<T> lhs, RBTreeIterator<T> rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_ITERATOR_HPP_INCLUDED
