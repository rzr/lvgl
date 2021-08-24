//
// Created by Mariotaku on 2021/08/21.
//
#include "draw/lv_draw_rect.h"
#include "hal/lv_hal_disp.h"
#include "core/lv_refr.h"
#include "lv_gpu_sdl2_utils.h"
#include "lv_gpu_sdl2_lru.h"
#include "lv_gpu_draw_cache.h"
#include "lv_gpu_sdl2_mask.h"

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t radius;
    lv_coord_t width, height;
} lv_draw_rect_bg_key_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_point_t size;
    lv_coord_t radius;
    lv_coord_t blur;
    lv_point_t offset;
} lv_draw_rect_shadow_key_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t rout, rin;
    lv_coord_t width, height;
    lv_coord_t thickness;
    lv_border_side_t side;
} lv_draw_rect_border_key_t;

static void draw_bg_color(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                          const lv_draw_rect_dsc_t *dsc);

static void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_border(SDL_Renderer *renderer, const lv_area_t *coords,
                        const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_outline(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc);

static void draw_border_generic(const lv_area_t *clip_area, const lv_area_t *outer_area, const lv_area_t *inner_area,
                                lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa,
                                lv_blend_mode_t blend_mode);

static void draw_border_simple(const lv_area_t *clip, const lv_area_t *outer_area, const lv_area_t *inner_area,
                               lv_color_t color, lv_opa_t opa);

LV_ATTRIBUTE_FAST_MEM static void draw_bg_compat(SDL_Renderer *renderer, const lv_area_t *coords,
                                                 const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                                                 const lv_draw_rect_dsc_t *dsc);

void lv_draw_rect(const lv_area_t *coords, const lv_area_t *mask, const lv_draw_rect_dsc_t *dsc) {
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Rect coords_rect, mask_rect, render_rect;
    lv_area_to_sdl_rect(coords, &coords_rect);
    lv_area_to_sdl_rect(mask, &mask_rect);
    SDL_IntersectRect(&coords_rect, &mask_rect, &render_rect);
    if (SDL_RectEmpty(&render_rect)) {
        return;
    }
    if (lv_draw_mask_get_cnt() > 0) {
        draw_bg_compat(renderer, coords, &coords_rect, &mask_rect, dsc);
    } else {
        draw_shadow(renderer, coords, &mask_rect, dsc);
        draw_bg_color(renderer, coords, &mask_rect, dsc);
        draw_bg_img(coords, mask, dsc);
        draw_border(renderer, coords, &coords_rect, &mask_rect, dsc);

        // Outline
        draw_outline(coords, mask, dsc);
    }
}

void draw_bg_color(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                   const lv_draw_rect_dsc_t *dsc) {
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);
    if (dsc->radius > 0) {
        // A small texture with a quarter of the rect is enough
        lv_coord_t bg_w = lv_area_get_width(coords), bg_h = lv_area_get_height(coords);
        lv_coord_t frag_w = LV_MIN(dsc->radius + 1, bg_w / 2), frag_h = LV_MIN(dsc->radius + 1, bg_h / 2);
        uint8_t frag_type = LV_GPU_CACHE_KEY_MAGIC_RECT_BG;
        if (dsc->radius == LV_RADIUS_CIRCLE) {
            if (bg_h > bg_w) {

            } else if (bg_w > bg_h) {

            } else {
                frag_type = LV_GPU_CACHE_KEY_MAGIC_CIRCLE_BG;
                frag_w = frag_h = bg_w;
            }
        }
        lv_draw_rect_bg_key_t key = {
                .magic = frag_type,
                .radius = dsc->radius,
                .width = frag_w,
                .height = frag_h,
        };
        lv_area_t coords_frag;
        lv_area_copy(&coords_frag, coords);
        lv_area_set_width(&coords_frag, frag_w);
        lv_area_set_height(&coords_frag, frag_h);
        SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
        if (texture == NULL) {
            lv_draw_mask_radius_param_t mask_rout_param;
            lv_draw_mask_radius_init(&mask_rout_param, coords, dsc->radius, false);
            int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
            texture = lv_sdl2_gen_mask_texture(renderer, &coords_frag);
            lv_draw_mask_remove_id(mask_rout_id);
            SDL_assert(texture);
            lv_gpu_draw_cache_put(&key, sizeof(key), texture);
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
        SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
        SDL_RenderSetClipRect(renderer, mask_rect);
        if (frag_type == LV_GPU_CACHE_KEY_MAGIC_CIRCLE_BG) {
            SDL_Rect coords_rect;
            lv_area_to_sdl_rect(coords, &coords_rect);
            SDL_RenderCopy(renderer, texture, NULL, &coords_rect);
        } else {
            SDL_Rect tmp_rect;
            lv_area_to_sdl_rect(&coords_frag, &tmp_rect);
            // Upper left
            SDL_RenderCopyEx(renderer, texture, NULL, &tmp_rect, 0, NULL, SDL_FLIP_NONE);
            // Upper right
            tmp_rect.x = coords->x2 - frag_w + 1;
            SDL_RenderCopyEx(renderer, texture, NULL, &tmp_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
            // Lower right
            tmp_rect.y = coords->y2 - frag_h + 1;
            SDL_RenderCopyEx(renderer, texture, NULL, &tmp_rect, 0, NULL, SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            // Lower left
            tmp_rect.x = coords->x1;
            SDL_RenderCopyEx(renderer, texture, NULL, &tmp_rect, 0, NULL, SDL_FLIP_VERTICAL);
            SDL_Rect solid_rect = {.w = 1, .h = 1, .x = frag_w - 1, .y= frag_h - 1};
            tmp_rect.w = bg_w - frag_w * 2;
            tmp_rect.h = bg_h;
            tmp_rect.x = coords->x1 + frag_w;
            tmp_rect.y = coords->y1;
            // Draw middle area
            SDL_RenderCopy(renderer, texture, &solid_rect, &tmp_rect);
            // Size for left/right edges
            tmp_rect.w = frag_w;
            tmp_rect.h = bg_h - frag_h * 2;
            tmp_rect.y = coords->y1 + frag_h;

            // Left edge
            tmp_rect.x = coords->x1;
            SDL_RenderCopy(renderer, texture, &solid_rect, &tmp_rect);
            // Right edge
            tmp_rect.x = coords->x2 - frag_w + 1;
            SDL_RenderCopy(renderer, texture, &solid_rect, &tmp_rect);
        }
    } else {
        SDL_Rect coords_rect;
        lv_area_to_sdl_rect(coords, &coords_rect);
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, dsc->bg_opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_RenderFillRect(renderer, &coords_rect);
    }
}

void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip,
                 const lv_draw_rect_dsc_t *dsc) {
    if (dsc->bg_img_src == NULL) return;
    if (dsc->bg_img_opa <= LV_OPA_MIN) return;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->bg_img_src);
    if (src_type == LV_IMG_SRC_SYMBOL) {
        lv_point_t size;
        lv_txt_get_size(&size, dsc->bg_img_src, dsc->bg_img_symbol_font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        lv_area_t a;
        a.x1 = coords->x1 + lv_area_get_width(coords) / 2 - size.x / 2;
        a.x2 = a.x1 + size.x - 1;
        a.y1 = coords->y1 + lv_area_get_height(coords) / 2 - size.y / 2;
        a.y2 = a.y1 + size.y - 1;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);
        label_draw_dsc.font = dsc->bg_img_symbol_font;
        label_draw_dsc.color = dsc->bg_img_recolor;
        label_draw_dsc.opa = dsc->bg_img_opa;
        lv_draw_label(&a, clip, &label_draw_dsc, dsc->bg_img_src, NULL);
    } else {
        lv_img_header_t header;
        lv_res_t res = lv_img_decoder_get_info(dsc->bg_img_src, &header);
        if (res != LV_RES_OK) {
            LV_LOG_WARN("Coudn't read the background image");
            return;
        }

        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.blend_mode = dsc->blend_mode;
        img_dsc.recolor = dsc->bg_img_recolor;
        img_dsc.recolor_opa = dsc->bg_img_recolor_opa;
        img_dsc.opa = dsc->bg_img_opa;

        /*Center align*/
        if (dsc->bg_img_tiled == false) {
            lv_area_t area;
            area.x1 = coords->x1 + lv_area_get_width(coords) / 2 - header.w / 2;
            area.y1 = coords->y1 + lv_area_get_height(coords) / 2 - header.h / 2;
            area.x2 = area.x1 + header.w - 1;
            area.y2 = area.y1 + header.h - 1;

            lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
        } else {
            lv_area_t area;
            area.y1 = coords->y1;
            area.y2 = area.y1 + header.h - 1;

            for (; area.y1 <= coords->y2; area.y1 += header.h, area.y2 += header.h) {

                area.x1 = coords->x1;
                area.x2 = area.x1 + header.w - 1;
                for (; area.x1 <= coords->x2; area.x1 += header.w, area.x2 += header.w) {
                    lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
                }
            }
        }
    }
}

void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                 const lv_draw_rect_dsc_t *dsc) {
    /*Check whether the shadow is visible*/
    if (dsc->shadow_width == 0) return;
    if (dsc->shadow_opa <= LV_OPA_MIN) return;

    if (dsc->shadow_width == 1 && dsc->shadow_ofs_x == 0 &&
        dsc->shadow_ofs_y == 0 && dsc->shadow_spread <= 0) {
        return;
    }

    int32_t sw = dsc->shadow_width;

    lv_area_t sh_rect_area;
    sh_rect_area.x1 = coords->x1 + dsc->shadow_ofs_x - dsc->shadow_spread;
    sh_rect_area.x2 = coords->x2 + dsc->shadow_ofs_x + dsc->shadow_spread;
    sh_rect_area.y1 = coords->y1 + dsc->shadow_ofs_y - dsc->shadow_spread;
    sh_rect_area.y2 = coords->y2 + dsc->shadow_ofs_y + dsc->shadow_spread;

    lv_area_t sh_area;
    sh_area.x1 = sh_rect_area.x1 - sw / 2 - 1;
    sh_area.x2 = sh_rect_area.x2 + sw / 2 + 1;
    sh_area.y1 = sh_rect_area.y1 - sw / 2 - 1;
    sh_area.y2 = sh_rect_area.y2 + sw / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;

    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    SDL_Rect sh_area_rect;
    lv_area_to_sdl_rect(&sh_area, &sh_area_rect);

    SDL_Color shadow_color;
    lv_color_to_sdl_color(&dsc->shadow_color, &shadow_color);
    uint16_t shadow_radius = dsc->radius;
    lv_draw_rect_shadow_key_t key = {
            .magic = LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW,
            .size= {lv_area_get_width(&sh_area), lv_area_get_height(&sh_area)},
            .radius = shadow_radius,
            .blur = dsc->shadow_width,
            .offset = {dsc->shadow_ofs_x, dsc->shadow_ofs_y}
    };
    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
    if (texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &sh_rect_area, shadow_radius, false);
        int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        lv_opa_t *mask_buf = lv_draw_mask_dump(&sh_area);
        lv_draw_mask_blur(mask_buf, lv_area_get_width(&sh_area), lv_area_get_height(&sh_area), sw / 2 + 1);
        texture = lv_sdl2_create_mask_texture(renderer, mask_buf, lv_area_get_width(&sh_area),
                                              lv_area_get_height(&sh_area));
        lv_mem_buf_release(mask_buf);
        lv_draw_mask_remove_id(mask_rout_id);
        SDL_assert(texture);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, shadow_color.r, shadow_color.g, shadow_color.b);
    SDL_RenderSetClipRect(renderer, mask_rect);
    SDL_RenderCopy(renderer, texture, NULL, &sh_area_rect);
}


