#include <stdio.h>
#include <stdlib.h>

#include "ll.h"

node *createnode(int v){
    node *newNode = malloc(sizeof(node));
    newNode->v = v;
    newNode->next = NULL;
    return newNode;
}

list *ll_new(){
    list *list = malloc(sizeof(list));
    list->head = list->tail = NULL;
    list->ct = 0;
    return list;
}

void ll_decapitate(list *list){
    if(list->head == NULL)
        return;
    list->head = list->head->next;
}

void ll_add(int v, list *list){
    if(list->head == NULL){
        list->head = list->tail = createnode(v);
    } else {
        list->tail = list->tail->next = createnode(v);
    }

    if(list->ct > 1024){
        ll_decapitate(list);
    } else {
        list->ct++;
    }
}

void ll_free(list * list){
    node *current = list->head;
    node *next = current;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
