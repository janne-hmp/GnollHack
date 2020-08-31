/*
 * soundset.h
 * Copyright (c) Janne Gustafsson, 2020
 */
#include "action.h" 

#ifndef SOUNDSET_H
#define SOUNDSET_H

#define BACKGROUND_MUSIC_VOLUME 0.04f
#define SIMPLE_STEALTH_VOLUME 0.08f
#define MAX_HEARING_DISTANCE 10


/*
* GHSOUNDS
*/
/* All GnollHack Sounds */
enum ghsound_types {
	GHSOUND_NONE = 0,
	GHSOUND_MUSIC_SPLASH,
	GHSOUND_MUSIC_PLAYER_SELECTION,
	GHSOUND_MUSIC_INTRO,
	GHSOUND_DUNGEON_NORMAL_MUSIC_NORMAL,
	GHSOUND_DUNGEON_NORMAL_MUSIC_ORACLE,
	GHSOUND_DUNGEON_NORMAL_MUSIC_TEMPLE,
	GHSOUND_DUNGEON_NORMAL_MUSIC_MEDUSA,
	GHSOUND_DUNGEON_NORMAL_MUSIC_CASTLE,
	GHSOUND_GNOMISH_MINES_MUSIC_NORMAL,
	GHSOUND_GNOMISH_MINES_MUSIC_TOWN,
	GHSOUND_SOKOBAN_MUSIC_NORMAL,
	GHSOUND_GEHENNOM_MUSIC_NORMAL,
	GHSOUND_GEHENNOM_MUSIC_VALLEY,
	GHSOUND_GEHENNOM_MUSIC_SANCTUM,
	GHSOUND_MUSIC_GAMEOVER,
	GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_NORMAL,
	GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_SHOPKEEPER_ANGRY,
	GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_SHOPKEEPER_DEAD,
	GHSOUND_DUNGEON_NORMAL_MUSIC_SHOP_DESERTED,
	GHSOUND_FOUNTAIN,
	GHSOUND_BEE,
	GHSOUND_FIRE,
	GHSOUND_STAFF_SWING,
	GHSOUND_STAFF_HIT,
	GHSOUND_GEHENNOM_VALLEY_AMBIENT,
	GHSOUND_MORGUE,
	GHSOUND_GARDEN,
	GHSOUND_UI_BUTTON_DOWN,
	GHSOUND_UI_MENU_SELECT,
	GHSOUND_SFX_QUAFF,
	GHSOUND_POISON_GAS_INSIDE,
	GHSOUND_RAY_ELEC_AMBIENT,
	GHSOUND_RAY_ELEC_CREATE,
	GHSOUND_RAY_ELEC_DESTROY,
	GHSOUND_RAY_ELEC_BOUNCE,
	GHSOUND_BOW_FIRE,
	GHSOUND_ARROW_HIT,
	GHSOUND_SWORD_SWING,
	GHSOUND_SWORD_HIT,
	GHSOUND_AXE_SWING,
	GHSOUND_AXE_HIT,
	GHSOUND_GOBLIN_DEATH,
	GHSOUND_DAGGER_SWING,
	GHSOUND_DAGGER_HIT,
	GHSOUND_HUMAN_BAREHANDED_SWING,
	GHSOUND_HUMAN_BAREHANDED_HIT,
	GHSOUND_HUMAN_KICK_SWING,
	GHSOUND_HUMAN_KICK_HIT,
	GHSOUND_DOOR_WHAM,
	GHSOUND_DOOR_BREAK,
	GHSOUND_DOOR_OPEN,
	GHSOUND_DOOR_CLOSE,
	GHSOUND_DOOR_UNLOCK,
	GHSOUND_DOOR_LOCK,
	GHSOUND_DOOR_RESISTS,
	GHSOUND_DOOR_TRY_LOCKED,
	GHSOUND_CHEST_UNLOCK,
	GHSOUND_CHEST_LOCK,
	GHSOUND_CROSSBOW_FIRE,
	GHSOUND_PICKUP_COINS,
	GHSOUND_DROP_COINS,
	GHSOUND_HORSE_FOOTSTEPS,
	GHSOUND_PUSH_BOULDER,
	GHSOUND_PUSH_EFFORT,
	GHSOUND_CAUSE_PARALYSIS,
	GHSOUND_CAUSE_SLEEPING,
	GHSOUND_MONSTER_DEATH_GENERIC,
	GHSOUND_CHEST_BREAK_LOCK,
	GHSOUND_CHEST_LID_SLAM,
	GHSOUND_GENERIC_ITEM_PICK_UP,
	GHSOUND_GENERIC_ITEM_DROP,
	GHSOUND_GENERIC_ITEM_THROW,
	GHSOUND_FOOTSTEPS_COTTON_SLIPPERS,
	GHSOUND_SWORD_DROP,
	GHSOUND_DAGGER_DROP,
	GHSOUND_GENERIC_FLY,
	GHSOUND_GENERIC_LEVITATE,
	GHSOUND_DISCARD_GENERIC,
	GHSOUND_SPARKS_FLY_GENERIC,
	GHSOUND_EAT_APPLE,
	GHSOUND_EAT_CARROT,
	GHSOUND_PLAYER_FOOTSTEPS_NORMAL,
	GHSOUND_EAT_GENERIC_AMBIENT,
	GHSOUND_EAT_GENERIC_START,
	GHSOUND_EAT_GENERIC_FINISH,
	GHSOUND_EAT_GENERIC_INTERRUPTED,
	GHSOUND_EAT_GENERIC_RESUME,
	GHSOUND_DIGGING_GENERIC_AMBIENT,
	GHSOUND_DIGGING_GENERIC_START,
	GHSOUND_DIGGING_GENERIC_FINISH,
	GHSOUND_DIGGING_GENERIC_INTERRUPTED,
	GHSOUND_DIGGING_GENERIC_RESUME,
	GHSOUND_DIGGING_GROUND_GENERIC_AMBIENT,
	GHSOUND_DIGGING_GROUND_GENERIC_START,
	GHSOUND_DIGGING_GROUND_GENERIC_FINISH,
	GHSOUND_DIGGING_GROUND_GENERIC_INTERRUPTED,
	GHSOUND_DIGGING_GROUND_GENERIC_RESUME,
	GHSOUND_DESECRATE_ALTAR,
	GHSOUND_SUMMON_DEMON,
	GHSOUND_SACRIFICE_PET,
	GHSOUND_SACRIFICE_COALIGNED_UNICORN,
	GHSOUND_ALTAR_ADD_ALIGNMENT,
	GHSOUND_ALTAR_THUNDERCLAP,
	GHSOUND_ALTAR_CHARGED_AIR,
	GHSOUND_ALTAR_ANGRY_ACCEPTS_SACRIFICE,
	GHSOUND_ALTAR_ANGRY_REJECTS_SACRIFICE,
	GHSOUND_ALTAR_POWER_INCREASE,
	GHSOUND_ALTAR_POWER_DECREASE,
	GHSOUND_ALTAR_GOD_MOLLIFIED,
	GHSOUND_ALTAR_INADEQUACY,
	GHSOUND_ALTAR_ABSOLVED,
	GHSOUND_ALTAR_HOPEFUL_FEELING,
	GHSOUND_ALTAR_RECONCILIATION,
	GHSOUND_ALTAR_GIFT,
	GHSOUND_ALTAR_FOUR_LEAF_CLOVER,
	GHSOUND_AURA_GLOW,
	GHSOUND_ALTAR_BURST_OF_FLAME,
	GHSOUND_ALTAR_OFFERING_VANISH,
	GHSOUND_DOG_YELP,
	GHSOUND_DOG_WARN_CURSED,
	GHSOUND_CAT_YELP,
	GHSOUND_CAT_WARN_CURSED,
	GHSOUND_GENERIC_YELP,
	GHSOUND_GENERIC_WARN_CURSED,
	GHSOUND_GENERIC_PRAY,
	GHSOUND_PRAY_FAIL,
	GHSOUND_PRAY_PLEASED,
	GHSOUND_PRAY_FIX_ALL_TROUBLES,
	GHSOUND_PRAY_FIX_ONE_TROUBLE,
	GHSOUND_PRAY_REPAIR,
	GHSOUND_PRAY_HEALING,
	GHSOUND_PRAY_UNCURSE,
	GHSOUND_PRAY_GIFT,
	GHSOUND_PRAY_CROWNING,
	GHSOUND_PRAY_SPELLBOOK,
	GHSOUND_PRAY_BLESS_WATER,
	GHSOUND_PRAY_CURSE_WATER,
	GHSOUND_PRAY_SHIMMERING_LIGHT,
	GHSOUND_CAMERA_FLASH_AMBIENT,
	GHSOUND_CAMERA_FLASH_CREATE,
	GHSOUND_QUEST_MUSIC_NORMAL,
	GHSOUND_VLAD_TOWER_MUSIC_NORMAL,
	GHSOUND_MODRON_MUSIC_NORMAL,
	GHSOUND_BOVINE_MUSIC_NORMAL,
	GHSOUND_ENDGAME_MUSIC_NORMAL,
	GHSOUND_ENDGAME_MUSIC_EARTH,
	GHSOUND_ENDGAME_MUSIC_AIR,
	GHSOUND_ENDGAME_MUSIC_WATER,
	GHSOUND_ENDGAME_MUSIC_FIRE,
	GHSOUND_ENDGAME_MUSIC_ASTRAL,
	GHSOUND_MUSIC_VICTORY,
	GHSOUND_ENCHANT_ITEM_SUCCESS,
	GHSOUND_ENCHANT_ITEM_BLESSED_SUCCESS,
	GHSOUND_ENCHANT_ITEM_UNCURSE_AND_OTHER,
	GHSOUND_ENCHANT_ITEM_NEGATIVE,
	GHSOUND_ENCHANT_ITEM_VIOLENT_GLOW,
	GHSOUND_ENCHANT_ITEM_VIBRATE_WARNING,
	GHSOUND_ENCHANT_ITEM_VIBRATE_AND_DESTROY,
	GHSOUND_ENCHANT_ITEM_SPECIAL_SUCCESS,
	GHSOUND_ENCHANT_ITEM_SPECIAL_NEGATIVE,
	GHSOUND_ENCHANT_ITEM_GENERAL_FAIL,
	GHSOUND_HANDS_ITCH,
	GHSOUND_PROTECT_ITEM_SUCCESS,
	GHSOUND_PROTECT_ITEM_FAIL,
	GHSOUND_REPAIR_ITEM_SUCCESS,
	GHSOUND_REPAIR_ITEM_FAIL,
	GHSOUND_BLESS_ITEM_SUCCESS,
	GHSOUND_UNCURSE_ITEM_SUCCESS,
	GHSOUND_CURSE_ITEM_SUCCESS,
	GHSOUND_UNBLESS_ITEM_SUCCESS,
	GHSOUND_DUNGEON_NORMAL_MUSIC_SMITH_NORMAL,
	GHSOUND_DISINTEGRATE,
	GHSOUND_LIFE_SAVED,
	GHSOUND_HEALING,
	GHSOUND_FULL_HEALING,
	GHSOUND_CURE_DISEASE,
	GHSOUND_CURE_AILMENT,
	GHSOUND_GAIN_ENERGY,
	GHSOUND_FULL_ENERGY,
	GHSOUND_GAIN_LEVEL,
	GHSOUND_GAIN_SKILL,
	GHSOUND_LOSE_LEVEL,
	GHSOUND_LOSE_ENERGY,
	GHSOUND_MUFFLED_SHATTER,
	GHSOUND_MUFFLED_CRACKING,
	GHSOUND_MUFFLED_CRASH,
	GHSOUND_MUFFLED_SPLAT,
	GHSOUND_SKILL_ADVANCED,
	GHSOUND_GENERIC_CAST,
	GHSOUND_DEAF_AMBIENT,
	GHSOUND_UNDERWATER_AMBIENT,
	GHSOUND_DUST_VORTEX_SWALLOW_AMBIENT,
	GHSOUND_RAY_FIRE_AMBIENT,
	GHSOUND_RAY_FIRE_CREATE,
	GHSOUND_RAY_FIRE_DESTROY,
	GHSOUND_RAY_FIRE_BOUNCE,
	GHSOUND_RAY_COLD_AMBIENT,
	GHSOUND_RAY_COLD_CREATE,
	GHSOUND_RAY_COLD_DESTROY,
	GHSOUND_RAY_COLD_BOUNCE,
	GHSOUND_RAY_DEATH_AMBIENT,
	GHSOUND_RAY_DEATH_CREATE,
	GHSOUND_RAY_DEATH_DESTROY,
	GHSOUND_RAY_DEATH_BOUNCE,
	GHSOUND_GENERAL_SPELL_AMBIENT,
	GHSOUND_GENERAL_SPELL_CREATE,
	MAX_GHSOUNDS
};


