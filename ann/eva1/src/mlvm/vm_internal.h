// Internal for MLVM vm only. Guard by MLVM_VM_SPEC and MLVM_VM_INTERNAL symbol.
//
// Not designed to be included in general. Use vm.h instead.

// =============================================================================
// internal vm spec
// =============================================================================
#if defined( MLVM_VM_SPEC ) || defined( MLVM_VM_INTERNAL )

#ifndef MLVM_MAX_TENSOR_COUNT
#define MLVM_MAX_TENSOR_COUNT 128
#endif  // MLVM_MAX_TENSOR_COUNT

#endif  // MLVM_VM_SPEC

// =============================================================================
// internal vm apis
// =============================================================================

#ifdef MLVM_VM_INTERNAL

#include <assert.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// the vm true def
// -----------------------------------------------------------------------------

struct _vm_list;

struct vm {
        // consider to use pages.
        struct vm_tensor handles[MLVM_MAX_TENSOR_COUNT];
        struct _vm_list *shapes;
};

// aux data structure.
struct _vm_list {
        void            *data;
        struct _vm_list *next;
};

// -----------------------------------------------------------------------------
// vm helpers
// -----------------------------------------------------------------------------

static inline struct vm_tensor *
vmGrabHandle( struct vm *vm, int handle )
{
        assert( handle >= 0 && handle < MLVM_MAX_TENSOR_COUNT );
        return &vm->handles[handle];
}

// tensor is allocated in pages. so, we set the fields and mark as used.
static inline void
vmFillHandle( struct vm_tensor *t, enum vm_dtype dtype, struct vm_sp *s,
              void *data )
{
        assert( !( t->used ) );
        assert( t->shape == NULL );
        assert( t->data == NULL );
        t->dtype = dtype;
        t->used  = 1;
        t->shape = spIncRef( s );
        t->data  = data;
}

// tensor is allocated in pages. so, we free the fields and mark as unused.
static inline void
vmReleaseHandle( struct vm_tensor *t )
{
        assert( t->used );
        free( t->data );
        spDecRef( t->shape );

        t->shape = NULL;
        t->data  = NULL;
        t->used  = 0;
}

#endif  // MLVM_VM_INTERNAL
