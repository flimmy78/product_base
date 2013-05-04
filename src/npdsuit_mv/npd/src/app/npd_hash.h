#ifndef __NPD_HASH_H__
#define __NPD_HASH_H__

#ifndef NULL
#define NULL	(0UL)
#endif

#define HASHTABSIZE     2048
#define HASH_KEY_KEY1   0
#define HASH_KEY_KEY2   1

struct hash_bucket
{
  void	*data;					/* pointer to hash data field*/
  struct hash_bucket *next;	/* next hash bucket for key1*/
  struct hash_bucket *next2; /* next hash bucket for key2*/
};

struct Hash
{
	int 		  size; 			/* Total hash bucket number*/
	int 		  count;		  	/* Total node number within all buckets */
  	struct hash_bucket    **index;      	/* Hash bucket pointer  for key1*/
  	struct hash_bucket    **index2;      	/* Hash bucket pointer for key2*/
  	unsigned int  (*hash_key)(void *); /* Key make function for key1*/
  	unsigned int  (*hash_key2)(void *); /* Key2 make function*/
  	int           (*hash_cmp)(void *, void *);/* Data compare function*/
};

struct Hash *hash_new 
(
	int size
);
/* allocate new hash bucket */
struct hash_bucket * hash_backet_new
( 
	void *data
);

struct hash_bucket *hash_head 
(
	struct Hash *hash, 
	int index ,
	int headType
);

void * hash_push
( 
	struct Hash *hash, 
	void *data
);

void *hash_pull 
(
	struct Hash * hash, 
	void *data
);


void *hash_search 
( 
	struct Hash *hash, 
	void *data,
	int (*compFunc)(void*,void*),
	int keyType

);

void hash_clean
(
	struct Hash *hash, 
	void (* func) (void *)
);

void hash_free 
(
	struct Hash *hash, 
	void (* func) (void *)
);

int hash_count
(
	struct Hash *hash

);

void hash_show
(
	struct Hash *hash,
	void (*showFunc)(void *,unsigned char)
);

/*
  * traversal hash table to get hash item by the specified filter,
  * and then process matched item as the processor requires.
  * If no filter specified, all hash items are relevant to processor.
  * If no processor specified, all filtered items are counted.
  *
  * PARAMETERS:
  *		data - data used by filter to pick hash items
  *		filter - filter function to filter out irrelevant items, 
  *			    function has two input arguments:
  *				first argument is the hash item need to filter
  *				second argument is data used to filter to.
  *		processor - trigger function called when filter matched,
  *			    function has one input arguments:
  *				input argument is the hash item
  */
unsigned int hash_traversal
(
	struct Hash *hash,
	unsigned int flag,
	void * data,
	unsigned int (*filter)(void *,void *),
	int (*processor)(void *,unsigned int)
);

/*
  * by zhubo@autelan.com show hash item by port
  * get hash table item by the specified filter,
  * If no filter specified, all hash items return.
  *
  * PARAMETERS:
  *		indata - data used by filter to pick hash items
  *		outdata - data return to user
  *		filter - filter function to filter out irrelevant items, 
  *			    function has two input arguments:
  *				first argument is the hash item need to filter
  *				second argument is data used to filter to.
  */
void hash_return
(
	struct Hash *hash,
	void * indata,
	void * outdata[],
	unsigned int len,
	unsigned int (*filter)(void *,void *)	
);

#endif
