#include "heap.h"

#include "defs.h"

namespace gadget {
MinHeap::MinHeap(const int n): pos_(n + 1, -1) {
  heap_.push_back({-1, -1});
}
void MinHeap::Insert(const int key, const int value) {
  const int size = heap_.size();
  heap_.push_back({key, value});
  Up(size, key, value);
}
void MinHeap::Delete(const int key) {
  const int size = heap_.size() - 1;
  const int k = heap_[size].key;
  const int v = heap_[size].val;
  heap_.pop_back();
  // sift down or up?
  if (size != pos_[key]) {
    const int h = pos_[key];
    if (h / 2 != 0 && heap_[h / 2].key > k) {
      Up(h, k, v);
    } else {
      Down(h, k, v);
    }
  }
  pos_[key] = -1;
}
Pair MinHeap::Top() const {
  ASSERT(heap_.size() > 1);
  return heap_[1];
}
void MinHeap::Up(const int h, const int key, const int value) {
  int c = h;      // child
  int p = c / 2;  // parent
  while (0 != p && heap_[p].key > key) {
    heap_[c] = heap_[p];
    pos_[heap_[p].key] = c;
    c = p;
    p = c / 2;
  }
  heap_[c] = {key, value};
  pos_[key] = c;
}
void MinHeap::Down(const int h, const int key, const int value) {
  const int size = heap_.size();
  int p = h;
  int c = p * 2;
  while (c < size) {
    if (c + 1 < size && heap_[c+1].key < heap_[c].key) ++c;
    if (heap_[c].key > key) break;
    heap_[p] = heap_[c];
    pos_[heap_[c].key] = p;
    p = c;
    c = p * 2;
  }
  heap_[p] = {key, value};
  pos_[key] = p;
}
}  // namespace gadget