/* 
 * SOUND SOURCES ETC.
 */
/* Mobile ambient sound sources */
enum soundsource_types {
	SOUNDSOURCE_OBJECT = 0,
	SOUNDSOURCE_MONSTER,
	SOUNDSOURCE_LOCATION,
	SOUNDSOURCE_REGION,
	MAX_SOUNDSOURCE_TYPES
};

enum soundsource_ambient_subtypes {
	SOUNDSOURCE_AMBIENT_GENERAL = 0,
	SOUNDSOURCE_AMBIENT_LIT,
	SOUNDSOURCE_AMBIENT_INSIDE_REGION,
	MAX_SOUNDSOURCE_AMBIENT_SUBTYPES
};


/* UI sound types */
enum ui_sound_types {
	UI_SOUND_ILLEGAL = 0,
	UI_SOUND_BUTTON_DOWN,
	UI_SOUND_MENU_SELECT,
	UI_SOUND_TICK_BOX,
	UI_SOUND_UNTICK_BOX,
	UI_SOUND_OPEN_WINDOW,
	MAX_UI_SOUND_TYPES
};

/* General effect sound types */
enum sfx_sound_types {
	SFX_ILLEGAL = 0,
	SFX_READ,
	SFX_QUAFF,
	SFX_OPEN_DOOR,
	SFX_CLOSE_DOOR,
	SFX_UNLOCK_DOOR,
	SFX_LOCK_DOOR,
	SFX_DOOR_RESISTS,
	SFX_DOOR_TRY_LOCKED,
	SFX_RESERVED_1,
	SFX_RESERVED_2,
	SFX_RESERVED_3,
	SFX_RESERVED_4,
	SFX_SHIELD_EFFECT,
	SFX_TALK_EFFECT,
	SFX_PUSH_BOULDER,
	SFX_CAUSE_PARALYSIS,
	SFX_CAUSE_SLEEPING,
//	SFX_CAUSE_STUN,
//	SFX_CAUSE_BLINDNESS,
//	SFX_CAUSE_TERMINAL_ILLNESS,
//	SFX_CAUSE_FOOD_POISONING,
//	SFX_CAUSE_MUMMY_ROT,
//	SFX_CAUSE_PETRIFICATION,
	SFX_DESECRATE_ALTAR,
	SFX_SUMMON_DEMON,
	SFX_SACRIFICE_PET,
	SFX_SACRIFICE_COALIGNED_UNICORN,
	SFX_ALTAR_ADD_ALIGNMENT,
	SFX_ALTAR_THUNDERCLAP,
	SFX_ALTAR_CHARGED_AIR,
	SFX_ALTAR_ANGRY_ACCEPTS_SACRIFICE,
	SFX_ALTAR_ANGRY_REJECTS_SACRIFICE,
	SFX_ALTAR_POWER_INCREASE,
	SFX_ALTAR_POWER_DECREASE,
	SFX_ALTAR_GOD_MOLLIFIED,
	SFX_ALTAR_INADEQUACY,
	SFX_ALTAR_ABSOLVED,
	SFX_ALTAR_HOPEFUL_FEELING,
	SFX_ALTAR_RECONCILIATION,
	SFX_ALTAR_GIFT,
	SFX_ALTAR_FOUR_LEAF_CLOVER,
	SFX_AURA_GLOW,
	SFX_ALTAR_BURST_OF_FLAME,
	SFX_ALTAR_OFFERING_VANISH,
	SFX_PRAY_FAIL,
	SFX_PRAY_PLEASED,
	SFX_PRAY_FIX_ALL_TROUBLES,
	SFX_PRAY_FIX_ONE_TROUBLE,
	SFX_PRAY_REPAIR,
	SFX_PRAY_HEALING,
	SFX_PRAY_UNCURSE,
	SFX_PRAY_GIFT,
	SFX_PRAY_CROWNING,
	SFX_PRAY_SPELLBOOK,
	SFX_PRAY_BLESS_WATER,
	SFX_PRAY_CURSE_WATER,
	SFX_PRAY_SHIMMERING_LIGHT,
	SFX_ENCHANT_ITEM_SUCCESS,
	SFX_ENCHANT_ITEM_BLESSED_SUCCESS,
	SFX_ENCHANT_ITEM_UNCURSE_AND_OTHER,
	SFX_ENCHANT_ITEM_NEGATIVE,
	SFX_ENCHANT_ITEM_VIOLENT_GLOW,
	SFX_ENCHANT_ITEM_VIBRATE_WARNING,
	SFX_ENCHANT_ITEM_VIBRATE_AND_DESTROY,
	SFX_ENCHANT_ITEM_SPECIAL_SUCCESS,
	SFX_ENCHANT_ITEM_SPECIAL_NEGATIVE,
	SFX_ENCHANT_ITEM_GENERAL_FAIL,
	SFX_HANDS_ITCH,
	SFX_PROTECT_ITEM_SUCCESS,
	SFX_PROTECT_ITEM_FAIL,
	SFX_REPAIR_ITEM_SUCCESS,
	SFX_REPAIR_ITEM_FAIL,
	SFX_BLESS_ITEM_SUCCESS,
	SFX_UNCURSE_ITEM_SUCCESS,
	SFX_CURSE_ITEM_SUCCESS,
	SFX_UNBLESS_ITEM_SUCCESS,
	SFX_DISINTEGRATE,
	SFX_LIFE_SAVED,
	SFX_HEALING,
	SFX_FULL_HEALING,
	SFX_CURE_DISEASE,
	SFX_CURE_AILMENT,
	SFX_GAIN_ENERGY,
	SFX_FULL_ENERGY,
	SFX_GAIN_LEVEL,
	SFX_GAIN_SKILL,
	SFX_LOSE_LEVEL,
	SFX_LOSE_ENERGY,
	SFX_MUFFLED_SHATTER,
	SFX_MUFFLED_CRACKING,
	SFX_MUFFLED_CRASH,
	SFX_MUFFLED_SPLAT,
	SFX_SKILL_ADVANCED,
	MAX_SFX_SOUND_TYPES
};


