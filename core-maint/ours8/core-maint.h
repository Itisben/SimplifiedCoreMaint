#pragma once
#include <vector>
#include <set>
#include <queue>
#include <math.h>       /* pow */
#include <climits>
#include <map>
using namespace std;


//for graph
const size_t MAX_INCREASE_CORE = 1024*16; //max increased core number, should be large enough
const size_t VECTOR_CAPACITY = 1024*1024;
typedef int node_t; // the vertex of graph
const node_t NONE = -1; 
typedef int edge_t; // the edge of graph
typedef int core_t; // the core number of graph 
typedef int deg_t;  // degree
typedef int color_t;
typedef int label_t;
typedef int worker_t;
typedef unsigned int counter_t;
typedef vector<vector<edge_t>> graph_t; 

// in the BackWard function
/*#define WITH_V_BLACK2GRAY*/

/*#define DEBUG_ORDER_LIST */
#ifdef DEBUG_ORDER_LIST
typedef unsigned long long int tag_t; // 64bit label tag;
const size_t MAX_TAG = 0xffffffffffffffff;
const size_t INIT_TAG_GAP = 10; // 32 bit integer
#else 

/*one tag is easy to implemented. 
* So we can use 128 bit tag to replace the double level tags.
* The 128 bit tag can insert 64 item at least between */
#if 1

typedef unsigned long long int tag_t; // 64bit label tag;
typedef unsigned int subtag_t; // 32bit label subtag;
const size_t MAX_TAG = 0xffffffffffffffff;
const size_t INIT_TAG_GAP = 0xffffffff; // 32 bit unsigned integer
const size_t MAX_SUBTAG = 0xffffffff; // 32 bit unsigned integer
/*#define WITH_SUBTAG */ // this is defined in makefile

#else // !!!my machine doesn't support this __int128 well. 
typedef unsigned __int128 tag_t; //128bit label tag;
const size_t MAX_TAG = 0xffffffffffffffffffffffffffffffff;
const size_t INIT_TAG_GAP = 0xffffffffffffffff;
#endif 
//typedef unsigned int subtag_t;

#endif 

/**priority queue for core maint begin*************/
struct DATA {
    node_t key;
    tag_t value;
#ifdef WITH_SUBTAG
    subtag_t value2;
    DATA(node_t key, tag_t value, subtag_t value2) :
        key(key), value(value), value2(value2){ } 
#else
    DATA(node_t key, tag_t value) :
        key(key), value(value){ } 
#endif
};

struct CompareOrder { 
    bool operator()(DATA const& a, DATA const& b) 
    { 
        // return "true" if "a" is ordered  
        // before "b", for example:
#ifdef WITH_SUBTAG
        if (likely(a.value != b.value))
            return a.value > b.value;
        else 
            return a.value2 > b.value2; 
            
#else
        return a.value > b.value;
#endif
    } 
}; 


class PRIORITY_Q {
private:
    typedef std::priority_queue<DATA, std::vector<DATA>, CompareOrder> PQ;
    PQ pq_;

public:
    PRIORITY_Q(size_t size) {
        std::vector<DATA> container;
        container.reserve(size);
        CompareOrder compare;
        pq_ = PQ(compare, std::move(container));
    } 
    PRIORITY_Q(){}
    inline void push(DATA d) { pq_.push(d);}
    inline DATA top() {return pq_.top();}
    inline void pop() {pq_.pop();}
    inline bool empty() {return pq_.empty(); }
    inline void clear() { while (!pq_.empty()){pq_.pop();} }
};
/**priority queue for core maint end *************/

/**priority queue for batchcore maint begin*************/
struct DATA2 {
    node_t key;
    core_t core;
    tag_t tag;
#ifdef WITH_SUBTAG
    subtag_t value2;
    DATA(node_t key, tag_t value, subtag_t value2) :
        key(key), value(value), value2(value2){ } 
#else
    DATA2(node_t key, core_t core, tag_t tag) :
        key(key), core(core), tag(tag){ } 
#endif
};

struct CompareOrder2 { 
    bool operator()(DATA2 const& a, DATA2 const& b) 
    { 
        // return "true" if "a" is ordered  
        // before "b", for example:
#ifdef WITH_SUBTAG
        if (likely(a.value != b.value))
            return a.value > b.value;
        else 
            return a.value2 > b.value2; 
            
#else
        if (a.core == b.core) return a.tag > b.tag;
        else return a.core > b.core;
#endif
    } 
}; 


