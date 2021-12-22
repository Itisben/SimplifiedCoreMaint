#include "sb_tree.h"

#include <cstdlib>
#include <ctime>

#include "defs.h"

namespace gadget {
SBTree::SBTree(const int n): n_(n) {
  nd_ = std::vector<SBTreeNode>(n_ + 1);
  nd_[n_].l = nd_[n_].r = n_;
  nd_[n_].s = 0;
}
void SBTree::Insert(const int z, const bool f, int& r) {
  if (n_ == r) {
    r = z;
    nd_[z].p = nd_[z].l = nd_[z].r = n_;
    nd_[z].s = 1;
  } else {
    ++nd_[r].s;
    int& c = f ? nd_[r].l : nd_[r].r;
    Insert(z, f, c);
    nd_[c].p = r;
    Maintain(!f, r);
  }
}
void SBTree::InsertAfter(const int z, const int v, int& r) {
  Insert(z, true, nd_[v].r);
  nd_[nd_[v].r].p = v;
  int x = nd_[v].r;
  int p = nd_[x].p;
  while (n_ != p) {
    ++nd_[p].s;
    int* ptr = &r;
    if (n_ != nd_[p].p) {
      if (p == nd_[nd_[p].p].l) {
        ptr = &nd_[nd_[p].p].l;
      } else {
        ptr = &nd_[nd_[p].p].r;
      }
    }
    Maintain(x != nd_[p].l, *ptr);
    x = *ptr;
    p = nd_[x].p;
  }
}
void SBTree::Delete(const int z, int& r) {
  int v = -1;
  if (n_ != nd_[z].r) {
    DeleteMin(nd_[z].r, v);
    nd_[v] = nd_[z];
    if (n_ != nd_[v].p) {
      if (z == nd_[nd_[v].p].l) {
        nd_[nd_[v].p].l = v;
      } else {
        nd_[nd_[v].p].r = v;
      }
    } else {
      r = v;
    }
    nd_[nd_[v].l].p = v;
    nd_[nd_[v].r].p = v;
  } else {
    nd_[nd_[z].l].p = nd_[z].p;
    if (n_ != nd_[z].p) {
      if (z == nd_[nd_[z].p].l) {
        nd_[nd_[z].p].l = nd_[z].l;
      } else {
        nd_[nd_[z].p].r = nd_[z].l;
      }
    } else {
      r = nd_[z].l;
    }
    if (n_ != nd_[z].l) {
      v = nd_[z].l;
      ++nd_[v].s;
    } else {
      v = nd_[z].p;
    }
  }
  while (n_ != v) {
    --nd_[v].s;
    const int p = nd_[v].p;
    if (n_ != p) {
      int& c = nd_[p].l == v ? nd_[p].l : nd_[p].r;
      Maintain(false, c);
      Maintain(true, c);
    } else {
      Maintain(false, r);
      Maintain(true, r);
    }
    v = p;
  }
}
int SBTree::Rank(const int x) const {
  int rank = nd_[nd_[x].l].s + 1;
  int y = x;
  int p = nd_[y].p;
  while (n_ != p) {
    if (y == nd_[p].r) {
      rank += nd_[p].s - nd_[y].s;
    }
    y = p;
    p = nd_[y].p;
  }
  return rank;
}
int SBTree::Size(const int r) const {
  return nd_[r].s;
}
void SBTree::Check(const int r) const {
  ASSERT(0 <= r && r < n_);
  ASSERT(nd_[r].p == n_);
  ASSERT(nd_[n_].l == n_ && nd_[n_].r == n_ && nd_[n_].s == 0);
  SubCheck(r);
}
void SBTree::LeftRotate(int& x) {
  const int y = nd_[x].r;
  nd_[y].p = nd_[x].p;
  nd_[x].p = y;
  nd_[x].r = nd_[y].l;
  nd_[nd_[x].r].p = x;
  nd_[y].l = x;
  nd_[y].s = nd_[x].s;
  nd_[x].s = nd_[nd_[x].l].s + nd_[nd_[x].r].s + 1;
  x = y;
}
void SBTree::RightRotate(int& x) {
  const int y = nd_[x].l;
  nd_[y].p = nd_[x].p;
  nd_[x].p = y;
  nd_[x].l = nd_[y].r;
  nd_[nd_[x].l].p = x;
  nd_[y].r = x;
  nd_[y].s = nd_[x].s;
  nd_[x].s = nd_[nd_[x].l].s + nd_[nd_[x].r].s + 1;
  x = y;
}
void SBTree::Maintain(const bool f, int& x) {
  if (!f) {
    const int y = nd_[x].l;
    if (nd_[nd_[y].l].s > nd_[nd_[x].r].s) {
      RightRotate(x);
    } else if (nd_[nd_[y].r].s > nd_[nd_[x].r].s) {
      LeftRotate(nd_[x].l);
      RightRotate(x);
    } else {
      return;
    }
  } else {
    const int y = nd_[x].r;
    if (nd_[nd_[y].r].s > nd_[nd_[x].l].s) {
      LeftRotate(x);
    } else if (nd_[nd_[y].l].s > nd_[nd_[x].l].s) {
      RightRotate(nd_[x].r);
      LeftRotate(x);
    } else {
      return;
    }
  }
  Maintain(false, nd_[x].l);
  Maintain(true, nd_[x].r);
  Maintain(false, x);
  Maintain(true, x);
}
void SBTree::DeleteMin(int& x, int& v) {
  ASSERT(x != n_);
  if (n_ == nd_[x].l) {
    v = x;
    nd_[nd_[x].r].p = nd_[x].p;
    x = nd_[x].r;
  } else {
    --nd_[x].s;
    DeleteMin(nd_[x].l, v);
    Maintain(false, x);
    Maintain(true, x);
  }
}
void SBTree::SubCheck(const int x) const {
  ASSERT(nd_[x].s == nd_[nd_[x].r].s + nd_[nd_[x].l].s + 1);
  ASSERT(nd_[nd_[x].l].s >= nd_[nd_[nd_[x].r].l].s);
  ASSERT(nd_[nd_[x].l].s >= nd_[nd_[nd_[x].r].r].s);
  ASSERT(nd_[nd_[x].r].s >= nd_[nd_[nd_[x].l].l].s);
  ASSERT(nd_[nd_[x].r].s >= nd_[nd_[nd_[x].l].r].s);
  if (n_ != nd_[x].l) {
    ASSERT(nd_[nd_[x].l].p == x);
    SubCheck(nd_[x].l);
  }
  if (n_ != nd_[x].r) {
    ASSERT(nd_[nd_[x].r].p == x);
    SubCheck(nd_[x].r);
  }
}
}  // namespace gadget

