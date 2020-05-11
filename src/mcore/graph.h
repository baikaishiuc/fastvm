

#ifndef __graph_h__
#define __graph_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

    typedef struct graph_edge {
        struct {
            struct graph_edge   *next;
            struct graph_edge   *prev;
        } in_graph;

        struct {
            struct graph_edge   *next;
            struct graph_edge   *prev;
        } in_edge;

        struct {
            struct graph_edge   *next;
            struct graph_edge   *prev;
        } out_edge;

        struct graph_vertex     *from;
        struct graph_vertex     *to;
        int weight;
        void*                   user_data;
    } graph_edge_t;

    typedef struct graph_vertex {
        struct {
            struct graph_vertex *next;
            struct graph_vertex *prev;
        } in_graph;

        struct {
            int counts;
            struct graph_edge *list;
        } in_edges;

        struct {
            int counts;
            struct graph_edge *list;
        } out_edges;

        int id;
        void *user_data;
    } graph_vertex_t;


    /*
    0:  equal
    1:  大于
    -1: 小于
    */
    typedef int (*graph_vertex_cmp)(void *lhs, void *rhs);

    typedef struct graph {
        struct {
            int id;
            int counts;
            struct graph_vertex *list;
        } nodes;

        struct {
            int id;
            int counts;
            struct graph_edge *list;
        } edges;

        graph_vertex_cmp      cmp;
    } graph_t;

    graph_t*                graph_new(graph_vertex_cmp cmp);
    void                    graph_delete(graph_t *g);

    struct graph_vertex*    graph_vertex_find(graph_t *g, void *user_data);
    struct graph_vertex*    graph_vertex_add(graph_t *g, void *user_data);
    int                     graph_vertex_del(graph_t *g, struct graph_vertex *node);
    int                     graph_edge_add(graph_t *g, graph_vertex_t *from, graph_vertex_t *to, int bi, int weight, void *user_data);
    int                     graph_edge_del(graph_t *g, graph_vertex_t *from, graph_vertex_t *to, int bidir);
    struct graph_edge*      graph_edge_find(graph_t *g, graph_vertex_t *from, graph_vertex_t *to);

    void*                   graph_vertex_user_data(struct graph_vertex *node);

    int                     graph_vertex_count(graph_t *g);
    int                     graph_edge_count(graph_t *g);

    struct graph_sssp_result
    {
        struct dynarray d;
        struct dynarray w;
        struct dynarray e;
    };
    /* 最短单源路径 
    @flag                   搜索策略，
                            0:  路径最短，找到u, v之间的最短路径
                            1:  可达，找到u, v之间的任意一条路径
    @return not NULL        路径节点
            NULL            没有找到路径
    */
    int                     graph_sssp(graph_t *g, struct graph_vertex *u, struct graph_vertex *v, struct graph_sssp_result *r, int flag);
    int                     graph_sssp_clear(struct graph_sssp_result *r);

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif
