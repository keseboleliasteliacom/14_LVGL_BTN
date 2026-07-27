#ifndef UART_DIAG_SHELL_HPP
#define UART_DIAG_SHELL_HPP

#include <string>
#include "../app_types.h"
#include "../SNTP/time_sync.h"

/**
 * @file uart_diag_shell.hpp
 * @brief Public interface for the UART diagnostic shell.
 *
 * Provides the command-entry function used by the UART task to parse
 * diagnostic input and dispatch shell commands against shared application
 * state.
 *
 * @defgroup UART UART
 * @brief UART communication and diagnostic helpers.
 * @{
 */

/**
 * @brief Parse one UART shell command and dispatch it to the appropriate handler.
 *
 * @param[in] input Command string received from the UART interface.
 * @param[in,out] state Shared application state read and updated by commands.
 *
 * @note Intended for task context.
 */
void handle_input(const std::string& input, app_state_t* state);

/** @} */

#endif
