/*
 * soundfx.cpp
 * GnollHack for Windows Sound Effects Using FMOD
 *
 * Copyright (c) Janne Gustafsson, 2020
 *
 */

#include <windows.h>
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "resource.h"
#include "soundset.h"

using namespace FMOD;
static System* fmod_core_system = (System*)0;
static Studio::System* fmod_studio_system = (Studio::System*)0;

struct GNHSoundInstance {
    Studio::EventInstance* eventInstance;
    enum ghsound_types ghsound;
    float normalVolume;
    int sound_type;
    struct GNHSoundInstance* next_instance;
};

#define NUM_IMMEDIATE_SOUND_INSTANCES 16

static GNHSoundInstance musicInstances[2] = { 0 };
static GNHSoundInstance levelAmbientInstances[2] = { 0 };
static GNHSoundInstance occupationAmbientInstances[2] = { 0 };
static GNHSoundInstance effectAmbientInstances[2] = { 0 };
static GNHSoundInstance environmentAmbientInstances[2] = { 0 };
static GNHSoundInstance immediateSoundInstances[NUM_IMMEDIATE_SOUND_INSTANCES] = { 0 };
static GNHSoundInstance* ambient_base = NULL; /* for sound source ambients */

static float general_volume = 1.0f;
static float general_music_volume = 1.0f;
static float general_ambient_volume = 1.0f;
static float general_sfx_volume = 1.0f;
static float general_ui_volume = 1.0f;

/* GHSound -> FMOD event mapping here */
enum sound_bank_types {
    SOUND_BANK_NONE = 0,
    SOUND_BANK_MASTER,
    MAX_SOUND_BANKS
};

struct sound_bank_definition {
    boolean isresource;
    int resource_id;
    const char* filename;
};


struct ghsound_eventmapping {
    enum sound_bank_types bank_id;
    const char* eventPath;
    float volume;
};


