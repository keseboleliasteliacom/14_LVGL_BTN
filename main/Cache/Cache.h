#ifndef CACHE_H
#define CACHE_H

#include "../Memory/Spiffs.h"

/**
 * @file Cache.h
 * @brief Public API for the CACHE module.
 *
 * Provides a small heap-backed wrapper around SPIFFS JSON file loading and
 * saving.
 *
 * @defgroup CACHE CACHE
 * @brief Cache management helpers for JSON file data.
 *
 * The module stores an in-memory copy of JSON content loaded from SPIFFS and
 * writes JSON data back through the storage layer.
 * @{
 */

/**
 * @brief Cache state container.
 *
 * Holds the current heap-allocated JSON payload managed by the cache module.
 */
typedef struct{
    char* data; /**< Heap-allocated JSON data, or NULL when empty. */
}Cache_t;

/**
 * @brief Initializes a cache instance.
 *
 * @param[in,out] cache Pointer to the cache object to initialize.
 *
 * @return `0` on success, nonzero on failure.
 */
int Cache_Initialize(Cache_t* cache);

/**
 * @brief Writes JSON data to a file.
 *
 * The cache object is accepted as part of the module API but is not modified by
 * this function.
 *
 * @param[in] cache Cache object associated with the write operation; currently not modified.
 * @param[in] data JSON text to write.
 * @param[in] file_name Target file name.
 *
 * @return `0` on success, nonzero on failure.
 */
int Cache_WriteFileJSON(Cache_t* cache, const char* data, const char* file_name);

/**
 * @brief Loads JSON data from a file into the cache.
 *
 * @param[in,out] cache Pointer to the cache object.
 * @param[in] file_name Source file name.
 *
 * @return `0` on success, nonzero on failure.
 */
int Cache_LoadFileJSON(Cache_t* cache, const char* file_name);

/**
 * @brief Releases the cached JSON buffer.
 *
 * Frees the owned buffer and resets `cache->data` to `NULL`. Calling this
 * function on an empty cache is safe.
 *
 * @param[in,out] cache Pointer to the cache object to dispose.
 */
void Cache_Dispose(Cache_t* cache);

/** @} */

#endif
