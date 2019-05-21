#ifndef __RBTREE_HPP_INCLUDED
#define __RBTREE_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <iostream>
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
#include <queue>
#include <string>
//#include <vector>
#endif

template <typename T, typename Compare = std::less<T>, typename Enable = void>
class RBTree;
template <typename T, typename Compare>
std::ostream& operator<<(std::ostream &, const RBTree<T, Compare> &);

template <typename T, typename Compare>
class RBTree<T, Compare, 
      typename std::enable_if<std::is_assignable<T&, T>::value
      && !std::is_reference<T>::value>::type> {

  using Node = RBTreeNode<T>;
  using pNode = std::shared_ptr<Node>;
  using cNode = std::shared_ptr<const Node>;
  using wNode = RBTreeNodePointer<T>;

  friend std::ostream& operator<< <> (std::ostream &, const RBTree &);
#ifndef NDEBUG
  friend void testInsertion();
#endif


public:

///////////////////////////////////////////////////////////////////////////////
// member types
  using value_type = T;
  using value_compare = Compare;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = RBTreeIterator<const T>;
  using const_iterator = RBTreeIterator<const T>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using difference_type = typename iterator::difference_type;
  
///////////////////////////////////////////////////////////////////////////////
// ctor
  explicit RBTree(const Compare& comp = Compare()) : _comp(comp) {}
  template <class InputIt>
  RBTree( InputIt first, InputIt last, const Compare& comp = Compare())
    : _comp(comp) {
    insert(first, last);
  }
  RBTree(const RBTree &other) 
    : _root(copy_node(other._root)), _comp(other._comp) {
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
  reverse_iterator rbegin() noexcept 
  {return std::make_reverse_iterator(end());}
  const_reverse_iterator crbegin() const noexcept 
  {return std::make_reverse_iterator(cend());}
  reverse_iterator rend() noexcept 
  {return std::make_reverse_iterator(begin());}
  const_reverse_iterator crend() const noexcept 
  {return std::make_reverse_iterator(cbegin());}

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

  template <class InputIt>
  void insert( InputIt first, InputIt last ) {
    while (first != last) insert(*first++);
  }

  iterator erase(iterator pos) {
    pNode p = std::const_pointer_cast<Node>(pos.lock());

    //assert(p)
    // prev/next
    wNode next = p->next();
    next->prev() = p->prev();
    (p->prev()?p->prev()->next():_begin) = next;

    // empty()
    if (_begin == _end) {clear(); return end();}

    if (p->leaf_child_count() == 0) {
      swap_but_value_prev_next(p, p->prev().lock());
    }
    //assert(check_parent());
    //assert(p->leaf_child_count() >= 0);
    
    if (p->is_red()) {
      // assert(p->leaf_child_count() == 2);
      // assert(!p->is_root());
      p->pointer_to_this() = nullptr;
    } else {
      pNode child = p->left()?p->left():p->right();
      if (child) {
        //assert(child->is_red());
        child->parent() = p->parent();
        child->set_black();
      } else erase_repair_tree(p);
      pointer_to_this(p) = child;
      // p is deleted!
    }

    --_size;
    return next;
  }

  iterator erase(const_iterator first, const_iterator last) {
    while (first != last) erase(first++);
    return last;
  }

  size_type erase(const_reference value) {
    auto it = find(value);
    if (it == end()) return 0;
    erase(it);
    return 1;
  }

  void swap(RBTree &other) noexcept {
    using std::swap;
    swap(_root, other._root);
    swap(_begin, other._begin);
    swap(_end, other._end);
    swap(_size, other._size);
    swap(_comp, other._comp);
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
  
///////////////////////////////////////////////////////////////////////////////
// observers
value_compare value_comp() const {return _comp;}

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
  Compare _comp;

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
  pNode &pointer_to_this(pNode p) {
    // assert(p);
    return p->is_root()?_root:p->pointer_to_this();
  }

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
    pNode &ptr2this = pointer_to_this(p);
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
    pNode &ptr2this = pointer_to_this(p);
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
  
  // assert(p)
  // assert(p->is_black())
  // path starting from non-empty pointer p is missing one black node,
  // fix it
  void erase_repair_tree(wNode curr) noexcept {
    erase_repair_tree(curr.lock());
  }
  // deletion
  void erase_repair_tree(pNode p) noexcept {
    using std::swap;
    if (p->is_root()) return;

    // assert(p->parent() && p->is_black());
    if (p->parent()->left() == p) {
      pNode sib = p->parent()->right();
      if(is_red(sib)) {
        //assert(sib && p->parent()->is_black());
        rotate_left(p->parent());
        swap(p->parent()->color(), sib->color());
      }
      sib = p->parent()->right();
      //assert(sib->is_black());
      if (erase_repair_tree_34(p, sib)) return;
      if (is_black(sib->right())) {
        // assert(is_red(sib->left()));
        rotate_right(p->parent()->right());
        swap(sib->parent()->color(), sib->color());
      }
      sib = p->parent()->right();
      // assert(is_red(sib->right()));
      sib->right()->set_black();
      swap(sib->parent()->color(), sib->color());
      rotate_left(p->parent());
    } else {
      pNode sib = p->parent()->left();
      if(is_red(sib)) {
        //assert(sib && p->parent()->is_black());
        rotate_right(p->parent());
        swap(p->parent()->color(), sib->color());
      }
      sib = p->parent()->left();
      //assert(sib->is_black());
      if (erase_repair_tree_34(p, sib)) return;
      if (is_black(sib->left())) {
        // assert(is_red(sib->right()));
        rotate_left(p->parent()->left());
        swap(sib->parent()->color(), sib->color());
      }
      sib = p->parent()->left();
      // assert(is_red(sib->left()));
      sib->left()->set_black();
      swap(sib->parent()->color(), sib->color());
      rotate_right(p->parent());
    }
  }
  // CASE 3-4
  bool erase_repair_tree_34(pNode p, pNode sib) noexcept {
    // assert(p->parent() && p->is_black());
    // assert(sib->is_black());
    if (sib && is_black(sib->left()) && is_black(sib->right())) {
      sib->set_red();
      if (p->parent()->is_black())
        erase_repair_tree(p->parent());
      else
        p->parent()->set_black();
      return true;
    }
    return false;
  }

  // swap everything except value and prev/next
  void swap_but_value_prev_next(pNode lhs, pNode rhs) {
    //if (lhs == rhs) return;

    using std::swap;
    
    // swap color
    swap(lhs->color(), rhs->color());

    // swap child
    wNode lparent = lhs, rparent = rhs;
    swap(
    lhs->left()?lhs->left()->parent():lparent,
    rhs->left()?rhs->left()->parent():rparent);
    lparent = lhs; rparent = rhs;
    swap(
    lhs->right()?lhs->right()->parent():lparent,
    rhs->right()?rhs->right()->parent():rparent);
    swap(lhs->left(), rhs->left());
    swap(lhs->right(), rhs->right());

    // swap parent
    swap(pointer_to_this(lhs), pointer_to_this(rhs));
    swap(lhs->parent(), rhs->parent());
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
    if (_comp(curr->value(), value)) {
      parent = curr; 
      return find(curr->right(), value, parent);
    }
    if (_comp(value, curr->value())) {
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

//#include <iostream>
#include <queue>
#include <string>
template <typename T, typename Compare>
std::ostream& operator<<(std::ostream &os, const RBTree<T, Compare> &rbt)
{
  using size_type = typename RBTree<T, Compare>::size_type;
  using cNode = typename RBTree<T, Compare>::cNode;

  static constexpr size_type SPACE = 4;
  std::queue<std::pair<size_type, cNode>> nq;
  size_type curr = 0;
  size_type next_enter = 1;
  nq.emplace(0, rbt._root);
  std::vector<std::string> lines;
  std::string line;
  while (!nq.empty()) {
    size_type i;
    cNode p;
    std::tie(i, p) = nq.front();
    nq.pop();
    if (i >= next_enter) {
      curr = next_enter;
      line.pop_back();
      lines.push_back(std::move(line));
      line.clear();
      next_enter <<= 1;
      ++next_enter;
    }
    line += std::string((i - curr) * SPACE, ' ');
    curr = i + 1;
    if (p) {
      auto tmp = std::to_string(p->value()) + (p->is_black()?"B,":"R,");
      if(tmp.size() < SPACE) 
        tmp = std::string((SPACE-tmp.size()), ' ') + tmp;
      line += tmp;
      nq.emplace(++(i<<=1), p->left());
      nq.emplace(++i, p->right());
    } else line += "nul,";
  }
  // last line is all nullptr
  //line.pop_back();
  //lines.push_back(std::move(line));

  os << "[";
  if (lines.empty()) return os << "nul]";
  os << std::endl;

  //size_type maxLen = 1;
  //for (size_type i = 1; i != lines.size(); ++i)
  //  maxLen *= 2;
  //for (size_type i = 0, x = 1; i != lines.size(); ++i) {
  //  os << std::string(SPACE / 2 * (maxLen - x), ' ') << lines[i] << std::endl;
  //  x *= 2;
  //}
  size_type maxLen = 1;
  for (size_type i = 1; i != lines.size(); ++i)
    maxLen *= 2;
  for (const auto &l : lines) {
    std::string delim (SPACE / 2 * (maxLen - 1), ' ');
    for (size_type pos = 0; pos < l.size(); pos += SPACE) {
      os << delim << l.substr(pos, SPACE) << delim;
    }
    maxLen /= 2;
    os << std::endl;
  }
  return os << "]";
}

#endif // __RBTREE_HPP_INCLUDED
