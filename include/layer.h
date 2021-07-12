/*
 * Layers. Copyright (c) Janne Gustafsson, 2020
 *
 */
#include "tradstdc.h"
#include "config.h"

#ifndef LAYER_H
#define LAYER_H

enum layer_types
{
    LAYER_FLOOR = 0,
    LAYER_FLOOR_DOODAD,   /* Doodads underneath features and traps */
    LAYER_FEATURE,
    LAYER_TRAP,
    LAYER_FEATURE_DOODAD, /* Doodads above features and traps */
    LAYER_BACKGROUND_EFFECT,
    LAYER_CHAIN,
    LAYER_OBJECT,
    LAYER_MONSTER,
    LAYER_MISSILE,
    LAYER_COVER_TRAP,
    LAYER_COVER_OBJECT,
    LAYER_COVER_FEATURE,
    LAYER_LEASH,
    LAYER_ENVIRONMENT,
    LAYER_ZAP,
    LAYER_GENERAL_EFFECT,
    LAYER_MONSTER_EFFECT,
    LAYER_GENERAL_UI,
    MAX_LAYERS
};


#define MAXLEASHED 2

struct layer_info {
    int glyph; /* For ascii compatibility */
    int bkglyph; /* For ascii compatibility */

    int layer_glyphs[MAX_LAYERS];

    unsigned long layer_flags;

    unsigned m_id;  /* check that the monster found at the square is the one that is supposed to be drawn by comparing their m_ids */

    struct obj* memory_objchn;     /* Pointer to the location's first memory object*/

    short damage_displayed;
    short hit_tile;
    short special_monster_layer_height;

    uchar missile_poisoned;
    uchar missile_elemental_enchantment;
    uchar missile_exceptionality;
    uchar missile_mythic_prefix;
    uchar missile_mythic_suffix;
    uchar missile_eroded;
    uchar missile_eroded2;
    unsigned long missile_flags;

    xchar leash_mon_x[MAXLEASHED + 1]; /* the last coordinate is the other end of the leash, i.e., u.ux at the time */
    xchar leash_mon_y[MAXLEASHED + 1]; /* the last coordinate is the other end of the leash, i.e., u.uy at the time */
};

/* These are standard flags replacing earlier glyph system -- To be used mostly in map_glyph for tty */
#define LFLAGS_M_PET                0x00000001UL
#define LFLAGS_M_PEACEFUL           0x00000002UL
#define LFLAGS_M_DETECTED           0x00000004UL
#define LFLAGS_M_RIDDEN             0x00000008UL
#define LFLAGS_M_SADDLED            0x00000010UL
#define LFLAGS_M_YOU                0x00000020UL
#define LFLAGS_M_MIMIC_OBJECT       0x00000040UL
#define LFLAGS_M_MIMIC_FURNITURE    0x00000080UL

/* These are flags for showing extra stuff in graphical interfaces */
#define LFLAGS_M_BEING_HIT          0x00000100UL
#define LFLAGS_U_TETHERED           0x00000200UL /* You are tethered */
#define LFLAGS_M_TETHERED           0x00000400UL /* Monster is tethered */
#define LFLAGS_M_DROPPING_PIERCER   0x00000800UL
#define LFLAGS_M_WORM_TAIL          0x00001000UL
#define LFLAGS_M_WORM_SEEN          0x00002000UL
#define LFLAGS_M_KILLED             0x00004000UL
/* free bit */
#define LFLAGS_M_MASK               0x0000FFFFUL

/* These are flags for showing extra stuff in graphical interfaces */
#define LFLAGS_O_PILE               0x00010000UL
#define LFLAGS_O_IN_PIT             0x00020000UL
#define LFLAGS_O_CHAIN              0x00040000UL
#define LFLAGS_O_MASK               0x00070000UL

/* free bit for CMAPs */
#define LFLAGS_CMAP_MASK            0x00080000UL

#define LFLAGS_ZAP_LEADING_EDGE     0x00100000UL /* First (leading) tile in a zap */
#define LFLAGS_ZAP_TRAILING_EDGE    0x00200000UL /* Last (trailing) tile in a zap */
#define LFLAGS_ZAP_MASK             (LFLAGS_ZAP_LEADING_EDGE | LFLAGS_ZAP_TRAILING_EDGE)

/* Traps */
#define LFLAGS_T_TRAPPED            0x00400000UL
/* free bit */

/* Important general flags */
#define LFLAGS_SHOWING_MEMORY       0x01000000UL /* also implies that you cannot see the location (and hence showing memory) */
#define LFLAGS_SHOWING_DETECTION    0x02000000UL /* Do not darken */
#define LFLAGS_ASCENSION_RADIANCE   0x04000000UL /* Lighten up */
#define LFLAGS_CAN_SEE              0x08000000UL /* cansee(x, y) is true (if not, then darken etc.) */
#define LFLAGS_UXUY                 0x10000000UL /* x == u.ux && y == u.uy is true */
#define LFLAGS_APPEARS_UNLIT        0x20000000UL

#define MISSILE_FLAGS_CORRODEABLE   0x00000001UL
#define MISSILE_FLAGS_ROTTABLE      0x00000002UL
#define MISSILE_FLAGS_FLAMMABLE     0x00000004UL
#define MISSILE_FLAGS_RUSTPRONE     0x00000008UL
#define MISSILE_FLAGS_ERODEPROOF    0x00000010UL
#define MISSILE_FLAGS_POISONABLE    0x00000020UL
#define MISSILE_FLAGS_TETHERED      0x00000040UL /* Missile is tethered */

#endif /* LAYER_H */