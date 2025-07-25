#ifndef ASSETS_H
#define ASSETS_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <zfw.h>

typedef enum {
    ek_texture_player,
    ek_texture_npcs,
    ek_texture_tiles,
    ek_texture_item_icons,
    ek_texture_projectiles,
    ek_texture_particles,
    ek_texture_misc,

    eks_texture_cnt
} e_texture;

static const char* TextureIndexToFilePath(const int index) {
    switch ((e_texture)index) {
        case ek_texture_player: return "assets/textures/player.png";
        case ek_texture_npcs: return "assets/textures/npcs.png";
        case ek_texture_tiles: return "assets/textures/tiles.png";
        case ek_texture_item_icons: return "assets/textures/item_icons.png";
        case ek_texture_projectiles: return "assets/textures/projectiles.png";
        case ek_texture_particles: return "assets/textures/particles.png";
        case ek_texture_misc: return "assets/textures/misc.png";

        default:
            assert(false && "Texture case not handled!");
            return NULL;
    }
}

typedef enum {
    ek_font_eb_garamond_20,
    ek_font_eb_garamond_24,
    ek_font_eb_garamond_28,
    ek_font_eb_garamond_32,
    ek_font_eb_garamond_48,
    ek_font_eb_garamond_80,

    eks_font_cnt
} e_font;

static zfw_s_font_load_info FontIndexToLoadInfo(const int index) {
    switch ((e_font)index) {
        case ek_font_eb_garamond_20:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 20
            };

        case ek_font_eb_garamond_24:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 24
            };

        case ek_font_eb_garamond_28:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 28
            };

        case ek_font_eb_garamond_32:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 32
            };

        case ek_font_eb_garamond_48:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 48
            };

        case ek_font_eb_garamond_80:
            return (zfw_s_font_load_info){
                .file_path = "assets/fonts/eb_garamond.ttf",
                .height = 80
            };

        default:
            assert(false && "Font case not handled!");
            return (zfw_s_font_load_info){0};
    }
}

typedef enum {
    ek_sound_type_button_click,
    ek_sound_type_item_drop_collect,

    eks_sound_type_cnt
} e_sound_type;

#endif
