/*
    Copyright 2007-2012 Janez Konc 

    If you use this program, please cite: 
    Janez Konc and Dusanka Janezic. An improved branch and bound algorithm for the 
    maximum clique problem. MATCH Commun. Math. Comput. Chem., 2007, 58, 569-590.

    More information at: http://www.sicmm.org/~konc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MCQD
#define MCQD

#include <iostream>
#include <algorithm>
#include <assert.h>

class Maxclique {

	const bool* const* e;
	unsigned pk, level;
	const float Tlimit;

    unsigned expectedClique;
	class Vertices {
		
		class Vertex {
			unsigned i, d;
			
			public:
			void set_i(const unsigned ii)  { 
				
				i = ii;
			}
			
			unsigned get_i() const { 
				return i;
			}
			
			void set_degree(unsigned dd) { 
				
				d = dd;
			}
			
			unsigned get_degree() const { 
				return d;
			}
		};
		
		Vertex *v;
		unsigned sz;
		
		static bool desc_degree(const Vertex vi, const Vertex vj) {
			
			return (vi.get_degree() > vj.get_degree());
		}
		
		public:

		Vertices(unsigned size) : sz(0) { 
			
			v = new Vertex[size];
		}
		
		~Vertices () {}
		
		void dispose() { 
			
			if (v) delete [] v;
		}
		
		void sort() { 
		
			std::sort(v, v+sz, desc_degree);
		}
		
		void init_colors();
		
		void set_degrees(Maxclique&);
		
		unsigned size() const { 
			return sz;
		}
		
		void push(const unsigned ii) { 
			v[sz++].set_i(ii);
		};
		
		void pop() { 
			sz--;
		};
		
		Vertex& at(const unsigned ii) const { 
			return v[ii];
		};
		
		Vertex& end() const { 
			
			return v[sz - 1];
		};
	};
	
	class ColorClass {
		
		unsigned *i;
		unsigned sz;
		
		public:

		ColorClass() : i(0), sz(0) {}
		
		ColorClass(const unsigned sz) : i(0), sz(sz) { 
		
			init(sz);
		}
		
		~ColorClass() { 
			if (i) delete [] i;
		}
		
		void init(const unsigned sz) { 
			
			i = new unsigned[sz]; rewind();
		}
		
		void push(const unsigned ii) { 
		
			i[sz++] = ii;
		};
		
		void pop() { 
		
			sz--;
		};
		
		void rewind() { 
		
			sz = 0; 
		};
		
		unsigned size() const { 
			
			return sz;
		}
		
		unsigned& at(const unsigned ii) const { 
			
			return i[ii];
		}
		
		ColorClass& operator=(const ColorClass& dh) {
			
			for (unsigned j = 0; j < dh.sz; j++) 
				i[j] = dh.i[j];
			
			sz = dh.sz;
			return *this;
		}
	};
	
	Vertices V;
	ColorClass *C, Q, QMAX;
	
	class StepCount {
		
		unsigned i1, i2;
		
		public:
		
		StepCount() : i1(0), i2(0) {}
		
		void set_i1(const unsigned ii)  { 
			
			i1 = ii;
		}
		
		unsigned get_i1() const { 
			
			return i1;
		}
		
		void set_i2(const unsigned ii)  { 
			
			i2 = ii;
		}
		
		unsigned get_i2() const { 
			
			return i2;
		}
		
		void inc_i1()  { 
			
			i1++;
		}
	
	};
	
	StepCount *S;
	
	bool connection(const unsigned i, const unsigned j) const { 
		
		return e[i][j];
	}
	
	bool cut1(const unsigned, const ColorClass&);
	
	void cut2(const Vertices&, Vertices&);
	
	void color_sort(Vertices&);
	
	void expand(Vertices);
	
	void expand_dyn(Vertices);
	
	void _mcq(unsigned*&, unsigned&, bool);
	
	void degree_sort(Vertices &R) { 
		
		R.set_degrees(*this); 
		R.sort();
	}
	
	public:

	Maxclique(const bool* const*, const unsigned, const float=0.025, const unsigned = 76);
	
	unsigned steps() const { 
	
		return pk;
	}
	
	void mcq(unsigned* &maxclique, unsigned &sz) { 
		
		_mcq(maxclique, sz, false);
	}
	
	void mcqdyn(unsigned* &maxclique, unsigned &sz) { 
		
		_mcq(maxclique, sz, true);
	}
	
	~Maxclique() {
		
		if (C) delete [] C;
		if (S) delete [] S;
		V.dispose();
	};
};

Maxclique::Maxclique (const bool* const* conn, const unsigned sz, const float tt, const unsigned cl) : pk(0), level(1), Tlimit(tt), V(sz), Q(sz), QMAX(sz) {
	
	assert(conn!=0 && sz>0);
	
    expectedClique = cl;

	for (unsigned i=0; i < sz; i++) 
		V.push(i);
	
	e = conn;
	
	C = new ColorClass[sz + 1];
	
	for (unsigned i=0; i < sz + 1; i++) 
		C[i].init(sz + 1);
	
	S = new StepCount[sz + 1];
}

void Maxclique::_mcq(unsigned* &maxclique, unsigned &sz, bool dyn) { 
	
	V.set_degrees(*this);
	V.sort();
	V.init_colors();

	if (dyn) {
		for (unsigned i=0; i < V.size() + 1; i++) {
			S[i].set_i1(0);
			S[i].set_i2(0);
		}
		
		expand_dyn(V);
  }else
	  expand(V);
  
  maxclique = new unsigned[QMAX.size()]; 
  
  for (unsigned i=0; i<QMAX.size(); i++) { 
	  maxclique[i] = QMAX.at(i);
  }
  
  sz = QMAX.size();
}

void Maxclique::Vertices::init_colors() { 
	
	const unsigned max_degree = v[0].get_degree();
	
	for (unsigned i = 0; i < max_degree; i++)
		v[i].set_degree(i + 1);
	
	for (unsigned i = max_degree; i < sz; i++)
		v[i].set_degree(max_degree + 1);
}

void Maxclique::Vertices::set_degrees(Maxclique &m) { 
	
	for (unsigned i=0; i < sz; i++) {
		unsigned d = 0;
		for (unsigned j=0; j < sz; j++)
			if (m.connection(v[i].get_i(), v[j].get_i())) d++;
		v[i].set_degree(d);
	}
}

bool Maxclique::cut1(const unsigned pi, const ColorClass &A) {
	
	for (unsigned i = 0; i < A.size(); i++)
		if (connection(pi, A.at(i)))
			return true;
	
	return false;
}

void Maxclique::cut2(const Vertices &A, Vertices &B) {
	
	for (unsigned i = 0; i < A.size() - 1; i++) {
		if (connection(A.end().get_i(), A.at(i).get_i()))
			B.push(A.at(i).get_i());
	}
}

void Maxclique::color_sort(Vertices &R) {
	
	unsigned j = 0;
	unsigned maxno = 1;
	unsigned min_k = QMAX.size() >= Q.size() ? QMAX.size() - Q.size() + 1 : 1;
	C[1].rewind();
	C[2].rewind();
	
	unsigned k = 1;
	for (unsigned i=0; i < R.size(); i++) {
		unsigned pi = R.at(i).get_i();
		k = 1;
		while (cut1(pi, C[k]))
			k++;
		if (k > maxno) {
			maxno = k;
			C[maxno + 1].rewind();
		}
		C[k].push(pi);
		if (k < min_k) {
			R.at(j++).set_i(pi);
		}
	}
	
	if (j > 0) 
		R.at(j-1).set_degree(0);
	
	for (k = min_k; k <= maxno; k++)
		for (unsigned i = 0; i < C[k].size(); i++) {
			R.at(j).set_i(C[k].at(i));
			R.at(j++).set_degree(k);
		}
}

void Maxclique::expand(Vertices R) {
	
	while (R.size()) {
		if (Q.size() + R.end().get_degree() > QMAX.size()) {
			Q.push(R.end().get_i());
			Vertices Rp(R.size());
			cut2(R, Rp);
			
			if (Rp.size()) {
				color_sort(Rp);
				pk++;
				expand(Rp);
			} else if (Q.size() > QMAX.size()) { 
				QMAX = Q;
			}    
			Rp.dispose();
			Q.pop();
		} else {
			return;
		}
		R.pop();
	}
}

void Maxclique::expand_dyn(Vertices R) {
	
	S[level].set_i1(S[level].get_i1() + S[level - 1].get_i1() - S[level].get_i2());
	S[level].set_i2(S[level - 1].get_i1());
	
	while (R.size()) {
        
		if (Q.size() + R.end().get_degree() > QMAX.size() && Q.size() + R.end().get_degree() >= expectedClique) {
            
			Q.push(R.end().get_i());
			Vertices Rp(R.size());
			cut2(R, Rp);
			if (Rp.size()) {
				if ((float)S[level].get_i1()/++pk < Tlimit) {
					degree_sort(Rp);
				}
				color_sort(Rp);
				S[level].inc_i1();
				level++;
				expand_dyn(Rp);
				level--;
			}
			else if (Q.size() > QMAX.size()) { 
				QMAX = Q;
			}
			Rp.dispose();
			Q.pop();
		}
		else {
			return;
		}
		R.pop();
	}
}
#endif
