/* txblistd.h -- blametroi's doubly linked list functions -- */
#ifndef TXBLISTD_H
#define TXBLISTD_H

/*
 * a header only implementation of a doubly linked list.
 *
 * the list is kept in order by a key, which can be either an
 * identifying long integer, or by some unique value in the payload
 * that each list node carries.
 *
 * each list will have a control block containing the approriate
 * counters, links, configuration information, and when function
 * pointers for routines to compare payload key values and to
 * dynamically free payload storage when a node is freed.
 *
 * the api is reasonably complete, but i'm still leaving the setup for
 * the list control blocks in open user code.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */


#ifdef TXBLISTD_H_IMPLEMENTATION

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#endif /* TXBLISTD_H_IMPLEMENTATION */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * a node of the doubly linked list. keying for ordering can use
 * either the id or results from the fncompare function. node
 * keys must be unique within a list.
 */

typedef struct listd_item_t {
   long id;                        /* either a user supplied id or an odometer value */
   void *payload;                  /* if provided, usually a pointer to the payload */
   struct listd_control_t *list;   /* owning list */
   struct listd_item_t *fwd;       /* and chain pointers */
   struct listd_item_t *bwd;
} listd_item_t;


/*
 * double list control block holding chain pointers and other
 * controling information. if a payload is carried, function pointers
 * to free the payload and also perform comparisons for ordering are
 * put here. an id field is provided for ordering and is set from an
 * odometer value.
 *
 * all functions take as their first argument a pointer to an instance
 * of this control block.
 */

typedef struct listd_control_t {
   listd_item_t *first;                /* head and tail item pointers */
   listd_item_t *last;
   void (*fnfree)(void *);             /* if a payload is carried and if it needs to be freed, place address here */
   long (*fncompare)(void *, void *);  /* if a payload is carried and used for keying, a compare function here */
   long odometer;                      /* just a counter of calls to the api */
   long makes;                         /* and the individual functions */
   long adds;
   long removes;
   long nexts;
   long prevs;
   long finds;
   long frees;
   long counts;
   long count;                         /* how many items are on the list? */
   bool initialized;                   /* ready to roll? */
   bool use_id;                        /* use the id field for ordering and finding */
   bool has_payload;                   /* this list's items carry a payload */
   bool dynamic_payload;               /* and the payload should be freed when the item is freed */
} listd_control_t;


/*
 * forward declarations for all functions.
 *
 * i hope that argument names are sufficiently wordy to aid in
 * understanding. more complete comment blocks are included with
 * the implementations.
 *
 * error checking and handling is limited. if things look bad,
 * execution is halted via an assert(). this is only done in cases
 * where continued execution is unwise, as in the list control block
 * is not initialized, or it appears that the link chain is broken.
 */


bool
reset_listd_control(
   listd_control_t *list
);


void
free_all_items(
   listd_control_t *list
);


listd_item_t *
make_item(
   listd_control_t *list,
   void *id_or_payload_pointer
);


bool
free_item(
   listd_control_t *list,
   listd_item_t **address_of_item_pointer
);


listd_item_t *
find_item(
   listd_control_t *list,
   void *id_or_payload_pointer
);


int
count_items(
   listd_control_t *list
);


bool
add_item(
   listd_control_t *list,
   listd_item_t *unlinked_item
);


listd_item_t *
remove_item(
   listd_control_t *list,
   void *id_or_payload_pointer
);


listd_item_t *
next_item(
   listd_control_t *list,
   listd_item_t *(*next_item)
);


listd_item_t *
prev_item(
   listd_control_t *list,
   listd_item_t *(*prior_item)
);

#ifdef TXBLISTD_H_IMPLEMENTATION


/*
 * initialize or reset the list control block. if the list is not
 * empty, the attempt fails and false is returned.
 */

bool
reset_listd_control(listd_control_t *list) {
   assert(list->initialized);
   if (list->first != NULL) {
      return false;
   }
   memset(list, 0, sizeof(*list));
   list->initialized = false;
   return true;
}


/*
 * remove and free all of the items linked on the list. this is
 * equivalent to repeatedly calling remove_item and free_item on
 * each item in the list.
 */

