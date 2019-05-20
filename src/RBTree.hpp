#ifndef __RBTREE_HPP_INCLUDED
#define __RBTREE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <iterator>
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
#include <queue>
#include <string>
//#include <vector>
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

#ifndef NDEBUG
  friend void testInsertion();
#endif


public:

///////////////////////////////////////////////////////////////////////////////
// member types
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = RBTreeIterator<const T>;
  using const_iterator = RBTreeIterator<const T>;
  //using reverse_iterator = std::reverse_iterator<iterator>;
  //using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using difference_type = typename iterator::difference_type;
  using size_type = std::size_t;
  
///////////////////////////////////////////////////////////////////////////////
// ctor
  RBTree() {}
  RBTree(const RBTree &other) 
    : _root(copy_node(other._root)) {
    build_prev_next(_root);
    build_begin(_root);
    build_end(_root);
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
  iterator end() noexcept {return _end;}
  const_iterator cend() const noexcept {return _end;}
  //reverse_iterator rbegin() noexcept 
  //{return std::make_reverse_iterator(begin());}
  //const_reverse_iterator crbegin() const noexcept 
  //{return std::make_reverse_iterator(cbegin());}
  //reverse_iterator rend() noexcept 
  //{return std::make_reverse_iterator(end());}
  //const_reverse_iterator crend() const noexcept 
  //{return std::make_reverse_iterator(cend());}

///////////////////////////////////////////////////////////////////////////////
// capacity
  bool empty() const noexcept {return cbegin() == cend();}
  size_type size() const noexcept {return _size;}

///////////////////////////////////////////////////////////////////////////////
// modifiers
  void clear() noexcept {
    _begin = _root = _end = nullptr;
    _size = 0;
  }

  std::pair<iterator, bool> insert(const_reference value) {
    pNode parent;
    auto find_result = find(_root, value, parent);
    if (find_result.second) return {find_result.first, false};
    pNode inserted = find_result.first = std::make_shared<Node>(value);
    inserted->parent() = parent;
    if (inserted->is_root()) {
      _begin = _root = inserted;
      _root->next() = _end = std::make_shared<Node>();
      _end->prev() = _root;
    } else {
      if (parent->left() == inserted) {
        if (parent->prev()) parent->prev()->next() = inserted;
        else _begin = inserted;
        inserted->prev() = parent->prev();
        parent->prev() = inserted;
        inserted->next() = parent;
      } else {
        parent->next()->prev() = inserted;
        inserted->next() = parent->next();
        parent->next() = inserted;
        inserted->prev() = parent;
      }
    }
    insert_repair_tree(inserted);
    ++_size;
    return {inserted, true};
  }

  void swap(RBTree &other) noexcept {
    using std::swap;
    swap(_root, other._root);
    swap(_begin, other._begin);
    swap(_end, other._end);
    swap(_size, other._size);
  }
  
///////////////////////////////////////////////////////////////////////////////
// lookup
  iterator find(const_reference value) {
    auto rtn = find(_root, value);
    return rtn.second ? rtn.first : end();
  }

  //const_iterator find(const_reference value) const {
  //  auto rtn = find(_root, value);
  //  return rtn.second ? rtn.first : cend();
  //}

#ifndef NDEBUG
///////////////////////////////////////////////////////////////////////////////
// DEBUG
  std::string serialize() const {
    static constexpr size_type SPACE = 4;
    std::queue<std::pair<size_type, cNode>> nq;
    size_type curr = 0;
    size_type next_enter = 1;
    nq.emplace(0, _root);
    std::string result = "[";
    while (!nq.empty()) {
      size_type i;
      cNode p;
      std::tie(i, p) = nq.front();
      nq.pop();
      if (i >= next_enter) {
        curr = next_enter;
        result += "\n";
        next_enter <<= 1;
        ++next_enter;
      }
      result += std::string((i - curr) * SPACE, ' ');
      curr = i + 1;
      if (p) {
          auto tmp = std::to_string(p->value()) + (p->is_black()?"B,":"R,");
          if(tmp.size() < SPACE) 
            tmp = std::string((SPACE-tmp.size()), ' ') + tmp;
          result += tmp;
          nq.emplace(++(i<<=1), p->left());
          nq.emplace(++i, p->right());
          if (p->left()) assert(p->left()->parent().lock() == p);
          if (p->right()) assert(p->right()->parent().lock() == p);
      } else
          result += "nul,";
    }
    result.pop_back();
    result += "]";
    return result;
  }

  bool check_parent() const {
    return check_parent(_root);
  }

  bool is_valid_rb_tree() const {
    return is_black(_root) && check_reds_children(_root)
      && check_black_height(_root).second;
  }

  template <typename Y>
  static bool is_red(Y n) {
    return n && n->is_red();
  }
  template <typename Y>
  static bool is_black(Y n) {
    return !is_red(n);
  }

  //int is_valid() const {
  //  size_type ctr = 0;
  //  std::vector<value_type> res(cbegin(), cend());
  //  if (size() != res.size()) return 1;
  //  for (auto it = cbegin(); it != cend(); ++it) {
  //    ++ctr;
  //  }
  //  for (auto it = cend(); it != cbegin(); --it) {
  //    ++ctr;
  //  }
  //}

#endif  //NDEBUG

private:
  pNode _root; // store above root instead of root?
  wNode _begin; // maybe _rend should not be _end
  pNode _end;
  size_type _size = 0;

///////////////////////////////////////////////////////////////////////////////
// copy ctor
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

  // build prev and next pointers, and size
  void build_prev_next(pNode curr) {
    // assume all prev and next starts at nullptr
    if (curr == nullptr) return;
    ++_size;
    if (curr->left()) {
      if (curr->prev()) curr->prev()->next() = curr->left();
      curr->left()->prev() = curr->prev();
      curr->left()->next() = curr;
      curr->prev() = curr->left();
    }
    if (curr->right()) {
      if (curr->next()) curr->next()->prev() = curr->right();
      curr->right()->next() = curr->next();
      curr->right()->prev() = curr;
      curr->next() = curr->right();
    }
    build_prev_next(curr->left());
    build_prev_next(curr->right());
  }

  void build_begin(pNode curr) {
    // assume _begin starts at nullptr
    if (!curr) return;
    _begin = curr;
    build_begin(curr->left());
  }

  void build_end(pNode curr) {
    // assume _begin starts at nullptr
    if (!curr) return;
    if (!curr->next()) {
      curr->next() = _end = std::make_shared<Node>();
      _end->prev() = curr;
      return;
    }
    build_end(curr->right());
  }

///////////////////////////////////////////////////////////////////////////////
// insertion/removal
  static void rotate_left(pNode &ptr2this) {
    pNode curr = ptr2this;
    // assert(curr);
    wNode parent = curr->parent();
    // assert(parent);
    // assert(curr->right());
    ptr2this = curr->right();
    curr->right()->parent() = parent;
    curr->right() = ptr2this->left();
    if (ptr2this->left()) ptr2this->left()->parent() = curr;
    ptr2this->left() = curr;
    curr->parent() = ptr2this;
  }
  void rotate_left(wNode curr) {
    // assert(curr);
    pNode p = curr.lock();
    wNode parent = p->parent();
    pNode &ptr2this = p->is_root()?_root:p->pointer_to_this();
    // assert(curr->right());
    ptr2this = p->right();
    p->right()->parent() = parent;
    p->right() = ptr2this->left();
    if (ptr2this->left()) ptr2this->left()->parent() = p;
    ptr2this->left() = p;
    p->parent() = ptr2this;
  }
  static void rotate_right(pNode &ptr2this) {
    pNode curr = ptr2this;
    // assert(curr);
    wNode parent = curr->parent();
    // assert(parent);
    // assert(curr->left());
    ptr2this = curr->left();
    curr->left()->parent() = parent;
    curr->left() = ptr2this->right();
    if (ptr2this->right()) ptr2this->right()->parent() = curr;
    ptr2this->right() = curr;
    curr->parent() = ptr2this;
  }
  void rotate_right(wNode curr) {
    // assert(curr);
    pNode p = curr.lock();
    wNode parent = p->parent();
    pNode &ptr2this = p->is_root()?_root:p->pointer_to_this();
    // assert(curr->left());
    ptr2this = p->left();
    p->left()->parent() = parent;
    p->left() = ptr2this->right();
    if (ptr2this->right()) ptr2this->right()->parent() = p;
    ptr2this->right() = p;
    p->parent() = ptr2this;
  }

  // insertion
  void insert_repair_tree(wNode curr) noexcept {
    // assert(curr);
    if (curr->is_root()) curr->set_black();
    else if (curr->parent()->is_red()) {
      auto uncle = curr->uncle();
      auto grandparent = curr->grandparent();
      if (uncle && uncle->is_red()) {
        curr->parent()->set_black();
        uncle->set_black();
        grandparent->set_red();
        insert_repair_tree(grandparent);
      } else {
        if (curr == curr->parent()->right() && 
            curr->parent() == grandparent->left()) {
          rotate_left(grandparent->left());
          curr = curr->left();
        } else if (curr == curr->parent()->left() && 
            curr->parent() == grandparent->right()) {
          rotate_right(grandparent->right());
          curr = curr->right();
        }
        wNode parent = curr->parent();
        wNode grandparent = parent->parent();
        if (grandparent->left() == parent)
          rotate_right(grandparent);
        else
          rotate_left(grandparent);
        parent->set_black();
        grandparent->set_red();
      }
    }
  }

///////////////////////////////////////////////////////////////////////////////
// lookup
  std::pair<pNode&, bool> find(pNode &curr, const_reference value) {
    pNode parent;
    return find(curr, value, parent);
  }

  //std::pair<cNode, bool> 
  //  find(cNode curr, const_reference value) const {
  //  return find(const_cast<pNode&>(curr), value);
  //}

  std::pair<pNode&, bool> find(pNode &curr, const_reference value, 
      pNode &parent) {
    if (!curr) return {curr, false};
    if (Compare()(curr->value(), value)) {
      parent = curr; 
      return find(curr->right(), value, parent);
    }
    if (Compare()(value, curr->value())) {
      parent = curr;
      return find(curr->left(), value, parent);
    }
    return {curr, true};
  }

///////////////////////////////////////////////////////////////////////////////
// DEBUG
  static bool check_parent(cNode n) {
    if (!n) return true;
    return (!n->left() || n->left()->parent() == n)
        && (!n->right() || n->right()->parent() == n)
        && check_parent(n->left())
        && check_parent(n->right());
  }

  static bool check_reds_children(cNode n) {
    if (!n) return true;
    if (n->is_red() && 
        ((n->left() && n->left()->is_red()) ||
         (n->right() && n->right()->is_red())))
      return false;
    return check_reds_children(n->left()) && 
           check_reds_children(n->right());
  }
  
  static std::pair<size_type, bool> check_black_height(cNode n) {
    if (!n) return {1, true};
    auto lh = check_black_height(n->left());
    if (!lh.second) return {0, false};
    auto rh = check_black_height(n->right());
    if (!rh.second || lh.first != rh.first) return {0, false};
    return {n->is_black() + lh.first, true};
  }
};

///////////////////////////////////////////////////////////////////////////////
// non-member functions
template <typename T, typename Compare>
void swap(RBTree<T, Compare> &lhs, RBTree<T, Compare> &rhs) noexcept
{
  lhs.swap(rhs);
}

#endif // __RBTREE_HPP_INCLUDED
