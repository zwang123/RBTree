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
  for (int i = 5; i >= 0; --i) {
    rbti.insert(i);
  }
  for (auto it = rbti.begin(); it != rbti.end(); ++it) {
    cout << *it << ' ';
  }
  cout << endl;
  const RBTree<int> rbti2 (rbti);
  for (auto it = rbti2.cbegin(); it != rbti2.cend(); ++it) {
    cout << *it << ' ';
  }
  cout << endl;
  for (auto it = rbti.end(); it != rbti.begin();) {
    cout << *--it << ' ';
  }
  cout << endl;
  RBTree<int> rbti3 (std::move(rbti));
  cout << rbti3.size() << endl;
  for (auto it = rbti3.cbegin(); it != rbti3.cend(); ++it) {
    cout << *it << ' ';
  }
  cout << endl;
}



int main(int, char **)
{
  testInitializer();
  testGet();
  testIterator();
  return 0;
}
