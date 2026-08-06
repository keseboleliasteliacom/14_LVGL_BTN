/**
 * @file UART.cpp
 * @brief UART console worker and helper functions.
 *
 * @ingroup UART
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_diag_shell.hpp"
#include "esp_log.h"
#include <stdio.h>
#include "esp_timer.h"
#include "../app_types.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_0
#define UART_BAUD 115200
#define BUF_SIZE 2048


static const char* TAG = "UART";
/**
 * @brief Reads up to one byte from the UART console.
 *
 * @param[out] byte Destination for the received byte.
 * @param[in] timeout Read timeout in FreeRTOS ticks.
 *
 * @return `true` if a byte was read, otherwise `false`.
 */
bool UART_ReadByte(uint8_t* byte, TickType_t timeout)
{
    return uart_read_bytes(UART_PORT, byte, 1, timeout) > 0;
}

/**
 * @brief Converts a string to lowercase.
 *
 * @param str Input string.
 *
 * @return Lowercase copy of the input string.
 */
std::string to_lower_copy(std::string str)
{
    for (char& c : str){
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return str;
}

/**
 * @brief Trims leading and trailing ASCII whitespace from a string.
 *
 * @param str Input string.
 *
 * @return Trimmed copy of the input string.
 */
std::string trim_copy(const std::string& str)
{
    const char* whitespace = " \t\r\n";
    // Searches from beginning of the first none-whitespace character index
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }

    // Same, but reversed - Seacrhing from end of string to find the first non-white space index
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}


/**
 * @brief Configures the UART console for the worker task.
 *
 * Sets up UART0 with the built-in console pins and a blocking RX driver.
 */
void UART_Init_new(void)
{
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));

    // Set GPIO pins, which is mostly uncanged in our current case since we don't want to connect to another physical device, but use the built in to get access to esp terminal
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    ));

    // Installing drivers.
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, 
        BUF_SIZE, //Size of uart's ring buffer
        0, //TX buffer so it becomes blocking
        0, // event queue size - No queue
        NULL, // event queue handle, dont want this atm
        0 //interupt flags, lets start minimum for now
    ));

    ESP_LOGI(TAG, "UART%d initialized at %d baud", UART_PORT, UART_BAUD);
}

/**
 * @brief UART worker task entry point.
 *
 * Runs in task context, initializes the UART console, and blocks while waiting
 * for input bytes from the RX driver.
 *
 * @param parameter Pointer to the application state passed to the task.
 */
extern "C" void UART_Work(void* parameter) {
    app_state_t* app = (app_state_t*)parameter;

    UART_Init_new();

   ESP_LOGI(TAG, "UART_Work started.");
   ESP_LOGI(TAG, "Type HELP for commands");

   char input[8];
   char line[128];
   int line_pos = 0;
   bool prompt_needed = true;

   uint8_t *buf = new uint8_t[BUF_SIZE];
   
   ESP_LOGI(TAG, "Uart WORK now entering main loop.");   

    while(1) {
        uint8_t byte;
        int len = uart_read_bytes(UART_PORT, &byte, 1, portMAX_DELAY);

        if (len > 0) {
            if (byte == '\n' || byte == '\r') {
                uart_write_bytes(UART_PORT, "\r\n", 2);
                line[line_pos] = '\0';
                ESP_LOGI(TAG, "Complete msg: %s", line);
                // trim and lowercase input.
                handle_input(to_lower_copy(trim_copy(line)), app);

                line_pos = 0;
                prompt_needed = true;
            }
            else {
                if (line_pos < sizeof(line) -1) {
                    line[line_pos++] = byte;
                    uart_write_bytes(UART_PORT, (const char*)&byte, 1);
                }
            }
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
