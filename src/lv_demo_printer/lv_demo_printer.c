/**
 * @file lv_demo_priner.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_printer_scr.h"
#include "lv_demo_printer_theme.h"

/*********************
 *      DEFINES
 *********************/
/*Bg positions*/
#define LV_DEMO_PRINTER_BG_NONE (-LV_VER_RES)
#define LV_DEMO_PRINTER_BG_FULL 0
#define LV_DEMO_PRINTER_BG_NORMAL (-2 * (LV_VER_RES / 3))

/*Animations*/
#define LV_DEMO_PRINTER_ANIM_Y (LV_VER_RES / 20)
#define LV_DEMO_PRINTER_ANIM_DELAY (50)
#define LV_DEMO_PRINTER_ANIM_TIME  (200)
#define LV_DEMO_PRINTER_ANIM_TIME_BG  (350)

/*Fonts*/
LV_FONT_DECLARE(lv_font_montserrat_20);
LV_FONT_DECLARE(lv_font_montserrat_28);

/*Padding*/
#define LV_DEMO_PRINTER_TITLE_PAD (LV_VER_RES / 20)


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void home_open(uint32_t delay);
LV_EVENT_CB_DECLARE(home_icon_event_cb);
static void print_open(uint32_t delay);
LV_EVENT_CB_DECLARE(usb_icon_event_cb);
static void usb_open(uint32_t delay);

static lv_obj_t * add_icon(lv_obj_t * parent, const void * src, const char * txt);
static lv_obj_t * add_title(const char * txt);

static void lv_demo_printer_anim_bg(uint32_t delay, lv_color_t color, uint32_t y);
static void lv_demo_printer_anim_in(lv_obj_t * obj, uint32_t delay);
static void lv_demo_printer_anim_out(lv_obj_t * obj, uint32_t delay);
static void lv_demo_printer_anim_out_all(lv_obj_t * obj, uint32_t delay);
static void lv_demo_printer_anim_in_all(lv_obj_t * obj, uint32_t delay);
static void anim_bg_color_cb(lv_anim_t * a, lv_anim_value_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * bg_top;
static lv_obj_t * bg_bottom;
static lv_color_t bg_color_prev;
static lv_color_t bg_color_act;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_printer(void)
{

    bg_color_prev = LV_DEMO_PRINTER_BLUE;
    bg_color_act = LV_DEMO_PRINTER_BLUE;

    lv_theme_t * th = lv_demo_printer_theme_init(LV_COLOR_BLACK, LV_COLOR_BLACK, 0,
            &lv_font_montserrat_14, &lv_font_montserrat_20, &lv_font_montserrat_24, &lv_font_montserrat_28);
    lv_theme_set_act(th);

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    bg_top = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_DEMO_PRINTER_BLUE);
    lv_obj_set_size(bg_top, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_y(bg_top, LV_DEMO_PRINTER_BG_NONE);

    home_open(0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void home_open(uint32_t delay)
{
    lv_obj_t * title = add_title("22 April 2020 15:36");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_RIGHT, -LV_DEMO_PRINTER_TITLE_PAD, LV_DEMO_PRINTER_TITLE_PAD);

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(title, delay);

    lv_coord_t box_w = (LV_HOR_RES * 9) / 10;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, LV_VER_RES / 2);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);

    LV_IMG_DECLARE(lv_demo_printer_img_scan);

    lv_obj_t * icon;

    icon = add_icon(box, &lv_demo_printer_img_scan, "COPY");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 1 * box_w / 8, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    icon = add_icon(box, &lv_demo_printer_img_scan, "SCAN");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 3 * box_w / 8, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    icon = add_icon(box, &lv_demo_printer_img_scan, "PRINT");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 5 * box_w / 8, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    icon = add_icon(box, &lv_demo_printer_img_scan, "SETUP");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 7 * box_w / 8, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(box, delay);


    box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, LV_HOR_RES * 6 / 10, LV_VER_RES / 6);
    lv_obj_align(box, NULL, LV_ALIGN_IN_BOTTOM_LEFT, LV_HOR_RES / 20, - LV_HOR_RES / 40);
    lv_obj_set_style_local_value_str(box, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "What do you want to do today?");

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(box, delay);

    box = lv_obj_create(lv_scr_act(), NULL);
    box_w = LV_HOR_RES / 4;
    lv_obj_set_size(box, LV_HOR_RES / 4, LV_VER_RES / 6);
    lv_obj_align(box, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, - LV_HOR_RES / 20, - LV_HOR_RES / 40);

    lv_obj_t * bar = lv_bar_create(box, NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x00ffff));
    lv_obj_set_size(bar, LV_HOR_RES / 30, LV_VER_RES / 10);
    lv_obj_align_origo(bar, NULL, LV_ALIGN_IN_LEFT_MID, 1 * box_w / 8, 0);
    lv_bar_set_value(bar, 60, LV_ANIM_ON);

    bar = lv_bar_create(box, NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0xff00ff));
    lv_obj_set_size(bar, LV_HOR_RES / 30, LV_VER_RES / 10);
    lv_obj_align_origo(bar, NULL, LV_ALIGN_IN_LEFT_MID, 3 * box_w / 8, 0);
    lv_bar_set_value(bar, 30, LV_ANIM_ON);

    bar = lv_bar_create(box, NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0xffff00));
    lv_obj_set_size(bar, LV_HOR_RES / 30, LV_VER_RES / 10);
    lv_obj_align_origo(bar, NULL, LV_ALIGN_IN_LEFT_MID, 5 * box_w / 8, 0);
    lv_bar_set_value(bar, 80, LV_ANIM_ON);

    bar = lv_bar_create(box, NULL);
    lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_size(bar, LV_HOR_RES / 30, LV_VER_RES / 10);
    lv_obj_align_origo(bar, NULL, LV_ALIGN_IN_LEFT_MID, 7 * box_w / 8, 0);
    lv_bar_set_value(bar, 20, LV_ANIM_ON);

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(box, delay);

    lv_demo_printer_anim_bg(0, LV_DEMO_PRINTER_BLUE, LV_DEMO_PRINTER_BG_NORMAL);
}

