#include <mlvm/vm.h>

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

// eva internal
#include <base/error.h>
#include <mlvm/vm_internal.h>

int
vmTensorNew( struct vm *vm, enum vm_dtype dtype, struct vm_sp *s )
{
    void             *data;
    struct vm_tensor *p = vm->handles;
    int               slot;
    for ( slot = 0; slot < MLVM_MAX_TENSOR_COUNT; slot++, p++ ) {
        if ( p->used == 0 ) goto alloc;
    }
    return errNew( "all handle spaces are used." );

alloc:
    switch ( dtype ) {
    case VM_F32:
        data = malloc( s->size * sizeof( f32_t ) );
        break;
    case VM_U64:
        data = malloc( s->size * sizeof( u64_t ) );
        break;
    default:
        errFatalAndExit( "unsupported dtype for new tensor %d", dtype );
    }

    vmFillHandle( p, dtype, s, data );
    return slot;
}

error_t
vmTensorFree( struct vm *vm, int handle )
{
    vmReleaseHandle( vmGrabHandle( vm, handle ) );
    return OK;
}

error_t
vmTensorInfo( struct vm *vm, int handle, enum vm_dtype *dtype,
              struct vm_sp **shape )
{
    struct vm_tensor *t = vmGrabHandle( vm, handle );

    assert( t->used );
    if ( dtype != NULL ) *dtype = t->dtype;
    if ( shape != NULL ) *shape = t->shape;
    return OK;
}

error_t
vmTensorData( struct vm *vm, int handle, void **data )
{
    struct vm_tensor *t = vmGrabHandle( vm, handle );

    assert( t->used );
    *data = t->data;
    return OK;
}

error_t
vmTensorSwap( struct vm *vm, int t, _mut_ void **data )
{
    struct vm_tensor *ts  = vmGrabHandle( vm, t );
    void             *old = ts->data;
    ts->data              = *data;
    *data                 = old;
    return OK;
}

void
vmTensorToSds( sds_t *s, struct vm *vm, int handle )
{
    struct vm_tensor *t = vmGrabHandle( vm, handle );
    tensorLikeToSds( s, t->shape, t->dtype, t->data );
    return;
}

void
tensorLikeToSds( sds_t *s, struct vm_sp *sp, enum vm_dtype dtype,
                 void *nullable_data )
{
    // shape
    sdsCatPrintf( s, "<" );
    for ( int i = 0; i < sp->rank; i++ ) {
        if ( i != sp->rank - 1 )
            sdsCatPrintf( s, "%d, ", sp->dims[i] );
        else
            sdsCatPrintf( s, "%d", sp->dims[i] );
    }
    sdsCatPrintf( s, "> " );

    // dtype
    switch ( dtype ) {
    case VM_F32:
        sdsCatPrintf( s, "f32" );
        break;
    case VM_U64:
        sdsCatPrintf( s, "u64" );
        break;
    default:
        errFatalAndExit( "unsupported dtype for new tensor %d", dtype );
    }

    if ( nullable_data == NULL ) return;

    sdsCatPrintf( s, " " );  // extra space to separate data
    void *data = nullable_data;

    // data
#ifndef MLVM_TENSOR_DUMP_SIZE
#define MLVM_TENSOR_DUMP_SIZE 10
#endif

#define print_data( dt, type_cast, fmt_str )                             \
    do {                                                                 \
        if ( dtype == ( dt ) ) {                                         \
            size_t size = sp->size;                                      \
            sdsCatPrintf( s, "[" );                                      \
            for ( size_t i = 0; i < size; i++ ) {                        \
                if ( i != size - 1 )                                     \
                    sdsCatPrintf( s, fmt_str ", ",                       \
                                  (double)( (type_cast)( data ) )[i] );  \
                else                                                     \
                    sdsCatPrintf( s, fmt_str,                            \
                                  (double)( (type_cast)( data ) )[i] );  \
                if ( i + 1 >= MLVM_TENSOR_DUMP_SIZE && i != size - 1 ) { \
                    sdsCatPrintf( s, "..." );                            \
                    break;                                               \
                }                                                        \
            }                                                            \
            sdsCatPrintf( s, "]" );                                      \
        }                                                                \
    } while ( 0 )

    print_data( VM_F32, f32_t *, "%.3f" );
    print_data( VM_U64, u64_t *, "%" PRIu64 );

#undef print_data
#undef MLVM_TENSOR_DUMP_SIZE
}
