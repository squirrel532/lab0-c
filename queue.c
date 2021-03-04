#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef SORT_TREND_FACTOR
#define SORT_TREND_FACTOR 2
#endif


#define SWAP(type, A, B) \
    do {                 \
        type tmp;        \
        tmp = A;         \
        A = B;           \
        B = tmp;         \
    } while (0)


list_ele_t *_list_ele_alloc(char *s);
void _list_ele_free(list_ele_t *ele);
void q_bubble_sort(list_ele_t **head, list_ele_t **tail);
void q_qsort(list_ele_t **head, list_ele_t **tail);
void _q_swap(list_ele_t **a, list_ele_t **b);
void _q_insert(list_ele_t **target, list_ele_t *e);

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL)
        return;

    while (q->size > 0) {
        q_remove_head(q, NULL, 0);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q == NULL)
        goto exit;

    list_ele_t *newh = _list_ele_alloc(s);
    if (newh == NULL)
        goto exit;

    newh->next = q->head;
    q->head = newh;
    if (q->size == 0) {
        q->tail = newh;
    }

    ++q->size;
    return true;

exit:
    return false;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q == NULL)
        goto exit;

    list_ele_t *newh = _list_ele_alloc(s);
    if (newh == NULL)
        goto exit;

    if (q->size == 0) {
        q->tail = newh;
        q->head = newh;
    } else {
        q->tail->next = newh;
        q->tail = newh;
    }

    ++q->size;
    return true;

exit:
    return false;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q == NULL)
        goto exit;

    if (q->head == NULL)
        goto exit;

    list_ele_t *ele = q->head;
    if (sp != NULL) {
        strncpy(sp, ele->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    q->head = ele->next;
    --q->size;
    if (q->size == 0) {
        q->tail = NULL;
    }

    _list_ele_free(ele);
    return true;

exit:
    return false;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL)
        return;

    if (q->size <= 1)
        return;

    list_ele_t *curr = q->head, *prev = NULL;
    while (curr != NULL) {
        list_ele_t *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    q->tail = q->head;
    q->head = prev;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == NULL)
        return;

    if (q->size < 10) {
        q_bubble_sort(&(q->head), &(q->tail->next));
        goto exit;
    }

    size_t asc_trend = 0, desc_trend = 0;
    list_ele_t *curr = q->head;
    while (curr->next != NULL) {
        int cmp = strncmp(curr->value, curr->next->value, MAX_VALUE_LENGTH);
        if (cmp > 0)
            ++desc_trend;
        else if (cmp < 0)
            ++asc_trend;
    }

    /* reverse qqueue before sorting if descending order is major in queue */
    if (desc_trend * SORT_TREND_FACTOR > q->size && desc_trend > asc_trend) {
        q_reverse(q);
    }

    q_qsort(&(q->head), &(q->tail->next));

exit:
    q->tail = q->head;
    while (q->tail->next != NULL)
        q->tail = q->tail->next;
}

void q_bubble_sort(list_ele_t **begin, list_ele_t **end)
{
    list_ele_t *finished = *end;
    while (finished != *begin) {
        list_ele_t **curr = begin;
        while ((*curr)->next != finished) {
            if (strncmp((*curr)->value, (*curr)->next->value,
                        MAX_VALUE_LENGTH) > 0) {
                _q_swap(curr, &((*curr)->next));
            }
            curr = &((*curr)->next);
        }
        finished = *curr;
    }
}

void q_qsort(list_ele_t **begin, list_ele_t **end)
{
    if (begin == end)  // zero element
        return;
    if ((*begin)->next == *end)  // single element
        return;

    list_ele_t *piviot = *begin;
    list_ele_t *curr = piviot->next, *curr_end = *end;
    piviot->next = *end;  // cut out rest of queue

    list_ele_t **left_begin = begin, **left_end = left_begin,
               **right_begin = &(piviot->next), **right_end = right_begin;
    size_t left = 0, right = 0;
    while (curr != curr_end) {
        list_ele_t *next = curr->next;
        int cmp = strncmp(piviot->value, curr->value, MAX_VALUE_LENGTH);
        if (cmp > 0) {
            _q_insert(left_end, curr);
            left_end = &((*left_end)->next);
            ++left;
        } else if (cmp == 0) {
            _q_insert(right_begin, curr);
            if (right_begin == right_end) {
                right_begin = right_end = &((*right_end)->next);
            } else {
                right_begin = &((*right_begin)->next);
            }
        } else {
            _q_insert(right_end, curr);
            right_end = &((*right_end)->next);
            ++right;
        }
        curr = next;
    }

    if (left < 5) {
        q_bubble_sort(left_begin, left_end);
    } else {
        q_qsort(left_begin, left_end);
    }

    if (right < 5) {
        q_bubble_sort(right_begin, right_end);
    } else {
        q_qsort(right_begin, right_end);
    }
}

void _q_swap(list_ele_t **a, list_ele_t **b)
{
    list_ele_t *a_ptr = *a, *b_ptr = *b;

    if (a_ptr == b_ptr)
        return;

    if (a_ptr->next == b_ptr) {
        a_ptr->next = b_ptr->next;
        b_ptr->next = a_ptr;
        *a = b_ptr;
    } else if (b_ptr->next == a_ptr) {
        b_ptr->next = a_ptr->next;
        a_ptr->next = b_ptr;
        *b = a_ptr;
    } else {
        SWAP(list_ele_t *, *a, *b);
        SWAP(list_ele_t *, (*a)->next, (*b)->next);
    }
}

/*
 * Insert element before *target
 */
void _q_insert(list_ele_t **target, list_ele_t *e)
{
    e->next = *target;
    *target = e;
}

/*
 * Allocate an element, and initialize value from s with max length to
 * MAX_VALUE_LENGTH, excluding terminating NULL byte ('\0');
 */
list_ele_t *_list_ele_alloc(char *s)
{
    list_ele_t *ele = malloc(sizeof(list_ele_t));
    if (ele == NULL)
        goto exit;

    int vlen = strlen(s);
    if (vlen > MAX_VALUE_LENGTH) {
        vlen = MAX_VALUE_LENGTH;
    }
    char *value = malloc(sizeof(char) * vlen + 1);
    if (value == NULL)
        goto free_ele;

    ele->value = value;
    ele->next = NULL;
    strncpy(ele->value, s, vlen);
    ele->value[vlen] = '\0';  // make sure val is always NULL terminated

    return ele;

free_ele:
    free(ele);

exit:
    return NULL;
}

/*
 * Free the ele and all its members
 */
void _list_ele_free(list_ele_t *ele)
{
    if (ele->value != NULL)
        free(ele->value);

    free(ele);
}
