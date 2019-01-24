#include <set>
#include "../gtcGraph.hh"

std::vector<std::vector<unsigned>> computeOrbits_nauty(Graph *G, std::set<unsigned> &V);
std::vector<std::vector<unsigned> > computeOrbitsAgressive_nauty(Graph *G, std::set<unsigned> &V);
std::vector<std::vector<unsigned>> computeOrbits_nautySparse(Graph *G, std::set<unsigned> &V);
