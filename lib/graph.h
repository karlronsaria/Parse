#pragma once
#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "VectorBitset.h"

class Graph: public std::vector<vector_bitset<DEFAULT_BITSET_SIZE>> {
private:
    void raise_size(size_t);
    void add_rows(size_t);
public:
    static size_t max(size_t, size_t);
    Graph(size_t newSize = 0);
    bool add(size_t, size_t);
    bool contains(size_t, size_t);
};

#endif
