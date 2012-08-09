//Linked List Structure.
#ifndef LIST_H
#define LIST_H

#define EQUALS  0
#define LESSER  -1
#define GREATER 1

typedef int (*equality_function)(void *, void*);
int default_equality(void *, void *);

typedef struct _list{
	void *data;
	void *key;
	struct _list *next;
	struct _list *previous;
	equality_function equals;
	int dummy[3];
} list;

list *create_list(void *key, void *data, equality_function equals);
void destroy_list(list **glist);
void list_add(void *key, void *data, equality_function equals, list **glist);
void list_remove(void *key, list **glist);

#endif //LIST_H
