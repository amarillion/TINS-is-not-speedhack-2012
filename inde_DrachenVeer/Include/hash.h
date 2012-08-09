#ifndef HASH_H
#define HASH_H
#include <list.h>

#define DEFAULT_BUCKETS 31 //Decent Prime Number with Spreadouts.

typedef int (*hash_function)(void *, int nbuckets);
int default_hash(void *, int nbuckets);

typedef struct _hash{
	list **buckets;
	int nbuckets;
	hash_function hasher;
	equality_function equals;
} hash;

hash *create_hash(hash_function hasher, equality_function equals, int nbuckets);
void hash_add(void *key, void *data, hash *ghash);
void hash_put(void *key, void *data, hash *ghash);
void *hash_get(void *key, hash *ghash);
void hash_del(void *key, hash *ghash);
void destroy_hash(hash **ghash);

#endif //HASH_H
