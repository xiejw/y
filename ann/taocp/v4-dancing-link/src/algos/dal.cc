#include <algos/dal.h>

#include <cassert>

// #include <stdio.h>
//
//// eva
// #include <adt/vec.h>
//
// typedef struct dal_node dal_node_t;
//
////
///-----------------------------------------------------------------------------
//// Helper methods prototypes.
////
///-----------------------------------------------------------------------------
//
// static void newNode(vec_t(dal_node_t) h, size_t id);
// static inline void linkLR(vec_t(dal_node_t) h, size_t end, size_t id);
// static inline void linkUD(vec_t(dal_node_t) h, size_t id_c, size_t id);
// static inline void cover_col(vec_t(dal_node_t) h, size_t c);
// static inline void uncover_col(vec_t(dal_node_t) h, size_t c);
// static int search(vec_t(dal_node_t) h, size_t k, vec_t(size_t) sols);
//

namespace algos::dal {

namespace {

void
FillNode( Node &node, std::size_t id )
{
    node.id = id;
    node.L  = id;
    node.R  = id;
    node.U  = id;
    node.D  = id;
    node.C  = 0;
}

void
LinkLR( Node *h, size_t end, size_t id )
{
    auto p    = &h[id];
    p->L      = end;
    p->R      = h[end].R;
    h[end].R  = id;
    h[p->R].L = id;
}

}  // namespace

Table::Table( std::size_t reserve_n, std::size_t n_col_heads )
    : m_nodes{ nullptr, std::free }
{
    assert( reserve_n >= n_col_heads + 1 );

    //
    this->m_num_colheads = n_col_heads;
    this->m_num_nodes    = 1;
    this->m_nodes.reset( (Node *)malloc( sizeof( Node ) * reserve_n ) );

    auto nodes = this->m_nodes.get( );
    FillNode( nodes[0], /*id=*/0 );

    for ( std::size_t i = 1; i <= n_col_heads; i++ ) {
        FillNode( nodes[i], i );
        LinkLR( nodes, i - 1, i );
    }
}

}  // namespace algos::dal

//
// void
// dalAppendOption(struct dal_table *ph, size_t num_ids, size_t *col_ids,
//                void *data)
//{
//        vec_t(dal_node_t) v = ph->nodes;
//        size_t n_id         = vecSize(v);
//
//        for (size_t i = 0; i < num_ids; i++) {
//                size_t id = n_id + i;
//                newNode(v, id);
//                linkUD(v, col_ids[i], id);
//                v[id].data = data;
//                if (i != 0) {
//                        linkLR(v, id - 1, id);
//                }
//        }
//
//        size_t size = num_ids + n_id;
//        vecSetSize(v, size);
//        ph->num_nodes = size;
//}
//
// void
// dalCoverCol(struct dal_table *t, size_t c)
//{
//        cover_col(t->nodes, c);
//}
//
//// sols must have enough capacity to hold result.
// int
// dalSearchSolution(struct dal_table *t, vec_t(size_t) sols)
//{
//         vecSetSize(sols, 0);
//         return search(t->nodes, 0, sols);
// }
//
////
///-----------------------------------------------------------------------------
//// Helper Methods Implementation.
////
///-----------------------------------------------------------------------------
//
//// link the `id` into table after node `end` (horizantal double link)
// void
//
//// link the `id` into table with column head `id_c` (vertical double link)
// void
// linkUD(vec_t(dal_node_t) h, size_t id_c, size_t id)
//{
//         dal_node_t *c = &h[id_c];
//         dal_node_t *p = &h[id];
//         p->C          = id_c;
//
//         c->S += 1;
//
//         size_t id_end = c->U;
//         c->U          = id;
//         h[id_end].D   = id;
//         p->D          = id_c;
//         p->U          = id_end;
// }
//
// void
// cover_col(vec_t(dal_node_t) h, size_t c)
//{
//         h[h[c].R].L = h[c].L;
//         h[h[c].L].R = h[c].R;
//         for (size_t i = h[c].D; i != c; i = h[i].D) {
//                 for (size_t j = h[i].R; j != i; j = h[j].R) {
//                         h[h[j].D].U = h[j].U;
//                         h[h[j].U].D = h[j].D;
//                         (h[h[j].C].S)--;
//                 }
//         }
// }
//
// void
// uncover_col(vec_t(dal_node_t) h, size_t c)
//{
//         for (size_t i = h[c].U; i != c; i = h[i].U) {
//                 for (size_t j = h[i].L; j != i; j = h[j].L) {
//                         (h[h[j].C].S)++;
//                         h[h[j].D].U = j;
//                         h[h[j].U].D = j;
//                 }
//         }
//         h[h[c].R].L = c;
//         h[h[c].L].R = c;
// }
//
// int
// search(vec_t(dal_node_t) h, size_t k, vec_t(size_t) sols)
//{
//         if (h[0].R == 0) {
//                 vecSetSize(sols, (size_t)k);
//                 return 1;
//         }
//
//         size_t c = h[0].R;
//         if (h[c].S == 0) {
//                 return 0;
//         }
//         if (vecCap(sols) < (size_t)k) {
//                 vecReserve(&sols, 2 * vecCap(sols));
//         }
//
//         cover_col(h, c);
//         for (size_t r = h[c].D; r != c; r = h[r].D) {
//                 sols[k] = r;
//                 for (size_t j = h[r].R; j != r; j = h[j].R) {
//                         cover_col(h, h[j].C);
//                 }
//                 if (search(h, k + 1, sols)) return 1;
//                 for (size_t j = h[r].L; j != r; j = h[j].L) {
//                         uncover_col(h, h[j].C);
//                 }
//         }
//         uncover_col(h, c);
//         return 0;
// }
