/**
 * @file uart_diag_shell.cpp
 * @brief Implementation of the UART diagnostic shell module.
 *
 * @ingroup UART
 */

#include "esp_log.h"
#include <iostream>
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_diag_shell.hpp"
#include <vector>
#include <sstream>
#include <cstdlib>
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "../app_types.h"
#include "UART.hpp"
#include <ctime>
#include "../Utils/TimeFormat.h"

#include "../LEOP/Price.h"
#include "../LEOP/Recommendation.h"
#include "../LEOP/Weather.h"
#include "../LEOP/LEOP_Fetcher.h"

#include "../Config/AppConfig.h"
#include <stdint.h>



static const char *TAG = "UART_DIAG_SHELL.CPP";

/**
 * @brief Print current sensor values and timing state.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_sensor(app_state_t *state);

/**
 * @brief Print current system status flags.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_status(app_state_t *state);

/**
 * @brief Print the latest LEOP recommendation data.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_leop(app_state_t *state);

/**
 * @brief Update runtime configuration from parsed shell tokens.
 *
 * @param[in] tokens Parsed command tokens.
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_config(std::vector<std::string> tokens, app_state_t *state);

/**
 * @brief Print runtime heap and task diagnostics.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_diag(app_state_t *state);

/**
 * @brief Print the current runtime configuration values.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void print_config(app_state_t *state);

/**
 * @brief Print shell help text.
 *
 * @param[in] wait_for_enter When `true`, pauses between help lines.
 */
void handle_help(bool wait_for_enter);

/**
 * @brief Print a single help line.
 *
 * @param[in] message Text to print.
 * @param[in] wait_for_enter When `true`, waits for Enter after printing.
 */
void print_help_line(const std::string message, bool wait_for_enter);

/**
 * @brief Print a message and wait for Enter on UART.
 *
 * @param[in] message Message shown before blocking for input.
 */
void print_and_wait_for_enter(const std::string);

/**
 * @brief Split a string using a delimiter character.
 *
 * @param[in] str Input string to split.
 * @param[in] delimiter Delimiter character.
 * @return Tokens extracted from the string.
 */
std::vector<std::string> split(const std::string &str, char delimiter);

/**
 * @brief Parse a decimal integer from a string.
 *
 * @param[in] str Input string to parse.
 * @param[out] out Parsed integer value on success.
 * @return `true` if the full string was parsed as an integer.
 */
bool parse_int(const std::string &str, int &out);

// *** HELPER FUNCTIONS ***
/**
 * @brief Split a string using a delimiter character.
 *
 * @param[in] str Input string to split.
 * @param[in] delimiter Delimiter character.
 * @return Tokens extracted from the string.
 */
std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
        result.push_back(item);
    }
    return result;
}

// todo - maybe change to enum instead of simple format helpers?
/**
 * @brief Return connection-state text for shell output.
 *
 * @param[in] value Connection state flag.
 * @return `"Connected"` when `true`, otherwise `"Disconnected"`.
 */
const char *connected_text(bool value)
{
    return value ? "Connected" : "Disconnected";
}

/**
 * @brief Return enabled-state text for shell output.
 *
 * @param[in] value Enabled state flag.
 * @return `"Enabled"` when `true`, otherwise `"Disabled"`.
 */
const char *enabled_text(bool value)
{
    return value ? "Enabled" : "Disabled";
}

/**
 * @brief Return generic status text for shell output.
 *
 * @param[in] value Status flag.
 * @return `"OK"` when `true`, otherwise `"Not OK"`.
 */
const char *ok_text(bool value)
{
    return value ? "OK" : "Not OK";
}

/**
 * @brief Print a Unix timestamp as local wall-clock time.
 *
 * @param[in,out] time Unix time value to format.
 */
static void print_local_time(time_t& time) {
    struct tm local_time;
    localtime_r(&time, &local_time);

    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_time);
    std::cout << "Last updated local time: " << buffer << std::endl;
}


// Takes in a std::string,
// .c_str() converts to C-style const char*,
// so std::strtol:
// Converts our c++ string to a C char *
// Reds and attempts to convert char to long.
// It gives saves two values: val(our actual value), and end(pointer to where we stopped tracking).
// 10 is the number base we working with(10 gives us the usual 0-9 numbers).
// If it stopped reading before a null-terminator, then our string includes characters that could not be converted to long.
// But if no issues, then the number can be parsed as int.
/**
 * @brief Parse a decimal integer from a string.
 *
 * @param[in] str Input string to parse.
 * @param[out] out Parsed integer value on success.
 * @return `true` if the full string was parsed as an integer.
 */
bool parse_int(const std::string &str, int &out)
{
    char *end;
    // Reads string from start to end. "val" is the number it managed to read, "end" is where it stopped reading
    long val = std::strtol(str.c_str(), &end, 10);

    if (*end != '\0')
    {
        return false;
    }
    out = static_cast<int>(val);
    return true;
}

