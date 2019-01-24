#include <stdio.h>
#include <assert.h>
#include <map>
#include <iostream>

#include <stdlib.h>
#include <string.h>

#include "cliqueComp.hh"
#include "gtcSolver.hh"

#define BIAS6 63
#define TOPBIT6 32
#define SMALLN 62

#define G6LEN(n)  (((n)*((n)-1)/2+5)/6+1) 

using namespace std;
int graphsize(char *s) {

    char *p;
    int n;

    p = s; 
    n = *p++ - BIAS6;

    if (n > SMALLN) 
    {    
        n = *p++ - BIAS6;
        if (n > SMALLN)
        {
            n = *p++ - BIAS6;
            n = (n << 6) | (*p++ - BIAS6);
            n = (n << 6) | (*p++ - BIAS6);
            n = (n << 6) | (*p++ - BIAS6);
            n = (n << 6) | (*p++ - BIAS6);
            n = (n << 6) | (*p++ - BIAS6);
        }
        else
        {
            n = (n << 6) | (*p++ - BIAS6);
            n = (n << 6) | (*p++ - BIAS6);
        }
    }    
    return n;
}

static Graph *stringtomat(char *s, set<unsigned> &V) {

	char *p;
	int i,j,k,x = 0;

    int n = graphsize(s);
    p = s + 1;
    k = 1;

    if (n > SMALLN)
        p+=3;

    Graph *G = new Graph(n);
    
    V.insert(0);

    for (j = 1; j < n; ++j) {
        V.insert(j);
        for (i = 0; i < j; ++i) {
            if (--k == 0) {
        		k = 6;
		        x = *(p++) - BIAS6;
            }
	    
            if (x & TOPBIT6) {
                G->add_edge(i,j);
            }
            x <<= 1;
        }
    }
    return G;
}



int main(int argc, char **argv) {

    FILE *file = stdin;

    assert(argc == 2);

    if (argc == 2) {
        file = fopen(argv[1], "r");
        assert(file);
    }

    set<unsigned> st;
    char *line = NULL;
    ssize_t read;
    size_t len = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        Graph *G = stringtomat(line,st);
        gtcSolver x = gtcSolver(G); 
        vector<unsigned> vec =  x.solve(st,0);
        cout << vec.size() << endl;
        st.clear();
        delete G;

    }

    free(line);
    return 0;
}   
