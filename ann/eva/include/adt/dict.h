// copyright: see license_antirez-bsd file.

#ifndef EVA_DICT_H_
#define EVA_DICT_H_

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

// eva
#include <adt/value.h>
#include <base/types.h>

// =============================================================================
// hash dict data strucut for both high level and low level
// =============================================================================

struct dict_entry {
        union value        key;
        union value        v;
        struct dict_entry *next;
};

typedef void *( *dict_dup_fn_t )( void *privdata, const void *data );
typedef void ( *dict_free_fn_t )( void *privdata, void *data );

// =============================================================================
// general hash dict data structure to support low level apis
// =============================================================================

struct dict_type {
        // required field
        //
        // NOTE: union value bits comparison or hashing might not be correct.
        uint64_t ( *hashFn )( const union value key );

        // required field
        //
        // NOTE: CmpFn returns 1 iff the keys are considered to be same.
        int ( *keyCmpFn )( void *privdata, const union value key1,
                           const union value key2 );

        // optional fields
        //
        // - if provided, the (void * data) field in key/value is passed to.
        //
        // - also note, these will be eagerly used when present. So all
        //   key/values will be assume to be .data field and passed into these
        //   functions unconditionally.
        //
        // usage:
        //
        // - keyDupFn is called on the provided new key when adding a new
        //   entry.
        //
        // - valDupFn is called on the provided new data value during setting,
        //   replacement, and addition.
        //
        // This is mainly for convenience so users do not need to dup the
        // key/value before using these apis.
        dict_dup_fn_t keyDupFn;
        dict_dup_fn_t valDupFn;

        // optional fields
        //
        // - freeFn is called during table free process or a key/value is
        //   removed during setting or replacement.
        //
        // - similarly, these will be eagerly used when present. So all
        //   key/values will be assume to be .data field and passed into these
        //   functions unconditionally.
        dict_free_fn_t keyFreeFn;
        dict_free_fn_t valFreeFn;
};

struct dict_table {
        // all fields are considered for internal usages. See public dict APIs.

        struct dict_entry **table;  // owned.
        unsigned long       size;
        unsigned long       sizemask;
        unsigned long       used;
};

struct dict {
        // all fields are considered for internal usages. See public APIs.

        int owned_dict_type;     // default: 0 (unowned). If 1, call free on it
        struct dict_type *type;  // ownership depends on 'owned_type'.
        void             *privdata;  // unowned.
        struct dict_table ht;
};

// =============================================================================
// public low-level apis
// =============================================================================

// create and free a dict with dict type (unowned) and privdata (unowned)
extern struct dict *dictNew( struct dict_type *type, void *privdata );
extern void         dictFree( struct dict *d );

// init and de-init the dict in-place with dict type (unowned) and privdata
// (unowned)
extern void dictInit( struct dict *d, struct dict_type *type, void *privdata );
extern void dictDeinit( struct dict *d );

// expand the dict to new size (could be larger).
extern error_t dictExpand( struct dict *d, unsigned long size );

// return 1 if deleted, 0 cannot find.
extern int dictDelete( struct dict *ht, const union value key );

// dictFind             return the entry for the key or NULL.
// dictAddOrFind        set existed state and returns the entry.
// dictAdd              require key must be new.
// dictReplace          add an element, discarding the old value if the key
//                      already exists.  Return 1 if the key was added from
//                      scratch, 0 if there was already an element with such
//                      key.
extern struct dict_entry *dictFind( struct dict *d, const union value key );
extern struct dict_entry *dictAddOrFind( struct dict *d, const union value key,
                                         _mut_ int *existed );
extern error_t            dictAdd( struct dict *d, const union value key,
                                   union value val );
extern int                dictReplace( struct dict *d, const union value key,
                                       union value val );

// low-level api for find.
//
// This api will auto expand table if needed.
//
// return values:
//
//   - if key already exists, NULL is returned, and "*existing" is populated
//     with the existing entry if existing is not NULL.
//   - if key was added, the new entry is returned to be manipulated by the
//     caller.
extern struct dict_entry *dictAddRaw( struct dict *d, const union value key,
                                      _out_ struct dict_entry **existing );

// -----------------------------------------------------------------------------
// public macros
// -----------------------------------------------------------------------------
#define dictGetKey( entry )  ( ( entry )->key )
#define dictGetData( entry ) ( ( entry )->v.data )
#define dictGetI64( entry )  ( ( entry )->v.i64 )
#define dictGetU64( entry )  ( ( entry )->v.u64 )
#define dictGetF64( entry )  ( ( entry )->v.f64 )
#define dictGetF32( entry )  ( ( entry )->v.f32 )

#define dictSetKey( d, entry, _union_key_ ) \
        _DICT_SET_KEY( d, entry, _union_key_ )
#define dictSetValue( d, entry, _union_val_ ) \
        _DICT_SET_VALUE( d, entry, _union_val_ )
