#ifndef UART_HPP
#define UART_HPP

/**
 * @file UART.hpp
 * @brief Public declarations for the UART console worker and helper functions.
 *
 * Declares the C and C++ entry points used by the UART console task.
 *
 * @ingroup UART
 * @defgroup UART UART
 * @brief UART console worker and helper functions.
 *
 * Provides the UART task entry point and byte-oriented console input helper
 * used by the application.
 * @{
 */

#include <stdint.h> // c-version since main compiles this file
#include "freertos/FreeRTOS.h"
#include "../app_types.h"


/**
 * @brief Reads one byte from the UART console.
 *
 * @param[out] byte Destination for the received byte.
 * @param[in] timeout UART read timeout in FreeRTOS ticks.
 *
 * @return `true` if a byte was read, otherwise `false`.
 */
 // c++ exposed functions
#ifdef __cplusplus
bool UART_ReadByte(uint8_t* byte, TickType_t timeout);
#endif

#ifdef __cplusplus
    extern "C" {
#endif
/**
 * @brief UART worker task entry point.
 *
 * @param parameter Pointer to the application state passed to the task.
 *
 * @note Runs in task context and blocks while waiting for UART input.
 */
 // c exposed functions
void UART_Work(void* parameter);


#ifdef __cplusplus
    }
#endif

/** @} */

#endif
