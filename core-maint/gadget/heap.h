#ifndef CORE_GADGET_HEAP_H_
#define CORE_GADGET_HEAP_H_

#include <vector>

namespace gadget {
struct Pair final {
  int key;
  int val;
};
class MinHeap final {
 public:
  explicit MinHeap(const int n);

  void Insert(const int key, const int value);
  void Delete(const int key);
  Pair Top() const;
  bool Contains(const int key) const {
    return pos_[key] != -1;
  }
  bool Empty() const {
    return heap_.size() == 1;
  }
  int Size() const {
    return heap_.size() - 1;
  }

 private:
  void Up(const int h, const int key, const int value);
  void Down(const int h, const int key, const int value);

  std::vector<Pair> heap_;
  std::vector<int> pos_;
};
}  // namespace gadget

#endif