const struct ghsound_eventmapping ghsound2event[MAX_GHSOUNDS] = {
    { SOUND_BANK_NONE, "", 0.0f },

    { SOUND_BANK_MASTER, "event:/Music/Music-Splash", 0.5f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Player-Selection", 0.3f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Intro", 0.1f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Normal-1", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Music-Oracle", 0.20f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Temple", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Music-Medusa", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Music-Castle", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Music-Gnomish-Mines-Normal", 0.07f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Gnomish-Mines-Town", 0.10f},
    
    { SOUND_BANK_MASTER, "event:/Music/Music-Sokoban-Normal", 0.10f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Gehennom-Normal", 0.12f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Gehennom-Valley", 0.08f},
    { SOUND_BANK_MASTER, "event:/Music/Music-Gehennom-Sanctum", 0.08f},
    { SOUND_BANK_MASTER, "event:/Music/Game Over", 0.1f},
    { SOUND_BANK_MASTER, "event:/Music/Shop/Normal", 0.07f},
    { SOUND_BANK_MASTER, "event:/Music/Shop/Angry", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Shop/Dead", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Music/Shop/Deserted", BACKGROUND_MUSIC_VOLUME},
    { SOUND_BANK_MASTER, "event:/Location/Fountain/Fountain Ambient", 0.75f},

    { SOUND_BANK_MASTER, "event:/Monster/Bee/Bee Ambient", 1.0f},
    { SOUND_BANK_MASTER, "event:/Location/Altar/Fire Ambient", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Quarterstaff/Quarterstaff Swing", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Quarterstaff/Quarterstaff Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Ambience/Level/Valley Ambient", 0.10f},
    { SOUND_BANK_MASTER, "event:/Ambience/Room/Morgue Ambient", 0.10f},
    { SOUND_BANK_MASTER, "event:/Ambience/Room/Garden Ambient", 0.03f},
    { SOUND_BANK_MASTER, "event:/UI/Button Down", 0.15f},
    { SOUND_BANK_MASTER, "event:/UI/Menu Select", 0.15f},
    { SOUND_BANK_MASTER, "event:/SFX/Action/Quaff", 1.0f},

    { SOUND_BANK_MASTER, "event:/Location/Poison Gas/Poison Gas", 1.0f},
    { SOUND_BANK_MASTER, "event:/Ray/Electricity/Ray Electricity Ambient", 1.0f},
    { SOUND_BANK_MASTER, "event:/Ray/Electricity/Ray Electricity Create", 1.0f},
    { SOUND_BANK_MASTER, "event:/Ray/Electricity/Ray Electricity Destroy", 1.0f},
    { SOUND_BANK_MASTER, "event:/Ray/Electricity/Ray Electricity Bounce", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Bow/Bow Fire", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Arrow/Arrow Hit", 4.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Sword/Sword Swing", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Sword/Sword Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Axe/Axe Swing", 1.0f},
    
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Axe/Axe Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Goblin/Goblin Death", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Dagger/Dagger Swing", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Dagger/Dagger Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Barehanded/Human Barehanded Swing", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Barehanded/Human Barehanded Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Barefooted/Human Kick Swing", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Barefooted/Human Kick Hit", 1.0f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Wham", 1.0f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Break", 1.0f},

    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Open", 0.1f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Close", 0.5f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Unlock", 0.5f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Lock", 0.5f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Resists", 0.5f},
    { SOUND_BANK_MASTER, "event:/Location/Door/Generic/Door Try Locked", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Unlock", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Lock", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Crossbow/Crossbow Fire", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/General/Coin/Coins Pick Up", 1.0f},

    { SOUND_BANK_MASTER, "event:/Object/General/Coin/Coins Drop", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Horse/Barefooted/Horse Footsteps", 0.3f},
    { SOUND_BANK_MASTER, "event:/Object/General/Boulder/Push Boulder", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Male/Push Effort", 0.25f},
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Cause Paralysis", 1.0f},
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Cause Sleeping", 1.0f},
    { SOUND_BANK_MASTER, "event:/Monster/Generic/Generic Monster Death", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Lock Break", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Lid Slam", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Item Pick Up", 0.3f},

    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Item Drop", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Item Throw", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/Armor/Cotton Slippers/Cotton Slipper Footsteps", 0.1f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Sword/Sword Drop", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Weapon/Dagger/Dagger Drop", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Fly", 0.4f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Levitate", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Discard", 0.5f},
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Sparks Fly", 1.0f},
    { SOUND_BANK_MASTER, "event:/Object/Food/Apple/Apple Eat", 0.5f},

    { SOUND_BANK_MASTER, "event:/Object/Food/Carrot/Carrot Eat", 0.5f},
    { SOUND_BANK_MASTER, "event:/Monster/Human/Barefooted/Player Footsteps", 1.0f},
    { SOUND_BANK_MASTER, "event:/Occupation/Eating/Eating Ambient", 0.4f},
    { SOUND_BANK_MASTER, "event:/Occupation/Eating/Eating Start", 0.4f},
    { SOUND_BANK_MASTER, "event:/Occupation/Eating/Eating Finish", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Eating/Eating Interrupted", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Eating/Eating Resume", 0.4f},
    { SOUND_BANK_MASTER, "event:/Occupation/Digging/Digging Ambient", 0.3f },
    { SOUND_BANK_MASTER, "event:/Occupation/Digging/Digging Start", 0.3f },
    { SOUND_BANK_MASTER, "event:/Occupation/Digging/Digging Finish", 0.3f },

    { SOUND_BANK_MASTER, "event:/Occupation/Digging/Digging Interrupted", 0.3f },
    { SOUND_BANK_MASTER, "event:/Occupation/Digging/Digging Resume", 0.3f },
    { SOUND_BANK_MASTER, "event:/Occupation/Digging Ground/Digging Ground Ambient", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Digging Ground/Digging Ground Start", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Digging Ground/Digging Ground Finish", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Digging Ground/Digging Ground Interrupted", 0.3f},
    { SOUND_BANK_MASTER, "event:/Occupation/Digging Ground/Digging Ground Resume", 0.3f},
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Desecrate Altar", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Miscellaneous/Summon Demon", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Sacrifice Pet", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Altar/Sacrifice Coaligned Unicorn", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Add Alignment", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Environment/Thunderclap", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Environment/Charged Air", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Accepts Sacrifice", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Rejects Sacrifice", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Power Increase", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Power Decrease", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/God Mollified", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Inadequacy", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Altar/Absolved", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Hopeful Feeling", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Reconciliation", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Gift", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Four-Leaf Clover", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Aura Glow", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Offering Burst of Flame", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Altar/Offering Vanish", 0.3f },
    { SOUND_BANK_MASTER, "event:/Monster/Dog/Dog Yelp", 0.7f },
    { SOUND_BANK_MASTER, "event:/Monster/Dog/Dog Warn Cursed", 0.3f },

    { SOUND_BANK_MASTER, "event:/Monster/Cat/Cat Yelp", 0.7f },
    { SOUND_BANK_MASTER, "event:/Monster/Cat/Cat Warn Cursed", 0.3f },
    { SOUND_BANK_MASTER, "event:/Monster/Generic/Generic Yelp", 0.3f },
    { SOUND_BANK_MASTER, "event:/Monster/Generic/Generic Warn Cursed", 0.3f },
    { SOUND_BANK_MASTER, "event:/Monster/Generic/Generic Pray", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Fail", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Pleased", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Fix All Troubles", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Fix One Trouble", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Repair", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Pray/Healing", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Uncurse", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Gift", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Crowning", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Spellbook", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Bless Water", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Curse Water", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Pray/Shimmering Light", 0.3f },
    { SOUND_BANK_MASTER, "event:/Ray/Camera Flash/Camera Flash Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Camera Flash/Camera Flash Create", 1.0f },

    { SOUND_BANK_MASTER, "event:/Music/Quest-Normal", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/Vlad-Tower-Normal", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/Modron-Normal", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/Bovine-Normal", 0.06f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Normal", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Earth", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Air", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Water", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Fire", 0.08f },
    { SOUND_BANK_MASTER, "event:/Music/End Game/Astral", 0.08f },

    { SOUND_BANK_MASTER, "event:/Music/Victory", 0.20f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Blessed Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Uncurse and Other", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Negative", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Violent Glow", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Vibrate Warning", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Vibrate and Destroy", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Special Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/Special Negative", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Spell/Enchant Item/General Fail", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Hands Itch", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Protect Item/Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Protect Item/Fail", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Repair Item/Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Repair Item/Fail", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Bless Item/Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Bless Item/Uncurse", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Curse Item/Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Curse Item/Unbless", 0.3f },

    { SOUND_BANK_MASTER, "event:/Music/Smith/Normal", 0.08f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Disintegrate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Life Saved", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Healing/Healing", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Healing/Full Healing", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Healing/Cure Disease", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Healing/Cure Ailment", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Mana/Gain Energy", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Mana/Full Energy", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Gain Level", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/General/Gain Skill", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Lose Level", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Mana/Lose Energy", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Muffled Shatter", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Muffled Cracking", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Muffled Crash", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Muffled Splat", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Skill Advanced", 0.3f },
    { SOUND_BANK_MASTER, "event:/Monster/Generic/Generic Cast", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ambience/Environment/Deaf", 0.4f },

    { SOUND_BANK_MASTER, "event:/Ambience/Environment/Underwater", 1.0f },
    { SOUND_BANK_MASTER, "event:/Monster/Dust Vortex/Swallow Ambient", 0.5f },
    { SOUND_BANK_MASTER, "event:/Ray/Fire/Ray Fire Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Fire/Ray Fire Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Fire/Ray Fire Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Fire/Ray Fire Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Cold/Ray Cold Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Cold/Ray Cold Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Cold/Ray Cold Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Cold/Ray Cold Bounce", 1.0f },

    { SOUND_BANK_MASTER, "event:/Ray/Death/Ray Death Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Death/Ray Death Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Death/Ray Death Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Death/Ray Death Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/General Spell/General Spell Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/General Spell/General Spell Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Magic Missile/Ray Magic Missile Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Magic Missile/Ray Magic Missile Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Magic Missile/Ray Magic Missile Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Magic Missile/Ray Magic Missile Bounce", 1.0f },

    { SOUND_BANK_MASTER, "event:/Ray/Disintegration/Ray Disintegration Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Disintegration/Ray Disintegration Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Disintegration/Ray Disintegration Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Disintegration/Ray Disintegration Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Sleep/Ray Sleep Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Sleep/Ray Sleep Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Sleep/Ray Sleep Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Sleep/Ray Sleep Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Poison Gas/Ray Poison Gas Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Poison Gas/Ray Poison Gas Create", 1.0f },

    { SOUND_BANK_MASTER, "event:/Ray/Poison Gas/Ray Poison Gas Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Poison Gas/Ray Poison Gas Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Acid/Ray Acid Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Acid/Ray Acid Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Acid/Ray Acid Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Acid/Ray Acid Bounce", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Petrification/Ray Petrification Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Petrification/Ray Petrification Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Petrification/Ray Petrification Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Petrification/Ray Petrification Bounce", 1.0f },

    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Dark", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Noxious", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Muddy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Wet", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Magical", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Fiery", 1.0f },
    { SOUND_BANK_MASTER, "event:/Explosion/Explosion Frosty", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Surprise Attack", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Statue Becomes Alive", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Was Hiding", 0.5f },

    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Piercer Drops", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Trap Found", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Hidden Door Found", 0.2f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Unearthed Object Found", 0.2f },
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/You Surprise Monster", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Arrow Trap Fire", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Dart Trap Fire", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Hole Opens", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Trap Door Opens", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Fall into Pit", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Land on Spikes", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Squeaky Board", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Falling Rock Trap Trigger", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Rolling Boulder Trap Trigger", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Teleport", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Bear Trap Closes", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Enveloped in Cloud of Gas", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Gush of Water Hits", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Steamy Bubbles", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Tower of Flame Erupts", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Teleport Trap Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Level Teleport Trap Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Level Teleport", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Shudder for Moment", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Wrenching Sensation", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Burn Spider Web", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Caught in Web", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Tear through Web", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Flash and Roar", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Shiver Runs Down Spine", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Distant Howling", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Magic Trap Weird Effect", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Pack Shaking", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Taming", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Magic Trap Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Anti Magic Trap Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Land Mine Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Polymorph Trap Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Magic Portal Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Vibrating Square/Vibrating Square Ambient", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Modron Glimmer Surrounds", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Modron Teleport Success", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Drain Energy", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Feeling Lethargic", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/You Rust", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/You Split", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Rock Hits Hard Helmet", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Rock Hits You on Head", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/General/Gain Intrinsic", 0.5f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Generic Physical Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Generic Magical Activate", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ambience/Effect/Rolling Boulder", 1.0f },
    { SOUND_BANK_MASTER, "event:/UI/Knapsack Full", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Environment/Fall into Water", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Pet in Way", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Pet Does Not Move", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Fountain/Bubbling Water", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Fountain/Water Falling on Coins", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Fountain/Splashing of Naiad", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Fountain/Soda Fountain", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Sink/Slow Drip", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Sink/Gurgling Noise", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Sink/Dishes Being Washed", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Throne/Courtly Conversation", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Throne/Sceptre Pounded", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Throne/Off With Head", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Throne/Queens Cats", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Swamp/Mosquitoes", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Swamp/Marsh Gas", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Swamp/Donald Duck", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Vault/Someone Counting Money", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Vault/Quarterback Calling Play", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Vault/Someone Searching", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Vault/Footsteps of Guard Patrol", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Vault/Ebenezer Scrooge", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Beehive/Low Buzzing", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Beehive/Angry Drone", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Beehive/Bees in Bonnet", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Library/Someone Demanding Quietness", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Library/Pages Turning in Head", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Dragon Lair/Coins Being Assembled", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Dragon Lair/Loud Dragon Snoring", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Dragon Lair/Loud Dragon Roar", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Dragon Lair/Fire and Death", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Barracks/Blades Being Honed", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Barracks/Loud Human Snoring", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Barracks/Dice Being Thrown", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Barracks/General McArthur", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Zoo/Elephant Stepping on Peanut", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Zoo/Seal Breaking", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Zoo/Doctor Dolittle", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Shop/Someone Cursing Shoplifters", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Shop/Chime of Cash Register", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Shop/Neiman and Marcus Arguing", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Deserted Shop/Faint Chime", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Temple/Someone Praising God", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Temple/Someone Beseeching", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Temple/Animal Carcass Being Offered", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Temple/Plea For Donations", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Smithy/Iron Being Forged", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Smithy/Loud Clanging", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Smithy/Water Being Vaporized", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/NPC/Someone Mumbling", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/NPC/Distant Chitchat", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/NPC/Footsteps at Distance", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Oracle/Strange Wind", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Oracle/Convulsive Ravings", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Oracle/Snoring Snakes", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Oracle/No More Woodchucks", 0.3f },

    { SOUND_BANK_MASTER, "event:/SFX/Level/Oracle/Loud Zot", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Armory/Swords Being Unsheated", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Armory/Armors Being Worn", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Armory/Weapons Being Assembled", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Level/Armory/Master Chief", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Lightning Strikes", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Stoning", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Sliming", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Strangulation", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Suffocation", 0.5f },

    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Food Poisoning", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Terminal Illness", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Blindness", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Confusion", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Stun", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Hallucination", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Slow", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Paralysis", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Fear", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Sleep", 0.5f },

    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Cancellation", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Silence", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Grab", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Mummy Rot", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Condition/Acquired/Lycanthropy", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Gain Ability", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Lose Ability", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Restore Ability", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Blinding Flash", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Clerical Monster/Scales Cover Eyes", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/General/Weapon Swapped", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Start Two Weapon Combat", 0.4f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Stop Two Weapon Combat", 0.4f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Electric Shock", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Malignant Aura Resisted", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Malignant Aura Surrounds", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Item Resists Negative Effect", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Miscellaneous/Summon Monster", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Vanishes in Puff of Smoke", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Disarm Trap Success", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Disarm Trap Fail", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/No Traps Found", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Cannot Disarm Trap", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Special Attack/Stumble on Mimic", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Chest Trap Trigger", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Flame Fizzles Out", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Needle Misses", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Cloud Blows Away", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Needle Pricks", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Blasted by Power", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Action/Read", 0.5f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Miscellaneous/Identify Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Recharge/Recharge Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Recharge/Blessed Recharge Success", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Recharge/Strip Charges", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Recharge/Charges at Maximum", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Recharge/Recharge Fail", 0.3f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Miscellaneous/Wizard Lock Klunk", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Spell/Miscellaneous/Knock Klick", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Jump", 1.0f },

    { SOUND_BANK_MASTER, "event:/SFX/Trap/Lever Switch Fail", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Lever Switch Success", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Lever Switch On Success", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/Trap/Lever Switch Off Success", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Digging Beam/Digging Beam Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Digging Beam/Digging Beam Create", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Digging Beam/Digging Beam Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Digging Beam/Digging Beam Hit Location", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Evaporation Beam/Evaporation Beam Ambient", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Evaporation Beam/Evaporation Beam Create", 1.0f },

    { SOUND_BANK_MASTER, "event:/Ray/Evaporation Beam/Evaporation Beam Destroy", 1.0f },
    { SOUND_BANK_MASTER, "event:/Ray/Evaporation Beam/Evaporation Beam Hit Location", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Wall Glows Then Fades", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Tree/Tree Break", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Stone/Stone Break", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Water Break", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Door/Door Consumed in Flames", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Door/Door Freezes and Shatters", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Petrify", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Door/Electricity Hits Door", 1.0f },

    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Burnt", 1.0f },
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Frozen", 1.0f },
    { SOUND_BANK_MASTER, "event:/Object/General/Generic/Generic Electrocuted", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Fountain/Fountain Dries Up", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Some Water Evaporates", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Water is Frozen", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Water Grounds Electricity", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Water Boils", 1.0f },
    { SOUND_BANK_MASTER, "event:/Location/Water/Water Dries Up", 1.0f },
    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Try Locked", 1.0f },

    { SOUND_BANK_MASTER, "event:/Object/Tool/Chest/Chest Resists Open", 1.0f },
    { SOUND_BANK_MASTER, "event:/SFX/General/Key Does Not Fit", 1.0f },
};

/*
    GHSOUND_FOUNTAIN_DRIES_UP,
    GHSOUND_SOME_WATER_EVAPORATES,
    GHSOUND_WATER_IS_FROZEN,
    GHSOUND_WATER_GROUNDS_ELECTRICITY,
    GHSOUND_WATER_BOILS,
    GHSOUND_WATER_DRIES_UP,
    GHSOUND_CHEST_TRY_LOCKED,
    GHSOUND_CHEST_RESISTS_OPEN,
*/
extern "C" 
{
    boolean
    initialize_fmod_studio()
    {
        void* extraDriverData = NULL;

        Studio::System::create(&fmod_studio_system);

        fmod_studio_system->getCoreSystem(&fmod_core_system);
        fmod_core_system->setSoftwareFormat(0, FMOD_SPEAKERMODE_DEFAULT, 0);

        FMOD_RESULT result = fmod_studio_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);

        boolean res = (result == FMOD_OK);
        return res;
    }

    boolean
    load_fmod_banks()
    {
        FMOD_RESULT       result;
        FMOD::Studio::Bank* bank[2] = { 0 };
        HINSTANCE hResInstance = (HINSTANCE)GetModuleHandle(NULL);

        for (int i = 0; i < 2; i++)
        {
            int rid[2] = { IDR_RCDATA_MASTER, IDR_RCDATA_STRINGS };
            HRSRC res = FindResource(hResInstance, MAKEINTRESOURCE(rid[i]), RT_RCDATA);

            if (res)
            {
                HGLOBAL mem = LoadResource(hResInstance, res);
                void* data = LockResource(mem);
                size_t len = SizeofResource(hResInstance, res);

                result = fmod_studio_system->loadBankMemory((const char*)data, (int)len, FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank[i]);
                if (result != FMOD_OK)
                    return FALSE;
            }
        }

        return TRUE;
    }

    boolean
    load_fmod_bank_from_resource(HINSTANCE hInstance, int rid)
    {
        FMOD_RESULT       result;
        FMOD::Studio::Bank* bank = 0;

        HRSRC res = FindResource(hInstance, MAKEINTRESOURCE(rid), RT_RCDATA);

        if (res)
        {
            HGLOBAL mem = LoadResource(hInstance, res);
            void* data = LockResource(mem);
            size_t len = SizeofResource(hInstance, res);

            result = fmod_studio_system->loadBankMemory((const char*)data, (int)len, FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
            if (result != FMOD_OK)
                return FALSE;
        }

        return TRUE;
    }

    boolean
    load_fmod_bank_from_file(HINSTANCE hInstance, char* filename)
    {
        FMOD_RESULT       result;
        FMOD::Studio::Bank* bank = 0;

        result = fmod_studio_system->loadBankFile(filename, FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
        if (result != FMOD_OK)
            return FALSE;

        return TRUE;
    }

    void
    close_fmod_studio()
    {
        FMOD_RESULT result;

        result = fmod_studio_system->unloadAll();
        result = fmod_studio_system->release();

        fmod_studio_system = (Studio::System*)0;
    }

    boolean 
    fmod_play_music(struct ghsound_music_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;
        if (soundid == GHSOUND_NONE)
        {
            if (musicInstances[0].eventInstance && musicInstances[0].normalVolume > 0.0f)
            {
                /* Stop music */
                result = musicInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
                musicInstances[0].normalVolume = 0.0f;
                result = fmod_studio_system->update();
                return (result == FMOD_OK);
            }

            /* Nothing to do */
            return TRUE;
        }


        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;

        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* musicDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &musicDescription);

        if (result != FMOD_OK)
            return FALSE;

        if (musicInstances[0].eventInstance)
        {
            Studio::EventInstance* earlierMusicInstance = musicInstances[0].eventInstance;
            Studio::EventDescription* earlierMusicDescription = NULL;
            earlierMusicInstance->getDescription(&earlierMusicDescription);
            if (earlierMusicDescription && musicDescription == earlierMusicDescription)
            {
                /* Already playing, just check the difference in volume */
                if (musicInstances[0].normalVolume == 0.0f && info.volume > 0.0f)
                {
                    /* Restart the music and adjust volume */
                    result = musicInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_music_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;

                    result = musicInstances[0].eventInstance->start();
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (musicInstances[0].normalVolume > 0.0f && info.volume == 0.0f)
                {
                    /* Stop music */
                    result = musicInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (musicInstances[0].normalVolume != info.volume)
                {
                    /* Adjust volume */
                    result = musicInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_music_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;
                }

                /* Finally, update */
                if (musicInstances[0].normalVolume != info.volume)
                {
                    musicInstances[0].normalVolume = info.volume;
                    result = fmod_studio_system->update();
                    if (result != FMOD_OK)
                        return FALSE;
                }

                return TRUE;
            }
        }

        /* Not playing yet, so make a new instance */
        Studio::EventInstance* musicInstance = NULL;
        result = musicDescription->createInstance(&musicInstance);
        if (result != FMOD_OK)
            return FALSE;

        musicInstance->setVolume(min(1.0f, info.volume * event_volume * general_music_volume * general_volume));
        if (info.volume > 0.0f)
        {
            result = musicInstance->start();
            if (result != FMOD_OK)
                return FALSE;
        }

        /* If starting succeeded, stop the old music in musicInstances[0] by fading */
        if (musicInstances[0].eventInstance)
        {
            musicInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

            /* move to musicInstances[1] for later release, and before that, release existing musicInstances[1] */
            if (musicInstances[1].eventInstance)
                musicInstances[1].eventInstance->release();

            musicInstances[1].eventInstance = musicInstances[0].eventInstance;
            musicInstances[1].ghsound = musicInstances[0].ghsound;
            musicInstances[1].normalVolume = musicInstances[0].normalVolume;
            musicInstances[1].sound_type = musicInstances[0].sound_type;
        }

        /* Set the new instance as musicInstances[0] */
        musicInstances[0].eventInstance = musicInstance;
        musicInstances[0].ghsound = info.ghsound;
        musicInstances[0].normalVolume = info.volume;
        musicInstances[0].sound_type = 0;

        result = fmod_studio_system->update();
        return (result == FMOD_OK);
    }


    boolean
    fmod_play_level_ambient_sound(struct ghsound_environment_ambient_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;

        if (soundid == GHSOUND_NONE)
        {
            if (levelAmbientInstances[0].eventInstance && levelAmbientInstances[0].normalVolume > 0.0f)
            {
                /* Stop ambient sound */
                result = levelAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
                levelAmbientInstances[0].normalVolume = 0.0f;
                result = fmod_studio_system->update();
                return (result == FMOD_OK);
            }

            /* Nothing to do */
            return TRUE;
        }

        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;

        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* occupationAmbientDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &occupationAmbientDescription);

        if (result != FMOD_OK)
            return FALSE;

        if (levelAmbientInstances[0].eventInstance)
        {
            Studio::EventInstance* earlierLevelAmbientInstance = levelAmbientInstances[0].eventInstance;
            Studio::EventDescription* earlierLevelAmbientDescription = NULL;
            earlierLevelAmbientInstance->getDescription(&earlierLevelAmbientDescription);
            if (earlierLevelAmbientDescription && occupationAmbientDescription == earlierLevelAmbientDescription)
            {
                /* Already playing, just check the difference in volume */
                if (levelAmbientInstances[0].normalVolume == 0.0f && info.volume > 0.0f)
                {
                    /* Restart the sound and adjust volume */
                    result = levelAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;

                    result = levelAmbientInstances[0].eventInstance->start();
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (levelAmbientInstances[0].normalVolume > 0.0f && info.volume == 0.0f)
                {
                    /* Stop ambient sound */
                    result = levelAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (levelAmbientInstances[0].normalVolume != info.volume)
                {
                    /* Adjust volume */
                    result = levelAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;
                }

                /* Finally, update */
                if (levelAmbientInstances[0].normalVolume != info.volume)
                {
                    levelAmbientInstances[0].normalVolume = info.volume;
                    result = fmod_studio_system->update();
                    if (result != FMOD_OK)
                        return FALSE;
                }

                return TRUE;
            }
        }

        /* Not playing yet, so make a new instance */
        Studio::EventInstance* levelAmbientInstance = NULL;
        result = occupationAmbientDescription->createInstance(&levelAmbientInstance);
        if (result != FMOD_OK)
            return FALSE;

        result = levelAmbientInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
        if (result != FMOD_OK)
            return FALSE;

        if (info.volume > 0.0f)
        {
            result = levelAmbientInstance->start();
            if (result != FMOD_OK)
                return FALSE;
        }
        /* If starting succeeded, stop the old ambient sound in levelAmbientInstances[0] by fading */
        if (levelAmbientInstances[0].eventInstance)
        {
            if(levelAmbientInstances[0].ghsound != GHSOUND_NONE)
                levelAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

            /* move to levelAmbientInstances[1] for later release, and before that, release existing levelAmbientInstances[1] */
            if (levelAmbientInstances[1].eventInstance)
                levelAmbientInstances[1].eventInstance->release();

            levelAmbientInstances[1].eventInstance = levelAmbientInstances[0].eventInstance;
            levelAmbientInstances[1].ghsound = levelAmbientInstances[0].ghsound;
            levelAmbientInstances[1].normalVolume = levelAmbientInstances[0].normalVolume;
            levelAmbientInstances[1].sound_type = levelAmbientInstances[0].sound_type;
        }

        /* Set the new instance as levelAmbientInstances[0] */
        levelAmbientInstances[0].eventInstance = levelAmbientInstance;
        levelAmbientInstances[0].ghsound = info.ghsound;
        levelAmbientInstances[0].normalVolume = info.volume;
        levelAmbientInstances[0].sound_type = 0;

        result = fmod_studio_system->update();
        return (result == FMOD_OK);
    }

    boolean
    fmod_play_environment_ambient_sound(struct ghsound_level_ambient_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;

        if (soundid == GHSOUND_NONE)
        {
            if (environmentAmbientInstances[0].eventInstance && environmentAmbientInstances[0].normalVolume > 0.0f)
            {
                /* Stop ambient sound */
                result = environmentAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
                environmentAmbientInstances[0].normalVolume = 0.0f;
                result = fmod_studio_system->update();
                return (result == FMOD_OK);
            }

            /* Nothing to do */
            return TRUE;
        }

        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;

        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* occupationAmbientDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &occupationAmbientDescription);

        if (result != FMOD_OK)
            return FALSE;

        if (environmentAmbientInstances[0].eventInstance)
        {
            Studio::EventInstance* earlierEnvironmentAmbientInstance = environmentAmbientInstances[0].eventInstance;
            Studio::EventDescription* earlierEnvironmentAmbientDescription = NULL;
            earlierEnvironmentAmbientInstance->getDescription(&earlierEnvironmentAmbientDescription);
            if (earlierEnvironmentAmbientDescription && occupationAmbientDescription == earlierEnvironmentAmbientDescription)
            {
                /* Already playing, just check the difference in volume */
                if (environmentAmbientInstances[0].normalVolume == 0.0f && info.volume > 0.0f)
                {
                    /* Restart the sound and adjust volume */
                    result = environmentAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;

                    result = environmentAmbientInstances[0].eventInstance->start();
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (environmentAmbientInstances[0].normalVolume > 0.0f && info.volume == 0.0f)
                {
                    /* Stop ambient sound */
                    result = environmentAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (environmentAmbientInstances[0].normalVolume != info.volume)
                {
                    /* Adjust volume */
                    result = environmentAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;
                }

                /* Finally, update */
                if (environmentAmbientInstances[0].normalVolume != info.volume)
                {
                    environmentAmbientInstances[0].normalVolume = info.volume;
                    result = fmod_studio_system->update();
                    if (result != FMOD_OK)
                        return FALSE;
                }

                return TRUE;
            }
        }

        /* Not playing yet, so make a new instance */
        Studio::EventInstance* environmentAmbientInstance = NULL;
        result = occupationAmbientDescription->createInstance(&environmentAmbientInstance);
        if (result != FMOD_OK)
            return FALSE;

        result = environmentAmbientInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
        if (result != FMOD_OK)
            return FALSE;

        if (info.volume > 0.0f)
        {
            result = environmentAmbientInstance->start();
            if (result != FMOD_OK)
                return FALSE;
        }
        /* If starting succeeded, stop the old ambient sound in environmentAmbientInstances[0] by fading */
        if (environmentAmbientInstances[0].eventInstance)
        {
            if (environmentAmbientInstances[0].ghsound != GHSOUND_NONE)
                environmentAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

            /* move to environmentAmbientInstances[1] for later release, and before that, release existing environmentAmbientInstances[1] */
            if (environmentAmbientInstances[1].eventInstance)
                environmentAmbientInstances[1].eventInstance->release();

            environmentAmbientInstances[1].eventInstance = environmentAmbientInstances[0].eventInstance;
            environmentAmbientInstances[1].ghsound = environmentAmbientInstances[0].ghsound;
            environmentAmbientInstances[1].normalVolume = environmentAmbientInstances[0].normalVolume;
            environmentAmbientInstances[1].sound_type = environmentAmbientInstances[0].sound_type;
        }

        /* Set the new instance as environmentAmbientInstances[0] */
        environmentAmbientInstances[0].eventInstance = environmentAmbientInstance;
        environmentAmbientInstances[0].ghsound = info.ghsound;
        environmentAmbientInstances[0].normalVolume = info.volume;
        environmentAmbientInstances[0].sound_type = 0;

        result = fmod_studio_system->update();
        return (result == FMOD_OK);
    }

    boolean
    fmod_play_occupation_ambient_sound(struct ghsound_occupation_ambient_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;

        if (soundid == GHSOUND_NONE)
        {
            if (occupationAmbientInstances[0].eventInstance && occupationAmbientInstances[0].normalVolume > 0.0f)
            {
                /* Stop ambient sound */
                result = occupationAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
                occupationAmbientInstances[0].normalVolume = 0.0f;
                result = fmod_studio_system->update();
                return (result == FMOD_OK);
            }

            /* Nothing to do */
            return TRUE;
        }

        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;

        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* occupationAmbientDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &occupationAmbientDescription);

        if (result != FMOD_OK)
            return FALSE;

        if (occupationAmbientInstances[0].eventInstance)
        {
            Studio::EventInstance* earlierOccupationAmbientInstance = occupationAmbientInstances[0].eventInstance;
            Studio::EventDescription* earlierOccupationAmbientDescription = NULL;
            earlierOccupationAmbientInstance->getDescription(&earlierOccupationAmbientDescription);
            if (earlierOccupationAmbientDescription && occupationAmbientDescription == earlierOccupationAmbientDescription)
            {
                /* Already playing, just check the difference in volume */
                if (occupationAmbientInstances[0].normalVolume == 0.0f && info.volume > 0.0f)
                {
                    /* Restart the sound and adjust volume */
                    result = occupationAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;

                    result = occupationAmbientInstances[0].eventInstance->start();
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (occupationAmbientInstances[0].normalVolume > 0.0f && info.volume == 0.0f)
                {
                    /* Stop ambient sound */
                    result = occupationAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                    if (result != FMOD_OK)
                        return FALSE;
                }
                else if (occupationAmbientInstances[0].normalVolume != info.volume)
                {
                    /* Adjust volume */
                    result = occupationAmbientInstances[0].eventInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
                    if (result != FMOD_OK)
                        return FALSE;
                }

                /* Finally, update */
                if (occupationAmbientInstances[0].normalVolume != info.volume)
                {
                    occupationAmbientInstances[0].normalVolume = info.volume;
                    result = fmod_studio_system->update();
                    if (result != FMOD_OK)
                        return FALSE;
                }

                return TRUE;
            }
        }

        /* Not playing yet, so make a new instance */
        Studio::EventInstance* occupationAmbientInstance = NULL;
        result = occupationAmbientDescription->createInstance(&occupationAmbientInstance);
        if (result != FMOD_OK)
            return FALSE;

        result = occupationAmbientInstance->setVolume(min(1.0f, info.volume * event_volume * general_ambient_volume * general_volume));
        if (result != FMOD_OK)
            return FALSE;

        if (info.volume > 0.0f)
        {
            result = occupationAmbientInstance->start();
            if (result != FMOD_OK)
                return FALSE;
        }
        /* If starting succeeded, stop the old ambient sound in occupationAmbientInstances[0] by fading */
        if (occupationAmbientInstances[0].eventInstance)
        {
            if (occupationAmbientInstances[0].ghsound != GHSOUND_NONE)
                occupationAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

            /* move to occupationAmbientInstances[1] for later release, and before that, release existing occupationAmbientInstances[1] */
            if (occupationAmbientInstances[1].eventInstance)
                occupationAmbientInstances[1].eventInstance->release();

            occupationAmbientInstances[1].eventInstance = occupationAmbientInstances[0].eventInstance;
            occupationAmbientInstances[1].ghsound = occupationAmbientInstances[0].ghsound;
            occupationAmbientInstances[1].normalVolume = occupationAmbientInstances[0].normalVolume;
            occupationAmbientInstances[1].sound_type = occupationAmbientInstances[0].sound_type;
        }

        /* Set the new instance as occupationAmbientInstances[0] */
        occupationAmbientInstances[0].eventInstance = occupationAmbientInstance;
        occupationAmbientInstances[0].ghsound = info.ghsound;
        occupationAmbientInstances[0].normalVolume = info.volume;
        occupationAmbientInstances[0].sound_type = 0;

        result = fmod_studio_system->update();
        return (result == FMOD_OK);
    }




    boolean
    fmod_set_effect_ambient_volume(struct effect_ambient_volume_info info)
    {
        FMOD_RESULT result;
        if (effectAmbientInstances[0].eventInstance)
        {
            struct ghsound_eventmapping eventmap = ghsound2event[effectAmbientInstances[0].ghsound];
            float event_volume = eventmap.volume;
            float volume = info.volume;

            /* Adjust volume */
            effectAmbientInstances[0].eventInstance->setVolume(min(1.0f, volume * event_volume * general_sfx_volume * general_volume));

            if (volume == 0.0f && effectAmbientInstances[0].normalVolume > 0.0f)
            {
                /* Stop ambient */
                result = effectAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
            }
            else if (volume > 0.0f && effectAmbientInstances[0].normalVolume == 0.0f)
            {
                /* Start ambient */
                result = effectAmbientInstances[0].eventInstance->start();
                if (result != FMOD_OK)
                    return FALSE;

            }

            effectAmbientInstances[0].normalVolume = volume;
            result = fmod_studio_system->update();
            return (result == FMOD_OK);
        }

        /* Nothing's playing */
        return FALSE;
    }

    boolean
    fmod_play_effect_ambient_sound(struct ghsound_effect_ambient_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;

        if (soundid == GHSOUND_NONE)
        {
            if (effectAmbientInstances[0].eventInstance)
            {
                /* Stop ambient sound */
                result = effectAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                if (result != FMOD_OK)
                    return FALSE;
                effectAmbientInstances[0].ghsound = GHSOUND_NONE;
                effectAmbientInstances[0].normalVolume = 0.0f;
                effectAmbientInstances[0].sound_type = 0;
                result = fmod_studio_system->update();
                return (result == FMOD_OK);
            }

            /* Nothing to do */
            return TRUE;
        }

        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;

        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* effectAmbientDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &effectAmbientDescription);

        if (result != FMOD_OK)
            return FALSE;

        if (effectAmbientInstances[0].eventInstance)
        {
            Studio::EventInstance* earlierEffectAmbientInstance = effectAmbientInstances[0].eventInstance;
            Studio::EventDescription* earlierEffectAmbientDescription = NULL;
            earlierEffectAmbientInstance->getDescription(&earlierEffectAmbientDescription);
            if (earlierEffectAmbientDescription && effectAmbientDescription == earlierEffectAmbientDescription)
            {
                /* Already selected, but may be stopped; just set the volume, which may start or stop the ambient */
                struct effect_ambient_volume_info vinfo = { 0 };
                vinfo.volume = info.volume;
                return fmod_set_effect_ambient_volume(vinfo);
            }
        }

        /* Not playing yet, so make a new instance */
        Studio::EventInstance* effectAmbientInstance = NULL;
        result = effectAmbientDescription->createInstance(&effectAmbientInstance);
        if (result != FMOD_OK)
            return FALSE;

        /* Note: effect ambient uses sfx volume instead of ambient volume */
        effectAmbientInstance->setVolume(min(1.0f, info.volume * event_volume * general_sfx_volume * general_volume));
        if (info.volume > 0.0f)
        {
            result = effectAmbientInstance->start();
            if (result != FMOD_OK)
                return FALSE;
        }

        /* If starting succeeded, stop the old ambient sound in effectAmbientInstances[0] by fading */
        if (effectAmbientInstances[0].eventInstance)
        {
            if (effectAmbientInstances[0].ghsound != GHSOUND_NONE && effectAmbientInstances[0].normalVolume > 0.0f)
                effectAmbientInstances[0].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

            /* move to effectAmbientInstances[1] for later release, and before that, release existing effectAmbientInstances[1] */
            if (effectAmbientInstances[1].eventInstance)
                effectAmbientInstances[1].eventInstance->release();

            effectAmbientInstances[1].eventInstance = effectAmbientInstances[0].eventInstance;
            effectAmbientInstances[1].ghsound = effectAmbientInstances[0].ghsound;
            effectAmbientInstances[1].normalVolume = effectAmbientInstances[0].normalVolume;
            effectAmbientInstances[1].sound_type = effectAmbientInstances[0].sound_type;
        }

        /* Set the new instance as effectAmbientInstances[0] */
        effectAmbientInstances[0].eventInstance = effectAmbientInstance;
        effectAmbientInstances[0].ghsound = info.ghsound;
        effectAmbientInstances[0].normalVolume = info.volume;
        effectAmbientInstances[0].sound_type = 0;

        result = fmod_studio_system->update();
        return (result == FMOD_OK);
    }

    boolean
    fmod_play_immediate_sound(struct ghsound_immediate_info info)
    {
        FMOD_RESULT result;

        enum ghsound_types soundid = info.ghsound;
        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;
        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;
        float relevant_general_volume = (info.sound_type == IMMEDIATE_SOUND_UI ? general_ui_volume : general_sfx_volume);

        Studio::EventDescription* immediateSoundDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &immediateSoundDescription);
        if (result != FMOD_OK)
            return FALSE;

        Studio::EventInstance* immediateSoundInstance = NULL;
        result = immediateSoundDescription->createInstance(&immediateSoundInstance);
        if (result != FMOD_OK)
            return FALSE;

        /* Set volume */
        result = immediateSoundInstance->setVolume(min(1.0f, info.volume * event_volume * relevant_general_volume * general_volume));
        if (result != FMOD_OK)
            return FALSE;

        if (result != FMOD_OK)
            return FALSE;

        /* Set parameters */
        FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
        for (int i = 0; i < MAX_SOUND_PARAMETERS; i++)
        {
            if(!info.parameter_names[i] || strcmp(info.parameter_names[i], "") == 0)
                break;

            result = immediateSoundDescription->getParameterDescriptionByName(info.parameter_names[i], &paramDesc);
            if (result != FMOD_OK)
                return FALSE;

            FMOD_STUDIO_PARAMETER_ID paramID = paramDesc.id;
            float parameterValue = info.parameter_values[i];
            result = immediateSoundInstance->setParameterByID(paramID, parameterValue);
            if (result != FMOD_OK)
                return FALSE;
        }

        /* Play sound */
        result = immediateSoundInstance->start();
        if (result != FMOD_OK)
            return FALSE;

        if (immediateSoundInstances[0].eventInstance)
        {
            /* Delete the last one */
            if (immediateSoundInstances[NUM_IMMEDIATE_SOUND_INSTANCES - 1].eventInstance)
            {
                /* Stop playing just in case */
                immediateSoundInstances[NUM_IMMEDIATE_SOUND_INSTANCES - 1].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                /* Release */
                immediateSoundInstances[NUM_IMMEDIATE_SOUND_INSTANCES - 1].eventInstance->release();
            }

            /* Move all others back */
            for (int i = NUM_IMMEDIATE_SOUND_INSTANCES - 1; i >= 1; i--)
            {
                immediateSoundInstances[i].eventInstance = immediateSoundInstances[i - 1].eventInstance;
                immediateSoundInstances[i].ghsound = immediateSoundInstances[i - 1].ghsound;
                immediateSoundInstances[i].normalVolume = immediateSoundInstances[i - 1].normalVolume;
                immediateSoundInstances[i].sound_type = immediateSoundInstances[i - 1].sound_type;
            }
        }

        /* Set the new instance as movementInstances[0] */
        immediateSoundInstances[0].eventInstance = immediateSoundInstance;
        immediateSoundInstances[0].ghsound = info.ghsound;
        immediateSoundInstances[0].normalVolume = info.volume;
        immediateSoundInstances[0].sound_type = info.sound_type;

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        return TRUE;
    }

    

    int
    fmod_adjust_ghsound_general_volumes(float new_general_volume, float new_general_music_volume, float new_general_ambient_volume, float new_general_sfx_volume, float new_general_ui_volume)
    {
        general_volume = new_general_volume;
        general_music_volume = new_general_music_volume;
        general_ambient_volume = new_general_ambient_volume;
        general_sfx_volume = new_general_sfx_volume;
        general_ui_volume = new_general_ui_volume;

        FMOD_RESULT result;
        for (int i = 0; i <= 1; i++)
        {
            if (musicInstances[i].eventInstance)
            {
                enum ghsound_types soundid = musicInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = musicInstances[i].eventInstance->setVolume(min(1.0f, musicInstances[i].normalVolume * event_volume * general_music_volume * general_volume));
            }
        }
        for (int i = 0; i <= 1; i++)
        {
            if (levelAmbientInstances[i].eventInstance)
            {
                enum ghsound_types soundid = levelAmbientInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = levelAmbientInstances[i].eventInstance->setVolume(min(1.0f, levelAmbientInstances[i].normalVolume * event_volume * general_ambient_volume * general_volume));
            }
        }
        for (int i = 0; i <= 1; i++)
        {
            if (environmentAmbientInstances[i].eventInstance)
            {
                enum ghsound_types soundid = environmentAmbientInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = environmentAmbientInstances[i].eventInstance->setVolume(min(1.0f, environmentAmbientInstances[i].normalVolume * event_volume * general_ambient_volume * general_volume));
            }
        }
        for (int i = 0; i <= 1; i++)
        {
            if (occupationAmbientInstances[i].eventInstance)
            {
                enum ghsound_types soundid = occupationAmbientInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = occupationAmbientInstances[i].eventInstance->setVolume(min(1.0f, occupationAmbientInstances[i].normalVolume * event_volume * general_ambient_volume * general_volume));
            }
        }
        for (int i = 0; i <= 1; i++)
        {
            if (effectAmbientInstances[i].eventInstance)
            {
                enum ghsound_types soundid = effectAmbientInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = effectAmbientInstances[i].eventInstance->setVolume(min(1.0f, effectAmbientInstances[i].normalVolume * event_volume * general_ambient_volume * general_volume));
            }
        }
        for (int i = 0; i < NUM_IMMEDIATE_SOUND_INSTANCES; i++)
        {
            if (immediateSoundInstances[i].eventInstance)
            {
                enum ghsound_types soundid = immediateSoundInstances[i].ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                float relevant_general_volume = (immediateSoundInstances[i].sound_type == IMMEDIATE_SOUND_UI ? general_ui_volume : general_sfx_volume);
                result = immediateSoundInstances[i].eventInstance->setVolume(min(1.0f, immediateSoundInstances[i].normalVolume * event_volume * relevant_general_volume * general_volume));
            }
        }
        for (GNHSoundInstance* curr = ambient_base; curr; curr = curr->next_instance)
        {
            if (curr->eventInstance)
            {
                enum ghsound_types soundid = curr->ghsound;
                struct ghsound_eventmapping eventmap = ghsound2event[soundid];
                float event_volume = eventmap.volume;
                result = curr->eventInstance->setVolume(min(1.0f, curr->normalVolume * event_volume * general_ambient_volume * general_volume));
            }
        }

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        return TRUE;
    }

    int
    fmod_add_ambient_ghsound(enum ghsound_types ghsound, float fmod_volume, void** ambient_sound_ptr_ptr)
    {
        FMOD_RESULT result;

        struct ghsound_eventmapping eventmap = ghsound2event[ghsound];
        float event_volume = eventmap.volume;
        if (!eventmap.eventPath || !strcmp(eventmap.eventPath, ""))
            return FALSE;

        Studio::EventDescription* ambientDescription = NULL;
        result = fmod_studio_system->getEvent(eventmap.eventPath, &ambientDescription);

        if (result != FMOD_OK)
            return FALSE;

        Studio::EventInstance* ambientInstance = NULL;
        result = ambientDescription->createInstance(&ambientInstance);
        if (result != FMOD_OK)
            return FALSE;

        /* Set volume */
        ambientInstance->setVolume(fmod_volume * event_volume * general_ambient_volume * general_volume);

        /* Create new GHSoundInstance */
        GNHSoundInstance* new_ghs_instance = (GNHSoundInstance*)malloc(sizeof(GNHSoundInstance));
        if (!new_ghs_instance)
        {
            result = ambientInstance->release();
            return FALSE;
        }

        /* Play sound */
        result = ambientInstance->start();
        if (result != FMOD_OK)
        {
            result = ambientInstance->release();
            free(new_ghs_instance);
            return FALSE;
        }

        memset((void*)new_ghs_instance, 0, sizeof(new_ghs_instance));
        new_ghs_instance->next_instance = ambient_base;
        ambient_base = new_ghs_instance;
        
        new_ghs_instance->eventInstance = ambientInstance;
        new_ghs_instance->ghsound = ghsound;
        new_ghs_instance->normalVolume = fmod_volume;

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        *ambient_sound_ptr_ptr = (void*)new_ghs_instance;

        return TRUE;
    }

    int
    fmod_delete_ambient_ghsound(void* ambient_sound_ptr)
    {
        if (!ambient_sound_ptr)
            return FALSE;

        FMOD_RESULT result;
        boolean found = FALSE;
        GNHSoundInstance* ghs_ptr = (GNHSoundInstance*)ambient_sound_ptr;
        for (GNHSoundInstance* ghs_curr = ambient_base; ghs_curr; ghs_curr = ghs_curr->next_instance)
        {
            if (ghs_curr == ghs_ptr)
            {
                found = TRUE;
                break;
            }
        }

        if (!found)
            return FALSE;

        if (!ghs_ptr->eventInstance)
            return FALSE;

        result = ghs_ptr->eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        if (result != FMOD_OK)
            return FALSE;

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        result = ghs_ptr->eventInstance->release();
        if (result != FMOD_OK)
            return FALSE;

        GNHSoundInstance* prev = NULL;
        for (GNHSoundInstance* curr = ambient_base; curr; curr = curr->next_instance)
        {
            if (curr == ghs_ptr)
            {
                if (prev == NULL)
                    ambient_base = curr->next_instance;
                else
                    prev->next_instance = curr->next_instance;

                break;
            }
            prev = curr;
        }
        
        free(ghs_ptr);

        return TRUE;
    }

    int
    fmod_set_ambient_ghsound_volume(void* ambient_sound_ptr, float fmod_volume)
    {
        if (!ambient_sound_ptr)
            return FALSE;

        if (fmod_volume < 0.0f || fmod_volume > 1.0f)
            return FALSE;

        FMOD_RESULT result;
        GNHSoundInstance* ghs_ptr = (GNHSoundInstance*)ambient_sound_ptr;
        if (!ghs_ptr->eventInstance)
            return FALSE;

        float old_volume;
        result = ghs_ptr->eventInstance->getVolume(&old_volume);
        if (result != FMOD_OK)
            return FALSE;

        enum ghsound_types soundid = ghs_ptr->ghsound;
        struct ghsound_eventmapping eventmap = ghsound2event[soundid];
        float event_volume = eventmap.volume;
        result = ghs_ptr->eventInstance->setVolume(fmod_volume * event_volume * general_ambient_volume * general_volume);
        if (result != FMOD_OK)
            return FALSE;

        if (old_volume == 0.0f && fmod_volume > 0.0f)
            result = ghs_ptr->eventInstance->start();
        else if (old_volume > 0.0f && fmod_volume == 0.0f)
            result = ghs_ptr->eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

        if (result != FMOD_OK)
            return FALSE;

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        ghs_ptr->normalVolume = fmod_volume;

        return TRUE;
    }


    boolean
    fmod_stop_all_sounds(struct stop_all_info info)
    {

        FMOD_RESULT result;
        if (info.stop_flags & 1)
        {
            for (int i = 0; i <= 1; i++)
            {
                if (musicInstances[i].eventInstance)
                {
                    if ((result = musicInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                        return FALSE;
                    if ((result = musicInstances[i].eventInstance->release()) != FMOD_OK)
                        return FALSE;
                    musicInstances[i].eventInstance = 0;
                }
            }
        }

        for (int i = 0; i <= 1; i++)
        {
            if (levelAmbientInstances[i].eventInstance)
            {
                if (levelAmbientInstances[i].normalVolume > 0.0f && (result = levelAmbientInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                    return FALSE;
                if ((result = levelAmbientInstances[i].eventInstance->release()) != FMOD_OK)
                    return FALSE;
                levelAmbientInstances[i].eventInstance = 0;
            }
        }

        for (int i = 0; i <= 1; i++)
        {
            if (environmentAmbientInstances[i].eventInstance)
            {
                if (environmentAmbientInstances[i].normalVolume > 0.0f && (result = environmentAmbientInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                    return FALSE;
                if ((result = environmentAmbientInstances[i].eventInstance->release()) != FMOD_OK)
                    return FALSE;
                environmentAmbientInstances[i].eventInstance = 0;
            }
        }

        for (int i = 0; i <= 1; i++)
        {
            if (occupationAmbientInstances[i].eventInstance)
            {
                if (occupationAmbientInstances[i].normalVolume > 0.0f && (result = occupationAmbientInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                    return FALSE;
                if ((result = occupationAmbientInstances[i].eventInstance->release()) != FMOD_OK)
                    return FALSE;
                occupationAmbientInstances[i].eventInstance = 0;
            }
        }
        for (int i = 0; i <= 1; i++)
        {
            if (effectAmbientInstances[i].eventInstance)
            {
                if (effectAmbientInstances[i].normalVolume > 0.0f && (result = effectAmbientInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                    return FALSE;
                if ((result = effectAmbientInstances[i].eventInstance->release()) != FMOD_OK)
                    return FALSE;
                effectAmbientInstances[i].eventInstance = 0;
            }
        }
        if (info.stop_flags & 2)
        {
            for (int i = 0; i < NUM_IMMEDIATE_SOUND_INSTANCES; i++)
            {
                if (immediateSoundInstances[i].eventInstance)
                {
                    if (immediateSoundInstances[i].normalVolume > 0.0f && (result = immediateSoundInstances[i].eventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT)) != FMOD_OK)
                        return FALSE;
                    if ((result = immediateSoundInstances[i].eventInstance->release()) != FMOD_OK)
                        return FALSE;
                    immediateSoundInstances[i].eventInstance = 0;
                }
            }
        }

        /* These we just set volume to zero, since they are linked with sound_sources */
        for (GNHSoundInstance* curr = ambient_base; curr; curr = curr->next_instance)
        {
            if (curr->eventInstance)
            {
                if (curr->normalVolume > 0.0f)
                    fmod_set_ambient_ghsound_volume((void*)curr, 0.0f);
            }
        }

        result = fmod_studio_system->update();
        if (result != FMOD_OK)
            return FALSE;

        return TRUE;
    }

}


/* soundfx.cpp */
