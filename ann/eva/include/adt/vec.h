#ifndef EVA_VEC_H_
#define EVA_VEC_H_

#include <assert.h>  // assert
#include <stdlib.h>  // free
#include <string.h>  // memcpy

// eva
#include <base/types.h>

// -----------------------------------------------------------------------------
//
// ------------------------
// design (inspired by sds)
// ------------------------
//
// +----+----+------
// |size|cap |buf
// +----+----+------
//           |
//  -2   -1   `-- ptr
//
// - Fast assessing:       Instead of a field lookup, `ptr` points to the buf
//                         head so ptr[x] can be used. With proper vecReserve,
//                         the address is safe.
//
// - Lazy initialization:  vecPushBack or vecReserve allocate the real memory.
//                         The vec must be initialized as NULL (vecNew).
//
// - Dynamic growth:       If the space is not enough, buf will be expanded to
//                         hold more elements in future.
//
// - Iteration:            for(size_t i = 0; i < vecSize(v); i++) { v[i] };
//
// - Fast modification:    Reserve a proper cap. Sets all values directly.
//                         Calls vecSetSize to corrects the size. Use with
//                         caution.
//
// ---------
// onwership
// ---------
//
// 1. Container `vec` does not own any elements. So caller should free them.
// 2. Must call vecFree to release the memory, used by 'vec', on heap.
//
// -------
// caveats
// -------
//
// 1. As the buf might be re-allocated (for growth), pass &vec for
//    modificaitons, e.g., vecReserve, vecPushBack, vecExtend.
// 2. When use vecSetSize, ensure the new values are initialized or old values
//    are freed properly.
//
// -------------
// memory safety
// -------------
// It is so common to use the vec type incorrectly, e.g. passing vec instead of
// &vec to vecReserve or passing &vec to vecSetSize. Due to impl, c compiler
// cannot detect the type issue for us.
//
// One idea to help developer for this is insertaing a magic number say
// 0xFEDCBA at the left of size, i.e.,
//
// +-----+----+----+------
// |magic|size|cap |buf      (EVA_VEC_CHECK_MAGIC mode)
// +-----+----+----+------
//                 |
//  -3   -2   -1   `-- ptr
//
// So, all calls will check this magic number to ensure the address is
// expected. This serves as sanity check and runs under EVA_VEC_CHECK_MAGIC mode
// (assert must work).
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// public apis/macros.
// -----------------------------------------------------------------------------

#define vec_t( type ) type *

#define vecNew( )      NULL
#define vecFree( vec ) _VEC_FREE_IMPL( vec )

#define vecSize( vec )    ( ( vec ) ? ( (size_t *)( vec ) )[-2] : (size_t)0 )
#define vecCap( vec )     ( ( vec ) ? ( (size_t *)( vec ) )[-1] : (size_t)0 )
#define vecIsEmpty( vec ) ( vecSize( ( vec ) ) == 0 )

#define vecSetSize( vec, new_s ) \
        _VEC_SET_SIZE_IMPL( vec, new_s )  // ret error_t
#define vecReserve( pvec, count ) \
        _VEC_RESERVE_IMPL( pvec, count )                       // ret error_t
#define vecPushBack( pvec, v ) _VEC_PUSH_BACK_IMPL( pvec, v )  // ret error_t
#define vecPopBack( vec )      _VEC_POP_BACK_IMPL( vec )       // ret last item
#define vecExtend( pdst, src ) _VEC_EXTEND_IMPL( pdst, src )   // ret error_t

// -----------------------------------------------------------------------------
// private prototype.
// -----------------------------------------------------------------------------

#ifdef EVA_VEC_CHECK_MAGIC
#define _VEC_MAGIC_NUM 0xFEDCBA
#define _VEC_CHECK_MAGIC( vec ) \
        ( assert( ( (size_t *)( vec ) )[-3] == _VEC_MAGIC_NUM ) )
#else
#define _VEC_CHECK_MAGIC( vec ) ( (void)0 )
#endif

#define _VEC_FREE_IMPL( vec ) _vecFree( vec )

#define _VEC_SET_SIZE_IMPL( vec, new_s )                      \
        ( ( vec ) ? ( _VEC_CHECK_MAGIC( ( vec ) ),            \
                      ( (size_t *)vec )[-2] = ( new_s ), OK ) \
                  : ENOTEXIST )

#define _VEC_RESERVE_IMPL( pvec, count ) \
        _vecReserve( (size_t **)( pvec ), count, sizeof( **( pvec ) ) )

#define _VEC_EXTEND_IMPL( pdst, src )                          \
        _vecExtend( (size_t **)( pdst ), vecSize( *( pdst ) ), \
                    sizeof( **( pdst ) ), (size_t *)( src ),   \
                    vecSize( ( src ) ) )

#define _VEC_PUSH_BACK_IMPL( pvec, v )                                  \
        ( _vecGrow( (size_t **)( pvec ), sizeof( **( pvec ) ) ) ||      \
          ( ( ( *( pvec ) )[( (size_t *)( *( pvec ) ) )[-2]] = ( v ) ), \
            ( (size_t *)( *( pvec ) ) )[-2]++, OK ) )

#define _VEC_POP_BACK_IMPL( vec )                                 \
        ( assert( ( vec ) != NULL ), _VEC_CHECK_MAGIC( ( vec ) ), \
          *( ( vec ) + --( ( (size_t *)( vec ) )[-2] ) ) )

#define VEC_INIT_BUF_SIZE 16

extern error_t _vecReserve( _mut_ size_t **vec, size_t new_cap,
                            size_t unit_size );

static inline error_t
_vecGrow( _mut_ size_t **vec, size_t unit_size )
{
        if ( !*vec ) {
                return _vecReserve( vec, VEC_INIT_BUF_SIZE, unit_size );
        }

        _VEC_CHECK_MAGIC( *vec );

        const size_t cap  = ( *vec )[-1];
        const size_t size = ( *vec )[-2];
        assert( size <= cap );
        if ( cap != size ) return OK;
        return _vecReserve( vec, 2 * cap, unit_size );
}

static inline error_t
_vecExtend( _mut_ size_t **pdst, size_t dst_size, size_t unit_size, size_t *src,
            size_t src_size )
{
#ifdef EVA_VEC_CHECK_MAGIC
        if ( *pdst != NULL ) _VEC_CHECK_MAGIC( *pdst );
        if ( src != NULL ) _VEC_CHECK_MAGIC( src );
#endif

        const size_t new_size = dst_size + src_size;
        error_t      err      = _vecReserve( pdst, new_size, unit_size );
        if ( err ) return err;
        memcpy( ( (char *)( *pdst ) ) + unit_size * dst_size, src,
                unit_size * src_size );
        ( *pdst )[-2] = new_size;
        return OK;
}

static inline void
_vecFree( void *vec )
{
        if ( vec != NULL ) {
                _VEC_CHECK_MAGIC( vec );
#ifdef EVA_VEC_CHECK_MAGIC
                free( &( (size_t *)vec )[-3] );
#else
                free( &( (size_t *)vec )[-2] );
#endif
        }
}

#endif  // EVA_VEC_H_
