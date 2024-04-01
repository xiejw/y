#include <algos/dal.h>

#include <cassert>

#include <eve/base/error.h>

namespace algos::dal {

void
Table::FillNode( Node &node, std::size_t id )
{
    node.id = id;
    node.L  = id;
    node.R  = id;
    node.U  = id;
    node.D  = id;
    node.C  = 0;
}

// Link the `id` into table after node `end` (horizantal double link)
void
Table::LinkLR( Node *h, size_t end, size_t id )
{
    auto p    = &h[id];
    p->L      = end;
    p->R      = h[end].R;
    h[end].R  = id;
    h[p->R].L = id;
}

// Link the `id` into table with column head `id_c` (vertical double link)
void
Table::LinkUD( Node *h, size_t id_c, size_t id )
{
    auto *c = &h[id_c];
    auto *p = &h[id];
    p->C    = id_c;

    c->S += 1;

    size_t id_end = c->U;
    c->U          = id;
    h[id_end].D   = id;
    p->D          = id_c;
    p->U          = id_end;
}

void
Table::CoverColumn( Node *h, size_t c )
{
    h[h[c].R].L = h[c].L;
    h[h[c].L].R = h[c].R;
    for ( size_t i = h[c].D; i != c; i = h[i].D ) {
        for ( size_t j = h[i].R; j != i; j = h[j].R ) {
            h[h[j].D].U = h[j].U;
            h[h[j].U].D = h[j].D;
            ( h[h[j].C].S )--;
        }
    }
}

void
Table::UncoverColumn( Node *h, size_t c )
{
    for ( size_t i = h[c].U; i != c; i = h[i].U ) {
        for ( size_t j = h[i].L; j != i; j = h[j].L ) {
            ( h[h[j].C].S )++;
            h[h[j].D].U = j;
            h[h[j].U].D = j;
        }
    }
    h[h[c].R].L = c;
    h[h[c].L].R = c;
}

Table::Table( std::size_t n_col_heads, std::size_t n_options_total )
    : m_nodes{ nullptr, std::free }
{
    auto total_reserved_nodes_count = 1 + n_col_heads + n_options_total;
    this->m_num_nodes_total         = total_reserved_nodes_count;
    this->m_num_nodes_added         = 1 + n_col_heads;
    this->m_nodes.reset(
        (Node *)malloc( sizeof( Node ) * total_reserved_nodes_count ) );

    auto nodes = this->m_nodes.get( );
    FillNode( nodes[0], /*id=*/0 );

    for ( std::size_t i = 1; i <= n_col_heads; i++ ) {
        FillNode( nodes[i], i );
        LinkLR( nodes, i - 1, i );
    }
}

void
Table::CoverCol( size_t c )
{
    CoverColumn( this->m_nodes.get( ), c );
}

void
Table::AppendOption( std::span<std::size_t> col_ids, void *data )
{
    auto  *nodes     = this->m_nodes.get( );
    size_t offset_id = this->m_num_nodes_added;
    auto   num_ids   = col_ids.size( );

    if ( offset_id + num_ids > this->m_num_nodes_total ) {
        panic(
            "Reserved space is not enough for dancing link table: reserved "
            "with %d, used %d, needed %d more.",
            this->m_num_nodes_total, offset_id, num_ids );
    }

    for ( size_t i = 0; i < num_ids; i++ ) {
        size_t id = offset_id + i;
        FillNode( nodes[id], id );
        LinkUD( nodes, col_ids[i], id );
        nodes[id].data = data;
        if ( i != 0 ) {
            LinkLR( nodes, id - 1, id );
        }
    }

    this->m_num_nodes_added += num_ids;
}

bool
Table::SearchSolution( std::vector<std::size_t> &sols )
{
    assert( sols.size( ) == 0 );
    return Search( sols, 0 );
}

bool
Table::Search( std::vector<std::size_t> &sols, std::size_t k )
{
    auto h = this->m_nodes.get( );

    if ( h[0].R == 0 ) {
        return true;
    }

    size_t c = h[0].R;
    if ( h[c].S == 0 ) {
        return false;
    }

    this->CoverColumn( h, c );
    for ( size_t r = h[c].D; r != c; r = h[r].D ) {
        if ( sols.size( ) == k ) {
            sols.push_back( r );
        } else {
            sols[k] = r;
        }
        assert( sols.size( ) >= k );
        for ( size_t j = h[r].R; j != r; j = h[j].R ) {
            this->CoverColumn( h, h[j].C );
        }
        if ( Search( sols, k + 1 ) ) return true;
        for ( size_t j = h[r].L; j != r; j = h[j].L ) {
            this->UncoverColumn( h, h[j].C );
        }
    }
    this->UncoverColumn( h, c );
    return false;
}

}  // namespace algos::dal

///-----------------------------------------------------------------------------
//// Helper Methods Implementation.
////
///-----------------------------------------------------------------------------
//
