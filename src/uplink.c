/* ***** ***** */

#include <stddef.h>
#include <stdbool.h>
#include "types.h"
#include "uplink.h"

/* ***** ***** */

extern
enum uplink_rel get_rel(uplink_t lk);

extern
uplink_t get_next(uplink_t lk);

extern
void set_next(uplink_t lk, uplink_t nxt);

extern
uplink_t get_prev(uplink_t lk);

extern
void set_prev(uplink_t lk, uplink_t prev);

extern
void reinit_uplink(uplink_t lk);

extern
void link(uplink_t lk1, uplink_t lk2);

void unlink(uplink_t lk)
{
   uplink_t prv = get_prev(lk);
   uplink_t nxt = get_next(lk);
   if (prv == -1) {
      if (nxt == -1) return;
      set_prev(nxt, -1);
      set_next(lk, -1);
      return;
   }
   if (nxt == -1) {
      set_next(prv, -1);
      set_prev(lk, -1);
      return;
   }
   set_prev(nxt, prv);
   set_next(prv, nxt);
   reinit_uplink(lk);
}

extern
node_t get_node(uplink_t lk);

/* ***** ***** */

extern
uplink_t get_head(uplink_dll_t lks);

extern
void set_head(uplink_dll_t lks, uplink_t h);

extern
void init_dll(uplink_dll_t lks);

extern
struct uplink_dll empty_dll();

extern
bool is_empty(uplink_dll_t lks);

extern
bool is_length_one(uplink_dll_t lks);

extern
void append(uplink_dll_t lks, uplink_t lk);

extern
void iter_dll(uplink_action_t act, uplink_dll_t lks);

/* ***** ***** */

// end of uplink.c