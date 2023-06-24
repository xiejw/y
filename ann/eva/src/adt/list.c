// copyright: see license_antirez-bsd file.

#include "adt/list.h"

#include <stdlib.h>

/* Create a new list. The created list can be freed with
 * listFree(), but private value of every node need to be freed
 * by the user before to call listFree(), or by setting a free method using
 * listSetFreeFn.
 *
 * On error, NULL is returned. Otherwise the pointer to the new list.
 */
struct list *
listNew(void)
{
        struct list *list;

        if ((list = malloc(sizeof(*list))) == NULL) return NULL;
        list->head = list->tail = NULL;
        list->len               = 0;
        list->dup               = NULL;
        list->free              = NULL;
        list->match             = NULL;
        return list;
}

/* Remove all the elements from the list without destroying the list itself. */
void
listClear(struct list *list)
{
        unsigned long len;
        struct list_node *current, *next;

        current = list->head;
        len     = list->len;
        while (len--) {
                next = current->next;
                if (list->free) list->free(current->v.data);
                free(current);
                current = next;
        }
        list->head = list->tail = NULL;
        list->len               = 0;
}

void
listFree(struct list *list)
{
        listClear(list);
        free(list);
}

/* Add a new node to the list, to head.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the new 'node' pointer is returned.
 */
struct list_node *
listAddNodeHead(struct list *list)
{
        struct list_node *node;

        if ((node = malloc(sizeof(*node))) == NULL) return NULL;
        if (list->len == 0) {
                list->head = list->tail = node;
                node->prev = node->next = NULL;
        } else {
                node->prev       = NULL;
                node->next       = list->head;
                list->head->prev = node;
                list->head       = node;
        }
        list->len++;
        return node;
}

/* Add a new node to the list, to tail.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the new 'node' pointer is returned.
 */
struct list_node *
listAddNodeTail(struct list *list)
{
        struct list_node *node;

        if ((node = malloc(sizeof(*node))) == NULL) return NULL;
        if (list->len == 0) {
                list->head = list->tail = node;
                node->prev = node->next = NULL;
        } else {
                node->prev       = list->tail;
                node->next       = NULL;
                list->tail->next = node;
                list->tail       = node;
        }
        list->len++;
        return node;
}

struct list_node *
listInsertNode(struct list *list, struct list_node *old_node, int after)
{
        struct list_node *node;

        if ((node = malloc(sizeof(*node))) == NULL) return NULL;
        if (after) {
                node->prev = old_node;
                node->next = old_node->next;
                if (list->tail == old_node) {
                        list->tail = node;
                }
        } else {
                node->next = old_node;
                node->prev = old_node->prev;
                if (list->head == old_node) {
                        list->head = node;
                }
        }
        if (node->prev != NULL) {
                node->prev->next = node;
        }
        if (node->next != NULL) {
                node->next->prev = node;
        }
        list->len++;
        return node;
}

// Remove the specified node from the specified list and free it.
void
listDelNode(struct list *list, struct list_node *node)
{
        listUnlinkNode(list, node);
        if (list->free) list->free(node->v.data);
        free(node);
}

// Remove the specified node from the specified list.
void
listUnlinkNode(struct list *list, struct list_node *node)
{
        if (node->prev)
                node->prev->next = node->next;
        else
                list->head = node->next;
        if (node->next)
                node->next->prev = node->prev;
        else
                list->tail = node->prev;
        list->len--;
}

// returns a list iterator 'iter'. After the initialization every
// call to listNext() will return the next element of the list.
struct list_iter *
listNewIter(struct list *list, int direction)
{
        struct list_iter *iter;

        if ((iter = malloc(sizeof(*iter))) == NULL) return NULL;
        if (direction == 1)
                iter->next = list->head;
        else
                iter->next = list->tail;
        iter->direction = direction;
        return iter;
}

void
listFreeIter(struct list_iter *iter)
{
        free(iter);
}

/* Create an iterator in the list private iterator structure */
void
listRewind(struct list *list, struct list_iter *li)
{
        li->next      = list->head;
        li->direction = 1;
}

