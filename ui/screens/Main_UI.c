/**
 * @file Main_UI.c
 * @brief Implementation of the main LVGL screen setup.
 *
 * @ingroup UI
 */

#include "../ui.h"
#include "Main_UI.h"

lv_obj_t *ui_Screen1 = NULL;
lv_obj_t *ui_Glennergy_Label = NULL;
lv_obj_t *ui_Tab_Main = NULL;
lv_obj_t *ui_TabPage_Home = NULL;
lv_obj_t *ui_TabPage_Electricity = NULL;
lv_obj_t *ui_Chart_Electricity = NULL;
lv_obj_t *ui_TabPage_Weather = NULL;
lv_obj_t *ui_Chart_Weather = NULL;
lv_obj_t *ui_Group_Settings = NULL;
lv_obj_t *ui_TabPage_WiFi = NULL;
lv_obj_t *ui_LEOP_Label = NULL;
lv_obj_t *ui_LEOP_Connected_Label = NULL;
lv_obj_t *ui_TabPage_Settings = NULL;

/**
 * @brief Implementation of Main_UI_Initialize.
 *
 * See header for full contract documentation.
 */
void Main_UI_Initialize()
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    ui_Glennergy_Label = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Glennergy_Label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_Glennergy_Label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_Glennergy_Label, -6);
    lv_obj_set_y(ui_Glennergy_Label, -233);
    lv_obj_set_align(ui_Glennergy_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Glennergy_Label, "GLENNERGY");
    lv_obj_set_style_text_letter_space(ui_Glennergy_Label, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui_Glennergy_Label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Glennergy_Label, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Glennergy_Label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LEOP_Label = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_LEOP_Label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LEOP_Label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LEOP_Label, 290);
    lv_obj_set_y(ui_LEOP_Label, -232);
    lv_obj_set_align(ui_LEOP_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LEOP_Label, "LEOP");
    lv_obj_set_style_text_letter_space(ui_LEOP_Label, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui_LEOP_Label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_LEOP_Label, LV_TEXT_ALIGN_AUTO, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LEOP_Connected_Label = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_LEOP_Connected_Label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LEOP_Connected_Label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LEOP_Connected_Label, 395);
    lv_obj_set_y(ui_LEOP_Connected_Label, -231);
    lv_obj_set_align(ui_LEOP_Connected_Label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LEOP_Connected_Label, "Checking...");
    lv_obj_set_style_text_color(ui_LEOP_Connected_Label, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LEOP_Connected_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);


    ui_Tab_Main = lv_tabview_create(ui_Screen1, LV_DIR_TOP, 50);
    lv_obj_set_width(ui_Tab_Main, 959);
    lv_obj_set_height(ui_Tab_Main, 470);
    lv_obj_set_x(ui_Tab_Main, 0);
    lv_obj_set_y(ui_Tab_Main, 36);
    lv_obj_set_align(ui_Tab_Main, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Tab_Main, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_MOMENTUM); /// Flags
    lv_obj_set_style_bg_color(ui_Tab_Main, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(ui_Tab_Main, 255, LV_PART_MAIN | LV_STATE_FOCUSED);

    lv_obj_set_style_bg_color(lv_tabview_get_tab_btns(ui_Tab_Main), lv_color_hex(0x975EED),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lv_tabview_get_tab_btns(ui_Tab_Main), 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(lv_tabview_get_tab_btns(ui_Tab_Main), lv_color_hex(0x4B3B5E),
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_tabview_get_tab_btns(ui_Tab_Main), LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(lv_tabview_get_tab_btns(ui_Tab_Main), lv_color_hex(0x6D2AB2),
                              LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lv_tabview_get_tab_btns(ui_Tab_Main), 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(lv_tabview_get_tab_btns(ui_Tab_Main), lv_color_hex(0x000000),
                                   LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(lv_tabview_get_tab_btns(ui_Tab_Main), LV_GRAD_DIR_VER, LV_PART_ITEMS | LV_STATE_DEFAULT);

    ui_TabPage_Home = lv_tabview_add_tab(ui_Tab_Main, "HOME");
    lv_obj_set_style_bg_color(ui_TabPage_Home, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TabPage_Home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_TabPage_Home, lv_color_hex(0x301E4D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_TabPage_Home, 254, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_TabPage_Home, 999, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_TabPage_Home, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_TabPage_Home, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_TabPage_Home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_TabPage_Home, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_TabPage_Home, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_TabPage_Home, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_TabPage_Home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TabPage_Electricity = lv_tabview_add_tab(ui_Tab_Main, "ELECTRICITY");
    lv_obj_set_style_bg_color(ui_TabPage_Electricity, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TabPage_Electricity, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor(ui_TabPage_Electricity, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui_TabPage_Electricity, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TabPage_Weather = lv_tabview_add_tab(ui_Tab_Main, "WEATHER");
    lv_obj_set_style_bg_color(ui_TabPage_Weather, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TabPage_Weather, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_TabPage_Weather, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_pad_all(ui_TabPage_Weather, 0, 0);
    lv_obj_set_style_border_width(ui_TabPage_Weather, 0, 0);


    ui_TabPage_WiFi = lv_tabview_add_tab(ui_Tab_Main, "WIFI");
    lv_obj_set_style_bg_color(ui_TabPage_WiFi, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TabPage_WiFi, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Group_Settings = lv_obj_create(ui_TabPage_WiFi);

    lv_obj_set_width(ui_Group_Settings, 404);
    lv_obj_set_height(ui_Group_Settings, 380);
    lv_obj_set_x(ui_Group_Settings, -263);
    lv_obj_set_y(ui_Group_Settings, -5);
    lv_obj_set_align(ui_Group_Settings, LV_ALIGN_CENTER);

    // Match your "non-scrollable" behavior
    lv_obj_clear_flag(ui_Group_Settings,
                      LV_OBJ_FLAG_SCROLLABLE |
                          LV_OBJ_FLAG_SCROLL_ELASTIC |
                          LV_OBJ_FLAG_SCROLL_MOMENTUM |
                          LV_OBJ_FLAG_SCROLL_CHAIN);

    // Style (based on your tab button style)
    lv_obj_set_style_bg_color(ui_Group_Settings, lv_color_hex(0x6E10CE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Group_Settings, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Group_Settings, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Group_Settings, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Optional: cleaner edges (tabview had implicit styling)
    lv_obj_set_style_radius(ui_Group_Settings, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Group_Settings, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Group_Settings, lv_color_hex(0x6E10CE), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TabPage_Settings = lv_tabview_add_tab(ui_Tab_Main, "SETTINGS");
    lv_obj_set_style_bg_color(ui_TabPage_Settings, lv_color_hex(0x1E1425), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TabPage_Settings, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}
