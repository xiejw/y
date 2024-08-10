// The purpose of this mlvm/main.c is to test performance of large matrix
// multiplication.
//
// clang-format off
//
// Each sample counts as 0.01 seconds.
//   %   cumulative   self              self     total
//  time   seconds   seconds    calls  Ts/call  Ts/call  name
//  88.11      8.00     8.00                             bli_sgemm_skx_asm_32x12_l2
//   6.28      8.57     0.57                             bli_sscal2v_skx_ref
//   2.75      8.82     0.25                             rng64StdNormalF
//   1.54      8.96     0.14                             bli_spackm_12xk_skx_ref
//   0.77      9.03     0.07                             rng64NextUint64
//   0.33      9.06     0.03                             bli_sgemm_ker_var2
//   0.11      9.07     0.01                             bli_gemm_ker_var2
//   0.11      9.08     0.01                             bli_sscal2m_unb_var1
//   0.00      9.08     0.00       12     0.00     0.00  vmExec
//   0.00      9.08     0.00       10     0.00     0.00  vmOpMatmulF32
//   0.00      9.08     0.00        4     0.00     0.00  spDecRef
//   0.00      9.08     0.00        3     0.00     0.00  spIncRef
//   0.00      9.08     0.00        3     0.00     0.00  vmTensorNew
//   0.00      9.08     0.00        2     0.00     0.00  vmOpRngF32
//   0.00      9.08     0.00        1     0.00     0.00  spNew
//   0.00      9.08     0.00        1     0.00     0.00  vmFree
//   0.00      9.08     0.00        1     0.00     0.00  vmNew
//
// clang-format on

// eva
#include <adt/sds.h>
#include <base/log.h>
#include <crypto/rng64.h>
#include <mlvm/vm.h>

#define NE( e ) _NO_ERR_IMPL_( e, __FILE__, __LINE__ )

#define _NO_ERR_IMPL_( e, f, l )                                   \
    do {                                                           \
        if ( e ) {                                                 \
            err = e;                                               \
            errDump( "unexpected error @ file %s line %d", f, l ); \
            goto cleanup;                                          \
        }                                                          \
    } while ( 0 )

#define DEFAULT_SHAPE \
    2, ( int[] ) { 4096, 4096 }

#define DEFAULT_ITERATION_COUNT 10

int
main( void )
{
    error_t err = OK;

    logInfo( "MLVM Demo: Matmul\n" );

    // ---
    // Defines vm with some shapes.

    struct vm      *vm = vmNew( );
    struct vm_sp   *s  = spNew( DEFAULT_SHAPE );
    struct vm_opopt opt;

    // ---
    // Prepares the seeds, one for model and one for input.

    struct rng64 *seed = rng64New( 123 );
    struct rng64 *rng;  // free after each use.

    // ---
    // Allocates the tensors for inputs (x,y) and output (z).

    int x = vmTensorNew( vm, VM_F32, s );
    int y = vmTensorNew( vm, VM_F32, s );
    int z = vmTensorNew( vm, VM_F32, s );

    // ---
    // Initializes x and y.
    rng      = rng64Split( seed );
    opt.mode = VM_OPT_RNG_STD_NORMAL | VM_OPT_MODE_R_BIT;
    opt.r    = *rng;
    NE( vmExec( vm, VM_OP_RNG, &opt, x, -1, -1 ) );
    rng64Free( rng );

    rng      = rng64Split( seed );
    opt.mode = VM_OPT_RNG_STD_NORMAL | VM_OPT_MODE_R_BIT;
    opt.r    = *rng;
    NE( vmExec( vm, VM_OP_RNG, &opt, y, -1, -1 ) );
    rng64Free( rng );

    // ---
    // Executes the matmul z = matmul(x, y).
    for ( int i = 0; i < DEFAULT_ITERATION_COUNT; i++ ) {
        logInfo( "iteration %d\n", i );
        NE( vmExec( vm, VM_OP_MATMUL, NULL, z, x, y ) );
    }

cleanup:
    spDecRef( s );
    rng64Free( seed );
    vmFree( vm );
    return err;
}
