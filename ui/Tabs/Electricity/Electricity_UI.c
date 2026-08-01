/**
 * @file Electricity_UI.c
 * @brief Implementation of the Electricity tab UI.
 *
 * @ingroup ELECTRICITY_UI
 */

#include "Electricity_UI.h"
#include "lvgl_port.h"
#include "../../screens/ui_Screen1.h"
#include "../../../main/LEOP/LEOP_Fetcher.h"

static const char *TAG = "Electricity";

static Electricity_UI electricity_ui = {
    .ui_Chart_Electricity = NULL,
};

static lv_chart_series_t *electricity_series;
static lv_coord_t electricity_chart_data[96];

static lv_obj_t *time_container;
static lv_obj_t *time_labels[24]; // max 24 hours

/**
 * @brief Handles chart draw events to color bars based on value.
 *
 * Called from the LVGL draw callback context. Avoid blocking work here.
 *
 * @param[in] e LVGL event object.
 */
static void chart_event_cb(lv_event_t *e)
{
    lv_obj_t *chart = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);

        if (dsc->part == LV_PART_ITEMS)
        {
            int32_t id = dsc->id; // bar index

            lv_coord_t val = electricity_chart_data[id];

            if (val < 25)
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0x00FF00); // green
            }
            else if (val < 75)
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0xFFFF00); // yellow
            }
            else
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0xFF0000); // red
            }

            dsc->rect_dsc->radius = 0;
        }
    }
}

/**
 * @brief Extracts the hour from an ISO timestamp string.
 *
 * @param[in] iso ISO 8601 timestamp string with the hour at positions 11-12.
 *
 * @return Parsed hour value in the range 0-23.
 */
int get_hour(const char *iso)
{
    return (iso[11] - '0') * 10 + (iso[12] - '0');
}

/**
 * @brief Creates the time-axis label container under the chart.
 *
 * @note The container is aligned relative to the chart and creates 24 labels
 *       for hourly markers.
 */
void create_time_axis_container()
{
    time_container = lv_obj_create(ui_TabPage_Electricity);

    lv_obj_set_size(time_container, 700, 60);
    lv_obj_align_to(time_container, electricity_ui.ui_Chart_Electricity, LV_ALIGN_OUT_BOTTOM_MID, 0, -5);

    lv_obj_set_flex_flow(time_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(time_container,
                          LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_bg_opa(time_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(time_container, 0, 0);

    for (int i = 0; i < 24; i++)
    {
        time_labels[i] = lv_label_create(time_container);
    }
}

/**
 * @brief Updates the time-axis labels from fetched recommendation data.
 *
 * @param[in] data Recommendation data used to derive the first displayed hour.
 */
void Electricity_Update_TimeAxis(const RecommendationList *data)
{
    int start_hour =
        get_hour(data->rec[0].timestamp);

    for (int i = 0; i < 24; i++)
    {
        int hour = (start_hour + i) % 24;

        char buf[8];
        snprintf(buf, sizeof(buf), "%02d", hour);

        lv_label_set_text(time_labels[i], buf);
    }
}

/**
 * @brief Implementation of Electricity_UI_Initialize.
 *
 * See header for full contract documentation.
 */
void Electricity_UI_Initialize()
{
    electricity_ui.ui_Chart_Electricity = lv_chart_create(ui_TabPage_Electricity);
    lv_obj_set_width(electricity_ui.ui_Chart_Electricity, 689);
    lv_obj_set_height(electricity_ui.ui_Chart_Electricity, 285);
    lv_obj_set_x(electricity_ui.ui_Chart_Electricity, 3);
    lv_obj_set_y(electricity_ui.ui_Chart_Electricity, 20);
    lv_obj_set_align(electricity_ui.ui_Chart_Electricity, LV_ALIGN_CENTER);
    lv_chart_set_type(electricity_ui.ui_Chart_Electricity, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(electricity_ui.ui_Chart_Electricity, 96);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_X, 0, 0, 0, 0, false, 0);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 25);
    electricity_series = lv_chart_add_series(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x808080),
                                             LV_CHART_AXIS_PRIMARY_Y);
    memset(electricity_chart_data, 0, sizeof(electricity_chart_data));
    lv_chart_set_ext_y_array(electricity_ui.ui_Chart_Electricity, electricity_series, electricity_chart_data);
    lv_chart_set_range(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_obj_set_style_bg_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x20142F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(electricity_ui.ui_Chart_Electricity, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x9240FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(electricity_ui.ui_Chart_Electricity, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0xFFFFFF), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(electricity_ui.ui_Chart_Electricity, 255, LV_PART_TICKS | LV_STATE_DEFAULT);

    create_time_axis_container();

    lv_obj_add_event_cb(electricity_ui.ui_Chart_Electricity, chart_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

/**
 * @brief Implementation of Electricity_UI_Update.
 *
 * See header for full contract documentation.
 */
void Electricity_UI_Update(void)
{
    static RecommendationList rec_list;
    if (recommendation_queue != NULL && xQueueReceive(recommendation_queue, &rec_list, 0) == pdPASS)
    {
        if (rec_list.status.recommendation_fetched)
        {
            for (int i = 0; i < 96; i++)
            {
                float value = rec_list.rec[i].recommendation;

                if (value < 0.0f)
                    value = 0.0f;

                if (value > 1.0f)
                    value = 1.0f;

                // ESP_LOGI(TAG, "value: %.f", value);

                electricity_chart_data[i] = (lv_coord_t)(value * 100.0f);
            }
            Electricity_Update_TimeAxis(&rec_list);
            lv_chart_refresh(electricity_ui.ui_Chart_Electricity);
        }
    }
}
