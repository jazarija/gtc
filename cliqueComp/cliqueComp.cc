#include "mcqd/mcqd.h"
#include "cliqueComp.hh"

/* Fixme. We want this to be an unsigned int, 
   so mcqd should be corrected accordingly. */

std::vector<unsigned> computeMaxClique_mcqd(Graph *G, std::set<unsigned> &V, const unsigned expectedClique) {

    unsigned cliqueOrder, *maxClique;
    
    unsigned i;

    unsigned n = V.size();

    if (n <= 1) {
        return std::vector<unsigned>(V.begin(), V.end());
    }

    bool **conn = new bool*[n];
    unsigned dict[n];

    for (i = 0; i < n ; i++) {
        conn[i] = new bool[n]{};
    }

    unsigned x = 0;

    for (auto i = V.begin(); i != V.end(); ++i) {
        dict[x] = *i;

        unsigned y = x+1;
        for (auto j = i; ++j != V.end(); y++) {
            if ( G->has_edge(*i, *j) ) {
                conn[x][y] = conn[y][x] = true;                
            }
        }                
        x++;
    }

    Maxclique md(conn, n, 0.025, expectedClique);

    md.mcqdyn(maxClique,  cliqueOrder);
    
    std::vector<unsigned> ret = std::vector<unsigned>(cliqueOrder);
    
    for (i = 0; i < cliqueOrder; i++) {
        ret[i] = dict[ maxClique[i] ];
    }

    delete[] maxClique;
 
    for (i = 0; i < n ; i++) {
        delete [] conn[i];
    }
    delete[] conn;

    return ret;
}
