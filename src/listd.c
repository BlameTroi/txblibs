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
   listd_control_t *list
) {
   assert(list->initialized);
   if (list->head != NULL) {
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
free_all_items(
   listd_control_t *list
) {
   assert(list->initialized);
   list->odometer += 1;
   list->frees += list->count;

   listd_item_t *curr = list->head;
   listd_item_t *next = NULL;

   while (curr) {
      if (list->has_payload && list->dynamic_payload) {
         list->free_payload(curr->payload);
      }
      next = curr->next;
      free(curr);
      curr = next;
   }

   list->head = NULL;
   list->tail = NULL;
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
make_item(
   listd_control_t *list,
   void *id_or_payload_pointer
) {
   assert(list->initialized);
   list->odometer += 1;
   list->makes += 1;

   listd_item_t *curr = calloc(1, sizeof(*curr));
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
free_item(
   listd_control_t *list,
   listd_item_t *(*address_of_item_pointer)
) {
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

listd_item_t *
find_item(
   listd_control_t *list,
   void *id_or_payload_pointer
) {
   assert(list->initialized);
   list->odometer += 1;
   list->finds += 1;

   listd_item_t *curr = list->head;
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
      curr = curr->next;
   }

   return NULL;
}

/*
 * returns the count of the items on the link list by chasing the
 * link chain.
 */

int
count_items(
   listd_control_t *list
) {
   assert(list->initialized);
   list->odometer += 1;
   list->counts += 1;

   long n = 0;
   listd_item_t *curr = list->head;
   while (curr) {
      n += 1;
      curr = curr->next;
   }
   assert(n == list->count);

   return n;
}

/*
 * add a item to the list. returns true if the item was added, or false if
 * the item is a duplicate another item in the list.
 */

bool
add_item(
   listd_control_t *list,
   listd_item_t *unlinked_item
) {
   assert(list->initialized && unlinked_item);
   list->odometer += 1;
   list->adds += 1;

   /* empty list is the easy case */
   if (list->head == NULL) {
      list->head = unlinked_item;
      list->tail = unlinked_item;
      unlinked_item->next = NULL;
      unlinked_item->prev = NULL;
      list->count += 1;
      return true;
   }

   /* find where this item id belongs in the list. if the id is already in
      the list, return false. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->compare_payload);
   }

   listd_item_t *curr = list->head;
   listd_item_t *last = NULL;
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
      curr = curr->next;
   }

   if (curr == NULL) {              /* tail of list */
      last->next = unlinked_item;
      list->tail = unlinked_item;
      unlinked_item->prev = last;

   } else if (curr->prev == NULL) {  /* head of list */
      list->head = unlinked_item;
      unlinked_item->prev = NULL;
      unlinked_item->next = curr;
      curr->prev = unlinked_item;

   } else {                        /* middle of list */
      curr->prev->next = unlinked_item;
      unlinked_item->prev = curr->prev;
      curr->prev = unlinked_item;
      unlinked_item->next = curr;
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

listd_item_t *
remove_item(
   listd_control_t *list,
   void *id_or_payload_pointer
) {
   assert(list->initialized);
   list->odometer += 1;
   list->removes += 1;

   /* find where this item is in the list. */

   if (!list->use_id) {
      assert(list->has_payload);
      assert(list->compare_payload);
   }

   listd_item_t *curr = list->head;
   long r = 0;

   while (curr) {

      r = list->use_id
          ? curr->id - (long)id_or_payload_pointer
          : list->compare_payload(curr->payload, id_or_payload_pointer);

      if (r < 0) {
         curr = curr->next;
         continue;
      } else if (r > 0) {
         return NULL;
      }

      if (curr->next == NULL && curr->prev == NULL) {

         /* this is the only item */
         list->head = NULL;
         list->tail = NULL;

      } else if (curr->prev == NULL) {

         /* this is the head */
         list->head = curr->next;
         curr->next->prev = NULL;

      } else if (curr->next == NULL) {

         /* is this the tail? */
         list->tail = curr->prev;
         curr->prev->next = NULL;

      } else {

         /* somewhere in the middle */
         curr->prev->next = curr->next;
         curr->next->prev = curr->prev;
      }

      /* removing dangling pointers and return */
      curr->next = NULL;
      curr->prev = NULL;
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
next_item(
   listd_control_t *list,
   listd_item_t *(*next_item)
) {
   assert(list->initialized);
   list->odometer += 1;
   list->nexts += 1;

   if (*next_item == NULL) {
      if (list->head == NULL) {
         return NULL;
      }
      *next_item = list->head;
      return *next_item;
   }

   *next_item = (*next_item)->next;
   return *next_item;
}

/*
 * iterate over items moving backward. the second argument is a
 * pointer to the address of item to iterate from. each call updates
 * this address as context for repeated calls. setting this to NULL
 * means iterate from the tail of the list. returns the next item in
 * sequence or NULL if no more items are available.
 */

listd_item_t *
prev_item(
   listd_control_t *list,
   listd_item_t *(*prior_item)
) {
   assert(list->initialized);
   list->odometer += 1;
   list->prevs += 1;

   if (*prior_item == NULL) {
      if (list->head == NULL) {
         return NULL;
      }
      *prior_item = list->tail;
      return *prior_item;
   }

   *prior_item = (*prior_item)->prev;
   return *prior_item;
}
