#include "game.h"

#define GROUND_LEVEL_BASE (TILEMAP_HEIGHT / 3)
#define GROUND_LEVEL_OFFS_LIM 8
#define GROUND_LEVEL_TOP (GROUND_LEVEL_BASE - GROUND_LEVEL_OFFS_LIM)
#define GROUND_LEVEL_BOTTOM (GROUND_LEVEL_BASE + GROUND_LEVEL_OFFS_LIM)
static_assert(GROUND_LEVEL_TOP >= 0 && GROUND_LEVEL_BOTTOM <= TILEMAP_HEIGHT, "Tilemap ground level range goes out of bounds!");
#define GROUND_LEVEL_OFFS_VAR 0.4f
static_assert(GROUND_LEVEL_OFFS_VAR >= 0.0f && GROUND_LEVEL_OFFS_VAR <= 1.0f, "Variance is out of range!");

static void GenWorldTilemapGround(s_tilemap_core* const tm_core) {
    assert(tm_core);

    int level = GROUND_LEVEL_BASE + ZFW_RandRangeI(-GROUND_LEVEL_OFFS_LIM, GROUND_LEVEL_OFFS_LIM);

    for (int tx = 0; tx < TILEMAP_WIDTH; tx++) {
        const int grass_depth = ZFW_RandRangeI(2, 4);
        const int dirt_depth = ZFW_RandRangeI(11, 15);
        assert(grass_depth < dirt_depth && level + dirt_depth <= TILEMAP_HEIGHT);

        for (int ty = level; ty < level + grass_depth; ty++) {
            AddTile(tm_core, (zfw_s_vec_2d_s32){tx, ty}, ek_tile_type_grass);
        }

        for (int ty = level + grass_depth; ty < level + dirt_depth; ty++) {
            AddTile(tm_core, (zfw_s_vec_2d_s32){tx, ty}, ek_tile_type_dirt);
        }

        for (int ty = level + dirt_depth; ty < TILEMAP_HEIGHT; ty++) {
            AddTile(tm_core, (zfw_s_vec_2d_s32){tx, ty}, ek_tile_type_stone);
        }

        if (ZFW_RandPerc() < GROUND_LEVEL_OFFS_VAR) {
            if (ZFW_RandPerc() < 0.5f) {
                level++;
            } else {
                level--;
            }

            level = ZFW_CLAMP(level, GROUND_LEVEL_TOP, GROUND_LEVEL_BOTTOM - 1);
        }
    }
}

void GenWorld(s_world_core* const world_core) {
    assert(world_core && IS_ZERO(*world_core));

    world_core->player_hp_max = PLAYER_INIT_HP_MAX;

    GenWorldTilemapGround(&world_core->tilemap_core);
}
