#include <set>
#include "gtcGraph.hh"

using namespace std;

class gtcSolver {

    Graph *G;

    vector <unsigned> maxClique;
public:

    unsigned N;
    gtcSolver(Graph *G);
    vector<unsigned> solve(set<unsigned> V, unsigned level);

    set<unsigned> neighbors(set<unsigned> &V, unsigned v);
};
