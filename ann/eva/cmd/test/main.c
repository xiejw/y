#include <testing/test_main.h>

int
main( )
{
    // ---------------------------------------------------------------------------
    // Adds all suites.
    //
    // Convenstion is for foo, a test suite fn run_foo_suite is called. For
    // customized case, use ADD_SUITE_NAME_AND_FN.

    ADD_SUITE( adt_value );          // adt/value_test.c
    ADD_SUITE( adt_vec );            // adt/vec_test.c
    ADD_SUITE( adt_dict );           // adt/dict_test.c
    ADD_SUITE( adt_list );           // adt/list_test.c
    ADD_SUITE( adt_sds );            // adt/sds_test.c
                                     //
    ADD_SUITE( algos_regex );        // algos/regex_test.c
                                     //
    ADD_SUITE( base_bitops );        // base/bitops_test.c
    ADD_SUITE( base_error );         // base/error_test.c
    ADD_SUITE( base_fpath );         // base/fpath_test.c
    ADD_SUITE( base_mm );            // base/mm_test.c
                                     //
    ADD_SUITE( crypto_sha256 );      // crypto/sha256_test.c
    ADD_SUITE( crypto_rng64 );       // crypto/rng64_test.c
    ADD_SUITE( crypto_rng64_stdn );  // crypto/rng64_stdn_test.c
                                     //
    ADD_SUITE( mlvm_kernel );        // mlvm/kernel_test.c
    ADD_SUITE( mlvm_shape );         // mlvm/shape_test.c
    ADD_SUITE( mlvm_tensor );        // mlvm/tensor_test.c
    ADD_SUITE( mlvm_vm );            // mlvm/vm_test.c
                                     //
#ifndef _SKIP_ASAN                   //
    ADD_SUITE( sched_cron );         // sched/cron_test.c
#endif                               // _SKIP_ASAN
#ifdef PTHREAD
    ADD_SUITE( sched_fork_join );  // sched/fork_join_test.c
#endif                             // PTHREAD
    // ---------------------------------------------------------------------------
    // Runs all suites and reports.
    int suites_failed = run_all_suites( );
    return suites_failed ? -1 : 0;
}