/* 
 * SOUND WINDOWPROC FUNCTION DEFINITIONS 
 */
#define MAX_SOUND_PARAMETERS 8
enum immediate_sound_types {
	IMMEDIATE_SOUND_SFX = 0,
	IMMEDIATE_SOUND_UI = 1
};

/* Used play_ghsound function input structs */
struct ghsound_immediate_info {
	enum ghsound_types ghsound;
	const char* parameter_names[MAX_SOUND_PARAMETERS];
	float parameter_values[MAX_SOUND_PARAMETERS];
	float volume;
	enum immediate_sound_types sound_type;
};

struct ghsound_music_info {
	enum ghsound_types ghsound;
	float volume;
	boolean stop_music;
};

struct ghsound_level_ambient_info {
	enum ghsound_types ghsound;
	float volume;
};

struct ghsound_environment_ambient_info {
	enum ghsound_types ghsound;
	float volume;
};

struct ghsound_effect_ambient_info {
	enum ghsound_types ghsound;
	float volume;
};

struct effect_ambient_volume_info {
	float volume;
};

struct stop_all_info {
	unsigned char stop_flags;
};

struct ghsound_occupation_ambient_info {
	enum ghsound_types ghsound;
	enum obj_material_types strike_surface;
	float volume;
};


/* 
 * SOUNDSET DEFINITIONS 
 */
