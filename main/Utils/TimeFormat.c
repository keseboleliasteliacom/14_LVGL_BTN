/**
 * @file TimeFormat.c
 * @brief Implementation of the TimeFormat module.
 *
 * @ingroup TIME_FORMAT
 */

#include "TimeFormat.h"

#include <inttypes.h>
#include <stdio.h>

/**
 * @brief Implementation of TimeFormat_FormatDuration.
 *
 * See header for full contract documentation.
 */
int TimeFormat_FormatDuration(
    char *buffer,
    size_t buffer_size,
    uint64_t total_seconds)
{
    if (buffer == NULL || buffer_size == 0)
    {
        return -1;
    }

    uint64_t days = total_seconds / 86400ULL;
    uint64_t hours = (total_seconds % 86400ULL) / 3600ULL;
    uint64_t minutes = (total_seconds % 3600ULL) / 60ULL;
    uint64_t seconds = total_seconds % 60ULL;

    if (days > 0)
    {
        return snprintf(
            buffer,
            buffer_size,
            "%" PRIu64 "d %" PRIu64 "h %" PRIu64 "m",
            days,
            hours,
            minutes);
    }

    if (hours > 0)
    {
        return snprintf(
            buffer,
            buffer_size,
            "%" PRIu64 "h %" PRIu64 "m %" PRIu64 "s",
            hours,
            minutes,
            seconds);
    }

    return snprintf(
        buffer,
        buffer_size,
        "%" PRIu64 "m %" PRIu64 "s",
        minutes,
        seconds);
}
