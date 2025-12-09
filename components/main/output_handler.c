// components/main/output_handler.c
#include "lvgl.h"
#include <stdio.h>

// Styles for backgrounds
static lv_style_t style_bg_red;
static lv_style_t style_bg_green;
static lv_style_t style_bg_white;

// Label and screen (kept static across calls)
static lv_obj_t *label = NULL;
static lv_obj_t *screen = NULL;

// Initialize styles (call once at startup)
void setup_styles(void)
{
    // Red
    lv_style_init(&style_bg_red);
    lv_style_set_bg_color(&style_bg_red, lv_color_hex(0xFF0000));
    lv_style_set_bg_opa(&style_bg_red, LV_OPA_COVER);

    // Green
    lv_style_init(&style_bg_green);
    lv_style_set_bg_color(&style_bg_green, lv_color_hex(0x00FF00));
    lv_style_set_bg_opa(&style_bg_green, LV_OPA_COVER);

    // White
    lv_style_init(&style_bg_white);
    lv_style_set_bg_color(&style_bg_white, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&style_bg_white, LV_OPA_COVER);
}

// Helper: remove all background styles we might add
static void remove_all_bg_styles(lv_obj_t *scr)
{
    if (scr == NULL) return;
    lv_obj_remove_style(scr, &style_bg_red, LV_PART_MAIN);
    lv_obj_remove_style(scr, &style_bg_green, LV_PART_MAIN);
    lv_obj_remove_style(scr, &style_bg_white, LV_PART_MAIN);
}

// Main entry: called with model outputs
void HandleOutput(float x_value, float y_value)
{
    // Ensure styles initialized; if not, initialize them now.
    // This is safe to call repeatedly.
    static bool styles_setup = false;
    if (!styles_setup) {
        setup_styles();
        styles_setup = true;
    }

    // Get active screen
    screen = lv_scr_act();
    if (screen == NULL) {
        // fall back to serial print if screen unavailable
        printf("HandleOutput: no active screen\n");
        return;
    }

    // Create label once
    if (label == NULL) {
        label = lv_label_create(screen);
        if (label == NULL) {
            printf("HandleOutput: failed to create label\n");
            return;
        }
        // set width, wrap and font
        lv_obj_set_width(label, LV_HOR_RES - 20);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_recolor(label, true);

        static lv_style_t style_label;
        lv_style_init(&style_label);
        lv_style_set_text_font(&style_label, &lv_font_montserrat_22); // ensure this font is available
        lv_style_set_text_align(&style_label, LV_TEXT_ALIGN_CENTER);
        lv_obj_add_style(label, &style_label, 0);
    }

    // Format and set text
    char display_str[128];
    // show x and y with reasonable precision
    snprintf(display_str, sizeof(display_str), "x: %.3f\ny: %.3f", (double)x_value, (double)y_value);
    lv_label_set_text(label, display_str);

    // Center the label
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Remove previous background styles
    remove_all_bg_styles(screen);

    // Add appropriate background style depending on y_value:
    // > 0.5 -> green
    // < -0.5 -> red
    // else -> white
    if (y_value > 0.5f) {
        lv_obj_add_style(screen, &style_bg_green, LV_PART_MAIN);
    } else if (y_value < -0.5f) {
        lv_obj_add_style(screen, &style_bg_red, LV_PART_MAIN);
    } else {
        lv_obj_add_style(screen, &style_bg_white, LV_PART_MAIN);
    }

    // Ask LVGL to refresh (typical pattern)
    lv_task_handler();

    // Also print serial for submission evidence
    printf("HandleOutput - x_value: %.6f, y_value: %.6f\n", (double)x_value, (double)y_value);
}
