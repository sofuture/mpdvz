#ifndef LL_HEADER
#define LL_HEADER

typedef struct node {
    int v;
    struct node *next;
} node;

typedef struct list {
    node *head, *tail;
    int ct;
} list;

list *ll_new();
void ll_add(int v, list *list);
void ll_decapitate(list *list);
void ll_free(list *list);

#endif
