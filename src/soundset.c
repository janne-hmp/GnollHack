/* GnollHack 4.0 soundset.c */
/* Copyright (c) Janne Gustafsson, 2020.                            */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h" /* for checking save modes */
#include "artifact.h"
#include "artilist.h"

NEARDATA struct soundsource_t* sound_base = 0;
STATIC_DCL void FDECL(set_hearing_array, (int, int, double));


NEARDATA struct player_soundset_definition player_soundsets[MAX_PLAYER_SOUNDSETS] =
{
	{
		"",
        {0, 0.0f}, SOUNDSOURCE_AMBIENT_GENERAL,
        OBJECT_SOUNDSET_NONE,
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}},
		{{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
	},
	{
		"Generic",
        {0, 0.0f}, SOUNDSOURCE_AMBIENT_GENERAL,
        OBJECT_SOUNDSET_NONE,
        {{GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
	{
		"",
        {0, 0.0f}, SOUNDSOURCE_AMBIENT_GENERAL,
        OBJECT_SOUNDSET_NONE,
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    }
};


NEARDATA struct monster_soundset_definition monster_soundsets[MAX_MONSTER_SOUNDSETS] =
{
	{
		"",
        {GHSOUND_NONE, 0}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
	{
		"Generic",
        {GHSOUND_NONE, 0}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
	{
		"Goblin",
        {GHSOUND_NONE, 0}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
	{
		"Dragon",
        {GHSOUND_NONE, 0}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
	{
		"Dracolich",
        {GHSOUND_NONE, 0}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}, {GHSOUND_PLAYER_FOOTSTEPS_NORMAL, 1.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    },
    {
        "Bee",
        {GHSOUND_BEE, 1.0f}, SOUNDSOURCE_AMBIENT_GENERAL,
        {OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE, OBJECT_SOUNDSET_NONE},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}},
        {{0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}, {0, 0.0f}}
    }
};

NEARDATA struct object_soundset_definition object_soundsets[MAX_OBJECT_SOUNDSETS] =
{
    {
        "",
        {
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    },
    {
        "general",
        {
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    },
    {
        "candle",
        {
            {GHSOUND_FIRE, 0.3f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_LIT
    },
    {
        "quarterstaff",
        {
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_STAFF_SWING, 1.0f},
            {GHSOUND_STAFF_HIT, 1.0f},
            {GHSOUND_NONE, 1.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    }
};

NEARDATA struct location_soundset_definition location_soundsets[MAX_LOCATION_SOUNDSETS] =
{
    {
        "",
        {
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    },
    {
        "general",
        {
            {GHSOUND_NONE, 0.0f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    },
    {
        "fountain",
        {
            {GHSOUND_FOUNTAIN, 0.75f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_GENERAL
    },
    {
        "altar",
        {
            {GHSOUND_FIRE, 0.5f},
            {GHSOUND_NONE, 0.0f}
        },
        SOUNDSOURCE_AMBIENT_LIT
    }
};


NEARDATA struct effect_sound_definition effect_sounds[MAX_EFFECT_SOUNDS] =
{
    {
        "",
        {GHSOUND_NONE, 0.0f}
    },
    {
        "general",
        {GHSOUND_NONE, 0.0f}
    },
    {
        "shield effect",
        {GHSOUND_NONE, 0.0f}
    },
    {
        "talk effect",
        {GHSOUND_NONE, 0.0f}
    }
};


void
play_dungeon_music(level_ptr, x, y, music_flags)
d_level* level_ptr;
int x, y;
unsigned long music_flags;
{
	if (!level_ptr)
		return;

	int dnum = level_ptr->dnum;
	struct mkroom* room_ptr = (struct mkroom*)0;
	room_ptr = room_ptr;

	struct ghsound_music_info musicinfo = { 0 };
	musicinfo.ghsound = get_dungeon_music(dnum);
    musicinfo.volume = BACKGROUND_MUSIC_VOLUME;

	play_ghsound_music(musicinfo);
}

void
play_game_music()
{
    struct ghsound_music_info musicinfo = { 0 };
    musicinfo.volume = (u.uz.dnum == sokoban_dnum ? SOKOBAN_MUSIC_VOLUME : BACKGROUND_MUSIC_VOLUME);
    musicinfo.ghsound = GHSOUND_NONE;

    if (context.game_started == FALSE)
    {
        musicinfo.ghsound = GHSOUND_MUSIC_INTRO;
    }
    else
    {
        struct mkroom* room_ptr = which_room(u.ux, u.uy);
        if (!room_ptr)
            musicinfo.ghsound = get_level_music(&u.uz);
        else
            musicinfo.ghsound = get_room_music(room_ptr);
    }

    /* play_ghsound_music will check if the music is currently playing and then do nothing if this is the case */
    play_ghsound_music(musicinfo);
}

void
stop_music()
{
    struct ghsound_music_info musicinfo = { 0 };
    musicinfo.ghsound = GHSOUND_NONE;
    musicinfo.volume = 0.0f;
    musicinfo.stop_music = TRUE;

    play_ghsound_music(musicinfo);
}

void
play_level_ambient_sounds()
{
    struct ghsound_level_ambient_info lainfo = { 0 };
    lainfo.volume = BACKGROUND_MUSIC_VOLUME;
    lainfo.ghsound = GHSOUND_NONE;

    if (context.game_started == FALSE)
    {
        lainfo.ghsound = GHSOUND_MUSIC_INTRO;
    }
    else
    {
        struct mkroom* room_ptr = which_room(u.ux, u.uy);
        if (!room_ptr)
            lainfo.ghsound = get_level_ambient_sounds(&u.uz);
        else
            lainfo.ghsound = get_room_ambient_sounds(room_ptr);
    }

    /* play_ghsound_level_ambient will check if the ambient sound is currently playing and then do nothing if this is the case */
    /* GHSOUND_NONE will stop ambient sounds */
    play_ghsound_level_ambient(lainfo);
}

void
play_movement_sound(mtmp, movement_flags)
struct monst* mtmp;
unsigned long movement_flags;
{
	if (!mtmp)
		return;
	boolean isyou = (mtmp == &youmonst);

    struct ghsound_immediate_info immediateinfo = { 0 };
	enum floor_surface_types floorid = FLOOR_SURFACE_CARPET; /* Set the appropriate floor here */
	enum ghsound_types soundid = GHSOUND_NONE;
    float volume = 1.0f;

	if (isyou)
	{
		enum player_soundset_types pss = get_player_soundset();
		soundid = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_ON_GROUND].ghsound;
        volume = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_ON_GROUND].volume;
		if (Flying)
		{
			soundid = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_FLYING].ghsound;
            volume = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_FLYING].volume;
            floorid = FLOOR_SURFACE_NONE;

		}
		else if (Levitation)
		{
			soundid = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_LEVITATING].ghsound;
            volume = player_soundsets[pss].movement_sounds[MOVEMENT_STYLE_LEVITATING].volume;
            floorid = FLOOR_SURFACE_NONE;
		}
	}
	else
	{
		enum monster_soundset_types mss = mtmp->female ? mtmp->data->female_soundset : mtmp->data->soundset;
		soundid = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_ON_GROUND].ghsound;
        volume = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_ON_GROUND].volume;

		if (is_flying(mtmp))
		{
			soundid = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_FLYING].ghsound;
            volume = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_FLYING].volume;
            floorid = FLOOR_SURFACE_NONE;

		}
		else if (is_levitating(mtmp))
		{
			soundid = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_LEVITATING].ghsound;
            volume = monster_soundsets[mss].movement_sounds[MOVEMENT_STYLE_LEVITATING].volume;
            floorid = FLOOR_SURFACE_NONE;
		}

        if (isok(mtmp->mx, mtmp->my))
        {
            float hearing = hearing_array[mtmp->mx][mtmp->my];
            if (hearing == 0.0f)
                return;
            else
                volume *= hearing_array[mtmp->mx][mtmp->my];
        }
	}

    immediateinfo.ghsound = soundid;
    immediateinfo.volume = volume;
    immediateinfo.parameter_names[0] = "Surface";
    immediateinfo.parameter_values[0] = (float)floorid;
    immediateinfo.parameter_names[1] = (char*)0;

    if(soundid > GHSOUND_NONE && volume > 0.0f)
        play_immediate_ghsound(immediateinfo);
}

void
play_simple_weapon_sound(magr, attack_number, weapon, sound_type)
struct monst* magr;
int attack_number;
struct obj* weapon;
enum object_sound_types sound_type;
{
    /* Do not use for hit sounds */

    if (!magr)
        return;

    enum ghsound_types soundid = GHSOUND_NONE;
    float volume = 1.0f;
    boolean you_attack = (magr == &youmonst);
    struct ghsound_immediate_info immediateinfo = { 0 };

    if (weapon)
    {
        if (weapon->oartifact && artilist[weapon->oartifact].soundset > OBJECT_SOUNDSET_NONE)
        {
            enum object_soundset_types oss = artilist[weapon->oartifact].soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
        else
        {
            enum object_soundset_types oss = objects[weapon->otyp].oc_soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
    }
    else
    {
        if (you_attack)
        {
            enum player_soundset_types pss = get_player_soundset();
            enum object_soundset_types oss = player_soundsets[pss].barehanded_soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
        else
        {
            enum monster_soundset_types mss = magr->female ? magr->data->female_soundset : magr->data->soundset;
            enum object_soundset_types oss = monster_soundsets[mss].attack_soundsets[attack_number];
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
            if (isok(magr->mx, magr->my))
            {
                float hearing = hearing_array[magr->mx][magr->my];
                if (hearing == 0.0f)
                    return;
                else
                    volume *= hearing_array[magr->mx][magr->my];
            }
        }
    }

    immediateinfo.ghsound = soundid;
    immediateinfo.volume = volume;

    if(soundid > GHSOUND_NONE && volume > 0.0f)
        play_immediate_ghsound(immediateinfo);

}

void
play_hit_sound(magr, mdef, attack_number, weapon, damage, thrown)
struct monst* magr;
struct monst* mdef;
int attack_number;
struct obj* weapon;
double damage;
enum hmon_atkmode_types thrown;
{
    if (!magr || !mdef)
        return;

    enum ghsound_types soundid = GHSOUND_NONE;
    float volume = 1.0f;
    boolean you_attack = (magr == &youmonst);
    boolean you_defend = (mdef == &youmonst);
    enum object_sound_types sound_type = (thrown == HMON_MELEE ? OBJECT_SOUND_TYPE_HIT_MELEE : OBJECT_SOUND_TYPE_HIT_THROW);
    struct ghsound_immediate_info immediateinfo = { 0 };
    xchar defx = 0, defy = 0;
    if (you_defend)
    {
        defx = u.ux;
        defy = u.uy;
    }
    else
    {
        defx = mdef->mx;
        defy = mdef->my;
    }

    if (weapon)
    {
        if (weapon->oartifact && artilist[weapon->oartifact].soundset > OBJECT_SOUNDSET_NONE)
        {
            enum object_soundset_types oss = artilist[weapon->oartifact].soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
        else
        {
            enum object_soundset_types oss = objects[weapon->otyp].oc_soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
    }
    else
    {
        if (you_attack)
        {
            enum player_soundset_types pss = get_player_soundset();
            enum object_soundset_types oss = player_soundsets[pss].barehanded_soundset;
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
        }
        else
        {
            enum monster_soundset_types mss = magr->female ? magr->data->female_soundset : magr->data->soundset;
            enum object_soundset_types oss = monster_soundsets[mss].attack_soundsets[attack_number];
            soundid = object_soundsets[oss].sounds[sound_type].ghsound;
            volume = object_soundsets[oss].sounds[sound_type].volume;
            /* Hit sound is based on the defender's location */
            if (isok(defx, defy))
            {
                float hearing = hearing_array[defx][defy];
                if (hearing == 0.0f)
                    return;
                else
                    volume *= hearing_array[defx][defy];
            }
        }
    }

    enum strike_surface_types surfaceid = STRIKE_SURFACE_FLESH;
    if (you_defend)
    {
        surfaceid = STRIKE_SURFACE_FLESH;
    }
    else if(mdef)
    {
        if(is_wooden(mdef->data))
            surfaceid = STRIKE_SURFACE_WOOD;
        else if (is_dragon(mdef->data))
            surfaceid = STRIKE_SURFACE_SCALES;
        else if (thick_skinned(mdef->data))
            surfaceid = STRIKE_SURFACE_THICK_HIDE;
        else if (is_whirly(mdef->data))
            surfaceid = STRIKE_SURFACE_AIR;
        else if (flaming(mdef->data))
            surfaceid = STRIKE_SURFACE_FIRE;
        else if (is_watery(mdef->data))
            surfaceid = STRIKE_SURFACE_WATER;
        else if (is_stony(mdef->data))
            surfaceid = STRIKE_SURFACE_STONE;
        else if (noncorporeal(mdef->data))
            surfaceid = STRIKE_SURFACE_INCORPOREAL;
    }

    immediateinfo.ghsound = soundid;
    immediateinfo.volume = volume;
    immediateinfo.parameter_names[0] = "Surface";
    immediateinfo.parameter_values[0] = (float)surfaceid;
    immediateinfo.parameter_names[1] = "Damage";
    immediateinfo.parameter_values[1] = (float)damage;
    immediateinfo.parameter_names[2] = (char*)0;

    if (soundid > GHSOUND_NONE && volume > 0.0f)
        play_immediate_ghsound(immediateinfo);

}

void
play_effect_sound(effect_sound_id)
enum effect_sounds_types effect_sound_id;
{

    enum ghsound_types soundid = GHSOUND_NONE;
    float volume = 1.0f;

    soundid = effect_sounds[effect_sound_id].sound.ghsound;
    volume = effect_sounds[effect_sound_id].sound.volume;

    struct ghsound_immediate_info immediateinfo = { 0 };
    immediateinfo.ghsound = soundid;
    immediateinfo.volume = volume;

    if (soundid > GHSOUND_NONE && volume > 0.0f)
        play_immediate_ghsound(immediateinfo);
}


void
play_effect_sound_at_location(effect_sound_id, x, y)
enum effect_sounds_types effect_sound_id;
int x, y;
{
    if (!isok(x, y) || hearing_array[x][y] == 0.0f)
        return;

    enum ghsound_types soundid = GHSOUND_NONE;
    float volume = 1.0f;

    soundid = effect_sounds[effect_sound_id].sound.ghsound;
    volume = effect_sounds[effect_sound_id].sound.volume;

    struct ghsound_immediate_info immediateinfo = { 0 };
    immediateinfo.ghsound = soundid;
    immediateinfo.volume = volume * hearing_array[x][y];

    if (immediateinfo.ghsound > GHSOUND_NONE && immediateinfo.volume > 0.0f)
        play_immediate_ghsound(immediateinfo);
}


enum player_soundset_types
get_player_soundset()
{
	return PLAYER_SOUNDSET_GENERAL;
}

void
dosetsoundvolume()
{
	adjust_ghsound_general_volumes();
}

void
update_hearing_array(mode)
int mode; /* 0 = normal, 1 = clear */
{
	/* Clear array*/
	memset(hearing_array, 0, sizeof(hearing_array));

	/* Can't hear anything */
	if (Deaf || mode == 1)
		return;

	int hear_distance = get_max_hearing_distance();

	/* Fill the array */
	hearing_array[u.ux][u.uy] = 1.0f;

	for (int r = 1; r <= hear_distance; r++)
	{
		int x_min = u.ux - r;
		int x_max = u.ux + r;
		int x_min_adjusted = max(1, x_min);
		int x_max_adjusted = min(COLNO - 1, x_max);

		int y_min = u.uy - r;
		int y_max = u.uy + r;
		int y_min_adjusted = max(0, y_min);
		int y_max_adjusted = min(COLNO - 1, y_max);

		boolean horizontal_min_done = FALSE;
		boolean horizontal_max_done = FALSE;

		/* 1. Horizontal lines, left to right */
		for (int i = 0; i <= 1; i++)
		{
			if (i == 0)
			{
				if(y_min_adjusted != y_min)
					continue;

				horizontal_min_done = TRUE;
			}
			else if (i == 1)
			{
				if(y_max_adjusted != y_max)
					continue;

				horizontal_max_done = TRUE;
			}

			int y = (i == 0 ? y_min_adjusted : y_max_adjusted);

			
			for (int x = x_min_adjusted; x <= x_max_adjusted; x++)
			{
				float prev_hearing = 0.0f;
				int prev_y = (i == 0) ? y + 1 : y - 1;

				if (r == 1)
				{
					prev_hearing = 1.0f;
				}
				else if (prev_y > y_max_adjusted || prev_y < y_min_adjusted)
				{
					//Nothing
				}
				else
				{
					float maximum = 0.0f;

					/* Take maximum from above or below from the previous round */
					for (int prev_x = max(x_min_adjusted + 1, x - 1); prev_x <= min(x_max_adjusted - 1, x + 1); prev_x++)
					{
						maximum = max(maximum, hearing_array[prev_x][prev_y]);
					}
	
					/* Take also previous from the same line */
					if(x > x_min_adjusted)
						maximum = max(maximum, hearing_array[x - 1][y]);

					prev_hearing = maximum;
				}

				if (prev_hearing == 0.0f)
				{
					continue;
					/* Current hearing is 0, too */
				}
				else
				{
					set_hearing_array(x, y, prev_hearing);
				}
			}
		}

		/* Vertical lines, top to bottom */
		for (int i = 0; i <= 1; i++)
		{
			if (i == 0 && x_min_adjusted != x_min)
				continue;
			if (i == 1 && x_max_adjusted != x_max)
				continue;

			int x = (i == 0 ? x_min_adjusted : x_max_adjusted);
			int upper_y_limit = y_max_adjusted - (horizontal_max_done ? 1 : 0);

			for (int y = y_min_adjusted + (horizontal_min_done ? 1 : 0); y <= upper_y_limit; y++)
			{
				float prev_hearing = 0.0f;
				int prev_x = (i == 0) ? x + 1 : x - 1;

				if (r == 1)
				{
					prev_hearing = 1.0f;
				}
				else if (prev_x > x_max_adjusted || prev_x < x_min_adjusted)
				{
					//Nothing
				}
				else
				{
					float maximum = 0.0f;
					for (int prev_y = max(y_min_adjusted + 1, y - 1); prev_y <= min(y_max_adjusted - 1, y + 1); prev_y++)
					{
						maximum = max(maximum, hearing_array[prev_x][prev_y]);
					}

					/* Take also previous from the same line */
					if (y > y_min_adjusted)
						maximum = max(maximum, hearing_array[x][y - 1]);

					prev_hearing = maximum;
				}

				if (prev_hearing == 0.0f)
				{
					continue;
					/* Current hearing is 0, too */
				}
				else
				{
					set_hearing_array(x, y, prev_hearing);
				}
			}
		}

		/* Vertical lines, bottom to top */
		for (int i = 0; i <= 1; i++)
		{
			if (i == 0 && x_max_adjusted != x_max)
				continue;
			if (i == 1 && x_min_adjusted != x_min)
				continue;

			int x = (i == 0 ? x_max_adjusted : x_min_adjusted);

			for (int y = y_max_adjusted - 1 ; y >= y_min_adjusted; y--)
			{
				if(hearing_array[x][y + 1] > 0.0f)
					set_hearing_array(x, y, hearing_array[x][y + 1]);
			}
		}

		/* Horizontal lines, right to left */
		for (int i = 0; i <= 1; i++)
		{
			if (i == 0 && y_max_adjusted != y_max)
				continue;
			if (i == 1 && y_min_adjusted != y_min)
				continue;

			int y = (i == 0 ? y_max_adjusted : y_min_adjusted);

			for (int x = x_max_adjusted - 1; x >= x_min_adjusted; x--)
			{
				if (hearing_array[x + 1][y] > 0.0f)
					set_hearing_array(x, y, hearing_array[x + 1][y]);
			}
		}
	}

}

STATIC_OVL
void set_hearing_array(x, y, prev_hearing)
int x, y;
double prev_hearing;
{
	struct monst* mtmp;
	if (IS_ROCK(levl[x][y].typ) && !IS_TREE(levl[x][y].typ))
	{
		/* Nothing */
	}
	else if ((mtmp = m_at(x, y)) && is_lightblocker_mappear(mtmp))
	{
		/* Nothing */
	}
	else if (IS_DOOR(levl[x][y].typ) && (levl[x][y].doormask != 0 && (levl[x][y].doormask & (D_NODOOR | D_ISOPEN | D_BROKEN)) == 0))
	{
		float new_hearing = (float)max(0.0f, min(1.0f, (prev_hearing) / (10.0f * 2.0f)));
		if(new_hearing > hearing_array[x][y])
			hearing_array[x][y] = new_hearing;
	}
	else
	{
		float new_hearing = (float)max(0.0f, min(1.0f, (prev_hearing) / 2.0f));
		if (new_hearing > hearing_array[x][y])
			hearing_array[x][y] = new_hearing;
	}

}

void
update_ambient_sounds()
{
    for (struct soundsource_t* curr = sound_base; curr; curr = curr->next)
    {
        if (!isok(curr->x, curr->y))
            continue;

        boolean lit = FALSE;

        /* Update sound source location */
        if (curr->type == SOUNDSOURCE_OBJECT)
        {
            if (get_obj_location(curr->id.a_obj, &curr->x, &curr->y, 0))
                ;

            if (curr->id.a_obj)
                lit = curr->id.a_obj->lamplit;
        }
        else if (curr->type == SOUNDSOURCE_MONSTER)
        {
            if (get_mon_location(curr->id.a_monst, &curr->x, &curr->y, 0))
                ;

            if (curr->id.a_monst)
                lit = curr->id.a_monst->data->lightrange;
        }
        else if (curr->type == SOUNDSOURCE_LOCATION)
        {
            curr->x = curr->id.a_coord.x;
            curr->y = curr->id.a_coord.y;

            if (isok(curr->id.a_coord.x, curr->id.a_coord.y))
                lit = levl[curr->id.a_coord.x][curr->id.a_coord.y].lamplit;
        }

        /* Update sound source heard volume */
        float old_heard_volume = curr->heard_volume;
        float hearing_volume = hearing_array[curr->x][curr->y];
        float total_volume = 0.0f;
        if (hearing_volume > 0.0f)
        {
            float source_volume = curr->source_volume;
            total_volume = source_volume * hearing_volume;
            if (curr->subtype == SOUNDSOURCE_AMBIENT_LIT && !lit)
                total_volume = 0.0f;
        }

        if(total_volume < 0.001f)
            total_volume = 0.0f; /* turn off if too quiet */

        curr->heard_volume = total_volume;
        if(curr->heard_volume != old_heard_volume)
            set_ambient_ghsound_volume(curr);
    }
}

void
update_hearing_array_and_ambient_sounds()
{
	update_hearing_array(0);
	update_ambient_sounds();
}

void
clear_hearing_array_and_ambient_sounds()
{
    update_hearing_array(1);
    update_ambient_sounds();
}

int get_max_hearing_distance()
{
	return (u.uswallow ? 1 : Underwater ? 2 : MAX_HEARING_DISTANCE);
}

void
update_hearing_array_and_ambient_sounds_if_point_within_hearing_range(px, py)
int px, py;
{
	int max_hear_dist = get_max_hearing_distance();
	if (abs(u.ux - px) <= max_hear_dist && abs(u.uy - py) <= max_hear_dist)
		update_hearing_array_and_ambient_sounds();
}

void
unblock_vision_and_hearing_at_point(x, y)
int x, y;
{
	unblock_point(x, y);
	update_hearing_array_and_ambient_sounds_if_point_within_hearing_range(x, y);
}

void
block_vision_and_hearing_at_point(x, y)
int x, y;
{
	block_point(x, y);
	update_hearing_array_and_ambient_sounds_if_point_within_hearing_range(x, y);
}




/*
 * Ambient sound sources.
 *
 * This implementation minimizes memory at the expense of extra
 * recalculations.
 *
 * Sound sources are "things" that have a physical position and volume.
 * They have a type, which gives us information about them.  Currently
 * they are only attached to objects, monsters, and locations.  Note:  the
 * polymorphed-player handling assumes that both youmonst.m_id and
 * youmonst.mx will always remain 0.
 *
 * Sound sources, like timers, either follow game play (RANGE_GLOBAL) or
 * stay on a level (RANGE_LEVEL).  Sound sources are unique by their
 * (type, id) pair.  For sound sources attached to objects, this id
 * is a pointer to the object.
 *
 * The structure of the save/restore mechanism is amazingly similar to
 * the timer save/restore.  This is because they both have the same
 * principals of having pointers into objects that must be recalculated
 * across saves and restores.
 */

 /* flags */
#define SSF_SHOW 0x1        /* display the sound source */
#define SSF_NEEDS_FIXUP 0x2 /* need oid fixup */
#define SSF_SILENCE_SOURCE 0x4 /* emits silence rather than sound */

STATIC_DCL void FDECL(write_soundsource, (int, sound_source*));
STATIC_DCL int FDECL(maybe_write_soundsource, (int, int, BOOLEAN_P));


/* Create a new sound source.  */
void
new_sound_source(x, y, ghsound, volume, type, subtype, id)
xchar x, y;
enum ghsound_types ghsound;
double volume;
enum soundsource_types type;
enum soundsource_ambient_subtypes subtype;
anything* id;
{
    sound_source* ss;
    double absvolume = volume > 0.0 ? volume : -volume;
    if (absvolume > 1.0 || absvolume < 0.0) 
    {
        impossible("new_sound_source:  illegal volume %d", volume);
        return;
    }

    ss = (sound_source*)alloc(sizeof(sound_source));

    ss->next = sound_base;
    ss->x = x;
    ss->y = y;
    ss->ghsound = ghsound;
    ss->source_volume = ((float)absvolume);
    ss->heard_volume = ss->source_volume * hearing_array[x][y];
    ss->type = type;
    ss->subtype = subtype;
    ss->id = *id;
    ss->flags = volume < 0 ? SSF_SILENCE_SOURCE : 0;
    sound_base = ss;

    add_ambient_ghsound(ss);

    hearing_full_recalc = 1; /* make the source show up */
}

/*
 * Delete a sound source. This assumes only one sound source is attached
 * to an object at a time.
 */
void
del_sound_source(type, id)
int type;
anything* id;
{
    sound_source* curr, * prev;
    anything tmp_id;

    tmp_id = zeroany;
    /* need to be prepared for dealing a with sound source which
       has only been partially restored during a level change
       (in particular: chameleon vs prot. from shape changers) */
    switch (type)
    {
    case SOUNDSOURCE_OBJECT:
        tmp_id.a_uint = id->a_obj->o_id;
        break;
    case SOUNDSOURCE_MONSTER:
        tmp_id.a_uint = id->a_monst->m_id;
        break;
    case SOUNDSOURCE_LOCATION:
        tmp_id = zeroany;
        tmp_id.a_coord.x = id->a_coord.x;
        tmp_id.a_coord.y = id->a_coord.y;
        break;
    default:
        tmp_id.a_uint = 0;
        break;
    }

    for (prev = 0, curr = sound_base; curr; prev = curr, curr = curr->next)
    {
        if (curr->type != type)
            continue;

        if ((type == SOUNDSOURCE_LOCATION && curr->id.a_coord.x == tmp_id.a_coord.x && curr->id.a_coord.y == tmp_id.a_coord.y)
            || (type != SOUNDSOURCE_LOCATION && curr->id.a_obj == ((curr->flags & SSF_NEEDS_FIXUP) ? tmp_id.a_obj : id->a_obj))
            )
        {
            if (prev)
                prev->next = curr->next;
            else
                sound_base = curr->next;

            if (delete_ambient_ghsound != 0)
                delete_ambient_ghsound(curr);

            free((genericptr_t)curr);
            hearing_full_recalc = 1;
            return;
        }
    }
    impossible("del_sound_source: not found type=%d, id=%s", type,
        fmt_ptr((genericptr_t)id->a_obj));
}


/* Save all sound sources of the given volume. */
void
save_sound_sources(fd, mode, volume)
int fd, mode, volume;
{
    int count, actual, is_global;
    sound_source** prev, * curr;

    if (perform_bwrite(mode))
    {
        count = maybe_write_soundsource(fd, volume, FALSE);
        bwrite(fd, (genericptr_t)&count, sizeof count);
        actual = maybe_write_soundsource(fd, volume, TRUE);
        if (actual != count)
        {
            panic("counted %d sound sources, wrote %d! [volume=%d]", count,
                actual, volume);
            return;
        }
    }

    if (release_data(mode)) 
    {
        for (prev = &sound_base; (curr = *prev) != 0;) 
        {
            if (!curr->id.a_monst)
            {
                impossible("save_sound_sources: no id! [volume=%d]", volume);
                is_global = 0;
            }
            else
                switch (curr->type) 
                {
                case SOUNDSOURCE_OBJECT:
                    is_global = !obj_is_local(curr->id.a_obj);
                    break;
                case SOUNDSOURCE_MONSTER:
                    is_global = !mon_is_local_mx(curr->id.a_monst);
                    break;
                case SOUNDSOURCE_LOCATION:
                    is_global = 0; /* always local by definition */
                    break;
                default:
                    is_global = 0;
                    impossible("save_sound_sources: bad type (%d) [volume=%d]",
                        curr->type, volume);
                    break;
                }
            /* if global and not doing local, or vice versa, remove it */
            if (is_global ^ (volume == RANGE_LEVEL)) 
            {
                *prev = curr->next;
                if(delete_ambient_ghsound != 0)
                    delete_ambient_ghsound(curr);
                free((genericptr_t)curr);
            }
            else
            {
                prev = &(*prev)->next;
            }
        }
    }
}

/*
 * Pull in the structures from disk, but don't recalculate the object
 * pointers.
 */
void
restore_sound_sources(fd)
int fd;
{
    int count;
    sound_source* ss;

    /* restore elements */
    mread(fd, (genericptr_t)&count, sizeof count);

    while (count-- > 0) {
        ss = (sound_source*)alloc(sizeof(sound_source));
        mread(fd, (genericptr_t)ss, sizeof(sound_source));
        ss->next = sound_base;
        sound_base = ss;
        add_ambient_ghsound(ss);
    }
}

/* to support '#stats' wizard-mode command */
void
sound_stats(hdrfmt, hdrbuf, count, size)
const char* hdrfmt;
char* hdrbuf;
long* count;
size_t* size;
{
    sound_source* ss;

    Sprintf(hdrbuf, hdrfmt, sizeof(sound_source));
    *count = *size = 0L;
    for (ss = sound_base; ss; ss = ss->next) {
        ++* count;
        *size += sizeof * ss;
    }
}

/* Relink all sounds that are so marked. */
void
relink_sound_sources(ghostly)
boolean ghostly;
{
    char which;
    unsigned nid;
    sound_source* ss;

    for (ss = sound_base; ss; ss = ss->next)
    {
        if (ss->flags & SSF_NEEDS_FIXUP)
        {
            if (ss->type == SOUNDSOURCE_OBJECT || ss->type == SOUNDSOURCE_MONSTER || ss->type == SOUNDSOURCE_LOCATION)
            {
                if (ghostly && ss->type != SOUNDSOURCE_LOCATION)
                {
                    if (!lookup_id_mapping(ss->id.a_uint, &nid))
                        impossible("relink_sound_sources: no id mapping");
                }
                else
                    nid = ss->id.a_uint;

                if (ss->type == SOUNDSOURCE_OBJECT)
                {
                    which = 'o';
                    ss->id.a_obj = find_oid(nid);
                }
                else if (ss->type == SOUNDSOURCE_MONSTER)
                {
                    which = 'm';
                    ss->id.a_monst = find_mid(nid, FM_EVERYWHERE);
                }
                else if (ss->type == SOUNDSOURCE_LOCATION)
                {
                    which = 'l';
                    ss->id = zeroany;
                    ss->id.a_coord.x = ss->x;
                    ss->id.a_coord.y = ss->y;
                }

                if (!ss->id.a_monst)
                    impossible("relink_sound_sources: cant find %c_id %d", which, nid);
            }
            else
                impossible("relink_sound_sources: bad type (%d)", ss->type);

            ss->flags &= ~SSF_NEEDS_FIXUP;
        }
    }
}

/*
 * Part of the sound source save routine.  Count up the number of sound
 * sources that would be written.  If write_it is true, actually write
 * the sound source out.
 */
STATIC_OVL int
maybe_write_soundsource(fd, volume, write_it)
int fd, volume;
boolean write_it;
{
    int count = 0, is_global;
    sound_source* ss;

    for (ss = sound_base; ss; ss = ss->next)
    {
        if (!ss->id.a_monst)
        {
            impossible("maybe_write_soundsource: no id! [volume=%d]", volume);
            continue;
        }

        switch (ss->type)
        {
        case SOUNDSOURCE_OBJECT:
            is_global = !obj_is_local(ss->id.a_obj);
            break;
        case SOUNDSOURCE_MONSTER:
            is_global = !mon_is_local_mx(ss->id.a_monst);
            break;
        case SOUNDSOURCE_LOCATION:
            is_global = 0; /* always local */
            break;
        default:
            is_global = 0;
            impossible("maybe_write_soundsource: bad type (%d) [volume=%d]", ss->type, volume);
            break;
        }

        /* if global and not doing local, or vice versa, count it */
        if (is_global ^ (volume == RANGE_LEVEL))
        {
            count++;
            if (write_it)
                write_soundsource(fd, ss);
        }
    }

    return count;
}

void
sound_sources_sanity_check()
{
    sound_source* ss;
    struct monst* mtmp;
    struct obj* otmp;
    unsigned int auint = 0;

    for (ss = sound_base; ss; ss = ss->next)
    {
        if (!ss->id.a_monst)
        {
            panic("insane sound source: no id!");
            return;
        }
        if (ss->type == SOUNDSOURCE_OBJECT)
        {
            otmp = (struct obj*)ss->id.a_obj;
            if (otmp)
                auint = otmp->o_id;

            if (find_oid(auint) != otmp)
            {
                panic("insane sound source: can't find obj #%u!", auint);
                return;
            }
        }
        else if (ss->type == SOUNDSOURCE_MONSTER)
        {
            mtmp = (struct monst*)ss->id.a_monst;
            if (mtmp)
                auint = mtmp->m_id;

            if (find_mid(auint, FM_EVERYWHERE) != mtmp)
            {
                panic("insane sound source: can't find mon #%u!", auint);
                return;
            }
        }
        else if (ss->type == SOUNDSOURCE_LOCATION)
        {
            coord c = ss->id.a_coord;
            if (!isok(c.x, c.y))
            {
                panic("insane sound source: invalid location coordinates (%d, %d)!", c.x, c.y);
                return;
            }
        }
        else
        {
            panic("insane sound source: bad ss type %d", ss->type);
            return;
        }
    }
}

/* Write a sound source structure to disk. */
STATIC_OVL void
write_soundsource(fd, ss)
int fd;
sound_source* ss;
{
    anything arg_save;
    struct obj* otmp;
    struct monst* mtmp;

    if (ss->type == SOUNDSOURCE_OBJECT || ss->type == SOUNDSOURCE_MONSTER || ss->type == SOUNDSOURCE_LOCATION)
    {
        if (ss->flags & SSF_NEEDS_FIXUP)
        {
            bwrite(fd, (genericptr_t)ss, sizeof(sound_source));
        }
        else
        {
            /* replace object pointer with id for write, then put back */
            arg_save = ss->id;
            if (ss->type == SOUNDSOURCE_OBJECT)
            {
                otmp = ss->id.a_obj;
                ss->id = zeroany;
                ss->id.a_uint = otmp->o_id;
                if (find_oid((unsigned)ss->id.a_uint) != otmp)
                    impossible("write_soundsource: can't find obj #%u!",
                        ss->id.a_uint);
            }
            else if (ss->type == SOUNDSOURCE_MONSTER)
            { /* ss->type == SOUNDSOURCE_MONSTER */
                mtmp = (struct monst*)ss->id.a_monst;
                ss->id = zeroany;
                ss->id.a_uint = mtmp->m_id;
                if (find_mid((unsigned)ss->id.a_uint, FM_EVERYWHERE) != mtmp)
                    impossible("write_soundsource: can't find mon #%u!",
                        ss->id.a_uint);
            }
            else if (ss->type == SOUNDSOURCE_LOCATION)
            {
                /* No need to do anything, coord can be written to disk as is */
            }

            ss->flags |= SSF_NEEDS_FIXUP;
            bwrite(fd, (genericptr_t)ss, sizeof(sound_source));
            ss->id = arg_save;
            ss->flags &= ~SSF_NEEDS_FIXUP;
        }
    }
    else
    {
        impossible("write_soundsource: bad type (%d)", ss->type);
    }
}

/* Change sound source's ID from src to dest. */
void
obj_move_sound_source(src, dest)
struct obj* src, * dest;
{
    sound_source* ss;

    for (ss = sound_base; ss; ss = ss->next)
    {
        if (ss->type == SOUNDSOURCE_OBJECT && ss->id.a_obj == src)
            ss->id.a_obj = dest;
    }

    src->makingsound = 0;
    dest->makingsound = 1;
}

/* return true if there exist any sound sources */
boolean
any_sound_source()
{
    return (boolean)(sound_base != (sound_source*)0);
}

/*
 * Snuff an object sound source if at (x,y).  This currently works
 * only for burning sound sources.
 */
void
snuff_sound_source(x, y)
int x, y;
{
    sound_source* ss;
    struct obj* obj;

    for (ss = sound_base; ss; ss = ss->next)
        /*
         * Is this position check valid??? Can I assume that the positions
         * will always be correct because the objects would have been
         * updated with the last vision update?  [Is that recent enough???]
         */
        if (ss->type == SOUNDSOURCE_OBJECT && ss->x == x && ss->y == y) {
            obj = ss->id.a_obj;
            if (1) //obj_is_burning(obj)) 
            {
                del_sound_source(LS_OBJECT, obj_to_any(obj));
                end_sound(obj, FALSE);
                /*
                 * The current ss element has just been removed (and
                 * ss->next is now invalid).  Return assuming that there
                 * is only one sound source attached to each object.
                 */
                return;
            }
        }
        else if (ss->type == LS_LOCATION && ss->x == x && ss->y == y)
        {
            if (levl[x][y].makingsound)
            {
                levl[x][y].makingsound = 0;
                del_sound_source(LS_LOCATION, xy_to_any(x, y));
                newsym(x, y);
                return;
            }
        }

}


/* copy the sound source(s) attached to src, and attach it/them to dest */
void
obj_split_sound_source(src, dest)
struct obj* src, * dest;
{
    sound_source* ss, * new_ss;

    for (ss = sound_base; ss; ss = ss->next)
        if (ss->type == SOUNDSOURCE_OBJECT && ss->id.a_obj == src) 
        {
            /*
             * Insert the new source at beginning of list.  This will
             * never interfere us walking down the list - we are already
             * past the insertion point.
             */
            new_ss = (sound_source*)alloc(sizeof(sound_source));
            *new_ss = *ss;
            new_ss->id.a_obj = dest;
            new_ss->next = sound_base;
            sound_base = new_ss;
            //dest->lamplit = 1; /* now an active sound source */
        }
}

/* sound source `src' has been folded into sound source `dest';
   used for merging lit candles and adding candle(s) to lit candelabrum */
void
obj_merge_sound_sources(src, dest)
struct obj* src, * dest;
{
    sound_source* ss;

    /* src == dest implies adding to candelabrum */
    if (src != dest)
        del_sound_source(LS_OBJECT, obj_to_any(src));
    //end_burn(src, TRUE); /* extinguish candles */


    for (ss = sound_base; ss; ss = ss->next)
        if (ss->type == SOUNDSOURCE_OBJECT && ss->id.a_obj == dest) 
        {
            //ss->volume = candle_sound_range(dest);
            hearing_full_recalc = 1; /* in case volume changed */
            break;
        }
}

/* sound source `obj' is being made brighter or dimmer */
void
obj_adjust_sound_volume(obj, new_volume)
struct obj* obj;
double new_volume;
{
    sound_source* ss;

    for (ss = sound_base; ss; ss = ss->next)
        if (ss->type == SOUNDSOURCE_OBJECT && ss->id.a_obj == obj) {
            if ((float)new_volume != ss->source_volume)
                hearing_full_recalc = 1;
            ss->source_volume = (float)new_volume;
            return;
        }
    impossible("obj_adjust_sound_volume: can't find %s", xname(obj));
}


/*
 * Start a burn timeout on the given object. If not "already lit" then
 * create a light source for the vision system.  There had better not
 * be a burn already running on the object.
 *
 */
void
begin_sound(obj, already_making_noise)
struct obj* obj;
boolean already_making_noise;
{
    long turns = 0;
    boolean do_timer = TRUE;

    obj->makingsound = 1;
    do_timer = FALSE;
    turns = 0;

    if (do_timer) 
    {
        if (start_timer(turns, TIMER_OBJECT, MAKE_SOUND_OBJECT, obj_to_any(obj))) 
        {
            obj->makingsound = 1;
            obj->age -= turns; /* Needs own timer, otherwise possible conflict with light sources */
            if (carried(obj) && !already_making_noise)
                update_inventory();
        }
        else 
        {
            obj->makingsound = 0;
        }
    }
    else 
    {
        if (carried(obj) && !already_making_noise)
            update_inventory();
    }

    if (obj->makingsound && !already_making_noise) 
    {
        xchar x, y;
        enum object_soundset_types objsoundset = objects[obj->otyp].oc_soundset;
        enum ghsound_types ghsound = object_soundsets[objsoundset].sounds[OBJECT_SOUND_TYPE_AMBIENT].ghsound;
        float volume = object_soundsets[objsoundset].sounds[OBJECT_SOUND_TYPE_AMBIENT].volume;
        enum soundsource_ambient_subtypes subtype = object_soundsets[objsoundset].ambient_subtype;

        if (get_obj_location(obj, &x, &y, CONTAINED_TOO | BURIED_TOO))
            new_sound_source(x, y, ghsound, (double)volume, SOUNDSOURCE_OBJECT, subtype, obj_to_any(obj));
        else
            impossible("begin_sound: can't get obj position");
    }
}

/*
 * Stop a burn timeout on the given object if timer attached.  Darken
 * light source.
 */
void
end_sound(obj, timer_attached)
struct obj* obj;
boolean timer_attached;
{
    if (!obj->makingsound) 
    {
        impossible("end_sound: obj %s not making sound", xname(obj));
        return;
    }

    if (1) //obj->otyp == MAGIC_LAMP || obj->otyp == MAGIC_CANDLE || artifact_light(obj) || obj_shines_magical_light(obj))
        timer_attached = FALSE;

    if (!timer_attached) {
        /* [DS] Cleanup explicitly, since timer cleanup won't happen */
        del_sound_source(LS_OBJECT, obj_to_any(obj));
        obj->makingsound = 0;
        if (obj->where == OBJ_INVENT)
            update_inventory();
    }
    else if (!stop_timer(MAKE_SOUND_OBJECT, obj_to_any(obj)))
        impossible("end_sound: obj %s not timed!", xname(obj));
}

boolean
obj_has_sound_source(obj)
struct obj* obj;
{
    return (obj->makingsound == TRUE);
}

enum ghsound_types
obj_ambient_sound(obj)
struct obj* obj;
{
    if (!obj || objects[obj->otyp].oc_soundset == OBJECT_SOUNDSET_NONE)
        return GHSOUND_NONE;

    return object_soundsets[objects[obj->otyp].oc_soundset].sounds[OBJECT_SOUND_TYPE_AMBIENT].ghsound;
}

double
obj_ambient_sound_volume(obj)
struct obj* obj;
{
    if (!obj || objects[obj->otyp].oc_soundset == OBJECT_SOUNDSET_NONE)
        return 0.0;

    return (double)object_soundsets[objects[obj->otyp].oc_soundset].sounds[OBJECT_SOUND_TYPE_AMBIENT].volume;
}

enum ghsound_types
get_location_ambient_sound_type(x, y, volume_ptr, subtype_ptr)
xchar x, y;
double* volume_ptr;
enum soundsource_ambient_subtypes *subtype_ptr;
{
    if (!isok(x, y))
        return GHSOUND_NONE;

    struct rm* lev = &levl[x][y];

    enum location_soundset_types lsoundset = level_location_types[lev->typ].soundset;

    if (lsoundset == LOCATION_SOUNDSET_NONE)
        return GHSOUND_NONE;

    if (volume_ptr)
        *volume_ptr = (double)location_soundsets[lsoundset].sounds[LOCATION_SOUND_TYPE_AMBIENT].volume;

    if (subtype_ptr)
        *subtype_ptr = location_soundsets[lsoundset].ambient_subtype;

    return location_soundsets[lsoundset].sounds[LOCATION_SOUND_TYPE_AMBIENT].ghsound;
}

enum ghsound_types
get_dungeon_music(dnum)
int dnum;
{
    enum ghsound_types res = GHSOUND_NONE;

    if (Inhell)
        res = GHSOUND_GEHENNOM_MUSIC_NORMAL;
    else if (dnum == mines_dnum)
        res = GHSOUND_GNOMISH_MINES_MUSIC_NORMAL;
    else if (dnum == sokoban_dnum)
        res = GHSOUND_SOKOBAN_MUSIC_NORMAL;
    else if (dnum == quest_dnum)
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL;
    else if (dnum == tower_dnum)
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL;
    else if (dnum == modron_dnum)
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL;
    else if (dnum == bovine_dnum)
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL;
    else
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL;

    return res;
}

enum ghsound_types
get_level_music(dlvl)
struct d_level* dlvl;
{
    enum ghsound_types res = GHSOUND_NONE;
    if (!dlvl)
        return res;

    int dnum = dlvl->dnum;


    if (Is_valley(dlvl))
        return GHSOUND_GEHENNOM_MUSIC_VALLEY;
    else if (Is_sanctum(dlvl))
        return GHSOUND_GEHENNOM_MUSIC_SANCTUM;
    else if (Is_stronghold(dlvl))
        return GHSOUND_DUNGEON_NORMAL_MUSIC_CASTLE;
    else if (Is_medusa_level(dlvl))
        return GHSOUND_DUNGEON_NORMAL_MUSIC_MEDUSA;
    else if (Is_asmo_level(dlvl))
        return GHSOUND_GEHENNOM_MUSIC_NORMAL;
    if (Is_minetown_level(dlvl))
        return GHSOUND_GNOMISH_MINES_MUSIC_TOWN;
    else
        return get_dungeon_music(dnum);

    return res;
}

enum ghsound_types
get_room_music(room)
struct mkroom* room;
{
    enum roomtype_types rtype = room->rtype;
    enum ghsound_types res = get_level_music(&u.uz);;

    switch (rtype)
    {
    case OROOM:
        break;
    case COURT:
        break;
    case SWAMP:
        break;
    case VAULT:
        break;
    case BEEHIVE:
        break;
    case DRAGONLAIR:
        break;
    case LIBRARY:
        break;
    case GARDEN:
        break;
    case MORGUE:
        break;
    case BARRACKS:
        break;
    case ZOO:
        break;
    case DELPHI:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_ORACLE;
        break;
    case TEMPLE:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_TEMPLE;
        break;
    case LEPREHALL:
        break;
    case COCKNEST:
        break;
    case ANTHOLE:
        break;
    case DESERTEDSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_DESERTED;
        break;
    case ARMORSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case SHOPBASE:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case SCROLLSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case POTIONSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case WEAPONSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case FOODSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case RINGSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case WANDSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case TOOLSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case BOOKSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case REAGENTSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case FODDERSHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    case CANDLESHOP:
        res = GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL;
        break;
    default:
        break;
    }

    return res;
}


enum ghsound_types
get_level_ambient_sounds(dlvl)
struct d_level* dlvl;
{
    enum ghsound_types res = GHSOUND_NONE;
    if (!dlvl)
        return res;

    int dnum = dlvl->dnum;


    if (Is_valley(dlvl))
        return GHSOUND_GEHENNOM_VALLEY_AMBIENT;
    else
        return GHSOUND_NONE;

    return res;
}

enum ghsound_types
get_room_ambient_sounds(room)
struct mkroom* room;
{
    enum roomtype_types rtype = room->orig_rtype;
    enum ghsound_types res = get_level_ambient_sounds(&u.uz);

    switch (rtype)
    {
    case OROOM:
        break;
    case COURT:
        break;
    case SWAMP:
        break;
    case VAULT:
        break;
    case BEEHIVE:
        break;
    case DRAGONLAIR:
        break;
    case LIBRARY:
        break;
    case GARDEN:
        res = GHSOUND_GARDEN;
        break;
    case MORGUE:
        res = GHSOUND_MORGUE;
        break;
    case BARRACKS:
        break;
    case ZOO:
        break;
    case DELPHI:
        break;
    case TEMPLE:
        break;
    case LEPREHALL:
        break;
    case COCKNEST:
        break;
    case ANTHOLE:
        break;
    case DESERTEDSHOP:
        res = GHSOUND_MORGUE;
        break;
    case ARMORSHOP:
        break;
    case SHOPBASE:
        break;
    case SCROLLSHOP:
        break;
    case POTIONSHOP:
        break;
    case WEAPONSHOP:
        break;
    case FOODSHOP:
        break;
    case RINGSHOP:
        break;
    case WANDSHOP:
        break;
    case TOOLSHOP:
        break;
    case BOOKSHOP:
        break;
    case REAGENTSHOP:
        break;
    case FODDERSHOP:
        break;
    case CANDLESHOP:
        break;
    default:
        break;
    }

    return res;
}



/* soundset.c */