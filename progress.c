#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "progress.h"
#include "file.h"

progress_bar_func progress_bar_callback = NULL;

struct timespec start_time = {0};
struct timespec last_update = {0};

static void human_readable_size(char *buf, size_t size, uint64_t bytes);
static void calculate_speed(char *buf, size_t size, uint64_t bytes, double elapsed);
static void calculate_eta(char *buf, size_t size, double total_elapsed, int percentage);
static double get_elapsed_time(struct timespec start, struct timespec end);
static void render_progress_bar(int bars, int percentage, const char *sent_str,
                                const char *total_str, const char *speed_str,
                                const char *eta_str);
static void print_progress(uint64_t sent, uint64_t total, int force_complete);
static void update_progress_bar(uint64_t current, uint64_t total);

void start_progress_bar(void)
{
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    last_update = start_time;
    progress_bar_callback = update_progress_bar;
}

/**
 * Update progress bar at regular intervals
 *
 * Updates the progress bar display, but only at certain intervals to avoid
 * excessive updates that could slow down the transfer. Updates occur every
 * `UPDATE_INTERVAL` bytes (e.g., every 500KB).
 *
 * @param current Current number of bytes transferred
 * @param total   Total number of bytes to transfer
 */
static void update_progress_bar(uint64_t current, uint64_t total)
{
    if (current % UPDATE_INTERVAL < CHUNK_SIZE) {
        print_progress(current, total, current >= total);
    }
}

void stop_progress_bar(uint64_t total)
{
    double elapsed;
    struct timespec end_time;
    char speed_str[32];

    /* Print the progress bar with "100%" */
    print_progress(total, total, 1);

    /* Print final stats */
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    elapsed = get_elapsed_time(start_time, end_time);

    calculate_speed(speed_str, sizeof(speed_str), total, elapsed);

    printf("\nFile sent successfully! Completed in %.2f seconds (%s avg)\n",
           elapsed, speed_str);

    progress_bar_callback = NULL;
}

/**
 * Convert byte count to human-readable string
 *
 * Converts a byte count into a human-readable string with appropriate
 * units (bytes, KB, MB, GB). The result is formatted with two decimal
 * places for units larger than bytes.
 *
 * @param buf   Buffer to store the formatted string
 * @param size  Size of the buffer
 * @param bytes Number of bytes to format
 */
static void human_readable_size(char *buf, size_t size, uint64_t bytes)
{
    if (bytes >= SIZE_GB) {
        snprintf(buf, size, "%.2f GB", bytes / (double)SIZE_GB);
    } else if (bytes >= SIZE_MB) {
        snprintf(buf, size, "%.2f MB", bytes / (double)SIZE_MB);
    } else if (bytes >= SIZE_KB) {
        snprintf(buf, size, "%.2f KB", bytes / (double)SIZE_KB);
    } else {
        snprintf(buf, size, "%" PRIu64 " bytes", bytes);
    }
}

/**
 * Calculate and format transfer speed
 *
 * Calculates the transfer speed (bytes/second) and formats it as a
 * human-readable string with appropriate units and "/s" suffix.
 *
 * @param buf     Buffer to store the formatted string
 * @param size    Size of the buffer
 * @param bytes   Number of bytes transferred
 * @param elapsed Time elapsed in seconds
 */
static void calculate_speed(char *buf, size_t size, uint64_t bytes, double elapsed)
{
    double speed = bytes / elapsed;
    human_readable_size(buf, size, (uint64_t)speed);
    strcat(buf, "/s");
}

/**
 * Calculate time difference between two timespec values
 *
 * @param start Start time
 * @param end   End time
 *
 * @return Time difference in seconds as a double
 */
static double get_elapsed_time(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

/**
 * Format estimated time remaining
 *
 * @param buf           Buffer to store the formatted string
 * @param size          Size of the buffer
 * @param total_elapsed Total elapsed time so far
 * @param percentage    Current progress percentage
 */
static void calculate_eta(char *buf, size_t size, double total_elapsed, int percentage)
{
    if (percentage > 0) {
        double eta = (total_elapsed / percentage) * (100 - percentage);
        snprintf(buf, size, " - ETA: %.1f s", eta);
    } else {
        buf[0] = '\0';
    }
}

/**
 * Format and print the progress bar
 *
 * @param bars       Number of filled progress bar characters
 * @param percentage Progress percentage
 * @param sent_str   Formatted string of bytes sent
 * @param total_str  Formatted string of total bytes
 * @param speed_str  Formatted string of transfer speed
 * @param eta_str    Formatted string of estimated time remaining
 */
static void render_progress_bar(int bars, int percentage, const char *sent_str,
                                const char *total_str, const char *speed_str,
                                const char *eta_str)
{
    printf("\r\033[K[%.*s%*s] %d%%",
           bars, BARS, PROGRESS_BAR_WIDTH - bars, "", percentage);
    printf(" (%s / %s) %s %s",
           sent_str, total_str, speed_str, eta_str);
    fflush(stdout);
}

/**
 * Display progress bar and transfer statistics
 *
 * Prints a progress bar with percentage, transfer amount, speed, and ETA.
 * Updates are rate-limited to avoid excessive screen updates.
 * If `force_complete` is set, displays 100% completion regardless
 * of sent/total.
 *
 * @param sent           Number of bytes sent so far
 * @param total          Total number of bytes to send
 * @param force_complete Flag to force display of completed progress (100%)
 */
static void print_progress(uint64_t sent, uint64_t total, int force_complete)
{
    struct timespec now = {0, 0};
    double elapsed, total_elapsed;
    int percentage, bars;
    char sent_str[32], total_str[32], speed_str[32], eta_str[32] = "";

    clock_gettime(CLOCK_MONOTONIC, &now);
    
    elapsed = get_elapsed_time(last_update, now);
    if (!force_complete && elapsed < 0.2) {
        return;
    }
    last_update = now;

    if (start_time.tv_sec == 0 && start_time.tv_nsec == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }

    total_elapsed = get_elapsed_time(start_time, now);
    percentage = force_complete ? 100 : (sent * 100) / total;
    bars = (percentage * PROGRESS_BAR_WIDTH) / 100;

    human_readable_size(sent_str, sizeof(sent_str), sent);
    human_readable_size(total_str, sizeof(total_str), total);
    calculate_speed(speed_str, sizeof(speed_str), sent, total_elapsed);
    
    if (percentage > 0 && !force_complete) {
        calculate_eta(eta_str, sizeof(eta_str), total_elapsed, percentage);
    }

    render_progress_bar(bars, percentage, sent_str, total_str, speed_str, eta_str);
}
