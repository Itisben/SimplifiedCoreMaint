#ifndef CORE_GADGET_GADGET_H_
#define CORE_GADGET_GADGET_H_

#include <utility>
#include <vector>
#include <time.h>

namespace gadget {
void RepeatWith(const char symbol, const int repeat);
std::vector<std::vector<int>> ReadGraph(const char* const path,
                                        int* const n, int* const m);
std::vector<std::pair<int, int>> ReadTempEdgesS(const char* const path,
                                                int* const n, int* const m);
std::vector<std::pair<int, int>> ReadEdgesS(const char* const path,
                                            int* const n, int* const m);

std::vector<std::pair<int, int>> ReadEdges(const char* const path,
                                            int* const n, int* const m);
                                            
std::vector<std::pair<int, int>> CSRReadEdges(char* const path,
                    int* const n, int* const m, int shuffle); 

void OutputCoreNumber(const char* const path, std::vector<int> &core, int n);


void OutputSampleEdgeCoreNumber(const char* const path, std::vector<int> &core, 
    std::vector<std::pair<int, int>> &edges, const int n);

std::vector<std::pair<int, int>> sampleEdges(const char* const path, std::vector<std::pair<int, int>> &edges, const float percent);

void CutEdges(std::vector<std::pair<int, int>> &edges, const int num);

}  // namespace gadget

#endif
