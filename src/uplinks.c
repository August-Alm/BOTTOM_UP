/* ***** ***** */

#include <stdlib.h>
#include "uplinks.h"

#include <stdio.h>
/* ***** ***** */

struct uplink_dll *get_uplinks(struct node nd)
{
    switch (KIND(nd)) {
    case VAR_NODE:
        return VAR(nd)->uplinks;
    case LAM_NODE:
        return LAM(nd)->uplinks;
    case APP_NODE:
        return APP(nd)->uplinks;
    }
    return NULL; // Just to please gcc.
}

void add_uplink_to(struct node nd, struct uplink_dll *lk)
{
    struct uplink_dll *uplinks;

    if (!nd.term) {
         printf("can't add link to nullnode\n");
         return;
    }
    
    switch (KIND(nd)) {
    case VAR_NODE:
        uplinks = VAR(nd)->uplinks;
        if (uplinks) {
            lk->next = uplinks;
            uplinks->prev = lk;
        }
        VAR(nd)->uplinks = lk;
        break;
    case LAM_NODE:
        uplinks = LAM(nd)->uplinks;
        if (uplinks) {
            lk->next = uplinks;
            uplinks->prev = lk;
        }
        LAM(nd)->uplinks = lk;
        break;
    case APP_NODE:
        uplinks = APP(nd)->uplinks;
        if (uplinks) {
            lk->next = uplinks;
            uplinks->prev = lk;
        }
        APP(nd)->uplinks = lk;
        break;
    }
}

void remove_uplink(struct uplink_dll lk)
{
    struct uplink_dll *pre = lk.prev;
    struct uplink_dll *nxt = lk.next;
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
    lk.prev = NULL;
    lk.next = NULL;
}

void foreach2_uplink(act2_uplink f, void *param, struct uplink_dll *uplinks)
{
    struct uplink_dll *tmp = uplinks;
    while (tmp) {
        f(param, tmp);
        tmp = tmp->next;
    }
}

void foreach_uplink(act_uplink f, struct uplink_dll *uplinks)
{
    struct uplink_dll *tmp = uplinks;
    while (tmp) {
        f(tmp);
        tmp = tmp->next;
    }
}

int is_length1(struct uplink_dll *uplinks)
{
    if (!uplinks) {
        return 0;
    }
    if (!(uplinks->next)) {
        return 1;
    }
    return 0;
}
