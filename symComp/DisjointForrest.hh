class DisjointForrest {

    int *disjointSet;
    
public:
    unsigned numTrees;

    DisjointForrest(unsigned n);
    ~DisjointForrest();

    void makeUnion(int x, int y);
    int getRoot(int x);
};