/*
namespace {
constexpr int n = 1000000;
}  // namespace

int main() {
  gadget::SBTree tree(n);
  std::vector<int> roots(100, n);
  std::vector<int> counts(100, 0);
  std::vector<int> numbers(n);

  for (int i = 0; i < n; ++i) {
    numbers[i] = rand() % 100;
    ++counts[numbers[i]];
    tree.Insert(i, rand() % 2, roots[numbers[i]]);
  }
  for (int i = 0; i < 100; ++i) {
    if (roots[i] != n) {
      tree.Check(roots[i]);
      ASSERT(tree.Size(roots[i]) == counts[i]);
    }
  }
  for (int i = 0; i < n; ++i) {
    const int b = numbers[i];
    const int nb = rand() % 100;
    if (nb != b) {
      --counts[b];
      ++counts[nb];
      numbers[i] = nb;
      tree.Delete(i, roots[b]);
      tree.Insert(i, rand() % 2, roots[nb]);
    }
  }
  for (int i = 0; i < 100; ++i) {
    if (roots[i] != n) {
      tree.Check(roots[i]);
      ASSERT(tree.Size(roots[i]) == counts[i]);
    }
  }
  for (int i = 0; i < n; ++i) {
    const int b = numbers[i];
    tree.Delete(i, roots[b]);
    --counts[b];
  }
  for (int i = 0; i < 100; ++i) {
    ASSERT(roots[i] == n);
  }
  for (int i = 0; i < n; ++i) {
    tree.Insert(i, false, roots[0]);
  }
  ASSERT(tree.Size(roots[0]) == n);
  for (int i = 0; i < n; ++i) {
    ASSERT(tree.Rank(i) == i + 1);
  }
  for (int i = 0; i < n; ++i) {
    tree.Delete(i, roots[0]);
    tree.InsertAfter(i, (n - 1 + i) % n, roots[0]);
  }
  for (int i = 0; i < n; ++i) {
    ASSERT(tree.Rank(i) == i + 1);
  }
  tree.Check(roots[0]);
}
*/
