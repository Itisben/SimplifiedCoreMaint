// The @Treap class implements the treap structure.
#ifndef CORE_GADGET_TREAP_H_
#define CORE_GADGET_TREAP_H_

#include <vector>

namespace gadget {
class Treap final {
 public:
  explicit Treap(const int n);

  void Insert(const int x, const bool f, int& r);
  void InsertAfter(const int x, const int y, int& r);
  void Delete(const int x, int& r);
  int Merge(const int r1, const int r2);
  int Rank(const int x) const;
  int Select(const int r, const int rank) const;
  int Root(const int x) const;
  int Minimum(const int x) const;
  int Maximum(const int x) const;
  int Size(const int r) const;
  void Check(const int r) const;

 private:
  struct TreapNode final {
    int p;  // the parent
    int l;  // the left child
    int r;  // the right child
    int s;  // the size of the subtree rooted at this node
    int w;  // the priority
  };

  void LeftRotate(int& x);
  void RightRotate(int& x);
  void SubCheck(const int x) const;

  int n_;
  std::vector<TreapNode> nd_;
};
}  // namespace gadget

#endif
