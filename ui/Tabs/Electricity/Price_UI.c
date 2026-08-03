/**
 * @file Price_UI.c
 * @brief Implementation of the electricity price UI tab.
 *
 * Creates the LVGL widgets for the electricity price view and refreshes them
 * from the queued price data when available.
 *
 * @ingroup Electricity
 */

#include "Price_UI.h"
#include "Electricity_UI.h"
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
    price_ui.price_panel = Electricity_UI_GetPricePanel();
    if (price_ui.price_panel == NULL)
    {
        ESP_LOGE(TAG, "Electricity dashboard was not initialized");
        return;
    }

    lv_obj_t *panel_label = lv_label_create(price_ui.price_panel);
    lv_obj_align(panel_label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text(panel_label, "Electricity price (SEK)");
    lv_obj_set_style_text_color(panel_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(panel_label, &lv_font_montserrat_20, 0);


    /* Scroll container */
    lv_obj_t *list = lv_obj_create(price_ui.price_panel);
    lv_obj_set_size(list, lv_pct(100), lv_pct(90));
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_pad_all(list, 4, 0);
    lv_obj_set_style_pad_row(list, 3, 0);

    lv_obj_set_layout(list, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

    for (int i = 0; i < 32; i++) {

        lv_obj_t *row = lv_obj_create(list);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, 36);
        lv_obj_set_style_pad_left(row, 6, 0);
        lv_obj_set_style_pad_right(row, 6, 0);

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
            size_t hour_count = (price_list.count + 3) / 4;
            if (hour_count > 32)
                hour_count = 32;

            for (size_t j = 0; j < 32; j++)
            {
                if (j >= hour_count)
                {
                    lv_obj_add_flag(lv_obj_get_parent(price_ui.hourLabel[j]), LV_OBJ_FLAG_HIDDEN);
                    continue;
                }

                lv_obj_clear_flag(lv_obj_get_parent(price_ui.hourLabel[j]), LV_OBJ_FLAG_HIDDEN);
                size_t i = j * 4;
                int hour, min;

                sscanf(price_list.price[i].timestamp, "%*d-%*d-%*dT%d:%d:%*d", &hour, &min);

                min = 0;

                char buf[6];
                snprintf(buf, sizeof(buf), "%02d:00", hour);

                char price[16];
                snprintf(price, sizeof(price), "%.2f SEK", price_list.price[i].current_prices);

                lv_label_set_text(price_ui.hourLabel[j], buf);
                lv_label_set_text(price_ui.priceLabel[j], price);
            }
        }
    }
}
