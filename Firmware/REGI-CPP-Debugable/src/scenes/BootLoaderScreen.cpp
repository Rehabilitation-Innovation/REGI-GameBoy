//
// Created by GRHRehabTech on 2025-07-30.
//
#include "BootLoaderScreen.h"
#include "tinyengine.h"
#include <cstring>
#include <dirent.h>
#include <hardware/flash.h>
#include <sys/stat.h>

#include "main.h"
#include "TinyEngineUI.h"
#include "blockdevice/sd.h"
#include "filesystem/fat.h"
#include "filesystem/vfs.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"

#include "BootLoaderSplashScreen.h"
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

#define SCROLL_DELAY_MS 5

// std::vector<TinyEngineUIText> files;
// std::vector<TinyEngineUIText> files_types;
// std::vector<TinyEngineUIText> files_sizes;

// Data structure for directory entries
typedef struct
{
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
static void format_file_size(off_t size, int is_dir, char* buf, size_t buf_size)
{
    if (is_dir)
    {
        snprintf(buf, buf_size, "DIR");
    }
    else if (size >= 1024 * 1024)
    {
        double mb = size / (1024.0 * 1024.0);
        snprintf(buf, buf_size, "%.1fMB", mb);
    }
    else
    {
        int kb = size / 1024;
        if (kb < 1)
            kb = 1;
        snprintf(buf, buf_size, "%dKB", kb);
    }
}


// Load directory entries into the global entries array
static void load_directory(const char* path)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        entry_count = 0;
        return;
    }
    entry_count = 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL && entry_count < MAX_ENTRIES)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        strncpy(entries[entry_count].name, ent->d_name, sizeof(entries[entry_count].name) - 1);
        entries[entry_count].name[sizeof(entries[entry_count].name) - 1] = '\0';
        printf("%s\n", entries[entry_count].name);
        // Build full path for stat
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, ent->d_name);
        // Determine if the entry is a directory and get file size
        if (ent->d_type != DT_UNKNOWN)
        {
            // entries[entry_count].is_dir = (ent->d_type == DT_DIR) ? 1 : 0;

            if (entries[entry_count].is_dir) continue; // Skipping adding directories to file tree.
            // // should be changed in the future for more advanced use. currently directories are not supported.

            // Get file size using stat even if we know the type from d_type
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0)
            {
                entries[entry_count].file_size = entries[entry_count].is_dir ? 0 : statbuf.st_size;
            }
            else
            {
                entries[entry_count].file_size = 0;
            }
        }
        else
        {
            struct stat statbuf;
            if (stat(full_path, &statbuf) == 0)
            {
                entries[entry_count].is_dir = S_ISDIR(statbuf.st_mode) ? 1 : 0;
                entries[entry_count].file_size = entries[entry_count].is_dir ? 0 : statbuf.st_size;
            }
            else
            {
                entries[entry_count].is_dir = 0;
                entries[entry_count].file_size = 0;
            }
        }
        entry_count++;
    }
    closedir(dir);
    selected_index = 0;
}

bool fs_init(void)
{
    telog("fs init SD\n");
    blockdevice_t* sd = blockdevice_sd_create(spi0,
        SD_MOSI_PIN,
        SD_MISO_PIN,
        SD_SCLK_PIN,
        SD_CS_PIN,
        125000000 / 2 / 4, // 15.6MHz
        true);
    filesystem_t* fat = filesystem_fat_create();
    int err = fs_mount("/sd", fat, sd);
    if (err == -1)
    {
        telog("format /sd\n");
        err = fs_format(fat, sd);
        if (err == -1)
        {
            telog("format err: %s\n", strerror(errno));
            return false;
        }
        err = fs_mount("/sd", fat, sd);
        if (err == -1)
        {
            telog("mount err: %s\n", strerror(errno));
            return false;
        }
    }
    return true;
}

