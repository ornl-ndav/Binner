#ifndef __HASH_TABLE_HH_
#define __HASH_TABLE_HH_
typedef unsigned char ub1;      /* unsigned 1-byte quantities */
typedef unsigned long int ub4;

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

/*
  --------------------------------------------------------------------
  mix -- mix 3 32-bit values reversibly.
  For every delta with one or two bits set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
   * If mix() is run forward or backward, at least 32 bits in a,b,c
     have at least 1/4 probability of changing.
   * If mix() is run forward, every bit of c will change between 1/3 and
     2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
     mix() was built out of 36 single-cycle latency instructions in a 
     structure that could supported 2x parallelism, like so:
     a -= b; 
     a -= c; x = (c>>13);
     b -= c; a ^= x;
     b -= a; x = (a<<8);
     c -= a; b ^= x;
     c -= b; x = (b>>13);
     ...
     Unfortunately, superscalar Pentiums and Sparcs can't take advantage of
     that parallelism.  They've also turned some of those single-cycle latency 
     instructions into multi-cycle latency instructions. Still, this is the 
     fastest good hash I could find. There were about 2^^68 to choose from.  
     I only looked at a billion or so.
 --------------------------------------------------------------------
 */
#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

#include <pthread.h>

typedef struct hashentry{

    //	pthread_mutex_t pDataLock;
    	void *pData;
    	void *pKey;
    	ub4 ulKeyLength;
    	ub4 ulHashVal;
    	struct hashentry *pNext;
	int iDataSize;
}HashEntry;

class MemRoot;
			   
class HashTable
{
  public:
    HashTable();
    HashTable(int);
    ~HashTable();

    int insert(void *, ub4, void *);
    int insert(void *, ub4, void *, unsigned int);
    int find(void *, ub4);
    int find(HashEntry **, void *, ub4);
    void statistics();
    HashEntry *getCurrent()
    {
        return pCurrentEntry;
    }

    void grow();
    bool deleteItem(void *, ub4 );

  protected:
      ub4 hash(ub1 * k, ub4 length, ub4 initval);
    void Init();

  protected:
    ub4 lastHash;
    HashEntry *pCurrentEntry;
    HashEntry **pTable;
    unsigned long int ulTableSize;
    size_t iMask;
    int iLogSize;
    unsigned long int ulCount;
    MemRoot * pMemRoot;

    pthread_mutex_t tableLock;
    
};
#endif
