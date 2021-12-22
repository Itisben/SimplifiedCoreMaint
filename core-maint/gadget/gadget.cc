#include "gadget.h"

#include <algorithm>
#include <cstdio>
#include <limits>
#include <set>
#include <string>
#include "defs.h"
#include "gm.h"

namespace gadget {
void RepeatWith(const char symbol, const int repeat) {
  ASSERT(repeat > 0);
  for (int i = 0; i < repeat; ++i) {
    printf("%c", symbol);
  }
  putchar('\n');
}
std::vector<std::vector<int>> ReadGraph(const char* const path,
                                        int* const n, int* const m) {
  auto file = fopen(path, "r");
  fscanf(file, "%d %d", n, m);
  ASSERT(*n > 0 && *m > 0);
  std::vector<std::vector<int>> graph(*n);
  // for debug purpose
  std::set<std::pair<int, int>> debug;
  // read the edges
  while (true) {
    int v1, v2;
    fscanf(file, "%d %d", &v1, &v2);
    if (feof(file)) break;
    // for debug purpose
    ASSERT(0 <= v1 && v1 < *n && 0 <= v2 && v2 < *n && v1 != v2);
    if (v1 > v2) std::swap(v1, v2);
    ASSERT(debug.count(std::make_pair(v1, v2)) == 0);
    debug.insert(std::make_pair(v1, v2));

    graph[v1].push_back(v2);
    graph[v2].push_back(v1);
  }
  ASSERT(static_cast<decltype(debug.size())>(*m) == debug.size());
  fclose(file);
  return graph;
}
std::vector<std::pair<int, int>> ReadTempEdgesS(const char* const path,
                                                int* const n, int* const m) {
  auto file = fopen(path, "r");
  fscanf(file, "%d %d", n, m);
  ASSERT(*n > 0 && *m > 0);
  std::vector<std::pair<int, int>> edges;
  // for debug purpose
  std::set<std::pair<int, int>> debug;
  int prev_time = std::numeric_limits<int>::min();
  // read the edges
  while (true) {
    int v1, v2, tm;
    fscanf(file, "%d %d %d", &v1, &v2, &tm);
    if (feof(file)) break;
    // for debug purpose
    ASSERT(0 <= v1 && v1 < *n && 0 <= v2 && v2 < *n && v1 != v2);
    ASSERT(prev_time <= tm); // time is non-decreasing
    if (v1 > v2) std::swap(v1, v2);
    const auto e = std::make_pair(v1, v2);
    ASSERT(debug.count(e) == 0);
    debug.insert(e);
    // insert the edge
    edges.push_back(e);
    prev_time = tm;
  }
  ASSERT(static_cast<int>(debug.size()) == *m);
  ASSERT(static_cast<int>(edges.size()) == *m);
  fclose(file);
  return edges;
}

/*read edges by shuffle*/
std::vector<std::pair<int, int>> ReadEdgesS(const char* const path,
                                            int* const n, int* const m) {
  srand(time(NULL));

  auto file = fopen(path, "r");
  fscanf(file, "%d %d", n, m);
  ASSERT(*n > 0 && *m > 0);
  std::vector<std::pair<int, int>> edges;
  // for debug purpose
  std::set<std::pair<int, int>> debug;
  // read the edges
  while (true) {
    int v1, v2;
    fscanf(file, "%d %d", &v1, &v2);
    if (feof(file)) break;
    // for debug purpose
    ASSERT(0 <= v1 && v1 < *n && 0 <= v2 && v2 < *n && v1 != v2);
    if (v1 > v2) std::swap(v1, v2);
    const auto e = std::make_pair(v1, v2);
    ASSERT(debug.count(e) == 0);
    debug.insert(e);
    // insert the edge
    edges.push_back(e);
  }
  ASSERT(static_cast<int>(debug.size()) == *m);
  ASSERT(static_cast<int>(edges.size()) == *m);
  // randomly shuffle the edges
  for (size_t e = 0; e < edges.size(); ++e) {
    const size_t e2 = e + rand() % (edges.size() - e);
    std::swap(edges[e], edges[e2]);
  }
  fclose(file);
  return edges;
}

/*read edges withou shuffle*/
std::vector<std::pair<int, int>> ReadEdges(const char* const path,
                                            int* const n, int* const m) {
  auto file = fopen(path, "r");
  fscanf(file, "%d %d", n, m);
  ASSERT(*n > 0 && *m > 0);
  std::vector<std::pair<int, int>> edges;
 
  // read the edges
  while (true) {
    int v1, v2;
    fscanf(file, "%d %d", &v1, &v2);
    if (feof(file)) break;
    // for debug purpose
    ASSERT(0 <= v1 && v1 < *n && 0 <= v2 && v2 < *n && v1 != v2);
    if (v1 > v2) std::swap(v1, v2);
    const auto e = std::make_pair(v1, v2);
    edges.push_back(e);
  }
  ASSERT(static_cast<int>(edges.size()) == *m);

  fclose(file);
  return edges;
}


std::vector<int> RepeateRandomRead(const char* path) {
    printf("read random number from: %s\n", path);
    auto file = fopen(path, "r");
    std::vector<int> rand; rand.reserve(1000000);
    // read the edges
    while (true) {
        int v;
        fscanf(file, "%d", &v);
        if (feof(file)) break;
        rand.push_back(v);
    }
    fclose(file);
    return rand;
}


/*read edges with shuffle
1. random shuffle, 2 repeated random shuffle, 0 without shuffle*/
std::vector<std::pair<int, int>> CSRReadEdges(char* const path,
                    int* const n, int* const m, int shuffle) 
{
    //void pin_CPU()
    
    {   
        printf("set pin CPU\n");
        #pragma omp parallel 
        {
            pthread_t thread;
            thread = pthread_self();
            cpu_set_t CPU;
            CPU_ZERO(&CPU);
            CPU_SET(omp_get_thread_num(), &CPU);
            pthread_setaffinity_np(thread, sizeof(CPU), &CPU);
        }
    }
    

    gm_graph G;
    struct timeval T1, T2;
    gettimeofday(&T1, NULL); 
    auto b = G.load_binary(path);
    if (!b) {
        printf("error reading graph\n");
        exit(EXIT_FAILURE);
    }
    gettimeofday(&T2, NULL); 
    printf("\ngraph loading time(ms): %lf\n", 
        (T2.tv_sec - T1.tv_sec)*1000 + 
        (T2.tv_usec - T1.tv_usec)*0.001 
    );

    std::vector<std::pair<int, int>> edges;
    edges.reserve(G.num_edges());
    
#if 1 //for remove the duplicate edges

    
    std::set<std::pair<int, int>> edgeset;
    
    for(node_t i = 0; i < G.num_nodes(); i++) {

        for (int idx = G.begin[i]; idx < G.begin[i+1]; idx++){
            node_t j = G.node_idx[idx];
            if (i == j) continue;
            if (i < j) {
                edgeset.insert(std::make_pair(i, j));
            } else {
                edgeset.insert(std::make_pair(j, i));
            }        
        }
    }
    

    *n = 0;
    for(auto e: edgeset) { 
        if (e.second > *n) *n = e.second;
        edges.push_back(e);
    }

#else   //without removing the duplicate edges. 
        // this may couse the core nubmer incorrect.
    
    for(node_t i = 0; i < G.num_nodes(); i++) {

        for (int idx = G.begin[i]; idx < G.begin[i+1]; idx++){
            node_t j = G.node_idx[idx];
            if (i < j) {
                edges.push_back(std::make_pair(i, j));
            } else {
                edges.push_back(std::make_pair(j, i));
            }  
        }
    }
#endif

    *n = G.num_nodes();
    *m = edges.size(); // edge number is update

     // randomly shuffle the edges
    if (1 == shuffle) {
        srand(time(NULL));
        for (size_t e = 0; e < edges.size(); ++e) {
            const size_t e2 = e + rand() % (edges.size() - e);
            std::swap(edges[e], edges[e2]);
        }
    }

    // repeated random shuffle the edges. 
    if (2 == shuffle) {
        size_t found;
        string str(path);
        found=str.find_last_of("/\\");
        str=str.substr(0,found); //folder str.substr(found+1) file
        str+="/random-1m.txt";

        std::vector<int> rand = RepeateRandomRead(str.c_str());
        int i = 0;
        for (size_t e = 0; e < edges.size(); ++e) {
            int r = rand[i++]; //get repeated random number 
            if (i >= rand.size()) {i = 0;} //reset
            const size_t e2 = e + r % (edges.size() - e);
            std::swap(edges[e], edges[e2]);
        }
    }

    return edges;
}


/*output the core number distribution for the graph*/
void OutputCoreNumber(const char* const path, std::vector<int> &core, const int n){
    std::vector<int> core_cnt(n, 0);
    for (size_t i = 0; i < n; i++){
        core_cnt[core[i]]++;
    }
    std::string newpath(path);
    newpath += ".core";
    auto file = fopen(newpath.c_str(), "w");
    std::string line;
    
    for (size_t i = 0; i < n; i++) {
        if (core_cnt[i] > 0) {
            line.clear();
            line += std::to_string(i);
            line += ",";
            line += std::to_string(core_cnt[i]);
            line += "\n";
            fputs(line.c_str(), file);
        }
    }
    fclose(file);
    printf("save the statistic core numbers to %s\n", newpath.c_str());
}

/*output the core number distribution for inserted edges by sampling 100 times*/
void OutputSampleEdgeCoreNumber(const char* const path, std::vector<int> &core, 
    std::vector<std::pair<int, int>> &edges, const int n) {
    
    std::vector<int> core_cnt(n, 0);
    std::vector<int> flag(n, 0);
    for (int num = 0; num < 1; num++) {
        //sample
        srand(time(NULL));
        for (size_t e = 0; e < edges.size(); ++e) {
            const size_t e2 = e + rand() % (edges.size() - e);
            std::swap(edges[e], edges[e2]);
        }

        //count first 10k edges
        for (int i  = 0; i < 100000; i++) {
            auto e = edges[i];
            if (0 == flag[e.first]) {core_cnt[core[e.first]]++; flag[e.first]=1; }
            if (0 == flag[e.second]) {core_cnt[core[e.second]]++; flag[e.second]=1;}
        }
        
    }

    std::string newpath(path);
    newpath += ".core10k";
    auto file = fopen(newpath.c_str(), "w");
    std::string line;
    
    for (size_t i = 0; i < n; i++) {
        if (core_cnt[i] > 0) {
            line.clear();
            line += std::to_string(i);
            line += ",";
            line += std::to_string(core_cnt[i]);
            line += "\n";
            fputs(line.c_str(), file);
        }
    }
    fclose(file);
    printf("save the statistic sample edge core numbers to %s\n", newpath.c_str());
}

/**/
std::vector<std::pair<int, int>>  sampleEdges(const char* const path, std::vector<std::pair<int, int>> &edges, const float percent) {

    size_t found;
    string str(path);
    found=str.find_last_of("/\\");
    str=str.substr(0,found); //folder str.substr(found+1) file
    str+="/random-1m.txt";

    std::vector<int> randvector = RepeateRandomRead(str.c_str());
    int i = 0;


    std::vector<std::pair<int, int>> edges2;
    int new_size = edges.size() * percent /100;
    edges2.reserve(new_size);

    srand(time(NULL));
    int edge_size = edges.size();

    for (auto it = edges.begin() ; it != edges.end(); ++it) {
        #if 0
        if (rand() % 100 < percent) {
            edges2.push_back(*it);
        }
        #else
        if (randvector[i++] % 100000 < percent * 1000) {
                edges2.push_back(*it);
        }
        if (i >= randvector.size()) {i = 0;} //reset
        #endif 
    }



    //printf("sample edge %d%%\n", percent_edge);
    printf("***SampleM = %f\%, before %d, after %d\n",  percent, edge_size, edges2.size());
    return edges2;
}

void CutEdges(std::vector<std::pair<int, int>> &edges, const int num) {
    size_t edge_size = edges.size();
    edges.erase(edges.begin() + num, edges.end());
    printf("***cut edges %d, before %d, after %d\n",  num, edge_size, edges.size());
}
   

}  // namespace gadget
