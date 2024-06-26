/*
 * single file header generated via:
 * buildhdr --macro TXBLISTD --intro LICENSE --pub inc/listd.h --priv src/listd.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBLISTD_SINGLE_HEADER
#define TXBLISTD_SINGLE_HEADER
/* *** begin pub *** */
/* txblistd.h -- blametroi's doubly linked list functions -- */

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

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * a node of the doubly linked list. keying for ordering can use
 * either the id or results from the compare_payload function. node
 * keys must be unique within a list.
 */

typedef struct listd_item_t {
   long id;                        /* either a user supplied id or an odometer value */
   void *payload;                  /* if provided, usually a pointer to the payload */
   struct listd_control_t *list;   /* owning list */
   struct listd_item_t *next;      /* and chain pointers */
   struct listd_item_t *prev;
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
   listd_item_t *head;                 /* head and tail item pointers */
   listd_item_t *tail;
   void (*free_payload)(void *);       /* if a payload is carried and if it needs to be freed, place address here */
   long (*compare_payload)(void *, void *);  /* if a payload is carried and used for keying, a compare function here */
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBLISTD_SINGLE_HEADER */

#ifdef TXBLISTD_IMPLEMENTATION
#undef TXBLISTD_IMPLEMENTATION
/* *** begin priv *** */
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


/*
 * initialize or reset the list control block. if the list is not
 * empty, the attempt fails and false is returned.
 */

bool
reset_listd_control(listd_control_t *list) {
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
free_all_items(listd_control_t *list) {
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
make_item(listd_control_t *list, void *id_or_payload_pointer) {
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
find_item(listd_control_t *list, void *id_or_payload_pointer) {
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
count_items(listd_control_t *list) {
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
add_item(listd_control_t *list, listd_item_t *unlinked_item) {
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
remove_item(listd_control_t *list, void *id_or_payload_pointer) {
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
next_item(listd_control_t *list, listd_item_t *(*next_item)) {
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
prev_item(listd_control_t *list, listd_item_t *(*prior_item)) {
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
/* *** end priv *** */

#endif /* TXBLISTD_IMPLEMENTATION */
