#include "adt/vec.h"

#include <assert.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// implementation.
// -----------------------------------------------------------------------------

error_t
_vecReserve(size_t **vec, size_t new_cap, size_t unit_size)
{
#ifdef EVA_VEC_CHECK_MAGIC
        const size_t head_size = 3;
#else
        const size_t head_size = 2;
#endif

        const size_t new_s = (new_cap)*unit_size + head_size * sizeof(size_t);

        if (*vec) {
                _VEC_CHECK_MAGIC(*vec);

                size_t *new_p = realloc(&(*vec)[-(ssize_t)head_size], new_s);
                if (new_p == NULL) return errMalloc();
                new_p[head_size - 1] = new_cap;  // other head fields unchanged.
                *vec                 = (void *)(new_p + head_size);
        } else {
                size_t *p = malloc(new_s);
                if (p == NULL) return errMalloc();
#ifdef EVA_VEC_CHECK_MAGIC
                p[head_size - 3] = _VEC_MAGIC_NUM;
#endif
                p[head_size - 2] = 0;
                p[head_size - 1] = new_cap;
                *vec             = (void *)(p + head_size);
        }
        return OK;
}
