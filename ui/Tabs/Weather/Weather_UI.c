/**
 * @file Weather_UI.c
 * @brief Implementation of the Weather UI module.
 *
 * @ingroup WEATHER_UI
 */

#include "Weather_UI.h"
#include "lvgl_port.h"
#include "../../screens/ui_Screen1.h"
#include "../../../main/LEOP/LEOP_Fetcher.h"

static const char *TAG = "Weather_UI";

static Weather_UI weather_ui;

/**
 * @brief Creates the hourly weather grid UI.
 *
 * Builds the 24-cell forecast layout on the weather tab page.
 */
void Weather_UI_Initialize()
{
    for (int i = 0; i < 24; i++)
    {
        lv_obj_t *panel = lv_obj_create(ui_TabPage_Weather);

        lv_obj_set_size(panel, 150, 95);

        // Calculate row and column
        int col = i % 6;
        int row = i / 6;

        lv_obj_set_pos(panel,
                       15 + col * (150 + 5),
                       15 + row * (95 + 5));

        weather_ui.hourLabel[i] = lv_label_create(panel);
        weather_ui.tempLabel[i] = lv_label_create(panel);
        weather_ui.iconLabel[i] = lv_label_create(panel);
        weather_ui.weather_code[i] = lv_label_create(panel);

        lv_obj_align(weather_ui.hourLabel[i], LV_ALIGN_TOP_MID, 0, 5);
        lv_obj_align(weather_ui.iconLabel[i], LV_ALIGN_CENTER, 0, 0);
        lv_obj_align(weather_ui.tempLabel[i], LV_ALIGN_BOTTOM_MID, 0, -5);
        lv_obj_align(weather_ui.weather_code[i], LV_ALIGN_BOTTOM_MID, 0, +10);
    }
}

/**
 * @brief Updates the hourly weather grid from the weather queue.
 *
 * Consumes the latest available weather snapshot from the shared queue and
 * refreshes the grid labels with time, temperature, UV index, and weather code
 * values.
 */
void Weather_UI_Update()
{
    static WeatherList weather_list;

    if (weather_queue != NULL && xQueueReceive(weather_queue, &weather_list, 0) == pdPASS)
    {
        if (weather_list.status.weather_fetched)
        {
            ESP_LOGI(TAG, "Should print");
            for (int j = 0; j < 24; j++)
            {
                int i = j * 4;
                int hour, min;

                sscanf(weather_list.weather[i].timestamp, "%*d-%*d-%*dT%d:%d:%*d", &hour, &min);

                min = 0;

                char buf[6];
                snprintf(buf, sizeof(buf), "%02d:00", hour);

                char temp[10];
                snprintf(temp, sizeof(temp), "%.2f", weather_list.weather[i].temp);

                char uv_index[10];
                snprintf(uv_index, sizeof(uv_index), "%d", weather_list.weather[i].uv_index);

                char weather_code[10];
                snprintf(weather_code, sizeof(weather_code), "%d", weather_list.weather[i].weather_code);

                lv_label_set_text(weather_ui.hourLabel[j], buf);
                lv_label_set_text(weather_ui.tempLabel[j], temp);
                lv_label_set_text(weather_ui.iconLabel[j], uv_index);
                lv_label_set_text(weather_ui.weather_code[j], weather_code);
            }
        }
    }
}

static lv_style_t style_card;
static lv_style_t style_title;
static lv_style_t style_temp;
static lv_style_t style_info;
static lv_style_t style_forecast_row;

/**
 * @brief Initializes the LVGL styles used by the weather dashboard.
 */
static void init_styles(void)
{
    lv_style_init(&style_card);

    lv_style_set_radius(&style_card, 24);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, lv_color_hex(0x4d6480));

    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);

    lv_style_set_bg_grad_dir(&style_card, LV_GRAD_DIR_VER);
    lv_style_set_bg_color(&style_card, lv_color_hex(0x143a66));
    lv_style_set_bg_grad_color(&style_card, lv_color_hex(0x0b203f));

    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_opa(&style_card, LV_OPA_30);
    lv_style_set_shadow_color(&style_card, lv_color_black());

    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_white());
    lv_style_set_text_font(&style_title, &lv_font_montserrat_26);

    lv_style_init(&style_temp);
    lv_style_set_text_color(&style_temp, lv_color_white());
    lv_style_set_text_font(&style_temp, &lv_font_montserrat_48);

    lv_style_init(&style_info);
    lv_style_set_text_color(&style_info, lv_color_hex(0xdfe9f5));
    lv_style_set_text_font(&style_info, &lv_font_montserrat_20);

    lv_style_init(&style_forecast_row);
    lv_style_set_bg_opa(&style_forecast_row, LV_OPA_TRANSP);
    lv_style_set_border_side(&style_forecast_row, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&style_forecast_row, lv_color_hex(0x345070));
}

static Weather_UI_test test;

