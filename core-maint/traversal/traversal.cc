#include "traversal/traversal.h"

#include <algorithm>
#include <map>
#include <set>
#include <queue>

#include "defs.h"

namespace core {
Traversal::Traversal(const int n): n_(n) {
  deg_ = std::vector<int>(n_);
  mcd_ = std::vector<int>(n_);
  pcd_ = std::vector<int>(n_);
  evicted_ = std::vector<bool>(n_);
  visited_ = std::vector<bool>(n_);
}
Traversal::~Traversal() {}

void Traversal::ComputeCore(std::vector<std::vector<int>>& graph,
                            const bool init_idx,
                            std::vector<int>& core) {
  std::vector<int>& deg = core;
  int max_deg = 0;
  for (int i = 0; i < n_; ++i) {
    deg[i] = graph[i].size();
    if (deg[i] > max_deg) {
      max_deg = deg[i];
    }
  }
  std::vector<int> bin(max_deg + 1, 0);
  for (int i = 0; i < n_; ++i) {
    ++bin[deg[i]];
  }
  int start = 0;
  for (int i = 0; i <= max_deg; ++i) {
    int temp = bin[i];
    bin[i] = start;
    start += temp;
  }
  std::vector<int> vert(n_);
  std::vector<int> pos(n_);
  for (int i = 0; i < n_; ++i) {
    pos[i] = bin[deg[i]];
    vert[pos[i]] = i;
    ++bin[deg[i]];
  }
  for (int i = max_deg; i > 0; --i) {
    bin[i] = bin[i-1];
  }
  bin[0] = 0;
  for (int i = 0; i < n_; ++i) {
    const int v = vert[i];
    for (const int u : graph[v]) {
      if (deg[u] > deg[v]) {
        const int pw = bin[deg[u]];
        const int pu = pos[u];
        if (pw != pu) {
          const int w = vert[pw];
          vert[pu] = w;
          pos[w] = pu;
          vert[pw] = u;
          pos[u] = pw;
        }
        ++bin[deg[u]];
        --deg[u];
      }
    }
  }
  // initialize RCD
  if (init_idx) {
    for (int v = 0; v < n_; ++v) {
      mcd_[v] = 0;
      for (const auto u : graph[v]) {
        if (core[u] >= core[v]) {
          ++mcd_[v];
        }
      }
    }
    for (int v = 0; v < n_; ++v) {
      pcd_[v] = 0;
      for (const auto u : graph[v]) {
        if ((core[u] == core[v] && mcd_[u] > core[u]) ||
            core[u] > core[v]) {
          ++pcd_[v];
        }
      }
    }
  }
}
void Traversal::Insert(const int v1, const int v2,
                       std::vector<std::vector<int>>& graph,
                       std::vector<int>& core) {
  // update the MCD and PCD
  if (core[v2] >= core[v1]) {
    ++mcd_[v1];
    if (core[v1] + 1 == mcd_[v1]) {
      for (const auto u : graph[v1]) {
        if (core[u] == core[v1]) ++pcd_[u];
      }
    }
    ++pcd_[v1];
  }
  if (core[v1] >= core[v2]) {
    ++mcd_[v2];
    if (core[v2] + 1 == mcd_[v2]) {
      for (const auto u : graph[v2]) {
        if (core[u] == core[v2]) ++pcd_[u];
      }
    }
    ++pcd_[v2];
  }
  // update the graph
  graph[v1].push_back(v2);
  graph[v2].push_back(v1);
  // initialization
  const auto root = core[v1] >= core[v2] ? v2 : v1;
  const auto K = core[root];
  std::vector<int> to_be_clear;
  std::vector<int> S;  // stack
  // the core of traversal insertion algorithm
  S.push_back(root);
  visited_[root] = true;
  deg_[root] = pcd_[root];
  to_be_clear.push_back(root);
  // a DFS similar search process
  while (!S.empty()) {
    const auto v = S[S.size()-1]; S.pop_back();
    if (deg_[v] > K) {
      for (const auto u : graph[v]) {
        if (K == core[u] && mcd_[u] > K && !visited_[u]) {
          S.push_back(u);
          visited_[u] = true;
          deg_[u] += pcd_[u];
          to_be_clear.push_back(u);
        }
      }
    } else if (!evicted_[v]) {
      PropagateEviction(graph, K, v, core, to_be_clear);
    }
  }
  std::vector<int> changed;
  // update the core numbers and clear "visited" and "evicted"
  for (const auto v : to_be_clear) {
    if (visited_[v] && !evicted_[v]) {
      ++core[v];
      changed.push_back(v);
    }
    evicted_[v] = false;
    visited_[v] = false;
    deg_[v] = 0;
  }
  if (!changed.empty()) {
    UpdateRCD(graph, K, K + 1, core, changed);
  }
}
void Traversal::Remove(const int v1, const int v2,
                       std::vector<std::vector<int>>& graph,
                       std::vector<int>& core) {
  // remove the edge from graph
  graph[v1].erase(std::find(graph[v1].begin(), graph[v1].end(), v2));
  graph[v2].erase(std::find(graph[v2].begin(), graph[v2].end(), v1));
  // prepare the RCDs
  if (core[v1] >= core[v2]) {
    --mcd_[v2];
    if (mcd_[v2] == core[v2]) {
      for (const auto u : graph[v2]) {
        if (core[u] == core[v2]) {
          --pcd_[u];
        }
      }
    }
    if (core[v1] > core[v2]) --pcd_[v2];
  }
  if (core[v2] >= core[v1]) {
    --mcd_[v1];
    if (mcd_[v1] == core[v1]) {
      for (const auto u : graph[v1]) {
        if (core[u] == core[v1]) {
          --pcd_[u];
        }
      }
    }
    if (core[v2] > core[v1]) --pcd_[v1];
  }
  if (core[v1] == core[v2]) {
    if (mcd_[v1] >= core[v1]) --pcd_[v2];
    if (mcd_[v2] >= core[v2]) --pcd_[v1];
  }
  // set the root
  const auto root = core[v1] <= core[v2] ? v1 : v2;
  const auto K = core[root];
  // update cores
  std::vector<int> to_be_clear;
  if (core[v1] != core[v2]) {
    visited_[root] = true;
    deg_[root] = mcd_[root];
    to_be_clear.push_back(root);
    if (deg_[root] < K) {
      PropagateDismissal(graph, K, root, core, to_be_clear);
    }
  } else {
    visited_[v1] = true;
    deg_[v1] = mcd_[v1];
    to_be_clear.push_back(v1);
    if (deg_[v1] < K) {
      PropagateDismissal(graph, K, v1, core, to_be_clear);
    }
    if (!visited_[v2]) {
      visited_[v2] = true;
      deg_[v2] = mcd_[v2];
      to_be_clear.push_back(v2);
      if (deg_[v2] < K) {
        PropagateDismissal(graph, K, v2, core, to_be_clear);
      }
    }
  }
  // recompute the RCDs
  std::vector<int> changed;
  for (const auto u : to_be_clear) {
    if (evicted_[u]) changed.push_back(u);
    visited_[u] = evicted_[u] = false;
    deg_[u] = 0;
  }
  if (!changed.empty()) {
    UpdateRCD(graph, K - 1, K, core, changed);
  }
}
void Traversal::PropagateEviction(const std::vector<std::vector<int>>& graph,
                                  const int K, const int v,
                                  const std::vector<int>& core,
                                  std::vector<int>& to_be_clear) {
  evicted_[v] = true;
  for (const auto u : graph[v]) {
    if (K == core[u]) {
      --deg_[u];
      if (-1 == deg_[u]) to_be_clear.push_back(u);
      if (K == deg_[u] && !evicted_[u]) {
        PropagateEviction(graph, K, u, core, to_be_clear);
      }
    }
  }
}
void Traversal::PropagateDismissal(const std::vector<std::vector<int>>& graph,
                                   const int K, const int v,
                                   std::vector<int>& core,
                                   std::vector<int>& to_be_clear) {
  evicted_[v] = true;
  --core[v];
  for (const auto u : graph[v]) {
    if (K == core[u]) {
      if (!visited_[u]) {
        deg_[u] += mcd_[u];
        visited_[u] = true;
        to_be_clear.push_back(u);
      }
      --deg_[u];
      if (deg_[u] < K && !evicted_[u]) {
        PropagateDismissal(graph, K, u, core, to_be_clear);
      }
    }
  }
}
void Traversal::UpdateRCD(const std::vector<std::vector<int>>& graph,
                          const int lb, const int ub,
                          const std::vector<int>& core,
                          std::vector<int>& changed) {
  for (const auto v : changed) {
    visited_[v] = true;
  }
  for (int h = 0; h < 2; ++h) {
    std::vector<int> updated;
    for (const auto v : changed) {
      for (const auto u : graph[v]) {
        if (!visited_[u] && lb <= core[u] && core[u] <= ub) {
          updated.push_back(u);
          visited_[u] = true;
        }
      }
    }
    for (const auto v : updated) {
      changed.push_back(v);
    }
    if (h == 0) {
      for (const auto v : changed) {
        mcd_[v] = 0;
        for (const auto u : graph[v]) {
          if (core[u] >= core[v]) {
            ++mcd_[v];
          }
        }
      }
    } else {
      for (const auto v : changed) {
        pcd_[v] = 0;
        for (const auto u : graph[v]) {
          if ((core[u] == core[v] &&  mcd_[u] > core[u]) ||
              core[u] > core[v]) {
            ++pcd_[v];
          }
        }
      }
    }
  }
  for (const auto v : changed) {
    visited_[v] = false;
  }
}
void Traversal::Check(const std::vector<std::vector<int>>& graph,
                      const std::vector<int>& core) const {
  for (int v = 0; v < n_; ++v) {
    ASSERT(deg_[v] == 0);
    ASSERT(!evicted_[v]);
    ASSERT(!visited_[v]);
  }
  for (int v = 0; v < n_; ++v) {
    int local_mcd = 0;
    for (const auto u : graph[v]) {
      ASSERT(u != v);
      if (core[u] >= core[v]) {
        ++local_mcd;
      }
    }
    ASSERT(mcd_[v] == local_mcd);
  }
  for (int v = 0; v < n_; ++v) {
    int local_pcd = 0;
    for (const auto u : graph[v]) {
      if (core[u] > core[v] ||
          (core[u] == core[v] && mcd_[u] > core[u])) {
        ++local_pcd;
      }
    }
    ASSERT(pcd_[v] == local_pcd);
  }
}
}  // namespace core