LV_EVENT_CB_DECLARE(home_icon_event_cb)
{
    if(e == LV_EVENT_CLICKED) {
       lv_demo_printer_anim_out_all(lv_scr_act(), 0);
       print_open(300);

    }
}


static void print_open(uint32_t delay)
{
    lv_obj_t * title = add_title("PRINT MENU");
    lv_demo_printer_anim_in_all(title, delay);

    lv_coord_t box_w = (LV_HOR_RES * 9) / 10;
    lv_obj_t * box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, LV_VER_RES / 2);
    lv_obj_align(box, NULL, LV_ALIGN_CENTER, 0, 0);

    LV_IMG_DECLARE(lv_demo_printer_img_scan);

    lv_obj_t * icon;

    icon = add_icon(box, &lv_demo_printer_img_scan, "USB");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 1 * box_w / 6, 0);
    lv_obj_set_event_cb(icon, usb_icon_event_cb);

    icon = add_icon(box, &lv_demo_printer_img_scan, "MOBILE");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 3 * box_w / 6, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    icon = add_icon(box, &lv_demo_printer_img_scan, "INTERNET");
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_LEFT_MID, 5 * box_w / 6, 0);
    lv_obj_set_event_cb(icon, home_icon_event_cb);

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(box, delay);

    box = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(box, box_w, LV_VER_RES / 6);
    lv_obj_align(box, NULL, LV_ALIGN_IN_BOTTOM_LEFT, LV_HOR_RES / 20, - LV_HOR_RES / 40);
    lv_obj_set_style_local_value_str(box, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "From where do you want to print?");

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(box, delay);

    lv_demo_printer_anim_bg(0, LV_DEMO_PRINTER_BLUE, LV_DEMO_PRINTER_BG_NORMAL);
}

LV_EVENT_CB_DECLARE(usb_icon_event_cb)
{
    if(e == LV_EVENT_CLICKED) {
       lv_demo_printer_anim_out_all(lv_scr_act(), 0);
       usb_open(300);

    }
}