/* General sound information */
struct ghsound_info {
	enum ghsound_types ghsound;
	float volume;
};


/* OCCUPATION SOUNDSETS */
enum occupation_sound_types {
	OCCUPATION_SOUND_TYPE_AMBIENT = 0,				/* Ambient sound while being occupied */
	OCCUPATION_SOUND_TYPE_START,					/* Sound upon starting the occupation */
	OCCUPATION_SOUND_TYPE_FINISH,					/* Sound upon finally finishing the occupation */
	OCCUPATION_SOUND_TYPE_INTERRUPTED,				/* Sound upon the occupation being interrupted */
	OCCUPATION_SOUND_TYPE_RESUME,					/* Sound upon resuming the occupation */
	MAX_OCCUPATION_SOUND_TYPES
};

struct occupation_soundset_definition {
	const char* soundset_name;
	struct ghsound_info sounds[MAX_OCCUPATION_SOUND_TYPES];
};

enum occupation_soundset_types {
	OCCUPATION_SOUNDSET_NONE = 0,			
	OCCUPATION_SOUNDSET_GENERIC_EATING,		
	OCCUPATION_SOUNDSET_GENERIC_OPENING_TIN,
	OCCUPATION_SOUNDSET_GENERIC_DIGGING_ROCK,
	OCCUPATION_SOUNDSET_GENERIC_SETTING_TRAP,
	OCCUPATION_SOUNDSET_GENERIC_WIPING_OFF,
	OCCUPATION_SOUNDSET_GENERIC_TAKING_OFF,
	OCCUPATION_SOUNDSET_GENERIC_PICKING_LOCK,
	OCCUPATION_SOUNDSET_GENERIC_FORCING_LOCK,
	OCCUPATION_SOUNDSET_GENERIC_STUDYING,
	OCCUPATION_SOUNDSET_GENERIC_DIGGING_GROUND,
	OCCUPATION_SOUNDSET_GENERIC_SWINGING,
	OCCUPATION_SOUNDSET_GENERIC_HITTING_DOOR,
	OCCUPATION_SOUNDSET_GENERIC_CUTTING_TREE,
	MAX_OCCUPATION_SOUNDSETS
};

