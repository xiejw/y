#include <mlvm/vm.h>

#include <assert.h>
#include <stdlib.h>

struct vm_sp *
spNew( int rank, int *dims )
{
        // some thoughts: if rank <= 3, we could build a bst to lookup existing
        // shape and reusing them. This should be fairly cheap to do and save
        // heap allocations.

        if ( rank == 0 ) return NULL;

        struct vm_sp *s =
            malloc( sizeof( struct vm_sp ) + (size_t)rank * sizeof( int ) );

        uint64_t size = 1;
        for ( int i = 0; i < rank; i++ ) {
                int d = dims[i];
                assert( d > 0 );
                size *= (size_t)d;
                s->dims[i] = d;
        }

        s->rank      = rank;
        s->ref_count = 1;
        s->size      = size;

        return s;
}

struct vm_sp *
spIncRef( struct vm_sp *p )
{
        p->ref_count++;
        return p;
}

struct vm_sp *
spDecRef( struct vm_sp *p )
{
        if ( --( p->ref_count ) == 0 ) {
                free( p );
                return NULL;
        }
        return p;
}
