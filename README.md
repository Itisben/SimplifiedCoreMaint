This is the c++ code for the draft paper "Simplified Algorithms for Order-Based Core Maintenance". The link is: https://link.springer.com/article/10.1007/s11227-024-06190-x
If you cannot download, please see my homepage: https://bin-guo.github.io/


The experiments test two methods, the order-based method and our simplified order-based method.


# The Structure of the Folder

1. ```~/core-maint/``` : include the source code.
* **core-maint.zip**: the source code for the order-based method. 
* **maindebug.cc**: the main program 
* **defs.h**: this file defines some useful macros
* **core.h**: this header file defines the base class for core maintenance algorithm
* **gadget**: this folder contains gadgets for reading files and implements several data structures
    * **disjoint.h, disjoint.cc**: a union-find-delete data structure with constant time deletion complexity
    * **gadget.h, gadget.cc**: for reading data files, the format of which will be explained in details later
    * **heap.h, heap.cc**: a minimum heap
    * **sb_tree.h, sb_tree.cc**: the size-balanced tree
    * **treap.h, treap.cc**: the treap (tree + heap)
* **traversal**: this folder contains the code for the traversal core maintenance algorithm
* **gm_graph**: this folder is to generate a library that can support loading the CSR format of graphs. 
* **glist**: this folder contains the code for the order-based core maintenance algorithm
* **ours8**: this folder contains the code for our simplified order-based core maintenance algorithm
3. ```~/experiment/```: include all the experiment scripts.
* **benchmark.sh**: the shell script that can run the experiment and get the test result as csv files.
* **paser-output.py**: the python script called by benchmark.sh to parse the test result.
* **graph**: this folder contains the tested example graphs.
* **results**: this folder contains all the tested results. 
* **convert**: this folder contains all the python script that transfer the test results to the figures and table that can be included in the paper.

# Compiling and Executing

1. The ubuntu 18.04 with g++ are used for compiling.

1. Compile the gm_graph runtime library and the scc binary by running make in the directory ```~/core-maint/gm_graph```.

1. Compile the source code by running make in the directory ```~/core-maint```.

1. To execute application, in the directory ```~/core-main```, run:

* For insert edges: ```./core -p <graph_path> -I <number_edges> -m <method> -T <graph_format>```
* For remove edges: ```./core -p <graph_path> -R <number_edges> -m <method> -T <graph_format>```
* method includes (-m): 2 order-based, 1 traversal, 3 ours (13 our batch insertion)
* graph format (-T): 1 for temporal graphs, 0 for ordinary graphs sample edges, 2 for debug without sampled edges, 3 csr sample edges, 4 csr without sample edges, 5 csr with repeated random

1. To execute application with special number of sampled edges in graph, in the directory ```~/core-main```, run:
* For insert edges: ```./core -p <graph_path> -I <number_edges> -m <method> -T <graph_format> -s <number_edges>```
* For remove edges: ```./core -p <graph_path> -R <number_edges> -m <method> -T <graph_format> -s <number_edges>```
* The nubmber of sampled edges (-s): 100k, 200k, 400k, 800k, ....

# Executing with Script

1. By varying the chunk size, the experiment can run with a script ```~/experiments/our-benchmark-parstep.sh```. Before running, you should renew the paths of all tested graphs. 

1. By varying the workers from 1 to 32, the experiments can run with a script ```~/experiments/our-benchmark.sh```. 

1. By sampling the edges and vertices, the experiments can run with a script ```~/experiments/our-benchmark-sample.sh```.



# Graph Conversion Tool
There is a graph conversion utility to convert the adjacency list or edge list graph files into the binary format used by the current application. 

* Before conversion, you can use the python script ```~/experiment/convert/format.py``` to remove the repeated edges.

* To execute this tool, in the directory ```~/experiment/convert```, run:

```./convert <InputName> <OutputName> [ -?=<0/1> -GMInputFormat=<string> -GMMeasureTime=<0/1> -GMNumThreads=<int> -GMOutputFormat=<string> ]```

Calling ./convert with no input parameters will print the help, which describes all of the options. 