extern struct occupation_soundset_definition occupation_soundset_definitions[MAX_OCCUPATION_SOUNDSETS];

/* RAY SOUNDSETS */
enum ray_sound_types {
	RAY_SOUND_TYPE_AMBIENT = 0,				/* Ambient sound while the ray is travelling */
	RAY_SOUND_TYPE_CREATE,					/* Sound upon creation of the ray */
	RAY_SOUND_TYPE_DESTROY,					/* Sound upon dissipation of the ray */
	RAY_SOUND_TYPE_BOUNCE,					/* Sound upon the ray bouncing */
	RAY_SOUND_TYPE_WHIZZES_BY_YOU,			/* Sound upon the ray wheezing by you */
	RAY_SOUND_TYPE_HIT_MONSTER,				/* Sound upon the ray hitting a monster */
	RAY_SOUND_TYPE_HIT_OBJECT,				/* Sound upon the ray hitting an object */
	RAY_SOUND_TYPE_HIT_LOCATION,			/* Sound upon the ray hitting a location, e.g. digging a wall */
	MAX_RAY_SOUND_TYPES
};

struct ray_soundset_definition {
	const char* soundset_name;
	struct ghsound_info sounds[MAX_RAY_SOUND_TYPES];
};

enum object_ray_soundset_types {
	/* 0...NRAYS-1 normal ray, NRAYS...2NRAYS-1 explosion */
	OBJECT_RAY_SOUNDSET_NONE = 2 * NRAYS,
	OBJECT_RAY_SOUNDSET_GENERIC,
	OBJECT_RAY_SOUNDSET_CAMERA,
	MAX_RAY_SOUNDSETS
};

