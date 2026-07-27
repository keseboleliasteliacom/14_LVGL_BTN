/**
 * @file leop.cpp
 * @brief Implementation of the LEOP module.
 *
 * @ingroup LEOP
 */

#include "fake_leop.hpp"
#include "leop.h"

static const char *TAG = "leop";

static bool fake_mode = true;

/**
 * @brief Implementation of Leop_Init.
 *
 * See header for full contract documentation.
 */
void Leop_Init(app_state_t* app)
{
    if (fake_mode == true)
    {
        fake_leop_fill(&app->leop_data);
    }
}

/**
 * @brief Implementation of Leop_GetData.
 *
 * See header for full contract documentation.
 */
bool Leop_GetData(LEOPData* leop)
{
    if (fake_mode == true)
    {
        // TODO - fake_leop_update(leop);
        return true;
    }

    return false;
}

/**
 * @brief Implementation of Leop_Work.
 *
 * See header for full contract documentation.
 */
void Leop_Work(void* parameter) {
    app_state_t* app = (app_state_t*)parameter;

    Leop_Init(app);

}
