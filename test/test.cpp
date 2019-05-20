#include <cassert>
#include <iostream>
#include <utility>
#include <RBTree.hpp>

struct Foo {};
struct Base {};
struct Derived : Base {};

using std::cout;
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



int main(int, char **)
{
  testInitializer();
  testGet();
  testIterator();
  return 0;
}
