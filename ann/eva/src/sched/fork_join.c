#include "sched/fork_join.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

error_t
fork_join_launch( size_t parallelism, void *( *fn_to_run )(void *), void *args,
                  size_t nbytes, int flag )
{
    error_t    err      = OK;
    pthread_t *pthreads = malloc( sizeof( pthread_t ) * parallelism );

    for ( size_t i = 0; i < parallelism; i++ ) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"
        void *arg;
        if ( flag == FORK_JOIN_ADDR_PTR_TO_VAL ) {
            arg = (void *)( (char *)args + i * nbytes );
        } else {
            assert( FORK_JOIN_ADDR_PTR_TO_PTR == flag );
            arg = (void *)*(char **)( (char *)args + i * nbytes );
        }
#pragma clang diagnostic pop

        if ( ( err = pthread_create( &pthreads[i], NULL, fn_to_run, arg ) ) !=
             OK ) {
            // we might have some memory leaks here. consider to
            // avoid them.
            err = errNew( "failed to launch thread %zu: %d", i, err );
            goto exit;
        }
    }

    for ( size_t i = 0; i < parallelism; i++ )
        pthread_join( pthreads[i], NULL );

exit:
    if ( pthreads != NULL ) free( pthreads );
    return err;
}
