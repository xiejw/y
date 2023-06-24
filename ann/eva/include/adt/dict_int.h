// copyright: see license_antirez-bsd file.

#ifndef EVA_DICT_INT_H_
#define EVA_DICT_INT_H_

#include <adt/dict.h>

// =============================================================================
// public high-level apis for int64 (dict.c)
//
// key is i64_t. It is using the same hash, cmp and dup function. And users
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
extern void dictI64Init(struct dict *d, _null_ void *privdata,
                        _null_ dict_dup_fn_t val_dup_fn,
                        _null_ dict_free_fn_t val_free_fn);
extern void dictI64Deinit(struct dict *d);

// For the following apis
//
// - NULL means no entry.
// - data cannot be NULL and return its alias (no ownership transfer)
// - take value from entry immediately, don't hold entry
extern struct dict_entry *dictI64Get(struct dict *d, i64_t key);
extern void *dictI64GetData(struct dict *d, i64_t key);

// For the following apis
//
// - data cannot be null
// - old value will be dropped if val_free_fn is provided.
// - new value will be created if val_dup_fn is provided.
extern void dictI64Put(struct dict *d, i64_t key, union value val);
extern void dictI64PutData(struct dict *d, i64_t key, void *data);
extern void dictI64Del(struct dict *d, i64_t key);

#endif  // EVA_DICT_INT_H_
