/* GnollHack 4.0 tiledata.c */
/* Copyright (c) Janne Gustafsson, 2020.                            */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL void FDECL(set_base_tileset_cmap, (int*, int, int));
STATIC_DCL void FDECL(set_base_tileset_cmap_variation, (int*, int, int));

#ifdef USE_TILES
int glyph2tile[MAX_GLYPH] = { 0 }; /* moved here from tile.c */
uchar glyphtileflags[MAX_GLYPH] = { 0 }; /* specifies how to use the tile and operations applied to the tile before use (e.g., flip or move to middle) */
short glyph2replacement[MAX_GLYPH] = { 0 };
short tile2autodraw[MAX_TILES] = { 0 };
short tile2replacement[MAX_TILES] = { 0 };
short tile2animation[MAX_TILES] = { 0 };
short tile2enlargement[MAX_TILES] = { 0 };
#endif

NEARDATA struct tileset_definition default_tileset_definition =
{
    {"dungeon-normal", "gnomish-mines", "gehennom", "dungeon-undead", "sokoban", "elemental-planes", "astral", "gehennom-cavernous", "cmap-8", "cmap-9", "cmap-10", "cmap-11", "cmap-12", "cmap-13", "cmap-14", "cmap-15"},
    {0.0, 0.85, 0.0, 0.85, 0.0, 0.95, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    {0.0, 0.70, 0.0, 0.70, 0.0, 0.85, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    {0.0, 0.40, 0.0, 0.40, 0.0, 0.70, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
    2,
    {TRUE, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, 1, 2,
    2,
    3
};

NEARDATA struct ui_component_definition ui_tile_component_array[MAX_UI_TILES] = {
    {"general-ui",              NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 3, 16, 16, {"checkbox-unchecked", "checkbox-checked", "checkbox-count", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"status",                  NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, MAX_STATUS_MARKS, 16, 16, {"petmark", "peacemark", "detectmark", "pilemark",  "hungry", "weak", "faint", "burdened",  "stressed", "strained", "overtaxed", "overloaded",  "two-weapon", "skill", "saddled", "low-hp",  "critical-hp", "spec-used", "trapped", "ustuck",  "inventory", "townguard-peaceful", "townguard-hostile", "",  "", "", "", "",  "", "", "", ""} },
    {"conditions",              NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, min(min(MAX_UI_TILE_16_x_16_COMPONENTS, MAX_UI_TILE_COMPONENTS), BL_MASK_BITS), 16, 16, {"", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"main-tile-mark",          NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 1, 64, 96, {"whole", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"u-tile-mark",             NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 1, 64, 96, {"whole", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"item-autodraw-graphics",  NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 1, 64, 96, {"whole", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"item2-autodraw-graphics", NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 1, 64, 96, {"whole", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"item-property-marks",     NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, min(min(MAX_UI_TILE_8_x_24_COMPONENTS, MAX_UI_TILE_COMPONENTS), MAX_ITEM_PROPERTY_MARKS), 8, 24, {"poisoned", "death-magical", "flaming", "freezing",  "electrified", "exceptional",  "elite", "celestial", "primordial", "infernal", "mythic", "legendary", "corroded", "rotted", "burnt", "rusty",  "very-corroded", "very-rotted", "very-burnt", "very-rusty", "thoroughly-corroded", "thoroughly-rotted", "thoroughly-burnt", "thoroughly-rusty",  "", "", "", "",  "", "", "", "" } },
    {"jar-graphics",            NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 2, 64, 48, {"background", "contents", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
    {"jar-foreground",          NO_REPLACEMENT, NO_ANIMATION, NO_ENLARGEMENT, 2, 64, 48, {"transparent", "opaque", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", "",  "", "", "", ""} },
};

boolean
has_generic_player_action_tile(action)
enum action_tile_types action;
{
    if (action > 0)
    {
        /* Do nothing at the moment */
    }
    return TRUE;
}


void
init_tiledata()
{
    /* fill out condition names*/
    int i;
    for (i = 0; i < BL_MASK_BITS; i++)
    {
        unsigned long bit = 1UL << i;
        const char* cond_name = get_condition_name(bit);
        ui_tile_component_array[CONDITION_MARKS].component_names[min(i, MAX_UI_TILE_COMPONENTS)] = cond_name;
    }

}

int
process_tiledata(process_style, save_file_name, tilemaparray, tilemapflags) /* Save tile data / read tile data / count tiles */
int process_style;  /* 0 = save data to file, 1 = read data from file, 2 = count tiles */
const char* save_file_name;
int* tilemaparray;
uchar* tilemapflags;
{
    struct tileset_definition* tsd = &default_tileset_definition;
    const char* fq_save = save_file_name;
    const char* tile_section_name;
    const char* set_name;
    int fd, i, j;
    int tile_count = 0;
    char buf[BUFSIZ];
    strcpy(buf, "");
    int glyph_offset = 0;
    int roleidx, raceidx, gender, alignment, glevel;
    int cmap_type_idx;

    if (process_style == 0)
    {
        (void)remove(fq_save);

#ifdef MAC
        fd = macopen(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, TEXT_TYPE);
#else
        fd = open(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, FCMASK);
#endif
    }

    /* Monster tiles */
    tile_section_name = "monsters";
    
    /* spsets must be in same order as enum action_tile_types */
    const char* monster_set_name_array[MAX_ACTION_TILES + 2] = { "normal", "attack", "throw", "fire", "cast-nodir", "cast-dir", "special-attack", "kick", "passive-defense", "special-attack-2", "special-attack-3", "item-use", "door-use", "death", "statue", "body" };

    for (gender = 0; gender <= 1; gender++)
    {
        if (gender == 1)
        {
            if (tsd->female_tile_style == 0)
                continue;
        }
        const char* gender_name = (gender == 0 ? "base" : "female");
        int spset;
        for (spset = 0; spset < MAX_ACTION_TILES + 2; spset++)
        {
            if (spset > ACTION_TILE_NO_ACTION && spset < MAX_ACTION_TILES)
            {
                if (!tsd->action_tile_style[spset])
                    continue;
            }

            if (spset == MAX_ACTION_TILES && !tsd->statue_tile_style)
                continue;
            if (spset == MAX_ACTION_TILES + 1 && !tsd->corpse_tile_style)
                continue;

            set_name = monster_set_name_array[spset];
            for (i = LOW_PM; i < NUM_MONSTERS; i++)
            {
                uchar gtflags = 0;
                if (spset >= ACTION_TILE_NO_ACTION && spset < MAX_ACTION_TILES && (mons[i].mflags5 & M5_HALF_SIZED_MONSTER_TILE))
                {
                    gtflags |= GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                }

                if (gender == 1)
                {
                    if (tsd->female_tile_style == 2 && !(mons[i].mflags5 & M5_FEMALE_TILE))
                        continue;
                }

                if (spset == MAX_ACTION_TILES + 1)
                {
                    if (tsd->corpse_tile_style == 2 && !(mons[i].mflags5 & M5_CORPSE_TILE))
                        continue;

                    gtflags = 0;

                    if (mons[i].mflags5 & M5_CORPSE_TILE)
                    {
                        if ((gender == 0 && (mons[i].mflags5 & M5_FULL_SIZED_CORPSE_TILE)) 
                            || (gender == 1 && (mons[i].mflags5 & M5_FULL_SIZED_FEMALE_CORPSE_TILE)))
                            gtflags = GLYPH_TILE_FLAG_FULL_SIZED_ITEM;
                        else
                            gtflags = GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                    }
                    else
                    {
                        if (objects[CORPSE].oc_flags4 & O4_FULL_SIZED_BITMAP)
                            gtflags = GLYPH_TILE_FLAG_FULL_SIZED_ITEM;
                        else
                            gtflags = GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                    }
                }

                if (spset > ACTION_TILE_NO_ACTION && spset < MAX_ACTION_TILES)
                {
                    unsigned long m5_action_flag = M5_ATTACK_TILE << (spset - 1);
                    if (tsd->action_tile_style[spset] == 2 && !(mons[i].mflags5 & m5_action_flag))
                        continue;
                }

                if (spset == MAX_ACTION_TILES)
                {
                    /* Statues should always be full-sized similar to monsters */
                    if (objects[STATUE].oc_flags4 & O4_FULL_SIZED_BITMAP)
                        gtflags = GLYPH_TILE_FLAG_FULL_SIZED_ITEM;
                    else
                        gtflags = GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                }


                short enlargement = 0;
                if (gender == 0)
                {
                    if (spset < MAX_ACTION_TILES)
                        enlargement = mons[i].enlargement.actions[spset];
                    else if (spset == MAX_ACTION_TILES)
                        enlargement = mons[i].enlargement.statue;
                    else if (spset == MAX_ACTION_TILES + 1)
                        enlargement = mons[i].enlargement.corpse;
                }
                else
                {
                    if (spset < MAX_ACTION_TILES)
                        enlargement = mons[i].female_enlargement.actions[spset];
                    else if (spset == MAX_ACTION_TILES)
                        enlargement = mons[i].female_enlargement.statue;
                    else if (spset == MAX_ACTION_TILES + 1)
                        enlargement = mons[i].female_enlargement.corpse;
                }

                if (process_style == 0)
                {
                    Sprintf(buf, "%s,%s,%s,%s", tile_section_name, gender_name, set_name, mons[i].mname);
                    if(enlargement > 0)
                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[enlargement].width_in_tiles, enlargements[enlargement].height_in_tiles, enlargements[enlargement].main_tile_x_coordinate);
                    else
                        Sprintf(eos(buf), ",1,1,0");
//                    int tile_template_idx = (int)(mons[i].mflags6 & M6_MONSTER_TILE_TEMPLATE_MASK);
//                    Sprintf(eos(buf), ",%d,%d,%s", (int)mons[i].mcolor, tile_template_idx, monster_tile_template_name_array[tile_template_idx]);
 
                    Sprintf(eos(buf), "\n");
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    int male_glyph_offset = (spset < MAX_ACTION_TILES) ? get_monster_action_glyph_offset(spset, 0) : spset == MAX_ACTION_TILES ? GLYPH_STATUE_OFF : GLYPH_BODY_OFF; // mon_glyph_offset_array[spset];
                    int female_glyph_offset = (spset < MAX_ACTION_TILES) ? get_monster_action_glyph_offset(spset, 1) : spset == MAX_ACTION_TILES ? GLYPH_FEMALE_STATUE_OFF : GLYPH_FEMALE_BODY_OFF;  //female_mon_glyph_offset_array[spset];

                    if (gender == 0)
                    {
                        tilemaparray[i + male_glyph_offset] = tile_count;
                        tilemapflags[i + male_glyph_offset] |= gtflags;
                        if (spset == ACTION_TILE_NO_ACTION)
                        {
                            enum action_tile_types action;
                            for (action = ACTION_TILE_ATTACK; action < MAX_ACTION_TILES; action++)
                            {
                                if (tsd->action_tile_style[action] != 1)
                                    tilemaparray[i + get_monster_action_glyph_offset(action, 0)] = tile_count;
                            }
                        }
                        else if (spset == ACTION_TILE_ATTACK)
                        {
                            /* Write these again if we get here */
                            enum action_tile_types action_array[6] = { ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK, ACTION_TILE_ITEM_USE };
                            int idx;
                            for (idx = 0; idx < 6; idx++)
                            {
                                enum action_tile_types action = action_array[idx];
                                if (tsd->action_tile_style[action] != 1)
                                    tilemaparray[i + get_monster_action_glyph_offset(action, 0)] = tile_count;
                            }
                        }
                        else if (spset == ACTION_TILE_DOOR_USE)
                        {
                            enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                            int idx;
                            for (idx = 0; idx < 3; idx++)
                            {
                                enum action_tile_types action = action_array[idx];
                                if (tsd->action_tile_style[action] != 1)
                                    tilemaparray[i + get_monster_action_glyph_offset(action, 0)] = tile_count;
                            }
                        }
                    }

                    /* write female versions twice just in case, first as base monster, and then override as female, if we get here */
                    tilemaparray[i + female_glyph_offset] = tile_count;
                    tilemapflags[i + female_glyph_offset] |= gtflags;
                    if (spset == ACTION_TILE_NO_ACTION)
                    {
                        enum action_tile_types action;
                        for (action = ACTION_TILE_ATTACK; action < MAX_ACTION_TILES; action++)
                        {
                            if (tsd->action_tile_style[action] != 1)
                                tilemaparray[i + get_monster_action_glyph_offset(action, 1)] = tile_count;
                        }
                    }
                    else if (spset == ACTION_TILE_ATTACK)
                    {
                        /* Write these again if we get here */
                        enum action_tile_types action_array[6] = { ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK, ACTION_TILE_ITEM_USE };
                        int idx;
                        for (idx = 0; idx < 6; idx++)
                        {
                            enum action_tile_types action = action_array[idx];
                            if (tsd->action_tile_style[action] != 1)
                                tilemaparray[i + get_monster_action_glyph_offset(action, 1)] = tile_count;
                        }
                    }
                    else if (spset == ACTION_TILE_DOOR_USE)
                    {
                        enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                        int idx;
                        for (idx = 0; idx < 3; idx++)
                        {
                            enum action_tile_types action = action_array[idx];
                            if (tsd->action_tile_style[action] != 1)
                                tilemaparray[i + get_monster_action_glyph_offset(action, 1)] = tile_count;
                        }
                    }
                }
                tile_count++;
            }
        }
    }

#define NUM_BASE_TILE_DIRS 3 /* Up, left, up-left */

#if 0
    const char* missile_direction_name_array[NUM_MISSILE_DIRS] = {
            "top-left", "top-center", "top-right",
            "middle-left",  "middle-right",
            "bottom-left", "bottom-center", "bottom-right" };
#endif

    const char* base_missile_direction_name_array[NUM_BASE_TILE_DIRS] = {
            "top-center", "middle-left",  "top-left" };

    /* Object tiles */
    tile_section_name = "objects";
    for (j = 0; j <= 1; j++)
    {
        /* j == 0 -> Normal objs */
        /* j == 1 -> Missile objs */

        if (j == 1 && !tsd->missile_tile_style)
            continue;

        boolean first_scroll_found = FALSE;
        int nameless_idx = 0;
        set_name = (j == 0 ? "normal" : "missile");
        glyph_offset =
            (j == 0 ? GLYPH_OBJ_OFF :
                GLYPH_OBJ_MISSILE_OFF);

        for (i = STRANGE_OBJECT; i < NUM_OBJECTS; i++)
        {
            int missile_tile_num = (objects[i].oc_flags4 & O4_SINGLE_MISSILE_TILE) ? 1 : NUM_BASE_TILE_DIRS;

            if (j == 1)
            {
                if (tsd->missile_tile_style == 2 && !(objects[i].oc_flags4 & O4_MISSILE_TILE))
                    continue;
            }


            const char* oclass_name = def_oc_syms[(int)objects[i].oc_class].name;
            /* Jump over gems that all look the same, just take pieces of glass and luckstone */
            if (objects[i].oc_class == GEM_CLASS)
            {
                if (i <= LAST_GEM || (i > LUCKSTONE && i <= FLINT))
                    continue;
            }
            else if (objects[i].oc_class == SCROLL_CLASS)
            {
                /* Special case for scrolls - they all look the same, except mail, so only two special cases */
                if (first_scroll_found)
                    continue;

                first_scroll_found = TRUE;
                uchar gtflags = (objects[i].oc_flags4 & O4_FULL_SIZED_BITMAP) ? GLYPH_TILE_FLAG_FULL_SIZED_ITEM : GLYPH_TILE_FLAG_HALF_SIZED_TILE;

                /* First, generic scroll tile */
                if (process_style == 0)
                {
                    if (j == 1)
                    {
                        int n;
                        for (n = 0; n < missile_tile_num; n++)
                        {
                            Sprintf(buf, "%s,%s,%s,%s,%s,%s,1,1,0,%d,%d,%s,%d\n", tile_section_name, set_name, oclass_name, 
                                "generic", "scroll", 
                                missile_tile_num == 1 ? "generic" : base_missile_direction_name_array[n], 
                                objects[i].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(i));
                            (void)write(fd, buf, strlen(buf));
                            tile_count++;
                        }
                    }
                    else
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%s", tile_section_name, set_name, oclass_name, "generic", "scroll");

                        if (objects[i].oc_flags4 & O4_FULL_SIZED_BITMAP)
                            Sprintf(eos(buf), ",1");
                        else
                            Sprintf(eos(buf), ",0");

                        if (j == 0 && obj_descr[i].enlargement > 0)
                            Sprintf(eos(buf), ",%d,%d,%d", enlargements[obj_descr[i].enlargement].width_in_tiles, enlargements[obj_descr[i].enlargement].height_in_tiles, enlargements[obj_descr[i].enlargement].main_tile_x_coordinate);
                        else
                            Sprintf(eos(buf), ",1,1,0");
                        Sprintf(eos(buf), ",%d,%d,%s,%d", objects[i].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(i));
                        Sprintf(eos(buf), "\n");
                        (void)write(fd, buf, strlen(buf));
                        tile_count++;
                    }
                }
                else if (process_style == 1)
                {
                    if (j == 1)
                    {
                        if(missile_tile_num == 1)
                        {
                            int n;
                            for (n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
                                /* Found scroll */
                                tilemaparray[i * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;

                                /* Add the tile to all scrolls */
                                int m;
                                for (m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                                {
                                    if (objects[m].oc_class == SCROLL_CLASS)
                                    {
                                        tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                        tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                    }
                                }
                            }
                            tile_count++;
                        }
                        else
                        {
                            for (int bn = 0; bn < NUM_BASE_TILE_DIRS; bn++)
                            {
                                for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                                {
                                    boolean hflip = FALSE;
                                    boolean vflip = FALSE;
                                    boolean isfrombasedir = is_dir_from_base_dir(n, bn, &hflip, &vflip);
                                    if (isfrombasedir)
                                    {
                                        /* Found scroll */
                                        tilemaparray[i * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                        tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                        if(hflip)
                                            tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                        if (vflip)
                                            tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;

                                        /* Add the tile to all scrolls */
                                        for (int m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                                        {
                                            if (objects[m].oc_class == SCROLL_CLASS)
                                            {
                                                tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                                tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                                if (hflip)
                                                    tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                                if (vflip)
                                                    tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                                            }
                                        }
                                    }
                                }
                                tile_count++;
                            }
                        }
                    }
                    else
                    {
                        tilemaparray[i + glyph_offset] = tile_count;
                        tilemapflags[i + glyph_offset] |= gtflags;

                        /* Add the tile to all scrolls */
                        for (int m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                        {
                            if (objects[m].oc_class == SCROLL_CLASS)
                            {
                                /* Other scroll's main tile */
                                tilemaparray[m + glyph_offset] = tile_count;
                                tilemapflags[m + glyph_offset] |= gtflags;

                                /* Add others if they do not have their own */
                                if (j == 0)
                                {
                                    if (tsd->missile_tile_style != 1)
                                    {
                                        int glyph_offset4 = GLYPH_OBJ_MISSILE_OFF;
                                        for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                                        {
                                            tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset4] = tile_count;
                                            tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset4] |= gtflags | GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                        }
                                    }
                                }
                            }
                        }

                        tile_count++;
                    }
                }
                else
                {
                    if (j == 1)
                        tile_count += missile_tile_num;
                    else
                        tile_count++;
                }

                /* Second, scroll of mail */
                if (process_style == 0)
                {
                    if (j == 1)
                    {
                        for (int n = 0; n < missile_tile_num; n++)
                        {
                            Sprintf(buf, "%s,%s,%s,%s,%s,%s,1,1,0,%d,%d,%s,%d\n", tile_section_name, set_name, oclass_name, 
                                "mail", "envelope", 
                                missile_tile_num == 1 ? "generic" : base_missile_direction_name_array[n], objects[i].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(i));
                            (void)write(fd, buf, strlen(buf));
                            if(missile_tile_num != 1)
                                tile_count++;
                        }
                        if(missile_tile_num == 1)
                            tile_count++;
                    }
                    else
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%s,0,1,1,0,%d,%d,%s,%d\n", tile_section_name, set_name, oclass_name, "mail", "envelope", objects[i].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(i));
                        (void)write(fd, buf, strlen(buf));
                        tile_count++;
                    }
                }
                else if (process_style == 1)
                {
                    /* Add the tile the scroll "mail" */
                    if (j == 1)
                    {
                        if (missile_tile_num == 1)
                        {
                            for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
#ifdef MAIL
                                tilemaparray[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                tilemapflags[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
#endif
                            }
                            tile_count++;
                        }
                        else
                        {
                            for (int bn = 0; bn < NUM_BASE_TILE_DIRS; bn++)
                            {
                                for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                                {
                                    boolean hflip = FALSE;
                                    boolean vflip = FALSE;
                                    boolean isfrombasedir = is_dir_from_base_dir(n, bn, &hflip, &vflip);
                                    if (isfrombasedir)
                                    {
#ifdef MAIL
                                        tilemaparray[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                        tilemapflags[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                        if (hflip)
                                            tilemapflags[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                        if (vflip)
                                            tilemapflags[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
#endif
                                    }
                                }
                                tile_count++;
                            }
                        }
                    }
                    else
                    {
#ifdef MAIL
                        gtflags = (objects[SCR_MAIL].oc_flags4 & O4_FULL_SIZED_BITMAP) ? GLYPH_TILE_FLAG_FULL_SIZED_ITEM : GLYPH_TILE_FLAG_HALF_SIZED_TILE;

                        /* Main tile */
                        tilemaparray[SCR_MAIL + glyph_offset] = tile_count;
                        tilemapflags[SCR_MAIL + glyph_offset] |= gtflags;

                        /* Add to others, if they do not have tiles of their own */
                        if (j == 0)
                        {
                            if (tsd->missile_tile_style != 1)
                            {
                                int glyph_offset4 = GLYPH_OBJ_MISSILE_OFF;
                                for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                                {
                                    tilemaparray[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset4] = tile_count;
                                    tilemapflags[SCR_MAIL * NUM_MISSILE_DIRS + n + glyph_offset4] |= gtflags | GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                }
                            }
                        }
#endif
                        tile_count++;
                    }
                }
                else
                {
                    if (j == 1)
                        tile_count += missile_tile_num;
                    else
                        tile_count++;
                }

                /* Move to next item after having found the first scroll */
                continue;
            }

            /* Normal item case starts here */
            int orig_idx = i;
            for (int idx = STRANGE_OBJECT; idx < NUM_OBJECTS; idx++)
            {
                if (objects[idx].oc_descr_idx == i)
                {
                    orig_idx = idx;
                    break;
                }
            }

            boolean nameless = !OBJ_NAME(objects[i]);
            boolean no_description = !obj_descr[objects[i].oc_name_idx].oc_descr;
            char nameless_name[BUFSZ];
            Sprintf(nameless_name, "nameless-%d", nameless_idx);
            if (!OBJ_NAME(objects[i]))
            {
                nameless = TRUE;
                nameless_idx++;
            }

            /* Full-sized item and other flags */
            uchar gtflags = (objects[i].oc_flags4 & O4_FULL_SIZED_BITMAP) ? GLYPH_TILE_FLAG_FULL_SIZED_ITEM : GLYPH_TILE_FLAG_HALF_SIZED_TILE;
            gtflags |= (objects[i].oc_flags4 & O4_FLOOR_TILE) ? GLYPH_TILE_FLAG_HAS_FLOOR_TILE : 0;

            if (process_style == 0)
            {
                if (j == 1)
                {
                    for (int n = 0; n < missile_tile_num; n++)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%s,%s,1,1,0,%d,%d,%s,%d\n", tile_section_name, set_name, oclass_name,
                            nameless ? nameless_name : OBJ_NAME(objects[i]),
                            no_description ? "no description" : obj_descr[objects[i].oc_name_idx].oc_descr,
                            missile_tile_num == 1 ? "generic" : base_missile_direction_name_array[n], 
                            objects[orig_idx].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(orig_idx));
                        (void)write(fd, buf, strlen(buf));
                        tile_count++;
                    }
                }
                else
                {
                    Sprintf(buf, "%s,%s,%s,%s,%s", tile_section_name, set_name, oclass_name,
                        nameless ? nameless_name : OBJ_NAME(objects[i]),
                        no_description ? "no description" : obj_descr[objects[i].oc_name_idx].oc_descr
                    );

                    if (objects[i].oc_flags4 & O4_FULL_SIZED_BITMAP)
                        Sprintf(eos(buf), ",1");
                    else
                        Sprintf(eos(buf), ",0");

                    if (j == 0 && obj_descr[i].enlargement > 0)
                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[obj_descr[i].enlargement].width_in_tiles, enlargements[obj_descr[i].enlargement].height_in_tiles, enlargements[obj_descr[i].enlargement].main_tile_x_coordinate);
                    else
                        Sprintf(eos(buf), ",1,1,0");
                    Sprintf(eos(buf), ",%d,%d,%s,%d", objects[orig_idx].oc_color, objects[i].oc_subtyp, get_otyp_subtype_name(i), has_otyp_floor_tile(orig_idx));
                    Sprintf(eos(buf), "\n");
                    (void)write(fd, buf, strlen(buf));
                    tile_count++;
                }
            }
            else if (process_style == 1)
            {
                if (j == 1)
                {
                    if (missile_tile_num == 1)
                    {
                        for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                        {
                            /* Write to the tile to the main glyph */
                            tilemaparray[i * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                            tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;

                            /* If this is a piece of glass or luckstone, add the tile to all other gems with the same color; others have been skipped */
                            if (objects[i].oc_class == GEM_CLASS && (i > LAST_GEM && i <= LUCKSTONE))
                            {
                                for (int m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                                {
                                    if (objects[m].oc_class == GEM_CLASS
                                        && (m <= LAST_GEM || (m > LUCKSTONE && m <= FLINT))
                                        && objects[m].oc_color == objects[i].oc_color
                                        )
                                    {
                                        /* Write to the tile to the main glyph */
                                        tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                        tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                    }
                                }
                            }
                        }
                        tile_count++;
                    }
                    else
                    {
                        for (int bn = 0; bn < NUM_BASE_TILE_DIRS; bn++)
                        {
                            for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
                                boolean hflip = FALSE;
                                boolean vflip = FALSE;
                                boolean isfrombasedir = is_dir_from_base_dir(n, bn, &hflip, &vflip);
                                if (isfrombasedir)
                                {

                                    /* Write to the tile to the main glyph */
                                    tilemaparray[i * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                    tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                    if (hflip)
                                        tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                    if(vflip)
                                        tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;

                                    /* If this is a piece of glass or luckstone, add the tile to all other gems with the same color; others have been skipped */
                                    if (objects[i].oc_class == GEM_CLASS && (i > LAST_GEM && i <= LUCKSTONE))
                                    {
                                        for (int m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                                        {
                                            if (objects[m].oc_class == GEM_CLASS
                                                && (m <= LAST_GEM || (m > LUCKSTONE && m <= FLINT))
                                                && objects[m].oc_color == objects[i].oc_color
                                                )
                                            {
                                                /* Write to the tile to the main glyph */
                                                tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                                tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                                if (hflip)
                                                    tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                                if (vflip)
                                                    tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                                            }
                                        }
                                    }
                                }
                            }
                            tile_count++;
                        }
                    }
                }
                else
                {
                    /* Write to the tile to the main glyph */
                    tilemaparray[i + glyph_offset] = tile_count;
                    tilemapflags[i + glyph_offset] |= gtflags;

                    if (j == 0)
                    {
                        if (tsd->missile_tile_style != 1)
                        {
                            int glyph_offset4 = GLYPH_OBJ_MISSILE_OFF;
                            for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
                                tilemaparray[i * NUM_MISSILE_DIRS + n + glyph_offset4] = tile_count;
                                tilemapflags[i * NUM_MISSILE_DIRS + n + glyph_offset4] |= gtflags | GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                            }
                        }
                    }

                    /* If this is a piece of glass or luckstone, add the tile to all other gems with the same color; others have been skipped */
                    if (objects[i].oc_class == GEM_CLASS && (i > LAST_GEM && i <= LUCKSTONE))
                    {
                        for (int m = STRANGE_OBJECT; m < NUM_OBJECTS; m++)
                        {
                            if (objects[m].oc_class == GEM_CLASS
                                && (m <= LAST_GEM || (m > LUCKSTONE && m <= FLINT))
                                && objects[m].oc_color == objects[i].oc_color
                                )
                            {
                                /* Write to the tile to the main glyph */
                                tilemaparray[m + glyph_offset] = tile_count;
                                tilemapflags[m + glyph_offset] |= gtflags;

                                /* Write to the tile to the inventory, lit, and inventory lit glyphs if they do not have their own */
                                if (j == 0)
                                {
                                    if (tsd->missile_tile_style != 1)
                                    {
                                        int glyph_offset4 = GLYPH_OBJ_MISSILE_OFF;
                                        for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                                        {
                                            tilemaparray[m * NUM_MISSILE_DIRS + n + glyph_offset4] = tile_count;
                                            tilemapflags[m * NUM_MISSILE_DIRS + n + glyph_offset4] |= gtflags | GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    /* Write generic corpse and statue tiles */
                    if (j == 0 &&
                         ((tsd->corpse_tile_style != 1 && i == CORPSE)
                          || (!tsd->statue_tile_style && i == STATUE)
                        )
                       )
                    {
                        for (gender = 0; gender <= 1; gender++)
                        {
                            for (int k = LOW_PM; k < NUM_MONSTERS; k++)
                            {
                                if (i == CORPSE && tsd->corpse_tile_style == 2 && (mons[k].mflags5 & M5_CORPSE_TILE))
                                    continue;

                                int glyph_offset2 = (gender == 0 ? (i == CORPSE ? GLYPH_BODY_OFF : GLYPH_STATUE_OFF) : (i == CORPSE ? GLYPH_FEMALE_BODY_OFF : GLYPH_FEMALE_STATUE_OFF));
                                tilemaparray[k + glyph_offset2] = tile_count;
                                tilemapflags[k + glyph_offset2] |= gtflags;
                            }
                        }
                    }


                    tile_count++;
                }
            }
            else
            {
                if (j == 1)
                    tile_count += missile_tile_num;
                else
                    tile_count++;
            }
        }
    }

    /* Artifact tiles */
    tile_section_name = "artifacts";
    for (j = 0; j <= 1; j++)
    {
        if (j == 1 && !tsd->missile_tile_style)
            continue;

        set_name = (j == 0 ? "normal" :"missile");
        glyph_offset = (j == 0 ? GLYPH_ARTIFACT_OFF : 
            GLYPH_ARTIFACT_MISSILE_OFF);

        for (i = 1; i <= NUM_ARTIFACTS; i++)
        {
            int missile_tile_num = (artilist[i].aflags2 & AF2_SINGLE_MISSILE_TILE) ? 1 : NUM_BASE_TILE_DIRS;

            if (j == 1)
            {
                if (tsd->missile_tile_style == 2 && !(artilist[i].aflags2 & AF2_MISSILE_TILE))
                    continue;
            }


            int base_item = artilist[i].otyp;
            boolean no_description = !artilist[i].desc;
            boolean no_base_item_name = !OBJ_NAME(objects[base_item]);
            boolean no_base_item_description = !obj_descr[objects[base_item].oc_name_idx].oc_descr;

            uchar gtflags = (artilist[i].aflags2 & AF2_FULL_SIZED_ITEM) ? GLYPH_TILE_FLAG_FULL_SIZED_ITEM : GLYPH_TILE_FLAG_HALF_SIZED_TILE;
            gtflags |= (artilist[i].aflags2 & AF2_FLOOR_TILE) ? GLYPH_TILE_FLAG_HAS_FLOOR_TILE : 0;

            if (process_style == 0)
            {
                if (j == 1)
                {
                    for (int n = 0; n < missile_tile_num; n++)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%s,%s,%s,1,1,0,%d,%d,%s,%s,%d\n", tile_section_name, set_name,
                            artilist[i].name,
                            no_description ? "no artifact description" : artilist[i].desc,
                            no_base_item_name ? "nameless base item" : OBJ_NAME(objects[base_item]),
                            no_base_item_description ? "no base item description" : obj_descr[objects[base_item].oc_name_idx].oc_descr,
                            missile_tile_num == 1 ? "generic " : base_missile_direction_name_array[n], 
                            artilist[i].ocolor, objects[artilist[i].otyp].oc_subtyp, get_otyp_subtype_name(artilist[i].otyp), def_oc_syms[(int)objects[artilist[i].otyp].oc_class].name,
                            has_artifact_floor_tile(i)
                        );
                        (void)write(fd, buf, strlen(buf));
                        tile_count++;
                    }
                }
                else
                {
                    Sprintf(buf, "%s,%s,%s,%s,%s,%s", tile_section_name, set_name,
                        artilist[i].name,
                        no_description ? "no artifact description" : artilist[i].desc,
                        no_base_item_name ? "nameless base item" : OBJ_NAME(objects[base_item]),
                        no_base_item_description ? "no base item description" : obj_descr[objects[base_item].oc_name_idx].oc_descr
                    );

                    if (objects[i].oc_flags4 & O4_FULL_SIZED_BITMAP)
                        Sprintf(eos(buf), ",1");
                    else
                        Sprintf(eos(buf), ",0");

                    if (j == 0 && artilist[i].enlargement > 0)
                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[artilist[i].enlargement].width_in_tiles, enlargements[artilist[i].enlargement].height_in_tiles, enlargements[artilist[i].enlargement].main_tile_x_coordinate);
                    else
                        Sprintf(eos(buf), ",1,1,0");
                    Sprintf(eos(buf), ",%d,%d,%s,%s", artilist[i].ocolor, objects[artilist[i].otyp].oc_subtyp, get_otyp_subtype_name(artilist[i].otyp), def_oc_syms[(int)objects[artilist[i].otyp].oc_class].name);
                    
                    if (has_artifact_floor_tile(i))
                        Sprintf(eos(buf), ",1");
                    else
                        Sprintf(eos(buf), ",0");

                    Sprintf(eos(buf), "\n");
                    (void)write(fd, buf, strlen(buf));
                    tile_count++;
                }
            }
            else if (process_style == 1)
            {
                if (j == 1)
                {
                    if (missile_tile_num == 1)
                    {
                        for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                        {
                            tilemaparray[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                            tilemapflags[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                        }
                        tile_count++;
                    }
                    else
                    {
                        for (int bn = 0; bn < NUM_BASE_TILE_DIRS; bn++)
                        {
                            for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
                                boolean hflip = FALSE;
                                boolean vflip = FALSE;
                                boolean isfrombasedir = is_dir_from_base_dir(n, bn, &hflip, &vflip);
                                if (isfrombasedir)
                                {
                                    tilemaparray[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] = tile_count;
                                    tilemapflags[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] &= ~GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                                    if(hflip)
                                        tilemapflags[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                    if (vflip)
                                        tilemapflags[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                                }
                            }
                            tile_count++;
                        }
                    }
                }
                else
                {
                    tilemaparray[(i - 1) + glyph_offset] = tile_count;
                    tilemapflags[(i - 1) + glyph_offset] |= gtflags;

                    if (j == 0)
                    {
                        if (tsd->missile_tile_style != 1)
                        {
                            int glyph_offset4 = GLYPH_ARTIFACT_MISSILE_OFF;
                            for (int n = 0; n < NUM_MISSILE_DIRS; n++)
                            {
                                tilemaparray[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset4] = tile_count;
                                tilemapflags[(i - 1) * NUM_MISSILE_DIRS + n + glyph_offset4] |= gtflags | GLYPH_TILE_FLAG_NORMAL_ITEM_AS_MISSILE;
                            }
                        }
                    }
                    tile_count++;
                }
            }
            else
            {
                if (j == 1)
                    tile_count += missile_tile_num;
                else
                    tile_count++;
            }
        }
    }

    /* CMAP tiles */
    int cmapset;
    for (cmapset = 0; cmapset <= 1; cmapset++)
    {
        tile_section_name = (cmapset == 0 ? "cmap" : "broken-cmap");
        int base_glyph_offset = (cmapset == 0 ? GLYPH_CMAP_OFF : GLYPH_BROKEN_CMAP_OFF);
        //int base_variation_glyph_offset = (cmapset == 0 ? GLYPH_CMAP_VARIATION_OFF : GLYPH_BROKEN_CMAP_VARIATION_OFF);

        for (int tileset_cmap_idx = 0; tileset_cmap_idx < MAX_CMAP_TYPES; tileset_cmap_idx++)
        {
            char namebuf[BUFSZ];
            if (tsd->cmap_names[tileset_cmap_idx] && strcmp(tsd->cmap_names[tileset_cmap_idx], ""))
                Sprintf(namebuf, "%s", tsd->cmap_names[tileset_cmap_idx]);
            else
                Sprintf(namebuf, "unnamed-cmap-%d", tileset_cmap_idx);

            for (i = 0; i < NUM_CMAP_TYPE_CHARS; i++)
            {
                if (tileset_cmap_idx > 0)
                {
                    if (!defsyms[i].included_in_cmap[tileset_cmap_idx])
                        continue;
                }

                if (cmapset == 1 && defsyms[i].has_broken_tile == FALSE)
                    continue;

                if (process_style == 0)
                {
                    Sprintf(buf, "%s,%s,%s,%s", tile_section_name, namebuf, get_cmap_tilename(i),
                        (defsyms[i].explanation && strcmp(defsyms[i].explanation, "")) ? defsyms[i].explanation : "no description");
                    int enl_idx = (cmapset == 0 ? defsyms[i].enlargement[tileset_cmap_idx] : defsyms[i].broken_enlargement[tileset_cmap_idx]);
                    if (enl_idx > 0)
                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl_idx].width_in_tiles, enlargements[enl_idx].height_in_tiles, enlargements[enl_idx].main_tile_x_coordinate);
                    else
                        Sprintf(eos(buf), ",1,1,0");
                    Sprintf(eos(buf), "\n");
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    glyph_offset = base_glyph_offset + tileset_cmap_idx * NUM_CMAP_TYPE_CHARS;
                    tilemaparray[i + glyph_offset] = tile_count;
                    if (i == S_extra_boulder)
                    {
                        if (objects[BOULDER].oc_flags4 & O4_FULL_SIZED_BITMAP)
                            tilemapflags[i + glyph_offset] |= GLYPH_TILE_FLAG_FULL_SIZED_ITEM;
                        else
                            tilemapflags[i + glyph_offset] |= GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                    }

                    /* copy an out-limited tiles to all limited cmaps */
                    for (int tileset_cmap_idx2 = 1; tileset_cmap_idx2 < MAX_CMAP_TYPES; tileset_cmap_idx2++)
                    {
                        if (tileset_cmap_idx2 != tileset_cmap_idx)
                        {
                            boolean included_in_this_tileset_cmap = defsyms[i].included_in_cmap[tileset_cmap_idx2];
                            int base_tileset_cmap = 0; // defsyms[i].base_cmap[tileset_cmap_idx2];
                            set_base_tileset_cmap(&base_tileset_cmap, i, tileset_cmap_idx2);
                            if (
                                (!included_in_this_tileset_cmap && base_tileset_cmap == tileset_cmap_idx)
                                )
                            {
                                int glyph_offset2 = base_glyph_offset + tileset_cmap_idx2 * NUM_CMAP_TYPE_CHARS;
                                tilemaparray[i + glyph_offset2] = tile_count;
                                if (i == S_extra_boulder)
                                {
                                    if (objects[BOULDER].oc_flags4 & O4_FULL_SIZED_BITMAP)
                                        tilemapflags[i + glyph_offset2] |= GLYPH_TILE_FLAG_FULL_SIZED_ITEM;
                                    else
                                        tilemapflags[i + glyph_offset2] |= GLYPH_TILE_FLAG_HALF_SIZED_TILE;
                                }
                            }
                        }
                    }
                }
                tile_count++;
            }
        }
    }

    /* CMAP variation tiles */
    for (cmapset = 0; cmapset <= 1; cmapset++)
    {
        tile_section_name = (cmapset == 0 ? "cmap-variation" : "broken-cmap-variation");
        int base_glyph_offset = (cmapset == 0 ? GLYPH_CMAP_VARIATION_OFF : GLYPH_BROKEN_CMAP_VARIATION_OFF);

        for (int tileset_cmap_idx = 0; tileset_cmap_idx < MAX_CMAP_TYPES; tileset_cmap_idx++)
        {
            char namebuf[BUFSZ];
            if (tsd->cmap_names[tileset_cmap_idx] && strcmp(tsd->cmap_names[tileset_cmap_idx], ""))
                Sprintf(namebuf, "%s", tsd->cmap_names[tileset_cmap_idx]);
            else
                Sprintf(namebuf, "unnamed-cmap-%d", tileset_cmap_idx);

            for (i = 0; i < MAX_VARIATIONS; i++)
            {
                if (tileset_cmap_idx > 0)
                {
                    if (!defsym_variations[i].included_in_cmap[tileset_cmap_idx])
                        continue;
                }

                if (cmapset == 1 && defsym_variations[i].has_broken_tile == FALSE)
                    continue;

                if (process_style == 0)
                {
                    Sprintf(buf, "%s,%s,%s", tile_section_name, namebuf,
                        (defsym_variations[i].variation_name && strcmp(defsym_variations[i].variation_name, "")) ? defsym_variations[i].variation_name : "no-variation-name");
                    int enl_idx = (cmapset == 0 ? defsym_variations[i].enlargement[tileset_cmap_idx] : defsym_variations[i].broken_enlargement[tileset_cmap_idx]);
                    if (enl_idx > 0)
                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl_idx].width_in_tiles, enlargements[enl_idx].height_in_tiles, enlargements[enl_idx].main_tile_x_coordinate);
                    else
                        Sprintf(eos(buf), ",1,1,0");
                    Sprintf(eos(buf), "\n");
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    glyph_offset = base_glyph_offset + tileset_cmap_idx * MAX_VARIATIONS;
                    tilemaparray[i + glyph_offset] = tile_count;

                    /* copy an out-limited tile to all limited cmaps */
                    for (int tileset_cmap_idx2 = 1; tileset_cmap_idx2 < MAX_CMAP_TYPES; tileset_cmap_idx2++)
                    {
                        if (tileset_cmap_idx2 != tileset_cmap_idx)
                        {
                            boolean included_in_this_tileset_cmap = defsym_variations[i].included_in_cmap[tileset_cmap_idx2];
                            int base_tileset_cmap = 0; //defsym_variations[i].base_cmap[tileset_cmap_idx2]
                            set_base_tileset_cmap_variation(&base_tileset_cmap, i, tileset_cmap_idx2);

                            if (
                                (!included_in_this_tileset_cmap && base_tileset_cmap == tileset_cmap_idx)
                                )
                            {
                                int glyph_offset2 = base_glyph_offset + tileset_cmap_idx2 * MAX_VARIATIONS;
                                tilemaparray[i + glyph_offset2] = tile_count;
                            }
                        }
                    }
                }
                tile_count++;
            }
        }
    }

    /* Miscellaneous tiles */
    tile_section_name = "misc";
    for (int misc_idx = 0; misc_idx < 6; misc_idx++)
    {
        set_name = (misc_idx == 0 ? "invisible" : misc_idx == 1 ? "explode" : misc_idx == 2 ? "zap" :
            misc_idx == 3 ? "swallow" : misc_idx == 4 ? "warning" : "worm");

        if (misc_idx == 0)
        {
            if (NUM_INVIS_GLYPHS == 1)
            {
                if (process_style == 0)
                {
                    Sprintf(buf, "%s,%s,generic,1,1,0\n", tile_section_name, set_name);
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    tilemaparray[GLYPH_INVIS_OFF] = tile_count;
                }
                tile_count++;
            }
            else
            {
                for (i = 0; i < NUM_INVIS_GLYPHS; i++)
                {
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,tile-%d,1,1,0\n", tile_section_name, set_name, i);
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        glyph_offset = GLYPH_INVIS_OFF;
                        tilemaparray[i + glyph_offset] = tile_count;
                    }
                    tile_count++;
                }
            }
        }
        else if (misc_idx == 1)
        {
            const char* explosion_direction_name_array[MAX_EXPLOSION_CHARS] = {
                    "top-left", "top-center", "top-right",
                    "middle-left", "middle-center", "middle-right",
                    "bottom-left", "bottom-center", "bottom-right" };

            for (j = 0; j < MAX_EXPLOSIONS; j++)
            {
                const char* explosion_name = explosion_type_definitions[j].name;
                for (i = 0; i < MAX_EXPLOSION_CHARS; i++)
                {
                    const char* explosion_direction_name = explosion_direction_name_array[i];
                    int x_coord = i % 3;
                    int y_coord = i / 3;
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%d,%d,3,3,1,1,0\n", tile_section_name, set_name, explosion_name, explosion_direction_name, x_coord, y_coord);
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        glyph_offset = GLYPH_EXPLODE_OFF + MAX_EXPLOSION_CHARS * j;
                        tilemaparray[i + glyph_offset] = tile_count;
                    }
                    tile_count++;
                }
            }
        }
        else if (misc_idx == 2)
        {
#define ZAP_TEMPLATE_WIDTH 5
#define ZAP_TEMPLATE_HEIGHT 4
#define NUM_ZAP_BASE_TILES 18
/* If updating, also update ZAP_INDEX_WITH_FIRST_TILE from tiledata.h */

#if 0
            const char* zap_direction_name_array[NUM_ZAP_CHARS] = {
                "up", "left", "up-left", "up-right",
                "down", "right", "down-right", "down-left",
                "bounce-top-clockwise", "bounce-right-clockwise", "bounce-bottom-clockwise", "bounce-left-clockwise",
                "bounce-top-counterclockwise", "bounce-right-counterclockwise", "bounce-bottom-counterclockwise", "bounce-left-counterclockwise",
                "back-bounce-up", "back-bounce-left", "back-bounce-up-left", "back-bounce-up-right",
                "back-bounce-down", "back-bounce-right", "back-bounce-down-right", "back-bounce-down-left",
                "up-left-extra-up", "up-left-extra-left", "up-right-extra-up", "up-right-extra-right",
                "down-right-extra-down", "down-right-extra-right", "down-left-extra-down", "down-left-extra-left",
                "leading-edge-up-left", "leading-edge-up-right", "leading-edge-down-right", "leading-edge-down-left",
                "up-left-trailing-edge-down", "up-left-trailing-edge-right", "up-right-trailing-edge-down", "up-right-trailing-edge-left",
                "down-right-trailing-edge-up", "down-right-trailing-edge-left", "down-left-trailing-edge-up", "down-left-trailing-edge-right",
                "trailing-edge-up-left", "trailing-edge-up-right", "trailing-edge-down-right", "trailing-edge-down-left",
                "leading-edge-up", "leading-edge-right", "leading-edge-down", "leading-edge-left",
                "trailing-edge-up", "trailing-edge-right", "trailing-edge-down", "trailing-edge-left",
            };
#endif

            const char* zap_template_tile_name_array[ZAP_TEMPLATE_WIDTH * ZAP_TEMPLATE_HEIGHT] = {
                "leading-edge-up", "leading-edge-left", "left", "trailing-edge-left", "back-bounce-left",
                "up", "bounce-top-clockwise", "leading-edge-up-left", "up-left-extra-up", "back-bounce-up-left",
                "trailing-edge-up", "bounce-right-clockwise", "up-left-extra-left", "up-left", "up-left-trailing-edge-right",
                "back-bounce-up", "", "", "up-left-trailing-edge-down", "trailing-edge-up-left",
            };

            int template_width = ZAP_TEMPLATE_WIDTH;
            int template_height = ZAP_TEMPLATE_HEIGHT;
            int zap_template_tiles = template_width * template_height;

            for (j = 0; j < MAX_ZAP_TYPES; j++)
            {
                const char* zap_name = zap_type_definitions[j].name;
                const int zap_color = zap_type_definitions[j].color;
                int bi = -1;
                for(int ti = 0; ti < zap_template_tiles; ti++)
                {
                    if(!strcmp(zap_template_tile_name_array[ti], ""))
                        continue;

                    bi++;

                    const char* base_zap_direction_name = zap_template_tile_name_array[ti];
                    int bx_coord = ti % template_width;
                    int by_coord = ti / template_width;
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%d,%d,%d,%d,1,1,0,%d,%d,%d\n", tile_section_name, set_name, zap_name, base_zap_direction_name, bx_coord, by_coord, template_width, template_height, 0, 0, zap_color);
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        for (i = 0; i < NUM_ZAP_CHARS; i++)
                        {
                            boolean flip_horizontal = FALSE;
                            boolean flip_vertical = FALSE;
                            boolean isfrombasezapchar = is_zap_char_from_base_zap_char(i, bi, &flip_horizontal, &flip_vertical);
                            if (isfrombasezapchar)
                            {
                                glyph_offset = GLYPH_ZAP_OFF + NUM_ZAP_CHARS * j;
                                tilemaparray[i + glyph_offset] = tile_count;
                                if(flip_horizontal)
                                    tilemapflags[i + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                if (flip_vertical)
                                    tilemapflags[i + glyph_offset] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                            }

                        }

                    }

                    tile_count++;
                }
            }
        }
        else if (misc_idx == 3)
        {
            const char* swallow_direction_name_array[MAX_SWALLOW_CHARS] = {
                    "top-left", "top-center", "top-right",
                    "middle-left", "middle-center", "middle-right",
                    "bottom-left", "bottom-center", "bottom-right" };

            boolean first_found = FALSE;
            for (j = 0; j < NUM_MONSTERS; j++)
            {
                if (!attacktype(&mons[j], AT_ENGL))
                    continue;

                for (i = 0; i < MAX_SWALLOW_CHARS; i++)
                {
                    const char* swallow_direction_name = swallow_direction_name_array[i];
                    int x_coord = i % 3;
                    int y_coord = i / 3;
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%d,%d,3,3,1,1,0\n", tile_section_name, set_name, mons[j].mname, swallow_direction_name, x_coord, y_coord);
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        glyph_offset = GLYPH_SWALLOW_OFF + MAX_SWALLOW_CHARS * j;
                        tilemaparray[i + glyph_offset] = tile_count;
                    }
                    tile_count++;
                }

                /* Write the first found swallow tile set for all monsters, just in case */
                if (!first_found && process_style == 1)
                {
                    first_found = TRUE;
                    for (int k = 0; k < NUM_MONSTERS; k++)
                    {
                        for (int m = 0; m < MAX_SWALLOW_CHARS; m++)
                        {
                            glyph_offset = GLYPH_SWALLOW_OFF + MAX_SWALLOW_CHARS * k;
                            tilemaparray[m + glyph_offset] = tile_count - MAX_SWALLOW_CHARS + m;
                        }
                    }
                }
            }
        }
        else if (misc_idx == 4)
        {
            for (i = 0; i < WARNCOUNT; i++)
            {
                if (process_style == 0)
                {
                    Sprintf(buf, "%s,%s,warn-level-%d,1,1,0\n", tile_section_name, set_name, i);
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    glyph_offset = GLYPH_WARNING_OFF;
                    tilemaparray[i + glyph_offset] = tile_count;
                }
                tile_count++;
            }
        }
        else if (misc_idx == 5)
        {        
            const char* long_worm_type_names[MAX_LONG_WORM_TYPES] = {
                "adult", "elder",
             };

            const char* worm_tile_name_array[MAX_WORM_TILES] = {
                "WORM_TILE_WORM_IS_RIGHT_GOING_UP_LEFT",
                "WORM_TILE_WORM_IS_DOWN_GOING_UP_LEFT",
                "WORM_TILE_WORM_IS_LEFT_GOING_DOWN_RIGHT",
                "WORM_TILE_WORM_IS_UP_GOING_DOWN_RIGHT",
                "WORM_TILE_WORM_SEGMENT_GOING_UP",
                "WORM_TILE_WORM_SEGMENT_GOING_LEFT",
                "WORM_TILE_WORM_SEGMENT_GOING_UP_LEFT",
                "WORM_TILE_WORM_SEGMENT_GOING_DOWN_RIGHT",
                "WORM_TILE_WORM_SEGMENT_COMING_FROM_UP",
                "WORM_TILE_WORM_SEGMENT_COMING_FROM_LEFT",
                "WORM_TILE_WORM_SEGMENT_COMING_FROM_UP_LEFT",
                "WORM_TILE_WORM_SEGMENT_COMING_FROM_DOWN_RIGHT",
                "WORM_TILE_WORM_SEGMENT_BOTTOM",
                "WORM_TILE_WORM_SEGMENT_MIDDLE",
                "WORM_TILE_WORM_SEGMENT_TOP",
                "WORM_TILE_WORM_RESERVED",
                "WORM_TILE_WORM_TAIL_GOING_UP",
                "WORM_TILE_WORM_TAIL_GOING_LEFT",
                "WORM_TILE_WORM_TAIL_GOING_UP_LEFT",
                "WORM_TILE_WORM_TAIL_GOING_DOWN_RIGHT",
                "WORM_TILE_WORM_HEAD_COMING_FROM_UP",
                "WORM_TILE_WORM_HEAD_COMING_FROM_LEFT",
                "WORM_TILE_WORM_HEAD_COMING_FROM_UP_LEFT",
                "WORM_TILE_WORM_HEAD_COMING_FROM_DOWN_RIGHT", 
            };

            int template_width = 4;
            int template_height = MAX_WORM_TILES / template_width;

            for (j = 0; j < MAX_LONG_WORM_TYPES; j++)
            {
                const char* long_worm_type_name = long_worm_type_names[j];
                for (i = 0; i < MAX_WORM_TILES; i++)
                {
                    const char* worm_tile_name = worm_tile_name_array[i];
                    int x_coord = i % template_width;
                    int y_coord = i / template_width;
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,%s,%s,%d,%d,%d,%d,1,1,0\n", tile_section_name, set_name, long_worm_type_name, worm_tile_name, x_coord, y_coord, template_width, template_height);
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        glyph_offset = GLYPH_WORM_OFF + MAX_WORM_TILES * j;
                        tilemaparray[i + glyph_offset] = tile_count;
                    }
                    tile_count++;
                }
            }
        }    
    }


    /* Player tiles */
    tile_section_name = "player";
    const char* player_set_name_array[MAX_ACTION_TILES] = { "normal", "attack", "throw", "fire", "cast-nodir", "cast-dir", "special-attack", "kick", "passive-defense", "special-attack-2", "special-attack-3", "item-use", "door-use", "death" };

    /* Initialize glyphs first with monster tiles */
    enum action_tile_types plaction;
    for (plaction = 0; plaction < MAX_ACTION_TILES; plaction++)
    {
        set_name = player_set_name_array[plaction];
        int player_glyph_offset = get_player_action_glyph_offset(plaction);
        if (tsd->player_tile_style == 0 || tsd->player_tile_style == 3) /* For style 3, fill out all cases with monster tiles, and override below */
        {
            /* Use player monster icons */
            if (process_style == 1)
            {
                for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
                {
                    int role_as_monster = roles[roleidx].monsternum;
                    for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
                    {
                        for (gender = 0; gender <= 1; gender++)
                        {
                            for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                            {
                                for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                                {
                                    int player_glyph = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel) + player_glyph_offset;
                                    tilemaparray[player_glyph] = tilemaparray[role_as_monster + get_monster_action_glyph_offset(plaction, gender)]; // tilemaparray[role_as_monster + ((gender == 0) ? player_as_mon_glyph_offset_array[plaction] : player_as_female_mon_glyph_offset_array[plaction])];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Now main player tiles */
    for (plaction = ACTION_TILE_NO_ACTION; plaction < MAX_ACTION_TILES; plaction++)
    {
        set_name = player_set_name_array[plaction];
        int player_glyph_offset = get_player_action_glyph_offset(plaction);

        if (tsd->player_tile_style == 1)
        {
            if (!has_generic_player_action_tile(plaction))
                continue;

            if (process_style == 0)
            {
                Sprintf(buf, "%s,%s,generic,1,1,0\n", tile_section_name, set_name);
                (void)write(fd, buf, strlen(buf));
            }
            else if (process_style == 1)
            {
                for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
                {
                    for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
                    {
                        for (gender = 0; gender <= 1; gender++)
                        {
                            for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                            {
                                for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                                {
                                    int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                                    int player_glyph = player_glyph_index + player_glyph_offset;
                                    tilemaparray[player_glyph] = tile_count;

                                    if (plaction == ACTION_TILE_NO_ACTION)
                                    {
                                        for (enum action_tile_types spset2 = ACTION_TILE_ATTACK; spset2 < MAX_ACTION_TILES; spset2++)
                                        {
                                            if (!has_generic_player_action_tile(spset2))
                                            {
                                                int player_glyph2 = get_player_action_glyph_offset(spset2);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                    else if (plaction == ACTION_TILE_ATTACK)
                                    {
                                        enum action_tile_types action_array[5] = { ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK };
                                        for (int idx = 0; idx < 5; idx++)
                                        {
                                            enum action_tile_types action = action_array[idx];
                                            if (!has_generic_player_action_tile(action))
                                            {
                                                int player_glyph2 = get_player_action_glyph_offset(action);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                    else if (plaction == ACTION_TILE_DOOR_USE)
                                    {
                                        enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                                        for (int idx = 0; idx < 3; idx++)
                                        {
                                            enum action_tile_types action = action_array[idx];
                                            if (!has_generic_player_action_tile(action))
                                            {
                                                int player_glyph2 = get_player_action_glyph_offset(action);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            tile_count++;
        }
        else if (tsd->player_tile_style == 2)
        {
            for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
            {
                const char* role_name = roles[roleidx].name.m;
                for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
                {
                    const char* race_name = races[raceidx].noun;
                    for (gender = 0; gender <= 1; gender++)
                    {
                        const char* gender_name = (gender == 0 ? "male" : "female");
                        for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                        {
                            const char* align_name = (alignment == A_CHAOTIC ? "chaotic" : alignment == A_NEUTRAL ? "neutral" : alignment == A_LAWFUL ? "lawful" : "unspecified");
                            for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                            {
                                if (!player_has_action_tile(plaction, roleidx, raceidx, gender, alignment, glevel))
                                    continue;

                                if (process_style == 0)
                                {
                                    Sprintf(buf, "%s,%s,%s,%s,%s,%s,level-%d", tile_section_name, set_name, role_name, race_name, gender_name, align_name, glevel);
                                    int pl_enl = get_player_enlargement(plaction, roleidx, raceidx, gender, alignment, glevel);
                                    if (pl_enl > 0)
                                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[pl_enl].width_in_tiles, enlargements[pl_enl].height_in_tiles, enlargements[pl_enl].main_tile_x_coordinate);
                                    else
                                        Sprintf(eos(buf), ",1,1,0");
                                    Sprintf(eos(buf), "\n");
                                    (void)write(fd, buf, strlen(buf));
                                }
                                else if (process_style == 1)
                                {
                                    int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                                    int player_glyph = player_glyph_index + player_glyph_offset;
                                    tilemaparray[player_glyph] = tile_count;

                                    if (plaction == 0)
                                    {
                                        for (enum action_tile_types action = ACTION_TILE_ATTACK; action < MAX_ACTION_TILES; action++)
                                        {
                                            if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                            {
                                                int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                    else if (plaction == ACTION_TILE_ATTACK)
                                    {
                                        enum action_tile_types action_array[5] = {ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK };
                                        for (int idx = 0; idx < 5; idx++)
                                        {
                                            enum action_tile_types action = action_array[idx];
                                            if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                            {
                                                int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                    else if (plaction == ACTION_TILE_DOOR_USE)
                                    {
                                        enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                                        for (int idx = 0; idx < 3; idx++)
                                        {
                                            enum action_tile_types action = action_array[idx];
                                            if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                            {
                                                int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                tilemaparray[player_glyph2] = tile_count;
                                            }
                                        }
                                    }
                                }
                                tile_count++;
                            }
                        }
                    }
                }
            }
        }
        else if (tsd->player_tile_style == 3)
        {
            for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
            {
                const char* role_name = roles[roleidx].name.m;
                for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
                {
                    if (!validrace(roleidx, raceidx))
                        continue;

                    const char* race_name = races[raceidx].noun;
                    for (gender = 0; gender <= 1; gender++)
                    {
                        const char* gender_name = (gender == 0 ? "male" : "female");
                        for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                        {
                            if (alignment > A_CHAOTIC && !(roles[roleidx].allow & ROLE_ALIGNMENT_TILES))
                                continue;

                            const char* align_name = (roles[roleidx].allow & ROLE_ALIGNMENT_TILES) ? (alignment == A_CHAOTIC ? "chaotic" : alignment == A_NEUTRAL ? "neutral" : alignment == A_LAWFUL ? "lawful" : "unspecified") : "any";
                            for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                            {
                                if (!player_has_action_tile(plaction, roleidx, raceidx, gender, alignment, glevel))
                                    continue;

                                if (process_style == 0)
                                {
                                    Sprintf(buf, "%s,%s,%s,%s,%s,%s,level-%d", tile_section_name, set_name, role_name, race_name, gender_name, align_name, glevel);
                                    int pl_enl = get_player_enlargement(plaction, roleidx, raceidx, gender, alignment, glevel);
                                    if (pl_enl > 0)
                                        Sprintf(eos(buf), ",%d,%d,%d", enlargements[pl_enl].width_in_tiles, enlargements[pl_enl].height_in_tiles, enlargements[pl_enl].main_tile_x_coordinate);
                                    else
                                        Sprintf(eos(buf), ",1,1,0");
                                    Sprintf(eos(buf), "\n");
                                    (void)write(fd, buf, strlen(buf));
                                }
                                else if (process_style == 1)
                                {
                                    if (alignment == A_CHAOTIC && !(roles[roleidx].allow & ROLE_ALIGNMENT_TILES))
                                    {
                                        for (int k = A_CHAOTIC; k <= A_LAWFUL; k++)
                                        {
                                            int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, k, glevel);
                                            int player_glyph = player_glyph_index + player_glyph_offset;
                                            tilemaparray[player_glyph] = tile_count;
                                            if (plaction == ACTION_TILE_NO_ACTION)
                                            {
                                                for (enum action_tile_types action = ACTION_TILE_ATTACK; action < MAX_ACTION_TILES; action++)
                                                {
                                                    if (!player_has_action_tile(action, roleidx, raceidx, gender, k, glevel))
                                                    {
                                                        int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                        tilemaparray[player_glyph2] = tile_count;
                                                    }
                                                }
                                            }
                                            else if (plaction == ACTION_TILE_ATTACK)
                                            {
                                                enum action_tile_types action_array[5] = { ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK };
                                                for (int idx = 0; idx < 5; idx++)
                                                {
                                                    enum action_tile_types action = action_array[idx];
                                                    if (!player_has_action_tile(action, roleidx, raceidx, gender, k, glevel))
                                                    {
                                                        int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                        tilemaparray[player_glyph2] = tile_count;
                                                    }
                                                }
                                            }
                                            else if (plaction == ACTION_TILE_DOOR_USE)
                                            {
                                                enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                                                for (int idx = 0; idx < 3; idx++)
                                                {
                                                    enum action_tile_types action = action_array[idx];
                                                    if (!player_has_action_tile(action, roleidx, raceidx, gender, k, glevel))
                                                    {
                                                        int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                        tilemaparray[player_glyph2] = tile_count;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                                        int player_glyph = player_glyph_index + player_glyph_offset;
                                        tilemaparray[player_glyph] = tile_count;
                                        if (plaction == ACTION_TILE_NO_ACTION)
                                        {
                                            for (enum action_tile_types action = ACTION_TILE_ATTACK; action < MAX_ACTION_TILES; action++)
                                            {
                                                if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                                {
                                                    int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                    tilemaparray[player_glyph2] = tile_count;
                                                }
                                            }
                                        }
                                        else if (plaction == ACTION_TILE_ATTACK)
                                        {
                                            enum action_tile_types action_array[5] = { ACTION_TILE_THROW, ACTION_TILE_FIRE, ACTION_TILE_CAST_DIR, ACTION_TILE_SPECIAL_ATTACK, ACTION_TILE_KICK };
                                            for (int idx = 0; idx < 5; idx++)
                                            {
                                                enum action_tile_types action = action_array[idx];
                                                if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                                {
                                                    int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                    tilemaparray[player_glyph2] = tile_count;
                                                }
                                            }
                                        }
                                        else if (plaction == ACTION_TILE_DOOR_USE)
                                        {
                                            enum action_tile_types action_array[3] = { ACTION_TILE_THROW, ACTION_TILE_CAST_DIR, ACTION_TILE_ITEM_USE };
                                            for (int idx = 0; idx < 3; idx++)
                                            {
                                                enum action_tile_types action = action_array[idx];
                                                if (!player_has_action_tile(action, roleidx, raceidx, gender, alignment, glevel))
                                                {
                                                    int player_glyph2 = player_glyph_index + get_player_action_glyph_offset(action);
                                                    tilemaparray[player_glyph2] = tile_count;
                                                }
                                            }
                                        }
                                    }
                                }
                                tile_count++;
                            }
                        }
                    }
                }
            }
        }
    }


    /* User interface tiles */
    tile_section_name = "user-interface";
    set_name = "special-effect";

    for (i = 0; i < MAX_SPECIAL_EFFECTS; i++)
    {
        if (process_style == 0)
        {
            Sprintf(buf, "%s,%s,%s,1,1,0\n", tile_section_name, set_name, special_effects[i].name);
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_SPECIAL_EFFECT_OFF;
            tilemaparray[i + glyph_offset] = tile_count;
        }
        tile_count++;
    }

    set_name = "cursor";
    for (i = 0; i < MAX_CURSORS; i++)
    {
        if (process_style == 0)
        {
            Sprintf(buf, "%s,%s,%s,1,1,0\n", tile_section_name, set_name, game_cursors[i].name);
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_CURSOR_OFF;
            tilemaparray[i + glyph_offset] = tile_count;
        }
        tile_count++;
    }

    set_name = "hit-tile";
    for (i = 0; i < MAX_HIT_TILES; i++)
    {
        if (process_style == 0)
        {
            Sprintf(buf, "%s,%s,%s,1,1,0\n", tile_section_name, set_name, hit_tile_definitions[i].name);
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_HIT_TILE_OFF;
            tilemaparray[i + glyph_offset] = tile_count;
        }
        tile_count++;
    }

    set_name = "general-tile";
    for (i = 0; i < MAX_GENERAL_TILES; i++)
    {
        if (process_style == 0)
        {
            Sprintf(buf, "%s,%s,%s,1,1,0\n", tile_section_name, set_name, general_tile_definitions[i].name);
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_GENERAL_TILE_OFF;
            tilemaparray[i + glyph_offset] = tile_count;
        }
        tile_count++;
    }

    set_name = "ui-tile";
    for (i = 0; i < MAX_UI_TILES; i++)
    {
        if (process_style == 0)
        {
            Sprintf(buf, "%s,%s,%s,%d,%d,%d", tile_section_name, set_name, 
                ui_tile_component_array[i].name, 
                ui_tile_component_array[i].number,
                ui_tile_component_array[i].width,
                ui_tile_component_array[i].height
            );
            for (j = 0; j < ui_tile_component_array[i].number; j++)
            {
                Sprintf(eos(buf), ",%s", ui_tile_component_array[i].component_names[j]);
            }
            Sprintf(eos(buf), "\n");
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_UI_TILE_OFF;
            tilemaparray[i + GLYPH_UI_TILE_OFF] = tile_count;
        }
        tile_count++;
    }

    set_name = "buff";
    for (i = 0; i < MAX_BUFF_TILES; i++)
    {
        if (process_style == 0)
        {
            int smalltilenum = (i < MAX_BUFF_TILES - 1 ? BUFFS_PER_TILE : LAST_PROP % BUFFS_PER_TILE);
            Sprintf(buf, "%s,%s,tile-%d,%d,%d,%d", tile_section_name, set_name,
                i,
                smalltilenum,
                BUFF_WIDTH,
                BUFF_HEIGHT
            );
            for (j = 0; j < smalltilenum; j++)
            {
                int propidx = i * BUFFS_PER_TILE + j + 1;
                if(property_definitions[propidx].show_buff && property_definitions[propidx].prop_tile_name && strcmp(property_definitions[propidx].prop_tile_name, ""))
                    Sprintf(eos(buf), ",%s", property_definitions[propidx].prop_tile_name);
                else
                    Sprintf(eos(buf), ",%s", "no_buff");
            }
            Sprintf(eos(buf), "\n");
            (void)write(fd, buf, strlen(buf));
        }
        else if (process_style == 1)
        {
            glyph_offset = GLYPH_BUFF_OFF;
            tilemaparray[i + GLYPH_BUFF_OFF] = tile_count;
        }
        tile_count++;
    }

    /* Replacement tiles */
    tile_section_name = "replacement";
    for (i = 1; i < MAX_REPLACEMENTS; i++)  /* replacement number, starts at 1 */
    {
        short base_tile = get_replacement_base_tile(i);
        for (j = 0; j < max(0, min(replacements[i].number_of_tiles, MAX_TILES_PER_REPLACEMENT)); j++) /* tile number */
        {
            if (process_style == 0)
            {
                Sprintf(buf, "%s,%s,%s,%d", tile_section_name,
                    replacements[i].replacement_name ? replacements[i].replacement_name : "unknown replacement",
                    replacements[i].tile_names[j],
                    base_tile
                );
                int enl = replacements[i].tile_enlargement[j];
                if (enl > 0)
                    Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl].width_in_tiles, enlargements[enl].height_in_tiles, enlargements[enl].main_tile_x_coordinate);
                else
                    Sprintf(eos(buf), ",1,1,0");
                Sprintf(eos(buf), "\n");
                (void)write(fd, buf, strlen(buf));
            }
            else if (process_style == 1)
            {
                tilemaparray[j + replacement_offsets[i] /* replacements[i].glyph_offset */ + GLYPH_REPLACEMENT_OFF] = tile_count;
            }
            tile_count++;
        }
    }

    /* Animation tiles */
    tile_section_name = "animation";
    for (i = 1; i < MAX_ANIMATIONS; i++)  /* animation number, starts at 1 */
    {
        short base_tile = get_animation_base_tile(i);
        int contained_anims = max(1, animations[i].number_of_tile_animations);
        if (animations[i].animation_type == ANIMATION_TYPE_NORMAL)
        {
            for (int m = 0; m < contained_anims; m++)
            {
                for (j = 0; j < max(0, min(animations[i].number_of_tiles, MAX_TILES_PER_ANIMATION)); j++) /* tile number */
                {
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,tile-%d,%d", tile_section_name,
                            animations[i].animation_name ? animations[i].animation_name : "unknown animation",
                            j,
                            base_tile + m
                        );
                        int enl = animations[i].tile_enlargement;
                        if (enl > 0)
                            Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl].width_in_tiles, enlargements[enl].height_in_tiles, enlargements[enl].main_tile_x_coordinate);
                        else
                            Sprintf(eos(buf), ",1,1,0");
                        Sprintf(eos(buf), "\n");
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        glyph_offset = GLYPH_ANIMATION_OFF;
                        int n_frames = min(animations[i].number_of_frames, MAX_FRAMES_PER_ANIMATION);
                        for (int k = 0; k < n_frames; k++)  /* frame number */
                        {
                            if (animations[i].frame2tile[k] == j || animations[i].frame2tile[k] == -1)
                            {
                                int used_tile = animations[i].frame2tile[k] == j ? tile_count : base_tile;
                                tilemaparray[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] = used_tile;
                            }
                        }
                    }
                    tile_count++;
                }
            }
        }
        else if (animations[i].animation_type == ANIMATION_TYPE_MISSILE)
        {
            for (int bm = 0; bm < NUM_BASE_TILE_DIRS; bm++)
            {
                for (j = 0; j < max(0, min(animations[i].number_of_tiles, MAX_TILES_PER_ANIMATION)); j++) /* tile number */
                {
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,tile-%d,%d", tile_section_name,
                            animations[i].animation_name ? animations[i].animation_name : "unknown animation",
                            j,
                            base_tile + bm
                        );
                        int enl = animations[i].tile_enlargement;
                        if (enl > 0)
                            Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl].width_in_tiles, enlargements[enl].height_in_tiles, enlargements[enl].main_tile_x_coordinate);
                        else
                            Sprintf(eos(buf), ",1,1,0");
                        Sprintf(eos(buf), "\n");
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        for (int m = 0; m < contained_anims; m++)
                        {
                            boolean hflip = FALSE;
                            boolean vflip = FALSE;

                            if (is_dir_from_base_dir(m, bm, &hflip, &vflip))
                            {
                                glyph_offset = GLYPH_ANIMATION_OFF;
                                int n_frames = min(animations[i].number_of_frames, MAX_FRAMES_PER_ANIMATION);
                                for (int k = 0; k < n_frames; k++)  /* frame number */
                                {
                                    if (animations[i].frame2tile[k] == j || animations[i].frame2tile[k] == -1)
                                    {
                                        int used_tile = animations[i].frame2tile[k] == j ? tile_count : base_tile;
                                        tilemaparray[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] = used_tile;
                                        if (hflip)
                                            tilemapflags[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                        if (vflip)
                                            tilemapflags[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                                    }
                                }
                            }
                        }
                    }
                    tile_count++;
                }
            }
        }
        else if (animations[i].animation_type == ANIMATION_TYPE_ZAP)
        {
            for (int bm = 0; bm < NUM_ZAP_BASE_TILES; bm++)
            {
                for (j = 0; j < max(0, min(animations[i].number_of_tiles, MAX_TILES_PER_ANIMATION)); j++) /* tile number */
                {
                    if (process_style == 0)
                    {
                        Sprintf(buf, "%s,%s,tile-%d,%d", tile_section_name,
                            animations[i].animation_name ? animations[i].animation_name : "unknown animation",
                            j,
                            base_tile + bm
                        );
                        int enl = animations[i].tile_enlargement;
                        if (enl > 0)
                            Sprintf(eos(buf), ",%d,%d,%d", enlargements[enl].width_in_tiles, enlargements[enl].height_in_tiles, enlargements[enl].main_tile_x_coordinate);
                        else
                            Sprintf(eos(buf), ",1,1,0");
                        Sprintf(eos(buf), "\n");
                        (void)write(fd, buf, strlen(buf));
                    }
                    else if (process_style == 1)
                    {
                        for (int m = 0; m < contained_anims; m++)
                        {
                            boolean hflip = FALSE;
                            boolean vflip = FALSE;

                            if (is_zap_char_from_base_zap_char(m, bm, &hflip, &vflip))
                            {
                                glyph_offset = GLYPH_ANIMATION_OFF;
                                int n_frames = min(animations[i].number_of_frames, MAX_FRAMES_PER_ANIMATION);
                                for (int k = 0; k < n_frames; k++)  /* frame number */
                                {
                                    if (animations[i].frame2tile[k] == j || animations[i].frame2tile[k] == -1)
                                    {
                                        int used_tile = animations[i].frame2tile[k] == j ? tile_count : base_tile;
                                        tilemaparray[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] = used_tile;
                                        if(hflip)
                                            tilemapflags[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] |= GLYPH_TILE_FLAG_FLIP_HORIZONTALLY;
                                        if (vflip)
                                            tilemapflags[k + n_frames * m + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF] |= GLYPH_TILE_FLAG_FLIP_VERTICALLY;
                                    }
                                }
                            }
                        }
                    }
                    tile_count++;
                }
            }
        }
    }


    /* Enlargement tiles */
    tile_section_name = "enlargement";
    for (i = 1; i < MAX_ENLARGEMENTS; i++)  /* enlargement number, starts at 1 */
    {
        short enl_anim_tiles = enlargements[i].number_of_animation_tiles ? enlargements[i].number_of_animation_tiles : 1;
        //short enl_anim_frames = enlargements[i].number_of_animation_frames ? enlargements[i].number_of_animation_frames : 1;
        for (int m = 0; m < enl_anim_tiles; m++)
        {
            /* m is the animation tile number */
            short enl_anim_tile_idx = enlargements[i].number_of_animation_tiles ? m : -1;
            short base_tile = get_enlargement_base_tile(i, enl_anim_tile_idx);
            for (j = 0; j < enlargements[i].number_of_enlargement_tiles; j++)
            {
                /* j is enlargement glyph index number, the same as position tile number */
                const char* pos_name = "unknown";
                const char* position_names[NUM_POSITIONS_IN_ENLARGEMENT] = { "top-left", "top", "top-right", "left", "right" };

                int position = -1;
                for (int k = 0; k < NUM_POSITIONS_IN_ENLARGEMENT; k++) /* position number */
                {
                    if (enlargements[i].position2tile[k] == j)
                    {
                        position = k;
                        break;
                    }
                }
                if (position >= 0)
                {
                    pos_name = position_names[position];

                }

                if (process_style == 0)
                {
                    char tilebuf[BUFSIZ];
                    strcpy(tilebuf, "");
//                    if (enl_anim_tile_idx >= 0)
//                        Sprintf(tilebuf, "_tile-%d", enl_anim_tile_idx);

                    char enlbuf[BUFSIZ];
                    Sprintf(enlbuf, "%s%s", enlargements[i].enlargement_name ? enlargements[i].enlargement_name : "unknown enlargement",
                        tilebuf);

                    Sprintf(buf, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d\n", tile_section_name,
                        enlbuf,
                        pos_name,
                        base_tile,
                        enlargements[i].width_in_tiles,
                        enlargements[i].height_in_tiles,
                        enlargements[i].main_tile_x_coordinate,
                        enlargements[i].number_of_enlargement_tiles,
                        position,
                        enl_anim_tile_idx
                    );
                    (void)write(fd, buf, strlen(buf));
                }
                else if (process_style == 1)
                {
                    glyph_offset = GLYPH_ENLARGEMENT_OFF;
                    int anim = get_enlargement_animation(i);
                    if (anim >= 0)
                    {
                        int addedindex = m * enlargements[i].number_of_enlargement_tiles; /* Move to the appropriate place for the animation tile */
                        tilemaparray[j + addedindex + enlargement_offsets[i] /* enlargements[i].glyph_offset */ + GLYPH_ENLARGEMENT_OFF] = tile_count;
#if 0
                        for (int f = 0; f < enl_anim_frames; f++)
                        {
                            if (animations[anim].frame2tile[f] == enl_anim_tile_idx)
                            {
                                int addedindex = f * enlargements[i].number_of_enlargement_tiles;
                                tilemaparray[j + addedindex + enlargement_offsets[i] /* enlargements[i].glyph_offset */ + GLYPH_ENLARGEMENT_OFF] = tile_count;
                            }
                        }
#endif
                    }
                    else
                    {
                        int addedindex = enl_anim_tile_idx >= 0 ?
                            enl_anim_tile_idx * enlargements[i].number_of_enlargement_tiles
                            : 0;
                        tilemaparray[j + addedindex + enlargement_offsets[i] /* enlargements[i].glyph_offset */ + GLYPH_ENLARGEMENT_OFF] = tile_count;
                    }

#if 0
                    for (int k = 0; k < min(enlargements[i].number_of_enlargement_tiles, NUM_POSITIONS_IN_ENLARGEMENT); k++)  /* frame number */
                    {
                        if (enlargements[i].position2tile[k] == j)
                            tilemaparray[k + addedindex + enlargement_offsets[i] /* enlargements[i].glyph_offset */ + GLYPH_ENLARGEMENT_OFF] = tile_count;
                    }
#endif
                }
                tile_count++;
            }
        }
    }


    /* Finalize */
    if (process_style == 0)
    {
        /* Finished */
        (void)close(fd);
    }
    else if (process_style == 1)
    {
        /*
         *Set tile2replacement, glyph2replacement, glyphtileflags, and tile2autodraw
         */

         /* Monsters */
        for (i = 0; i < NUM_MONSTERS; i++)
        {
            enum action_tile_types action;
            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].replacement.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 0);
                    int tile = tilemaparray[glyph];
                    int repl = mons[i].replacement.actions[action];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
            }
            if (mons[i].replacement.corpse)
            {
                int glyph = i + GLYPH_BODY_OFF;
                int tile = tilemaparray[glyph];
                int repl = mons[i].replacement.corpse;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
            if (mons[i].replacement.statue)
            {
                int glyph = i + GLYPH_STATUE_OFF;
                int tile = tilemaparray[glyph];
                int repl = mons[i].replacement.statue;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }


            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].female_replacement.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 1);
                    int tile = tilemaparray[glyph];
                    int repl = mons[i].female_replacement.actions[action];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
            }
            if (mons[i].female_replacement.corpse)
            {
                int glyph = i + GLYPH_FEMALE_BODY_OFF;
                int tile = tilemaparray[glyph];
                int repl = mons[i].female_replacement.corpse;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
            if (mons[i].female_replacement.statue)
            {
                int glyph = i + GLYPH_FEMALE_STATUE_OFF;
                int tile = tilemaparray[glyph];
                int repl = mons[i].female_replacement.statue;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* Objects */
        for (i = 0; i < NUM_OBJECTS; i++)
        {
            if (obj_descr[i].replacement)
            {
                int glyph = objnum_to_glyph(i);
                int tile = glyph2tile[glyph];
                int repl = obj_descr[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* Artifacts */
        for (i = 1; i <= NUM_ARTIFACTS; i++)
        {
            if (artilist[i].replacement)
            {
                int glyph = objnum_to_glyph(i);
                int tile = glyph2tile[glyph];
                int repl = artilist[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* CMAP */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_CMAPPED_CHARS; i++)
            {
                if (defsyms[i].replacement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    int repl = defsyms[i].replacement[cmap_type_idx];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
                if (defsyms[i].broken_replacement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_BROKEN_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    int repl = defsyms[i].broken_replacement[cmap_type_idx];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
            }
        }

        /* CMAP Variation */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_VARIATIONS; i++)
            {
                if (defsym_variations[i].replacement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    int repl = defsym_variations[i].replacement[cmap_type_idx];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
                if (defsym_variations[i].broken_replacement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_BROKEN_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    int repl = defsym_variations[i].broken_replacement[cmap_type_idx];
                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                    {
                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                    }
                }
            }
        }

        /* Player */
        for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
        {
            for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
            {
                for (gender = 0; gender <= 1; gender++)
                {
                    for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                    {
                        for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                        {
                            int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                            for (enum action_tile_types action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
                            {
                                int repl = get_player_replacement(action, roleidx, raceidx, gender, alignment, glevel);
                                if (repl)
                                {
                                    int glyph = player_glyph_index + get_player_action_glyph_offset(action);
                                    int tile = glyph2tile[glyph];
                                    tile2replacement[tile] = glyph2replacement[glyph] = repl;
                                    tile2autodraw[tile] = replacements[repl].general_autodraw;
                                    for (j = 0; j < replacements[repl].number_of_tiles; j++)
                                    {
                                        int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                                        glyphtileflags[rglyph] = glyphtileflags[glyph];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Special Effects */
        for (i = 0; i < MAX_SPECIAL_EFFECTS; i++)
        {
            if (special_effects[i].replacement)
            {
                int glyph = i + GLYPH_SPECIAL_EFFECT_OFF;
                int tile = glyph2tile[glyph];
                int repl = special_effects[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* Cursors */
        for (i = 0; i < MAX_CURSORS; i++)
        {
            if (game_cursors[i].replacement)
            {
                int glyph = i + GLYPH_CURSOR_OFF;
                int tile = glyph2tile[glyph];
                int repl = game_cursors[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* Hit tiles */
        for (i = 0; i < MAX_HIT_TILES; i++)
        {
            if (hit_tile_definitions[i].replacement)
            {
                int glyph = i + GLYPH_HIT_TILE_OFF;
                int tile = glyph2tile[glyph];
                int repl = hit_tile_definitions[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* General tiles */
        for (i = 0; i < MAX_GENERAL_TILES; i++)
        {
            if (general_tile_definitions[i].replacement)
            {
                int glyph = i + GLYPH_GENERAL_TILE_OFF;
                int tile = glyph2tile[glyph];
                int repl = general_tile_definitions[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* UI Tiles */
        for (i = 0; i < MAX_UI_TILES; i++)
        {
            if (ui_tile_component_array[i].replacement)
            {
                int glyph = i + GLYPH_UI_TILE_OFF;
                int tile = glyph2tile[glyph];
                int repl = ui_tile_component_array[i].replacement;
                tile2replacement[tile] = glyph2replacement[glyph] = repl;
                tile2autodraw[tile] = replacements[repl].general_autodraw;
                for (j = 0; j < replacements[repl].number_of_tiles; j++)
                {
                    int rglyph = j + replacement_offsets[repl] + GLYPH_REPLACEMENT_OFF;
                    glyphtileflags[rglyph] = glyphtileflags[glyph];
                }
            }
        }

        /* Buffs have no replacements currently */


        /*
         * Set tile2autodraw for replacement tiles; the rest has been done above
         */
        for (i = 0; i < MAX_REPLACEMENTS; i++)
        {
            for (j = 0; j < replacements[i].number_of_tiles; j++)
            {
                int glyph = i + replacement_offsets[j] + GLYPH_REPLACEMENT_OFF;
                int tile = tilemaparray[glyph];
                tile2autodraw[tile] = replacements[i].tile_autodraw[j];
            }
        }


        /*
         * Set tile2animation 
         */

        /* Monsters */
        for (i = 0; i < NUM_MONSTERS; i++)
        {
            enum action_tile_types action;
            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].animation.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 0);
                    int tile = tilemaparray[glyph];
                    tile2animation[tile] = mons[i].animation.actions[action];
                }
            }
            if (mons[i].animation.corpse)
            {
                int glyph = i + GLYPH_BODY_OFF;
                int tile = tilemaparray[glyph];
                tile2animation[tile] = mons[i].animation.corpse;
            }
            if (mons[i].animation.statue)
            {
                int glyph = i + GLYPH_STATUE_OFF;
                int tile = tilemaparray[glyph];
                tile2animation[tile] = mons[i].animation.statue;
            }


            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].female_animation.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 1);
                    int tile = tilemaparray[glyph];
                    tile2animation[tile] = mons[i].female_animation.actions[action];
                }
            }
            if (mons[i].female_animation.corpse)
            {
                int glyph = i + GLYPH_FEMALE_BODY_OFF;
                int tile = tilemaparray[glyph];
                tile2animation[tile] = mons[i].female_animation.corpse;
            }
            if (mons[i].female_animation.statue)
            {
                int glyph = i + GLYPH_FEMALE_STATUE_OFF;
                int tile = tilemaparray[glyph];
                tile2animation[tile] = mons[i].female_animation.statue;
            }
        }

        /* Objects */
        for (i = 0; i < NUM_OBJECTS; i++)
        {
            if (obj_descr[i].stand_animation)
            {
                int glyph = objnum_to_glyph(i);
                int tile = glyph2tile[glyph];
                tile2animation[tile] = obj_descr[i].stand_animation;
            }
        }

        /* Artifacts */
        for (i = 1; i <= NUM_ARTIFACTS; i++)
        {
            if (artilist[i].stand_animation)
            {
                int glyph = objnum_to_glyph(i);
                int tile = glyph2tile[glyph];
                tile2animation[tile] = artilist[i].stand_animation;
            }
        }

        /* CMAP */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_CMAPPED_CHARS; i++)
            {
                if (defsyms[i].stand_animation[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = defsyms[i].stand_animation[cmap_type_idx];
                }
                if (defsyms[i].broken_animation[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_BROKEN_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = defsyms[i].broken_animation[cmap_type_idx];
                }
            }
        }

        /* CMAP Variation */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_VARIATIONS; i++)
            {
                if (defsym_variations[i].stand_animation[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = defsym_variations[i].stand_animation[cmap_type_idx];
                }
                if (defsym_variations[i].broken_animation[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_BROKEN_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = defsym_variations[i].broken_animation[cmap_type_idx];
                }
            }
        }

        /* Player */
        for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
        {
            for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
            {
                for (gender = 0; gender <= 1; gender++)
                {
                    for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                    {
                        for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                        {
                            int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                            for (enum action_tile_types action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
                            {
                                int anim_idx = get_player_animation(action, roleidx, raceidx, gender, alignment, glevel);
                                if (anim_idx)
                                {
                                    int player_glyph = player_glyph_index + get_player_action_glyph_offset(action);
                                    int tile = glyph2tile[player_glyph];
                                    tile2animation[tile] = anim_idx;
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Explosion */
        for (i = 0; i < MAX_EXPLOSIONS; i++)
        {
            if (explosion_type_definitions[i].animation)
            {
                for (j = 0; j < MAX_EXPLOSION_CHARS; j++)
                {
                    int glyph = j + i * MAX_EXPLOSION_CHARS + GLYPH_EXPLODE_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = explosion_type_definitions[i].animation;
                }
            }
        }

        /* Zap */
        for (i = 0; i < MAX_ZAP_TYPES; i++)
        {
            if (zap_type_definitions[i].animation)
            {
                for (j = 0; j < NUM_ZAP_CHARS; j++)
                {
                    int glyph = j + i * NUM_ZAP_CHARS + GLYPH_ZAP_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = zap_type_definitions[i].animation;
                }
            }
        }

        /* Swallow */
        for (i = 0; i < NUM_MONSTERS; i++)
        {
            if (mons[i].animation.swallow)
            {
                for (j = 0; j < MAX_SWALLOW_CHARS; j++)
                {
                    int glyph = j + i * MAX_SWALLOW_CHARS + GLYPH_SWALLOW_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = mons[i].animation.swallow;
                }
            }
            if (mons[i].female_animation.swallow)
            {
                for (j = 0; j < MAX_SWALLOW_CHARS; j++)
                {
                    int glyph = j + i * MAX_SWALLOW_CHARS + GLYPH_SWALLOW_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = mons[i].female_animation.swallow;
                }
            }
        }

        /* Special Effects */
        for (i = 0; i < MAX_SPECIAL_EFFECTS; i++)
        {
            if (special_effects[i].animation)
            {
                int glyph = i + GLYPH_SPECIAL_EFFECT_OFF;
                int tile = glyph2tile[glyph];
                tile2animation[tile] = special_effects[i].animation;
            }
        }

        /* Cursors */
        for (i = 0; i < MAX_CURSORS; i++)
        {
            if (game_cursors[i].animation)
            {
                int glyph = i + GLYPH_CURSOR_OFF;
                int tile = glyph2tile[glyph];
                tile2animation[tile] = game_cursors[i].animation;
            }
        }

        /* Hit tiles */
        for (i = 0; i < MAX_HIT_TILES; i++)
        {
            if (hit_tile_definitions[i].animation)
            {
                int glyph = i + GLYPH_HIT_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2animation[tile] = hit_tile_definitions[i].animation;
            }
        }

        /* General tiles */
        for (i = 0; i < MAX_GENERAL_TILES; i++)
        {
            if (general_tile_definitions[i].animation)
            {
                int glyph = i + GLYPH_GENERAL_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2animation[tile] = general_tile_definitions[i].animation;
            }
        }

        /* UI Tiles */
        for (i = 0; i < MAX_UI_TILES; i++)
        {
            if (ui_tile_component_array[i].animation)
            {
                int glyph = i + GLYPH_UI_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2animation[tile] = ui_tile_component_array[i].animation;
            }
        }

        /* Buffs have no animations currently */


        /* Replacements */
        for (i = 1; i < MAX_REPLACEMENTS; i++)
        {
            for (j = 0; j < replacements[i].number_of_tiles; j++)
            {
                int anim_idx = replacements[i].tile_animation[j];
                if (anim_idx)
                {
                    int glyph = j + replacement_offsets[i] /* replacements[i].glyph_offset */ + GLYPH_REPLACEMENT_OFF;
                    int tile = glyph2tile[glyph];
                    tile2animation[tile] = anim_idx;
                }
            }
        }


        /*
         * Set tile2enlargement
         */

        /* Monsters */
        for (i = 0; i < NUM_MONSTERS; i++)
        {
            enum action_tile_types action;
            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].enlargement.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 0);
                    int tile = tilemaparray[glyph];
                    tile2enlargement[tile] = mons[i].enlargement.actions[action];
                }
            }
            if (mons[i].enlargement.corpse)
            {
                int glyph = i + GLYPH_BODY_OFF;
                int tile = tilemaparray[glyph];
                tile2enlargement[tile] = mons[i].enlargement.corpse;
            }
            if (mons[i].enlargement.statue)
            {
                int glyph = i + GLYPH_STATUE_OFF;
                int tile = tilemaparray[glyph];
                tile2enlargement[tile] = mons[i].enlargement.statue;
            }

            for (action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
            {
                if (mons[i].female_enlargement.actions[action])
                {
                    int glyph = i + get_monster_action_glyph_offset(action, 1);
                    int tile = tilemaparray[glyph];
                    tile2enlargement[tile] = mons[i].female_enlargement.actions[action];
                }
            }
            if (mons[i].female_enlargement.corpse)
            {
                int glyph = i + GLYPH_FEMALE_BODY_OFF;
                int tile = tilemaparray[glyph];
                tile2enlargement[tile] = mons[i].female_enlargement.corpse;
            }
            if (mons[i].female_enlargement.statue)
            {
                int glyph = i + GLYPH_FEMALE_STATUE_OFF;
                int tile = tilemaparray[glyph];
                tile2enlargement[tile] = mons[i].female_enlargement.statue;
            }
        }
        /* Objects */
        for (i = 0; i < NUM_OBJECTS; i++)
        {
            if (obj_descr[i].enlargement)
            {
                int glyph = objnum_to_glyph(i);
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = obj_descr[i].enlargement;
            }
        }

        /* Artifacts */
        for (i = 1; i <= NUM_ARTIFACTS; i++)
        {
            if (artilist[i].enlargement)
            {
                int glyph = i + GLYPH_ARTIFACT_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = artilist[i].enlargement;
            }
        }

        /* CMAP */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_CMAPPED_CHARS; i++)
            {
                if (defsyms[i].enlargement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = defsyms[i].enlargement[cmap_type_idx];
                }
                if (defsyms[i].broken_enlargement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * NUM_CMAP_TYPE_CHARS + GLYPH_BROKEN_CMAP_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = defsyms[i].broken_enlargement[cmap_type_idx];
                }
            }
        }

        /* CMAP Variation */
        for (cmap_type_idx = 0; cmap_type_idx < MAX_CMAP_TYPES; cmap_type_idx++)
        {
            for (i = 0; i < MAX_VARIATIONS; i++)
            {
                if (defsym_variations[i].enlargement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = defsym_variations[i].enlargement[cmap_type_idx];
                }
                if (defsym_variations[i].broken_enlargement[cmap_type_idx])
                {
                    int glyph = i + cmap_type_idx * MAX_VARIATIONS + GLYPH_BROKEN_CMAP_VARIATION_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = defsym_variations[i].broken_enlargement[cmap_type_idx];
                }
            }
        }

        /* Player */
        for (roleidx = 0; roleidx < NUM_ROLES; roleidx++)
        {
            for (raceidx = 0; raceidx < NUM_RACES; raceidx++)
            {
                for (gender = 0; gender <= 1; gender++)
                {
                    for (alignment = A_CHAOTIC; alignment <= A_LAWFUL; alignment++)
                    {
                        for (glevel = 0; glevel < NUM_PLAYER_GLYPH_LEVELS; glevel++)
                        {
                            int player_glyph_index = player_to_glyph_index(roleidx, raceidx, gender, alignment, glevel);
                            for (enum action_tile_types action = ACTION_TILE_NO_ACTION; action < MAX_ACTION_TILES; action++)
                            {
                                int enlargement_idx = get_player_enlargement(action, roleidx, raceidx, gender, alignment, glevel);
                                if (enlargement_idx)
                                {
                                    int player_glyph = player_glyph_index + get_player_action_glyph_offset(action);
                                    int tile = glyph2tile[player_glyph];
                                    tile2enlargement[tile] = enlargement_idx;
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Special Effects */
        for (i = 0; i < MAX_SPECIAL_EFFECTS; i++)
        {
            if (special_effects[i].enlargement)
            {
                int glyph = i + GLYPH_SPECIAL_EFFECT_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = special_effects[i].enlargement;
            }
        }

        /* Cursors */
        for (i = 0; i < MAX_CURSORS; i++)
        {
            if (game_cursors[i].enlargement)
            {
                int glyph = i + GLYPH_CURSOR_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = game_cursors[i].enlargement;
            }
        }

        /* Hit tiles */
        for (i = 0; i < MAX_HIT_TILES; i++)
        {
            if (hit_tile_definitions[i].enlargement)
            {
                int glyph = i + GLYPH_HIT_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = hit_tile_definitions[i].enlargement;
            }
        }

        /* General tiles */
        for (i = 0; i < MAX_GENERAL_TILES; i++)
        {
            if (general_tile_definitions[i].enlargement)
            {
                int glyph = i + GLYPH_GENERAL_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = general_tile_definitions[i].enlargement;
            }
        }

        /* UI Tiles */
        for (i = 0; i < MAX_UI_TILES; i++)
        {
            if (ui_tile_component_array[i].enlargement)
            {
                int glyph = i + GLYPH_UI_TILE_OFF;
                int tile = glyph2tile[glyph];
                tile2enlargement[tile] = ui_tile_component_array[i].enlargement;
            }
        }

        /* Buffs have no enlargements */


        /* Replacements */
        for (i = 1; i < MAX_REPLACEMENTS; i++)
        {
            for (j = 0; j < replacements[i].number_of_tiles; j++)
            {
                int enl_idx = replacements[i].tile_enlargement[j];
                if (enl_idx)
                {
                    int glyph = j + replacement_offsets[i] /* replacements[i].glyph_offset */ + GLYPH_REPLACEMENT_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = enl_idx;
                }
            }
        }

        /* Animations */
        for (i = 1; i < MAX_ANIMATIONS; i++)
        {
            for (j = 0; j < max(0, min(MAX_FRAMES_PER_ANIMATION, animations[i].number_of_frames)); j++)
            {
                char animation_tile_glyph_index = animations[i].frame2tile[j];
                if (animation_tile_glyph_index >= 0 && animations[i].tile_enlargement)
                {
                    int glyph = (int)animation_tile_glyph_index + animation_offsets[i] /* animations[i].glyph_offset */ + GLYPH_ANIMATION_OFF;
                    int tile = glyph2tile[glyph];
                    tile2enlargement[tile] = animations[i].tile_enlargement;
                }
            }
        }
    }

    return tile_count;
}

STATIC_OVL void
set_base_tileset_cmap(dest_var_ptr, sym_idx, cmap_idx)
int* dest_var_ptr;
int sym_idx, cmap_idx;
{
    if (!dest_var_ptr)
        return;

    if (defsyms[sym_idx].included_in_cmap[cmap_idx])
    {
        *dest_var_ptr = defsyms[sym_idx].base_cmap[cmap_idx];
    }
    else
    {
        if(defsyms[sym_idx].base_cmap[cmap_idx] != cmap_idx)
            set_base_tileset_cmap(dest_var_ptr, sym_idx, defsyms[sym_idx].base_cmap[cmap_idx]);
        else
            *dest_var_ptr = defsyms[sym_idx].base_cmap[0];
    }
}

STATIC_OVL void
set_base_tileset_cmap_variation(dest_var_ptr, sym_idx, cmap_idx)
int* dest_var_ptr;
int sym_idx, cmap_idx;
{
    if (!dest_var_ptr)
        return;

    if (defsym_variations[sym_idx].included_in_cmap[cmap_idx])
    {
        *dest_var_ptr = defsym_variations[sym_idx].base_cmap[cmap_idx];
    }
    else
    {
        if (defsym_variations[sym_idx].base_cmap[cmap_idx] != cmap_idx)
            set_base_tileset_cmap_variation(dest_var_ptr, sym_idx, defsym_variations[sym_idx].base_cmap[cmap_idx]);
        else
            *dest_var_ptr = defsym_variations[sym_idx].base_cmap[0];
    }
}

boolean
is_dir_from_base_dir(dir_index, base_dir_index, hflip_ptr, vflip_ptr)
int dir_index, base_dir_index;
boolean *hflip_ptr, *vflip_ptr;
{
    if (!hflip_ptr || !vflip_ptr)
        return FALSE;

    if (base_dir_index < 0 || base_dir_index >= NUM_BASE_TILE_DIRS || dir_index < 0 || dir_index >= NUM_MISSILE_DIRS)
        return FALSE;

    switch (base_dir_index)
    {
    case 0:
        if (dir_index == 1)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (dir_index == 6)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 1:
        if (dir_index == 3)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (dir_index == 4)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;
    case 2:
        if (dir_index == 0)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (dir_index == 2)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (dir_index == 5)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (dir_index == 7)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    default:
        break;
    }
    return FALSE;
}

boolean
is_zap_char_from_base_zap_char(zap_char_index, base_zap_char_index, hflip_ptr, vflip_ptr)
int zap_char_index, base_zap_char_index;
boolean* hflip_ptr, * vflip_ptr;
{
    if (!hflip_ptr || !vflip_ptr)
        return FALSE;

    if (base_zap_char_index < 0 || base_zap_char_index >= NUM_ZAP_BASE_TILES || zap_char_index < 0 || zap_char_index >= NUM_ZAP_CHARS)
        return FALSE;


    /*
            const char* zap_direction_name_array[NUM_ZAP_CHARS] = {
                "up", "left", "up-left", "up-right",
                "down", "right", "down-right", "down-left",
                "bounce-top-clockwise", "bounce-right-clockwise", "bounce-bottom-clockwise", "bounce-left-clockwise",
                "bounce-top-counterclockwise", "bounce-right-counterclockwise", "bounce-bottom-counterclockwise", "bounce-left-counterclockwise",
                "back-bounce-up", "back-bounce-left", "back-bounce-up-left", "back-bounce-up-right",
                "back-bounce-down", "back-bounce-right", "back-bounce-down-right", "back-bounce-down-left",
                "up-left-extra-up", "up-left-extra-left", "up-right-extra-up", "up-right-extra-right",
                "down-right-extra-down", "down-right-extra-right", "down-left-extra-down", "down-left-extra-left",
                "leading-edge-up-left", "leading-edge-up-right", "leading-edge-down-right", "leading-edge-down-left",
                "up-left-trailing-edge-down", "up-left-trailing-edge-right", "up-right-trailing-edge-down", "up-right-trailing-edge-left",
                "down-right-trailing-edge-up", "down-right-trailing-edge-left", "down-left-trailing-edge-up", "down-left-trailing-edge-right",
                "trailing-edge-up-left", "trailing-edge-up-right", "trailing-edge-down-right", "trailing-edge-down-left",
                "leading-edge-up", "leading-edge-right", "leading-edge-down", "leading-edge-left",
                "trailing-edge-up", "trailing-edge-right", "trailing-edge-down", "trailing-edge-left",
            };

            const char* zap_template_tile_name_array[ZAP_TEMPLATE_WIDTH * ZAP_TEMPLATE_HEIGHT] = {
                "leading-edge-up", "leading-edge-left", "left", "trailing-edge-left", "back-bounce-left",
                "up", "bounce-top-clockwise", "leading-edge-up-left", "up-left-extra-up", "back-bounce-up-left",
                "trailing-edge-up", "bounce-right-clockwise", "up-left-extra-left", "up-left", "up-left-trailing-edge-right",
                "back-bounce-up", "", "", "up-left-trailing-edge-down", "trailing-edge-up-left",
            };



    */

    switch (base_zap_char_index)
    {
    case 0:
        if (zap_char_index == 48)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 50)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 1:
        if (zap_char_index == 51)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 49)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;
    case 2:
        if (zap_char_index == 1)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 5)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;
    case 3:
        if (zap_char_index == 55)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 53)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;
    case 4:
        if (zap_char_index == 17)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 21)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;

    case 5:
        if (zap_char_index == 0)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 4)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 6:
        if (zap_char_index == 8)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 10)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 12)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 14)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;    
    case 7:
        if (zap_char_index == 32)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 33)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 34)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 35)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 8:
        if (zap_char_index == 24)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 26)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 28)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 30)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 9:
        if (zap_char_index == 18)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 19)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 22)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 23)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 10:
        if (zap_char_index == 52)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 54)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 11:
        if (zap_char_index == 9)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 11)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 13)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 15)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        break;    
    case 12:
        if (zap_char_index == 25)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 27)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 29)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 31)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 13:
        if (zap_char_index == 2)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 3)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 6)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 7)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 14:
        if (zap_char_index == 37)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 39)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 41)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 43)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 15:
        if (zap_char_index == 16)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 20)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 16:
        if (zap_char_index == 36)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 38)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 40)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 42)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;
    case 17:
        if (zap_char_index == 44)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 45)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = FALSE;
            return TRUE;
        }
        else if (zap_char_index == 46)
        {
            *hflip_ptr = TRUE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        else if (zap_char_index == 47)
        {
            *hflip_ptr = FALSE;
            *vflip_ptr = TRUE;
            return TRUE;
        }
        break;

    default:
        break;
    }

#if 0
    int x_coord = 0;
    int y_coord = 0;

    switch (i)
    {
    case 0:
        x_coord = 0;
        y_coord = 0;
        break;
    case 1:
        x_coord = 1;
        y_coord = 0;
        break;
    case 2:
        x_coord = 3;
        y_coord = 1;
        break;
    case 3:
        x_coord = 3;
        y_coord = 1;
        flip_horizontal = TRUE;
        break;
    case 4:
        x_coord = 0;
        y_coord = 0;
        flip_vertical = TRUE;
        break;
    case 5:
        x_coord = 1;
        y_coord = 0;
        flip_horizontal = TRUE;
        break;
    case 6:
        x_coord = 3;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 7:
        x_coord = 3;
        y_coord = 1;
        flip_vertical = TRUE;
        break;
    case 8:
        x_coord = 0;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 9:
        x_coord = 1;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 10:
        x_coord = 0;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 11:
        x_coord = 1;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 12:
        x_coord = 0;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 13:
        x_coord = 1;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    case 14:
        x_coord = 0;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    case 15:
        x_coord = 1;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 16:
        x_coord = 4;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 17:
        x_coord = 4;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 18:
        x_coord = 2;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 19:
        x_coord = 2;
        y_coord = 0;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 20:
        x_coord = 4;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    case 21:
        x_coord = 4;
        y_coord = 0;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 22:
        x_coord = 2;
        y_coord = 0;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 23:
        x_coord = 2;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    case 24:
        x_coord = 3;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 25:
        x_coord = 2;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    case 26:
        x_coord = 3;
        y_coord = 0;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 27:
        x_coord = 2;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = FALSE;
        break;
    case 28:
        x_coord = 3;
        y_coord = 0;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 29:
        x_coord = 2;
        y_coord = 1;
        flip_horizontal = TRUE;
        flip_vertical = TRUE;
        break;
    case 30:
        x_coord = 3;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    case 31:
        x_coord = 2;
        y_coord = 1;
        flip_horizontal = FALSE;
        flip_vertical = TRUE;
        break;
    default:
        x_coord = 0;
        y_coord = 0;
        flip_horizontal = FALSE;
        flip_vertical = FALSE;
        break;
    }
#endif

    return FALSE;
}

int
get_enlargement_animation(enlargement_idx)
int enlargement_idx;
{
    for (int i = 0; i < MAX_ANIMATIONS; i++)
    {
        if (animations[i].tile_enlargement == enlargement_idx)
            return i;
    }

    return -1;
}

/*tiledata.c*/
