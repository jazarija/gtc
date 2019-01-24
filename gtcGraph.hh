#ifndef GTC_GRAPH
#define GTC_GRAPH

#include <vector>

class Graph {

    std::vector< std::vector<bool> > adj;

    unsigned ord;
    unsigned sz;
    bool dense = true;

public :
    /* FIXME. Move out of public */
    /* FIXME. Move out of public */

    
    Graph(const unsigned n);
    ~Graph();

    void add_edge(const unsigned i, const unsigned j);
    bool has_edge(const unsigned i, const unsigned j);

    bool isDense(void);
    void setDensity(void);

    unsigned order(void) ;
};

#endif
