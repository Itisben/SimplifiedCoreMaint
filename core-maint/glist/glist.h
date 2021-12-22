// The @GLIST class implements the order-based core maintenance algorithm
// in the paper. There is another class called @LLIST which has slightly
// better performance but the class is not included here.
#ifndef CORE_GLIST_GLIST_H_
#define CORE_GLIST_GLIST_H_

#include "core.h"

#include "gadget/heap.h"
#include "gadget/treap.h"

namespace core {
class GLIST final: public CoreMaintenance {
 public:
  explicit GLIST(const int n);
  ~GLIST();

  void ComputeCore(std::vector<std::vector<int>>& graph,
                   const bool init_idx,
                   std::vector<int>& core);
  void Insert(const int v1, const int v2,
              std::vector<std::vector<int>>& graph,
              std::vector<int>& core);
  void Remove(const int v1, const int v2,
              std::vector<std::vector<int>>& graph,
              std::vector<int>& core);
  void Check(const std::vector<std::vector<int>>& graph,
             const std::vector<int>& core) const;

 private:
  struct ListNode {
    int rem;
    int ext;
    int prev;
    int next;
  };

  void Keep(const std::vector<std::vector<int>>& graph,
            const int v, const int K,
            const std::vector<int>& core,
            int& list_t, std::vector<int>& swap);
  void PropagateDismissal(const std::vector<std::vector<int>>& graph,
                          const int K, const int v,
                          std::vector<int>& core,
                          std::vector<int>& to_be_clear,
                          std::vector<int>& changed);
  int GetRank(const int v) {
    if (0 == rank_[v]) {
      rank_[v] = tree_.Rank(v);
      garbage_.push_back(v);
    }
    return rank_[v];
  }

  int n_;
  std::vector<int> head_;
  std::vector<int> tail_;
  std::vector<ListNode> node_;
  std::vector<int> mcd_;
  std::vector<int> deg_;
  std::vector<int> rank_;
  std::vector<int> root_;
  std::vector<bool> evicted_;
  std::vector<bool> visited_;
  gadget::Treap tree_;
  gadget::MinHeap heap_;
  std::vector<int> garbage_;
};
}  // namespace core

#endif