// *** MAIN FUNCTIONS ***
/**
 * @brief Implementation of handle_input.
 *
 * See header for full contract documentation.
 */
void handle_input(const std::string &input, app_state_t *state)
{
    int msg_len = input.length();
    ESP_LOGI(TAG, "msg len: %d\n", msg_len);
    std::vector<std::string> tokens = split(input, ' ');
    if (tokens.size() == 0)
    {
        std::cout << "No or incorrect input. Try again. " << std::endl;
        return;
    }
    const std::string &cmd = tokens[0];

    // todo - testing, to be removed
    if (cmd == "reboot"){
        std::cout << "restarting device.. " << std::endl;
        std::cout.flush();
        esp_restart();
    }

    if (tokens.size() == 2 && cmd == "help" && tokens[1] == "immersive")
    {
        handle_help(true);
    }
    else if (cmd == "help")
    {
        handle_help(false);
    }
    else if (cmd == "status")
    {
        handle_status(state);
    }
    else if (cmd == "sensor")
    {
        handle_sensor(state);
    }
    else if (cmd == "leop")
    {
        handle_leop(state);
    }
    else if (cmd == "config")
    {
        handle_config(tokens, state);
    }
    else if (cmd == "pconfig")
    {
        print_config(state);
    }
    else if (cmd == "diag")
    {
        handle_diag(state);
    }
    else
    {
        std::cout << "Unknown command: " << input << std::endl;
    }
}

// Todo - Update counter currently throttled to once a second. Disable this?
/**
 * @brief Print current system status flags.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_status(app_state_t* state)
{
    bool wifiStatus = state->system_status.wifi_connected;
    bool LEOPStatus = state->system_status.leop_connected;
    bool sensorValid = state->sensor_data.valid;
    bool TimeSynced = TimeSync_IsSynced();
    bool AllOK = false;
    if (!wifiStatus || !LEOPStatus || !sensorValid || !TimeSynced)
    {
        std::cout << "Degraded." << std::endl;
    }
    else {
        std::cout << "All systems OK." << std::endl;
    }
    
    uint64_t uptime_seconds = (uint64_t)esp_timer_get_time() / 1000000ULL;
    std::cout << "\nUptime: " << uptime_seconds << std::endl;


    std::cout << "Wifi: " << connected_text(wifiStatus) << std::endl;
    std::cout << "LEOP: " << connected_text(LEOPStatus) << std::endl;
    
    if (sensorValid) {
        std::cout << "Sensor: OK" << std::endl;
    }
    else if (state->sensor_data.last_update_seconds > 0) {
        std::cout << "Sensor: Not OK(no successful reading since startup)"<< std::endl; 
    }
    else {
        char duration[32];
        uint64_t now = esp_timer_get_time() / 1000000ULL;
        uint64_t age = now - state->sensor_data.last_update_seconds;

        TimeFormat_FormatDuration(duration, sizeof(duration), age);

        std::cout << "Sensor: Not OK (last worked " << duration << " ago)" << std::endl;
    }

    if (TimeSynced) {
        std::cout << "Time sync: Synchronized" << std::endl;
    }
    else {
        std::cout << "Time sync: Not synchronized" << std::endl;
    }

}

/**
 * @brief Print current sensor values and timing state.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_sensor(app_state_t *state)
{
    std::cout << "Debug valid: " << state->sensor_data.valid << std::endl;
    std::cout << "Temperature raw: " << state->sensor_data.temperature << std::endl;

    if (!state->sensor_data.valid) {
        std::cout << "Sensor: No valid data yet." << std::endl;
        return;
    }

    std::cout << "Last updated monotinic time: " << state->sensor_data.last_update_seconds << std::endl;
    // If we have synced our local clock to SNTP via wifi and have a valid unix time, we print that to user
    if (state->sensor_data.wall_time_valid) {
        print_local_time(state->sensor_data.last_unix_time);
    }
    else {
        std::cout << "Last updated local time: not synced yet" << std::endl;
    }
    std::cout << "Temperature - " << state->sensor_data.temperature << std::endl;
    std::cout << "Pressure    - " << state->sensor_data.pressure << std::endl;
    std::cout << "Humidity    - " << state->sensor_data.humidity << std::endl;
}

/**
 * @brief Print the current runtime configuration values.
 *
 * @param[in,out] state Shared application state used by the shell.
 */
void print_config(app_state_t *state)
{
    std::cout << "fetch_interval_minutes: " << state->config_data.fetch_interval_minutes << std::endl;
    std::cout << "sensor_interval_ms: " << state->config_data.sensor_interval_ms << std::endl;
    std::cout << "test_mode: " << enabled_text(state->config_data.test_mode) << std::endl;
}

