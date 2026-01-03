// copyright: see license_antirez-bsd file.

#ifndef EVA_DICT_STR_H_
#define EVA_DICT_STR_H_

#include <adt/dict.h>

// =============================================================================
// public high-level apis for str (dict.c)
//
// key is string. It is using the fixed hash, cmp and dup function. And users
// cannot customize this. For high level apis, we optimize for convenience.
//
// value data cannot be NULL. And value data's dup and free fns are set by
// users.
//
// All fns provided will panic if unexpected error happens.
// =============================================================================

// In-place init or deinit.
//
// - val_dup_fn and valDupFn are NULL-able.
extern void dictStrInit( struct dict *d, _null_ void *privdata,
                         _null_ dict_dup_fn_t  val_dup_fn,
                         _null_ dict_free_fn_t val_free_fn );
extern void dictStrDeinit( struct dict *d );

// For the following apis
//
// - NULL means no entry.
// - data cannot be NULL and return its alias (no ownership transfer)
// - take value from entry immediately, don't hold entry
extern struct dict_entry *dictStrGet( struct dict *d, const char *key );
extern void              *dictStrGetData( struct dict *d, const char *key );

// For the following apis.
//
// - data cannot be null
// - old value will be dropped if val_free_fn is provided.
// - new value will be created if val_dup_fn is provided.
extern void dictStrPut( struct dict *d, const char *key, union value val );
extern void dictStrPutData( struct dict *d, const char *key, void *data );
extern void dictStrDel( struct dict *d, const char *key );

#endif  // EVA_DICT_STR_H_
