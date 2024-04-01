#include <algos/dal.h>

#include <cassert>

#include <eve/base/error.h>

namespace algos::dal {

void
Table::fillNode( Node &node, std::size_t Id )
{
    node.Id = Id;
    node.L  = Id;
    node.R  = Id;
    node.U  = Id;
    node.D  = Id;
    node.C  = 0;
}

// Link the `Id` into table after node `end` (horizantal double link)
void
Table::linkLR( Node *h, size_t end, size_t Id )
{
    auto p    = &h[Id];
    p->L      = end;
    p->R      = h[end].R;
    h[end].R  = Id;
    h[p->R].L = Id;
}

// Link the `Id` into table with column head `Id_c` (vertical double link)
void
Table::linkUD( Node *h, size_t Id_c, size_t Id )
{
    auto *c = &h[Id_c];
    auto *p = &h[Id];
    p->C    = Id_c;

    c->S += 1;

    size_t Id_end = c->U;
    c->U          = Id;
    h[Id_end].D   = Id;
    p->D          = Id_c;
    p->U          = Id_end;
}

void
Table::coverColumn( Node *h, size_t c )
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
Table::uncoverColumn( Node *h, size_t c )
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
    : mNodes{ nullptr, std::free }
{
    auto total_reserved_nodes_count = 1 + n_col_heads + n_options_total;
    this->mNumNodesTotal            = total_reserved_nodes_count;
    this->mNumNodesAdded            = 1 + n_col_heads;
    this->mNodes.reset(
        (Node *)malloc( sizeof( Node ) * total_reserved_nodes_count ) );

    auto nodes = this->mNodes.get( );
    fillNode( nodes[0], /*Id=*/0 );

    for ( std::size_t i = 1; i <= n_col_heads; i++ ) {
        fillNode( nodes[i], i );
        linkLR( nodes, i - 1, i );
    }
}

void
Table::coverCol( size_t c )
{
    coverColumn( this->mNodes.get( ), c );
}

void
Table::appendOption( std::span<std::size_t> col_Ids, void *Data )
{
    auto  *nodes     = this->mNodes.get( );
    size_t offset_Id = this->mNumNodesAdded;
    auto   num_Ids   = col_Ids.size( );

    if ( offset_Id + num_Ids > this->mNumNodesTotal ) {
        panic(
            "Reserved space is not enough for dancing link table: reserved "
            "with %d, used %d, needed %d more.",
            this->mNumNodesTotal, offset_Id, num_Ids );
    }

    for ( size_t i = 0; i < num_Ids; i++ ) {
        size_t Id = offset_Id + i;
        fillNode( nodes[Id], Id );
        linkUD( nodes, col_Ids[i], Id );
        nodes[Id].Data = Data;
        if ( i != 0 ) {
            linkLR( nodes, Id - 1, Id );
        }
    }

    this->mNumNodesAdded += num_Ids;
}

bool
Table::searchSolution( std::vector<std::size_t> &sols )
{
    assert( sols.size( ) == 0 );
    return search( sols, 0 );
}

bool
Table::search( std::vector<std::size_t> &sols, std::size_t k )
{
    auto h = this->mNodes.get( );

    if ( h[0].R == 0 ) {
        return true;
    }

    size_t c = h[0].R;
    if ( h[c].S == 0 ) {
        return false;
    }

    this->coverColumn( h, c );
    for ( size_t r = h[c].D; r != c; r = h[r].D ) {
        if ( sols.size( ) == k ) {
            sols.push_back( r );
        } else {
            sols[k] = r;
        }
        assert( sols.size( ) >= k );
        for ( size_t j = h[r].R; j != r; j = h[j].R ) {
            this->coverColumn( h, h[j].C );
        }
        if ( search( sols, k + 1 ) ) return true;
        for ( size_t j = h[r].L; j != r; j = h[j].L ) {
            this->uncoverColumn( h, h[j].C );
        }
    }
    this->uncoverColumn( h, c );
    return false;
}

}  // namespace algos::dal

///-----------------------------------------------------------------------------
//// Helper Methods Implementation.
////
///-----------------------------------------------------------------------------
//
