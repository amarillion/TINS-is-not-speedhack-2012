//Pool Array Structure.
#ifndef POOL_H
#define POOL_H

#define DEFAULT_POOL_SIZE 32

typedef struct _pool{
	int maxitems;
	int itemsize;
	int curscan;
	int pad;
	char *allocdata;
	char *data;
	void *pad2[2];
} pool;

pool *create_pool(int itemsize, int maxitems);
void destroy_pool(pool **gpool);
void *pool_alloc(pool *gpool);
void pool_free(pool *gpool, void *data);

#endif //POOL_H
