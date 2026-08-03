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
static lv_coord_t electricity_chart_data[LEOP_FORECAST_MAX_ENTRIES];
static RecommendationAction electricity_recommendations[LEOP_FORECAST_MAX_ENTRIES];

static lv_obj_t *dashboard;
static lv_obj_t *recommendation_panel;
static lv_obj_t *price_panel;
static lv_obj_t *time_container;
static lv_obj_t *time_labels[12];

/**
 * @brief Handles chart draw events to color bars based on value.
 *
 * Called from the LVGL draw callback context. Avoid blocking work here.
 *
 * @param[in] e LVGL event object.
 */
static void chart_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);

        if (dsc->part == LV_PART_ITEMS)
        {
            int32_t id = dsc->id; // bar index

            RecommendationAction action = LEOP_RECOMMENDATION_UNKNOWN;
            if (id >= 0 && id < LEOP_FORECAST_MAX_ENTRIES)
                action = electricity_recommendations[id];

            if (action == LEOP_RECOMMENDATION_BUY)
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0x00FF00); // green
            }
            else if (action == LEOP_RECOMMENDATION_HOLD)
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0xFFFF00); // yellow
            }
            else if (action == LEOP_RECOMMENDATION_SELL)
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0xFF0000); // red
            }
            else
            {
                dsc->rect_dsc->bg_color = lv_color_hex(0x808080); // unknown
            }

            dsc->rect_dsc->radius = 0;
        }
    }
}

/**
 * @brief Extracts the hour from an ISO 8601 timestamp string.
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
 * @note Twelve two-hour markers keep the timeline precise without overlap.
 */
void create_time_axis_container()
{
    time_container = lv_obj_create(recommendation_panel);

    lv_obj_set_size(time_container, LV_PCT(92), 28);

    lv_obj_set_flex_flow(time_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(time_container,
                          LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_bg_opa(time_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(time_container, 0, 0);

    for (int i = 0; i < 12; i++)
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
    for (int i = 0; i < 12; i++)
    {
        size_t index = data->count > 1
                           ? ((size_t)i * (data->count - 1)) / 11
                           : 0;
        int hour = get_hour(data->rec[index].timestamp);
        int minute = (data->rec[index].timestamp[14] - '0') * 10 +
                     (data->rec[index].timestamp[15] - '0');

        char buf[12];
        snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);

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
    lv_obj_clear_flag(ui_TabPage_Electricity, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(ui_TabPage_Electricity, 8, 0);

    dashboard = lv_obj_create(ui_TabPage_Electricity);
    lv_obj_remove_style_all(dashboard);
    lv_obj_set_size(dashboard, LV_PCT(100), LV_PCT(100));
    lv_obj_set_layout(dashboard, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(dashboard, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(dashboard, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_column(dashboard, 8, 0);
    lv_obj_clear_flag(dashboard, LV_OBJ_FLAG_SCROLLABLE);

    recommendation_panel = lv_obj_create(dashboard);
    lv_obj_set_size(recommendation_panel, 0, LV_PCT(100));
    lv_obj_set_flex_grow(recommendation_panel, 7);
    lv_obj_set_layout(recommendation_panel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(recommendation_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(recommendation_panel, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(recommendation_panel, 8, 0);
    lv_obj_set_style_pad_row(recommendation_panel, 4, 0);
    lv_obj_set_style_bg_color(recommendation_panel, lv_color_hex(0x20142F), 0);
    lv_obj_clear_flag(recommendation_panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(recommendation_panel);
    lv_label_set_text(title, "Energy recommendation");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    lv_obj_t *scale_help = lv_label_create(recommendation_panel);
    lv_label_set_text(scale_help, "Score 0-100 (height)  |  Color shows recommendation");
    lv_obj_set_style_text_color(scale_help, lv_color_hex(0xD8CCE5), 0);

    electricity_ui.ui_Chart_Electricity = lv_chart_create(recommendation_panel);
    /* Leave room for Y-axis tick labels, which LVGL draws outside the chart. */
    lv_obj_set_width(electricity_ui.ui_Chart_Electricity, LV_PCT(92));
    lv_obj_set_flex_grow(electricity_ui.ui_Chart_Electricity, 1);
    lv_chart_set_type(electricity_ui.ui_Chart_Electricity, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(electricity_ui.ui_Chart_Electricity, LEOP_FORECAST_MAX_ENTRIES);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_X, 0, 0, 0, 0, false, 0);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 2, true, 50);
    lv_chart_set_axis_tick(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_SECONDARY_Y, 10, 5, 5, 2, true, 25);
    electricity_series = lv_chart_add_series(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x808080),
                                             LV_CHART_AXIS_PRIMARY_Y);
    memset(electricity_chart_data, 0, sizeof(electricity_chart_data));
    memset(electricity_recommendations, 0, sizeof(electricity_recommendations));
    lv_chart_set_ext_y_array(electricity_ui.ui_Chart_Electricity, electricity_series, electricity_chart_data);
    lv_chart_set_range(electricity_ui.ui_Chart_Electricity, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_obj_set_style_bg_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x20142F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(electricity_ui.ui_Chart_Electricity, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0x9240FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(electricity_ui.ui_Chart_Electricity, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(electricity_ui.ui_Chart_Electricity, lv_color_hex(0xFFFFFF), LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(electricity_ui.ui_Chart_Electricity, 255, LV_PART_TICKS | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(electricity_ui.ui_Chart_Electricity, 6, LV_PART_MAIN);
    lv_obj_set_style_pad_right(electricity_ui.ui_Chart_Electricity, 6, LV_PART_MAIN);

    create_time_axis_container();

    lv_obj_t *legend = lv_label_create(recommendation_panel);
    lv_label_set_recolor(legend, true);
    lv_label_set_text(legend,
                      "#00FF00 Buy#   #FFFF00 Hold#   #FF0000 Sell#   #808080 Unknown#");

    price_panel = lv_obj_create(dashboard);
    lv_obj_set_size(price_panel, 0, LV_PCT(100));
    lv_obj_set_flex_grow(price_panel, 3);
    lv_obj_set_style_pad_all(price_panel, 8, 0);
    lv_obj_set_style_bg_color(price_panel, lv_color_hex(0x20142F), 0);
    lv_obj_clear_flag(price_panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(electricity_ui.ui_Chart_Electricity, chart_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

/**
 * @brief Returns the dashboard panel reserved for electricity prices.
 *
 * @return Pointer to the price panel object created during initialization.
 */
lv_obj_t *Electricity_UI_GetPricePanel(void)
{
    return price_panel;
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
            size_t count = rec_list.count;
            if (count > LEOP_FORECAST_MAX_ENTRIES)
                count = LEOP_FORECAST_MAX_ENTRIES;

            memset(electricity_chart_data, 0, sizeof(electricity_chart_data));
            memset(electricity_recommendations, 0, sizeof(electricity_recommendations));
            lv_chart_set_point_count(electricity_ui.ui_Chart_Electricity, count);

            for (size_t i = 0; i < count; i++)
            {
                float value = rec_list.rec[i].recommendation;

                if (value < 0.0f)
                    value = 0.0f;

                if (value > 1.0f)
                    value = 1.0f;

                // ESP_LOGI(TAG, "value: %.f", value);

                electricity_chart_data[i] = (lv_coord_t)(value * 100.0f);
                electricity_recommendations[i] = rec_list.rec[i].action;
            }
            if (count > 0)
                Electricity_Update_TimeAxis(&rec_list);
            lv_chart_refresh(electricity_ui.ui_Chart_Electricity);
        }
    }
}