/**
 * @brief Update runtime configuration from parsed shell tokens.
 *
 * @param[in] tokens Parsed command tokens.
 * @param[in,out] state Shared application state used by the shell.
 */
void handle_config(std::vector<std::string> tokens, app_state_t *state)
{
    std::string help_msg = "syntax: \"config <config_name> <value>\".\nFields (name) (value):\n \tfetch_interval_minutes uint32_t\n\ttest_mode bool\n";
    if (tokens.size() != 3)
    {
        std::cout << help_msg;
        return;
    }
    const std::string &key = tokens[1];
    const std::string &value = tokens[2];
    if (key == "fetch_interval_minutes")
    {
        // production TODO - Fetch interval can be set by any value from 1min to 24h. In production change this to 15min?
        int int_value;
        // Use helper function to see if we can parse something as int
        if (parse_int(value, int_value) && int_value >= 1 && int_value <= 1440)
        {            
            std::cout << "Now setting \"fetch_interval_minutes\" to \"" << int_value << "\"." << std::endl;
            state->config_data.fetch_interval_minutes = int_value;
            // Also save the changed settings to NVS
            int result = Config_WriteToNVS_FetchIntervalMinutes(int_value);
            if (result != 0) {
                ESP_LOGW(TAG, "Something failed when attempting to write \"new fetch_interval_minutes\" to NVS.");
            }
        }
        else { 
            std::cout << "You must enter a int value between 1(1 minute) and 1440 minutes(1 day)." << std::endl;
        }
    }
    else if (key == "sensor_interval_ms")
    {
        // Minimum och maximum mellan 1s och 60s
        int int_value; 
        if (parse_int(value, int_value))
        {
            if (int_value >= 1000 && int_value <= 60000)
            {
                std::cout << "Now setting \"sensor_interval_ms\" to \"" << int_value << "\"." << std::endl;
                state->config_data.sensor_interval_ms = int_value;
                int result = Config_WriteToNVS_SensorIntervalMs(int_value);
                if (result != 0) {
                    ESP_LOGW(TAG, "Something failed when attempting to write new \"sensor_interval_ms\" to NVS.");
                }
            }
            else {
                std::cout << "You must enter a int value between 1 000 and 60 000ms(1-60s)" << std::endl;
            }
        }
        else {
                std::cout << "You must enter a int value between 1 000 and 60 000ms(1-60s)" << std::endl;
        }
    }
    else if (key == "test_mode")
    {
        // TODO - add logic to not unecessecarily write new config if new value is same as old(true -> true)?
        if (value == "true")
        {
            std::cout << "Now setting \test_mode\" to \"true\"." << std::endl;
            state->config_data.test_mode = true;
            int result = Config_WriteToNVS_TestMode(true);
            if (result != 0) {
                ESP_LOGW(TAG, "Something failed when attempting to write new \"test_mode=true\"  to NVS.");
            }

        }
        else if (value == "false")
        {
            std::cout << "Now setting \test_mode\" to \"false\"." << std::endl;
            state->config_data.test_mode = false;
            int result = Config_WriteToNVS_TestMode(false);
            if (result != 0) {
                ESP_LOGW(TAG, "Something failed when attempting to write new \"test_mode=false\" to NVS.");
            }
        }
    }
    else
    {
        std::cout << help_msg;
    }
}

/**
 * @brief Print the latest LEOP recommendation data.
 *
 * @param[in,out] app Shared application state used by the shell.
 */
void handle_leop(app_state_t *app)
{
    if (app == NULL) {
        ESP_LOGE(TAG, "app is null in handle_leop");
    }
    LEOPData &leop = app->leop_data;
    uint32_t total_entries = leop.recommendations.count;
    std::cout << "--- Latest leop data --- " << std::endl;
    std::cout << "Number of entries: " << total_entries << std::endl;
    std::cout << "Now printing entries  recommendation and timestamp" << std::endl;
    for (int i = 0; i < total_entries; i++)
    {
        std::cout << leop.recommendations.rec[i].recommendation << ", " << leop.recommendations.rec[i].timestamp << std::endl;
    }
    
}

/**
 * @brief Print basic FreeRTOS stack usage information for one task.
 *
 * @param[in] name Display name for the task.
 * @param[in] handle Task handle to inspect.
 * @param[in] stack_size Expected total stack size used for the calculation.
 */
void print_task_stack(const char* name, TaskHandle_t handle, uint32_t stack_size)
{
    if (handle == NULL)
    {
        std::cout << name << ": no handle: " << std::endl;
        return;
    }

    UBaseType_t free = uxTaskGetStackHighWaterMark(handle);
    uint32_t used = stack_size - free;
    uint32_t used_percent = (used * 100) / stack_size;

    std::cout << name << ": used " << used << "/" << stack_size << " {" << used_percent << "%), min free " << free << std::endl;
}

