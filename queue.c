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
    if (sp) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    list_del(head->next);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return list_empty(head) ? NULL
                            : q_remove_head(head->prev->prev, sp, bufsize);
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

#define q_find_mid(head, mid, midnext)                                      \
    mid = (head)->next;                                                     \
    midnext = (head)->prev;                                                 \
    for (; mid != midnext && mid->next != midnext; midnext = midnext->prev) \
        mid = mid->next;

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *tail;
    q_find_mid(head, head, tail);
    list_del(head);
    q_release_element(list_entry(head, element_t, list));
    return true;
}

bool q_delete_somthing(struct list_head *head, int condition)
{
    if (!head)
        return false;
    bool reverse = condition & 0x80000000;
    for (struct list_head *left = reverse ? head->prev : head->next,
                          *right = reverse ? left->prev : left->next;
         right != head; right = reverse ? right->prev : right->next) {
        while (right != head) {
            int k = strcmp(
                list_entry(reverse ? right->next : right->prev, element_t, list)
                    ->value,
                list_entry(right, element_t, list)->value);
            if ((k >> 7) - (-k >> 7) != !!condition)
                break;
            right = reverse ? right->prev : right->next;
        }
        if (!condition &&
            !strcmp(list_entry(left, element_t, list)->value,
                    list_entry(left->next, element_t, list)->value))
            left = left->prev;
        for (struct list_head *tmp = reverse ? left->prev : left->next;
             tmp != right; tmp = reverse ? left->prev : left->next) {
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        }
        if (right == head)
            break;
        left = right;
    }
    if (condition && head->next->next != head) {
        if (reverse &&
            strcmp(list_entry(head->next, element_t, list)->value,
                   list_entry(head->next->next, element_t, list)->value) < 0) {
            struct list_head *tmp = head->next;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        } else if (strcmp(
                       list_entry(head->prev, element_t, list)->value,
                       list_entry(head->prev->prev, element_t, list)->value) >
                   0) {
            struct list_head *tmp = head->next;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        }
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    return q_delete_somthing(head, 0);
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    q_reverseK(head, 2);
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
    if (!head)
        return;
    struct list_head *prev, *left = head->next, *right, *next;
    do {
        right = left;
        for (int i = k; --i && right->next != head;)
            right = right->next;
        prev = left->prev;
        next = right->next;
        struct list_head tmp_head;
        left->prev = &tmp_head;
        right->next = &tmp_head;
        tmp_head.next = left;
        tmp_head.prev = right;
        q_reverse(&tmp_head);
        prev->next = tmp_head.next;
        tmp_head.next->prev = prev;
        next->prev = tmp_head.prev;
        tmp_head.prev->next = next;
        left = next;
    } while (left != head);
}

void q_merge_two(struct list_head *head,
                 struct list_head *another,
                 bool descend)
{
    if (!head && !another)
        return;
    struct list_head **indirect = &head, *head1 = head->next,
                     *head2 = another->next, **node;
    for (; head1 != head && head2 != another; *node = (*node)->next) {
        node = ((strcmp(list_entry(head1, element_t, list)->value,
                        list_entry(head2, element_t, list)->value) < 0) ^
                descend)
                   ? &head1
                   : &head2;
        (*node)->prev = *indirect;
        (*indirect)->next = *node;
        indirect = &(*indirect)->next;
    }
    node = head1 == head ? &another : &head;
    if (head1 == head)
        head1 = head2;
    head1->prev = *indirect;
    (*indirect)->next = head1;
    for (; head1->next != *node;)
        head1 = head1->next;
    head1->next = head;
    head->prev = head1;
    INIT_LIST_HEAD(another);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next->next == head)
        return;
    struct list_head *mid, *midnext, head2;
    q_find_mid(head, mid, midnext);
    if (mid == midnext)
        midnext = midnext->next;
    head->prev->next = &head2;
    head2.prev = head->prev;
    midnext->prev = &head2;
    head2.next = midnext;
    mid->next = head;
    head->prev = mid;
    q_sort(head, descend);
    q_sort(&head2, descend);
    q_merge_two(head, &head2, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    q_delete_somthing(head, 1);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    q_delete_somthing(head, -1);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    for (;;) {
        struct list_head *cur = head->next, *next;
        for (next = cur->next;
             next != head &&
             q_size(list_entry(next, queue_contex_t, chain)->q) == 0;)
            next = next->next;
        if (next == head)
            break;
    LOOP:
        q_merge_two(list_entry(cur, queue_contex_t, chain)->q,
                    list_entry(next, queue_contex_t, chain)->q, descend);
        list_entry(next, queue_contex_t, chain)->size = 0;
        for (cur = next->next;
             cur != head &&
             q_size(list_entry(cur, queue_contex_t, chain)->q) == 0;)
            cur = cur->next;
        if (cur == head)
            continue;
        for (next = cur->next;
             next != head &&
             q_size(list_entry(next, queue_contex_t, chain)->q) == 0;)
            next = next->next;
        if (next != head)
            goto LOOP;
    }
    return list_entry(head->next, queue_contex_t, chain)->size =
               q_size(list_entry(head->next, queue_contex_t, chain)->q);
}