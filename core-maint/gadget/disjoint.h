// The @DisjointSets class implements a union-find-delete structure with
// - O(log n) find operation
// - O(1) union operation
// - O(1) delete operation
// - O(\alpha(m)) amortized complexity where m is the # of operations and
//   \alpha is the inverse Ackermann function
#ifndef CORE_GADGET_DISJOINT_H_
#define CORE_GADGET_DISJOINT_H_

#include <vector>

namespace gadget {
class DisjointSets final {
 public:
  explicit DisjointSets(const int n);

  int MakeSet(const std::vector<int>& items);
  int Find(const int item);
  int Union(const int root_item1, const int root_item2);
  void Delete(const int item);

 private:
  // primitive methods for manipulating the list
  void ListInit(const int x,
                std::vector<int>& prev,
                std::vector<int>& next);
  void ListInsertBefore(const int x, const int y,
                        std::vector<int>& prev,
                        std::vector<int>& next);
  void ListInsertAfter(const int x, const int y,
                       std::vector<int>& prev,
                       std::vector<int>& next);
  void ListRemove(const int x,
                  std::vector<int>& prev,
                  std::vector<int>& next);
  void DFSListInsertBefore(const int bx, const int ex, const int y);
  void DFSListInsertAfter(const int bx, const int ex, const int y);
  void DFSListRemove(const int bx, const int ex);
  // higher-level methods for manipulating the child list and the DFS list
  void InsertChild(const int x, const int y);
  void DeleteChild(const int x, const int y);
  // methods for manipulating union-find-delete structure
  void Unhang(const int x, const int y);
  void Hang(const int x, const int y);
  void Relink(const int x, const int y, const int z);
  void CreateLeaf(const int item, const int node);
  int FindLeaf(const int x) const;
  // other gadget methods
  bool IsRoot(const int x) const;
  bool IsLeaf(const int x) const;
  bool IsReduced(const int r) const;
  bool IsOnlyChild(const int x) const;
  bool IsOnlyNLChild(const int x) const;
  bool IsSmall(const int r) const;

  std::vector<int> nl_child_;    // the first non-leaf child (valid for roots)
  std::vector<int> nl_prev_;     // the next non-leaf sibling
  std::vector<int> nl_next_;     // the last non-leaf sibling
  std::vector<int> child_;       // the first child
  std::vector<int> prev_;        // the last sibling
  std::vector<int> next_;        // the next sibling
  std::vector<int> dfs_prev_;    // the last node in the DFS list
  std::vector<int> dfs_next_;    // the next node in the DFS list
  std::vector<int> parent_;      // the parent
  std::vector<int> rank_;        // ranks of nodes
  std::vector<int> to_item_;     // given a node, find the item
  std::vector<int> to_node_;     // given an item, find the node
  std::vector<int> free_nodes_;  // nodes that are free
  int n_;
};
}  // namespace gadget

#endif
