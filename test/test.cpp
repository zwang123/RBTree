#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <set>
#include <unordered_set>
#include <utility>
#include <RBTree.hpp>

struct Foo {};
struct Base {};
struct Derived : Base {};

using std::cout;
using std::ostream;
using std::set;
using std::endl;
//using std::chrono;

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
    //cout << rbti.serialize() << endl;
    cout << rbti << endl;
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
  //     << rbti._rend << ' '
  //     << rbti._rend->next().lock() << ' '
       << rbti._end << ' '
       << rbti._size << ' '
       << endl;
  cout << *rbti.begin() << ' '
       << *rbti.end() << ' '
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
  //     << rbti._rend << ' '
  //     << rbti._rend->next().lock() << ' '
       << rbti._end << ' '
       << rbti._size << ' '
       << endl;
  cout << *rbti.begin() << ' '
       << *rbti.end() << ' '
       << endl;
  cout << rbti._root->left() << ' '
       << rbti._root->right() << ' '
       << endl;

  //const RBTree<int> crbti(rbti);
  //crbti.find(3);
}

void check_validity(set<int> &si, RBTree<int> &rbti)
{
  //cout << __LINE__ << endl;
  assert(std::equal(si.begin(), si.end(), rbti.begin()));
  //cout << __LINE__ << endl;
  //{
  //  auto its = si.rbegin();
  //  for (auto it = rbti.cend(); it != rbti.cbegin();)
  //    assert(*--it == *its++);
  //}
  assert(std::equal(si.rbegin(), si.rend(), rbti.rbegin()));
  //cout << __LINE__ << endl;
  assert(si.size() == rbti.size());
  //cout << __LINE__ << endl;
  assert(si.empty() == rbti.empty());
  //cout << __LINE__ << endl;
}


void testRandomInsertion(std::size_t num)
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, num);

  RBTree<int> rbti;
  std::set<int> si;
  check_validity(si, rbti);

  std::vector<RBTree<int>::iterator> vitrbti;
  std::vector<std::set<int>::iterator> vitsi;

  for (std::size_t i = 0; i != num; ++i) {
    auto x = dist(mt);
    auto ir1 = rbti.insert(x);
    if (ir1.second) vitrbti.push_back(ir1.first);
    auto ir2 = si.insert(x);
    if (ir2.second) vitsi.push_back(ir2.first);

    assert(std::equal(vitrbti.begin(), vitrbti.end(),
                      vitsi.begin(), [](const auto &l, const auto &r)
                      {return *l==*r;}));


    check_validity(si, rbti);
    //cout << rbti.serialize() << endl;
    assert(rbti.check_parent());
    assert(rbti.is_valid_rb_tree());

    if (i %200 == 0) {
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

template <typename T>
std::size_t benchmark(std::size_t num) {
  T rbti;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<typename T::value_type> dist(0, num);
  auto beg = std::chrono::high_resolution_clock::now();
  for (std::size_t i = 0; i != num; ++i) {
    rbti.insert(dist(mt));
  }
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>
    (end-beg).count();
}

template <typename T>
std::size_t benchmark_removal(T &rbti, 
    std::vector<typename T::iterator> vit) {
  auto beg = std::chrono::high_resolution_clock::now();
  for (auto it : vit) {
    rbti.erase(it);
  }
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>
    (end-beg).count();
}

void benchmark() {
  for (std::size_t num = 1000; num < 50000; num *= 2) {
    double result = benchmark<RBTree<int>>(num);
    cout << num << " " 
         << result << "us "
         << result / static_cast<double>(num) << " "
         << result / static_cast<double>(num) / log2(num) << " "
         << result / static_cast<double>(num) / num << " "
         << endl;
  }
}

template <typename T>
std::vector<typename T::iterator> build_iterator_vector(T& container, 
    const std::vector<std::size_t> &idx)
{
  if (container.empty()) return {};
  std::vector<typename T::iterator> rtn;
  for (const auto i : idx) {
    auto it = container.begin();
    std::advance(it, i);
    rtn.push_back(std::move(it));
  }
  return std::move(rtn);
}

template <typename T>
std::vector<typename T::iterator> build_iterator_vector(T& container)
{
  using std::swap;

  if (container.empty()) return {};
  std::vector<typename T::iterator> rtn(container.size());
  std::iota(rtn.begin(), rtn.end(), container.begin());

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, container.size()-1);
  for (std::size_t i = 0; i != container.size(); ++i) {
    if (dist(mt) % 5 < 3) {
      //60%
      swap(rtn[dist(mt)], rtn[dist(mt)]);
    } else {
      rtn[dist(mt)] = container.end();
    }
  }
  auto end = std::remove_if(rtn.begin(), rtn.end(),
      [&container](const auto &it){return it==container.end();});
  rtn.erase(end, rtn.end());
  return rtn;
}

template <typename T, typename U>
std::pair<std::vector<typename T::iterator>, 
          std::vector<typename U::iterator>>
  build_iterator_vector(T& c1, U& c2)
{
  using std::swap;

  assert(c1.size() == c2.size());
  if (c1.empty()) return {};

  std::vector<typename T::iterator> rtn1(c1.size());
  std::vector<typename U::iterator> rtn2(c1.size());

  std::iota(rtn1.begin(), rtn1.end(), c1.begin());
  std::iota(rtn2.begin(), rtn2.end(), c2.begin());

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, c1.size()-1);

  for (std::size_t k = 0; k != c1.size(); ++k) {
    if (dist(mt) % 5 < 3) {
      //60%
      auto i = dist(mt), j = dist(mt);
      swap(rtn1[i], rtn1[j]);
      swap(rtn2[i], rtn2[j]);
    } else {
      auto i = dist(mt);
      rtn1[i] = c1.end();
      rtn2[i] = c2.end();
    }
  }

  auto end1 = std::remove_if(rtn1.begin(), rtn1.end(),
      [&c1](const auto &it){return it==c1.end();});
  rtn1.erase(end1, rtn1.end());
  auto end2 = std::remove_if(rtn2.begin(), rtn2.end(),
      [&c2](const auto &it){return it==c2.end();});
  rtn2.erase(end2, rtn2.end());

  return {std::move(rtn1), std::move(rtn2)};
}

