//Linked List Structure Implementation.
#include <list.h>
#include <stdio.h>
#include <stdlib.h>

int default_equality(void * a, void * b){
	if(a == b)
		return EQUALS;
	if(a < b)
		return LESSER;
	//if(a > b)
		return GREATER;
}

list *create_list(void *key, void *data, equality_function equals) {
	list *newlist = NULL;
	newlist = malloc(sizeof(list));
	if(newlist) {
		newlist->key = key;
		newlist->data = data;
		newlist->next = NULL;
		newlist->previous = NULL;
		newlist->equals = equals;
	}
	return newlist;
}

void destroy_list(list **glist){
	if(glist) {
		list *curlist = *glist;
		while(curlist) {
			list *next = curlist->next;
			curlist->next = NULL;
			next->previous = NULL;
			free(curlist);
			curlist = next;
		}
		*glist = curlist;
	}
}

void list_add(void *key, void *data, equality_function equals, list **glist) {
	if(glist) {
		list *curlist = *glist;
		if(!curlist) {
			curlist = create_list(key, data, equals);
			*glist = curlist;
			return;
		}
		while(curlist) {
			//curlist->equals = equals;
			list *next = curlist->next;
			if(equals && equals(curlist->key, key) == EQUALS) {
				curlist->data = data;
				return;
			}
			if(!next) {
				next = curlist->next = create_list(key, data, equals);
				if(next)
					next->previous=curlist;
				return;
			}
			curlist = next;
		}
	}
}

void list_remove(void *key, list **glist) {
	if(glist) {
		list *curlist = *glist;
		if(!curlist) 
			return;
		while(curlist) {
			equality_function equals = curlist->equals;
			list *next = curlist->next;
			if(equals && equals(curlist->key, key) == EQUALS) {
				list *previous = curlist->previous;
				if(previous)
					previous->next = next;
				next->previous = previous;
				curlist->previous = NULL;
				curlist->next = NULL;
				free(curlist);
				if(!previous)
					*glist = next;
				return;
			}
			curlist = next;
		}
	}
}
