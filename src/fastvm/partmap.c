
#include "vm.h"
#include "partmap.h"

typedef struct partnode 
{
    struct rb_node node;
    linetype key;
    valuetype value;
} partnode;

#define _container_of(_node)    (partnode *)((char *)(_node) - offsetof(partnode, node))

/* upperbound 是模拟 std::map 里的upper_bound

它返回大于key的最接近的一个值，因为我们用c实现，所以用红黑树进行模拟

当能够找到唯一的那个key时，返回它顺序排列的下一个值
当没有找到，且自己比上一次查找的小时，直接返回上一次查找过的节点(父节点)
当没有找到，且自己比上一次查找的大时，返回父节点的顺序排列的下一个值

*/
static partnode*   partmap_uppperbound(partmap *p, linetype key)
{
    struct rb_node *node = p->tree.rb_node;
    partnode *data, *parent = NULL;
    int ret;

    while (node) {
        data = _container_of(node);

        ret = p->cmp(key, data->key);
        if (ret < 0)
            node = node->rb_left;
        else if (ret > 0)
            node = node->rb_right;
        else {
            node = rb_next(node);
            return _container_of(node);
        }

        parent = data;
    }

    if (!parent) return NULL;
    if (ret < 0) return parent;

    node = rb_next(&parent->node);
    return _container_of(node);
}

static partnode*   partmap_find(partmap *p, linetype key)
{
    struct rb_node *node = p->tree.rb_node;
    partnode *data;
    int ret;

    while (node) {
        data = _container_of(node);

        ret = p->cmp(key, data->key);
        if (ret < 0)
            node = node->rb_left;
        else if (ret > 0)
            node = node->rb_right;
        else
            return data;
    }

    return NULL;
}

partmap*    partmap_new(partmap_cmp cmp, valuetype defaultvalue)
{
    partmap *p;
    p = vm_mallocz(sizeof(p[0]));

    p->cmp = cmp;

    return p;
}

void        partmap_delete(partmap *p)
{
    free(p);
}

valuetype   partmap_getValue(partmap *p, linetype key)
{
    partnode *node = partmap_uppperbound(p, key);
    struct rb_node *rb_node;

    if (&node->node == rb_first(&p->tree))
        return p->defaultvalue;

    rb_node = rb_prev(&node->node);
    return _container_of(rb_node);
}

int         partmap_insert(partmap *p, linetype key, valuetype v)
{
    partnode *data = NULL;
    struct rb_node **n = &(p->tree.rb_node), *parent = NULL;
    int ret;

    while (*n) {
        partnode *t = _container_of(*n);
        ret = p->cmp(key, t->key);
        parent = *n;
        if (ret < 0)
            n = &((*n)->rb_left);
        else if (ret > 0)
            n = &((*n)->rb_right);
        else
            return FALSE;
    }
    data = vm_mallocz(sizeof(data[0]));

    rb_link_node(&data->node, parent, n);
    rb_insert_color(&data->node, &p->tree);

    return 0;
}

valuetype   partmap_split(partmap *p, linetype pnt)
{
    partnode *node = partmap_uppperbound(p, pnt), *pnode;
    struct rb_node *rb_node;
    valuetype v = p->defaultvalue;

    if (&node->node == rb_first(&p->tree)) {
        rb_node = rb_prev(&node->node);
        pnode = _container_of(rb_node);
        if (pnode->key == pnt)
            return pnode->value;

        v = pnode->value;
    }

    partmap_insert(p, pnt, v);
    p->count++;
    return v;
}

void partmap_erase(partmap *p, struct rb_node *beg, struct rb_node *end)
{
    struct rb_node *next;
    partnode *data;

    while (1) {
        next = rb_next(beg);
        data = _container_of(beg);

        rb_erase(beg, &p->tree);
        p->count--;
        vm_free(data);

        if (beg == end)
            break;

        beg = next;
    }
}

valuetype   partmap_clearRange(partmap *p, linetype pnt1, linetype pnt2)
{
    struct rb_node *beg, *end;
    partnode *data;
    partmap_split(p, pnt1);
    partmap_split(p, pnt2);

    beg = &(partmap_find(p, pnt1)->node);
    end = &(partmap_find(p, pnt2)->node);

    data = _container_of(beg);

    partmap_erase(p, rb_next(beg), end);

    return data->value;
}

valuetype   partmap_bounds(partmap *p, linetype pnt, linetype *before, linetype *after, int *valid)
{
    partnode *pnode;
    struct rb_node *iter, *enditer;

    if (!p->count) {
        *valid = 3;
        return p->defaultvalue;
    }

    pnode = partmap_uppperbound(p, pnt);
    enditer = pnode ? &pnode->node:NULL;
    if (enditer != rb_first(&p->tree)) {
        iter = enditer;
        iter = rb_prev(iter);
        *before = (_container_of(iter))->key;
        if (enditer == rb_last(&p->tree))
            *valid = 2;
        else {
            *after = (_container_of(enditer))->key;
            *valid = 0;
        }

        return (_container_of(iter))->value;
    }

    *valid = 1;
    *after = (_container_of(enditer))->key;
    return p->defaultvalue;
}