template <typename T>
void remove_duplicate(T &c)
{
  std::unordered_set<typename T::value_type> s;
  auto end = std::remove_if(c.begin(), c.end(),
      [&s](const auto &i) {return !s.insert(i).second;});
  c.erase(end, c.end());
}

void benchmark_removal() {
  for (std::size_t num = 1000; num < 50000; num *= 2) {
  //for (std::size_t num = 50; num < 5000000; num *= 2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, num);

    RBTree<int> rbti;
    std::set<int> si;
    for (std::size_t i = 0; i != num; ++i) {
      auto x = dist(mt);
      rbti.insert(x);
      si.insert(x);
    }

    auto setsize = si.size();
    auto itv = build_iterator_vector(rbti, si);
    auto n = itv.first.size();
    double nlogn = setsize * log2(setsize) - (setsize-n) * log2(setsize-n) -
                   n * log2(std::exp(1));
    cout << num << " " << setsize << " " << n << " " << std::endl;

    double resultrbti = benchmark_removal(rbti, itv.first);
    //cout << __LINE__ << std::endl;
    cout << "\t";
    cout << resultrbti << "us "
         << resultrbti / static_cast<double>(n) << " "
         << resultrbti / nlogn << " "
         << resultrbti / static_cast<double>(n) / n << " ";
    cout << std::endl;
    double resultsi = benchmark_removal(si, itv.second);
    //cout << __LINE__ << std::endl;
    cout << "\t";
    cout << resultsi << "us "
         << resultsi / static_cast<double>(n) << " "
         << resultsi / nlogn << " "
         << resultsi / static_cast<double>(n) / n << " ";
    cout << std::endl;
    
    cout << "\t\tmultiplier ";
    cout << resultrbti/resultsi;
    cout << endl;

    //std::vector<std::size_t> idx;
    //for (std::size_t i = 0; i != si.size(); ++i) {
    //  idx.push_back(dist(mt)%si.size());
    //}
    //remove_duplicate(idx);

    //{
    //for (auto xx:idx) cout << xx << ' ';
    //cout <<endl;
    //}


    //double result, resultrbt;

    //{
    ////cout << __LINE__ << std::endl;
    //auto itv = build_iterator_vector(si);
    //auto n = itv.size();
    //cout << num << " " << setsize << " " << n << " ";
    ////cout << __LINE__ << std::endl;
    //result = benchmark_removal(si, itv);
    ////cout << __LINE__ << std::endl;
    //double nlogn = setsize * log2(setsize) - (setsize-n) * log2(setsize-n) -
    //               n * log2(std::exp(1));
    //cout << result << "us "
    //     << result / static_cast<double>(n) << " "
    //     << result / nlogn << " "
    //     << result / static_cast<double>(n) / n << " ";
    //cout << std::endl;
    //}
    //{
    ////cout << __LINE__ << std::endl;
    ////cout << __LINE__ << std::endl;
    //result = benchmark_removal(rbti, itv.first);
    ////cout << __LINE__ << std::endl;
    //cout << result << "us "
    //     << result / static_cast<double>(n) << " "
    //     << result / nlogn << " "
    //     << result / static_cast<double>(n) / n << " ";
    //cout << std::endl;
    //}

    //cout << "RBTree ";
    //result = benchmark_removal(rbti, build_iterator_vector(rbti, idx));
    //resultrbt = result;
    //cout << result << "us "
    //     << result / static_cast<double>(num) << " "
    //     << result / static_cast<double>(num) / log2(num) << " "
    //     << result / static_cast<double>(num) / num << " ";
    //cout << "std::set ";
    //result = benchmark_removal(si, build_iterator_vector(si, idx));
    //cout << result << "us "
    //     << result / static_cast<double>(num) << " "
    //     << result / static_cast<double>(num) / log2(num) << " "
    //     << result / static_cast<double>(num) / num << " ";

    //cout << "multiplier ";
    //cout << resultrbt/result;
    //cout << endl;
  }
}