class PRIORITY_Q2 {
private:
    typedef std::priority_queue<DATA2, std::vector<DATA2>, CompareOrder2> PQ2;
    PQ2 pq_;

public:
    PRIORITY_Q2(size_t size) {
        std::vector<DATA2> container;
        container.reserve(size);
        CompareOrder2 compare;
        pq_ = PQ2(compare, std::move(container));
    } 
    PRIORITY_Q2(){}
    inline void push(DATA2 d) { pq_.push(d);}
    inline DATA2 top() {return pq_.top();}
    inline void pop() {pq_.pop();}
    inline bool empty() {return pq_.empty(); }
    inline void clear() { while (!pq_.empty()){pq_.pop();} }
};
/**priority queue for core maint end *************/

#if 1
/**queue for core maint begin*********************/
// this queue can be vector
class QUEUE {
private:
    std::vector<node_t> container;
public:
    QUEUE(size_t size) {
        container.reserve(size);
    }
    QUEUE() {}
    inline void push(node_t v) {container.push_back(v);}
    inline node_t top() {return container.back();}
    inline void pop() {container.pop_back();}
    inline bool empty() {return container.empty(); }
    inline void clear() { container.clear();}
};
/***queue for core maint end*********************/
#endif
#if 0 // this implementation is more efficient
class QUEUE {
private:
    //std::vector<node_t> container;
    node_t *container = NULL;
    int ptr = 0;
public:
    QUEUE(size_t size) {
        //container = std::vector<node_t>(size);
        container = new node_t[size];
    }
    //~QUEUE(){delete[] container;}
    QUEUE() {}
    inline void push(node_t v) {container[ptr++] = v;}
    inline node_t top() {return container[ptr-1];}
    inline void pop() {ptr--;}
    inline bool empty() {return 0 == ptr; }
    inline void clear() { ptr = 0;}
};
#endif

#if 0
/*this stack is more efficient than the STL stack */
class STACK {
public:
    STACK(int max_sz) {
        stack = new int32_t[max_sz];
        if (NULL == stack) printf("stack is null!\n");
        stack_ptr = 0;
        max_stack = 0;
    }
    STACK() {}
    ~STACK() { if (stack != NULL) {delete[] stack; stack = NULL;}}


    inline void push(node_t n) {
        stack[stack_ptr] = n;
        stack_ptr ++;
         if (stack_ptr > max_stack) {
             max_stack = stack_ptr;
         }
        //max_stack++;
    }
    inline int size()   {return stack_ptr;}
    inline int32_t top() {return stack[stack_ptr - 1];}
    inline void set_top(int32_t v) {stack[stack_ptr - 1] = v;}
    inline void pop()   {stack_ptr--;}
    inline bool empty() {return (stack_ptr == 0);}
    int32_t* get_ptr()  {return stack;}
    int32_t get_max_stack() {return max_stack;}
private:
    int32_t* stack = NULL;
    int32_t stack_ptr;
    int32_t max_stack;
};
#endif 

enum{
    WHITE   = 0, /*initial*/
    DARK, /*visited in PQ wating to be propagated*/
    GRAY    = 1, /*visited, not in V* but in V+*/
    BLACK   = 2, /*visited, in V* and in V+*/
};

namespace SeqCM{
    class Node {
    public:
        //Computer Core number
        deg_t degout; // deg+
        deg_t degin;  // deg*
        deg_t mcd;    //for edge removing.
       
        //Order Maintenance
        node_t pre;  //double linked list
        node_t next; //double linked list
        tag_t tag;   // label tag for each item range is 0 - n^2
#ifdef WITH_SUBTAG
        subtag_t subtag; // range 0 to logn.
#endif

        color_t color; // black, white, gray.
        label_t inQ;   // node is in priority queue PQ
        label_t inR;    // node is in R can be defined with color
        //label_t outR;   // node is already backward.
        Node(){
            degout = degin = mcd = tag = 0; // init to 0 
#ifdef WITH_SUBTAG
            subtag =0;
#endif
            pre = next = NONE;
            color = WHITE;
            inQ = false;
            inR = false; 
        }
    };

    /*first version, with one level tags (label), only tag is used.
    * take O(logn) time for insert*/
    class CoreMaint {
        private:
        size_t n; // the number of vertices in graph.
        core_t max_core; // the max core number
        graph_t& graph;
        vector<core_t>& core;
        vector<Node> V; // node information include head and tail
        // head-> x -> y -> tail (easy for insert and remove)
        
        /*maintaining the order   head(avoid empty list check)->node->node(tail)*/
        vector<core_t> H;  //each core list has a head to avoid empty list  
        vector<core_t> T;  //each core list has a tail;
        