void
listRewindTail(struct list *list, struct list_iter *li)
{
        li->next      = list->tail;
        li->direction = 0;
}

/* Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage
 * pattern is:
 *
 * iter = listNewIter(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 * */
struct list_node *
listNext(struct list_iter *iter)
{
        struct list_node *current = iter->next;

        if (current != NULL) {
                if (iter->direction == 1)
                        iter->next = current->next;
                else
                        iter->next = current->prev;
        }
        return current;
}

/* Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupFn() function is used
 * to copy the node value. Otherwise the same value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
struct list *
listDup(struct list *orig)
{
        struct list *copy;
        struct list_iter iter;
        struct list_node *node;
        struct list_node *new_node;

        if ((copy = listNew()) == NULL) return NULL;
        copy->dup   = orig->dup;
        copy->free  = orig->free;
        copy->match = orig->match;
        listRewind(orig, &iter);
        while ((node = listNext(&iter)) != NULL) {
                void *data;

                new_node = listAddNodeTail(copy);
                if (new_node == NULL) {
                        listFree(copy);
                        return NULL;
                }

                if (copy->dup) {
                        // preset to avoid listFree to free the wrong address.
                        new_node->v.data = NULL;
                        data             = copy->dup(node->v.data);
                        if (data == NULL) {
                                listFree(copy);
                                return NULL;
                        }
                        new_node->v.data = data;
                } else {
                        // struct copy.
                        new_node->v = node->v;
                }
        }
        return copy;
}

/* Search the list for a node matching a given key.
 *
 * The match is performed using the 'match' method
 * set with listSetMatchFn(). If no 'match' method
 * is set, the 'v.data' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned. */
struct list_node *
listSearchKey(struct list *list, void *key)
{
        struct list_iter iter;
        struct list_node *node;

        listRewind(list, &iter);
        while ((node = listNext(&iter)) != NULL) {
                if (list->match) {
                        if (list->match(node->v.data, key)) {
                                return node;
                        }
                } else {
                        if (key == node->v.data) {
                                return node;
                        }
                }
        }
        return NULL;
}

/* Return the element at the specified zero-based index
 * where 0 is the head, 1 is the element next to head
 * and so on. Negative integers are used in order to count
 * from the tail, -1 is the last element, -2 the penultimate
 * and so on. If the index is out of range NULL is returned. */
struct list_node *
listIndex(struct list *list, long index)
{
        struct list_node *n;

        if (index < 0) {
                index = (-index) - 1;
                n     = list->tail;
                while (index-- && n) n = n->prev;
        } else {
                n = list->head;
                while (index-- && n) n = n->next;
        }
        return n;
}

/* Rotate the list removing the tail node and inserting it to the head. */
void
listRotateTailToHead(struct list *list)
{
        if (listLen(list) <= 1) return;

        /* Detach current tail */
        struct list_node *tail = list->tail;
        list->tail             = tail->prev;
        list->tail->next       = NULL;
        /* Move it as head */
        list->head->prev = tail;
        tail->prev       = NULL;
        tail->next       = list->head;
        list->head       = tail;
}

/* Rotate the list removing the head node and inserting it to the tail. */
void
listRotateHeadToTail(struct list *list)
{
        if (listLen(list) <= 1) return;

        struct list_node *head = list->head;
        /* Detach current head */
        list->head       = head->next;
        list->head->prev = NULL;
        /* Move it as tail */
        list->tail->next = head;
        head->next       = NULL;
        head->prev       = list->tail;
        list->tail       = head;
}

/* Add all the elements of the list 'o' at the end of the
 * list 'l'. The list 'o' remains empty but otherwise valid. */
void
listJoin(struct list *l, struct list *o)
{
        if (o->len == 0) return;

        o->head->prev = l->tail;

        if (l->tail)
                l->tail->next = o->head;
        else
                l->head = o->head;

        l->tail = o->tail;
        l->len += o->len;

        /* Setup other as an empty list. */
        o->head = o->tail = NULL;
        o->len            = 0;
}
