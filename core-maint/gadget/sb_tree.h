// The @SBTree class implements the size-balanced tree.
#ifndef CORE_GADGET_SB_TREE_H_
#define CORE_GADGET_SB_TREE_H_

#include <vector>

namespace gadget {
class SBTree final {
 public:
  explicit SBTree(const int n);

  void Insert(const int z, const bool f, int& r);
  void InsertAfter(const int z, const int v, int& r);
  void Delete(const int z, int& r);
  int Rank(const int x) const;
  int Size(const int r) const;
  void Check(const int r) const;

 private:
  struct SBTreeNode final {
    int p;  // the parent
    int l;  // the left child
    int r;  // the right child
    int s;  // the size of the subtree rooted at this node
  };

  void LeftRotate(int& x);
  void RightRotate(int& x);
  void Maintain(const bool f, int& x);
  void DeleteMin(int& x, int& v);
  void SubCheck(const int x) const;

  int n_;
  std::vector<SBTreeNode> nd_;
};
}  // namespace gadget

#endif
