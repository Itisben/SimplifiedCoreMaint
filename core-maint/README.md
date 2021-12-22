## **About the Code** ##

The code implements the **traversal algorithm** proposed by the paper "Streaming Algorithms for k-Core Decomposition" in VLDB'13 and the **order-based core maintenance algorithm**. 

## **File Structure** ##

The code consists of

* **main.cc, test.cc**: the main program and the test program
* **defs.h**: this file defines some useful macros
* **core.h**: this header file defines the base class for core maintenance algorithm
* **gadget**: this folder contains gadgets for reading files and implements several data structures
    * **disjoint.h, disjoint.cc**: a union-find-delete data structure with constant time deletion complexity
    * **gadget.h, gadget.cc**: for reading data files, the format of which will be explained in details later
    * **heap.h, heap.cc**: a minimum heap
    * **sb_tree.h, sb_tree.cc**: the size-balanced tree
    * **treap.h, treap.cc**: the treap (tree + heap)
* **traversal**: this folder contains the code for the traversal core maintenance algorithm
* **glist**: this folder contains the code for the order-based core maintenance algorithm

## **Data File Format** ##

The code assumes the input graph is undirected and simple. In gadget/gadget.h, two functions are provided to read temporal graphs and ordinary graphs, respectively.

* For temporal graphs, the file format is as follows:

        n  m

        u_1 v_1 t_1

        u_2 v_2 t_2

        ......
 
        u_m v_m t_m

That is, the first line of the file specifies the # of vertices (n) and the # of edges (m) in the graph. The following m lines are of the format "u v t", where (u, v) is an edge and "t" is the timestamp. It is required that **t_1 <= t_2 <= ... <= t_m**.

* For ordinary graphs, the file format is same as that of temporal graphs except that no timestamp is required.

## **Command** ##

There are four options to run the code. 

* **-p**: the path of the data file
* **-r**: the proportion of edges that is sampled out for insertion/removal
* **-m**: the algorithm to be used: **glist** or **traversal**
* **-T**: 1 for temporal graphs and 0 for ordinary graphs

**An example command**: ./core -p ./facebook.txt -r 0.05 -m glist -T 1

## **Compiling the Code** ##

Use Makefile included and notice that in default, -O2 optimization option is triggered and the code is compiled based on C++11.