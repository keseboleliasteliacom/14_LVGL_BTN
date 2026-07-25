#ifndef TIME_FORMAT_H
#define TIME_FORMAT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Formats a duration as compact, human-readable text.
 *
 * Examples:
 * - 14 seconds     -> "0m 14s"
 * - 7334 seconds   -> "2h 2m 14s"
 * - 199860 seconds -> "2d 7h 31m"
 *
 * @param buffer Destination buffer.
 * @param buffer_size Size of destination buffer.
 * @param total_seconds Duration to format.
 *
 * @return Number of characters that would have been written, matching
 *         snprintf semantics, or a negative value for invalid arguments.
 */
int TimeFormat_FormatDuration(
    char *buffer,
    size_t buffer_size,
    uint64_t total_seconds);

#ifdef __cplusplus
}
#endif

#endif