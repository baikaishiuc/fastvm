
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_util.h"
#include "dynarray.h"
#include "macro_list.h"
#include "bitset.h"
#include "mheap.h"
#include "graph.h"


graph_t*                graph_new(graph_vertex_cmp cmp)
{
    struct graph    *g = (struct graph *)calloc(1, sizeof(g[0]));

    if (!g)
        return NULL;

    g->cmp = cmp;

    return g;
}

void                    graph_delete(graph_t *g)
{
    struct graph_edge *edge;
    struct graph_vertex *node;

    if (!g) return;

    while ((edge = g->edges.list)) {
        mlist_del(g->edges, edge, in_graph);
        free(edge);
    }

    while ((node = g->nodes.list)) {
        mlist_del(g->nodes, node, in_graph);
        free(node);
    }

    free(g);
}

struct graph_vertex*      graph_vertex_find(graph_t *g, void *user_data)
{
    struct graph_vertex *v;
    int i;

    mlist_for_each(g->nodes, v, in_graph, i) {
        if (0 == g->cmp(v->user_data, user_data)) {
            return v;
        }
    }
    return NULL;
}

struct graph_vertex*      graph_vertex_add(graph_t *g, void *user_data)
{
    struct graph_vertex *node;

    if ((node = graph_vertex_find(g, user_data)))
        return node;

    node = (struct graph_vertex *)calloc(1, sizeof (node[0]));
    if (!node)
        return NULL;

    node->id = g->nodes.counts + 1;
    node->user_data = user_data;
    mlist_add(g->nodes, node, in_graph);

    return node;
}

int                     graph_vertex_del(graph_t *g, struct graph_vertex *node)
{
    struct graph_edge *edge;

    /* 删除单个点时，需要把自己发出的所有出边，从自己的出边列表和别人的入边列表摘除 */
    while ((edge = node->out_edges.list))
    {
        mlist_del(node->out_edges, edge, out_edge);
        mlist_del(edge->to->in_edges, edge, in_edge);
        mlist_del(g->edges, edge, in_graph);
        free(edge);
    }

    while ((edge = node->in_edges.list))
    {
        mlist_del(node->in_edges, edge, in_edge);
        mlist_del(edge->from->out_edges, edge, out_edge);
        mlist_del(g->edges, edge, in_graph);
        free(edge);
    }

    /* 把自己从总的节点列表中摘除 */
    mlist_del(g->nodes, node, in_graph);
    free(node);

    return 0;
}

struct graph_edge*      graph_edge_find(graph_t *g, graph_vertex_t *from, graph_vertex_t *to)
{
    struct graph_edge *edge;
    int i;

    mlist_for_each(from->out_edges, edge, out_edge, i) {
        if (edge->to == to)
            return edge;
    }
    return NULL;
}

int                     graph_edge_add(graph_t *g, graph_vertex_t *from, graph_vertex_t *to, int bi, int weight, void *user_data)
{
    struct graph_edge *edge = NULL;

    if (!(graph_vertex_find(g, from->user_data)) || 
        !(graph_vertex_find(g, to->user_data)) ||
        (edge = graph_edge_find(g, from ,to))) {
        return - 1;
    }

    edge = (struct graph_edge *) calloc(1, sizeof (edge[0]));
    if (!edge) {
        return -2;
    }
    edge->weight = weight;
    edge->from = from;
    edge->to = to;
    edge->user_data = user_data;

	mlist_add(from->out_edges, edge, out_edge);
	mlist_add(to->in_edges, edge, in_edge);

    mlist_add(g->edges, edge, in_graph);

	// printf("from_node = %d, to_node = %d\n", from->id, to->id);

    return 0;
}

int                     graph_edge_del(graph_t *g, graph_vertex_t *from, graph_vertex_t *to, int bidir)
{
    if (!graph_vertex_find(g, from->user_data) || !graph_vertex_find(g, to->user_data)) {
        return -1;
    }

    return 0;
}

int                     graph_vertex_count(graph_t *g)
{
    return g->nodes.counts;
}

int                     graph_edge_count(graph_t *g)
{
    return g->edges.counts;
}

void*                   graph_vertex_user_data(struct graph_vertex *node)
{
    return node->user_data;
}

void*                   graph_edge_user_data(struct graph_edge *node)
{
    return node->user_data;
}

static int inline comp_weight(void *ref, const void *lhs, const void *rhs)
{
	int *weight = (int *)ref;

	return weight[((struct graph_vertex *)lhs)->id] - weight[((struct graph_vertex *)rhs)->id];
}

int                     graph_sssp(graph_t *g, struct graph_vertex *u, struct graph_vertex *v, struct graph_sssp_result *r, int flag)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "graph_sssp(g:%p, u:%d, v:%d, r:%p) "
#define func_format()   g, u->id, v->id, r
    struct bitset *bs;
    int i, count;
	struct graph_vertex *u1;

    unsigned int *weight;
    struct graph_edge **trans;

    if (!g || !u || !v || !r) {
        print_err("failed with invalid param.");
        return -1;
    }

	count = g->nodes.counts + 1;
    weight = (unsigned int *)calloc(1, sizeof (weight[0]) * count);
    trans = (struct graph_edge **)calloc(1, sizeof (trans[0]) * count);
    if (!weight || !trans) {
        if (weight)     free(weight);
        print_err("failed with calloc().");
        return -1;
    }
    memset(weight, 0xff, count * sizeof (weight[0]));

	struct bitset *heap_bs = bitset_new(count);
    bs = bitset_new(count);
	struct mheap *h = mheap_new(count, weight, comp_weight);

	mheap_insert(h, u);
    while ((u1 = (struct graph_vertex *)mheap_delmin(h))) {
        struct graph_edge *edge;
        bitset_set(bs, u1->id, 1);

        mlist_for_each(u1->out_edges, edge, out_edge, i) {
            if (bitset_get(bs, edge->to->id))
                continue;

            unsigned int w = weight[u1->id] + edge->weight;
            if (weight[edge->to->id] > w) {
                weight[edge->to->id] = w;
                trans[edge->to->id] = edge;

				if (!bitset_get(heap_bs, edge->to->id)) {
					mheap_insert(h, edge->to);
					bitset_set(heap_bs, edge->to->id, 1);
				}
				else {
					mheap_deckey(h, edge->to);
				}
            }
        }

        if (flag && trans[v->id])
            break;
    }

    dynarray_add(&r->d, graph_vertex_user_data(v));
    for (i = v->id; trans[i]; i = trans[i]->from->id) {
        dynarray_add(&r->d, graph_vertex_user_data(trans[i]->from));
        dynarray_add(&r->e, trans[i]->user_data);
        dynarray_add(&r->w, (void *)(long)trans[i]->weight);
    }
    dynarray_reverse(&r->d);
    dynarray_reverse(&r->e);
    dynarray_reverse(&r->w);

	if (heap_bs)	bitset_delete(heap_bs);
    if (bs)         bitset_delete(bs);
    if (weight)     free(weight);
    if (trans)      free(trans);
	if (h)			mheap_delete(h);

    return 0;
}

int                     graph_sssp_clear(struct graph_sssp_result *r)
{
    if (!r)
        return 0;

    dynarray_reset(&r->d);
    dynarray_reset(&r->w);
    dynarray_reset(&r->e);

    return 0;
}
