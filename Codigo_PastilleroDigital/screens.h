#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *alarma;
    lv_obj_t *label_alarma_dose;
    lv_obj_t *label_alarma_time;
    lv_obj_t *label_date;
    lv_obj_t *label_ip;
    lv_obj_t *label_next_dose;
    lv_obj_t *label_time;
    lv_obj_t *label_wifi_status;
    lv_obj_t *panel_alarma;
    lv_obj_t *panel_buttom;
    lv_obj_t *panel_center;
    lv_obj_t *panel_top;
    lv_obj_t *text_alarma_tomar;
    lv_obj_t *text_dose;
    lv_obj_t *text_mi_pastilelro;
    lv_obj_t *text_wifi;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_ALARMA = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_alarma();
void tick_screen_alarma();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/