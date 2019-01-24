#include "symCompNauty.hh"

#include <nauty.h>
#include <nautinv.h>
#include <nausparse.h>

#include <string>
#include <map>

/* Remove @ some point */
#include <stdlib.h>
#include <iostream>
#include <assert.h>

std::vector<std::vector<unsigned>> computeOrbits_nauty(Graph *G, std::set<unsigned> &V) {

    DYNALLSTAT(graph,g,g_sz);
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);

    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;

    unsigned m;
    unsigned n = V.size();

    /* In case nauty breaks  
    */

    options.invarproc = twopaths;
    m = SETWORDSNEEDED(n);

    DYNALLOC2(graph,g,g_sz,m,n,"malloc failed");
    DYNALLOC1(int,lab,lab_sz,n,"malloc failed");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc failed");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc failed");

    EMPTYGRAPH(g,m,n);

    unsigned dict[V.size()];
    unsigned x = 0;
    for (auto i = V.begin(); i != V.end(); ++i) {
        dict[x] = *i;

        unsigned y = x+1;
        for (auto j = i; ++j != V.end(); y++) {
            if (G->has_edge(*i, *j)) { 
                ADDONEEDGE(g,x,y,m);
            }
        }                
        x++;
    }

    densenauty(g,lab,ptn,orbits,&options,&stats,m,n,NULL);

    std::vector< std::vector<unsigned> > ret(stats.numorbits);

    int freeSlot[n];

    memset(freeSlot, -1, sizeof(int)*n);
    unsigned curFree = 0;

    for (unsigned i = 0; i < n; i++) {
        if (freeSlot[orbits[i]] == -1) {
            freeSlot[orbits[i]] = curFree++;
        }
        ret[ freeSlot[orbits[i]] ].push_back( dict[i] );
    }

    DYNFREE(orbits, orbits_sz);
    DYNFREE(ptn, ptn_sz);
    DYNFREE(lab, lab_sz);
    DYNFREE(g, g_sz);

    return ret;
}

/* FIXME. This is just for demonstrational purposes. This part is extremely ugly. */
std::vector<std::vector<unsigned> > computeOrbitsAgressive_nauty(Graph *G, std::set<unsigned> &V) {
   
    std::vector<std::vector<unsigned> > orb = computeOrbits_nauty(G, V);

    if (orb.size() == 1) {
        return orb;
    }
    
    printf("Started with %lu orbits \n", orb.size() );

    std::vector<std::vector<unsigned> > nbrs = std::vector<std::vector<unsigned>>(orb.size());
    unsigned order = V.size();
    unsigned i,j,k;
    unsigned size = 0;

    for (i = 0; i < orb.size() ; i++) {
        for (unsigned v: V) {
            if (G->has_edge(orb[i][0], v)) {
                nbrs[i].push_back(v);
            }
        }
        size += nbrs[i].size();
    }

    DYNALLSTAT(graph, g, g_sz);
    DYNALLSTAT(int, lab, lab_sz);
    DYNALLSTAT(int, ptn, ptn_sz);
    DYNALLSTAT(int, orbits, orbits_sz);

    static DEFAULTOPTIONS_GRAPH(options);
    statsblk stats;

    unsigned m;
    unsigned n = order+size; 

    m = SETWORDSNEEDED(n);

    DYNALLOC2(graph, g, g_sz, m, n, "malloc failed");
    DYNALLOC1(int, lab, lab_sz, n, "malloc failed");
    DYNALLOC1(int, ptn, ptn_sz, n, "malloc failed");
    DYNALLOC1(int, orbits, orbits_sz, n, "malloc failed");

    unsigned localDict[order];

    unsigned curFreeVert = orb.size();
    EMPTYGRAPH(g,m,n);

    for (i = 0; i < nbrs.size(); i++) {
        memset(localDict, 0, order*sizeof(unsigned));

       /* We create the neighbor graph of vertex v */
        for (j = 0; j < nbrs[i].size(); j++) {
            if (localDict[j] == 0) {
                localDict[j] = curFreeVert++;
            }                   
 
            ADDONEEDGE(g,i,localDict[j],m);
            for (k = j+1; k < nbrs[i].size(); k++) {

                if (localDict[k] == 0) {
                    localDict[k] = curFreeVert++;
                }

                if (G->has_edge( nbrs[i][j] , nbrs[i][k])) {
                    ADDONEEDGE(g, localDict[k], localDict[j], m);           
                } 
            }
        }
    }

    nbrs.clear();

    densenauty(g,lab,ptn,orbits,&options,&stats,m,n,NULL);
    int *pos = new int[order+size];
    
    memset(pos, -1, sizeof(int)*(order+size));

    unsigned cur_free = 0;
        
    std::vector<std::vector<unsigned>> localOrbits = std::vector<std::vector<unsigned> >(order+size);

    /* TODO. It would be nice to make this more efficient. */
    for (unsigned i = 0; i < orb.size(); i++) {
         unsigned root = (unsigned) orbits[i];

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
            ret[i].insert(ret[i].end(), orb[ localOrbits[i][j] ].begin(), orb[ localOrbits[i][j] ].end());            
        }
    }

    printf("Ended with %lu orbits \n", ret.size() );
    delete[] pos;
    return ret;
}   

