#ifndef __MEMROOT_H_
#define __MEMROOT_H_
#define MEMSTART 0
#define MEMMAX 32000
# define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))

typedef struct mement{
	struct mement * next;
}MemEnt;


class MemRoot{

	public:
		MemRoot(size_t);
		~MemRoot();

		void * NewMem();
		void DelMem(MemEnt *);
	protected:

		void * NewX();

		void * remalloc(int);
	protected:
	MemEnt * pList;
	MemEnt *pTrash;
	size_t iSize;
	size_t iLogSize;
	int iNumLeft;


};

#endif