static void usb_open(uint32_t delay)
{
    lv_obj_t * title = add_title("PRINTING FROM USB DRIVE");
    lv_demo_printer_anim_in_all(title, delay);

    lv_coord_t box_w = (LV_HOR_RES * 5) / 10;
    lv_obj_t * list = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list, box_w, LV_VER_RES / 2);
    lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 0);

    const char * dummy_file_list[] = {
            "File 1",  "File 1",  "File 1",  "File 1",
            "File 1",  "File 1",  "File 1",  "File 1",
            "File 1",  "File 1",  "File 1",  "File 1",
            "File 1",  "File 1",  "File 1",  "File 1",
            "File 1",  "File 1",  "File 1",  "File 10"
    };

    uint32_t i;
    for(i = 0; i < sizeof(dummy_file_list) / sizeof(dummy_file_list[0]); i++) {
        lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "File 1");
        lv_btn_set_checkable(btn, true);
    }

    delay += LV_DEMO_PRINTER_ANIM_DELAY;
    lv_demo_printer_anim_in_all(list, delay);

    lv_demo_printer_anim_bg(0, LV_DEMO_PRINTER_BLUE, LV_DEMO_PRINTER_BG_NORMAL);
}


static lv_obj_t * add_icon(lv_obj_t * parent, const void * src, const char * txt)
{
    lv_obj_t * icon = lv_img_create(parent, NULL);
    lv_theme_apply(icon, LV_DEMO_PRINTER_THEME_ICON);
    lv_img_set_src(icon, src);
    lv_obj_set_click(icon, true);

    lv_obj_set_style_local_value_str(icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, txt);

    return icon;
}


static lv_obj_t * add_title(const char * txt)
{
    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_theme_apply(title, LV_DEMO_PRINTER_THEME_TITLE);
    lv_label_set_text(title, txt);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DEMO_PRINTER_TITLE_PAD);
    return title;
}



static void lv_demo_printer_anim_bg(uint32_t delay, lv_color_t color, uint32_t y)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bg_top);
    lv_anim_set_time(&a, LV_DEMO_PRINTER_ANIM_TIME_BG);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(bg_top), y);
    lv_anim_start(&a);

    bg_color_prev = bg_color_act;
    bg_color_act = color;

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_bg_color_cb);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_start(&a);


}

static void lv_demo_printer_anim_in(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, LV_DEMO_PRINTER_ANIM_TIME);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(obj) - LV_DEMO_PRINTER_ANIM_Y, lv_obj_get_y(obj));
    lv_anim_start(&a);

    lv_obj_fade_in(obj, LV_DEMO_PRINTER_ANIM_TIME, delay);
}

static void lv_demo_printer_anim_out(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, LV_DEMO_PRINTER_ANIM_TIME);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(obj), lv_obj_get_y(obj) + LV_DEMO_PRINTER_ANIM_Y);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

    lv_obj_fade_out(obj, LV_DEMO_PRINTER_ANIM_TIME, delay);
}


static void lv_demo_printer_anim_out_all(lv_obj_t * obj, uint32_t delay)
{
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child) {
        lv_demo_printer_anim_out_all(child, delay);
        delay += LV_DEMO_PRINTER_ANIM_DELAY;
        child = lv_obj_get_child_back(obj, child);
    }

    if(obj != bg_top && obj != bg_bottom && obj != lv_scr_act()) {
        lv_demo_printer_anim_out(obj, delay);
        delay += LV_DEMO_PRINTER_ANIM_DELAY;
    }

}

static void lv_demo_printer_anim_in_all(lv_obj_t * obj, uint32_t delay)
{
    if(obj != bg_top && obj != bg_bottom && obj != lv_scr_act()) {
        lv_demo_printer_anim_in(obj, delay);
        delay += LV_DEMO_PRINTER_ANIM_DELAY;
    }

    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);

    while(child) {
        delay += LV_DEMO_PRINTER_ANIM_DELAY;
        lv_demo_printer_anim_in_all(child, delay);
        child = lv_obj_get_child_back(obj, child);
    }

}

static void anim_bg_color_cb(lv_anim_t * a, lv_anim_value_t v)
{
    lv_color_t c = lv_color_mix(bg_color_act, bg_color_prev, v);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, c);
}