ostream &output(std::size_t num = 30, ostream &os = std::cout) {
  RBTree<int> rbti;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, num);
  for (std::size_t i = 0; i != num; ++i) {
    rbti.insert(dist(mt));
  }
  return os << rbti << std::endl;
}

void testRemoval() {
  RBTree<int> rbti;
  for (std::size_t i = 0; i != 50; ++i) {
    rbti.insert(i);
  }
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, 49);
  for (std::size_t i = 0; i != 20; ++i) {
    rbti.erase(dist(mt));

    assert(rbti.check_parent());
    assert(rbti.is_valid_rb_tree());
  }
}

void testRandomRemoval(std::size_t num) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, num);

  RBTree<int> rbti;
  std::set<int> si;
  for (std::size_t i = 0; i != num; ++i) {
    auto x = dist(mt);
    rbti.insert(x);
    si.insert(x);
  }

  std::size_t empty_counter = 0;
  for (std::size_t i = 0; empty_counter != 10; ++i) {
    //auto x = dist(mt);
    int x;
    if (rbti.empty()) {
      x = dist(mt);
      ++empty_counter;
    } else {
      auto it = rbti.begin();
      //for (auto it = rbti.begin(); it != rbti.end(); ++it) {
      //  std::cout << it.lock() << std::endl << " " << *it << std::endl;
      //}
      std::advance(it, dist(mt)%rbti.size());
      x = *it + dist(mt) % 2;
    }
    //cout << rbti << endl;
    //cout << i << " delete " << x << endl;
    //cout << __LINE__ << endl;
    rbti.erase(x);
    //cout << rbti << endl;
    //cout << *rbti.begin() << endl;
    //cout << *rbti.rbegin() << endl;
    //cout << __LINE__ << endl;
    si.erase(x);
    //cout << __LINE__ << endl;
    assert(rbti.check_parent());
    //cout << __LINE__ << endl;
    assert(rbti.is_valid_rb_tree());
    //cout << __LINE__ << endl;
    check_validity(si, rbti);
    //cout << __LINE__ << endl;
  }

  for (std::size_t i = 0; i != num; ++i) {
    auto x = dist(mt);
    rbti.insert(x);
    si.insert(x);
  }
  auto itp = build_iterator_vector(rbti, si);
  //decltype(rbti.begin()) itrbti;
  //decltype(si.begin()) itsi;
  for (std::size_t i = 0; i != itp.first.size(); ++i) {
    rbti.erase(itp.first[i]);
    si.erase(itp.second[i]);
    assert(rbti.check_parent());
    assert(rbti.is_valid_rb_tree());
    check_validity(si, rbti);
  }
}



int main(int, char **)
{
  testInitializer();
  testGet();
  testInsertion();
  testIterator();
  testRandomInsertion(10000);
  testRandomRemoval(10000);
  benchmark();
  benchmark_removal();
  output();
  return 0;
}