static bool __not_in_flash_func(is_same_existing_program)(FILE* fp)
{
    telog("checking program for sameness");
    uint8_t buffer[FLASH_SECTOR_SIZE] = { 0 };
    size_t program_size = 0;
    size_t len = 0;
    while ((len = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        uint8_t* flash = (uint8_t*)(XIP_BASE + SD_BOOT_FLASH_OFFSET + program_size);
        if (memcmp(buffer, flash, len) != 0)
            return false;
        program_size += len;
    }
    return true;
}

// This function must run from RAM since it erases and programs flash memory
static bool __not_in_flash_func(load_program)(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    telog("file open for loading");
    if (fp == NULL)
    {
        telog("open %s fail: %s\n", filename, strerror(errno));
        return false;
    }
    // if (is_same_existing_program(fp))
    // {
    //     telog("program is same as already here");
    //     return true;
    //     // Program is up to date
    // }

    // Check file size to ensure it doesn't exceed the available flash space
    if (fseek(fp, 0, SEEK_END) == -1)
    {
        telog("seek err: %s\n", strerror(errno));
        fclose(fp);
        return false;
    }

    long file_size = ftell(fp);
    if (file_size <= 0)
    {
        telog("invalid size: %ld\n", file_size);
        fclose(fp);
        return false;
    }

    if (file_size > MAX_APP_SIZE)
    {
        telog("file too large: %ld > %d\n", file_size, MAX_APP_SIZE);
        fclose(fp);
        return false;
    }

    telog("updating: %ld bytes\n", file_size);
    if (fseek(fp, 0, SEEK_SET) == -1)
    {
        telog("seek err: %s\n", strerror(errno));
        fclose(fp);
        return false;
    }

    size_t program_size = 0;
    uint8_t buffer[FLASH_SECTOR_SIZE] = { 0 };
    size_t len = 0;

    // Erase and program flash in FLASH_SECTOR_SIZE chunks
    while ((len = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        // Ensure we don't write beyond the application area
        if ((program_size + len) > MAX_APP_SIZE)
        {
            telog("err: write beyond app area\n");
            fclose(fp);
            return false;
        }

        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(SD_BOOT_FLASH_OFFSET + program_size, FLASH_SECTOR_SIZE);
        flash_range_program(SD_BOOT_FLASH_OFFSET + program_size, buffer, len);
        restore_interrupts(ints);

        program_size += len;
    }
    // telog("program loaded\n");
    fclose(fp);
    return true;
}

// This function jumps to the application entry point
// It must update the vector table and stack pointer before jumping
void __not_in_flash_func(launch_application_from)(uint32_t* app_location)
{
    // https://vanhunteradams.com/Pico/Bootloader/Bootloader.html
    uint32_t* new_vector_table = app_location;
    volatile uint32_t* vtor = (uint32_t*)(PPB_BASE + VTOR_OFFSET);
    *vtor = (uint32_t)new_vector_table;
    asm volatile(
        "msr msp, %0\n"
        "bx %1\n"
        :
    : "r"(new_vector_table[0]), "r"(new_vector_table[1])
        : );
}

// Check if a valid application exists in flash by examining the vector table
static bool is_valid_application(uint32_t* app_location)
{
    // Check that the initial stack pointer is within a plausible RAM region (assumed range for Pico: 0x20000000 to 0x20040000)
    uint32_t stack_pointer = app_location[0];
    if (stack_pointer < 0x20000000 || stack_pointer > MAX_RAM)
    {
        return false;
    }

    // Check that the reset vector is within the valid flash application area
    uint32_t reset_vector = app_location[1];
    if (reset_vector < (0x10000000 + SD_BOOT_FLASH_OFFSET) || reset_vector >(0x10000000 + PICO_FLASH_SIZE_BYTES))
    {
        return false;
    }
    return true;
}

int load_firmware_by_path(const char* path)
{
    // text_directory_ui_set_status("STAT: loading app...");

    // Attempt to load the application from the SD card
    // bool load_success = load_program(FIRMWARE_PATH);
    bool load_success = load_program(path);
    telog("program loaded");
    // Get the pointer to the application flash area
    uint32_t* app_location = (uint32_t*)(XIP_BASE + SD_BOOT_FLASH_OFFSET);

    // Check if there is an already valid application in flash
    bool has_valid_app = is_valid_application(app_location);


    if (load_success || has_valid_app)
    {
        // text_directory_ui_set_status("STAT: launching app...");
        // telog("launching app\n");
        // Small delay to allow printf to complete
        sleep_ms(100);
        launch_application_from(app_location);
    }
    else
    {
        // text_directory_ui_set_status("ERR: No valid app");
        telog("no valid app, halting\r\n");

        sleep_ms(2000);

        // Trigger a watchdog reboot
        watchdog_reboot(0, 0, 0);
    }

    // We should never reach here
    while (1)
    {
        tight_loop_contents();
    }
}

/**
 * Create scrolling text for long filenames
 * Creates a continuous scroll effect for text that exceeds visible area
 */
static void get_scrolling_text(const char* text, char* out, size_t out_size, int visible_chars)
{
    char scroll_buffer[512];
    snprintf(scroll_buffer, sizeof(scroll_buffer), "%s   %s", text, text);
    int scroll_len = strlen(scroll_buffer);
    uint32_t time_ms = time_us_64() / 1000;
    int offset = (time_ms / SCROLL_DELAY_MS) % scroll_len;

    int i;
    for (i = 0; i < visible_chars && i < out_size - 1; i++)
    {
        int idx = (offset + i) % scroll_len;
        out[i] = scroll_buffer[idx];
    }
    out[i] = '\0';
}

BootLoaderScreen::~BootLoaderScreen()
{
}

uint8_t entry_curr = 0, loading = 0;

uint8_t sd_error = 0;

void BootLoaderScreen::create()
{
    GameScene::create();

    // Initialize filesystem
    if (!fs_init())
    {
        // text_directory_ui_set_status("Failed to mount SD card!");
        telog("Failed to mount SD card\n");
        // sleep_ms(2000);
        // watchdog_reboot(0, 0, 0);
        sd_error = 1;
    }

    strncpy(current_path, "/sd", sizeof(current_path));
    load_directory(current_path);
    // load_directory("/sd/FW");

    m_engine.bind_serial_input_event('w', [&]
        {
            if (entry_curr > 0) entry_curr--;
        });

    // m_engine.bind_serial_input_event('w', [&] {
    //     if (entry_num > 0) entry_num--;
    // });
    //
    // m_engine.bind_serial_input_event('w', [&] {
    //     if (entry_num > 0) entry_num--;
    // });

    m_engine.bind_serial_input_event('s', [&]
        {
            entry_curr++;
            if (entry_curr > entry_count) entry_curr = 0;
        });

    m_engine.bind_serial_input_event('f', [&]
        {
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

char file_string_temp[255] = { 0 };

void BootLoaderScreen::render()
{
    GameScene::render();

    m_framebuffer.clear_with(bootloader_splash_screen);

    if (sd_error)
    {
        m_framebuffer.draw_filled_rectangle(50, 100, 275 - 50, 50, 15);
    }
    else
    {
        for (int i = 0; i < entry_count; ++i)
        {
            if (!entries[i].is_dir)
            {
                if (i == entry_curr)
                    m_framebuffer.draw_filled_rectangle(25, (75 + 25 / 2) + i * 10, 275 - 25, 10,
                        44);
                // file.render(m_framebuffer);
                sprintf(file_string_temp, "%c | %dKB | %s", entries[i].is_dir ? 'D' : 'F', entries[i].file_size / 1000,
                    entries[i].name);
                m_framebuffer.draw_string(file_string_temp, 25, (75 + 25 / 2) + i * 10, 15);
            }
        }

        if (loading)
        {
            m_framebuffer.draw_filled_rectangle(100, 100, 100, 12, 44);
            m_framebuffer.draw_string("Loading...", 100, 100, 15);
        }
    }
    m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

char temp2[512] = { 0 };

void BootLoaderScreen::update(double frameTime)
{
    GameScene::update(frameTime);

    if (loading && entries[entry_curr].is_dir)
    {
        /* Dont handle directories for simplicity. */
        // loading = 0;
        // sprintf(temp2, "/sd/%s", entries[entry_curr].name);
        // // printf(temp2);
        // load_directory(temp2);
        // entry_curr = 0;
    }

    // not the best way to do this ik.
    if (loading && !entries[entry_curr].is_dir)
    {
        if (std::string(entries[entry_curr].name).ends_with(".bin"))
        {
            telog("/sd/%s", entries[entry_curr].name);
            char new_path[255] = { 0 };
            snprintf(new_path, sizeof(new_path), "/sd/%s", entries[entry_curr].name);
            load_firmware_by_path(new_path);
        }
        else
        {
            telog("Not a valid file %s", entries[entry_curr].name);
        }
        loading = 0;
    }
}

void BootLoaderScreen::destroy()
{
    GameScene::destroy();
}
