#include "game.h"

#define TILE_PLACE_DEFAULT_USE_BREAK 2

const s_item_type g_item_types[] = {
    [ek_item_type_dirt_block] = {
        .name = "Dirt Block",
        .icon_spr = ek_sprite_dirt_block_item_icon,
        .use_type = ek_item_use_type_tile_place,
        .use_break = TILE_PLACE_DEFAULT_USE_BREAK,
        .consume_on_use = true,
        .tile_place_type = ek_tile_type_dirt
    },
    [ek_item_type_stone_block] = {
        .name = "Stone Block",
        .icon_spr = ek_sprite_stone_block_item_icon,
        .use_type = ek_item_use_type_tile_place,
        .use_break = TILE_PLACE_DEFAULT_USE_BREAK,
        .consume_on_use = true,
        .tile_place_type = ek_tile_type_stone
    },
    [ek_item_type_grass_block] = {
        .name = "Grass Block",
        .icon_spr = ek_sprite_grass_block_item_icon,
        .use_type = ek_item_use_type_tile_place,
        .use_break = TILE_PLACE_DEFAULT_USE_BREAK,
        .consume_on_use = true,
        .tile_place_type = ek_tile_type_grass
    },
    [ek_item_type_copper_pickaxe] = {
        .name = "Copper Pickaxe",
        .icon_spr = ek_sprite_copper_pickaxe_item_icon,
        .use_type = ek_item_use_type_tile_hurt,
        .use_break = 10,
        .tile_hurt_dist = 4
    },
    [ek_item_type_wooden_sword] = {
        .name = "Wooden Sword",
        .icon_spr = ek_sprite_item_icon_template,
        .use_type = ek_item_use_type_tile_place,
        .use_break = 10
    },
    [ek_item_type_wooden_bow] = {
        .name = "Wooden Bow",
        .icon_spr = ek_sprite_item_icon_template,
        .use_type = ek_item_use_type_shoot,
        .use_break = 10,
        .shoot_proj_type = ek_projectile_type_wooden_arrow,
        .shoot_proj_spd = 7.0f,
        .shoot_proj_dmg = 3
    }
};

STATIC_ARRAY_LEN_CHECK(g_item_types, eks_item_type_cnt);

bool IsItemUsable(const e_item_type item_type, const s_world* const world, const zfw_s_vec_2d_s32 mouse_tile_pos) {
    const zfw_s_vec_2d_s32 player_tile_pos = CameraToTilePos(world->player.pos);
    const int player_to_mouse_tile_dist = TileDist(player_tile_pos, mouse_tile_pos);

    switch (g_item_types[item_type].use_type) {
        case ek_item_use_type_tile_place:
            if (!IsTilePosInBounds(mouse_tile_pos)
                || IsTileActive(&world->core.tilemap_core.activity, mouse_tile_pos)
                || !IsTilePosFree(world, mouse_tile_pos)) {
                return false;
            }

            return player_to_mouse_tile_dist <= TILE_PLACE_DIST;

        case ek_item_use_type_tile_hurt:
            if (!IsTilePosInBounds(mouse_tile_pos) || !IsTileActive(&world->core.tilemap_core.activity, mouse_tile_pos)) {
                return false;
            }

            return player_to_mouse_tile_dist <= g_item_types[item_type].tile_hurt_dist;

        case ek_item_use_type_shoot:
            return true;

        default:
            assert(false && "Unhandled switch case!");
            return false;
   }
}