void draw_border(SDL_Renderer *renderer, const lv_area_t *coords,
                 const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                 const lv_draw_rect_dsc_t *dsc) {
    if (dsc->border_opa <= LV_OPA_MIN) return;
    if (dsc->border_width == 0) return;
    if (dsc->border_side == LV_BORDER_SIDE_NONE) return;
    if (dsc->border_post) return;

    SDL_Color border_color;
    lv_color_to_sdl_color(&dsc->border_color, &border_color);


    if (dsc->border_side != LV_BORDER_SIDE_FULL) {
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, dsc->border_opa);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int w = 0; w <= dsc->border_width; w++) {
            if (dsc->border_side & LV_BORDER_SIDE_TOP) {
                SDL_RenderDrawLine(renderer, coords->x1, coords->y1 + w, coords->x2, coords->y1 + w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_BOTTOM) {
                SDL_RenderDrawLine(renderer, coords->x1, coords->y2 - w, coords->x2, coords->y2 - w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_LEFT) {
                SDL_RenderDrawLine(renderer, coords->x1 + w, coords->y1, coords->x1 + w, coords->y2);
            }
            if (dsc->border_side & LV_BORDER_SIDE_RIGHT) {
                SDL_RenderDrawLine(renderer, coords->x2 - w, coords->y1, coords->x2 - w, coords->y2);
            }
        }
    } else {
        int32_t coords_w = lv_area_get_width(coords);
        int32_t coords_h = lv_area_get_height(coords);
        int32_t rout = dsc->radius;
        int32_t short_side = LV_MIN(coords_w, coords_h);
        if (rout > short_side >> 1) rout = short_side >> 1;

        /*Get the inner area*/
        lv_area_t area_inner;
        lv_area_copy(&area_inner, coords);
        area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : -(dsc->border_width + rout));

        lv_coord_t rin = rout - dsc->border_width;
        if (rin < 0) rin = 0;
        draw_border_generic(mask_rect, coords, &area_inner, rout, rin, dsc->border_color, dsc->border_opa,
                            dsc->blend_mode);
    }
}

