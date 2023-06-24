// copyright: see license_antirez-bsd file.

#include <adt/dict.h>
#include <adt/dict_int.h>
#include <adt/dict_ptr.h>
#include <adt/dict_str.h>

#include <assert.h>

// ============================================================================
// prototypes for helpers
// ============================================================================
#define DICT_HT_INITIAL_SIZE 128

static unsigned int dict_force_resize_ratio = 5;

static void _dictRehash(struct dict *d, struct dict_table *new_ht);
static error_t _dictExpandIfNeeded(struct dict *d);
static void _dictReset(struct dict_table *ht);
static unsigned long _dictNextPower(unsigned long size);
static error_t _dictExpand(struct dict *d, unsigned long size);
static long _dictKeyIndex(struct dict *d, const union value key, uint64_t hash,
                          struct dict_entry **existing);
static error_t _dictClearTable(struct dict *d, struct dict_table *ht);

static inline void
_dictInit(struct dict *d, struct dict_type *type, void *privDataPtr)
{
        assert(d != NULL);
        assert(type != NULL);
        assert(type->hashFn != NULL);
        assert(type->keyCmpFn != NULL);

        _dictReset(&d->ht);
        d->owned_dict_type = 0;
        d->type            = type;
        d->privdata        = privDataPtr;
}

static inline void
_dictDeinit(struct dict *d)
{
        if (d == NULL) return;
        _dictClearTable(d, &d->ht);
        if (d->owned_dict_type) free(d->type);
}

// ============================================================================
// impl for public high level apis for str
// ============================================================================

void
dictStrInit(struct dict *d, void *privdata, dict_dup_fn_t val_dup_fn,
            dict_free_fn_t val_free_fn)
{
        struct dict_type *type = malloc(sizeof(*type));
        type->hashFn           = hashFnStr;
        type->keyCmpFn         = keyCmpFnStr;
        type->keyDupFn         = dupFnStr;
        type->valDupFn         = val_dup_fn;   // nullable
        type->keyFreeFn        = freeFnStr;
        type->valFreeFn        = val_free_fn;  // nullable

        _dictInit(d, type, privdata);
        d->owned_dict_type = 1;
}

void
dictStrDeinit(struct dict *d)
{
        _dictDeinit(d);
}

struct dict_entry *
dictStrGet(struct dict *d, const char *key)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        return dictFind(d, VDATA(key));
#pragma clang diagnostic pop
}

void *
dictStrGetData(struct dict *d, const char *key)
{
        struct dict_entry *de = dictStrGet(d, key);
        return de == NULL ? NULL : de->v.data;
}

void
dictStrPut(struct dict *d, const char *key, union value val)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictReplace(d, VDATA(key), val);
#pragma clang diagnostic pop
}

void
dictStrPutData(struct dict *d, const char *key, void *data)
{
        assert(data != NULL);
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictReplace(d, VDATA(key), VDATA(data));
#pragma clang diagnostic pop
}

void
dictStrDel(struct dict *d, const char *key)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictDelete(d, VDATA(key));
#pragma clang diagnostic pop
}

// ============================================================================
// impl for public high level apis for ptr
// ============================================================================

void
dictPtrInit(struct dict *d, void *privdata, dict_dup_fn_t val_dup_fn,
            dict_free_fn_t val_free_fn)
{
        struct dict_type *type = malloc(sizeof(*type));
        type->hashFn           = hashFnPtr;
        type->keyCmpFn         = keyCmpFnPtr;
        type->keyDupFn         = NULL;
        type->valDupFn         = val_dup_fn;   // nullable
        type->keyFreeFn        = NULL;
        type->valFreeFn        = val_free_fn;  // nullable

        _dictInit(d, type, privdata);
        d->owned_dict_type = 1;
}

void
dictPtrDeinit(struct dict *d)
{
        _dictDeinit(d);
}

struct dict_entry *
dictPtrGet(struct dict *d, const void *key)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        return dictFind(d, VDATA(key));
#pragma clang diagnostic pop
}

