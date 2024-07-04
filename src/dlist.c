/* listd.c -- blametroi's doubly linked list functions -- */

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

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/listd.h"

/*
 * initialize or reset the list control block. if the list is not
 * empty, the attempt fails and false is returned.
 */

bool
reset_listd_control(
   ldcb_t *list
) {
   if (list->first != NULL) {
      return false;
   }
   memset(list, 0, sizeof(*list));
   return true;
}

/*
 * remove and free all of the items linked on the list. this is
 * equivalent to repeatedly calling remove_item and free_item on
 * each item in the list.
 */

void
ld_free_all(
   ldcb_t *list
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   lditem_t *curr = list->first;
   lditem_t *next = NULL;

   while (curr) {
      if (list->has_payload && list->dynamic_payload) {
         list->free_payload(curr->payload);
      }
      next = curr->fwd;
      free(curr);
      curr = next;
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

lditem_t *
ld_new(
   ldcb_t *list,
   void *id_or_payload_pointer
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   lditem_t *curr = calloc(1, sizeof(*curr));
   assert(curr);

   curr->list = list;

   if (list->use_id) {
      curr->id = (long)id_or_payload_pointer;
   } else {
      curr->id = list->odometer;
      curr->payload = id_or_payload_pointer;
   }

   return curr;
}

/*
 * free a item's storage. this expects the item to be owned by the
 * controlling list but does not check to see if the item is on the
 * link chain.
 *
 * you should have removed the item from the list before calling free
 * if you intend to remove the item from the list.
 *
 * an item created by make_item can be freed by this function even if
 * it was never added to the list. this is the preferred method for
 * freeing items.
 *
 * pass the address of the item pointer, and not the item pointer
 * itself, as the second argument.
 *
 * if there is a payload and it is dynamically managed in memory, the
 * list's free_payload function is called to release the payload
 * storage.
 *
 * if the item is owned by the controlling list, its storage is freed,
 * the pointer to the item that was passed is set to NULL, and true is
 * returned.
 *
 * otherwise, false is returned and the item pointer is left
 * unchanged.
 */

bool
ld_free(
   ldcb_t *list,
   lditem_t *(*address_of_item_pointer)
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   if (address_of_item_pointer == NULL || *address_of_item_pointer == NULL) {
      return false;
   }
   if ((*address_of_item_pointer)->list != list) {
      return false;
   }

   if (list->has_payload && list->dynamic_payload) {
      list->free_payload((*address_of_item_pointer)->payload);
   }

   free(*address_of_item_pointer);
   *address_of_item_pointer = NULL;

   return true;
}

/*
 * find a item in the list by either id or payload. the second
 * argument is used to identify the item in the list. since the list
 * is ordered, the search will stop if the item's possible location is
 * passed.
 *
 * returns a pointer to the item in the list or NULL.
 */

lditem_t *
ld_find(
   ldcb_t *list,
   void *id_or_payload_pointer
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   lditem_t *curr = list->first;
   if (curr == NULL) {
      return NULL;
   }

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->compare_payload);
   }

   long r;
   while (curr) {
      r = list->use_id
          ? curr->id - (long)id_or_payload_pointer
          : list->compare_payload(curr->payload, id_or_payload_pointer);
      if (r == 0) {
         return curr;
      } else if (r > 0) {
         return NULL;
      }
      curr = curr->fwd;
   }

   return NULL;
}

/*
 * returns the count of the items on the link list by chasing the
 * link chain.
 */

int
ld_count(
   ldcb_t *list
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   long n = 0;
   lditem_t *curr = list->first;
   while (curr) {
      n += 1;
      curr = curr->fwd;
   }
   assert(n == list->count);

   return n;
}

/*
 * add a item to the list. returns true if the item was added, or false if
 * the item is a duplicate another item in the list.
 */

bool
ld_add(
   ldcb_t *list,
   lditem_t *unlinked_item
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   assert(unlinked_item &&
          memcmp(unlinked_item->tag, LDITEM_TAG, sizeof(unlinked_item->tag)) == 0 &&
          "invalid LDITEM");
   list->odometer += 1;

   /* empty list is the easy case */
   if (list->first == NULL) {
      list->first = unlinked_item;
      list->last = unlinked_item;
      unlinked_item->fwd = NULL;
      unlinked_item->bwd = NULL;
      list->count += 1;
      return true;
   }

   /* find where this item id belongs in the list. if the id is already in
      the list, return false. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->compare_payload);
   }

   lditem_t *curr = list->first;
   lditem_t *last = NULL;
   long r = 0;

   while (curr) {
      r = list->use_id
          ? curr->id - unlinked_item->id
          : list->compare_payload(curr->payload, unlinked_item->payload);
      if (r == 0) {
         return false;
      }
      if (r > 0) {
         break;
      }
      last = curr;
      curr = curr->fwd;
   }

   if (curr == NULL) {              /* tail of list */
      last->fwd = unlinked_item;
      list->last = unlinked_item;
      unlinked_item->bwd = last;

   } else if (curr->bwd == NULL) {  /* head of list */
      list->first = unlinked_item;
      unlinked_item->bwd = NULL;
      unlinked_item->fwd = curr;
      curr->bwd = unlinked_item;

   } else {                        /* middle of list */
      curr->bwd->fwd = unlinked_item;
      unlinked_item->bwd = curr->bwd;
      curr->bwd = unlinked_item;
      unlinked_item->fwd = curr;
   }

   list->count += 1;
   return true;
}

/*
 * remove a item from the list by either id or payload. the second
 * argument should be whatever the compare_payload function expects as
 * its second argument. returns a pointer to the unlinked item or NULL
 * if the item was not found in the list.
 */

lditem_t *
ld_remove(
   ldcb_t *list,
   void *id_or_payload_pointer
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

   /* find where this item is in the list. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->compare_payload);
   }

   lditem_t *curr = list->first;
   long r = 0;

   while (curr) {

      r = list->use_id
          ? curr->id - (long)id_or_payload_pointer
          : list->compare_payload(curr->payload, id_or_payload_pointer);

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

lditem_t *
ld_next(
   ldcb_t *list,
   lditem_t *(*next_item)
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

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
 * iterate over items moving backward. the second argument is a
 * pointer to the address of item to iterate from. each call updates
 * this address as context for repeated calls. setting this to NULL
 * means iterate from the tail of the list. returns the next item in
 * sequence or NULL if no more items are available.
 */

lditem_t *
ld_prev(
   ldcb_t *list,
   lditem_t *(*prior_item)
) {
   assert(list &&
          memcmp(list->tag, LDCB_TAG, sizeof(list->tag)) == 0 &&
          "invalid LDCB");
   list->odometer += 1;

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
