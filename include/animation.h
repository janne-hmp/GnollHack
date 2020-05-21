/*
 * animation.h
 * Copyright 2020 by Janne Gustafsson
 */

#ifndef ANIMATION_H
#define ANIMATION_H


/* Enlargement sets */
#define MAX_FRAMES_PER_ENLARGEMENT 5
#define MAX_TILES_PER_ENLARGEMENT MAX_FRAMES_PER_ENLARGEMENT

struct enlargement_definition {
    char* enlargement_name;
    int base_glyph_id; /* For monster, object, etc. being enlarged */
    char number_of_tiles;
    char number_of_frames;
    int glyph_offset;
    char width_in_tiles;
    char height_in_tiles;
    char main_tile_x_coordinate; /* Always 0 or 1 on the last row */
    char frame2tile[MAX_FRAMES_PER_ENLARGEMENT];
    /* Enlargement frame position: number from 0 to 4, X = -1 indicates main tile
            0 1 2    
            3 X 4
    */
    char frame_z_order[MAX_FRAMES_PER_ENLARGEMENT];
    /* 0 = normal, -X in back, +X in front */
    char frame_flags[MAX_FRAMES_PER_ENLARGEMENT];
};

#define ENLFLAGS_H_FLIP 0x01


enum enlargement_types
{
    NO_ENLARGEMENT = 0,
    DRACOLICH_ENLARGEMENT /* Keep this last */
};

#define NUM_ENLARGEMENTS DRACOLICH_ENLARGEMENT

#define DRACOLICH_ENLARGEMENT_OFF (0)
#define DRACOLICH_ENLARGEMENT_FRAMES 5

#define MAX_ENLARGEMENT_FRAMES (DRACOLICH_ENLARGEMENT_FRAMES + DRACOLICH_ENLARGEMENT_OFF)

extern NEARDATA struct enlargement_definition enlargements[];


/* Animation tiles */
#define MAX_FRAMES_PER_ANIMATION 32
#define MAX_TILES_PER_ANIMATION MAX_FRAMES_PER_ANIMATION

enum main_tile_use_types
{
    ANIMATION_MAIN_TILE_USE_FIRST = 0,
    ANIMATION_MAIN_TILE_USE_LAST,
    ANIMATION_MAIN_TILE_IGNORE
};

struct animation_definition {
    char* animation_name;
    char number_of_tiles;
    char number_of_frames;
    int glyph_offset;
    int intervals_between_frames;
    enum main_tile_use_types main_tile_use_style; /* 0 = play as first tile and frame, 1 = play as last tile and frame, 2 = ignore */
    char frame2tile[MAX_FRAMES_PER_ANIMATION];
    short tile_enlargement[MAX_TILES_PER_ANIMATION];
};

enum animation_types
{
    NO_ANIMATION = 0,
    HANDCRAFTED_CANDLE_LIT_ANIMATION,
    TWISTED_CANDLE_LIT_ANIMATION /* Keep this last */
};

#define NUM_ANIMATIONS TWISTED_CANDLE_LIT_ANIMATION

#define HANDCRAFTED_CANDLE_ANIMATION_OFF (0)
#define HANDCRAFTED_CANDLE_ANIMATION_FRAMES 1
#define TWISTED_CANDLE_ANIMATION_OFF (HANDCRAFTED_CANDLE_ANIMATION_FRAMES + HANDCRAFTED_CANDLE_ANIMATION_OFF)
#define TWISTED_CANDLE_ANIMATION_FRAMES 6

#define MAX_ANIMATION_FRAMES (TWISTED_CANDLE_ANIMATION_FRAMES + TWISTED_CANDLE_ANIMATION_OFF)

extern NEARDATA struct animation_definition animations[];


#endif /* ANIMATION_H */