void *
dictPtrGetData(struct dict *d, const void *key)
{
        struct dict_entry *de = dictPtrGet(d, key);
        return de == NULL ? NULL : de->v.data;
}

void
dictPtrPut(struct dict *d, const void *key, union value val)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictReplace(d, VDATA(key), val);
#pragma clang diagnostic pop
}

void
dictPtrPutData(struct dict *d, const void *key, void *data)
{
        assert(data != NULL);
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictReplace(d, VDATA(key), VDATA(data));
#pragma clang diagnostic pop
}

void
dictPtrDel(struct dict *d, const void *key)
{
        // avoid clang error due to dropping const cast
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
        dictDelete(d, VDATA(key));
#pragma clang diagnostic pop
}

// ============================================================================
// impl for public high level apis for int
// ============================================================================

void
dictI64Init(struct dict *d, void *privdata, dict_dup_fn_t val_dup_fn,
            dict_free_fn_t val_free_fn)
{
        struct dict_type *type = malloc(sizeof(*type));
        type->hashFn           = valueHashFnI64;
        type->keyCmpFn         = valueCmpI64;
        type->keyDupFn         = NULL;
        type->valDupFn         = val_dup_fn;   // nullable
        type->keyFreeFn        = NULL;
        type->valFreeFn        = val_free_fn;  // nullable

        _dictInit(d, type, privdata);
        d->owned_dict_type = 1;
}

void
dictI64Deinit(struct dict *d)
{
        _dictDeinit(d);
}

struct dict_entry *
dictI64Get(struct dict *d, i64_t key)
{
        return dictFind(d, VI64(key));
}

void *
dictI64GetData(struct dict *d, i64_t key)
{
        struct dict_entry *de = dictI64Get(d, key);
        return de == NULL ? NULL : de->v.data;
}

void
dictI64Put(struct dict *d, i64_t key, union value val)
{
        dictReplace(d, VI64(key), val);
}

void
dictI64PutData(struct dict *d, i64_t key, void *data)
{
        assert(data != NULL);
        dictReplace(d, VI64(key), VDATA(data));
}

void
dictI64Del(struct dict *d, i64_t key)
{
        dictDelete(d, VI64(key));
}

// ============================================================================
// impl for public low level apis
// ============================================================================

struct dict *
dictNew(struct dict_type *type, void *privDataPtr)
{
        struct dict *d = malloc(sizeof(*d));
        _dictInit(d, type, privDataPtr);
        return d;
}

void
dictFree(struct dict *d)
{
        _dictDeinit(d);
        free(d);
}

void
dictInit(struct dict *d, struct dict_type *type, void *privDataPtr)
{
        _dictInit(d, type, privDataPtr);
}

void
dictDeinit(struct dict *d)
{
        _dictDeinit(d);
}

error_t
dictExpand(struct dict *d, unsigned long size)
{
        return _dictExpand(d, size);
}

error_t
dictDelete(struct dict *d, const union value key)
{
        uint64_t h;
        struct dict_entry *de, *prevde;
        struct dict_table *ht = &d->ht;

        if (ht->size == 0) return 0;  // not found

        h  = dictHashKey(d, key) & ht->sizemask;
        de = ht->table[h];

        prevde = NULL;
        while (de) {
                if (dictCmpKeys(d, key, de->key)) {
                        /* Unlink the element from the list */
                        if (prevde)
                                prevde->next = de->next;
                        else
                                ht->table[h] = de->next;

                        dictFreeKey(d, de);
                        dictFreeValue(d, de);
                        free(de);
                        ht->used--;
                        return 1;  // deleted
                }
                prevde = de;
                de     = de->next;
        }
        return 0;  // not found
}

