#ifndef OLIST_H
#define OLIST_H

#define _PASTER_DECL(name) struct _ ## name ## _list; typedef struct _ ## name ## _list name ## _list; \
        static void name ## _list_create ( name ## _list ** slist, int num_shapes); \
        static void name ## _list_cleanup ( name ## _list ** slistptr); \
        static void name ## _list_set ( name ## _list * slist, int which, name * s);
#define _PASTER_DEFN(name) struct _ ## name ## _list { int size; name **list; }
#define _PASTER_CREATE(name) static void \
        name ## _list_create ( name ## _list ** slist, int num) \
        { \
          int i; \
          if (NULL == slist) \
            return; \
          (*slist) = ( name ## _list *) malloc (sizeof ( name ## _list)); \
          (*slist)->size = num; \
          (*slist)->list = (name **) malloc (num * sizeof (name *)); \
          for (i = 0; i < num; ++i) \
          { \
            (*slist)->list[i] = NULL; \
          } \
        }
#define _PASTER_CLEANUP(name) static void \
        name ## _list_cleanup ( name ## _list ** slistptr) \
        { \
          int i; \
          name ## _list *slist; \
          if (NULL == slistptr || NULL == *slistptr || NULL == (*slistptr)->list) \
            return; \
          slist = *slistptr; \
          for (i = 0; i < slist->size; ++i) \
          { \
            if (slist->list[i]) \
              free (slist->list[i]); \
          } \
          slist->size = 0; \
          free (slist->list); \
          free (slist); \
          *slistptr = NULL; \
        }
#define _PASTER_SET(name) static void \
        name ## _list_set ( name ## _list * slist, int which, name * s) \
        { \
          if (NULL == slist || NULL == slist->list || (which < 0) || (which > slist->size)) \
            return; \
          slist->list[which] = s; \
        }

//Struct Types and API
#define DECLARE_LIST(name) _PASTER_DECL(name); _PASTER_DEFN(name);
#define DEFINE_LIST(name) _PASTER_CREATE(name); _PASTER_CLEANUP(name); _PASTER_SET(name) 

#endif //OLIST_H