void
free_all_items(listd_control_t *list) {
   assert(list->initialized);
   list->odometer += 1;
   list->frees += list->count;

   listd_item_t *p = list->first;
   listd_item_t *n = NULL;

   while (p) {
      if (list->has_payload && list->dynamic_payload) {
         list->fnfree(p->payload);
      }
      n = p->fwd;
      free(p);
      p = n;
   }

   list->first = NULL;
   list->last = NULL;
   list->count = 0;
}


/*
 * allocate storage and do basic initialization of a list item. this
 * storage should be freed when no longer needed. while it can be
 * freed via the standard free call, using free_item will handle
 * releasing a payload if one was provided.
 *
 * the second argument is for the payload and can be either a pointer
 * or an atomic value that can be cast to a void pointer and stored in
 * the list item. if user assigned ids are used, cast that to a long
 * and pass it here.
 */

listd_item_t *
make_item(listd_control_t *list, void *id_or_payload_pointer) {
   assert(list->initialized);
   list->odometer += 1;
   list->makes += 1;

   listd_item_t *n = calloc(1, sizeof(*n));
   assert(n);

   n->list = list;

   if (list->use_id) {
      n->id = (long)id_or_payload_pointer;
   } else {
      n->id = list->odometer;
      n->payload = id_or_payload_pointer;
   }

   return n;
}


/*
 * free a item's storage. this expects the item to be owned by the
 * controlling list but does not check to see if the item is on the
 * link chain. you should have removed the item from the list before
 * calling free if you intend to remove the item from the list.
 *
 * a item created by make_item can be freed by this function even if
 * it was never added to the list. this is the preferred method for
 * freeing items.
 *
 * pass the address of the item pointer, and not the item pointer
 * itself, as the second argument.
 *
 * if there is a payload and it is dynamically managed in memory, the
 * list's fnfree function is called to release the payload storage.
 *
 * if the item is owned by the controlling list, its storage is freed,
 * the pointer to the item that was passed is set to NULL, and true is
 * returned.
 *
 * otherwise, false is returned and the item pointer is left
 * unchanged.
 */

bool
free_item(listd_control_t *list, listd_item_t *(*address_of_item_pointer)) {
   assert(list->initialized);
   list->odometer += 1;
   list->frees += 1;

   if (address_of_item_pointer == NULL || *address_of_item_pointer == NULL) {
      return false;
   }
   if ((*address_of_item_pointer)->list != list) {
      return false;
   }

   if (list->has_payload && list->dynamic_payload) {
      list->fnfree((*address_of_item_pointer)->payload);
   }

   free(*address_of_item_pointer);
   *address_of_item_pointer = NULL;

   return true;
}


/*
 * find a item in the list by either id or payload. the second
 * argument is used to identify the item in the list. since
 * the list is ordered, the search will stop if the item's
 * possible location is passed.
 *
 * returns a pointer to the item in the list or NULL.
 */

listd_item_t *
find_item(listd_control_t *list, void *id_or_payload_pointer) {
   assert(list->initialized);
   list->odometer += 1;
   list->finds += 1;

   listd_item_t *p = list->first;
   if (p == NULL) {
      return NULL;
   }

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->fncompare);
   }

   long r;
   while (p) {
      r = list->use_id
          ? p->id - (long)id_or_payload_pointer
          : list->fncompare(p->payload, id_or_payload_pointer);
      if (r == 0) {
         return p;
      } else if (r > 0) {
         return NULL;
      }
      p = p->fwd;
   }

   return NULL;
}


/*
 * returns the count of the items on the link list by chasing the
 * link chain.
 */

int
count_items(listd_control_t *list) {
   assert(list->initialized);
   list->odometer += 1;
   list->counts += 1;

   long n = 0;
   listd_item_t *p = list->first;
   while (p) {
      n += 1;
      p = p->fwd;
   }
   assert(n == list->count);

   return n;
}


/*
 * add a item to the list. returns true if the item was added, or false if
 * the item is a duplicate another item in the list.
 */

