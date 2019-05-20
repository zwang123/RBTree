#include <cstddef>
#include <random>
#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <RBTree.hpp>

struct Foo {};
struct Base {};
struct Derived : Base {};

using std::cout;
using std::set;
using std::endl;

void testInitializer()
{
  RBTree<int> rbti;
  RBTree<Foo> rbtf;
  RBTree<Base> rbtb;
  RBTree<Derived> rbtd;
  RBTreeNode<int> rbtni;
  RBTreeNode<Foo> rbtnf;
  RBTreeNode<Base> rbtnb;
  RBTreeNode<Derived> rbtnd;
}

void testGet()
{
  RBTree<int> rbti;
  rbti.root();
  const RBTree<int> crbti = rbti;
  crbti.root();

  RBTreeNode<int> rbtni;
  rbtni.left();
  rbtni.right();
  const RBTreeNode<int> crbtni;
  crbtni.left();
  crbtni.right();
}

void testIterator()
{
  RBTree<int> rbti;
  assert(rbti.size() == 0);
  assert(rbti.empty());

  for (int i = 5; i >= 0; --i) {
    rbti.insert(i);
    cout << rbti.serialize() << endl;
  }
  assert(rbti.size() == 6);
  assert(!rbti.empty());

  {
    int idx = 0;
    for (auto it = rbti.begin(); it != rbti.end(); ++it) {
      cout << *it << ' ';
      assert(*it == idx++);
    }
    cout << endl;
  }

  {
    int idx = rbti.size();
    for (auto it = rbti.end(); it != rbti.begin();) {
      //cout << *--it << ' ';
      assert(*--it == --idx);
    }
    //cout << endl;
  }

  const RBTree<int> rbti2 (rbti);
  for (auto it = rbti2.cbegin(); it != rbti2.cend(); ++it) {
    cout << *it << ' ';
  }
  cout << endl;

  {
    RBTree<int> rbtit (rbti);
    RBTree<int> rbti3 (std::move(rbtit));
    cout << rbti3.size() << endl;
    for (auto it = rbti3.cbegin(); it != rbti3.cend(); ++it) {
      cout << *it << ' ';
    }
    cout << endl;
  }

  {
    RBTree<int> rbtit (rbti);
    RBTree<int> rbti3;
    rbti3 = std::move(rbtit);
    cout << rbti3.size() << endl;
    for (auto it = rbti3.cbegin(); it != rbti3.cend(); ++it) {
      cout << *it << ' ';
    }
    cout << endl;
  }

  {
    using std::swap;
    RBTree<int> rbti3;
    cout << rbti3.size() << endl;
    swap(rbti3, rbti);
    cout << rbti3.size() << endl;
    cout << rbti.size() << endl;
    for (auto it = rbti3.cbegin(); it != rbti3.cend(); ++it) {
      cout << *it << ' ';
    }
    cout << endl;
    swap(rbti3, rbti);
  }
}

void testInsertion()
{
  RBTree<int> rbti;
  using pNode = RBTree<int>::pNode;
  pNode p;
  std::pair<pNode, bool> fr = rbti.find(rbti._root, 4, p);
  cout << fr.first << ' ' << fr.second << ' ' << p << endl;
  auto ir = rbti.insert(4);
  cout << ir.first.lock() << ' ' << ir.second << endl;
  cout << rbti._root << ' ' 
       << rbti._begin.lock() << ' '
       << rbti._end << ' '
       << rbti._size << ' '
       << endl;
  cout << rbti._begin->value() << ' '
       << rbti._end->value() << ' '
       << endl;

  p.reset();
  fr = rbti.find(rbti._root, 3, p);
  cout << fr.first << ' ' << fr.second << ' ' << p << endl;
  p.reset();
  fr = rbti.find(rbti._root, 4, p);
  cout << fr.first << ' ' << fr.second << ' ' << p << endl;
  p.reset();
  fr = rbti.find(rbti._root, 5, p);
  cout << fr.first << ' ' << fr.second << ' ' << p << endl;

  ir = rbti.insert(3);
  cout << ir.first.lock() << ' ' << ir.second << endl;
  cout << rbti._root << ' ' 
       << rbti._begin.lock() << ' '
       << rbti._end << ' '
       << rbti._size << ' '
       << endl;
  cout << rbti._begin->value() << ' '
       << rbti._end->value() << ' '
       << endl;
  cout << rbti._root->left() << ' '
       << rbti._root->right() << ' '
       << endl;

  //const RBTree<int> crbti(rbti);
  //crbti.find(3);
}

void check_validity(set<int> &si, RBTree<int> &rbti)
{
  assert(std::equal(si.begin(), si.end(), rbti.begin()));
  {
    auto its = si.rbegin();
    for (auto it = rbti.cend(); it != rbti.cbegin();)
      assert(*--it == *its++);
  }
  //assert(std::equal(si.rbegin(), si.rend(), rbti.rbegin()));
  assert(si.size() == rbti.size());
  assert(si.empty() == rbti.empty());
}


void testRandomInsertion(std::size_t num)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, 100);

  RBTree<int> rbti;
  std::set<int> si;
  check_validity(si, rbti);
  for (std::size_t i = 0; i != num; ++i) {
    auto x = dist(mt);
    rbti.insert(x);
    si.insert(x);


    check_validity(si, rbti);
    //cout << rbti.serialize() << endl;
    assert(rbti.is_valid_rb_tree());

    RBTree<int> rbti2(rbti);
    check_validity(si, rbti2);
    rbti2 = rbti2;
    check_validity(si, rbti2);
    RBTree<int> rbti3;
    rbti3 = rbti2;
    check_validity(si, rbti3);
    RBTree<int> rbti4(std::move(rbti3));
    check_validity(si, rbti4);
    rbti2 = std::move(rbti4);
    check_validity(si, rbti2);
  }

  for (std::size_t i = 0; i != num; ++i) {
    auto x = dist(mt);
    auto it = rbti.find(x);
    assert((si.find(x) == si.end()) ==
                  (it == rbti.cend()));
    assert(it == rbti.end() || *it == x);
  }

  RBTree<int> rbti2(rbti);
  std::set<int> si2(si);

  rbti.clear();
  si.clear();
  check_validity(si, rbti);
  check_validity(si2, rbti2);

  using std::swap;
  swap(rbti, rbti2);
  swap(si, si2);
  check_validity(si, rbti);
  check_validity(si2, rbti2);

}


int main(int, char **)
{
  testInitializer();
  testGet();
  //testInsertion();
  testIterator();
  testRandomInsertion(100);
  return 0;
}
