#include "game.h"

#define BUTTON_FONT ek_font_eb_garamond_32
#define BUTTON_COLOR WHITE
#define BUTTON_COLOR_INACTIVE GRAY
#define BUTTON_COLOR_HOVER YELLOW

static inline bool IsButtonsValid(const s_buttons* const btns) {
    assert(btns);

    if (IS_ZERO(*btns)) {
        return true;
    }

    return btns->buf && btns->cnt > 0;
}

s_button* GetButton(s_buttons* const btns, const int index) {
    assert(btns);
    assert(index >= 0 && index < btns->cnt);

    return &btns->buf[index];
}

s_button* GetButtonConst(const s_buttons* const btns, const int index) {
    assert(btns);
    assert(index >= 0 && index < btns->cnt);

    return &btns->buf[index];
}

static inline bool LoadButtonCollider(s_rect* const collider, const s_button* const btn, const s_fonts* const fonts, s_mem_arena* const temp_mem_arena) {
    assert(collider && IS_ZERO(*collider));
    assert(btn);

    return LoadStrCollider(collider, btn->str, BUTTON_FONT, fonts, btn->pos, ek_str_hor_align_center, ek_str_ver_align_center, temp_mem_arena);
}

bool LoadIndexOfFirstButtonContainingPoint(int* const index, const s_buttons* const btns, const s_vec_2d pt, const s_fonts* const fonts, s_mem_arena* const temp_mem_arena) {
    assert(index);
    assert(btns && IsButtonsValid(btns));

    *index = -1;

    for (int i = 0; i < btns->cnt; i++) {
        const s_button* const btn = GetButtonConst(btns, i);

        if (btn->inactive) {
            continue;
        }

        s_rect btn_str_collider = {0};

        if (!LoadButtonCollider(&btn_str_collider, btn, fonts, temp_mem_arena)) {
            return false;
        }

        if (IsPointInRect(pt, btn_str_collider)) {
            *index = i;
            break;
        }
    }

    return true;
}

bool RenderButton(const s_rendering_context* const rendering_context, const s_button* const btn, const bool hovered, const s_fonts* const fonts, s_mem_arena* const temp_mem_arena) {
    const s_color color = btn->inactive ? GRAY : (hovered ? YELLOW : WHITE);
    return RenderStr(rendering_context, btn->str, BUTTON_FONT, fonts, btn->pos, ek_str_hor_align_center, ek_str_ver_align_center, color, temp_mem_arena);
}
