//
// Created by GRHRehabTech on 2025-07-30.
//
#include "main.h"
#include "BootLoaderScreen.h"

#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

#include "TinyEngineUI.h"
#include "blockdevice/sd.h"
#include "filesystem/fat.h"
#include "filesystem/vfs.h"
#include "hardware/watchdog.h"

// Maximum number of directory entries
#define MAX_ENTRIES 128

// Vector and RAM offset
#if PICO_RP2040
#define VTOR_OFFSET M0PLUS_VTOR_OFFSET
#define MAX_RAM 0x20040000
#elif PICO_RP2350
#define VTOR_OFFSET M33_VTOR_OFFSET
#define MAX_RAM 0x20080000
#endif

std::vector<TinyEngineUIText> files;

// Data structure for directory entries
typedef struct {
    char name[256];
    int is_dir; // 1 if directory, 0 if file
    off_t file_size; // Size of the file in bytes
} dir_entry_t;

// Global variables for UI state
static char current_path[512] = "/sd"; // Current directory path
static dir_entry_t entries[MAX_ENTRIES]; // Directory entries
static int entry_count = 0; // Number of entries in the current directory
static int selected_index = 0; // Currently selected entry index
static char status_message[256] = ""; // Status message
static uint32_t status_timestamp = 0; // Timestamp for status message
// static final_selection_callback_t final_callback = NULL; // Callback for file selection

/**
 * Format file size into human-readable string
 * Converts raw byte count to KB or MB with appropriate suffix
 */
static void format_file_size(off_t size, int is_dir, char *buf, size_t buf_size) {
    if (is_dir) {
        snprintf(buf, buf_size, "DIR");
    } else if (size >= 1024 * 1024) {
        double mb = size / (1024.0 * 1024.0);
        snprintf(buf, buf_size, "%.1fMB", mb);
    } else {
        int kb = size / 1024;
        if (kb < 1)
            kb = 1;
        snprintf(buf, buf_size, "%dKB", kb);
    }
}


// Load directory entries into the global entries array
static void load_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        entry_count = 0;
        return;
    }
    entry_count = 0;
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL && entry_count < MAX_ENTRIES) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        strncpy(entries[entry_count].name, ent->d_name, sizeof(entries[entry_count].name) - 1);
        entries[entry_count].name[sizeof(entries[entry_count].name) - 1] = '\0';
        printf("%s\n", entries[entry_count].name);
        // Build full path for stat
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, ent->d_name);
        files.emplace_back(TinyEngineUIText(0, 10 * entry_count, full_path, 15));
        // Determine if the entry is a directory and get file size
        if (ent->d_type != DT_UNKNOWN) {
            entries[entry_count].is_dir = (ent->d_type == DT_DIR) ? 1 : 0;

            // Get file size using stat even if we know the type from d_type
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0) {
                entries[entry_count].file_size = entries[entry_count].is_dir ? 0 : statbuf.st_size;
            } else {
                entries[entry_count].file_size = 0;
            }
        } else {
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0) {
                entries[entry_count].is_dir = S_ISDIR(statbuf.st_mode) ? 1 : 0;
                entries[entry_count].file_size = entries[entry_count].is_dir ? 0 : statbuf.st_size;
            } else {
                entries[entry_count].is_dir = 0;
                entries[entry_count].file_size = 0;
            }
        }
        entry_count++;
    }
    closedir(dir);
    selected_index = 0;
}

bool fs_init(void) {
    telog("fs init SD\n");
    blockdevice_t *sd = blockdevice_sd_create(spi0,
                                              SD_MOSI_PIN,
                                              SD_MISO_PIN,
                                              SD_SCLK_PIN,
                                              SD_CS_PIN,
                                              125000000 / 2 / 4, // 15.6MHz
                                              true);
    filesystem_t *fat = filesystem_fat_create();
    int err = fs_mount("/sd", fat, sd);
    if (err == -1) {
        telog("format /sd\n");
        err = fs_format(fat, sd);
        if (err == -1) {
            telog("format err: %s\n", strerror(errno));
            return false;
        }
        err = fs_mount("/sd", fat, sd);
        if (err == -1) {
            telog("mount err: %s\n", strerror(errno));
            return false;
        }
    }
    return true;
}

BootLoaderScreen::~BootLoaderScreen() {
}


uint8_t entry_curr = 0, loading = 0;

void BootLoaderScreen::create() {
    GameScene::create();

    // Initialize filesystem
    if (!fs_init()) {
        // text_directory_ui_set_status("Failed to mount SD card!");
        telog("Failed to mount SD card\n");
        sleep_ms(2000);
        watchdog_reboot(0, 0, 0);
    }

    strncpy(current_path, "/sd", sizeof(current_path));
    load_directory(current_path);
    // load_directory("/sd/FW");

    m_engine.bind_serial_input_event('w', [&] {
        if (entry_curr > 0) entry_curr--;
    });

    // m_engine.bind_serial_input_event('w', [&] {
    //     if (entry_num > 0) entry_num--;
    // });
    //
    // m_engine.bind_serial_input_event('w', [&] {
    //     if (entry_num > 0) entry_num--;
    // });

    m_engine.bind_serial_input_event('s', [&] {
        entry_curr++;
        if (entry_curr > entry_count) entry_curr = 0;
    });

    m_engine.bind_serial_input_event('\n', [&] {
        // if (entry_num > 0) entry_num--;
        if (!loading)
            loading = 1;
    });

    // // Prepare filename with directory indicator
    // char full_file_name[300];
    // snprintf(full_file_name, sizeof(full_file_name), "%s%s",
    //         entries[entry_idx].name,
    //         entries[entry_idx].is_dir ? "/" : "");
}

void BootLoaderScreen::render() {
    GameScene::render();

    m_framebuffer.clear(0);

    int i = 0;
    for (auto file: files) {
        if (i == entry_curr)
            m_framebuffer.draw_filled_rectangle(file.get_m_x(), file.get_m_y(), file.get_text().length() * (8 + 2), 12,
                                                44);
        file.render(m_framebuffer);
        i++;
    }

    if (loading) {
        m_framebuffer.draw_filled_rectangle(100, 100, 100, 12, 44);
        m_framebuffer.draw_string("Loading...", 100, 7100, 15);
    }

    // m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

void BootLoaderScreen::update(double frameTime) {
    GameScene::update(frameTime);
}

void BootLoaderScreen::destroy() {
    GameScene::destroy();
}
