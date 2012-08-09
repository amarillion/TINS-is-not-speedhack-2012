//Pool Array Structure Implementation.
#include <pool.h>
#include <stdlib.h>
#include <string.h>

pool *create_pool(int itemsize, int maxitems){
	pool *newpool = NULL;
	if(itemsize <= 0)
		return newpool;
	if(maxitems <= 0)
		maxitems = DEFAULT_POOL_SIZE;
	newpool = malloc(sizeof(pool));
	if(newpool) {
		newpool->maxitems = maxitems;
		newpool->itemsize = itemsize;
		newpool->curscan=0;
		newpool->allocdata = NULL;
		newpool->data = NULL;
		newpool->allocdata = malloc(sizeof(char)*maxitems);
		newpool->data = malloc(maxitems*itemsize);
		if(!newpool->data || !newpool->allocdata) {
			destroy_pool(&newpool);
		}
		memset(newpool->allocdata, 0, sizeof(char)*maxitems);
	}
	return newpool;
}

void destroy_pool(pool **gpool){
	if(gpool) {
		pool *givpool = *gpool;
		if(givpool) {
			givpool->maxitems=0;
			givpool->itemsize=0;
			givpool->curscan=0;
			if(givpool->allocdata) {
				free(givpool->allocdata);
				givpool->allocdata = NULL;
			}
			if(givpool->data) {
				free(givpool->data);
				givpool->data = NULL;
			}
			free(givpool);
			givpool = NULL;
		}
		*gpool = NULL;
	}
}

void *pool_alloc(pool *gpool){
	void *allocmem = NULL;
	if(!gpool)
		return allocmem;
	{
		int start=gpool->curscan;
		int reached = start;
		do {
			if(gpool->allocdata[reached] == 0)
			{
				char *callocmem;
				callocmem=gpool->data+(gpool->itemsize*reached);
				//memset(callocmem, 0, gpool->itemsize);
				gpool->allocdata[reached] = 1;
				allocmem = (void *)callocmem;
			}
			++reached;
			if(reached >= gpool->maxitems)
				reached = 0;
			//Next Free Mem to Scan.
			gpool->curscan = reached;
			if(allocmem) return allocmem;
		} while(reached != start);
	}
	return allocmem;
}

void pool_free(pool *gpool, void *data){
	if(gpool) {
		char *datapos = (char *)data;
		int offset = (int)(datapos - gpool->data);
		if(offset >= 0) {
			offset /= gpool->itemsize;
			gpool->allocdata[offset]=0;
			gpool->curscan=offset;
		}
	}
}
