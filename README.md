
**Description**

gtc (get that clique) is a C++ program used for computing the clique number of graphs exibithing a certain degree of symmetry. Specifically, graphs that have a non-trivial automorphism group and,na more generally, graphs having vertices whose neighborhoods induce isomorphic graphs. Gtc relies on [nauty](http://users.cecs.anu.edu.au/~bdm/nauty/), [bliss](http://www.tcs.hut.fi/Software/bliss/) as well as the clique computing program [mcqd](https://gitlab.com/janezkonc/mcqd).

A variant of this program was crucial for the publication of the papere  [There is no (75, 32, 10, 16) strongly regular graph](https://arxiv.org/abs/1509.05933) and [There is no (95, 40, 12, 20) strongly regular graph](https://arxiv.org/abs/1603.02032)


**Usage**

gtc accepts a file whose lines are graphs in the graph6 string representation form. Its output is a sequence of lines, each line containing the clique number of the coresponding graph in the input. It is trivial to modify gtc to print the respective clique as well.

**Examples**


Computing the clique of a graph of order 592 and density 0.166 with a lower threshold (THRESHOLD_ORDER=150, level=4)

    $ time ./gtc example592 
    20

    real    0m0.076s
    user    0m0.072s
    sys     0m0.000s



Computing the clique of a graph of order 854 with density 0.33 (THRESHOLD_ORDER=150, level=4)

    $ time ./gtc example854 
    22

    real    0m2.561s
    user    0m2.560s
    sys     0m0.000s

Note that setting THRESHOLD_ORDER>=200 drastically increases the time of computation.


Computing the clique of a graph of order 3978 and density 0.251 (THRESHOLD_ORDER=100, level=10)

    $ time ./gtc example3978
    67  

	real	3m20.644s
	user	3m20.580s
	sys	0m0.012s
