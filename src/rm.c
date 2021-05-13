/* GnollHack 4.0 rm.c */
/* Copyright (c) Janne Gustafsson, 2020.                            */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"

NEARDATA struct location_type_definition location_type_definitions[MAX_TYPE] = {
    {"STONE",           S_stone,    CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"VWALL",           S_vwall,    CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"HWALL",           S_hwall,    CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TLCORNER",        S_tlcorn,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TRCORNER",        S_trcorn,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"BLCORNER",        S_blcorn,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"BRCORNER",        S_blcorn,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"CROSSWALL",       S_crwall,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TUWALL",          S_tuwall,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TDWALL",          S_tdwall,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TLWALL",          S_tlwall,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"TRWALL",          S_trwall,   CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"DBWALL",          S_vcdbridge,CORR,   MAT_WOOD,    LOCATION_SOUNDSET_STONE},
    {"TREE",            S_tree,     GRASS,  MAT_WOOD,    LOCATION_SOUNDSET_TREE},
    {"SDOOR",           S_vcdoor,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"SCORR",           S_corr,     CORR,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"POOL",            S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_WATER},
    {"MOAT",            S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_WATER},
    {"WATER",           S_pool,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_WATER},
    {"DRAWBRIDGE_UP",   S_vcdbridge,GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_WATER},
    {"LAVAPOOL",        S_lava,     GROUND, MAT_LIQUID,  LOCATION_SOUNDSET_LAVA},
    {"IRON_BARS",       S_bars,     ROOM,   MAT_IRON,    LOCATION_SOUNDSET_GENERAL},
    {"DOOR",            S_vcdoor,   ROOM,   MAT_WOOD,    LOCATION_SOUNDSET_DOOR},
    {"CORR",            S_litcorr,  0,      MAT_MINERAL, LOCATION_SOUNDSET_NONE},
    {"ROOM",            S_room,     0,      MAT_MINERAL, LOCATION_SOUNDSET_NONE },
    {"STAIRS",          S_dnstair,  ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_STONE},
    {"LADDER",          S_dnladder, ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_GENERAL},
    {"FOUNTAIN",        S_fountain, ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_FOUNTAIN},
    {"THRONE",          S_throne,   ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_GENERAL},
    {"SINK",            S_sink,     ROOM,   MAT_METAL,   LOCATION_SOUNDSET_SINK },
    {"GRAVE",           S_grave,    GROUND, MAT_MINERAL, LOCATION_SOUNDSET_GENERAL},
    {"BRAZIER",         S_brazier,  ROOM,   MAT_METAL,   LOCATION_SOUNDSET_BRAZIER},
    {"SIGNPOST",        S_signpost, ROOM,   MAT_WOOD,    LOCATION_SOUNDSET_GENERAL},
    {"ANVIL",           S_anvil,    ROOM,   MAT_METAL,   LOCATION_SOUNDSET_GENERAL},
    {"ALTAR",           S_altar,    ROOM,   MAT_MINERAL, LOCATION_SOUNDSET_ALTAR},
    {"ICE",             S_ice,      GROUND, MAT_ICE,     LOCATION_SOUNDSET_ICE},
    {"DRAWBRIDGE_DOWN", S_vodbridge,GROUND, MAT_WOOD,    LOCATION_SOUNDSET_GENERAL},
    {"AIR",             S_air,      0,      MAT_AIR,     LOCATION_SOUNDSET_NONE},
    {"CLOUD",           S_cloud,    0,      MAT_AIR,     LOCATION_SOUNDSET_NONE},
    {"GRASS",           S_grass,    0,      MAT_VEGGY,   LOCATION_SOUNDSET_NONE},
    {"GROUND",          S_ground,   0,      MAT_SOIL,    LOCATION_SOUNDSET_NONE},
    {"UNDEFINED_LOCATION",      S_unexplored,0,     MAT_NONE,    LOCATION_SOUNDSET_NONE}
};

struct location_subtype_definition corridor_subtype_definitions[MAX_CORRIDOR_SUBTYPES] =
{
    { "normal corridor", CORRIDOR_SUBTYPE_NORMAL_VARIATIONS, 0 },
};

struct location_subtype_definition grass_subtype_definitions[MAX_GRASS_SUBTYPES] =
{
    { "normal grass", GRASS_SUBTYPE_NORMAL_VARIATIONS, 0 },
    { "swampy grass", GRASS_SUBTYPE_SWAMPY_VARIATIONS, GRASS_SUBTYPE_NORMAL_VARIATIONS },
};

struct location_subtype_definition ground_subtype_definitions[MAX_GROUND_SUBTYPES] =
{
    { "normal ground", GROUND_SUBTYPE_NORMAL_VARIATIONS, 0 },
    { "swampy ground", GROUND_SUBTYPE_SWAMPY_VARIATIONS, GROUND_SUBTYPE_NORMAL_VARIATIONS },
};

struct location_subtype_definition floor_subtype_definitions[MAX_FLOOR_SUBTYPES] =
{
    { "normal floor", FLOOR_SUBTYPE_NORMAL_VARIATIONS, 0 },
    { "marble floor", FLOOR_SUBTYPE_MARBLE_VARIATIONS, FLOOR_SUBTYPE_NORMAL_VARIATIONS },
    { "parquet", FLOOR_SUBTYPE_PARQUET_VARIATIONS, FLOOR_SUBTYPE_MARBLE_VARIATIONS + FLOOR_SUBTYPE_NORMAL_VARIATIONS },
};

struct location_subtype_definition stone_subtype_definitions[MAX_STONE_SUBTYPES] =
{
    { "normal stone", STONE_SUBTYPE_NORMAL_VARIATIONS, 0 },
};

struct location_subtype_definition wall_subtype_definitions[MAX_WALL_SUBTYPES] =
{
    { "normal wall", WALL_SUBTYPE_NORMAL_VARIATIONS, 0 }, /* Must be aligned with stone normal subtypes */
};


struct door_subtype_definition door_subtype_definitions[MAX_DOOR_SUBTYPES] =
{
    {"wooden door",          "door", MAT_WOOD, LOCATION_SOUNDSET_DOOR, DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES | DSTFLAGS_BROKEN_BY_STRONG_MONSTERS | DSTFLAGS_BROKEN_BY_DIGGING | DSTFLAGS_BROKEN_BY_KICKING | DSTFLAGS_BROKEN_BY_STRIKING | DSTFLAGS_BROKEN_BY_BEING_BOOBY_TRAPPED },
    {"windowed wooden door", "door", MAT_WOOD, LOCATION_SOUNDSET_DOOR, DSTFLAGS_BROKEN_BY_STRONG_MONSTERS | DSTFLAGS_BROKEN_BY_DIGGING | DSTFLAGS_BROKEN_BY_KICKING | DSTFLAGS_BROKEN_BY_STRIKING | DSTFLAGS_BROKEN_BY_BEING_BOOBY_TRAPPED },
    {"iron door",            "door", MAT_IRON, LOCATION_SOUNDSET_DOOR, DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"windowed iron door",   "door", MAT_IRON, LOCATION_SOUNDSET_DOOR, DSTFLAGS_NONE },
    {"iron bar door",        "door", MAT_IRON, LOCATION_SOUNDSET_DOOR, DSTFLAGS_NONE },
    {"magic door",           "door", MAT_MINERAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_INDESTRUCTIBLE | DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"windowed magic door",  "door", MAT_MINERAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_INDESTRUCTIBLE },
    {"alien door",           "door", MAT_MODRONITE, LOCATION_SOUNDSET_DOOR, DSTFLAGS_NO_LOCK_DESCRIPTION_IS_DEFAULT | DSTFLAGS_INDESTRUCTIBLE | DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"reinforced door",      "door", MAT_METAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_INDESTRUCTIBLE | DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"stone door",           "door", MAT_MINERAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_NO_LOCK_DESCRIPTION_IS_DEFAULT | DSTFLAGS_BROKEN_BY_DIGGING | DSTFLAGS_BROKEN_BY_STRIKING | DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"obsidian door",        "door", MAT_MINERAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_NO_LOCK_DESCRIPTION_IS_DEFAULT | DSTFLAGS_BLOCKS_VISION_AND_SOUND | DSTFLAGS_BLOCKS_PROJECTILES},
    {"black gate",           "gate", MAT_METAL, LOCATION_SOUNDSET_DOOR, DSTFLAGS_BLOCKS_VISION_AND_SOUND},
};

struct tree_subtype_definition tree_subtype_definitions[MAX_TREE_SUBTYPES] =
{
    {"tree",          "tree",  
        TREE_SUBTYPE_NORMAL_VARIATIONS, 0,  
        TREE_CLASS_GENERAL, 
        {10, 10, 0, 0, 0, 10, 10, 10, 0, 0},
        0, 0, 0, 0, 0, 0, 0, 0,  -1, -1,  0UL,
    },
    {"spruce tree",   "tree",  
        TREE_SUBTYPE_SPRUCE_VARIATIONS, TREE_SUBTYPE_NORMAL_VARIATIONS,  
        TREE_CLASS_CONIFEROUS, 
        {10, 0, 10, 0, 0, 10, 10, 10, 10, 10},
        0, 0, 0, 0, 0, 0, 0, 0,  -1, -1,  0UL,
    },
    {"fir tree",      "tree",  
        TREE_SUBTYPE_FIR_VARIATIONS, TREE_SUBTYPE_SPRUCE_VARIATIONS + TREE_SUBTYPE_NORMAL_VARIATIONS,  
        TREE_CLASS_CONIFEROUS, 
        {10, 0, 10, 0, 0, 10, 10, 10, 10, 10},
        0, 0, 0, 0, 0, 0, 0, 0,  -1, -1,  0UL,
    },
    {"date palm",      "tree",
        TREE_SUBTYPE_DATE_PALM_VARIATIONS, TREE_SUBTYPE_FIR_VARIATIONS + TREE_SUBTYPE_SPRUCE_VARIATIONS + TREE_SUBTYPE_NORMAL_VARIATIONS,
        TREE_CLASS_TROPICAL, 
        {0, 0, 0, 10, 10, 10, 0, 0, 0, 0},
        CLUSTER_OF_DATES, 2, 4, 0, 50, 1, 2, 0,  -1, -1,  0UL,
    },
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
get_initial_tree_subtype(forest_type)
schar forest_type;
{
    if (forest_type < 0 || forest_type >= MAX_FOREST_TYPES)
    {
        forest_type = level.flags.forest_type;
        if (forest_type < 0 || forest_type >= MAX_FOREST_TYPES)
            return 0;
    }

    int i = 0, totalprob = 0;

    for (i = 0; i < MAX_TREE_SUBTYPES; i++)
        
        totalprob += tree_subtype_definitions[i].probability[forest_type];

    if (totalprob <= 0)
        return 0;

    int roll = rn2(totalprob);
    for (i = 0; i < MAX_TREE_SUBTYPES; i++)
    {
        roll -= tree_subtype_definitions[i].probability[forest_type];
        if (roll < 0)
            break;
    }

    if (i >= MAX_TREE_SUBTYPES)
        return 0;

    return i;
}


int
get_initial_location_subtype(ltype)
int ltype;
{
    if (ltype == FOUNTAIN)
        return FOUNTAIN_MAGIC + rn2(MAX_FOUNTAIN_SUBTYPES - FOUNTAIN_MAGIC);
    else if (ltype == TREE)
        return get_initial_tree_subtype(level.flags.forest_type);
    else
        return ltype == GRASS && level.flags.swampy ? GRASS_SUBTYPE_SWAMPY : ltype == GROUND && level.flags.swampy ? GROUND_SUBTYPE_SWAMPY : 0;
}

int
get_initial_location_vartype(typ, subtyp)
int typ, subtyp;
{
    struct location_subtype_definition* sub_def = 0;
    int num_subs = 0;

    if (typ == CORR)
    {
        sub_def = corridor_subtype_definitions;
        num_subs = MAX_CORRIDOR_SUBTYPES;
    }
    else if (typ == GRASS)
    {
        sub_def = grass_subtype_definitions;
        num_subs = MAX_GRASS_SUBTYPES;
    }
    else if (typ == GROUND)
    {
        sub_def = ground_subtype_definitions;
        num_subs = MAX_GROUND_SUBTYPES;
    }
    else if (typ == ROOM)
    {
        sub_def = floor_subtype_definitions;
        num_subs = MAX_FLOOR_SUBTYPES;
    }
    else if (typ == STONE)
    {
        sub_def = stone_subtype_definitions;
        num_subs = MAX_STONE_SUBTYPES;
    }
    else if (IS_WALL(typ))
    {
        sub_def = wall_subtype_definitions;
        num_subs = MAX_WALL_SUBTYPES;
    }
    else
        return 0;

    if (!sub_def || subtyp < 0 || subtyp >= num_subs)
        return 0;

    int num = sub_def[subtyp].number_of_vartypes;

    if (num <= 1)
        return 0;
    else
        return rn2(num);
}

const char*
get_door_name_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return "illegal door location";

    struct rm* door = &levl[x][y];
    return  get_door_name_at_ptr(door);

}

const char*
get_door_name_at_ptr(door)
struct rm* door;
{
    if (!door)
        return "no door pointer";

    if (door->typ == SDOOR)
        return "secret door";
    else if (!IS_DOOR(door->typ))
        return "not a door";

    if (door->subtyp < 0 || door->subtyp >= MAX_DOOR_SUBTYPES)
        return "door";
    else
        return door_subtype_definitions[door->subtyp].description;
}

const char*
get_short_door_name_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return "illegal door location";

    struct rm* door = &levl[x][y];
    return  get_short_door_name_at_ptr(door);

}

