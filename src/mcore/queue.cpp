

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_util.h"
#include "mtime_ex.h"
#include "queue.h"

struct mqueue {
    int capacity;
    int size;
    int front;
    int rear;
    void **data;
};

struct mqueue*   mqueue_new(int capacity)
{
    struct mqueue *q = (struct mqueue *)calloc(1, sizeof(q[0]));

    if (!q)
        return NULL;

    q->capacity = capacity;
    q->data = (void **)calloc(1, sizeof (q->data[0]) * q->capacity);
    if (!q->data) {
        free(q);
        return NULL;
    }
    
    q->front = 0;
    q->rear = capacity - 1;

    return q;
}

int             mqueue_delete(struct mqueue *q)
{
    if (q) { 
        free(q->data);
        free(q);
    }

    return 0;
}

int             mqueue_is_full(struct mqueue *q)
{
    return q->size == q->capacity;
}

int             mqueue_is_empty(struct mqueue *q)
{
    return q->size == 0;
}

int             mqueue_enmqueue(struct mqueue *q, void *item)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "mqueue_enmqueue(q:%p, item:%p) "
#define func_format()   q, item
    if (mqueue_is_full(q)) {
        print_err("overflow.");
        return -1;
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = item;
    q->size++;

    return 0;
}

void*           mqueue_demqueue(struct mqueue *q)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "mqueue_demqueue(q:%p) "
#define func_format()   q
    void *item;

    if (mqueue_is_empty(q)) {
        print_err("downflow.");
        return NULL;
    }

    item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    return item;
}

int             mqueue_length(struct mqueue *q)
{
    return (q->rear - q->front) + 1;
}