extern struct ray_soundset_definition ray_soundsets[MAX_RAY_SOUNDSETS];

/* OBJECT SOUNDSETS */
enum object_sound_types {
	OBJECT_SOUND_TYPE_AMBIENT = 0,		/* Ambient sound that is typically either continuously played or played when the object is lit */

	/* Combat */
	OBJECT_SOUND_TYPE_SWING_MELEE,		/* Played at the start of the attack animation */
	OBJECT_SOUND_TYPE_HIT_MELEE,		/* Played at the end of the melee attack animation and at the end of throw if the attack hits target */
	OBJECT_SOUND_TYPE_THROW,			/* Played when the object is thrown */
	OBJECT_SOUND_TYPE_FIRE,				/* Played when a launcher is fired */
	OBJECT_SOUND_TYPE_HIT_THROW,		/* Played at the end of the throw/fire attack animation and at the end of throw if the attack hits target */

	/* Handling */
	OBJECT_SOUND_TYPE_PICK_UP,
	OBJECT_SOUND_TYPE_DROP,
	OBJECT_SOUND_TYPE_OPEN_CONTAINER,
	OBJECT_SOUND_TYPE_CLOSE_CONTAINER,
	OBJECT_SOUND_TYPE_UNLOCK_CONTAINER,
	OBJECT_SOUND_TYPE_LOCK_CONTAINER,
	OBJECT_SOUND_TYPE_BREAK_LOCK_CONTAINER,
	OBJECT_SOUND_TYPE_LID_SLAM_CONTAINER,
	OBJECT_SOUND_TYPE_WEAR,
	OBJECT_SOUND_TYPE_TAKEOFF,
	OBJECT_SOUND_TYPE_WIELD,
	OBJECT_SOUND_TYPE_UNWIELD,
	OBJECT_SOUND_TYPE_QUIVER,
	OBJECT_SOUND_TYPE_UNQUIVER,
	OBJECT_SOUND_TYPE_APPLY,
	OBJECT_SOUND_TYPE_INVOKE,
	OBJECT_SOUND_TYPE_QUAFF,
	OBJECT_SOUND_TYPE_READ,

