/**
 * @file progress.h
 * @brief Terminal progress bar display for file transfers
 *
 * Provides a callback-based progress bar system that displays transfer
 * progress, speed, and ETA. The progress bar is automatically throttled
 * to avoid excessive terminal updates.
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/** Visual representation of progress bar using hash characters */
#define BARS "########################################"

/** Width of the progress bar in terminal columns */
#define PROGRESS_BAR_WIDTH  40

/** Update threshold in bytes - progress bar updates every 500KB */
#define UPDATE_INTERVAL    (500*1024)

#define SIZE_KB  1024                /**< Bytes in a kilobyte */
#define SIZE_MB (1024*1024)          /**< Bytes in a megabyte */
#define SIZE_GB (1024*1024*1024)     /**< Bytes in a gigabyte */

/**
 * Progress callback function signature
 *
 * Callback function invoked during file transfer to update progress display.
 * Implementations should handle throttling to avoid excessive updates.
 *
 * @param current Current number of bytes transferred
 * @param total Total number of bytes to transfer
 */
typedef void (*progress_bar_func)(size_t current, size_t total);

/**
 * Global progress callback pointer
 *
 * Set by `start_progress_bar()` to enable progress updates.
 * Called from file transfer routines (e.g., `file_send()`).
 * Set to `NULL` to disable progress updates.
 */
extern progress_bar_func progress_bar_callback;

/**
 * Transfer start time
 *
 * Recorded when `start_progress_bar()` is called.
 * Used to calculate overall transfer speed and elapsed time.
 */
extern struct timespec start_time;

/**
 * Last update timestamp
 *
 * Tracks when the progress bar was last rendered.
 * Used to throttle updates to avoid terminal flicker (0.2s minimum interval).
 */
extern struct timespec last_update;

/**
 * Initialize and start the progress bar
 *
 * Records the start time for speed calculations and sets up the
 * callback function for progress updates. This should be called
 * before starting the file transfer.
 */
void start_progress_bar(void);

/**
 * Finalize the progress bar and show summary
 *
 * Completes the progress bar (showing 100%), calculates the total elapsed time
 * and average transfer speed, and displays a summary of the completed transfer.
 * Also disables the progress callback to prevent further updates.
 *
 * @param total Total number of bytes transferred
 */
void stop_progress_bar(size_t total);