/* 
    INPUT:
        - Two sparse graphs `sg' and `cg' being able to hold a graph of order V.size() 
        - Our main Graph `G'
        - A subset of vertices `V' of G.
        - A dictionary `dict' of how vertices from V map to vertices of G

    OUTPUT:
        - A canonical certificate for the subgraph of G induced by V. 
 
    FIXME:  
        
        - Use pointers !! 
*/
static std::string canonicalCertificate(sparsegraph *sg, sparsegraph *cg, std::vector<unsigned> &V, Graph *G, unsigned *dict) {


    unsigned n = V.size();
    sg->nv = n;
    sg->nde= 0;
    if (n <= 1) {
        return std::to_string(n);
    }
    std::vector<std::vector<unsigned> > nbrs = std::vector<std::vector<unsigned>>(n);

    unsigned e = 0;
    for (unsigned i = 0; i < n; i++) {
        for (unsigned j = i+1; j < n; j++) {
            if ( G->has_edge( dict[V[i]], dict[V[j]] )) {
               nbrs[i].push_back(j);
               nbrs[j].push_back(i);
               sg->nde+=2;
            }
        }
        /* ko sem enkrat tu, vem degree za vertex 0 so I may as well fix the guy up.! */
        sg->d[i] = nbrs[i].size();
        sg->v[i] = e;
        for (unsigned k = 0; k < nbrs[i].size(); k++) {
            sg->e[e++] = nbrs[i][k];
        }
    }
    nbrs.clear();

    DEFAULTOPTIONS_SPARSEGRAPH(options);
    options.getcanon = TRUE;

    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);

    statsblk stats;
    
    DYNALLOC1(int,lab,lab_sz,n,"malloc failed");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc failed");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc failed");

    sparsenauty(sg,lab,ptn,orbits,&options,&stats, cg);

    std::string s = std::to_string(n) + ",";
    for (unsigned i = 0; i < (unsigned) cg->nv ; i++) {
        for (unsigned j = 0; j < (unsigned) cg->d[i]; j++) {
            if ( i < (unsigned) cg->e[cg->v[i] + j]) {
                s += "(" + std::to_string(i) + "," + std::to_string( cg->e[ cg->v[i] + j ] )+"),";
            /* According to nauty's manually the neighbors of each vertex are sorted */                    
            } else {
                /* TODO CAN WE BREAK? */
            }
        }
    }
//    std::cout << "Order " << cg->nv << s << std::endl;

    DYNFREE(lab, lab_sz);
    DYNFREE(ptn, ptn_sz);
    DYNFREE(orbits, orbits_sz);

    return s;
}
/* TODO document.

   ask people on C++ what would they improve 
*/
std::vector<std::vector<unsigned>> computeOrbits_nautySparse(Graph *G, std::set<unsigned> &V) {

    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);

    DEFAULTOPTIONS_SPARSEGRAPH(options);
    statsblk stats;
    sparsegraph sg;
    sparsegraph cg;

    unsigned n = V.size();
    unsigned m = SETWORDSNEEDED(n);

    SG_INIT(sg);
    SG_INIT(cg);

    /* FIXME. PERHAPS I CAN REMOVE THIS ? */
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

    DYNALLOC1(int,lab,lab_sz,n,"malloc failed");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc failed");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc failed");

    std::vector<std::vector<unsigned>> nbrs = std::vector<std::vector<unsigned>>(n);
    

    unsigned dict[V.size()];
    
    unsigned size = 0;

    unsigned x = 0;
    for (auto i = V.begin(); i != V.end(); ++i) {
        dict[x] = *i;

        unsigned y = x+1;
        for (auto j = i; ++j != V.end(); y++) {

            if (G->has_edge(*i, *j)) { 
                nbrs[x].push_back(y);
                nbrs[y].push_back(x);
                size+=2;
            }
        }                
        x++;
    }

    SG_ALLOC(sg, n, size, "malloc failed");
    SG_ALLOC(cg, n, size, "malloc failed");

    sg.nv = n;
    sg.nde = size;
     
    x = 0;
    unsigned e = 0;
    for (unsigned i = 0; i < nbrs.size() ; i++) {
        sg.d[i] = nbrs[i].size();
        sg.v[i] = e;
        for (unsigned j = 0; j < nbrs[i].size(); j++) {
            sg.e[e++] = nbrs[i][j];
        }                
    }

    sparsenauty(&sg,lab,ptn,orbits,&options,&stats, NULL);

    std::vector< std::vector<unsigned> > ret(stats.numorbits);
    int freeSlot[n];

    memset(freeSlot, -1, sizeof(int)*n);
    unsigned curFree = 0;

    std::map<std::string, std::vector<unsigned>> mergedOrbits;

    for (unsigned i = 0; i < n; i++) {
        if (freeSlot[orbits[i]] == -1) {
            freeSlot[orbits[i]] = curFree++;
        }
        ret[ freeSlot[orbits[i]] ].push_back( i );
    }
    /* zdej ti imas stevila od 0.... n-1 ki ti oznacujejo orbite. */
    for (unsigned i = 0; i < ret.size() ; i++) {
        auto o = ret[i];
        unsigned v = o[0];
        std::string s = canonicalCertificate(&sg, &cg, nbrs[v], G, dict);

        if (mergedOrbits.find(s) == mergedOrbits.end()) {
            mergedOrbits[s] = std::vector<unsigned>();
            mergedOrbits[s].push_back(i);
        } else {
            auto f = mergedOrbits.find(s);
            f->second.push_back(i);
        }
    }
    auto orb = std::vector< std::vector<unsigned> >( mergedOrbits.size() );
    
    unsigned c = 0;

    for (auto it = mergedOrbits.begin(); it != mergedOrbits.end(); ++it) {
        orb[c] = std::vector<unsigned>();
        for (unsigned i: it->second) {
            for (unsigned v: ret[i]) {
                orb[c].push_back( dict[v] ) ;
            }                     
        }
        c++;
    }

    DYNFREE(orbits, orbits_sz);
    DYNFREE(ptn, ptn_sz);
    DYNFREE(lab, lab_sz);

    SG_FREE(sg);
    SG_FREE(cg);
    return orb;
}


