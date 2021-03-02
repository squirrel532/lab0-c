#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"


#define SWAP(type, A, B) \
    do {                 \
        type tmp;        \
        tmp = A;         \
        A = B;           \
        B = tmp;         \
    } while (0)


list_ele_t *_list_ele_alloc(char *s);
void _list_ele_free(list_ele_t *ele);
void q_bubble_sort(queue_t *q);
void _q_swap(list_ele_t **a, list_ele_t **b);

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* TODO: What if malloc returned NULL? */
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
    /* TODO: You need to write the code for this function */
    /* TODO: Remove the above comment when you are about to implement. */

    q_bubble_sort(q);
}

void q_bubble_sort(queue_t *q)
{
    if (q == NULL)
        return;

    if (q->head == NULL)
        return;

    list_ele_t *finished = NULL;
    while (q->head != finished) {
        list_ele_t **curr = &(q->head);
        while ((*curr)->next != finished) {
            if (strncmp((*curr)->value, (*curr)->next->value,
                        MAX_VALUE_LENGTH) > 0) {
                _q_swap(curr, &((*curr)->next));
            }
            curr = &((*curr)->next);
        }
        finished = *curr;
    }

    q->tail = q->head;
    while (q->tail->next != NULL)
        q->tail = q->tail->next;
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
