// components/main/output_handler.c
#include "lvgl.h"
#include <stdio.h>

// If your project has a MicroPrintf function, declare it here.
// If not, you can remove the MicroPrintf lines.
extern void MicroPrintf(const char *fmt, ...);

// Styles (file-scope)
static lv_style_t style_bg_red;
static lv_style_t style_bg_green;
static lv_style_t style_bg_white;

// Label and screen
static lv_obj_t* label = NULL;
static lv_obj_t* screen = NULL;
static int styles_ready = 0;

// Initialize styles (call once)
static void setup_styles(void)
{
    if (styles_ready) return;
    styles_ready = 1;

    lv_style_init(&style_bg_red);
    lv_style_set_bg_opa(&style_bg_red, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_red, lv_color_hex(0xFF0000));

    lv_style_init(&style_bg_green);
    lv_style_set_bg_opa(&style_bg_green, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_green, lv_color_hex(0x00FF00));

    lv_style_init(&style_bg_white);
    lv_style_set_bg_opa(&style_bg_white, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg_white, lv_color_hex(0xFFFFFF));
}

// Exported function called from main.c
void HandleOutput(float x_value, float y_value)
{
    setup_styles();

    screen = lv_scr_act();
    if (screen == NULL) {
        MicroPrintf ? MicroPrintf("Error: Active screen is null.\n") : 0;
        return;
    }

    if (label == NULL) {
        label = lv_label_create(screen);
        if (label == NULL) {
            MicroPrintf ? MicroPrintf("Error: Failed to create label.\n") : 0;
            return;
        }
        lv_obj_set_width(label, LV_HOR_RES - 20);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_recolor(label, true);

        static lv_style_t style;
        lv_style_init(&style);
        lv_style_set_text_font(&style, &lv_font_montserrat_22);
        lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
        lv_obj_add_style(label, &style, 0);
    }

    char display_str[128];
    snprintf(display_str, sizeof(display_str), "Output: %.4f", y_value);
    lv_label_set_text(label, display_str);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Remove any of the three styles (safe)
    lv_obj_remove_style(screen, &style_bg_red, LV_PART_MAIN);
    lv_obj_remove_style(screen, &style_bg_green, LV_PART_MAIN);
    lv_obj_remove_style(screen, &style_bg_white, LV_PART_MAIN);

    // Decide which background to add
    if (y_value > 0.5f) {
        lv_obj_add_style(screen, &style_bg_green, LV_PART_MAIN);
    } else if (y_value < -0.5f) {
        lv_obj_add_style(screen, &style_bg_red, LV_PART_MAIN);
    } else {
        lv_obj_add_style(screen, &style_bg_white, LV_PART_MAIN);
    }

    // Refresh the display
    lv_task_handler();

    // Log
    MicroPrintf ? MicroPrintf("x_value: %f, y_value: %f\n", (double)x_value, (double)y_value) : 0;
}