const char*
get_short_door_name_at_ptr(door)
struct rm* door;
{
    if (!door)
        return "no door pointer";

    if(door->typ == SDOOR)
        return "secret door";
    else if(!IS_DOOR(door->typ))
        return "not a door";

    if (door->subtyp < 0 || door->subtyp >= MAX_DOOR_SUBTYPES)
        return "door";
    else
        return door_subtype_definitions[door->subtyp].short_description;
}

boolean
m_can_destroy_door(mtmp, door, include_eating)
struct monst* mtmp;
struct rm* door;
boolean include_eating;
{
    if (!mtmp || !door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;
    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (include_eating && metallivorous(mtmp->data)
        && (door_subtype_definitions[subtyp].material == MAT_METAL || door_subtype_definitions[subtyp].material == MAT_IRON))
        return TRUE;

    if (include_eating && tunnels(mtmp->data) && !needspick(mtmp->data)
        && (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_DIGGING)
        && (door_subtype_definitions[subtyp].material == MAT_MINERAL))
        return TRUE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_STRONG_MONSTERS)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_indestructible_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_indestructible_at_ptr(door);

}


boolean
is_door_indestructible_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >=0  && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;
    if ((door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE) != 0
        || (door->flags & L_INDESTRUCTIBLE) != 0)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_nonpassable_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_nonpassable_at_ptr(door);

}