static void draw_outline(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc) {
    if (dsc->outline_opa <= LV_OPA_MIN) return;
    if (dsc->outline_width == 0) return;

    lv_opa_t opa = dsc->outline_opa;

    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get the inner radius*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);

    /*Extend the outline into the background area if it's overlapping the edge*/
    lv_coord_t pad = (dsc->outline_pad == 0 ? (dsc->outline_pad - 1) : dsc->outline_pad);
    area_inner.x1 -= pad;
    area_inner.y1 -= pad;
    area_inner.x2 += pad;
    area_inner.y2 += pad;

    lv_area_t area_outer;
    lv_area_copy(&area_outer, &area_inner);

    area_outer.x1 -= dsc->outline_width;
    area_outer.x2 += dsc->outline_width;
    area_outer.y1 -= dsc->outline_width;
    area_outer.y2 += dsc->outline_width;


    int32_t inner_w = lv_area_get_width(&area_inner);
    int32_t inner_h = lv_area_get_height(&area_inner);
    int32_t rin = dsc->radius;
    int32_t short_side = LV_MIN(inner_w, inner_h);
    if (rin > short_side >> 1) rin = short_side >> 1;

    lv_coord_t rout = rin + dsc->outline_width;

    draw_border_generic(clip, &area_outer, &area_inner, rout, rin, dsc->outline_color, dsc->outline_opa,
                        dsc->blend_mode);
}

