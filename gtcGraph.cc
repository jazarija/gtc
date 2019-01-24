#include "gtcGraph.hh"

void Graph::add_edge(const unsigned i, const unsigned j) {
    adj[i][j] = adj[j][i] = true;
    sz++;
}

bool Graph::has_edge(const unsigned i, const unsigned j) {
    return adj[i][j];
}

unsigned Graph::order(void) {
    return ord;
}

bool Graph::isDense(void) {
    return dense;
}

void Graph::setDensity(void) {
    dense = (sz > ord*(ord-1)/4) ? true : false;
}

Graph::Graph(unsigned n) {

    ord = n;

    adj = std::vector<std::vector<bool> >(n);

    for (unsigned i = 0; i < n; i++) {
        adj[i] = std::vector<bool>(n);
    }
}

Graph::~Graph() {

}