boolean
is_door_nonpassable_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;
    if ((door_subtype_definitions[subtyp].flags & DSTFLAGS_NONPASSABLE) != 0
        || (door->flags & L_NON_PASSDOOR) != 0)
        return TRUE;
    else
        return FALSE;

}

boolean
has_door_normal_lock_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  has_door_normal_lock_at_ptr(door);

}


boolean
has_door_normal_lock_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    if ((door->key_otyp == STRANGE_OBJECT || door->key_otyp == SKELETON_KEY) && door->special_quality == 0)
        return TRUE;
    else
        return FALSE;

}

boolean
has_box_normal_lock(box)
struct obj* box;
{
    if (!box || !Is_box(box))
        return FALSE;

    if ((box->keyotyp == STRANGE_OBJECT || box->keyotyp == NON_PM || box->keyotyp == SKELETON_KEY) && box->special_quality == 0)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_diggable_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_diggable_at_ptr(door);

}


boolean
is_door_diggable_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_DIGGING)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_kickable_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_kickable_at_ptr(door);

}


boolean
is_door_kickable_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_KICKING)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_destroyed_by_booby_trap_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_destroyed_by_booby_trap_at_ptr(door);

}


boolean
is_door_destroyed_by_booby_trap_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_BEING_BOOBY_TRAPPED)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_destroyed_by_striking_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_destroyed_by_striking_at_ptr(door);

}