/*********************
 * WEATHER ICON
 *********************/

LV_IMG_DECLARE(icons8_summer_50);
LV_IMG_DECLARE(icons8_partly_cloudy_day_50);
LV_IMG_DECLARE(icons8_clouds_50);
LV_IMG_DECLARE(icons8_rain_50);
LV_IMG_DECLARE(icons8_heavy_rain_50);
LV_IMG_DECLARE(icons8_snow_50);

/**
 * @brief Selects the weather icon image for a forecast code.
 *
 * @param obj LVGL image object to update.
 * @param weather_code Weather code reported by the forecast data.
 */
static void set_icon(lv_obj_t *obj, int weather_code)
{
    switch (weather_code)
    {
    case 0:
        lv_img_set_src(obj, &icons8_summer_50);
        break;
    case 1:
    case 2:
        lv_img_set_src(obj, &icons8_partly_cloudy_day_50);
        break;
    case 3:
    case 45:
    case 48:
        lv_img_set_src(obj, &icons8_clouds_50);
        break;
    case 51:
    case 53:
    case 55:
    case 56:
    case 57:
    case 61:
    case 66:
    case 80:
    case 81:
        lv_img_set_src(obj, &icons8_rain_50);
        break;
    case 63:
    case 65:
    case 67:
    case 82:
    case 95:
    case 96:
    case 99:
        lv_img_set_src(obj, &icons8_heavy_rain_50);
        break;
    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86:
        lv_img_set_src(obj, &icons8_snow_50);
        break;
    default:
        lv_img_set_src(obj, &icons8_clouds_50);
        break;
    }
}


/*********************
 * FORECAST ROW
 *********************/

/**
 * @brief Creates one forecast row widget set.
 *
 * @param parent Parent container for the row.
 *
 * @return The created forecast row widgets.
 */
static forecast_row_t create_forecast_row(lv_obj_t *parent)
{
    forecast_row_t r;

    r.row = lv_obj_create(parent);
    lv_obj_remove_style_all(r.row);
    lv_obj_add_style(r.row, &style_forecast_row, 0);

    lv_obj_set_size(r.row, LV_PCT(100), 45);
    lv_obj_clear_flag(r.row, LV_OBJ_FLAG_SCROLLABLE);

    r.icon = lv_img_create(r.row);
    lv_img_set_zoom(r.icon, 128);
    lv_obj_align(r.icon, LV_ALIGN_LEFT_MID, 10, 0);

    r.temp = lv_label_create(r.row);
    lv_obj_set_style_text_color(r.temp, lv_color_white(), 0);
    lv_obj_set_style_text_font(r.temp, &lv_font_montserrat_24, 0);
    lv_obj_align(r.temp, LV_ALIGN_LEFT_MID, 90, 0);

    r.time = lv_label_create(r.row);
    lv_obj_set_style_text_color(r.time, lv_color_hex(0xc0cfe0), 0);
    lv_obj_align(r.time, LV_ALIGN_RIGHT_MID, -10, 0);

    return r;
}

/*********************
 * MAIN SCREEN
 *********************/

/**
 * @brief Creates the weather dashboard view.
 *
 * Initializes the shared LVGL styles and builds the current-conditions card
 * plus the 24-hour forecast list on the weather tab page.
 */