        /*list operation is here*/
        inline void ListDelete(node_t x);
        inline void ListInsert(node_t x, node_t y);
        inline void MultiListInsert(node_t x, vector<node_t> &y);
        //inline void ListLink(vector<node_t> &y); //link all nodes in x.

        /*order operation*/
        inline bool Order(node_t x, node_t y); // return x < y
        inline bool SameCoreOrder(node_t x, node_t y);
        inline bool TagOrder(node_t x, node_t y) {return V[x].tag < V[y].tag;}
        int OrderInsert(node_t x, node_t y); // insert y after x
        int MultiOrderInsert(node_t x, vector<node_t> &y);
        inline int OrderDelete(node_t x);   // remove x

        QUEUE R; 
      
        //std::queue<node_t> R;
        PRIORITY_Q PQ;      // queue backward
        
        PRIORITY_Q2 PQ2;    // for batch insertion.

        vector<node_t> Vblack;  // the order of black nodes (may include gray)
#ifdef WITH_V_BLACK2GRAY
        vector<node_t> Vblack2gray; // for ordered black to gray nodes  
#endif
        vector<node_t> Vcolor; // all colored vertices.
        
        std::set<std::pair<node_t, node_t>> E2; //all visited vertices. this can be optimized !!!

        /*batch insertion */
        //PRIORITY_Q2 PQ2;
        //std::vector<pair<node_t, node_t>> tempedges, tempedges2;
        /*batch insertion end */

        public:
        /*the insert and remove algorithm*/
        CoreMaint(size_t n, graph_t &graph, vector<core_t> &core);
        ~CoreMaint(){}
        void Init(vector<int> &odv);
        void ComputeCore(graph_t &graph, vector<int> &core, vector<int> &order_v, bool with_init);
        
        // for edge insert
        inline void Forward(node_t w);
        inline void Backward(node_t w);
        inline void DoPre(node_t u);
        inline void DoAdj(node_t u);
        int EdgeInsert(node_t x, edge_t y); // insert to double-linked list
        
        inline void BatchForward(node_t w, PRIORITY_Q2 &PQ2);
        inline void BatchBackward(node_t w);
        inline void BatchDoPre(node_t u);
        inline void BatchDoAdj(node_t u);
        int BatchEdgeInsert(std::vector<pair<node_t, node_t>> edges, int m, int m2);

        inline void BeginRemove(node_t u, node_t v);
        inline void BeginRemoveContinue(node_t u, node_t v);
        inline void FindVs(const core_t K);
        inline void AfterRemove(const core_t K);
        int EdgeRemove(node_t x, edge_t y); //remove in double-linked list.
        
        // check the correctness of algorithm
        int Check(node_t, node_t, int id, vector<core_t> &tmp_core, vector<node_t> &order_v); 
    };

    //second version, with two level tags (label)
    // class OrderList2:CoreMaint {

    // };

}




/*this parallel version should put into another file*/
#if 0 
/****************************
 * parallel version of List Order Maintenance
 * by using openmp lock 
 * ***************************/
namespace ParCM{
    class Node {
    public:
        //Computer Core number
        core_t k;
        deg_t degout; // deg+
        deg_t degin;  // deg*
        deg_t mcd;    //for edge removing.
        color_t color;

        //Order Maintenance
        node_t pre;  //double linked list
        node_t next; //double linked list
        
        tag_t tag;   // label tag for each item range is 0 - n^2
        subtag_t subtag; //for sublinked list.

        worker_t p;

        inline void Lock() {omp_set_lock(&lock);}
        inline void Unlock() {omp_unset_lock(&lock);}
        inline bool TestLock() {return omp_test_lock(&lock);}

    private:
        // node locker
        omp_lock_t lock; //for concurrency, each node its locker.
    };

    //first version, with one level tags (label), 
    //using lock simply
    class CoreMaint {
    private:
        int n; // the number of vertices in graph.
        graph_t graph;
        vector<Node> V; // node information
        
        vector<std::atomic<counter_t>> relabel_cnt; //each list has a relabel counter
        vector<Node> head;  //each k has a list with head     
        inline bool Order(node_t x, node_t y, core_t k); // return x < y
        int OrderInsert(node_t x, node_t y); // insert y after x
        int OrderRemove(node_t x);   // remove x


    public:
        CoreMaint(const size_t n);
        int EdgeInsert(const node_t x, const edge_t y); // insert to double-linked list
        int EdgeRemove(node_t x, edge_t y); //remove in double-linked list.

    };

    //section version, with two level tages 
    class CoreMaint2 {

    }

}
#endif

