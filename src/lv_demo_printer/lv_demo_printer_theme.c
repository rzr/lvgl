/**
 * @file lv_demo_printer_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_printer.h"
#include "lv_demo_printer_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void theme_apply(lv_obj_t * obj, lv_theme_style_t name);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t theme;
static lv_style_t style_bg;
static lv_style_t style_box;
static lv_style_t style_btn;
static lv_style_t style_title;
static lv_style_t style_back;
static lv_style_t style_icon;
static lv_style_t style_bar_bg;
static lv_style_t style_bar_indic;
static lv_style_t style_scrollbar;
static lv_style_t style_list_btn;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void basic_init(void)
{

    lv_style_init(&style_bg);
    lv_style_set_bg_opa(&style_bg, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg, LV_STATE_DEFAULT, LV_DEMO_PRINTER_LIGHT);
    lv_style_set_text_font(&style_bg, LV_STATE_DEFAULT, theme.font_normal);

    lv_style_init(&style_box);
    lv_style_set_clip_corner(&style_box, LV_STATE_DEFAULT, true);
    lv_style_set_bg_opa(&style_box, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_radius(&style_box, LV_STATE_DEFAULT, 10);
    lv_style_set_value_color(&style_box, LV_STATE_DEFAULT, LV_DEMO_PRINTER_GRAY);
    lv_style_set_value_font(&style_box, LV_STATE_DEFAULT, theme.font_normal);

    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, LV_STATE_DEFAULT, LV_DEMO_PRINTER_WHITE);
    lv_style_set_text_font(&style_title, LV_STATE_DEFAULT, theme.font_title);

    lv_style_init(&style_icon);
    lv_style_set_value_align(&style_icon, LV_STATE_DEFAULT, LV_ALIGN_OUT_BOTTOM_MID);
    lv_style_set_value_ofs_y(&style_icon, LV_STATE_DEFAULT, LV_VER_RES / 30);
    lv_style_set_value_ofs_y(&style_icon, LV_STATE_PRESSED, LV_VER_RES / 20);
    lv_style_set_value_color(&style_icon, LV_STATE_DEFAULT, LV_DEMO_PRINTER_GRAY);
    lv_style_set_value_font(&style_icon, LV_STATE_DEFAULT, theme.font_normal);
    lv_style_set_transform_zoom(&style_icon, LV_STATE_PRESSED, 350);
    lv_style_set_transition_time(&style_icon, LV_STATE_DEFAULT, 250);
    lv_style_set_transition_delay(&style_icon, LV_STATE_PRESSED, 0);
    lv_style_set_transition_delay(&style_icon, LV_STATE_DEFAULT, 150);
    lv_style_set_transition_prop_1(&style_icon, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_ZOOM);
    lv_style_set_transition_prop_2(&style_icon, LV_STATE_DEFAULT, LV_STYLE_VALUE_OFS_Y);

    lv_style_init(&style_bar_bg);
    lv_style_set_radius(&style_bar_bg, LV_STATE_DEFAULT, 10);
    lv_style_set_border_color(&style_bar_bg, LV_STATE_DEFAULT, LV_DEMO_PRINTER_LIGHT_GRAY);
    lv_style_set_border_width(&style_bar_bg, LV_STATE_DEFAULT, 3);
    lv_style_set_border_post(&style_bar_bg, LV_STATE_DEFAULT, true);
    lv_style_set_pad_left(&style_bar_bg, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_right(&style_bar_bg, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_top(&style_bar_bg, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_bottom(&style_bar_bg, LV_STATE_DEFAULT, 2);

    lv_style_init(&style_bar_indic);
    lv_style_set_bg_opa(&style_bar_indic, LV_STATE_DEFAULT, LV_OPA_COVER);

    lv_style_init(&style_scrollbar);
    lv_style_set_bg_opa(&style_scrollbar, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_radius(&style_scrollbar, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_scrollbar, LV_STATE_DEFAULT, LV_DEMO_PRINTER_LIGHT_GRAY);
    lv_style_set_size(&style_scrollbar, LV_STATE_DEFAULT, LV_HOR_RES / 80);
    lv_style_set_pad_right(&style_scrollbar, LV_STATE_DEFAULT, LV_HOR_RES / 60);

    lv_style_init(&style_list_btn);
    lv_style_set_bg_opa(&style_list_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_list_btn, LV_STATE_DEFAULT, LV_DEMO_PRINTER_WHITE);
    lv_style_set_bg_color(&style_list_btn, LV_STATE_PRESSED, LV_DEMO_PRINTER_LIGHT_GRAY);
    lv_style_set_bg_color(&style_list_btn, LV_STATE_CHECKED, LV_DEMO_PRINTER_GREEN);
    lv_style_set_bg_color(&style_list_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_darken(LV_DEMO_PRINTER_GREEN, LV_OPA_20));
    lv_style_set_text_color(&style_list_btn, LV_STATE_DEFAULT, LV_DEMO_PRINTER_GRAY);
    lv_style_set_text_color(&style_list_btn, LV_STATE_PRESSED, lv_color_darken(LV_DEMO_PRINTER_GRAY, LV_OPA_20));
    lv_style_set_text_color(&style_list_btn, LV_STATE_CHECKED, LV_DEMO_PRINTER_WHITE);
    lv_style_set_text_color(&style_list_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, LV_DEMO_PRINTER_WHITE);
    lv_style_set_image_recolor(&style_list_btn, LV_STATE_DEFAULT, LV_DEMO_PRINTER_GRAY);
    lv_style_set_image_recolor(&style_list_btn, LV_STATE_PRESSED, lv_color_darken(LV_DEMO_PRINTER_GRAY, LV_OPA_20));
    lv_style_set_image_recolor(&style_list_btn, LV_STATE_CHECKED, LV_DEMO_PRINTER_WHITE);
    lv_style_set_image_recolor(&style_list_btn, LV_STATE_CHECKED | LV_STATE_PRESSED, LV_DEMO_PRINTER_WHITE);
    lv_style_set_pad_left(&style_list_btn, LV_STATE_DEFAULT, LV_HOR_RES / 25);
    lv_style_set_pad_right(&style_list_btn, LV_STATE_DEFAULT, LV_HOR_RES / 25);
    lv_style_set_pad_top(&style_list_btn, LV_STATE_DEFAULT, LV_HOR_RES / 100);
    lv_style_set_pad_bottom(&style_list_btn, LV_STATE_DEFAULT, LV_HOR_RES / 100);
    lv_style_set_pad_inner(&style_list_btn, LV_STATE_DEFAULT, LV_HOR_RES / 50);
}


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * lv_demo_printer_theme_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                    const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                    const lv_font_t * font_title)
{
    theme.color_primary = color_primary;
    theme.color_secondary = color_secondary;
    theme.font_small = font_small;
    theme.font_normal = font_normal;
    theme.font_subtitle = font_subtitle;
    theme.font_title = font_title;
    theme.flags = flags;

    basic_init();

    theme.apply_xcb = theme_apply;

    return &theme;
}


static void theme_apply(lv_obj_t * obj, lv_theme_style_t name)
{
    lv_style_list_t * list;

    switch(name) {
        case LV_THEME_NONE:
            break;

        case LV_THEME_SCR:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            lv_style_list_add_style(list, &style_bg);
            break;

        case LV_THEME_OBJ:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_OBJ_PART_MAIN);
            lv_style_list_add_style(list, &style_box);
            break;

        case LV_THEME_CONT:
            lv_obj_clean_style_list(obj, LV_OBJ_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_CONT_PART_MAIN);
            lv_style_list_add_style(list, &style_box);
            break;
        case LV_THEME_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            lv_style_list_add_style(list, &style_btn);
            break;

        case LV_THEME_BAR:
            lv_obj_clean_style_list(obj, LV_BAR_PART_BG);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_BG);
            lv_style_list_add_style(list, &style_bar_bg);
            lv_style_list_add_style(list, &style_bar_bg);

            lv_obj_clean_style_list(obj, LV_BAR_PART_INDIC);
            list = lv_obj_get_style_list(obj, LV_BAR_PART_INDIC);
            lv_style_list_add_style(list, &style_bar_indic);
            break;

//        case LV_THEME_SWITCH:
//            lv_obj_clean_style_list(obj, LV_SWITCH_PART_BG);
//            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_BG);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_tight);
//            lv_style_list_add_style(list, &style_round);
//
//            lv_obj_clean_style_list(obj, LV_SWITCH_PART_INDIC);
//            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_INDIC);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_color);
//
//            lv_obj_clean_style_list(obj, LV_SWITCH_PART_KNOB);
//            list = lv_obj_get_style_list(obj, LV_SWITCH_PART_KNOB);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_tight);
//            lv_style_list_add_style(list, &style_round);
//            break;

        case LV_THEME_IMAGE:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            break;

        case LV_DEMO_PRINTER_THEME_ICON:
            lv_obj_clean_style_list(obj, LV_IMG_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_IMG_PART_MAIN);
            lv_style_list_add_style(list, &style_icon);
            break;

        case LV_THEME_LABEL:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            break;

        case LV_DEMO_PRINTER_THEME_TITLE:
            lv_obj_clean_style_list(obj, LV_LABEL_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_LABEL_PART_MAIN);
            lv_style_list_add_style(list, &style_title);
            break;

//        case LV_THEME_ARC:
//            lv_obj_clean_style_list(obj, LV_ARC_PART_BG);
//            list = lv_obj_get_style_list(obj, LV_ARC_PART_BG);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_tick_line);
//            lv_style_list_add_style(list, &style_round);
//
//            lv_obj_clean_style_list(obj, LV_ARC_PART_INDIC);
//            list = lv_obj_get_style_list(obj, LV_ARC_PART_INDIC);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_color);
//            lv_style_list_add_style(list, &style_tick_line);
//            break;

//        case LV_THEME_SLIDER:
//            lv_obj_clean_style_list(obj, LV_SLIDER_PART_BG);
//            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_BG);
//            lv_style_list_add_style(list, &style_bg);
//
//            lv_obj_clean_style_list(obj, LV_SLIDER_PART_INDIC);
//            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_INDIC);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_color);
//
//            lv_obj_clean_style_list(obj, LV_SLIDER_PART_KNOB);
//            list = lv_obj_get_style_list(obj, LV_SLIDER_PART_KNOB);
//            lv_style_list_add_style(list, &style_bg);
//            lv_style_list_add_style(list, &style_round);
//            break;

        case LV_THEME_LIST:
            lv_obj_clean_style_list(obj, LV_LIST_PART_BG);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_BG);
            lv_style_list_add_style(list, &style_box);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLABLE);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLABLE);

            lv_obj_clean_style_list(obj, LV_LIST_PART_SCROLLBAR);
            list = lv_obj_get_style_list(obj, LV_LIST_PART_SCROLLBAR);
            lv_style_list_add_style(list, &style_scrollbar);
            break;

        case LV_THEME_LIST_BTN:
            lv_obj_clean_style_list(obj, LV_BTN_PART_MAIN);
            list = lv_obj_get_style_list(obj, LV_BTN_PART_MAIN);
            lv_style_list_add_style(list, &style_list_btn);
            break;

        default:
            break;
    }


    lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/
