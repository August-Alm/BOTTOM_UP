/* ***** ***** */

#ifndef UPLINKS_H
#define UPLINKS_H

/* ***** ***** */

#include "types.h"

/* ***** ***** */

#define LAM_OF_BOD(lk) ((struct lam*)&lk)

#define APP_OF_FUN(lk) ((struct app*)&lk)

#define APP_OF_ARG(lk) ((struct app*)(&lk - sizeof(struct uplink_dll)))

/* ***** ***** */

typedef void (*act_uplink) (struct uplink_dll *uplinks);

typedef void (*act2_uplink) (void* param, struct uplink_dll *uplinks);

/* ***** ***** */

struct uplink_dll *get_uplinks(struct node nd);

void add_uplink_to(struct node nd, struct uplink_dll *lk);

void remove_uplink(struct uplink_dll lk);

int is_length1(struct uplink_dll *uplinks);

void foreach_uplink(act_uplink f, struct uplink_dll *uplinks);

void foreach2_uplink(act2_uplink f, void* param, struct uplink_dll *uplinks);

/* ***** ***** */

#endif // UPLINKS_H

