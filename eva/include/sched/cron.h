#ifndef EVA_CRON_H_
#define EVA_CRON_H_

#include <time.h>

// eva
#include <base/types.h>

// -----------------------------------------------------------------------------
// public data structure and apis for cron
//
// Simple implementation for cron expression.
//
// Example code:
//
//    struct cron_expr expr;
//    cronExprInit(&expr);
//
//    expr.minute = 2
//
//    time_t current_time = time(NULL);
//    time_t next_time;
//
//    if (cronExprNext(&expr, current_time, &next_time)) exit(1);
//    // seconds to next '* 2 * * *'
//    printf("%ld", (long)(next_time - current_time));
//    return 0;
// -----------------------------------------------------------------------------

// field related
#define CRON_FIELD_KIND_ANY          0
#define CRON_FIELD_KIND_SINGLE_VALUE 1

struct cron_field {
        int kind;  // see CRON_FIELD_KIND_XXX above
        int value;
};

struct cron_expr {
        struct cron_field minute;
        struct cron_field hour;
        struct cron_field day;
        struct cron_field month;
        struct cron_field day_of_week;
};

extern void cronExprInit(struct cron_expr *expr);
extern error_t cronExprNext(struct cron_expr *expr, time_t start_time,
                            _mut_ time_t *next_time);
extern int cronExprMatch(struct cron_expr *expr, time_t time);

int cronFieldMatch(struct cron_field *field, int v);

// -----------------------------------------------------------------------------
// inline helper methods.
// -----------------------------------------------------------------------------
static inline void
cronFieldSetAsAny(struct cron_field *p)
{
        p->kind = CRON_FIELD_KIND_ANY;
}

static inline void
cronFieldSetSingleValue(struct cron_field *p, int v)
{
        p->kind  = CRON_FIELD_KIND_SINGLE_VALUE;
        p->value = v;
}

#endif  // EVA_CRON_H_
