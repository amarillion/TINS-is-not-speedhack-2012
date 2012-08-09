#include <hash.h>
#include <stdlib.h>

int default_hash(void *key, int nbuckets){
	long long a = (long long)key;
	if(nbuckets > 1)
		a %= nbuckets;
	else 
		a = 0;
	return (int)a;
}

void destroy_hash(hash **ghash){
     hash *curhash;
	if(!ghash)
		return;
	curhash = *ghash;
	if(curhash) {
		int i;
		for(i = 0; i < curhash->nbuckets; ++i) {
			destroy_list(&curhash->buckets[i]);
		}
		free(curhash);
		curhash = NULL;
	}
	*ghash = curhash;
}

hash *create_hash(hash_function hasher, equality_function equals, int nbuckets){
	hash *newhash = NULL;
	if(nbuckets <= 0)
		nbuckets = DEFAULT_BUCKETS;
	newhash = malloc(sizeof(hash));
	if(newhash) {
		newhash->buckets = NULL;
		newhash->nbuckets = nbuckets;
		newhash->hasher = hasher;
		newhash->equals = equals;
		newhash->buckets = malloc(sizeof(list *)*nbuckets);
		if(!newhash->buckets) {
			destroy_hash(&newhash);
		} else {
			int i;
			for(i = 0; i < nbuckets; ++i) {
				newhash->buckets[i] = NULL;
			}
		}
	}
	return newhash;
}

void *hash_get(void *key, hash *hash){
	return NULL;
}

void hash_del(void *key, hash *hash){
}

void hash_put(void *key, void *data, hash *hash){
}

void hash_add(void *key, void *data, hash *hash){
	if(hash_get(key, hash) != NULL)
		hash_put(key, data, hash);
}

