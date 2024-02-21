// vim: ft=cpp
#pragma once

#include <cstdlib>
#include <memory>

namespace algos::dal {

struct Node {
    std::size_t id;
    std::size_t L;
    std::size_t R;
    std::size_t U;
    std::size_t D;
    union {
        std::size_t C;  // head id of the vertical col. used by non-head.
        std::size_t S;  // count of the vertical col. used by head.
    };
    void *data;
};

struct Table {
  private:
    size_t                                         m_num_colheads;
    size_t                                         m_num_nodes;
    std::unique_ptr<Node, decltype( std::free ) *> m_nodes;

  public:
    // Reserve reserve_n nodes. It must cover 1 header, all column heads
    // (items) and all options.
    // // allocate n column heads and link them as the horizantal link list.
    Table( std::size_t reserve_n, std::size_t num_col_heads );
};

// // append one option to cover num_ids column heads specified by col_ids.
// //
// // all nodes will be linked to the vertical list of column head and
// horizantal
// // list of this option.
// extern void dalAppendOption(struct dal_table *h, size_t num_ids,
//                             size_t *col_ids, void *priv_data);
//
// // cover all nodes in a column. also unlink nodes from their columns
// belonging
// // to the same option.
// extern void dalCoverCol(struct dal_table *t, size_t c);
//
// // search a solution and stop. return 1 if found any.
// extern int dalSearchSolution(struct dal_table *h, vec_t(size_t) sols);
//
// #define dalNodeData(t, id) ((t)->nodes[id].data)
//
}  // namespace algos::dal
