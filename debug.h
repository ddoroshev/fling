/**
* @file debug.h
* Debug utilities and macros for development builds
*/
#pragma once

#include <stdio.h>

#ifdef DEBUG
/**
 * Debug print macro with file, line, and function information
 * @param fmt Format string (`printf`-style)
 * @param ... Variable arguments for the format string
 *
 * Prints debug information in the format:
 *
 * ``[DEBUG] filename:line:function(): message
 * ``
 *
 * Only active when `DEBUG` is defined at compile time.
 * In release builds, this macro expands to nothing.
 */
# define DPRINT(fmt, ...) \
    printf("[DEBUG] %s:%d:%s(): " fmt "\n", \
        __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
# define DPRINT(x)
#endif
