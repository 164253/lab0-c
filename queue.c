#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *it, *safe;
    list_for_each_entry_safe (it, safe, head, list) {
        free(it->value);
        free(it);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *node = malloc(sizeof(element_t));
    char *val = strdup(s);
    if (!node || !val || !head) {
        free(node);
        free(val);
        return false;
    }
    node->value = val;
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *elem = list_first_entry(head, element_t, list);
    if (sp)
        strncpy(sp, elem->value, bufsize - 1);
    list_del(head->next);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_head(head->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *pos;
    list_for_each (pos, head)
        ++len;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    for (struct list_head *tail = head->prev;
         head != tail && head->next != tail; tail = tail->prev)
        head = head->next;
    list_del(head);
    q_release_element(list_entry(head, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    struct list_head *pos;
    list_for_each (pos, head->next) {
        if (!strcmp(list_entry(pos->prev, element_t, list)->value,
                    list_entry(pos, element_t, list)->value)) {
            pos->prev = pos->prev->prev;
            q_release_element(list_entry(pos->prev->next, element_t, list));
            pos->prev->next = pos;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || !head->next)
        return;
    struct list_head *now = head, *tail;
    head->prev->next = NULL;
    head = head->next;
    for (; now && now->next; now = now->next) {
        now->next->prev = now->prev;
        now->prev = now->next;
        now->next = now->next->next;
        now->prev->next = now;
        tail = now;
    }
    if (now)
        tail = now;
    tail->next = head;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *now = head, *tmp;
    do {
        tmp = now->next;
        now->next = now->prev;
        now->prev = tmp;
        now = tmp;
    } while (now != head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