	/* General */
	OBJECT_SOUND_TYPE_BREAK,			/* Sound upon breaking the item */
	OBJECT_SOUND_TYPE_DISCARD,			/* Sound upon discarding a tin */
	OBJECT_SOUND_TYPE_SPARKS_FLY,		/* Sound when you hit and sparks fly */
	OBJECT_SOUND_TYPE_PUSH,				/* Sound upon pushing the item on the ground */

	/* Movement */
	OBJECT_SOUND_TYPE_WALK,				/* Walk sound if used as boots */
	OBJECT_SOUND_TYPE_FLY,				/* Fly if attached to back */
	OBJECT_SOUND_TYPE_LEVITATION,		/* Levitate when worn / used */
	OBJECT_SOUND_TYPE_SWIM,				/* Swim when worn */

	MAX_OBJECT_SOUND_TYPES
};

enum object_occupation_types {
	OCCUPATION_NONE = 0,
	OCCUPATION_EATING,
	OCCUPATION_OPENING_TIN,
	OCCUPATION_DIGGING_ROCK,
	OCCUPATION_SETTING_TRAP,
	OCCUPATION_WIPING_OFF,
	OCCUPATION_TAKING_OFF,
	OCCUPATION_PICKING_LOCK,
	OCCUPATION_FORCING_LOCK,
	OCCUPATION_STUDYING,
	OCCUPATION_DIGGING_GROUND,
	OCCUPATION_SWINGING,
	OCCUPATION_HITTING_DOOR,
	OCCUPATION_CUTTING_TREE,
	MAX_OBJECT_OCCUPATION_TYPES
};

struct object_soundset_definition {
	const char* soundset_name;
	struct ghsound_info sounds[MAX_OBJECT_SOUND_TYPES];
	enum soundsource_ambient_subtypes ambient_subtype;
	enum occupation_soundset_types occupation_soundsets[MAX_OBJECT_OCCUPATION_TYPES];
	enum object_ray_soundset_types ray_soundset;
};

enum object_soundset_types {
	OBJECT_SOUNDSET_NONE = 0,
	OBJECT_SOUNDSET_GENERAL,
	OBJECT_SOUNDSET_CANDLE,
	OBJECT_SOUNDSET_QUARTERSTAFF,
	OBJECT_SOUNDSET_BOW,
	OBJECT_SOUNDSET_CROSSBOW,
	OBJECT_SOUNDSET_ARROW,
	OBJECT_SOUNDSET_SWORD,
	OBJECT_SOUNDSET_AXE,
	OBJECT_SOUNDSET_DAGGER,
	OBJECT_SOUNDSET_BONE_DAGGER,
	OBJECT_SOUNDSET_HUMAN_BAREHANDED,
	OBJECT_SOUNDSET_HUMAN_BAREFOOTED,
	OBJECT_SOUNDSET_HORSE_BAREFOOTED,
	OBJECT_SOUNDSET_COTTON_SLIPPERS,
	OBJECT_SOUNDSET_CHEST,
	OBJECT_SOUNDSET_COIN,
	OBJECT_SOUNDSET_BOULDER,
	OBJECT_SOUNDSET_CAMERA,
	MAX_OBJECT_SOUNDSETS
};

extern struct object_soundset_definition object_soundsets[MAX_OBJECT_SOUNDSETS];


/* MONSTER SOUNDSETS */
enum monster_sound_types {
	MONSTER_SOUND_TYPE_AMBIENT = 0,
	MONSTER_SOUND_TYPE_SWALLOW_AMBIENT,
	MONSTER_SOUND_TYPE_COUGH,
	MONSTER_SOUND_TYPE_PUSH_EFFORT,
	MONSTER_SOUND_TYPE_YELP,
	MONSTER_SOUND_TYPE_WARN_CURSED,
	MONSTER_SOUND_TYPE_OUCH,
	MONSTER_SOUND_TYPE_SEARCH,
	MONSTER_SOUND_TYPE_CAST,
	MONSTER_SOUND_TYPE_PRAY,
	MONSTER_SOUND_TYPE_DEATH,
	MAX_MONSTER_SOUND_TYPES
};

struct monster_soundset_definition {
	const char* soundset_name;

	/* General */
	struct ghsound_info sounds[MAX_MONSTER_SOUND_TYPES];
	enum soundsource_ambient_subtypes ambient_subtype;

