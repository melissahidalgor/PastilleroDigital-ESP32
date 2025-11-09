#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_top
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_top = obj;
            lv_obj_set_pos(obj, -2, 0);
            lv_obj_set_size(obj, 321, 33);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff505050), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_time = obj;
            lv_obj_set_pos(obj, 76, 78);
            lv_obj_set_size(obj, 170, 35);
            lv_label_set_text(obj, "HH:MM:SS");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_buttom
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_buttom = obj;
            lv_obj_set_pos(obj, 9, 154);
            lv_obj_set_size(obj, 302, 49);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffa2a2e7), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_nextDose
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_next_dose = obj;
            lv_obj_set_pos(obj, 21, 168);
            lv_obj_set_size(obj, 280, 21);
            lv_label_set_text(obj, "Medicamento HH:MM");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff030352), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // text_dose
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.text_dose = obj;
            lv_obj_set_pos(obj, 95, 127);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Siguiente dosis:");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff030352), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_date = obj;
            lv_obj_set_pos(obj, 80, 51);
            lv_obj_set_size(obj, 160, 27);
            lv_label_set_text(obj, "aaaa/mm/dd");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // text_wifi
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.text_wifi = obj;
            lv_obj_set_pos(obj, 12, 9);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Wifi ");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_wifiStatus
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_wifi_status = obj;
            lv_obj_set_pos(obj, 51, 9);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "XX");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // text_miPastilelro
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.text_mi_pastilelro = obj;
            lv_obj_set_pos(obj, 150, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Mi Pastillero Digital");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffffdfd), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_ip
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_ip = obj;
            lv_obj_set_pos(obj, 228, 218);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "192.168.172.101");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff080808), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_main() {
}

void create_screen_alarma() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.alarma = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // panel_alarma
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_alarma = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 55);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffc7c7c7), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_alarmaTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_alarma_time = obj;
            lv_obj_set_pos(obj, 75, 11);
            lv_obj_set_size(obj, 170, 35);
            lv_label_set_text(obj, "HH:MM");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // panel_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_center = obj;
            lv_obj_set_pos(obj, 9, 99);
            lv_obj_set_size(obj, 302, 89);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffb7e4ab), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_alarmaDose
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_alarma_dose = obj;
            lv_obj_set_pos(obj, 20, 143);
            lv_obj_set_size(obj, 280, 33);
            lv_label_set_text(obj, "Medicamento");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff030352), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // text_alarmaTomar
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.text_alarma_tomar = obj;
            lv_obj_set_pos(obj, 125, 110);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Tomar");
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff030352), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_alarma() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_alarma();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_alarma,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
