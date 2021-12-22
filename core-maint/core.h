#ifndef CORE_CORE_H_
#define CORE_CORE_H_

#include <vector>

namespace core {
class CoreMaintenance {
 public:
  virtual ~CoreMaintenance() {}

  virtual void ComputeCore( std::vector<std::vector<int>>& graph,
                           const bool init_idx, // initialize the index?
                           std::vector<int>& core) = 0;
  virtual void Insert(const int v1, const int v2,
                      std::vector<std::vector<int>>& graph,
                      std::vector<int>& core) = 0;
  virtual void Remove(const int v1, const int v2,
                      std::vector<std::vector<int>>& graph,
                      std::vector<int>& core) = 0;
  virtual void Check(const std::vector<std::vector<int>>& graph,
                     const std::vector<int>& core) const = 0;
};
}  // namespace core


/*for debug*/
extern int cnt_edge;
extern int cnt_edge2;
extern int cnt_Vs; 
extern int cnt_Vp;
extern int cnt_S;
extern int cnt_gap_adjust;
extern int global_check_num;


/*define the csr graph, */
class GRAPH {
    typedef int node_t;
public:
     /* graph create by csr format 
    * for (i = begin(v); i++; i < end(v))*/
    const int n;
    const int m; // size of vertices and edges
    std::vector<node_t> node_idx;
    std::vector<node_t> begin;
    std::vector<node_t> end;

    GRAPH(int n, int m, std::vector<std::pair<int, int>> &edges):
            n{n}, m{m}{
        node_idx = std::vector<node_t>(m, -1);
        begin=std::vector<node_t>(n+1);
        end=std::vector<node_t>(n+1);

        //allocate the memory
        for (auto const e: edges) { //cnt the edge number for each node
            end[e.first]++; end[e.second]++; // size of edges
        }
        begin[0] = 0;
        for (node_t v = 1; v < n+1; v++) {
            begin[v] = begin[v-1] + end[v-1]; //accumulate the size
        }
        for (node_t v = 0; v < n+1; v++) {
            end[v] = begin[v]; // initially the graph is empty
        }
    }

    void insert(node_t u, node_t v) { // for undirected graph
        node_idx[end[u]] = v; ++end[u];
        node_idx[end[v]] = u; ++end[v];
    }
    
    void remove(node_t u, node_t v) {
        for (int i = begin[u]; i < end[u]; i++) {
            if (v == node_idx[i]) { // edges are not empty
                std::swap(node_idx[i], node_idx[end[u] - 1]);
                --end[u];
                break;
            }
        }
        for (int i = begin[v]; i < end[v]; i++) {
            if (u == node_idx[i]) { // edges are not empty
                std::swap(node_idx[i], node_idx[end[v] - 1]);
                --end[v];
                break;
            }
        }
    }

    inline node_t get(int id) {return node_idx[id];}
    
};
   
    

#endif
