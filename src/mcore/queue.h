
#ifndef __mqueue_h__
#define __mqueue_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

    struct mqueue;

    struct mqueue*   mqueue_new(int size);
    int             mqueue_delete(struct mqueue *q);

    int             mqueue_enmqueue(struct mqueue *q, void *u);
    void*           mqueue_demqueue(struct mqueue *q);

    int             mqueue_is_empty(struct mqueue *q);
    int             mqueue_length(struct mqueue *q);

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif