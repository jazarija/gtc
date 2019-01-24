#include "DisjointForrest.hh"

DisjointForrest::DisjointForrest(unsigned n) {

    disjointSet = new int[n];
    numTrees = n;   

    /* FIXME make this faster */       
    for (unsigned i = 0; i < n; i++) {
        disjointSet[i] = -1;
    }
}


/* FIXME Get rid of recursion */
int DisjointForrest::getRoot(int x) {

    if ( disjointSet[x] < 0 ) {
        return x;
    }

    return getRoot(disjointSet[x]);
}

void DisjointForrest::makeUnion(int x, int y)
{
    int xroot = getRoot(x);
    int yroot = getRoot(y);

    if ( xroot == yroot ) {
        return;
    }

    numTrees--;

    if ( disjointSet[xroot] < disjointSet[yroot] ) {
        disjointSet[yroot] = disjointSet[yroot] + disjointSet[xroot];
        disjointSet[xroot] = yroot;
    } else {
        disjointSet[xroot] = disjointSet[xroot] + disjointSet[yroot];
        disjointSet[yroot] = xroot;
    }
}

DisjointForrest::~DisjointForrest() {
    delete[] disjointSet;
}
