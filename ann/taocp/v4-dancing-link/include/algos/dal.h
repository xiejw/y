// vim: ft=cpp
#pragma once

#include <cstdlib>
#include <memory>
#include <span>
#include <vector>

namespace algos::dal {

struct Node {
  private:
    std::size_t Id;
    std::size_t L;
    std::size_t R;
    std::size_t U;
    std::size_t D;
    union {
        std::size_t C;  // head Id of the vertical col. used by non-head.
        std::size_t S;  // count of the vertical col. used by head.
    };
    void *Data;  // unowned.

  public:
    friend struct Table;
};

// The dancing link table used to solve the problem.
struct Table {
  private:
    size_t                                         mNumNodesTotal;
    size_t                                         mNumNodesAdded;
    std::unique_ptr<Node, decltype( std::free ) *> mNodes;

  public:
    // One time allocation to reserve in total 1 + n_col_heads +
    // n_options_total nodesin the table.
    //
    // It must cover 1 system header, all column heads (items) and all options.
    // In addition, also allocate n_col_heads column heads and link them as the
    // horizantal link list.
    Table( std::size_t n_col_heads, std::size_t n_options_total );

  public:
    //  Cover all nodes in a column. Also unlink nodes from their columns
    //  belonging to the same option.
    void coverCol( size_t c );

    // Append a group of options which are mutually exclusive.
    //
    // All nodes will be linked to the vertical list of column head and
    // horizantal list of this group.
    //
    // The `priv_data` is not owned by this table.
    void appendOption( std::span<std::size_t> col_ids, void *priv_data );

    void *getNodeData( std::size_t Id )
    {
        return this->mNodes.get( )[Id].Data;
    };

    bool searchSolution( std::vector<std::size_t> &sols );

  private:
    void fillNode( Node &node, std::size_t Id );
    void linkLR( Node *h, size_t end, size_t Id );
    void linkUD( Node *h, size_t id_c, size_t Id );
    void coverColumn( Node *h, size_t c );
    void uncoverColumn( Node *h, size_t c );
    bool search( std::vector<std::size_t> &sols, std::size_t depth );
};

//
//
// // search a solution and stop. return 1 if found any.
// extern int dalSearchSolution(struct dal_table *h, vec_t(size_t) sols);
//
// #define dalNodeData(t, Id) ((t)->nodes[Id].Data) //
}  // namespace algos::dal
