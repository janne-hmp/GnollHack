/* GnollHack 4.0 rm.c */
/* Copyright (c) Janne Gustafsson, 2020.                            */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"

NEARDATA struct location_type_definition location_type_definitions[MAX_TYPE] = {
    {"STONE",           S_stone,    CORR,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"VWALL",           S_vwall,    ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"HWALL",           S_hwall,    ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TLCORNER",        S_tlcorn,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TRCORNER",        S_trcorn,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"BLCORNER",        S_blcorn,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"BRCORNER",        S_blcorn,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"CROSSWALL",       S_crwall,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TUWALL",          S_tuwall,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TDWALL",          S_tdwall,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TLWALL",          S_tlwall,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"TRWALL",          S_trwall,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"DBWALL",          S_vcdbridge,ROOM,   MAT_WOOD,    LOCATION_SOUNDSET_NONE},
    {"TREE",            S_tree,     GRASS,  MAT_WOOD,    LOCATION_SOUNDSET_NONE},
    {"SDOOR",           S_vcdoor,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"SCORR",           S_corr,     CORR,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"POOL",            S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_NONE},
    {"MOAT",            S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_NONE},
    {"WATER",           S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_NONE},
    {"DRAWBRIDGE_UP",   S_vcdbridge,GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_DOOR},
    {"LAVAPOOL",        S_lava,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_NONE},
    {"IRON_BARS",       S_bars,     ROOM,   MAT_IRON,    LOCATION_SOUNDSET_NONE},
    {"DOOR",            S_vcdoor,   ROOM,   MAT_WOOD,    LOCATION_SOUNDSET_DOOR},
    {"CORR",            S_litcorr,  0,      MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"ROOM",            S_room,     0,      MAT_MINERAL, LOCATION_SOUNDSET_NONE },
    {"STAIRS",          S_dnstair,  ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"LADDER",          S_dnladder, ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"FOUNTAIN",        S_fountain, ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_FOUNTAIN},
    {"THRONE",          S_throne,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"SINK",            S_sink,     ROOM,   MAT_METAL,   LOCATION_SOUNDSET_NONE },
    {"GRAVE",           S_grave,    GROUND, MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"ANVIL",           S_anvil,    ROOM,   MAT_METAL,   LOCATION_SOUNDSET_NONE},
    {"ALTAR",           S_altar,    ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_ALTAR},
    {"ICE",             S_ice,      GROUND, MAT_ICE,     LOCATION_SOUNDSET_NONE},
    {"DRAWBRIDGE_DOWN", S_vodbridge,GROUND, MAT_WOOD,    LOCATION_SOUNDSET_NONE},
    {"AIR",             S_air,      0,      MAT_AIR,     LOCATION_SOUNDSET_NONE},
    {"CLOUD",           S_cloud,    AIR,    MAT_AIR,     LOCATION_SOUNDSET_NONE},
    {"GRASS",           S_grass,    0,      MAT_VEGGY,   LOCATION_SOUNDSET_NONE},
    {"GROUND",          S_ground,   0,      MAT_SOIL,    LOCATION_SOUNDSET_NONE},
    {"UNEXPLORED",      S_unexplored,0,     MAT_NONE,    LOCATION_SOUNDSET_NONE}
};


struct category_definition grass_category_definitions[MAX_GRASS_CATEGORIES] =
{
    { GRASS_SUBTYPE_NORMAL, 3},
    { GRASS_SUBTYPE_SWAMPY, 1},
};

struct category_definition ground_category_definitions[MAX_GROUND_CATEGORIES] =
{
    { GROUND_SUBTYPE_NORMAL, 1},
    { GROUND_SUBTYPE_SWAMPY, 1},
};

struct category_definition floor_category_definitions[MAX_FLOOR_CATEGORIES] =
{
    { FLOOR_SUBTYPE_NORMAL, 4},
    { FLOOR_SUBTYPE_MARBLE, 1},
    { FLOOR_SUBTYPE_PARQUET, 1},
};


/* force linkage */
void
init_rm()
{
    return;
}

enum obj_material_types
get_location_type_material(typ, subtyp)
int typ, subtyp;
{
    if (subtyp <= 0)
        return typ >= 0 && typ < MAX_TYPE ? location_type_definitions[typ].material : MAT_NONE;

    enum obj_material_types material = location_type_definitions[typ].material;

    if (typ == ROOM)
    {
        enum floor_subtypes subtype = subtyp;
        switch (subtype)
        {
        case FLOOR_SUBTYPE_PARQUET:
            material = MAT_WOOD;
            break;
        case FLOOR_SUBTYPE_NORMAL:
        case FLOOR_SUBTYPE_NORMAL_VARIATION_1:
        case FLOOR_SUBTYPE_NORMAL_VARIATION_2:
        case FLOOR_SUBTYPE_NORMAL_VARIATION_3:
        case FLOOR_SUBTYPE_MARBLE:
            /* Base material type */
            break;
        default:
            break;
        }
    }

    return material;
}

int
get_location_subtype_by_category(ltype, category_id)
int ltype;
int category_id;
{
    struct category_definition* cat_def = 0;
    int cat_size = 0;

    if (ltype == GRASS)
    {
        cat_def = grass_category_definitions;
        cat_size = MAX_GRASS_CATEGORIES;
    }
    else if (ltype == GROUND)
    {
        cat_def = ground_category_definitions;
        cat_size = MAX_GROUND_CATEGORIES;
    }
    else if (ltype == ROOM)
    {
        cat_def = floor_category_definitions;
        cat_size = MAX_FLOOR_CATEGORIES;
    }

    if (cat_def == 0)
        return 0;

    if (category_id < 0 || category_id >= cat_size)
        return 0;

    int first_st = cat_def[category_id].first_subtype;
    int num_st = cat_def[category_id].number_of_subtypes;

    if (num_st <= 0)
        return 0;
    else if (num_st == 1)
        return max(0, first_st);
    else
        return max(0, first_st) + rn2(num_st);

}

/* rm.c */