//  Low level add or find:
//
//  This function adds the entry but instead of setting a value returns the
//  dictEntry structure to the user, that will make sure to fill the value
//  field as he wishes.
//
//  This function is also directly exposed to the user API to be called
//  mainly in order to store non-pointers inside the hash value, example:
//
//      entry = dictAddRaw(dict, mykey, NULL);
//      if (entry != NULL) dictSetI64(entry, 1000);
//
//  Return values:
//
//  If key already exists NULL is returned, and "*existing" is populated
//  with the existing entry if existing is not NULL.
//
//  If key was added, the new entry is returned to be manipulated by the
//  caller.
struct dict_entry *
dictAddRaw(struct dict *d, const union value key, struct dict_entry **existing)
{
        long index;
        struct dict_entry *entry;
        struct dict_table *ht = &d->ht;

        /* Get the index of the new element, or -1 if the element already
         * exists. */
        if ((index = _dictKeyIndex(d, key, dictHashKey(d, key), existing)) ==
            -1) {
                return NULL;
        }

        /* Allocate the memory and store the new entry.
         * Insert the element in top, with the assumption that in a database
         * system it is more likely that recently added entries are accessed
         * more frequently. */
        entry            = malloc(sizeof(*entry));
        entry->next      = ht->table[index];
        ht->table[index] = entry;
        ht->used++;

        // set key
        if (d->type->keyDupFn) {
                entry->key.data = d->type->keyDupFn(d->privdata, key.data);
        } else {
                entry->key = key;
        }

        // clear val to make it safe to use.
        //
        // Many operations/macros operated in entry assumes entry must be valid
        // and might free the data field if present. So setting to NULL solves
        // this.
        entry->v.data = NULL;
        return entry;
}

error_t
dictAdd(struct dict *d, const union value key, union value val)
{
        struct dict_entry *entry = dictAddRaw(d, key, NULL);

        if (!entry) return errNew("entry already existed.");

        if (d->type->valDupFn != NULL) {
                entry->v.data = d->type->valDupFn(d->privdata, val.data);
        } else {
                entry->v = val;
        }
        return OK;
}

// Add or Overwrite:
//
// Add an element, discarding the old value if the key already exists.
// Return 1 if the key was added from scratch, 0 if there was already an
// element with such key and dictReplace() just performed a value update
// operation.
int
dictReplace(struct dict *d, const union value key, union value val)
{
        struct dict_entry *entry, *existing;

        /* Try to add the element. If the key
         * does not exists dictAdd will succeed. */
        entry = dictAddRaw(d, key, &existing);
        if (entry) {
                if (d->type->valDupFn)
                        entry->v.data =
                            d->type->valDupFn(d->privdata, val.data);
                else
                        entry->v = val;
                return 1;
        }

        dictSetValue(d, existing, val);
        return 0;
}

// Add or Find:
//
// dictAddOrFind() is simply a version of dictAddRaw() that always
// returns the hash entry of the specified key, even if the key already
// exists and can't be added (in that case the entry of the already
// existing key is returned.)
//
// See dictAddRaw() for more information.
struct dict_entry *
dictAddOrFind(struct dict *d, const union value key, int *existed)
{
        struct dict_entry *entry, *existing;
        entry = dictAddRaw(d, key, &existing);
        if (existed != NULL) *existed = entry == NULL;
        return entry ? entry : existing;
}

struct dict_entry *
dictFind(struct dict *d, const union value key)
{
        struct dict_entry *he;
        uint64_t h, idx;

        if (d->ht.size == 0) return NULL;

        h   = dictHashKey(d, key);
        idx = h & d->ht.sizemask;
        he  = d->ht.table[idx];
        while (he) {
                if (dictCmpKeys(d, key, he->key)) return he;
                he = he->next;
        }
        return NULL;
}

// ----------------------------------------------------------------------------
// impl for private fns
// ----------------------------------------------------------------------------