#define dictSetData( d, entry, _val_ ) _DICT_SET_DATA( d, entry, _val_ )
#define dictSetI64( entry, _val_ )     _DICT_SET_I64( entry, _val_ )
#define dictSetU64( entry, _val_ )     _DICT_SET_U64( entry, _val_ )
#define dictSetF64( entry, _val_ )     _DICT_SET_F64( entry, _val_ )
#define dictSetF32( entry, _val_ )     _DICT_SET_F32( entry, _val_ )

#define dictFreeKey( d, entry )   _DICT_FREE_KEY( d, entry )
#define dictFreeValue( d, entry ) _DICT_FREE_VALUE( d, entry )

#define dictCmpKeys( d, union_key1, union_key2 ) \
        _DICT_CMP_KEYS( d, union_key1, union_key2 )

#define dictHashKey( d, union_key ) ( ( d )->type->hashFn( union_key ) )

// =============================================================================
// (internal) macro impl
// =============================================================================

// Set the new value and free the old one. Note that it is important
// to do that in this order demonstrated in the next few macros, as the value
// may just be exactly the same as the previous one. In this context, think to
// reference counting, you want to increment (set), and then decrement (free),
// and not the reverse.
#define _DICT_SET_KEY( d, entry, _union_key_ )                                 \
        do {                                                                   \
                void *prev_data = NULL;                                        \
                if ( ( d )->type->keyFreeFn && ( entry )->key.data != NULL ) { \
                        prev_data = ( entry )->key.data;                       \
                }                                                              \
                if ( ( d )->type->keyDupFn ) {                                 \
                        ( entry )->key.data = ( d )->type->keyDupFn(           \
                            ( d )->privdata, ( _union_key_ ).data );           \
                } else {                                                       \
                        ( entry )->key = ( _union_key_ );                      \
                }                                                              \
                if ( prev_data != NULL ) {                                     \
                        ( d )->type->keyFreeFn( ( d )->privdata, prev_data );  \
                }                                                              \
        } while ( 0 )

#define _DICT_SET_VALUE( d, entry, _union_val_ )                              \
        do {                                                                  \
                void *prev_data = NULL;                                       \
                if ( ( d )->type->valFreeFn && ( entry )->v.data != NULL ) {  \
                        prev_data = ( entry )->v.data;                        \
                }                                                             \
                if ( ( d )->type->valDupFn ) {                                \
                        ( entry )->v.data = ( d )->type->valDupFn(            \
                            ( d )->privdata, ( _union_val_ ).data );          \
                } else {                                                      \
                        ( entry )->v = ( _union_val_ );                       \
                }                                                             \
                if ( prev_data != NULL ) {                                    \
                        ( d )->type->valFreeFn( ( d )->privdata, prev_data ); \
                }                                                             \
        } while ( 0 )

#define _DICT_SET_DATA( d, entry, _val_ )                                     \
        do {                                                                  \
                void *prev_data = NULL;                                       \
                if ( ( d )->type->valFreeFn && ( entry )->v.data != NULL ) {  \
                        prev_data = ( entry )->v.data;                        \
                }                                                             \
                if ( ( d )->type->valDupFn )                                  \
                        ( entry )->v.data = ( d )->type->valDupFn(            \
                            ( d )->privdata, ( _val_ ) );                     \
                else {                                                        \
                        ( entry )->v.data = ( _val_ );                        \
                }                                                             \
                if ( prev_data != NULL ) {                                    \
                        ( d )->type->valFreeFn( ( d )->privdata, prev_data ); \
                }                                                             \
        } while ( 0 )

#define _DICT_SET_I64( entry, _val_ )         \
        do {                                  \
                ( entry )->v.i64 = ( _val_ ); \
        } while ( 0 )

#define _DICT_SET_U64( entry, _val_ )         \
        do {                                  \
                ( entry )->v.u64 = ( _val_ ); \
        } while ( 0 )

#define _DICT_SET_F64( entry, _val_ )         \
        do {                                  \
                ( entry )->v.f64 = ( _val_ ); \
        } while ( 0 )

#define _DICT_SET_F32( entry, _val_ )         \
        do {                                  \
                ( entry )->v.f32 = ( _val_ ); \
        } while ( 0 )

#define _DICT_FREE_KEY( d, entry )    \
        if ( ( d )->type->keyFreeFn ) \
        ( d )->type->keyFreeFn( ( d )->privdata, ( entry )->key.data )

#define _DICT_FREE_VALUE( d, entry )  \
        if ( ( d )->type->valFreeFn ) \
        ( d )->type->valFreeFn( ( d )->privdata, ( entry )->v.data )

#define _DICT_CMP_KEYS( d, key1, key2 ) \
        ( ( d )->type->keyCmpFn( ( d )->privdata, key1, key2 ) )

#endif  // EVA_DICT_H_
