
#ifndef _FIB_H_
#define _FIB_H_


#include <vector>
using std::vector;

typedef float key_type;
struct pheap_el {
	struct	pheap_el *lchild;
	struct	pheap_el *left;
	struct	pheap_el *right;
	void	*data;
};


class dque {
public:
  	struct pheap_el *front ;
  	struct pheap_el *back ;
  	int size;

	void push_front(pheap_el *);
	void push_back(pheap_el *);
	pheap_el *pop();
	void remove(pheap_el *);
	void clear();

  	dque() {
	  front = back = NULL ;
 	  size = 0 ;
  	}

	private:
} ;



class pheap {

	public:

	pheap_el *root;
	int size;
	void initial(int n);
	void clear();
	void insert(pheap_el *);
	pheap_el *extract();
	void decreaseKey(pheap_el *, key_type);

	private:

	vector<pheap_el *>	tmp;
	//vector<pheap_el *>	store;
	//int numStore;
	inline pheap_el *cmpAndlink(pheap_el *, pheap_el *);
	inline pheap_el *combineSiblings(pheap_el *);
	inline void multiPassPairing(vector<pheap_el *>  &forest, int num);	
	inline void twoPassPairing(vector<pheap_el *>  &forest, int num);	
	
};



/*
//dnol defie  fake heap
void fh_push_front(fibque*, void*);
void fh_push_back(fibque*, void*);
void* fh_pop(fibque*);
bool fh_replace_que(fibque*, fibheap_el*, int);
void fh_que_reset(fibque*);
*/
#endif /* _FIB_H_ */