bool ProcItemUsage(s_world* const world, const zfw_s_input_state* const input_state, const zfw_s_vec_2d_s32 display_size) {
    if (world->player.item_use_break > 0) {
        world->player.item_use_break--;
        return true;
    }

    if (world->player_inv_open) {
        return true;
    }

    s_inventory_slot* const slot = &world->player_inv_slots[0][world->player_inv_hotbar_slot_selected];

    if (slot->quantity == 0) {
        // Selected slot is empty, no item to use.
        return true;
    }

    const zfw_s_vec_2d mouse_cam_pos = DisplayToCameraPos(input_state->mouse_pos, &world->cam, display_size);
    const zfw_s_vec_2d_s32 mouse_tile_pos = CameraToTilePos(mouse_cam_pos);

    if (!ZFW_IsMouseButtonDown(zfw_ek_mouse_button_code_left, input_state)
        || !IsItemUsable(slot->item_type, world, mouse_tile_pos)) {
        return true;
    }

    const s_item_type* const item_type = &g_item_types[slot->item_type];

    switch (item_type->use_type) {
        case ek_item_use_type_tile_place:
            PlaceWorldTile(world, mouse_tile_pos, item_type->tile_place_type);
            break;

        case ek_item_use_type_tile_hurt:
            if (!HurtWorldTile(world, mouse_tile_pos)) {
                return false;
            }

            break;

        case ek_item_use_type_shoot:
            {
                const zfw_s_vec_2d dir = ZFW_Vec2DDir(world->player.pos, mouse_cam_pos);
                const zfw_s_vec_2d vel = ZFW_Vec2DScaled(dir, item_type->shoot_proj_spd);

                if (!SpawnProjectile(world, item_type->shoot_proj_type, true, item_type->shoot_proj_dmg, world->player.pos, vel)) {
                    return false;
                }
            }

            break;
    }

    world->player.item_use_break = item_type->use_break;

    if (item_type->consume_on_use) {
        slot->quantity--;
    }

    return true;
}

bool SpawnItemDrop(s_world* const world, const zfw_s_vec_2d pos, const e_item_type item_type, const int item_quantity) {
    assert(world);
    assert(item_quantity > 0);

    if (world->item_drop_active_cnt == ITEM_DROP_LIMIT) {
        return false;
    }

    s_item_drop* const drop = &world->item_drops[world->item_drop_active_cnt];
    assert(IS_ZERO(*drop));
    drop->item_type = item_type;
    drop->quantity = item_quantity;
    drop->pos = pos;

    world->item_drop_active_cnt++;

    return true;
}

bool UpdateItemDrops(s_world* const world, zfw_s_audio_sys* const audio_sys, const zfw_s_sound_types* const snd_types, const t_settings* const settings) {
    assert(world);

    bool collected = false; // Was an item drop collected?

    const zfw_s_rect player_collider = PlayerCollider(world->player.pos);

    for (int i = 0; i < world->item_drop_active_cnt; i++) {
        s_item_drop* const drop = &world->item_drops[i];

        // Process movement.
        drop->vel.y += GRAVITY;

        ProcVerTileCollisions(&drop->pos, &drop->vel.y, ItemDropColliderSize(drop->item_type), ITEM_DROP_ORIGIN, &world->core.tilemap_core.activity);

        drop->pos = ZFW_Vec2DSum(drop->pos, drop->vel);

        // Process collection.
        const bool collectable = DoesInventoryHaveRoomFor((s_inventory_slot*)world->player_inv_slots, PLAYER_INVENTORY_LEN, drop->item_type, drop->quantity);

        if (collectable) {
            const zfw_s_rect drop_collider = ItemDropCollider(drop->pos, drop->item_type);

            if (ZFW_DoRectsInters(player_collider, drop_collider)) {
                collected = true;

                const int remaining = AddToInventory((s_inventory_slot*)world->player_inv_slots, PLAYER_INVENTORY_LEN, drop->item_type, drop->quantity);
                assert(remaining == 0); // Sanity check.

                // Remove this item drop.
                world->item_drop_active_cnt--;
                world->item_drops[i] = world->item_drops[world->item_drop_active_cnt];
                ZERO_OUT(world->item_drops[world->item_drop_active_cnt]);

                i--;
            }
        }
    }

    if (collected) {
        // This is called here instead of above so the same sound doesn't get stacked.
        if (!ZFW_PlaySound(audio_sys, snd_types, ek_sound_type_item_drop_collect, ZFW_VOL_DEFAULT * SettingPerc(settings, ek_setting_volume), ZFW_PAN_DEFAULT, ZFW_PITCH_DEFAULT)) {
            return false;
        }
    }

    return true;
}

void RenderItemDrops(const zfw_s_rendering_context* const rendering_context, const s_item_drop* const drops, const int drop_cnt, const zfw_s_textures* const textures) {
    assert(rendering_context);
    assert(drops);
    assert(drop_cnt >= 0);

    for (int i = 0; i < drop_cnt; i++) {
        const s_item_drop* const drop = &drops[i];
        const e_sprite spr = g_item_types[drop->item_type].icon_spr;
        RenderSprite(rendering_context, spr, textures, drop->pos, ITEM_DROP_ORIGIN, (zfw_s_vec_2d){1.0f, 1.0f}, 0.0f, ZFW_WHITE);
    }
}
