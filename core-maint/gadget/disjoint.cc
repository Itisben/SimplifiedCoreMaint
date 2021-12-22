#include "disjoint.h"

#include "defs.h"

namespace gadget {
DisjointSets::DisjointSets(const int n) {
  ASSERT((n_ = n) >= 0);
  nl_child_ = std::vector<int>(n_);
  nl_prev_  = std::vector<int>(n_ + 1);
  nl_next_  = std::vector<int>(n_ + 1);
  child_    = std::vector<int>(n_);
  prev_     = std::vector<int>(n_ + 1);
  next_     = std::vector<int>(n_ + 1);
  dfs_prev_ = std::vector<int>(n_);
  dfs_next_ = std::vector<int>(n_);
  parent_   = std::vector<int>(n_);
  rank_     = std::vector<int>(n_);
  to_item_  = std::vector<int>(n_);
  to_node_  = std::vector<int>(n_);
  // currently all nodes are free
  for (int i = 0; i < n_; ++i) {
    free_nodes_.push_back(i);
  }
}
int DisjointSets::MakeSet(const std::vector<int>& items) {
  ASSERT(likely(items.size() <= free_nodes_.size() && 0 < items.size()));
  const auto root_item = items[0];
  const auto root_node = free_nodes_[free_nodes_.size() - 1];
  free_nodes_.pop_back();
  // create the root node
  CreateLeaf(root_item, root_node);
  // single item
  if (1 == items.size()) {
    return root_item;
  }
  rank_[root_node] = 1;
  // create the leaves if there are more items
  for (size_t i = 1; i < items.size(); ++i) {
    const auto node = free_nodes_[free_nodes_.size() - 1];
    free_nodes_.pop_back();
    CreateLeaf(items[i], node);
    Hang(node, root_node);
  }
  return root_item;
}
int DisjointSets::Find(const int item) {
  // x is the node associated with the item
  auto x = to_node_[item];
  // path splitting
  while (!IsRoot(parent_[x])) {
    const auto y = parent_[x];
    Relink(x, y, parent_[y]);
    const auto c = child_[y];
    if (next_[next_[c]] == n_) { // not full
      Relink(c, y, parent_[y]);
      Relink(child_[y], y, parent_[y]);
    }
    x = y;
  }
  // set the root rank if the tree is reduced
  const auto r = parent_[x];
  if (!IsLeaf(r) && IsReduced(r)) {
    rank_[r] = 1;
  }
  return to_item_[r];
}
int DisjointSets::Union(const int root_item1, const int root_item2) {
  const auto root1 = to_node_[root_item1];
  const auto root2 = to_node_[root_item2];
  ASSERT(IsRoot(root1) && IsRoot(root2));
  // one of two trees is of size < 4
  if (IsSmall(root1) || IsSmall(root2)) {
    const auto x = IsSmall(root1) ? root1 : root2;
    const auto y = root1 + root2 - x;
    Hang(x, y);
    while (!IsLeaf(x)) {
      Relink(child_[x], x, y);
    }
    if (IsReduced(y)) {
      rank_[y] = 1;
    }
    return to_item_[y];
  }
  // both trees are full
  const auto x = rank_[root1] <= rank_[root2] ? root1 : root2;
  const auto y = root1 + root2 - x;
  Hang(x, y);
  rank_[y] += (rank_[x] == rank_[y]);
  return to_item_[y];
}
void DisjointSets::Delete(const int item) {
  auto x = to_node_[item];
  auto y = parent_[x];
  // first case: this is a reduced tree
  if (IsRoot(y) && IsReduced(y)) {
    if (!IsLeaf(y)) {
      if (y == x) {
        x = child_[y];
        to_item_[y] = to_item_[x];
        to_node_[to_item_[y]] = y;
      }
      Unhang(x, y);
    }
    free_nodes_.push_back(x);
  } else { // second case: this is a full tree
    const auto z = FindLeaf(x);
    to_item_[x] = to_item_[z];
    to_node_[to_item_[x]] = x;
    // the problem now is reduced to removing a leaf z
    y = parent_[z];
    Unhang(z, y);
    free_nodes_.push_back(z);
    // maitain the total value
    if (IsRoot(y)) {
      const auto c = nl_child_[y];
      for (int i = 0; i < 3; ++i) {
        Relink(child_[c], c, y);
      }
      if (!IsLeaf(c) &&
          (next_[child_[c]] == n_ || next_[next_[child_[c]]] == n_)) {
        while (!IsLeaf(c)) {
          Relink(child_[c], c, y);
        }
      }
      if (IsReduced(y)) {
        rank_[y] = 1;
      }
    } else {
      const auto yp = parent_[y];
      for (int i = 0; i < 2; ++i) {
        Relink(child_[y], y, yp);
      }
      if (!IsLeaf(y) &&
          (next_[child_[y]] == n_ || next_[next_[child_[y]]] == n_)) {
        while (!IsLeaf(y)) {
          Relink(child_[y], y, yp);
        }
      }
      if (IsRoot(yp) && IsReduced(yp)) {
        rank_[yp] = 1;
      }
    }
  }
  to_node_[item] = -1;
}
void DisjointSets::ListInit(const int x,
                            std::vector<int>& prev,
                            std::vector<int>& next) {
  next[x] = prev[x] = n_;
}
void DisjointSets::ListInsertBefore(const int x, const int y,
                                    std::vector<int>& prev,
                                    std::vector<int>& next) {
  const auto z = prev[y];
  prev[x] = z;
  next[x] = y;
  prev[y] = x;
  next[z] = x;
}
void DisjointSets::ListInsertAfter(const int x, const int y,
                                   std::vector<int>& prev,
                                   std::vector<int>& next) {
  const auto z = next[y];
  prev[x] = y;
  next[x] = z;
  next[y] = x;
  prev[z] = x;
}
void DisjointSets::ListRemove(const int x,
                              std::vector<int>& prev,
                              std::vector<int>& next) {
  const auto y = prev[x];
  const auto z = next[x];
  next[y] = z;
  prev[z] = y;
}
void DisjointSets::DFSListInsertBefore(const int bx, const int ex,
                                       const int y) {
  const auto z = dfs_prev_[y];
  dfs_prev_[bx] = z;
  dfs_next_[ex] = y;
  dfs_prev_[y] = ex;
  dfs_next_[z] = bx;
}
void DisjointSets::DFSListInsertAfter(const int bx, const int ex,
                                      const int y) {
  const auto z = dfs_next_[y];
  dfs_prev_[bx] = y;
  dfs_next_[ex] = z;
  dfs_prev_[z] = ex;
  dfs_next_[y] = bx;
}
void DisjointSets::DFSListRemove(const int bx, const int ex) {
  const auto y = dfs_prev_[bx];
  const auto z = dfs_next_[ex];
  dfs_next_[y] = z;
  dfs_prev_[z] = y;
}
void DisjointSets::InsertChild(const int x, const int y) {
  // update the child list
  if (IsLeaf(y)) {
    ListInit(x, prev_, next_);
  } else {
    ListInsertBefore(x, child_[y], prev_, next_);
  }
  child_[y] = x;
  parent_[x] = y;
  // update the NL list if necessary
  if (!IsLeaf(x)) {
    if (IsReduced(y)) {
      ListInit(x, nl_prev_, nl_next_);
    } else {
      ListInsertBefore(x, nl_child_[y], nl_prev_, nl_next_);
    }
    nl_child_[y] = x;
  }
}
void DisjointSets::DeleteChild(const int x, const int y) {
  // update the child list
  if (IsOnlyChild(x)) {
    child_[y] = y;
    rank_[y] = 0;
  } else {
    if (x == child_[y]) {
      child_[y] = next_[x];
    }
    ListRemove(x, prev_, next_);
  }
  // now y becomes a leaf
  if (IsLeaf(y) && !IsRoot(y)) {
    const auto z = parent_[y];
    if (IsRoot(z)) {
      if (IsOnlyNLChild(y)) {
        nl_child_[z] = z;
      } else {
        if (y == nl_child_[z]) {
          nl_child_[z] = nl_next_[y];
        }
        ListRemove(y, nl_prev_, nl_next_);
      }
    }
  }
}
void DisjointSets::Unhang(const int x, const int y) {
  // assume x is a leaf
  ASSERT(IsLeaf(x));
  DeleteChild(x, y);
  DFSListRemove(x, x);
}
void DisjointSets::Hang(const int x, const int y) {
  ASSERT(IsRoot(y) && IsRoot(x));
  InsertChild(x, y);
  DFSListInsertAfter(x, dfs_prev_[x], y);
}
void DisjointSets::Relink(const int x, const int y, const int z) {
  const auto t = next_[x];
  DeleteChild(x, y);
  parent_[x] = z;
  if (t == n_) {  // x is the last child of y
    ListInsertAfter(x, y, prev_, next_);
  } else {
    ListInsertBefore(x, y, prev_, next_);
    if (y == child_[z]) {
      child_[z] = x;
    }
    const auto dfs_beg = x;
    const auto dfs_end = dfs_prev_[t];
    DFSListRemove(dfs_beg, dfs_end);
    DFSListInsertBefore(dfs_beg, dfs_end, y);
  }
  if (IsRoot(z) && !IsLeaf(x)) {
    if (IsReduced(z)) {
      ListInit(x, nl_prev_, nl_next_);
    } else {
      ListInsertBefore(x, nl_child_[z], nl_prev_, nl_next_);
    }
    nl_child_[z] = x;
  }
}
void DisjointSets::CreateLeaf(const int item, const int node) {
  // bind the item and the node
  to_item_[node] = item;
  to_node_[item] = node;
  // node setting
  child_[node] = node;
  nl_child_[node] = node;
  parent_[node] = node;
  rank_[node] = 0;
  dfs_prev_[node] = node;
  dfs_next_[node] = node;
}
int DisjointSets::FindLeaf(const int x) const {
  if (IsLeaf(x)) {
    return x;
  } else if (IsRoot(x)) {
    return dfs_prev_[x];
  } else if (next_[x] != n_) {
    return dfs_prev_[next_[x]];
  } else {
    return dfs_prev_[x];
  }
}
bool DisjointSets::IsRoot(const int x) const {
  return x == parent_[x];
}
bool DisjointSets::IsLeaf(const int x) const {
  return x == child_[x];
}
bool DisjointSets::IsReduced(const int r) const {
  return r == nl_child_[r];
}
bool DisjointSets::IsOnlyChild(const int x) const {
  return prev_[x] == n_ && next_[x] == n_;
}
bool DisjointSets::IsOnlyNLChild(const int x) const {
  return nl_prev_[x] == n_ && nl_next_[x] == n_;
}
bool DisjointSets::IsSmall(const int r) const {
  return IsReduced(r) &&
      (IsLeaf(r) || next_[child_[r]] == n_ || next_[next_[child_[r]]] == n_);
}
}  // namespace gadget