bool
add_item(listd_control_t *list, listd_item_t *unlinked_item) {
   assert(list->initialized && unlinked_item);
   list->odometer += 1;
   list->adds += 1;

   /* empty list is the easy case */
   if (list->first == NULL) {
      list->first =unlinked_item;
      unlinked_item->fwd = NULL;
      unlinked_item->bwd = NULL;
      list->count += 1;
      return true;
   }

   /* find where this item id belongs in the list. if the id is already in
      the list, return false. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->fncompare);
   }

   listd_item_t *curr = list->first;
   listd_item_t *last = NULL;
   long r = 0;

   while (curr) {
      r = list->use_id
          ? curr->id - unlinked_item->id
          : list->fncompare(curr->payload, unlinked_item->payload);
      if (r == 0) {
         return false;
      }
      if (r > 0) {
         break;
      }
      last = curr;
      curr = curr->fwd;
   }

   /* tail of list */

   if (curr == NULL) {
      last->fwd = unlinked_item;
      unlinked_item->bwd = last;
      list->last = unlinked_item;
      list->count += 1;
      return true;
   }

   /* head of list */

   if (curr->bwd == NULL) {
      list->first = unlinked_item;
      unlinked_item->bwd = NULL;
      unlinked_item->fwd = curr;
      curr->bwd = unlinked_item;
      list->count += 1;
      return true;
   }

   /* middle of list */

   curr->bwd->fwd = unlinked_item;
   unlinked_item->bwd = curr->bwd;
   curr->bwd = unlinked_item;
   unlinked_item->fwd = curr;
   list->count += 1;
   return true;
}


/*
 * remove a item from the list by either id or payload. the second
 * argument should be whatever the fncompare function expects as its
 * second argument. returns a pointer to the unlinked item or NULL if
 * the item was not found in the list.
 */

listd_item_t *
remove_item(listd_control_t *list, void *id_or_payload_pointer) {
   assert(list->initialized);
   list->odometer += 1;
   list->removes += 1;

   /* find where this item is in the list. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->fncompare);
   }

   listd_item_t *curr = list->first;
   long r = 0;

   while (curr) {

      r = list->use_id
          ? curr->id - (long)id_or_payload_pointer
          : list->fncompare(curr->payload, id_or_payload_pointer);

      if (r < 0) {
         curr = curr->fwd;
         continue;
      } else if (r > 0) {
         return NULL;
      }

      if (curr->fwd == NULL && curr->bwd == NULL) {

         /* this is the only item */
         list->first = NULL;
         list->last = NULL;

      } else if (curr->bwd == NULL) {

         /* this is the head */
         list->first = curr->fwd;
         curr->fwd->bwd = NULL;

      } else if (curr->fwd == NULL) {

         /* is this the tail? */
         list->last = curr->bwd;
         curr->bwd->fwd = NULL;

      } else {

         /* somewhere in the middle */
         curr->bwd->fwd = curr->fwd;
         curr->fwd->bwd = curr->bwd;
      }

      /* removing dangling pointers and return */
      curr->fwd = NULL;
      curr->bwd = NULL;
      list->count -= 1;
      return curr;
   }

   /* it's not there */
   return NULL;
}


/*
 * iterate over items moving forward. the second argument is a pointer
 * to the address of item to iterate from. each call updates this
 * address as context for repeated calls. setting this to NULL means
 * iterate from the head of the list. returns the next item in
 * sequence or NULL if no more items are available.
 */

listd_item_t *
next_item(listd_control_t *list, listd_item_t *(*next_item)) {
   assert(list->initialized);
   list->odometer += 1;
   list->nexts += 1;

   if (*next_item == NULL) {
      if (list->first == NULL) {
         return NULL;
      }
      *next_item = list->first;
      return *next_item;
   }

   *next_item = (*next_item)->fwd;
   return *next_item;
}


/*
 * iterate over items moving backward. the second argument is a pointer
 * to the address of item to iterate from. each call updates this
 * address as context for repeated calls. setting this to NULL means
 * iterate from the tail of the list. returns the next item in
 * sequence or NULL if no more items are available.
 */

listd_item_t *
prev_item(listd_control_t *list, listd_item_t *(*prior_item)) {
   assert(list->initialized);
   list->odometer += 1;
   list->prevs += 1;

   if (*prior_item == NULL) {
      if (list->first == NULL) {
         return NULL;
      }
      *prior_item = list->last;
      return *prior_item;
   }

   *prior_item = (*prior_item)->bwd;
   return *prior_item;
}


#endif /* TXBLISTD_H_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TXBLISTD_H */
