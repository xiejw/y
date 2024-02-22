// vim: ft=cpp
#pragma once

#include <cstdlib>
#include <memory>
#include <span>

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
    void *data;  // unowned.
};

struct Table {
  private:
    size_t                                         m_num_nodes_total;
    size_t                                         m_num_nodes_added;
    std::unique_ptr<Node, decltype( std::free ) *> m_nodes;

  public:
    // Reserve reserve_n nodes. It must cover 1 header, all column heads
    // (items) and all options.
    // // allocate n column heads and link them as the horizantal link list.
    Table( std::size_t n_col_heads, std::size_t n_options_total );

    //  Cover all nodes in a column. also unlink nodes from their columns
    //  belonging to the same option.
    void CoverCol( size_t c );

    // Append one option to cover num_ids column heads specified by col_ids.
    //
    // all nodes will be linked to the vertical list of column head and
    // horizantal
    // list of this option.
    //
    // The `priv_data` is not owned by this table.
    void AppendOption( std::span<std::size_t> col_ids, void *priv_data );
};

//
//
// // search a solution and stop. return 1 if found any.
// extern int dalSearchSolution(struct dal_table *h, vec_t(size_t) sols);
//
// #define dalNodeData(t, id) ((t)->nodes[id].data)
//
}  // namespace algos::dal