void
_dictRehash(struct dict *d, struct dict_table *new_ht)
{
        struct dict_table *ht = &d->ht;
        size_t table_size     = ht->size;

        for (size_t i = 0; i < table_size && ht->used > 0; i++) {
                struct dict_entry *de, *nextde;
                de = d->ht.table[i];
                if (de == NULL) continue;

                // Move all the keys in this bucket from the old to the new hash
                // table, new_ht.
                while (de) {
                        uint64_t h;

                        nextde = de->next;
                        /* Get the index in the new hash table */
                        h        = dictHashKey(d, de->key) & new_ht->sizemask;
                        de->next = new_ht->table[h];
                        new_ht->table[h] = de;
                        new_ht->used++;

                        d->ht.used--;
                        de = nextde;
                }
                d->ht.table[i] = NULL;
        }

        // clean up the old table.
        free(d->ht.table);
        d->ht = *new_ht;
}

// expand the hash table if needed.
static error_t
_dictExpandIfNeeded(struct dict *d)
{
        /* If the hash table is empty expand it to the initial size. */
        if (d->ht.size == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);

        /* If we reached the some ratio, and we are allowed to resize the hash
         * table (global setting) or we should avoid it but the ratio between
         * elements/buckets is over the "safe" threshold, we resize doubling
         * the number of buckets. */
        if (d->ht.used >= d->ht.size &&
            d->ht.used / d->ht.size > dict_force_resize_ratio) {
                return dictExpand(d, d->ht.used + 1);
        }
        return OK;
}

void
_dictReset(struct dict_table *ht)
{
        ht->table    = NULL;
        ht->size     = 0;
        ht->sizemask = 0;
        ht->used     = 0;
}

// our hash table capability is a power of two.
//
// This is an optimization to calculate the slot index.
unsigned long
_dictNextPower(unsigned long size)
{
        unsigned long i = DICT_HT_INITIAL_SIZE;

        if (size >= LONG_MAX) return LONG_MAX + 1LU;
        while (1) {
                if (i >= size) return i;
                i *= 2;
        }
}

error_t
_dictExpand(struct dict *d, unsigned long size)
{
        struct dict_table n; /* the new hash table */
        unsigned long realsize = _dictNextPower(size);

        if (d->ht.used > size) return errNew("expand should cover used items.");
        if (realsize == d->ht.size) return OK;

        /* Allocate the new hash table and initialize all pointers to NULL */
        n.size     = realsize;
        n.sizemask = realsize - 1;
        n.table    = calloc(realsize, sizeof(struct dict_entry *));
        n.used     = 0;

        /* Is this the first initialization? If so it's not really a rehashing
         * we just set the first hash table so that it can accept keys. */
        if (d->ht.table == NULL) {
                d->ht = n;
        } else {
                _dictRehash(d, &n);
        }
        return OK;
}

/* Returns the index of a free slot that can be populated with
 * a hash entry for the given 'key'.
 * If the key already exists, -1 is returned
 * and the optional output parameter may be filled.
 */
long
_dictKeyIndex(struct dict *d, const union value key, uint64_t hash,
              struct dict_entry **existing)
{
        unsigned long idx;
        struct dict_entry *he;
        if (existing) *existing = NULL;

        // expand the hash table if needed
        if (_dictExpandIfNeeded(d))
                return errEmitNote("failed to expand the table.");

        idx = hash & d->ht.sizemask;
        // search if this slot does not already contain the given key
        he = d->ht.table[idx];
        while (he != NULL) {
                if (dictCmpKeys(d, key, he->key)) {
                        if (existing) *existing = he;
                        return -1;
                }
                he = he->next;
        }
        return (long)idx;
}

// destroy an entire dictionary
error_t
_dictClearTable(struct dict *d, struct dict_table *ht)
{
        unsigned long i;

        /* Free all the elements */
        for (i = 0; i < ht->size && ht->used > 0; i++) {
                struct dict_entry *he, *nextHe;

                if ((he = ht->table[i]) == NULL) continue;

                while (he) {
                        nextHe = he->next;
                        dictFreeKey(d, he);
                        dictFreeValue(d, he);
                        free(he);
                        ht->used--;
                        he = nextHe;
                }
        }
        free(ht->table);
        _dictReset(ht);
        return OK; /* never fails */
}
