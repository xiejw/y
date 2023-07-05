#ifndef OPUS_DAL_H_
#define OPUS_DAL_H_

// eva
#include <adt/vec.h>

// -----------------------------------------------------------------------------
// public data structures and apis
// -----------------------------------------------------------------------------
struct dal_node {
        size_t id;
        size_t L;
        size_t R;
        size_t U;
        size_t D;
        union {
                size_t C;  // head id of the vertical col. used by non-head.
                size_t S;  // count of the vertical col. used by head.
        };
        void *data;
};

struct dal_table {
        size_t num_colheads;
        size_t num_nodes;
        vec_t(struct dal_node) nodes;
};

// -----------------------------------------------------------------------------
// public apis
// -----------------------------------------------------------------------------

// create and free dancing link data structure.
//
// dalNew:
//     Reserve reserve_n nodes. It must cover 1 header, all column heads
//     (items) and all options. After this method, call dalAllocColHeads
//     immediately.
extern struct dal_table *dalNew(size_t reserve_n);
extern void dalFree(struct dal_table *h);

// allocate n column heads and link them as the horizantal link list.
extern void dalAllocColHeads(struct dal_table *h, size_t num_col_heads);

// append one option to cover num_ids column heads specified by col_ids.
//
// all nodes will be linked to the vertical list of column head and horizantal
// list of this option.
extern void dalAppendOption(struct dal_table *h, size_t num_ids,
                            size_t *col_ids, void *priv_data);

// cover all nodes in a column. also unlink nodes from their columns belonging
// to the same option.
extern void dalCoverCol(struct dal_table *t, size_t c);

// search a solution and stop. return 1 if found any.
extern int dalSearchSolution(struct dal_table *h, vec_t(size_t) sols);

#define dalNodeData(t, id) ((t)->nodes[id].data)

#endif  // OPUS_DAL_H_
