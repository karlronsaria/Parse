#include "graph.h"

size_t Graph::max(size_t first, size_t secnd) {
    return first > secnd ? first : secnd;
}

void Graph::raise_size(size_t newSize) {
    if (size() > newSize)
        return;

    for (int i = 0; i < size(); ++i)
        (*this)[i].raise_size(newSize);

    add_rows(newSize);
}

void Graph::add_rows(size_t newSize) {
    vector_bitset temp;
    temp.raise_size(newSize);
    size_t currentSize = size();

    for (int i = 1; i <= newSize - currentSize; ++i)
        push_back(temp);
}

Graph::Graph(size_t newSize) {
    add_rows(newSize);
}

bool Graph::add(size_t from, size_t to) {
    raise_size(max(from, to) + 1);
    size_t hasEdge = contains(from, to);
    at(from).set(to);
    return hasEdge;
}

bool Graph::contains(size_t from, size_t to) {
    if (max(from, to) + 1 > size())
        return false;

    return at(from).at(to);
}