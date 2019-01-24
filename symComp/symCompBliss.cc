#include <string.h>

#include "graph.hh"

#include "DisjointForrest.hh"
#include "symCompBliss.hh"

static void mergeOrbits(void *user_param, unsigned n, const unsigned *aut) {

    /* FIXME. This was int before. Make sure it actually still works. 
        
       REMOVE after thorrow check.
    */
    unsigned tmp,marker,cur;
    bool done[n];
    
    tmp = marker = cur = 0;
    /* FIXME. Get rid of this */ 
    memset(done, false, sizeof(bool)*n);

    DisjointForrest *orb = (DisjointForrest *) user_param;

    while (1) {
        while (cur < n && done[cur]) {
            cur+=1;
        }            

        if (cur == n) {
            break;
        }         

        marker = tmp = cur;
        done[cur] = true;

        while (aut[tmp] != marker) {
            
            orb->makeUnion(cur, aut[tmp]);
            tmp = aut[tmp];
            done[tmp] = true;
        }
    }
}

/* Compute the orbits of the subgraph induced by V */
std::vector< std::vector<unsigned> > computeOrbits_bliss(Graph *G, std::set<unsigned> &V) {

    unsigned order = V.size();

    std::vector<std::vector<unsigned> > orbits;

    DisjointForrest *orb = new DisjointForrest(order);

    bliss::Stats s;
    bliss::Graph *blissGraph = new bliss::Graph(order);

    unsigned dict[order];

    unsigned x = 0;

    for (auto i = V.begin(); i != V.end(); ++i) {
        dict[x] = *i;

        unsigned y = x+1;
        for (auto j = i; ++j != V.end(); y++) {
            if (G->has_edge(*i, *j)) {
                blissGraph->add_edge(x, y);
            }
        }                
        x++;
    }

    blissGraph->find_automorphisms(s, mergeOrbits, orb);    

    unsigned i;
    int pos[order];
    
    memset(pos, -1, sizeof(int)*order);

    unsigned cur_free = 0;
        
    orbits = std::vector<std::vector<unsigned> >(orb->numTrees);

    /* TODO. It would be nice to make this more efficient. */
    for (i = 0; i < order; i++) {
         unsigned root = (unsigned) orb->getRoot(i);
         
         if (pos[root] == -1) {
            pos[root] = cur_free;
            cur_free++;
         } 
         orbits[pos[root]].push_back( dict[i] );
    }        

    delete blissGraph;   
    delete orb;

    return orbits;
}

/* FIXME. This is just for demonstrational purposes. This part is extremely ugly. */
std::vector<std::vector<unsigned> > computeOrbitsAgressive_bliss(Graph *G, std::set<unsigned> &V) {
   
    std::vector<std::vector<unsigned> > orbits = computeOrbits_bliss(G, V);
    
    if (orbits.size() == 1) {
        return orbits;
    }
    unsigned order = orbits.size();
    unsigned i,j,k;
    unsigned size = 0;

    std::vector<std::vector<unsigned> > nbrs = std::vector<std::vector<unsigned>>(order);
  
    for (i = 0; i < order ; i++) {
        for (unsigned v: V) {
            if (G->has_edge(orbits[i][0], v)) {
                nbrs[i].push_back(v);
            }
        }
        size += nbrs[i].size();
    }

    bliss::Stats s;
    bliss::Graph *blissGraph = new bliss::Graph(order + size);
    unsigned localDict[order+size];

    unsigned curFreeVert = order;

    for (i = 0; i < nbrs.size(); i++) {
        memset(localDict, 0, order*sizeof(unsigned));

        /* We create the neighbor graph of vertex v */
        for (j = 0; j < nbrs[i].size(); j++) {
            if (localDict[j] == 0) {
                localDict[j] = curFreeVert++;
            }                   
 
            blissGraph->add_edge(i, localDict[j]);
            for (k = j+1; k < nbrs[i].size(); k++) {

                if (localDict[k] == 0) {
                    localDict[k] = curFreeVert++;
                }

                if (G->has_edge(nbrs[i][j], nbrs[i][k])) {
                    blissGraph->add_edge(localDict[k], localDict[j]);           
                } 
            }
        }
    }
    nbrs.clear();

    DisjointForrest *orb = new DisjointForrest(order+size);

    blissGraph->find_automorphisms(s, mergeOrbits, orb);    
    
    int pos[order+size];
    
    memset(pos, -1, sizeof(int)*(order+size));

    unsigned cur_free = 0;
        
    std::vector<std::vector<unsigned>> localOrbits = std::vector<std::vector<unsigned> >(order+size);

    /* TODO. It would be nice to make this more efficient. */
    for (unsigned i = 0; i < orbits.size(); i++) {
         unsigned root = (unsigned) orb->getRoot(i);

         if (pos[root] == -1) {
            pos[root] = cur_free;
            cur_free++;
         } 
         localOrbits[pos[root]].push_back(i);
    }        
    
    localOrbits.resize(cur_free);

    std::vector<std::vector<unsigned>> ret = std::vector<std::vector<unsigned>>(cur_free);

    for (i = 0; i < localOrbits.size(); i++) {
        for (j = 0; j < localOrbits[i].size(); j++) {
            ret[i].insert(ret[i].end(), orbits[ localOrbits[i][j] ].begin(), orbits[ localOrbits[i][j] ].end());            
        }
    }

    delete blissGraph;
    delete orb;

    return ret;
}   