boolean
is_door_destroyed_by_striking_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_STRIKING)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_destroyed_by_monsters_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  is_door_destroyed_by_monsters_at_ptr(door);

}


boolean
is_door_destroyed_by_monsters_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_INDESTRUCTIBLE)
        return FALSE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BROKEN_BY_STRONG_MONSTERS)
        return TRUE;
    else
        return FALSE;

}

boolean
door_blocks_vision_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return FALSE;

    struct rm* door = &levl[x][y];
    return  door_blocks_vision_at_ptr(door);

}


boolean
door_blocks_vision_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return FALSE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;

    /* Secret doors always block vision */
    if (door->typ == SDOOR)
        return TRUE;

    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_BLOCKS_VISION_AND_SOUND)
        return TRUE;
    else
        return FALSE;

}

boolean
is_door_normally_without_lock_at(x, y)
xchar x, y;
{
    if (!isok(x, y))
        return TRUE;

    struct rm* door = &levl[x][y];
    return  is_door_normally_without_lock_at_ptr(door);

}


boolean
is_door_normally_without_lock_at_ptr(door)
struct rm* door;
{
    if (!door || !IS_DOOR_OR_SDOOR(door->typ))
        return TRUE;

    enum door_subtypes_types subtyp = door->subtyp >= 0 && door->subtyp < MAX_DOOR_SUBTYPES ? door->subtyp : 0;
    if (door_subtype_definitions[subtyp].flags & DSTFLAGS_NO_LOCK_DESCRIPTION_IS_DEFAULT)
        return TRUE;
    else
        return FALSE;

}

