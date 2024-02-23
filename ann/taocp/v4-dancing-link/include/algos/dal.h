// vim: ft=cpp
#pragma once

#include <cstdlib>
#include <memory>
#include <span>
#include <vector>

namespace algos::dal {

struct Node {
  private:
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

  public:
    friend struct Table;
};

// The dancing link table used to solve the problem.
struct Table {
  private:
    size_t                                         m_num_nodes_total;
    size_t                                         m_num_nodes_added;
    std::unique_ptr<Node, decltype( std::free ) *> m_nodes;

  public:
    // One time allocation to reserve in total 1 + n_col_heads +
    // n_options_total nodesin the table.
    //
    // It must cover 1 system header, all column heads (items) and all options.
    // In addition, also allocate n_col_heads column heads and link them as the
    // horizantal link list.
    Table( std::size_t n_col_heads, std::size_t n_options_total );

    //  Cover all nodes in a column. Also unlink nodes from their columns
    //  belonging to the same option.
    void CoverCol( size_t c );

    // Append a group of options which are mutually exclusive.
    //
    // All nodes will be linked to the vertical list of column head and
    // horizantal list of this group.
    //
    // The `priv_data` is not owned by this table.
    void AppendOption( std::span<std::size_t> col_ids, void *priv_data );

    void *NodeData( std::size_t id ) { return this->m_nodes.get( )[id].data; };

    bool SearchSolution( std::vector<std::size_t> &sols );

  private:
    void FillNode( Node &node, std::size_t id );
    void LinkLR( Node *h, size_t end, size_t id );
    void LinkUD( Node *h, size_t id_c, size_t id );
    void CoverColumn( Node *h, size_t c );
    void UncoverColumn( Node *h, size_t c );
    bool Search( std::vector<std::size_t> &sols, std::size_t depth );
};

//
//
// // search a solution and stop. return 1 if found any.
// extern int dalSearchSolution(struct dal_table *h, vec_t(size_t) sols);
//
// #define dalNodeData(t, id) ((t)->nodes[id].data)
//
}  // namespace algos::dal
