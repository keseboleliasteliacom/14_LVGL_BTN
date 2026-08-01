/**
 * @file Price_UI.c
 * @brief Implementation of the electricity price UI tab.
 *
 * @ingroup Electricity
 */

#include "Price_UI.h"
#include "lvgl_port.h"
#include "../../screens/ui_Screen1.h"
#include "../../../main/LEOP/LEOP_Fetcher.h"

static const char *TAG = "Price_UI";

static Price_UI price_ui;

/**
 * @brief Implementation of Price_UI_Initialize.
 *
 * See header for full contract documentation.
 */
void Price_UI_Initialize()
{
    /* Main panel */
    price_ui.price_panel = lv_obj_create(ui_TabPage_Electricity);
    lv_obj_set_size(price_ui.price_panel, 320, 400);
    lv_obj_center(price_ui.price_panel);
    lv_obj_set_y(price_ui.price_panel, 450);

    lv_obj_t* panel_label = lv_label_create(ui_TabPage_Electricity);
    lv_obj_set_align(panel_label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(panel_label, 420);
    lv_label_set_text(panel_label, "Electricity Price");


    /* Scroll container */
    lv_obj_t *list = lv_obj_create(price_ui.price_panel);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_center(list);

    lv_obj_set_layout(list, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    for (int i = 0; i < 24; i++) {

        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, 36);

        lv_obj_set_layout(row, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row,
                              LV_FLEX_ALIGN_SPACE_BETWEEN,
                              LV_FLEX_ALIGN_CENTER,
                              LV_FLEX_ALIGN_CENTER);
        
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        /* Placeholder time */
        price_ui.hourLabel[i] = lv_label_create(row);
        lv_label_set_text(price_ui.hourLabel[i], "--:--");

        /* Placeholder price */
        price_ui.priceLabel[i] = lv_label_create(row);
        lv_label_set_text(price_ui.priceLabel[i], "--.--");
    }
}

/**
 * @brief Implementation of Price_UI_Update.
 *
 * See header for full contract documentation.
 */
void Price_UI_Update()
{
    static PriceList price_list;

    if (price_queue != NULL && xQueueReceive(price_queue, &price_list, 0) == pdPASS)
    {
        if (price_list.status.electricity_fetched)
        {
            //ESP_LOGI(TAG, "Should print");
            for (int j = 0; j < 24; j++)
            {
                int i = j * 4;
                int hour, min;

                sscanf(price_list.price[i].timestamp, "%*d-%*d-%*dT%d:%d:%*d", &hour, &min);

                min = 0;

                char buf[6];
                snprintf(buf, sizeof(buf), "%02d:00", hour);

                char price[10];
                snprintf(price, sizeof(price), "%.2f", price_list.price[i].current_prices);

                lv_label_set_text(price_ui.hourLabel[j], buf);
                lv_label_set_text(price_ui.priceLabel[j], price);
            }
        }
    }
}
