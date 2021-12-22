#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>

#include "core.h"
#include "defs.h"
#include "gadget/gadget.h"
#include "glist/glist.h"
#include "traversal/traversal.h"
#include "ours8/core-maint.h"

/*counting*/
int cnt_edge = 0;
int cnt_edge2 = 0;
int cnt_Vs = 0; // count of size V*
int cnt_Vp = 0; // count of size V+
int cnt_S = 0;  // count of all visited
int cnt_tag = 0;
int cnt_gap_adjust = 0;
int global_check_num = 0;

extern const bool WITH_E2;

int main(int argc, char** argv) {
  char path[128];      // path of input graph
  int method = 0;
  double ratio = 0.0;  // ratio of edges to insert
  int optInsertNum = 0;
  int optRemoveNum = 0;
  int temp = 0;   // temporal graph 
  float edgePercent = 100;

  if(argc < 2) {
    printf("-p: the path of the data file\n");
    printf("-r: the proportion of edges that is sampled out for insertion/removal\n");
    printf("-I: the number of insert edges that for operation\n");
    printf("-R: the number of remove edges that for operation\n");
    printf("-m: the algorithm to be used: 2 glist, 1 traversal, or 3 ours (13 batch insertion)\n");
    //printf("-M: the sub algorithm to be used: 1 ours batch insertion\n");
    printf("-T: 1 for temporal graphs, 0 for ordinary graphs sample edges, 2 for debug without sampled edges\n");
    printf("    3 csr sample edges, 4 csr without sample edges, 5 csr with repeated random \n");
    printf("-c: 1 Output the statistic of core numbers to file *.bin.core for original graph\n");
    printf("    2 Output the statistic of sampled 10k edges core numbers to \n");
    printf("-s: [<=100]the percent samele the percent of edges \n");
    printf("    [>=100000] the number of sameled of edges, 100k, 200k ... \n");
    exit(0);
  }
  // initialize the options
  int option = -1;
  int outputCore = 0;
  while (-1 != (option = getopt(argc, argv, "p:r:I:R:m:M:T:c:s:"))) {
    switch (option) {
      case 'p':
        strcpy(path, optarg);
        break;
      case 'r':
        ASSERT(0.0 <= (ratio = atof(optarg)) && 1.0 >= ratio);
        break;
      case 'I':
        optInsertNum = atoi(optarg);
        break;
      case 'R':
        optRemoveNum = atoi(optarg);
        break;
      case 'm':
        method = atoi(optarg);
        break;
      case 'T':
        temp = atoi(optarg);
        break;
      case 'c':
        outputCore = atoi(optarg);
        break;
      case 's':
        edgePercent = atof(optarg);
        break;
    }
  }
  // read the graph
  int n, m, m2;
  std::vector<std::pair<int, int>> edges;
  if(1 == temp) {
    //const auto read_func = temp ? gadget::ReadTempEdgesS : gadget::ReadEdgesS;
    edges = gadget::ReadTempEdgesS(path, &n, &m);
  } else if (0 == temp) {
      edges = gadget::ReadEdgesS(path, &n, &m); // random shuffle the edges. 
      
  }  else if (2 == temp) {
      edges = gadget::ReadEdges(path, &n, &m); // without shuffle
      
  } else if (3 == temp ) {
        edges = gadget::CSRReadEdges(path, &n, &m, 1); // csr with shuffle
  } else if (4 == temp ) {
        edges = gadget::CSRReadEdges(path, &n, &m, 0); // csr without shuffle.
  } else if (5 == temp) {
         edges = gadget::CSRReadEdges(path, &n, &m, 2); // csr with repeated random shuffle.
  } else {
    printf("wrong graphs\n");
    exit(1);
  }

    //sample the edges by percentage
    if (edgePercent < 100) {
        edges = gadget::sampleEdges(path, edges, edgePercent);
        printf("edges size: %f\%\n", edgePercent);
    } else if (edgePercent >= 100000){
        if (edgePercent < m) {
            // edgepercent is the number of sampled edges.
            gadget::CutEdges(edges, edgePercent);
            printf("edges size: %d\n", edges.size());
        } else {
            // all edges
            edgePercent = m;
            printf("edges size: %d\n", edges.size());
        }
    } 
    m = edges.size();
  
  // get the number of edges for inserting or deleting.
  if (optInsertNum > 0) { m2 = m - optInsertNum;} 
  if (optRemoveNum > 0) { m2 = m;} //load the whole graph for removing.
  if (ratio > 0.0){ m2 = m - static_cast<int>(m * ratio);}

  // print the configurations
  gadget::RepeatWith('*', 80);
  printf("# of vertices: %d\n", n);
  printf("# of (all) edges: %d\n", m);
  printf("ratio: %f\n", ratio);
  printf("# of edges to insert: %d\n", optInsertNum);
  printf("# of edges to delete: %d\n", optRemoveNum);
  printf("path of the input file: %s\n", path);
  printf("method: %d  (2 glist, 1 traversal, or 3 ours)\n", method);
  printf("graph: %d  (1 for temporal graphs, 0 for ordinary graphs sample edges, 2 for debug without sampled edges)\n", temp);
  printf("            3 csr sample edges, 4 csr without sample edges\n");
  printf("---- with set E2 for insert: %d\n", WITH_E2);
#ifdef WITH_SUBTAG
    printf("---- with sub tag\n");
#endif
#ifdef WITH_V_BLACK2GRAY
    printf("---- with V Black2Gray in Backward for Insertion\n");
#endif
  gadget::RepeatWith('*', 80);
    

    // graph: create the adjacent list representation
    std::vector<std::vector<int>> graph(n);
  
    /* graph create by csr format 
    * for (i = begin(v); i++; i < end(v))*/
    /*graph csr end*/
    for (int i = 0; i < m2; i++) {
        int v1 = edges[i].first;
        int v2 = edges[i].second;
        graph[v1].push_back(v2);
        graph[v2].push_back(v1);
    }

    printf("creat adjacent list\n");

    // compute the base core
    std::vector<int> core(n);
    std::vector<int> order_v(n);
    std::vector<int> tmp_core(n);

  // initialize the core component
  core::CoreMaintenance* cm = nullptr;
  SeqCM::CoreMaint *ourcm = nullptr;
  if (method%10 == 1) { //traversal
    cm = new core::Traversal(n);
  } else if (method%10 == 2) { //glist
    cm = new core::GLIST(n);
  } else if (method%10 == 3 ) {
    ourcm = new SeqCM::CoreMaint(n, graph, core);
  } else {
      printf("wrong method!");
      exit(0);
  }

  printf("new the working object\n");
 

     //statistic the core numbers
    if (1 == outputCore) {
        for (auto e: edges) {
            graph[e.first].push_back(e.second);
            graph[e.second].push_back(e.first);
        }
        ourcm = new SeqCM::CoreMaint(n, graph, core);
        ourcm->ComputeCore(graph, core, order_v, false);
        gadget::OutputCoreNumber(path, core, n);
        goto END;
    } else if (2 == outputCore) {
        for (auto e: edges) {
            graph[e.first].push_back(e.second);
            graph[e.second].push_back(e.first);
        }
        ourcm = new SeqCM::CoreMaint(n, graph, core);
        ourcm->ComputeCore(graph, core, order_v, false);
        gadget::OutputSampleEdgeCoreNumber(path, core, edges, n);
        goto END;
    }



    const auto init_beg = std::chrono::steady_clock::now();
    if (method%10 != 3) { //3 is ours method 
      cm->ComputeCore(graph, true, core);
    } else { // ours 
      // compute the k-order by bz. 
      ourcm->ComputeCore(graph, core, order_v, true);
      ourcm->Init(order_v);   // init to our class.
    }
    const auto init_end = std::chrono::steady_clock::now();
    const auto init_dif = init_end - init_beg;
    //printf("initialization costs \x1b[1;31m%f\x1b[0m ms\n",
          //std::chrono::duration<double, std::milli>(init_dif).count());
    printf("initialization costs(ms): %f\n", std::chrono::duration<double, std::milli>(init_dif).count());

    // verify the result, not necessary.
    // {
    //   if (method != 3) {
    //     cm->Check(graph, core);
    //   } else { // ours
    //     ourcm->ComputeCore(graph, tmp_core, order_v);
    //     ourcm->Check(-1, -1, -1, tmp_core, order_v);
    //   }
      
    // }


  if (optRemoveNum > 0) goto REMOVE;

 #if 1 // insert edges
    printf("*********begin insertion %d %d!\n", m2, m);
  const auto ins_beg = std::chrono::steady_clock::now();

 
  if (method%10 != 3) {
    for (int i = m2; i < m; ++i) {
      cm->Insert(edges[i].first, edges[i].second, graph, core);
#ifdef DEBUG
      //cm->ComputeCore(graph, false, tmp_core);
      //ASSERT_INFO(tmp_core == core, "wrong result after insert");
#endif
    }
  } else { // ours
        if (method/10 == 1) {
            int repeat = ourcm->BatchEdgeInsert(edges, m2, m); 
#ifdef DEBUG
                // check each time. 
                ourcm->ComputeCore(graph, tmp_core, order_v, false);
                int r = ourcm->Check(-1, -1, -1, tmp_core, order_v);  
#endif
            printf("batch insertion repeat: %d\n", repeat);
        } else {
            for (int i = m2; i < m; ++i) {
                ourcm->EdgeInsert(edges[i].first, edges[i].second);
#ifdef DEBUG
                // check each time. 
                ourcm->ComputeCore(graph, tmp_core, order_v, false);
                int r = ourcm->Check(edges[i].first, edges[i].second, i - m2, tmp_core, order_v);  
#endif
      }
        }
  }
  const auto ins_end = std::chrono::steady_clock::now();
  const auto ins_dif = ins_end - ins_beg;
  //printf("core insert costs \x1b[1;31m%f\x1b[0m ms\n",
         //std::chrono::duration<double, std::milli>(ins_dif).count());
    printf("core IorR costs(ms): %f\n", std::chrono::duration<double, std::milli>(ins_dif).count());

  /*see count*/
    printf("temp edge number 1: %d\n", cnt_edge);
    printf("temp edge number 2: %d\n", cnt_edge2);
    
    printf("V* size: %d\n", cnt_Vs);
    printf("V+ size: %d\n", cnt_Vp);
    printf("S size: %d\n", cnt_S);
    printf("our adjust tag: %d\n", cnt_tag);
  /*see count end*/


  // verify the result
  {
    ERROR("check: insert", false);
    
    if (method%10 != 3) {
      cm->ComputeCore(graph, false, tmp_core);
      cm->Check(graph, core);
    }else {
      ourcm->ComputeCore(graph, tmp_core, order_v, false);
      ourcm->Check(-1, -1, -1, tmp_core, order_v);
    }

  
    ERROR("check passed", false);

  }
#endif  //end edge insert 

goto END;


REMOVE:
#if 1   // remove edges
  printf("begin remove !\n");
  if (optRemoveNum > 0) { m2 = m - optRemoveNum;} 

    const auto rmv_beg = std::chrono::steady_clock::now();
  
    if (method%10 != 3) {
        for (int i = m - 1; i >= m2; --i) {
            cm->Remove(edges[i].first, edges[i].second, graph, core);
        }
    } else {
      for (int i = m - 1; i >= m2; --i) {
        ourcm->EdgeRemove(edges[i].first, edges[i].second);
#ifdef DEBUG
        ourcm->ComputeCore(graph, tmp_core, order_v, false);
        int r = ourcm->Check(edges[i].first, edges[i].second, i - m2, tmp_core, order_v);  
        // if (r != 0) {
        //   for(auto e: edges){
        //     printf("%d %d\n", e.first, e.second);
        //   }
        //   return 0;
        // }
        //ASSERT_INFO(tmp_core == core, "wrong result after insert");
#endif
      }
    }
  const auto rmv_end = std::chrono::steady_clock::now();
  const auto rmv_dif = rmv_end - rmv_beg;
  //printf("core remove costs \x1b[1;31m%f\x1b[0m ms\n",
        // std::chrono::duration<double, std::milli>(rmv_dif).count());
  printf("core IorR costs(ms): %f\n", std::chrono::duration<double, std::milli>(rmv_dif).count());  

  /*see count*/
    printf("temp edge number 1: %d\n", cnt_edge);
    printf("temp edge number 2: %d\n", cnt_edge2);
    
    printf("V* size: %d\n", cnt_Vs);
    printf("V+ size: %d\n", cnt_Vp);
    printf("S size: %d\n", cnt_S);
    printf("our adjust tag: %d\n", cnt_tag);
  /*see count end*/


  // verify the result
  {
    ERROR("check: remove", false);
    if (method%10 != 3) {
      std::vector<int> tmp_core(n);
      cm->ComputeCore(graph, false, tmp_core);
      ASSERT_INFO(tmp_core == core, "wrong result after remove");
      cm->Check(graph, core);
    } else {
        ourcm->ComputeCore(graph, tmp_core, order_v, false);
        ourcm->Check(-1, -1, -1, tmp_core, order_v);
    }
    ERROR("check passed", false);
  }
#endif // end remove

END:
  if (cm != nullptr) delete cm;
  if (ourcm != nullptr) delete ourcm;
}
