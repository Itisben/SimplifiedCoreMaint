// The @Traversal class implements the traversal algorithm proposed in
// the paper "streaming algorithms for k-core decomposition". The code
// for the journal version will be updated soon.
#ifndef CORE_TRAVERSAL_TRAVERSAL_H_
#define CORE_TRAVERSAL_TRAVERSAL_H_

#include "core.h"

namespace core {
class Traversal final: public CoreMaintenance {
 public:
  explicit Traversal(const int n);
  ~Traversal();

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
  void PropagateEviction(const std::vector<std::vector<int>>& graph,
                         const int K, const int v,
                         const std::vector<int>& core,
                         std::vector<int>& to_be_clear);
  void PropagateDismissal(const std::vector<std::vector<int>>& graph,
                          const int K, const int v,
                          std::vector<int>& core,
                          std::vector<int>& to_be_clear);
  void UpdateRCD(const std::vector<std::vector<int>>& graph,
                 const int lb, const int ub,
                 const std::vector<int>& core,
                 std::vector<int>& changed);

  int n_;
  std::vector<int> deg_;
  std::vector<int> mcd_;
  std::vector<int> pcd_;
  std::vector<bool> evicted_;
  std::vector<bool> visited_;
};
}  // namespace core

#endif
