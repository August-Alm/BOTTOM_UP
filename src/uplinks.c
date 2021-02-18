/* ***** ***** */

#include <stdlib.h>
#include "uplinks.h"

#include <stdio.h>
/* ***** ***** */

struct uplink_list *get_uplinks(struct term t)
{
    if (!t.ptr) { return NULL; }
    switch (KIND(t)) {
    case VAR_TERM:
        return t.ptr - sizeof(char*) - sizeof(int);
    case LAM_TERM:
        return t.ptr - sizeof(struct uplink);
    default: // APP_TERM
        return t.ptr - 2 * sizeof(struct uplink);
    }
}

void insert_first(struct uplink_list *l, struct uplink *n)
{
    if (!l) {
        fprintf(stderr, "Invalid reference.\n");
        return;
    }
    if (!(l->head)) {
        l->head = n;
        return;
    }
    n->next = l->head;
    l->head->prev = n;
    while (n->prev) { n = n->prev; }
    l->head = n;
}

void add_uplink_to(struct term t, struct uplink *uplk)
{
    struct uplink_list *uplinks = get_uplinks(t);
    insert_first(uplinks, uplk); 
}

void remove_uplink(struct uplink *uplk)
{
    struct uplink *pre = uplk->prev;
    struct uplink *nxt = uplk->next;
    if (pre) {
        if (nxt) {
            pre->next = nxt;
            nxt->prev = pre;       
        }
        else {
            pre->next = NULL;
        }
    }
    else if (nxt) {
        nxt->prev = NULL;
    }
    uplk->prev = NULL;
    uplk->next = NULL;
}

void foreach2_uplink(act2_uplink f, void *param, struct uplink_list uplinks)
{
    struct uplink *tmp = uplinks.head;
    while (tmp) {
        f(param, tmp);
        tmp = tmp->next;
    }
}

void foreach_uplink(act_uplink f, struct uplink_list uplinks)
{
    struct uplink *tmp = uplinks.head;
    while (tmp) {
        f(tmp);
        tmp = tmp->next;
    }
}

int is_length1(struct uplink_list uplinks)
{
    struct uplink *h = uplinks.head;
    if (!h) { return 0; }
    if (!(h->next)) { return 1; }
    return 0;
}