void draw_border_generic(const lv_area_t *clip_area, const lv_area_t *outer_area, const lv_area_t *inner_area,
                         lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode) {
    opa = opa >= LV_OPA_COVER ? LV_OPA_COVER : opa;

    if (rout == 0 || rin == 0) {
        draw_border_simple(clip_area, outer_area, inner_area, color, opa);
        return;
    }

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    lv_draw_rect_border_key_t key = {
            .magic = LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER,
            .rout = rout,
            .rin = rin,
            .width = lv_area_get_width(outer_area),
            .height = lv_area_get_width(inner_area),
            .side = LV_BORDER_SIDE_FULL,
            .thickness = inner_area->x1 - outer_area->x1 + 1
    };
    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
    if (texture == NULL) {
        /*Get the real radius*/

        /*Create mask for the outer area*/
        int16_t mask_rout_id = LV_MASK_ID_INV;
        lv_draw_mask_radius_param_t mask_rout_param;
        if (rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, outer_area, rout, false);
            mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        /*Create mask for the inner mask*/
        if (rin < 0) rin = 0;
        lv_draw_mask_radius_param_t mask_rin_param;
        lv_draw_mask_radius_init(&mask_rin_param, inner_area, rin, true);
        int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

        texture = lv_sdl2_gen_mask_texture(renderer, outer_area);

        lv_draw_mask_remove_id(mask_rin_id);
        lv_draw_mask_remove_id(mask_rout_id);
        SDL_assert(texture);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }

    SDL_Rect clip_rect, outer_rect;
    lv_area_to_sdl_rect(clip_area, &clip_rect);
    lv_area_to_sdl_rect(outer_area, &outer_rect);
    SDL_Color color_sdl;
    lv_color_to_sdl_color(&color, &color_sdl);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color_sdl.r, color_sdl.g, color_sdl.b);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, texture, NULL, &outer_rect);
}

static void draw_border_simple(const lv_area_t *clip, const lv_area_t *outer_area, const lv_area_t *inner_area,
                               lv_color_t color, lv_opa_t opa) {

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Color color_sdl;
    lv_color_to_sdl_color(&color, &color_sdl);
    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(clip, &clip_rect);

    SDL_SetRenderDrawColor(renderer, color_sdl.r, color_sdl.g, color_sdl.b, opa);
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect simple_rect;
    simple_rect.w = inner_area->x2 - outer_area->x1 + 1;
    simple_rect.h = inner_area->y2 - outer_area->y1 + 1;
    // Top border
    simple_rect.x = outer_area->x1;
    simple_rect.y = outer_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);
    // Bottom border
    simple_rect.x = inner_area->x1;
    simple_rect.y = inner_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);

    simple_rect.w = inner_area->x1 - outer_area->x1 + 1;
    simple_rect.h = inner_area->y2 - outer_area->y1 + 1;
    // Left border
    simple_rect.x = outer_area->x1;
    simple_rect.y = outer_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);
    // Right border
    simple_rect.x = inner_area->x2;
    simple_rect.y = outer_area->y2;
    SDL_RenderFillRect(renderer, &simple_rect);

}

// Slow draw function
void draw_bg_compat(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect,
                    const SDL_Rect *mask_rect, const lv_draw_rect_dsc_t *dsc) {
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);

    SDL_Surface *indexed = lv_sdl2_apply_mask_surface(coords);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, indexed);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
    SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
    SDL_RenderSetClipRect(renderer, mask_rect);
    SDL_RenderCopy(renderer, texture, NULL, coords_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(indexed);
}
