#include <testing/testing.h>

#include <mlvm/vm.h>

#include <string.h>

static char *
test_vm_new( )
{
    struct vm *vm = vmNew( );
    ASSERT_TRUE( "vm", vm != NULL );
    vmFree( vm );
    return NULL;
}

static char *
test_vm_exec( )
{
    struct vm *vm = vmNew( );

    int t1, t2, td;
    {
        // create handles.
        struct vm_sp *s = vmSpNew( vm, 2, (int[]){ 1, 2 } );

        t1 = vmTensorNew( vm, VM_F32, s );
        t2 = vmTensorNew( vm, VM_F32, s );
        td = vmTensorNew( vm, VM_F32, s );
        ASSERT_TRUE( "t1", t1 == 0 );
        ASSERT_TRUE( "t2", t2 == 1 );
        ASSERT_TRUE( "td", td == 2 );
    }

    {
        // cp data into t1 and t2.
        f32_t  t1_src[2] = { 2.34f, 5.67f };
        f32_t  t2_src[2] = { 4.34f, 3.67f };
        f32_t *data;

        ASSERT_TRUE( "err", OK == vmTensorData( vm, t1, (void **)&data ) );
        memcpy( data, t1_src, 2 * sizeof( f32_t ) );
        ASSERT_TRUE( "err", OK == vmTensorData( vm, t2, (void **)&data ) );
        memcpy( data, t2_src, 2 * sizeof( f32_t ) );
    }

    ASSERT_TRUE( "err", OK == vmExec( vm, VM_OP_ADD, NULL, td, t1, t2 ) );

    {
        sds_t s = sdsEmpty( );
        vmTensorToSds( &s, vm, td );
        const char *expected = "<1, 2> f32 [6.680, 9.340]";
        ASSERT_TRUE( "dump", 0 == strcmp( s, expected ) );
        sdsFree( s );
    }
    vmFree( vm );
    return NULL;
}

static char *
test_vm_batch( )
{
    struct vm *vm = vmNew( );

    int t1, t2, td;
    {
        // create handles.
        struct vm_sp *s = vmSpNew( vm, 2, (int[]){ 1, 2 } );

        t1 = vmTensorNew( vm, VM_F32, s );
        t2 = vmTensorNew( vm, VM_F32, s );
        td = vmTensorNew( vm, VM_F32, s );
        ASSERT_TRUE( "t1", t1 == 0 );
        ASSERT_TRUE( "t2", t2 == 1 );
        ASSERT_TRUE( "td", td == 2 );
    }

    {
        // cp data into t1 and t2.
        f32_t  t1_src[2] = { 2.34f, 5.67f };
        f32_t  t2_src[2] = { 4.34f, 3.67f };
        f32_t *data;

        ASSERT_TRUE( "err", OK == vmTensorData( vm, t1, (void **)&data ) );
        memcpy( data, t1_src, 2 * sizeof( f32_t ) );
        ASSERT_TRUE( "err", OK == vmTensorData( vm, t2, (void **)&data ) );
        memcpy( data, t2_src, 2 * sizeof( f32_t ) );
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"

    const struct vm_oparg program[] = {
        // clang-format off
                {VM_OP_ADD, td, t1, t2, /*has_opt*/ 0},
                {VM_OP_MUL, td, td, -1, /*has_opt*/ 1, {.mode = VM_OPT_MODE_F_BIT, .f = 2.0}},
        // clang-format on
    };

#pragma clang diagnostic pop

    ASSERT_TRUE(
        "err", OK == vmBatch( vm, sizeof( program ) / sizeof( struct vm_oparg ),
                              program ) );

    {
        sds_t s = sdsEmpty( );
        vmTensorToSds( &s, vm, td );
        const char *expected = "<1, 2> f32 [13.360, 18.680]";
        ASSERT_TRUE( "dump", 0 == strcmp( s, expected ) );
        sdsFree( s );
    }
    vmFree( vm );
    return NULL;
}

DECLARE_TEST_SUITE( mlvm_vm )
{
    RUN_TEST( test_vm_new );
    RUN_TEST( test_vm_exec );
    RUN_TEST( test_vm_batch );
    return NULL;
}
