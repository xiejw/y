#include "sched/cron.h"

#include <assert.h>

typedef struct tm tm_t;

enum cron_comp {
        cron_minute,
        cron_hour,
        cron_day,
        cron_day_of_week,
        cron_month,
        cron_final_component_marker
};

// -----------------------------------------------------------------------------
// helper methods prototypes.
// -----------------------------------------------------------------------------

static void rewind(tm_t *candidate, enum cron_comp up_to);
static error_t validate(tm_t *candidate, tm_t *start_time);
static int value(tm_t *time_tm, enum cron_comp comp);
static void increase(tm_t *time_tm, enum cron_comp comp);
static error_t searchNextMatching(tm_t *start_time, struct cron_field *field,
                                  enum cron_comp comp, tm_t *candidate,
                                  int *changed);

// -----------------------------------------------------------------------------
// implementation.
// -----------------------------------------------------------------------------

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-enum"

int
cronFieldMatch(struct cron_field *field, int v)
{
        if (field->kind == CRON_FIELD_KIND_ANY) return 1;
        assert(field->kind == CRON_FIELD_KIND_SINGLE_VALUE);
        return field->value == v;
}

void
cronExprInit(struct cron_expr *expr)
{
        assert(expr != NULL);
        cronFieldSetAsAny(&expr->minute);
        cronFieldSetAsAny(&expr->hour);
        cronFieldSetAsAny(&expr->day);
        cronFieldSetAsAny(&expr->month);
        cronFieldSetAsAny(&expr->day_of_week);
}

int
cronExprMatch(struct cron_expr *expr, time_t time)
{
        tm_t time_tm;
        localtime_r(&time, &time_tm);

        if (!cronFieldMatch(&expr->minute, time_tm.tm_min)) return 0;
        if (!cronFieldMatch(&expr->hour, time_tm.tm_hour)) return 0;
        if (!cronFieldMatch(&expr->day, time_tm.tm_mday)) return 0;
        // tm_wday starts with 0 (Sunday).
        if (!cronFieldMatch(&expr->day_of_week, time_tm.tm_wday)) return 0;
        // tm_mon starts with zero.
        if (!cronFieldMatch(&expr->month, time_tm.tm_mon + 1)) return 0;

        return 1;
}

// Algorithm to find next time matching the `expr`
//
// Step 1:   Go to next minute and reset the second field.
// Step 2    (mainLoop:) : Search minute until match.
// Step 3:   Search hour until match. If hour changed, reset lower components
//           (minute in this case) and jump to `mainLoop`.
// Step 4:   Search day until match. If day changed, reset lower components
//           (hour and minute in this case) and jump to `mainLoop`.
// Step 5-6: Search month and year. Same rule as step 3 and 4.
error_t
cronExprNext(struct cron_expr *expr, time_t start_time, time_t *next_time)
{
        tm_t candidate;
        localtime_r(&start_time, &candidate);
        tm_t start_point = candidate;  // make a copy.

        // Advance the minute once to start the search and reset the second
        // field.
        candidate.tm_min += 1;
        candidate.tm_sec = 0;

mainLoop:
        while (1) {
                for (enum cron_comp comp = cron_minute;
                     comp < cron_final_component_marker; comp++) {
                        struct cron_field *field = NULL;
                        switch (comp) {
                        case cron_minute:
                                field = &expr->minute;
                                break;
                        case cron_hour:
                                field = &expr->hour;
                                break;
                        case cron_day:
                                field = &expr->day;
                                break;
                        case cron_day_of_week:
                                field = &expr->day_of_week;
                                break;
                        case cron_month:
                                field = &expr->month;
                                break;
                        default:
                                errFatalAndExit("unexpected component: %d",
                                                comp);
                        }

                        int changed;
                        if (searchNextMatching(/*start_time=*/&start_point,
                                               /*field=*/field,
                                               /*comp=*/comp,
                                               /*candidate=*/&candidate,
                                               /*changed=*/&changed))
                                return ERROR;

                        if (changed) goto mainLoop;
                }
                break;
        }

        *next_time = mktime(&candidate);
        return OK;
}

// -----------------------------------------------------------------------------
// helper methods implementation.
// -----------------------------------------------------------------------------

// Rewinds all components in the candidate up to `up_to`. For example, if
// `up_to` is `day`, then `hour`, `minute` are rewinded to `zero`.
void
rewind(tm_t *candidate, enum cron_comp up_to)
{
        switch (up_to) {
        case cron_month:
                candidate->tm_mday = 1;
                __attribute__((fallthrough));
        case cron_day_of_week:
                __attribute__((fallthrough));
        case cron_day:
                candidate->tm_hour = 0;
                __attribute__((fallthrough));
        case cron_hour:
                candidate->tm_min = 0;
                break;
        case cron_minute:
                // no-op
                break;
        default:
                errFatalAndExit("unexpected component: %d", up_to);
        }
}

// Validates the candidate. We only search up to 5 years.
error_t
validate(tm_t *candidate, tm_t *start_time)
{
        return candidate->tm_year - start_time->tm_year > 5;
}

// Retrieves the component from the time.
int
value(tm_t *time_tm, enum cron_comp comp)
{
        switch (comp) {
        case cron_minute:
                return time_tm->tm_min;
        case cron_hour:
                return time_tm->tm_hour;
        case cron_day:
                return time_tm->tm_mday;
        case cron_day_of_week:
                return time_tm->tm_wday;
        case cron_month:
                // Month starts with 0 in `tm_t`.
                return time_tm->tm_mon + 1;
        default:
                errFatalAndExit("unexpected component: %d", comp);
                return 0;
        }
}

// Increases the component of `time_tm` by 1. Adjusted all other components
// accordingly.
//
// For example: if the minute reaches 60, minute will be reset as 0 and hour
// will be increaesed.
void
increase(tm_t *time_tm, enum cron_comp comp)
{
        switch (comp) {
        case cron_minute:
                time_tm->tm_min += 1;
                break;
        case cron_hour:
                time_tm->tm_hour += 1;
                break;
        case cron_day:
                time_tm->tm_mday += 1;
                break;
        case cron_day_of_week:
                time_tm->tm_mday += 1;
                break;
        case cron_month:
                time_tm->tm_mon += 1;
                break;
        default:
                errFatalAndExit("unexpected component: %d", comp);
        }
        mktime(time_tm);
}

// Focusing only on the comp inside the `candidate`, searching the next
// value, by keeping increasing it, until matching the `field` of the
// expression.
error_t
searchNextMatching(tm_t *start_time, struct cron_field *field,
                   enum cron_comp comp, tm_t *candidate, int *changed)
{
        int initial_value = value(candidate, comp);
        if (cronFieldMatch(field, initial_value)) {
                *changed = 0;
                return OK;
        }

        *changed = 1;
        rewind(candidate, comp);

        while (1) {
                increase(candidate, comp);
                if (validate(candidate, start_time)) {
                        return ERROR;
                }
                int new_value = value(candidate, comp);

                if (cronFieldMatch(field, new_value)) {
                        return OK;
                }
        }
}

#pragma clang diagnostic pop