	/* Attacks */
	enum object_soundset_types attack_soundsets[NATTK + 1]; /* attack # NATTK is used for hit sounds in kick command and for walking sound */

#define BAREHANDED_ATTACK_NUMBER 0
#define BAREFOOTED_ATTACK_NUMBER NATTK
};

enum monster_soundset_types {
	MONSTER_SOUNDSET_NONE = 0,
	MONSTER_SOUNDSET_GENERAL,
	MONSTER_SOUNDSET_GOBLIN,
	MONSTER_SOUNDSET_DRAGON,
	MONSTER_SOUNDSET_DRACOLICH,
	MONSTER_SOUNDSET_BEE,
	MONSTER_SOUNDSET_HORSE,
	MONSTER_SOUNDSET_DOG,
	MONSTER_SOUNDSET_CAT,
	MAX_MONSTER_SOUNDSETS
};

extern struct monster_soundset_definition monster_soundsets[MAX_MONSTER_SOUNDSETS];



/* PLAYER SOUNDSETS */
enum player_attack_soundset_types {
	PLAYER_ATTACK_SOUNDSET_BAREHANDED = 0,
	PLAYER_ATTACK_SOUNDSET_BAREFOOTED,
	MAX_PLAYER_ATTACK_SOUNDSETS
};

struct player_soundset_definition {
	const char* soundset_name;

	/* Overrides monster-specific values */
	struct ghsound_info sounds[MAX_MONSTER_SOUND_TYPES];
	enum soundsource_ambient_subtypes ambient_subtype;

	/* Bare-handed and kick soundsets */
	enum object_soundset_types attack_soundsets[MAX_PLAYER_ATTACK_SOUNDSETS];
};

enum player_soundset_types {
	PLAYER_SOUNDSET_NONE = 0,
	PLAYER_SOUNDSET_GENERAL,
	PLAYER_SOUNDSET_FEMALE_ELVEN_WIZARD,
	MAX_PLAYER_SOUNDSETS
};

extern struct player_soundset_definition player_soundsets[MAX_PLAYER_SOUNDSETS];



/* LOCATION SOUNDSETS */
enum location_sound_types {
	LOCATION_SOUND_TYPE_AMBIENT = 0,	/* Ambient sound that is typically either continuously played or played when the location is lit */
	LOCATION_SOUND_TYPE_WHAM,			/* Sound upon kicking the location */
	LOCATION_SOUND_TYPE_BREAK,			/* Sound upon breaking the location */
	MAX_LOCATION_SOUND_TYPES
};

struct location_soundset_definition {
	const char* soundset_name;
	struct ghsound_info sounds[MAX_LOCATION_SOUND_TYPES];
	enum soundsource_ambient_subtypes ambient_subtype;
};

enum location_soundset_types {
	LOCATION_SOUNDSET_NONE = 0,
	LOCATION_SOUNDSET_GENERAL,
	LOCATION_SOUNDSET_FOUNTAIN,
	LOCATION_SOUNDSET_ALTAR,
	LOCATION_SOUNDSET_DOOR,
	MAX_LOCATION_SOUNDSETS
};

extern struct location_soundset_definition location_soundsets[MAX_LOCATION_SOUNDSETS];


/* REGION SOUNDSETS */
enum region_sound_types {
	REGION_SOUND_TYPE_AMBIENT = 0,				/* Player inside or outside of the region, based on the square with best hearing value: Ambient sound that is typically either continuously played or played when the region is lit */
	REGION_SOUND_TYPE_CREATE,					/* Sound upon creation of the region */
	REGION_SOUND_TYPE_DESTROY,					/* Sound upon destruction of the region */
	MAX_REGION_SOUND_TYPES
};

struct region_soundset_definition {
	const char* soundset_name;
	struct ghsound_info sounds[MAX_REGION_SOUND_TYPES];
	enum soundsource_ambient_subtypes ambient_subtype;
};

enum region_soundset_types {
	REGION_SOUNDSET_NONE = 0,
	REGION_SOUNDSET_POISON_GAS,
	MAX_REGION_SOUNDSETS
};

extern struct region_soundset_definition region_soundsets[MAX_REGION_SOUNDSETS];

/* EFFECT SOUNDS */
struct effect_sound_definition {
	const char* sound_name;
	struct ghsound_info sound;
	boolean affected_by_deafness;
};

extern struct effect_sound_definition ui_sounds[MAX_UI_SOUND_TYPES];
extern struct effect_sound_definition sfx_sounds[MAX_SFX_SOUND_TYPES];



#endif /* SOUNDSET_H */