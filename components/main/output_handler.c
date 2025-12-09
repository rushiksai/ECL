// components/main/output_handler.c
#include "lvgl.h"
#include <stdio.h>
#include <math.h>

// Forward declaration
void setup_styles(void);

// Static LVGL objects & styles
static lv_obj_t *label = NULL;
static lv_style_t style_text;
static lv_style_t style_bg_red;
static lv_style_t style_bg_green;
static lv_style_t style_bg_white;
static bool styles_initialized = false;

// Initialize styles (call once)
void setup_styles(void)
{
    if (styles_initialized) return;
    styles_initialized = true;

    // Text style
    lv_style_init(&style_text);
    lv_style_set_text_font(&style_text, &lv_font_montserrat_22);
    lv_style_set_text_align(&style_text, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_color(&style_text, lv_color_hex(0x000000)); // black text

    // Red background
    lv_style_init(&style_bg_red);
    lv_style_set_bg_opa(&style_bg_red, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_red, lv_color_hex(0xFF0000)); // red

    // Green background
    lv_style_init(&style_bg_green);
    lv_style_set_bg_opa(&style_bg_green, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_green, lv_color_hex(0x00FF00)); // green

    // White background
    lv_style_init(&style_bg_white);
    lv_style_set_bg_opa(&style_bg_white, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_white, lv_color_hex(0xFFFFFF)); // white
}

// HandleOutput: display the y_value and change background color per thresholds
void HandleOutput(float x_value, float y_value)
{
    // Ensure LVGL styles are ready
    setup_styles();

    lv_obj_t *screen = lv_scr_act();
    if (screen == NULL) {
        printf("Error: lv_scr_act() returned NULL\n");
        return;
    }

    // Create label if not created yet
    if (label == NULL) {
        label = lv_label_create(screen);
        if (label == NULL) {
            printf("Error: Failed to create label\n");
            return;
        }
        lv_obj_set_width(label, LV_HOR_RES - 20);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_recolor(label, true);
        lv_obj_add_style(label, &style_text, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    // Format text
    char display_str[128];
    // show x and y to help debug, format with limited decimals
    snprintf(display_str, sizeof(display_str), "x: %.3f\ny: %.3f", (double)x_value, (double)y_value);
    lv_label_set_text(label, display_str);

    // Remove any of our background styles first (safe remove)
    lv_obj_remove_style(screen, &style_bg_red, LV_PART_MAIN);
    lv_obj_remove_style(screen, &style_bg_green, LV_PART_MAIN);
    lv_obj_remove_style(screen, &style_bg_white, LV_PART_MAIN);

    // Add the chosen background style based on y_value
    if (y_value > 0.5f) {
        lv_obj_add_style(screen, &style_bg_green, LV_PART_MAIN);
    } else if (y_value < -0.5f) {
        lv_obj_add_style(screen, &style_bg_red, LV_PART_MAIN);
    } else {
        lv_obj_add_style(screen, &style_bg_white, LV_PART_MAIN);
    }

    // Force refresh — LVGL usually runs in a loop; this helps if user code isn't calling it elsewhere.
    lv_task_handler();
}
