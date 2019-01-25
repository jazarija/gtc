#include "gtcSolver.hh"
#include "symCompBliss.hh"
#include "symCompNauty.hh"
#include "cliqueComp.hh"

#include <stdio.h>
#include <stdlib.h>

#define LEVEL 10 /* Hard stop on the branching process that computes cliques recursively. Experiment with a lower level if the graph is sparse */
#define THRESHOLD_ORDER 100 /* Lower limit after which we simply invoke the standard clique solving algorithm */
#define EXPECTED_CLIQUE 67 /* Use this if you know that the underying graph is supposed to have a clique of size at least >= EXPECTED_CLIQUE. Note this
should always be greater than LEVEL ! */
//#define VERBOSE 1 /* Uncoment definition to obtain information about the compuation of the clique */

gtcSolver::gtcSolver(Graph *G) {

    N = THRESHOLD_ORDER;
    this->G = G;
}

unsigned pickMinDegOrbit(vector <vector <unsigned> > o, Graph *G, set<unsigned> &V)  {

    unsigned long cur_max = V.size();
    unsigned ret = 0;

    for (unsigned i = 0; i < o.size() ; i++) {
        unsigned deg = 0;
        for (unsigned v: V) {
            if (G->has_edge(v, o[i][0]) ) {
                deg++;
            }                    
        }
        if ( deg < cur_max) {
            cur_max = deg;
            ret = i;
        }
    }
    return ret;
}


/* Returns a (indexed) representative of the largest orbit of orb */
unsigned pickMaxOrbit(vector <vector <unsigned> > o) {

    unsigned long cur_max = 0;
    unsigned ret = 0;

    for (unsigned i = 0; i < o.size() ; i++) {
        if (o[i].size() > cur_max) {
            cur_max = o[i].size();
            ret = i;
        }
    }
    return ret;
}


/* Returns the neighbors of v in the subgraph induced by V */
set<unsigned> gtcSolver::neighbors(set<unsigned> &V, unsigned v) {

    set <unsigned> ret = set<unsigned>();

    for (unsigned el : V) {
        if (G->has_edge(v, el)) {
            ret.insert(el);
        }
    }
    return ret;
}

/* Find the maximal clique in the subgraph induced by the vertices of V */
vector<unsigned> gtcSolver::solve(set<unsigned> V, unsigned level = 0) {

    if (V.size() <= 1) {
        return vector<unsigned> (V.begin(), V.end());
    } 

    vector <unsigned> cl;

    vector< vector<unsigned> > orbits;
    vector <unsigned> cmax_clique = vector<unsigned>();
   
    if (level>=LEVEL) {
        goto out;
    }
    if (V.size() > N) {
        orbits = computeOrbits_nauty(G, V); 
    }
    while (V.size() > N) {

        unsigned o = pickMaxOrbit(orbits); 
        unsigned v = orbits[o][0];
        set<unsigned> U = neighbors(V, v);

        if (U.size() > 0) {
#ifdef VERBOSE
            unsigned kk; 
            for (kk = 0; kk < level; kk++) {
                putchar('\t');
            }   
            printf("%u-%u : Got orbit of length %lu giving graph of order %lu. \n", level, V.size(), orbits[o].size(), U.size());
#endif


            cl = solve(U, level+1);

            if (cl.size() + 1 > cmax_clique.size()) {
                cmax_clique = cl;
                cmax_clique.push_back(v);
            }
        }

        /* Delete the orbit from the set of vertices - very inefficient. Improvement needed. */ 
        for (unsigned el: orbits[o]) {
            V.erase(el);
        }           

        orbits.erase(orbits.begin()+o);
    }
out:    
    cl = computeMaxClique_mcqd(G, V, EXPECTED_CLIQUE-level);
    
    return cl.size() > cmax_clique.size() ? cl : cmax_clique;    
}
