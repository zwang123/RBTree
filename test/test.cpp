#include <RBTree.hpp>

struct Foo {};
struct Base {};
struct Derived : Base {};

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
  const RBTree<int> crbti;// = rbti;
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
  rbti.begin();
}



int main(int, char **)
{
  testInitializer();
  testGet();
  testIterator();
  return 0;
}
