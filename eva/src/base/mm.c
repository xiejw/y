#include "base/mm.h"

// eva
#include <base/bitops.h>

// -----------------------------------------------------------------------------
// real data structure
// -----------------------------------------------------------------------------
struct arr_stack {
        struct arr arr;
        size_t top;  // point to next slot id
};

struct arr_bitmap {
        struct arr arr;
        unsigned long bits[];  // statically allocated bits for bitmap
};

#define ARR_STACK_MODE 0
// #define ARR_BITMAP_MODE 1

// -----------------------------------------------------------------------------
// impl for common apis
// -----------------------------------------------------------------------------
void
arrFree(struct arr *p)
{
        if (p == NULL) return;

        vec_t(void *) lines = p->lines;
        if (lines != NULL) {
                size_t count = vecSize(lines);
                for (size_t i = 0; i < count; i++) {
                        free(lines[i]);
                }
                vecFree(lines);
        }
        free(p);
}

// -----------------------------------------------------------------------------
// impl for stack mode
// -----------------------------------------------------------------------------

struct arr *
arrStackNew(size_t slot_byte_size, size_t suggested_slots_per_line,
            size_t init_num_slots)
{
        // create a stack
        struct arr *arr = malloc(sizeof(struct arr_stack));
        if (arr == NULL) return NULL;

        assert(slot_byte_size > 0);
        assert(suggested_slots_per_line > 0);
        assert(init_num_slots > 0);

        size_t slots_per_line;
        size_t mask;
        long num_bits_to_shift;
        {
                // calculate the best slots_per_line (as 2 powers) along with
                // the mask and bits to shift. So, we can calculate the index
                // on particular line much faster.
                slots_per_line = bitOpsNext2Power(suggested_slots_per_line,
                                                  &num_bits_to_shift);
                if (num_bits_to_shift == -1) {
                        errPanic(
                            "suggested_slots_per_line for stack is too large.");
                        return NULL;
                }
                mask = (1UL << num_bits_to_shift) - 1;
        }

        size_t num_lines;
        vec_t(void *) lines = vecNew();
        {
                // If user asks for 7 as init_num_slots and slots_per_line is
                // 4.  Then we should alloc 2 lines. For init_num_slots is 8,
                // same for 2 lines.
                num_lines =
                    (init_num_slots + slots_per_line - 1) / slots_per_line;
                size_t per_line_size = slots_per_line * slot_byte_size;

                // allocate each line.
                vecReserve(&lines, num_lines);
                vecSetSize(lines, num_lines);
                for (size_t i = 0; i < num_lines; i++) {
                        lines[i] = malloc(per_line_size);
                }
        }

        // fill struct arr
        arr->slot_byte_size = slot_byte_size;
        arr->slots_per_line = slots_per_line;
        arr->lines          = lines;
        arr->cap            = num_lines * slots_per_line;
        arr->mask           = mask;
        arr->sbits = (unsigned int)num_bits_to_shift;  // enough to hold
        arr->mode  = ARR_STACK_MODE;
        arr->flag  = 0;

        // fill struct arr_stack
        ((struct arr_stack *)arr)->top = 0;

        return arr;
}

void *
arrPush(struct arr *raw)
{
        assert(raw->mode == ARR_STACK_MODE);
        struct arr_stack *arr = (struct arr_stack *)raw;

        size_t top = arr->top;
        if (top < raw->cap) {  // common case
                size_t num_lines = top >> raw->sbits;
                size_t index     = top & raw->mask;
                (arr->top)++;
                return (char *)(raw->lines[num_lines]) +
                       index * raw->slot_byte_size;
        }

        // grow one more line
        size_t per_line_size = raw->slots_per_line * raw->slot_byte_size;
        void *new_line       = malloc(per_line_size);
        vecPushBack(&raw->lines, new_line);
        raw->cap += raw->slots_per_line;
        (arr->top)++;
        return new_line;
}

void *
arrPop(struct arr *raw)
{
        // pop has no auto shrink now.

        assert(raw->mode == ARR_STACK_MODE);
        struct arr_stack *arr = (struct arr_stack *)raw;

        if (arr->top == 0) {
                return NULL;
        }

        size_t top       = --(arr->top);
        size_t num_lines = top >> raw->sbits;
        size_t index     = top & raw->mask;
        return (char *)(raw->lines[num_lines]) + index * raw->slot_byte_size;
}

int
arrIsEmpty(struct arr *raw)
{
        assert(raw->mode == ARR_STACK_MODE);
        struct arr_stack *arr = (struct arr_stack *)raw;

        return arr->top == 0;
}