void
set_initial_location_floortype(lev, fountain_on_grass, fountain_on_ground, tree_on_ground, throne_on_ground)
struct rm* lev;
boolean fountain_on_grass, fountain_on_ground, tree_on_ground, throne_on_ground;
{
    if (!lev)
        return;

    if (lev->typ == FOUNTAIN && fountain_on_grass)
    {
        lev->floortyp = GRASS;
        lev->floorsubtyp = get_initial_location_subtype(lev->floortyp);
        lev->floorvartyp = get_initial_location_vartype(lev->floortyp, lev->floorsubtyp);
    }
    else if (lev->typ == FOUNTAIN && fountain_on_ground)
    {
        lev->floortyp = GROUND;
        lev->floorsubtyp = get_initial_location_subtype(lev->floortyp);
        lev->floorvartyp = get_initial_location_vartype(lev->floortyp, lev->floorsubtyp);
    }
    else if (lev->typ == TREE && tree_on_ground)
    {
        lev->floortyp = GROUND;
        lev->floorsubtyp = get_initial_location_subtype(lev->floortyp);
        lev->floorvartyp = get_initial_location_vartype(lev->floortyp, lev->floorsubtyp);
    }
    else if (lev->typ == THRONE && throne_on_ground)
    {
        lev->floortyp = GROUND;
        lev->floorsubtyp = get_initial_location_subtype(lev->floortyp);
        lev->floorvartyp = get_initial_location_vartype(lev->floortyp, lev->floorsubtyp);
    }
    else
    {
        lev->floortyp = location_type_definitions[lev->typ].initial_floor_type;
        lev->floorsubtyp = !IS_FLOOR(lev->floortyp) ? 0 : get_initial_location_subtype(lev->floortyp);
        lev->floorvartyp = !IS_FLOOR(lev->floortyp) ? 0 : get_initial_location_vartype(lev->floortyp, lev->floorsubtyp);
    }
}

void
initialize_location(lev)
struct rm* lev;
{
    if (!lev)
        return;

    int type = lev->typ;
    if (type == TREE)
    {
        int subtype = lev->subtyp;
        if (tree_subtype_definitions[subtype].fruit_type > STRANGE_OBJECT)
        {
            lev->special_quality = max(0, d(tree_subtype_definitions[subtype].fruit_d, tree_subtype_definitions[subtype].fruit_n) + tree_subtype_definitions[subtype].fruit_p);
        }
    }
}
/* rm.c */