/**
 * @brief Print runtime heap and task diagnostics.
 *
 * @param[in,out] app Shared application state used by the shell.
 */
void handle_diag(app_state_t *app)
{
    // Mirkoseconds since boot, then converted to seconds and divide by matching type (unsigned long long)
    uint64_t uptime_seconds = esp_timer_get_time() / 1000000ULL;

    // current normal heap available
    size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    // returns the lowest heap has ever gotten since we started the program
    size_t min_free_heap = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);

    UBaseType_t task_count = uxTaskGetNumberOfTasks();

    std::cout << "Diagnostics: " << std::endl;
    std::cout << "Uptime: " << uptime_seconds << " seconds." << std::endl;
    std::cout << "Free Heap: " << free_heap << " bytes. " << std::endl;
    std::cout << "Minimum free heap: " << min_free_heap << " bytes." << std::endl;
    std::cout << "Task count: " << task_count << std::endl;

    print_task_stack(app->system_task_handlers.wifi_task.name, app->system_task_handlers.wifi_task.handle, app->system_task_handlers.wifi_task.stack_size);
    print_task_stack(app->system_task_handlers.ui_task.name, app->system_task_handlers.ui_task.handle, app->system_task_handlers.ui_task.stack_size);
    print_task_stack(app->system_task_handlers.sensor_task.name, app->system_task_handlers.sensor_task.handle, app->system_task_handlers.sensor_task.stack_size);
    print_task_stack(app->system_task_handlers.uart_task.name, app->system_task_handlers.uart_task.handle, app->system_task_handlers.uart_task.stack_size);
    print_task_stack(app->system_task_handlers.leop_task.name, app->system_task_handlers.leop_task.handle, app->system_task_handlers.leop_task.stack_size);
}

// *** HELP helper functions ***
/**
 * @brief Print shell help text.
 *
 * @param[in] wait_for_enter When `true`, pauses between help lines.
 */
void handle_help(bool wait_for_enter)
{
    if (wait_for_enter == true)
    {
        print_and_wait_for_enter("So, you're looking for a little bit of help, huh?");
        print_and_wait_for_enter("Well, people rarely come here having their shit together. You're no different, from what I can tell.");
        print_and_wait_for_enter("I'll let you know beforehand, those who come seeking advice, but tries to slither away when it comes to collecting payment, they usually end up...worse. Worse than when they came to find me.");
        print_and_wait_for_enter("Now let me tell you the three pieces of advice I've learned while soaring from these filthy streets to now becoming..a king, of sorts.");
        print_and_wait_for_enter("#1. Invest in index funds. Small incremental steps and stacking interest will eventually help you living, not just surviving, when you are older.");
        print_and_wait_for_enter("#2. Don't neglect your family, friends or partner. Relationships are what makes life worth living after all.");
        print_and_wait_for_enter("#3. If someone who claims to be a prince contacts you and wants your financial help in securing his inheritage, turn around and don't look back. You'll lose everything you have. Been there, done that.");
        print_and_wait_for_enter(".");
        print_and_wait_for_enter("..");
        print_and_wait_for_enter("...");
        print_and_wait_for_enter("What? You're looking for some other sort of help?");
        print_and_wait_for_enter("Well, I do know a little bit of this, a little bit of that...though I doubt it'll be helpful for you.");
        print_and_wait_for_enter("Last week I helper a grandma collect 8 blue cranberries, and she did give me a note from her late husband, saying it was the only thing she should repay the honorary action with.");
        print_and_wait_for_enter("(He gives you the note)");
    }
    std::cout << "Here are the most common commands when attempting to use the UART-diagnostics interface when working with ESP32S3 units:" << std::endl;
    std::cout << "STATUS - Shows system health(WiFi, LEOP-connection, sensor, uptime)" << std::endl;
    std::cout << "LEOP - Shows latest data received from the LEOP-server." << std::endl;
    std::cout << "SENSOR - Shows current BME280-readings." << std::endl;
    std::cout << "CONFIG <param> <value> - Change config values." << std::endl;
    std::cout << "PCONFIG - prints out current config values." << std::endl;
    std::cout << "DIAG - shows system diagnostics(Task-statistics, heap-usage etc.)" << std::endl;
    std::cout << "HELP - This command =)" << std::endl;
}

/**
 * @brief Print a message and wait for Enter on UART.
 *
 * @param[in] message Message shown before blocking for input.
 *
 * @note Blocks in task context while waiting for UART input.
 */
void print_and_wait_for_enter(const std::string message)
{
    uint8_t byte;
    std::cout << message << std::endl;
    std::cout.flush();

    while (1) {
        if (UART_ReadByte(&byte, portMAX_DELAY)) {
            if (byte == '\n' || byte == '\r') {
                std::cout << std::endl;
                return;
            }
        }
    }
}
