/*
 * action.h
 * Copyright (c) Janne Gustafsson, 2020
 *
 */

#ifndef ACTION_H
#define ACTION_H

/* Action tiles */
enum action_tile_types
{
    ACTION_TILE_NO_ACTION = 0,
    ACTION_TILE_ATTACK,
    ACTION_TILE_THROW,
    ACTION_TILE_FIRE,
	ACTION_TILE_CAST_NODIR,
	ACTION_TILE_CAST_DIR,
	ACTION_TILE_SPECIAL_ATTACK,
    ACTION_TILE_KICK,
    ACTION_TILE_PASSIVE_DEFENSE,
    ACTION_TILE_DEFEND,
    ACTION_TILE_RECEIVE_DAMAGE,
    ACTION_TILE_ITEM_USE,
    ACTION_TILE_DOOR_USE,
    ACTION_TILE_DEATH,
    MAX_ACTION_TILES
};


#define ACTION_ITEM_USE_FLAGS_NONE        0x00000000UL
#define ACTION_ITEM_USE_FLAGS_CAMERA      0x00000001UL
#define ACTION_ITEM_USE_FLAGS_PICK_AXE    0x00000002UL
#define ACTION_ITEM_USE_FLAGS_JAR         0x00000004UL
#define ACTION_ITEM_USE_FLAGS_POTION      0x00000008UL
#define ACTION_ITEM_USE_FLAGS_SCROLL      0x00000010UL
#define ACTION_ITEM_USE_FLAGS_ALL         0xFFFFFFFFUL

#define ACTION_ATTACK_FLAGS_NONE          0x00000000UL
#define ACTION_ATTACK_FLAGS_PICK_AXE      0x00000001UL

#define ACTION_SPECIAL_ATTACK_FLAGS_NONE        0x00000000UL
#define ACTION_SPECIAL_ATTACK_FLAGS_CAMERA      0x00000001UL

#endif /* ACTION_H */