void weather_dashboard_create(void)
{
    init_styles();

    lv_obj_t *scr = ui_TabPage_Weather;

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x081321), 0);

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /******** LEFT CARD ********/

    test.left_card = lv_obj_create(scr);
    lv_obj_add_style(test.left_card, &style_card, 0);

    lv_obj_set_size(test.left_card, LV_PCT(40), LV_PCT(95));
    lv_obj_set_style_pad_all(test.left_card, 10, 0);

    lv_obj_t *title = lv_label_create(test.left_card);
    lv_obj_add_style(title, &style_title, 0);
    lv_label_set_text(title, "Current weather");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    

    test.icon = lv_img_create(test.left_card);
    lv_img_set_zoom(test.icon, 512);
    lv_obj_align(test.icon, LV_ALIGN_TOP_MID, 0, 120);

    test.current_temp = lv_label_create(test.left_card);
    lv_obj_add_style(test.current_temp, &style_temp, 0);

    lv_label_set_text(test.current_temp, "22°C");
    lv_obj_align(test.current_temp, LV_ALIGN_CENTER, 0, 60);

    test.current_weather = lv_label_create(test.left_card);
    lv_label_set_text(test.current_weather, "Delvis molnigt");

    lv_obj_set_style_text_color(
        test.current_weather,
        lv_color_hex(0xdbe5ef),
        0);

    lv_obj_set_style_text_font(
        test.current_weather,
        &lv_font_montserrat_28,
        0);

    lv_obj_align(test.current_weather, LV_ALIGN_CENTER, 0, 150);

    lv_obj_t *line = lv_obj_create(test.left_card);
    lv_obj_remove_style_all(line);
    lv_obj_set_width(line, LV_PCT(90));
    lv_obj_set_height(line, 1);

    lv_obj_set_style_bg_color(
        line,
        lv_color_hex(0x4a6480),
        0);

    lv_obj_align(line, LV_ALIGN_BOTTOM_MID, 0, -220);

    lv_obj_t *info = lv_obj_create(test.left_card);
    lv_obj_remove_style_all(info);

    lv_obj_set_size(info, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -80);

    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        info,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);

    /******** RIGHT CARD ********/

    test.right_card = lv_obj_create(scr);
    lv_obj_add_style(test.right_card, &style_card, 0);

    lv_obj_set_size(test.right_card, LV_PCT(58), LV_PCT(95));
    lv_obj_set_style_pad_all(test.right_card, 10, 0);

    lv_obj_t *forecast_title = lv_label_create(test.right_card);
    lv_obj_add_style(forecast_title, &style_title, 0);

    lv_label_set_text(
        forecast_title,
        "Forecast 24 hours");

    lv_obj_align(
        forecast_title,
        LV_ALIGN_TOP_LEFT,
        25,
        -2);

    test.forecast_list = lv_obj_create(test.right_card);
    lv_obj_remove_style_all(test.forecast_list);

    lv_obj_set_size(test.forecast_list, LV_PCT(100), LV_PCT(90));
    lv_obj_set_style_pad_row(test.forecast_list, 6, 0);
    lv_obj_align(test.forecast_list, LV_ALIGN_BOTTOM_MID, 0, -20);

    lv_obj_set_flex_flow(test.forecast_list, LV_FLEX_FLOW_COLUMN);

    for (int i = 0; i < 24; i++)
    {
        test.forecast_rows[i] = create_forecast_row(test.forecast_list);
    }
}

/**
 * @brief Returns a short text label for a weather code.
 *
 * @param code Weather code reported by the forecast data.
 *
 * @return Static text label for the weather code.
 */
static const char *weather_code_to_text(int code)
{
    switch (code)
    {
    case 0:
        return "Clear sky";
    case 1:
        return "Mainly clear";
    case 2:
        return "Partly cloudy";
    case 3:
        return "Overcast";
    case 45:
        return "Fog";
    case 48:
        return "Rime fog";
    case 51:
        return "Light drizzle";
    case 53:
        return "Drizzle";
    case 55:
        return "Heavy drizzle";
    case 56:
        return "Light freezing drizzle";
    case 57:
        return "Freezing drizzle";
    case 61:
        return "Light rain";
    case 63:
        return "Rain";
    case 65:
        return "Heavy rain";
    case 66:
        return "Light freezing rain";
    case 67:
        return "Freezing rain";
    case 71:
        return "Light snow";
    case 73:
        return "Snow";
    case 75:
        return "Heavy snow";
    case 77:
        return "Snow grains";
    case 80:
        return "Light rain showers";
    case 81:
        return "Rain showers";
    case 82:
        return "Heavy rain showers";
    case 85:
        return "Light snow showers";
    case 86:
        return "Heavy snow showers";
    case 95:
        return "Thunderstorm";
    case 96:
        return "Thunderstorm with hail";
    case 99:
        return "Heavy thunderstorm with hail";
    default:
        return "Unknown";
    }
}

/**
 * @brief Updates the dashboard view from the weather queue.
 *
 * Consumes the latest available weather snapshot from the shared queue and
 * refreshes the current conditions card and the 24-row forecast list.
 */
void Weather_UI_Update_test()
{
    static WeatherList weather_list;

    if (weather_queue != NULL &&
        xQueueReceive(weather_queue, &weather_list, 0) == pdPASS)
    {
        if (!weather_list.status.weather_fetched)
            return;

        char current_temp[10];
        snprintf(current_temp, sizeof(current_temp), "%.1f °C", weather_list.weather[0].temp);

        lv_label_set_text(test.current_temp, current_temp);
        lv_label_set_text(test.current_weather, weather_code_to_text(weather_list.weather[0].weather_code));

        set_icon(test.icon,
                 weather_list.weather[0].weather_code);

        lv_img_set_zoom(test.icon, 512);

        for (int j = 0; j < 24; j++)
        {
            int i = j * 4;

            int hour, min;
            sscanf(weather_list.weather[i].timestamp,
                   "%*d-%*d-%*dT%d:%d:%*d",
                   &hour, &min);

            char time_buf[6];
            snprintf(time_buf, sizeof(time_buf), "%02d:00", hour);

            char temp_buf[10];
            snprintf(temp_buf, sizeof(temp_buf),
                     "%.1f °C", weather_list.weather[i].temp);

            forecast_row_t *r = &test.forecast_rows[j];

            set_icon(r->icon,
                     weather_list.weather[i].weather_code);

            lv_label_set_text(r->temp, temp_buf);
            lv_label_set_text(r->time, time_buf);
        }
    }
}
