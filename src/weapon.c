/* NetHack 0.1	weapon.c	$NHDT-Date: 1548209744 2019/01/23 02:15:44 $  $NHDT-Branch: NetHack-3.6.2-beta01 $:$NHDT-Revision: 1.69 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2011. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *      This module contains code for calculation of "to hit" and damage
 * GnollHacknuses for any given weapon used, as well as weapons selection
 *      code for monsters.
 */
#include "hack.h"

STATIC_DCL void FDECL(give_may_advance_msg, (int));
STATIC_DCL boolean FDECL(could_advance, (int));
STATIC_DCL boolean FDECL(peaked_skill, (int));
STATIC_DCL int FDECL(slots_required, (int));
STATIC_DCL void FDECL(skill_advance, (int));

/* Categories whose names don't come from OBJ_NAME(objects[type])
 */
#define PN_BARE_HANDED (-1) /* includes martial arts */
#define PN_TWO_WEAPONS (-2)
#define PN_RIDING (-3)
#define PN_POLEARMS (-4)
#define PN_SABER (-5)
#define PN_HAMMER (-6)
#define PN_WHIP (-7)
#define PN_ARCANE_SPELL (-8)
#define PN_CLERIC_SPELL (-9)
#define PN_HEALING_SPELL (-10)
#define PN_DIVINATION_SPELL (-11)
#define PN_ABJURATION_SPELL (-12)
#define PN_MOVEMENT_SPELL (-13)
#define PN_TRANSMUTATION_SPELL (-14)
#define PN_ENCHANTMENT_SPELL (-15)
#define PN_CONJURATION_SPELL (-16)
#define PN_NECROMANCY_SPELL (-17)
#define PN_DISARM_TRAP (-18)


STATIC_VAR NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
    0, DAGGER, KNIFE, AXE, PICK_AXE, SHORT_SWORD, BROADSWORD, LONG_SWORD,
    TWO_HANDED_SWORD, SCIMITAR, PN_SABER, CLUB, MACE, MORNING_STAR, FLAIL,
    PN_HAMMER, QUARTERSTAFF, PN_POLEARMS, SPEAR, TRIDENT, LANCE, BOW, SLING,
    CROSSBOW, DART, SHURIKEN, BOOMERANG, PN_WHIP, UNICORN_HORN,
    PN_ARCANE_SPELL, PN_CLERIC_SPELL, PN_HEALING_SPELL, PN_DIVINATION_SPELL,
    PN_ABJURATION_SPELL, PN_MOVEMENT_SPELL, PN_TRANSMUTATION_SPELL, PN_ENCHANTMENT_SPELL, PN_CONJURATION_SPELL, PN_NECROMANCY_SPELL,
    PN_BARE_HANDED, PN_TWO_WEAPONS, PN_RIDING, PN_DISARM_TRAP
};

/* note: entry [0] isn't used */
STATIC_VAR NEARDATA const char *const odd_skill_names[] = {
    "no skill", "bare hands", /* use barehands_or_martial[] instead */
    "two weapon combat", "riding", "polearms", "saber", "hammer", "whip",
    "arcane spells", "clerical spells", "healing spells", "divination spells", "abjuration spells",
	"movement spells", "transmutation spells", "enchantment spells", "conjuration spells", "necromancy spells", "disarm trap",
};
/* indexed vis `is_martial() */
STATIC_VAR NEARDATA const char *const barehands_or_martial[] = {
    "bare handed combat", "martial arts"
};

#define P_NAME(type)                                    \
    ((skill_names_indices[type] > 0)                    \
         ? OBJ_NAME(objects[skill_names_indices[type]]) \
         : (type == P_BARE_HANDED_COMBAT)               \
               ? barehands_or_martial[martial_bonus()]  \
               : odd_skill_names[-skill_names_indices[type]])

static NEARDATA const char kebabable[] = { S_XORN, S_DRAGON, S_JABBERWOCK,
                                           S_NAGA, S_GIANT,  '\0' };

STATIC_OVL void
give_may_advance_msg(skill)
int skill;
{
    You_feel("more confident in your %sskills.",
             (skill == P_NONE) ? ""
                 : (skill <= P_LAST_WEAPON) ? "weapon "
                     : (skill <= P_LAST_SPELL) ? "spell casting "
                         : "fighting ");
}

/* weapon's skill category name for use as generalized description of weapon;
   mostly used to shorten "you drop your <weapon>" messages when slippery
   fingers or polymorph causes hero to involuntarily drop wielded weapon(s) */
const char *
weapon_descr(obj)
struct obj *obj;
{
    int skill = weapon_type(obj);
    const char *descr = P_NAME(skill);

    /* assorted special cases */
    switch (skill) {
    case P_NONE:
        /* not a weapon or weptool: use item class name;
           override class name "food" for corpses, tins, and eggs,
           "large rock" for statues and boulders, and "tool" for towels */
        descr = (obj->otyp == CORPSE || obj->otyp == TIN || obj->otyp == EGG
                 || obj->otyp == STATUE || obj->otyp == BOULDER
                 || obj->otyp == TOWEL)
                    ? OBJ_NAME(objects[obj->otyp])
                    : def_oc_syms[(int) obj->oclass].name;
        break;
    case P_SLING:
        if (is_ammo(obj))
            descr = (is_rock(obj) || is_graystone(obj))
                        ? "stone"
                        /* avoid "rock"; what about known glass? */
                        : (obj->oclass == GEM_CLASS)
                            ? "gem"
                            /* in case somebody adds odd sling ammo */
                            : (obj->otyp == LEADEN_SLING_BULLET || obj->otyp == IRON_SLING_BULLET || obj->otyp == SILVER_SLING_BULLET || is_graystone(obj))
								? "sling-bullet"
								: def_oc_syms[(int) obj->oclass].name;
        break;
    case P_BOW:
        if (is_ammo(obj))
            descr = "arrow";
        break;
    case P_CROSSBOW:
        if (is_ammo(obj))
            descr = "bolt";
        break;
    case P_FLAIL:
        if (obj->otyp == GRAPPLING_HOOK)
            descr = "hook";
        break;
    case P_PICK_AXE:
        /* even if "dwarvish mattock" hasn't been discovered yet */
        if (obj->otyp == DWARVISH_MATTOCK)
            descr = "mattock";
        break;
    default:
        break;
    }
    return makesingular(descr);
}

int
weapon_range(ammo, launcher)
struct obj* ammo;
struct obj* launcher;
{
	if (!ammo && !launcher)
		return 0;

	int baserange = 0, range = 0;
	boolean thrown = TRUE;

	/* Ammunition range */
	if (!ammo && launcher && is_launcher(launcher)) {
		if(objects[launcher->otyp].oc_range > 0)
			baserange = objects[launcher->otyp].oc_range;										/* Crossbows and the like */
		else if (objects[launcher->otyp].oc_range < 0)
			baserange = max(1, (int)((ACURRSTR * -objects[launcher->otyp].oc_range) /100));		/* Bows */

		/* No more info supplied */
		range = baserange;
	}
	else if (ammo && is_ammo(ammo) && launcher && ammo_and_launcher(ammo, launcher)) {
			thrown = FALSE;
			if (objects[launcher->otyp].oc_range > 0)
				baserange = objects[launcher->otyp].oc_range;										/* Crossbows and the like */
			else if (objects[launcher->otyp].oc_range < 0)
				baserange = max(1, (int)((ACURRSTR * -objects[launcher->otyp].oc_range) / 100));		/* Bows */

			range = baserange;
			if(!(objects[launcher->otyp].oc_flags & O1_WEIGHT_DOES_NOT_REDUCE_RANGE || objects[ammo->otyp].oc_flags & O1_WEIGHT_DOES_NOT_REDUCE_RANGE))
				range = range - (int)(ammo->owt / 100);
	}
	else if(ammo) //Normal thrown weapons are half distance
	{
		boolean overriden = FALSE;

		/* oc_range can be used to override usual throwing range for non-launchers */
		if (!is_launcher(ammo) && objects[ammo->otyp].oc_range != 0)
		{
			overriden = TRUE;
			if (objects[ammo->otyp].oc_range > 0)
				baserange = objects[ammo->otyp].oc_range;										/* Crossbows and the like */
			else if (objects[ammo->otyp].oc_range < 0)
				baserange = max(1, (int)((ACURRSTR * -objects[ammo->otyp].oc_range) / 100));		/* Bows */
		}

		if(!overriden)
		{
			if(objects[ammo->otyp].oc_flags & O1_THROWN_WEAPON)
				baserange = (int)(ACURRSTR / 2);
			else
				baserange = (int)(ACURRSTR / 3);
		}

		//Weight of the object reduces range
		if (objects[ammo->otyp].oc_flags & O1_WEIGHT_DOES_NOT_REDUCE_RANGE)
			range = baserange;
		else
		{
			if (objects[ammo->otyp].oc_flags & O1_THROWN_WEAPON)
				range = baserange - (int)(ammo->owt / 100);
			else
				range = baserange - (int)(ammo->owt / 40);
		}
	}

	if (ammo && uball && ammo == uball) {
		if (u.ustuck || (u.utrap && u.utraptype == TT_INFLOOR))
			range = 1;
		else if (range >= 5)
			range = 5;
	}

#if 0
	/* Kludges removed by JG; Items and monsters should have the right weight and strength */
	if (ammo->otyp == BOULDER)
		if (throws_rocks(youmonst.data))
			range = 20; /* you must be giant */
		else
			range = 10; /* non-giant */
	else if (ammo->oartifact == ART_MJOLLNIR)
		range = (range + 1) / 2; /* it's heavy */
#endif

	if (range < 1)
		range = 1;


	return range;
}




/*
 *      hitval returns an integer representing the "to hit" bonuses
 *      of "otmp" against the monster.
 */
int basehitval(otmp)
struct obj* otmp;
{
	if (!otmp)
		return 0;

	int tmp = 0;
	boolean Is_weapon = is_weapon(otmp);

	if (Is_weapon)
		tmp += otmp->spe;

	tmp += objects[otmp->otyp].oc_hitbonus;

	return tmp;

}
int
hitval(otmp, mon, mattacker)
struct obj *otmp;
struct monst *mon;
struct monst* mattacker;
{
    int tmp = 0;
    struct permonst *ptr = mon->data;
	boolean Is_weapon = is_weapon(otmp);

	tmp += basehitval(otmp);

    /* Put weapon vs. monster type "to hit" bonuses in below: */

    /* Blessed weapons used against undead or demons */
    if (Is_weapon && otmp->blessed
        && (is_demon(ptr) || is_undead(ptr) || is_vampshifter(mon)))
        tmp += 2;

    if (is_spear(otmp) && index(kebabable, ptr->mlet))
        tmp += 2;

    /* trident is highly effective against swimmers */
    if (otmp->otyp == TRIDENT && is_swimmer(ptr)) {
        if (is_pool(mon->mx, mon->my))
            tmp += 4;
        else if (ptr->mlet == S_EEL || ptr->mlet == S_SNAKE)
            tmp += 2;
    }

    /* Picks used against xorns and earth elementals */
    if (is_pick(otmp) && (passes_walls(ptr) && thick_skinned(ptr)))
        tmp += 2;

    /* Check specially named weapon "to hit" bonuses */
    if (otmp->oartifact)
        tmp += spec_abon(otmp, mon);

    return tmp;
}

/* Historical note: The original versions of Hack used a range of damage
 * which was similar to, but not identical to the damage used in Advanced
 * Dungeons and Dragons.  I figured that since it was so close, I may as well
 * make it exactly the same as AD&D, adding some more weapons in the process.
 * This has the advantage that it is at least possible that the player would
 * already know the damage of at least some of the weapons.  This was circa
 * 1987 and I used the table from Unearthed Arcana until I got tired of typing
 * them in (leading to something of an imbalance towards weapons early in
 * alphabetical order).  The data structure still doesn't include fields that
 * fully allow the appropriate damage to be described (there's no way to say
 * 3d6 or 1d6+1) so we add on the extra damage in dmgval() if the weapon
 * doesn't do an exact die of damage.
 *
 * Of course new weapons were added later in the development of NetHack.  No
 * AD&D consistency was kept, but most of these don't exist in AD&D anyway.
 *
 * Second edition AD&D came out a few years later; luckily it used the same
 * table.  As of this writing (1999), third edition is in progress, not
 * released.  Let's see if the weapon table stays the same.  --KAA
 * October 2000: It didn't.  Oh, well.
 */

/*
 *      dmgval returns an integer representing the damage bonuses
 *      of "otmp" against the monster.
 */
int
dmgval(otmp, mon, mattacker)
struct obj *otmp;
struct monst *mon;
struct monst* mattacker;
{
	if (!otmp || !mon)
		return 0;

    int tmp = 0, otyp = otmp->otyp;
    struct permonst *ptr = mon->data;
	boolean Is_weapon = is_weapon(otmp);
	boolean youdefend = (mon == &youmonst);

    if (otyp == CREAM_PIE)
        return 0;
	
	if(Is_weapon || objects[otyp].oc_class == GEM_CLASS)
	{
		if (
			(objects[otyp].oc_damagetype == AD_FIRE && (youdefend ? Fire_resistance : resists_fire(mon)))
			|| (objects[otyp].oc_damagetype == AD_COLD && (youdefend ? Cold_resistance : resists_cold(mon)))
			|| (objects[otyp].oc_damagetype == AD_ELEC && (youdefend ? Shock_resistance : resists_elec(mon)))
			)
			tmp += 0;
		else
		{ 
			if (bigmonst(ptr)) {
				if (objects[otyp].oc_wldam > 0 && objects[otyp].oc_wldice > 0)
					tmp += d(objects[otyp].oc_wldice, objects[otyp].oc_wldam);
				tmp += objects[otyp].oc_wldmgplus;
			}
			else {
				if (objects[otyp].oc_wsdam > 0 && objects[otyp].oc_wsdice > 0)
					tmp += d(objects[otyp].oc_wsdice, objects[otyp].oc_wsdam);
				tmp += objects[otyp].oc_wsdmgplus;
			}
			tmp += otmp->spe;
		}
        /* negative enchantment mustn't produce negative damage */
        if (tmp < 0)
            tmp = 0;

		/*
		if (eligible_for_extra_damage(otmp,mon,mattacker))
		{
			int extradmg = 0;
			if (objects[otyp].oc_wedam > 0 && objects[otyp].oc_wedice > 0)
				extradmg += d(objects[otyp].oc_wedice, objects[otyp].oc_wedam);
			extradmg += objects[otyp].oc_wedmgplus;
		
			if(objects[otyp].oc_flags3 & O3_DEALS_DOUBLE_DAMAGE_TO_PERMITTED_TARGETS)
				extradmg += tmp;

			tmp += extradmg;

		}
		*/

    }

    if (objects[otyp].oc_material <= MAT_LEATHER && thick_skinned(ptr))
        /* thick skinned/scaled creatures don't feel it */
        tmp = 0;
    if (ptr == &mons[PM_SHADE] && !shade_glare(otmp))
        tmp = 0;

    /* "very heavy iron ball"; weight increase is in increments */
    if (otyp == HEAVY_IRON_BALL && tmp > 0) {
        int wt = (int) objects[HEAVY_IRON_BALL].oc_weight;

        if ((int) otmp->owt > wt) {
            wt = ((int) otmp->owt - wt) / IRON_BALL_W_INCR;
            tmp += rnd(4 * wt);
            if (tmp > 25)
                tmp = 25; /* objects[].oc_wldam */
        }
    }

    /* Put weapon vs. monster type damage bonuses in below: */
    if (Is_weapon || otmp->oclass == GEM_CLASS || otmp->oclass == BALL_CLASS
        || otmp->oclass == CHAIN_CLASS) {
        int bonus = 0;

        if (otmp->blessed
            && (is_undead(ptr) || is_demon(ptr) || is_vampshifter(mon)))
            bonus += rnd(4);
        if (is_axe(otmp) && is_wooden(ptr))
            bonus += rnd(4);
        if (objects[otyp].oc_material == MAT_SILVER && mon_hates_silver(mon))
            bonus += rnd(20);
        if ((artifact_light(otmp) || (objects[otmp->otyp].oc_flags2 & O2_SHINES_MAGICAL_LIGHT)) && otmp->lamplit && hates_light(ptr))
            bonus += rnd(8);

        /* if the weapon is going to get a double damage bonus, adjust
           this bonus so that effectively it's added after the doubling */
        if (bonus > 1 && otmp->oartifact && spec_dbon(otmp, mon, 25) >= 25)
            bonus = (bonus + 1) / 2;

        tmp += bonus;
    }

    if (tmp > 0) {
        /* It's debatable whether a rusted blunt instrument
           should do less damage than a pristine one, since
           it will hit with essentially the same impact, but
           there ought to some penalty for using damaged gear
           so always subtract erosion even for blunt weapons. */
        tmp -= greatest_erosion(otmp);
        if (tmp < 1)
            tmp = 1;
    }

    return  tmp;
}


int
totaldmgval(otmp, mon, mattacker)
struct obj* otmp;
struct monst* mon;
struct monst* mattacker;
{
	int basedmg = dmgval(otmp, mon, mattacker);
	int edmg = extradmgval(otmp, mon, mattacker, basedmg);

	return basedmg + edmg;
}

int
extradmgval(otmp, mon, mattacker, basedmg)
struct obj* otmp;
struct monst* mon;
struct monst* mattacker;
int basedmg;
{
	if (!otmp || !mon)
		return 0;

	int tmp = 0, otyp = otmp->otyp;
	boolean youdefend = (mon == &youmonst);
	boolean extra_is_critical = FALSE;
	boolean criticalstrikesucceeded = FALSE;

	if ((objects[otmp->otyp].oc_aflags & AFLAGS_CRITICAL_STRIKE) && !(objects[otmp->otyp].oc_aflags & AFLAGS_CRITICAL_STRIKE_IS_DEADLY))
	{
		extra_is_critical = TRUE;
		if (
			((objects[otmp->otyp].oc_aflags & AFLAGS_CRITICAL_STRIKE_DISRESPECTS_TARGETS) || eligible_for_extra_damage(otmp, mon, mattacker))
			&& ((objects[otmp->otyp].oc_aflags & AFLAGS_CRITICAL_STRIKE_DISRESPECTS_CHARACTERS) || !inappropriate_monster_character_type(mattacker, otmp))
			)
		{
			if (rn2(100) < objects[otmp->otyp].oc_critical_strike_percentage)
				criticalstrikesucceeded = TRUE;
		}
	}

	if ((extra_is_critical && criticalstrikesucceeded) ||
		(!extra_is_critical && (
		((objects[otmp->otyp].oc_aflags & AFLAGS_EXTRA_DAMAGE_DISRESPECTS_TARGETS) || eligible_for_extra_damage(otmp, mon, mattacker))
			&& ((objects[otmp->otyp].oc_aflags & AFLAGS_EXTRA_DAMAGE_DISRESPECTS_CHARACTERS) || !inappropriate_monster_character_type(mattacker, otmp))
			)))
	{
		if (
			(objects[otyp].oc_extra_damagetype == AD_FIRE && (youdefend ? Fire_resistance : resists_fire(mon)))
			|| (objects[otyp].oc_extra_damagetype == AD_COLD && (youdefend ? Cold_resistance : resists_cold(mon)))
			|| (objects[otyp].oc_extra_damagetype == AD_ELEC && (youdefend ? Shock_resistance : resists_elec(mon)))
			)
			tmp += 0;
		else
		{

			int extradmg = 0;
			if (objects[otyp].oc_wedam > 0 && objects[otyp].oc_wedice > 0)
				extradmg += d(objects[otyp].oc_wedice, objects[otyp].oc_wedam);
			extradmg += objects[otyp].oc_wedmgplus;

			tmp += extradmg;
		}
	}

	/* Double damage uses the main damage type */
	if (
		(objects[otyp].oc_damagetype == AD_FIRE && (youdefend ? Fire_resistance : resists_fire(mon)))
		|| (objects[otyp].oc_damagetype == AD_COLD && (youdefend ? Cold_resistance : resists_cold(mon)))
		|| (objects[otyp].oc_damagetype == AD_ELEC && (youdefend ? Shock_resistance : resists_elec(mon)))
		)
		tmp += 0;
	else
	{
		if ((objects[otyp].oc_aflags & AFLAGS_DEALS_DOUBLE_DAMAGE_TO_PERMITTED_TARGETS) && eligible_for_extra_damage(otmp, mon, mattacker))
			tmp += basedmg;
	}

	if (tmp < 0)
		tmp = 0;

	return tmp;
}

boolean
eligible_for_extra_damage(otmp, mon, mattacker)
struct obj* otmp;
struct monst* mon;
struct monst* mattacker;
{
	int otyp = otmp->otyp;
	struct permonst* ptr = mon->data;

	if ((!mattacker ? (objects[otyp].oc_power_permissions == PERMITTED_ALL) : !inappropriate_monster_character_type(mattacker, otmp))
		&& (objects[otyp].oc_target_permissions == ALL_TARGETS && ((objects[otyp].oc_flags3 & (O3_PERMTTED_TARGET_CHAOTIC | O3_PERMTTED_TARGET_NEUTRAL | O3_PERMTTED_TARGET_LAWFUL)) == 0) || (
		((objects[otyp].oc_flags3 & O3_TARGET_PERMISSION_IS_M1_FLAG) && (ptr->mflags1 & objects[otyp].oc_target_permissions))
			|| ((objects[otyp].oc_flags3 & O3_TARGET_PERMISSION_IS_M2_FLAG) && (ptr->mflags2 & objects[otyp].oc_target_permissions))
			|| ((objects[otyp].oc_flags3 & O3_TARGET_PERMISSION_IS_M3_FLAG) && (ptr->mflags3 & objects[otyp].oc_target_permissions))
			|| (((objects[otyp].oc_flags3 & (O3_TARGET_PERMISSION_IS_M1_FLAG | O3_TARGET_PERMISSION_IS_M2_FLAG | O3_TARGET_PERMISSION_IS_M3_FLAG)) == 0) && (ptr->mlet == objects[otyp].oc_target_permissions))
			|| ((objects[otyp].oc_flags3 & O3_PERMTTED_TARGET_CHAOTIC) && mon->malign < 0)
			|| ((objects[otyp].oc_flags3 & O3_PERMTTED_TARGET_NEUTRAL) && mon->malign == 0)
			|| ((objects[otyp].oc_flags3 & O3_PERMTTED_TARGET_LAWFUL) && mon->malign > 0)
			)
			))
		return TRUE;
	else
		return FALSE;
}

/* check whether blessed and/or silver damage applies for *non-weapon* hit;
   return value is the amount of the extra damage */
int
special_dmgval(magr, mdef, armask, silverhit_p)
struct monst *magr, *mdef;
long armask; /* armor mask, multiple bits accepted for W_ARMC|W_ARM|W_ARMU
              * or W_ARMG|W_RINGL|W_RINGR only */
long *silverhit_p; /* output flag mask for silver bonus */
{
    struct obj *obj;
    struct permonst *ptr = mdef->data;
    boolean left_ring = (armask & W_RINGL) ? TRUE : FALSE,
            right_ring = (armask & W_RINGR) ? TRUE : FALSE;
    long silverhit = 0L;
    int bonus = 0;

    obj = 0;
    if (armask & (W_ARMC | W_ARM | W_ARMU)) {
        if ((armask & W_ARMC) != 0L
            && (obj = which_armor(magr, W_ARMC)) != 0)
            armask = W_ARMC;
        else if ((armask & W_ARM) != 0L
                 && (obj = which_armor(magr, W_ARM)) != 0)
            armask = W_ARM;
        else if ((armask & W_ARMU) != 0L
                 && (obj = which_armor(magr, W_ARMU)) != 0)
            armask = W_ARMU;
        else
            armask = 0L;
    } else if (armask & (W_ARMG | W_RINGL | W_RINGR)) {
        armask = ((obj = which_armor(magr, W_ARMG)) != 0) ?  W_ARMG : 0L;
    } else {
        obj = which_armor(magr, armask);
    }

    if (obj) {
        if (obj->blessed
            && (is_undead(ptr) || is_demon(ptr) || is_vampshifter(mdef)))
            bonus += rnd(4);
        /* the only silver armor is shield of reflection (silver dragon
           scales refer to color, not material) and the only way to hit
           with one--aside from throwing--is to wield it and perform a
           weapon hit, but we include a general check here */
        if (objects[obj->otyp].oc_material == MAT_SILVER
            && mon_hates_silver(mdef)) {
            bonus += rnd(20);
            silverhit |= armask;
        }

    /* when no gloves we check for silver rings (blessed rings ignored) */
    } else if ((left_ring || right_ring) && magr == &youmonst) {
        if (left_ring && uleft) {
            if (objects[uleft->otyp].oc_material == MAT_SILVER
                && mon_hates_silver(mdef)) {
                bonus += rnd(20);
                silverhit |= W_RINGL;
            }
        }
        if (right_ring && uright) {
            if (objects[uright->otyp].oc_material == MAT_SILVER
                && mon_hates_silver(mdef)) {
                /* two silver rings don't give double silver damage
                   but 'silverhit' messages might be adjusted for them */
                if (!(silverhit & W_RINGL))
                    bonus += rnd(20);
                silverhit |= W_RINGR;
            }
        }
    }

    if (silverhit_p)
        *silverhit_p = silverhit;
    return bonus;
}

/* give a "silver <item> sears <target>" message;
   not used for weapon hit, so we only handle rings */
void
silver_sears(magr, mdef, silverhit)
struct monst *magr UNUSED;
struct monst *mdef;
long silverhit;
{
    char rings[20]; /* plenty of room for "rings" */
    int ltyp = ((uleft && (silverhit & W_RINGL) != 0L)
                ? uleft->otyp : STRANGE_OBJECT),
        rtyp = ((uright && (silverhit & W_RINGR) != 0L)
                ? uright->otyp : STRANGE_OBJECT);
    boolean both,
        l_ag = (objects[ltyp].oc_material == MAT_SILVER && uleft->dknown),
        r_ag = (objects[rtyp].oc_material == MAT_SILVER && uright->dknown);

    if ((silverhit & (W_RINGL | W_RINGR)) != 0L) {
        /* plural if both the same type (so not multi_claw and both rings
           are non-Null) and either both known or neither known, or both
           silver (in case there is ever more than one type of silver ring)
           and both known; singular if multi_claw (where one of ltyp or
           rtyp will always be STRANGE_OBJECT) even if both rings are known
           silver [see hmonas(uhitm.c) for explanation of 'multi_claw'] */
        both = ((ltyp == rtyp && uleft->dknown == uright->dknown)
                || (l_ag && r_ag));
        Sprintf(rings, "ring%s", both ? "s" : "");
        Your("%s%s %s %s!",
             (l_ag || r_ag) ? "silver "
             : both ? ""
               : ((silverhit & W_RINGL) != 0L) ? "left "
                 : "right ",
             rings, vtense(rings, "sear"), mon_nam(mdef));
    }
}

STATIC_DCL struct obj *FDECL(oselect, (struct monst *, int));
#define Oselect(x)                      \
    if ((otmp = oselect(mtmp, x)) != 0) \
        return otmp;

STATIC_OVL struct obj *
oselect(mtmp, x)
struct monst *mtmp;
int x;
{
    struct obj *otmp;

    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
        if (otmp->otyp == x
            /* never select non-cockatrice corpses */
            && !((x == CORPSE || x == EGG)
                 && !touch_petrifies(&mons[otmp->corpsenm]))
            && (!otmp->oartifact || touch_artifact(otmp, mtmp)))
            return otmp;
    }
    return (struct obj *) 0;
}

/* TODO: have monsters use aklys' throw-and-return */
static NEARDATA const int rwep[] = {
    DWARVISH_SPEAR, SILVER_SPEAR, ELVEN_SPEAR, SPEAR, ORCISH_SPEAR, JAVELIN,
    SHURIKEN, YA, SILVER_ARROW, ELVEN_ARROW, ARROW, ORCISH_ARROW,
    CROSSBOW_BOLT, SILVER_DAGGER, ELVEN_DAGGER, DAGGER, ORCISH_DAGGER, KNIFE,
    FLINT, ROCK, STONE_PEBBLE, CLAY_PEBBLE, LOADSTONE, LUCKSTONE, DART,
    /* BOOMERANG, */ CREAM_PIE
};

static NEARDATA const int pwep[] = { HALBERD,       BARDICHE, SPETUM,
                                     BILL_GUISARME, VOULGE,   RANSEUR,
                                     GUISARME,      GLAIVE,   LUCERN_HAMMER,
                                     BEC_DE_CORBIN, FAUCHARD, PARTISAN,
                                     LANCE };

static struct obj *propellor;

/* select a ranged weapon for the monster */
struct obj *
select_rwep(mtmp)
register struct monst *mtmp;
{
    register struct obj *otmp;
    struct obj *mwep;
    boolean mweponly;
    int i;

    char mlet = mtmp->data->mlet;

    propellor = (struct obj *) &zeroobj;
    Oselect(EGG);      /* cockatrice egg */
    if (mlet == S_KOP) /* pies are first choice for Kops */
        Oselect(CREAM_PIE);
    if (throws_rocks(mtmp->data)) /* ...boulders for giants */
        Oselect(BOULDER);

    /* Select polearms first; they do more damage and aren't expendable.
       But don't pick one if monster's weapon is welded, because then
       we'd never have a chance to throw non-wielding missiles. */
    /* The limit of 13 here is based on the monster polearm range limit
     * (defined as 5 in mthrowu.c).  5 corresponds to a distance of 2 in
     * one direction and 1 in another; one space beyond that would be 3 in
     * one direction and 2 in another; 3^2+2^2=13.
     */
    mwep = MON_WEP(mtmp);
    /* NO_WEAPON_WANTED means we already tried to wield and failed */
    mweponly = (mwelded(mwep) && mtmp->weapon_check == NO_WEAPON_WANTED);
    if (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 13
        && couldsee(mtmp->mx, mtmp->my)) {
        for (i = 0; i < SIZE(pwep); i++) {
            /* Only strong monsters can wield big (esp. long) weapons.
             * Big weapon is basically the same as bimanual.
             * All monsters can wield the remaining weapons.
             */
            if ((((strongmonst(mtmp->data) || mtmp->data->str >= 14)
                  && (mtmp->misc_worn_check & W_ARMS) == 0)
                 || !objects[pwep[i]].oc_bimanual)
                && (objects[pwep[i]].oc_material != MAT_SILVER
                    || !mon_hates_silver(mtmp))) {
                if ((otmp = oselect(mtmp, pwep[i])) != 0
                    && (otmp == mwep || !mweponly)) {
                    propellor = otmp; /* force the monster to wield it */
                    return otmp;
                }
            }
        }
    }

    /*
     * other than these two specific cases, always select the
     * most potent ranged weapon to hand.
     */
    for (i = 0; i < SIZE(rwep); i++) {
        int prop;

        /* shooting gems from slings; this goes just before the darts */
        /* (shooting rocks is already handled via the rwep[] ordering) */
        if (rwep[i] == DART && !likes_gems(mtmp->data)
            && m_carrying(mtmp, SLING)) { /* propellor */
            for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
                if (otmp->oclass == GEM_CLASS
                    && (!(objects[otmp->otyp].oc_flags & O1_CANNOT_BE_DROPPED_IF_CURSED) || !otmp->cursed)) {
                    propellor = m_carrying(mtmp, SLING);
                    return otmp;
                }
        }

        /* KMH -- This belongs here so darts will work */
        propellor = (struct obj *) &zeroobj;

        prop = objects[rwep[i]].oc_skill;
        if (prop < 0) {
            switch (-prop) {
            case P_BOW:
                propellor = oselect(mtmp, YUMI);
				if (!propellor)
					propellor = oselect(mtmp, COMPOSITE_LONG_BOW);
				if (!propellor)
                    propellor = oselect(mtmp, ELVEN_LONG_BOW);
				if (!propellor)
					propellor = oselect(mtmp, LONG_BOW);
				if (!propellor)
					propellor = oselect(mtmp, COMPOSITE_SHORT_BOW);
				if (!propellor)
                    propellor = oselect(mtmp, SHORT_BOW);
                if (!propellor)
                    propellor = oselect(mtmp, ORCISH_SHORT_BOW);
                break;
            case P_SLING:
                propellor = oselect(mtmp, SLING);
                break;
            case P_CROSSBOW:
				propellor = oselect(mtmp, HEAVY_CROSSBOW);
				if (!propellor)
					propellor = oselect(mtmp, CROSSBOW);
				if (!propellor)
					propellor = oselect(mtmp, HAND_CROSSBOW);
			}
            if ((otmp = MON_WEP(mtmp)) && mwelded(otmp) && otmp != propellor
                && mtmp->weapon_check == NO_WEAPON_WANTED)
                propellor = 0;
        }
        /* propellor = obj, propellor to use
         * propellor = &zeroobj, doesn't need a propellor
         * propellor = 0, needed one and didn't have one
         */
        if (propellor != 0) {
            /* Note: cannot use m_carrying for loadstones, since it will
             * always select the first object of a type, and maybe the
             * monster is carrying two but only the first is unthrowable.
             */
            if (!(objects[rwep[i]].oc_flags & O1_CANNOT_BE_DROPPED_IF_CURSED)) {
                /* Don't throw a cursed weapon-in-hand or an artifact */
                if ((otmp = oselect(mtmp, rwep[i])) && !otmp->oartifact
                    && !(otmp == MON_WEP(mtmp) && mwelded(otmp)))
                    return otmp;
            } else
                for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
                    if ((objects[otmp->otyp].oc_flags & O1_CANNOT_BE_DROPPED_IF_CURSED) && !otmp->cursed)
                        return otmp;
                }
        }
    }

    /* failure */
    return (struct obj *) 0;
}

/* is 'obj' a type of weapon that any monster knows how to throw? */
boolean
monmightthrowwep(obj)
struct obj *obj;
{
    short idx;

    for (idx = 0; idx < SIZE(rwep); ++idx)
        if (obj->otyp == rwep[idx])
            return TRUE;
    return FALSE;
}

/* Weapons in order of preference */
static const NEARDATA short hwep[] = 
{
	BLACK_BLADE_OF_DISINTEGRATION, GLASS_SWORD, VORPAL_SWORD, BATTLE_AXE_OF_CLEAVING, SWORD_OF_SHARPNESS, MORGUL_BLADE,
    TSURUGI, RUNESWORD, SWORD_OF_HOLY_VENGEANCE, SWORD_OF_UNHOLY_DESECRATION, SWORD_OF_DEFENSE, SWORD_OF_WOUNDING, SWORD_OF_LIFE_DRAINING,
	SWORD_OF_DRAGON_SLAYING, SWORD_OF_GIANT_SLAYING, SWORD_OF_TROLL_SLAYING, SWORD_OF_ORC_SLAYING, MACE_OF_DISRUPTION,
	CORPSE, /* cockatrice corpse */
	TRIPLE_HEADED_FLAIL, DWARVISH_MATTOCK, TWO_HANDED_SWORD, BATTLE_AXE,
    KATANA, DOUBLE_HEADED_FLAIL, UNICORN_HORN, CRYSKNIFE, TRIDENT, SILVER_LONG_SWORD, LONG_SWORD, ELVEN_BROADSWORD,
    BROADSWORD, SCIMITAR, SILVER_SABER, INFERNAL_JAGGED_TOOTHED_CLUB, INFERNAL_ANCUS, MORNING_STAR, ELVEN_SHORT_SWORD,
    DWARVISH_SHORT_SWORD, SHORT_SWORD, ORCISH_SHORT_SWORD, SILVER_MACE, MACE, AXE,
    DWARVISH_SPEAR, SILVER_SPEAR, ELVEN_SPEAR, SPEAR, ORCISH_SPEAR, FLAIL,
    BULLWHIP, QUARTERSTAFF, JAVELIN_OF_RETURNING, JAVELIN, AKLYS, CLUB, PICK_AXE, RUBBER_HOSE,
    WAR_HAMMER, SILVER_DAGGER, ELVEN_DAGGER, DAGGER, ORCISH_DAGGER, ATHAME,
    SCALPEL, KNIFE, WORM_TOOTH
};

/* select a hand to hand weapon for the monster */
struct obj *
select_hwep(mtmp)
register struct monst *mtmp;
{
    register struct obj *otmp;
    register int i;
    boolean strong = (strongmonst(mtmp->data) || mtmp->data->str >= 13);
    boolean wearing_shield = (mtmp->misc_worn_check & W_ARMS) != 0;

    /* prefer artifacts to everything else */
    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
        if (otmp->oclass == WEAPON_CLASS && otmp->oartifact
            && touch_artifact(otmp, mtmp)
            && ((strong && !wearing_shield)
                || !objects[otmp->otyp].oc_bimanual))
            return otmp;
    }

    if (is_giant(mtmp->data)) /* giants just love to use clubs */
        Oselect(CLUB);

    /* only strong monsters can wield big (esp. long) weapons */
    /* big weapon is basically the same as bimanual */
    /* all monsters can wield the remaining weapons */
    for (i = 0; i < SIZE(hwep); i++) {
        if (hwep[i] == CORPSE && !(mtmp->misc_worn_check & W_ARMG)
            && !resists_ston(mtmp))
            continue;
        if (((strong && !wearing_shield) || !objects[hwep[i]].oc_bimanual)
            && (objects[hwep[i]].oc_material != MAT_SILVER
                || !mon_hates_silver(mtmp)))
            Oselect(hwep[i]);
    }

    /* failure */
    return (struct obj *) 0;
}


/* select a nth suitable hand to hand weapon for the marilith */
struct obj*
select_multiweapon_nth_hwep(mtmp, handindex)
register struct monst* mtmp;
int handindex;
{
	register struct obj* otmp;
	boolean strong = (strongmonst(mtmp->data) || mtmp->data->str >= 13);
	boolean wearing_shield = (mtmp->misc_worn_check & W_ARMS) != 0;
	int weaponindex = 1; //Start with second hand, if free

	/* If the monster has not wielded a weapon, then all weapons are unwielded */
	if(!mtmp || !MON_WEP(mtmp))
		return (struct obj*) 0;

	if ((handindex == 1 || (handindex ==2 && (objects[MON_WEP(mtmp)->otyp].oc_bimanual || wearing_shield))))
		return MON_WEP(mtmp);

	//Never select MON_WEP otherwise select weaponindex'th first suitable weapon, if none, then return 0
	if (MON_WEP(mtmp) && objects[MON_WEP(mtmp)->otyp].oc_bimanual || wearing_shield)
		weaponindex++; //Second hand is not free, previously returned MON_WEP

	//Is in hwep table, extra hands do not use two-handed weapons for simplicity (maybe too weak)
	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
		if (otmp != MON_WEP(mtmp) && !objects[otmp->otyp].oc_bimanual
			&& !(objects[otmp->otyp].oc_material == MAT_SILVER && mon_hates_silver(mtmp)) && otmp->otyp != CORPSE)
		{
			//Suitable weapons are in hwep array
			for (int i = 0; i < SIZE(hwep); i++) {
				if (otmp->otyp == hwep[i])
				{
					//Suitable weapon found
					weaponindex++;
					if (weaponindex == handindex)
						return otmp;

				}
			}
		}
	}
	/* failure, no weapons left for extra hands */
	return (struct obj*) 0;
}

/* Called after polymorphing a monster, robbing it, etc....  Monsters
 * otherwise never unwield stuff on their own.  Might print message.
 */
void
possibly_unwield(mon, polyspot)
struct monst *mon;
boolean polyspot;
{
    struct obj *obj, *mw_tmp;

    if (!(mw_tmp = MON_WEP(mon)))
        return;
    for (obj = mon->minvent; obj; obj = obj->nobj)
        if (obj == mw_tmp)
            break;
    if (!obj) { /* The weapon was stolen or destroyed */
        MON_NOWEP(mon);
        mon->weapon_check = NEED_WEAPON;
        return;
    }
    if (!attacktype(mon->data, AT_WEAP)) {
        setmnotwielded(mon, mw_tmp);
        mon->weapon_check = NO_WEAPON_WANTED;
        obj_extract_self(obj);
        if (cansee(mon->mx, mon->my)) {
            pline("%s drops %s.", Monnam(mon), distant_name(obj, doname));
            newsym(mon->mx, mon->my);
        }
        /* might be dropping object into water or lava */
        if (!flooreffects(obj, mon->mx, mon->my, "drop")) {
            if (polyspot)
                bypass_obj(obj);
            place_object(obj, mon->mx, mon->my);
            stackobj(obj);
        }
        return;
    }
    /* The remaining case where there is a change is where a monster
     * is polymorphed into a stronger/weaker monster with a different
     * choice of weapons.  This has no parallel for players.  It can
     * be handled by waiting until mon_wield_item is actually called.
     * Though the monster still wields the wrong weapon until then,
     * this is OK since the player can't see it.  (FIXME: Not okay since
     * probing can reveal it.)
     * Note that if there is no change, setting the check to NEED_WEAPON
     * is harmless.
     * Possible problem: big monster with big cursed weapon gets
     * polymorphed into little monster.  But it's not quite clear how to
     * handle this anyway....
     */
    if (!(mwelded(mw_tmp) && mon->weapon_check == NO_WEAPON_WANTED))
        mon->weapon_check = NEED_WEAPON;
    return;
}

/* Let a monster try to wield a weapon, based on mon->weapon_check.
 * Returns 1 if the monster took time to do it, 0 if it did not.
 */
int
mon_wield_item(mon)
register struct monst *mon;
{
    struct obj *obj;

    /* This case actually should never happen */
    if (mon->weapon_check == NO_WEAPON_WANTED)
        return 0;
    switch (mon->weapon_check) {
    case NEED_HTH_WEAPON:
        obj = select_hwep(mon);
        break;
    case NEED_RANGED_WEAPON:
        (void) select_rwep(mon);
        obj = propellor;
        break;
    case NEED_PICK_AXE:
        obj = m_carrying(mon, PICK_AXE);
        /* KMH -- allow other picks */
        if (!obj && !which_armor(mon, W_ARMS))
            obj = m_carrying(mon, DWARVISH_MATTOCK);
        break;
    case NEED_AXE:
        /* currently, only 2 types of axe */
        obj = m_carrying(mon, BATTLE_AXE);
        if (!obj || which_armor(mon, W_ARMS))
            obj = m_carrying(mon, AXE);
        break;
    case NEED_PICK_OR_AXE:
        /* prefer pick for fewer switches on most levels */
        obj = m_carrying(mon, DWARVISH_MATTOCK);
        if (!obj)
            obj = m_carrying(mon, BATTLE_AXE);
        if (!obj || which_armor(mon, W_ARMS)) {
            obj = m_carrying(mon, PICK_AXE);
            if (!obj)
                obj = m_carrying(mon, AXE);
        }
        break;
    default:
        impossible("weapon_check %d for %s?", mon->weapon_check,
                   mon_nam(mon));
        return 0;
    }
    if (obj && obj != &zeroobj) {
        struct obj *mw_tmp = MON_WEP(mon);

        if (mw_tmp && mw_tmp->otyp == obj->otyp) {
            /* already wielding it */
            mon->weapon_check = NEED_WEAPON;
            return 0;
        }
        /* Actually, this isn't necessary--as soon as the monster
         * wields the weapon, the weapon welds itself, so the monster
         * can know it's cursed and needn't even bother trying.
         * Still....
         */
        if (mw_tmp && mwelded(mw_tmp)) {
            if (canseemon(mon)) {
                char welded_buf[BUFSZ];
                const char *mon_hand = mbodypart(mon, HAND);

                if (bimanual(mw_tmp))
                    mon_hand = makeplural(mon_hand);
                Sprintf(welded_buf, "%s welded to %s %s",
                        otense(mw_tmp, "are"), mhis(mon), mon_hand);

                if (obj->otyp == PICK_AXE) {
                    pline("Since %s weapon%s %s,", s_suffix(mon_nam(mon)),
                          plur(mw_tmp->quan), welded_buf);
                    pline("%s cannot wield that %s.", mon_nam(mon),
                          xname(obj));
                } else {
                    pline("%s tries to wield %s.", Monnam(mon), doname(obj));
                    pline("%s %s!", Yname2(mw_tmp), welded_buf);
                }
                mw_tmp->bknown = 1;
            }
            mon->weapon_check = NO_WEAPON_WANTED;
            return 1;
        }
        mon->mw = obj; /* wield obj */
        setmnotwielded(mon, mw_tmp);
        mon->weapon_check = NEED_WEAPON;
        if (canseemon(mon)) {
            pline("%s wields %s!", Monnam(mon), doname(obj));
            if (mwelded(mw_tmp)) {
                pline("%s %s to %s %s!", Tobjnam(obj, "weld"),
                      is_plural(obj) ? "themselves" : "itself",
                      s_suffix(mon_nam(mon)), mbodypart(mon, HAND));
                obj->bknown = 1;
            }
        }
        if (obj && (artifact_light(obj) || ((objects[obj->otyp].oc_flags2 & O2_SHINES_MAGICAL_LIGHT) && !inappropriate_monster_character_type(mon, obj))) && !obj->lamplit) {
            begin_burn(obj, FALSE);
            if (canseemon(mon))
                pline("%s %s in %s %s!", Tobjnam(obj, "shine"),
                      arti_light_description(obj), s_suffix(mon_nam(mon)),
                      mbodypart(mon, HAND));
        }
        obj->owornmask = W_WEP;
        return 1;
    }
    mon->weapon_check = NEED_WEAPON;
    return 0;
}

/* force monster to stop wielding current weapon, if any */
void
mwepgone(mon)
struct monst *mon;
{
    struct obj *mwep = MON_WEP(mon);

    if (mwep) {
        setmnotwielded(mon, mwep);
        mon->weapon_check = NEED_WEAPON;
    }
}

/* attack bonus for strength & dexterity */
int
abon()
{
    int sbon = 0;
    int str = ACURR(A_STR), dex = ACURR(A_DEX);

//    if (Upolyd)
//        return (adj_lev(&mons[u.umonnum]) - 3);

	sbon += strength_tohit_bonus(str);
	sbon += dexterity_tohit_bonus(dex);

    /* Game tuning kludge: make it a bit easier for a low level character to
     * hit */
    sbon += (u.ulevel < 3) ? 1 : 0;
	return sbon;

}

/* attack bonus for dexterity only for ranged*/
int
ranged_abon()
{
	int sbon = 0;
	int str = ACURR(A_STR), dex = ACURR(A_DEX);

	//    if (Upolyd)
	//        return (adj_lev(&mons[u.umonnum]) - 3);

	sbon += dexterity_tohit_bonus(dex);

	/* Game tuning kludge: make it a bit easier for a low level character to
	 * hit */
	sbon += (u.ulevel < 3) ? 1 : 0;
	return sbon;
}


int strength_tohit_bonus(str)
int str;
{
	int sbon = 0;
	if (str < 6)
		sbon = -2;
	else if (str < 8)
		sbon = -1;
	else if (str < 17)
		sbon = 0;
	else if (str <= STR18(50))
		sbon = 1; /* up to 18/50 */
	else if (str < STR18(100))
		sbon = 2;
	else if (str == STR18(100))
		sbon = 3;
	else if (str == STR19(19))
		sbon = 3;
	else if (str == STR19(20))
		sbon = 4;
	else if (str == STR19(21))
		sbon = 4;
	else if (str == STR19(22))
		sbon = 5;
	else if (str == STR19(23))
		sbon = 5;
	else if (str == STR19(24))
		sbon = 6;
	else
		sbon = 6;

	return sbon;
}

/* damage bonus for strength */
int
dbon()
{
    int str = ACURR(A_STR);

	return strength_damage_bonus(str);
}

/* damage bonus for strength for thrown weapons (bows get full strength)*/
int
tdbon()
{
	return dbon() / 2;
}


int
strength_damage_bonus(str)
int str;
{
	if (str < 6)
		return -1;
	else if (str < 16)
		return 0;
	else if (str < 18)
		return 1;
	else if (str == 18)
		return 2; /* up to 18 */
	else if (str <= STR18(75))
		return 3; /* up to 18/75 */
	else if (str <= STR18(90))
		return 4; /* up to 18/90 */
	else if (str < STR18(100))
		return 5; /* up to 18/99 */
	else if (str == STR18(100))
		return 6; /* up to 18/00 */
	else if (str == STR19(19))
		return 7; /* up to 19 */
	else if (str == STR19(20))
		return 8; /* up to 20 */
	else if (str == STR19(21))
		return 9; /* up to 21 */
	else if (str == STR19(22))
		return 10; /* up to 22 */
	else if (str == STR19(23))
		return 11; /* up to 23 */
	else if (str == STR19(24))
		return 12; /* up to 24 */
	else
		return 13;

}
/* monster damage bonus for strength*/
int
mdbon(mon)
struct monst* mon;
{
	int bonus = 0;

	if(mon)
	{
		bonus += strength_damage_bonus(monster_current_str(mon));
	}

	return bonus;

}

/* monster damage bonus for strength for throw weapons (bows get full damage bonus)*/
int
mtdbon(mon)
struct monst* mon;
{
	int bonus = 0;

	if (mon)
	{
		bonus += mdbon(mon) / 2;
	}

	return bonus;

}

int
monster_current_str(mon)
struct monst* mon;
{
	int currstr = 0;
	struct obj* marmg;

	if (!mon)
		return 0;

	if ((marmg = which_armor(mon, W_ARMG)) != 0
		&& marmg->otyp == GAUNTLETS_OF_OGRE_POWER)
		currstr = STR18(100);
	else
		currstr = mon->mstr;

	if (currstr > STR19(25))
		currstr = STR19(25);

	return currstr;
}

int
monster_current_dex(mon)
struct monst* mon;
{
	int currdex = 0;
	struct obj* marmg;

	if (!mon)
		return 0;

	currdex = mon->mdex;

	if ((marmg = which_armor(mon, W_ARMG)) != 0
		&& marmg->otyp == GAUNTLETS_OF_DEXTERITY)
		currdex += marmg->spe;

	if (currdex > 25)
		currdex = 25;

	return currdex;
}

int
monster_current_con(mon)
struct monst* mon;
{
	int currcon = 0;

	if (!mon)
		return 0;

	currcon = mon->mcon;

	if (currcon > 25)
		currcon = 25;

	return currcon;
}



/* monster to hit bonus for strength and dex*/
int
mabon(mon)
struct monst* mon;
{
	int bonus = 0;

	if (mon)
	{
		bonus += strength_tohit_bonus(monster_current_str(mon));
		bonus += dexterity_tohit_bonus(monster_current_dex(mon));
	}
	return bonus;

}

/* monster to hit bonus for dex only for ranged*/
int
mrabon(mon)
struct monst* mon;
{
	int bonus = 0;

	if (mon)
	{
		bonus += dexterity_tohit_bonus(monster_current_dex(mon));
	}
	return bonus;

}

int
dexterity_ac_bonus(dex)
int dex;
{
	if (dex < 1)
		return -5;
	else if (dex < 7)
		return -7 + dex;
	else if (dex < 14)
		return 0;
	else if (dex <= 25)
		return dex - 14;
	else
		return 11;
}

int dexterity_tohit_bonus(dex)
int dex;
{
	int sbon = 0;

	if (dex < 4)
		sbon = -3;
	else if (dex < 6)
		sbon = -2;
	else if (dex < 8)
		sbon = -1;
	else if (dex < 14)
		sbon = 0;
	else if (dex == 14)
		sbon = 1;
	else if (dex <= 25)
		sbon = (dex - 13) / 2;
	else
		sbon = 6;

	return sbon;
}

int constitution_hp_bonus(con)
int con;
{
	int sbon = 0;

	if (con <= 3)
		sbon = -2;
	else if (con <= 6)
		sbon = -1;
	else if (con < 15)
		sbon = 0;
	else if (con <= 25)
		sbon = (con - 14);
	else
		sbon = 11;

	return sbon;
}


/* increase a towel's wetness */
void
wet_a_towel(obj, amt, verbose)
struct obj *obj;
int amt; /* positive: new value; negative: increment by -amt; zero: no-op */
boolean verbose;
{
    int newspe = (amt <= 0) ? obj->spe - amt : amt;

    /* new state is only reported if it's an increase */
    if (newspe > obj->spe) {
        if (verbose) {
            const char *wetness = (newspe < 3)
                                     ? (!obj->spe ? "damp" : "damper")
                                     : (!obj->spe ? "wet" : "wetter");

            if (carried(obj))
                pline("%s gets %s.", Yobjnam2(obj, (const char *) 0),
                      wetness);
            else if (mcarried(obj) && canseemon(obj->ocarry))
                pline("%s %s gets %s.", s_suffix(Monnam(obj->ocarry)),
                      xname(obj), wetness);
        }
    }
    obj->spe = min(newspe, 7);

    /* if hero is wielding this towel, don't give "you begin bashing
       with your wet towel" message on next attack with it */
	update_unweapon();
	/*
    if (obj == uwep)
        unweapon = !is_wet_towel(obj);
	*/
}

/* decrease a towel's wetness */
void
dry_a_towel(obj, amt, verbose)
struct obj *obj;
int amt; /* positive: new value; negative: decrement by -amt; zero: no-op */
boolean verbose;
{
    int newspe = (amt <= 0) ? obj->spe + amt : amt;

    /* new state is only reported if it's a decrease */
    if (newspe < obj->spe) {
        if (verbose) {
            if (carried(obj))
                pline("%s dries%s.", Yobjnam2(obj, (const char *) 0),
                      !newspe ? " out" : "");
            else if (mcarried(obj) && canseemon(obj->ocarry))
                pline("%s %s drie%s.", s_suffix(Monnam(obj->ocarry)),
                      xname(obj), !newspe ? " out" : "");
        }
    }
    newspe = min(newspe, 7);
    obj->spe = max(newspe, 0);

    /* if hero is wielding this towel and it is now dry, give "you begin
       bashing with your towel" message on next attack with it */
	update_unweapon();
	/*
    if (obj == uwep)
        unweapon = !is_wet_towel(obj);
	*/
}

/* copy the skill level name into the given buffer */
char *
skill_level_name(skill, buf, ismax)
int skill;
char *buf;
boolean ismax;
{
    const char *ptr;

    switch (ismax ? P_MAX_SKILL(skill) : P_SKILL(skill)) {
    case P_UNSKILLED:
        ptr = "Unskilled";
        break;
    case P_BASIC:
        ptr = "Basic";
        break;
    case P_SKILLED:
        ptr = "Skilled";
        break;
    case P_EXPERT:
        ptr = "Expert";
        break;
    /* these are for unarmed combat/martial arts only */
    case P_MASTER:
        ptr = "Master";
        break;
    case P_GRAND_MASTER:
        ptr = "Grand Master";
        break;
    default:
        ptr = "Unknown";
        break;
    }
    Strcpy(buf, ptr);
    return buf;
}

const char *
skill_name(skill)
int skill;
{
    return P_NAME(skill);
}

/* return the # of slots required to advance the skill */
STATIC_OVL int
slots_required(skill)
int skill;
{
    int tmp = P_SKILL(skill);

    /* The more difficult the training, the more slots it takes.
     *  unskilled -> basic      1
     *  basic -> skilled        2
     *  skilled -> expert       3
     */
    if (skill <= P_LAST_WEAPON || skill == P_TWO_WEAPON_COMBAT)
        return tmp;

    /* Fewer slots used up for unarmed or martial.
     *  unskilled -> basic      1
     *  basic -> skilled        1
     *  skilled -> expert       2
     *  expert -> master        2
     *  master -> grand master  3
     */
    return (tmp + 1) / 2;
}

/* return true if this skill can be advanced */
boolean
can_advance(skill, speedy)
int skill;
boolean speedy;
{
    if (P_RESTRICTED(skill)
        || P_SKILL(skill) >= P_MAX_SKILL(skill)
        || u.skills_advanced >= P_SKILL_LIMIT)
        return FALSE;

    if (wizard && speedy)
        return TRUE;

    return (boolean) ((int) P_ADVANCE(skill)
                      >= practice_needed_to_advance(skill, P_SKILL(skill))
                      && u.weapon_slots >= slots_required(skill));
}

/* return true if this skill could be advanced if more slots were available */
STATIC_OVL boolean
could_advance(skill)
int skill;
{
    if (P_RESTRICTED(skill)
        || P_SKILL(skill) >= P_MAX_SKILL(skill)
        || u.skills_advanced >= P_SKILL_LIMIT)
        return FALSE;

    return (boolean) ((int) P_ADVANCE(skill)
                      >= practice_needed_to_advance(skill, P_SKILL(skill)));
}

/* return true if this skill has reached its maximum and there's been enough
   practice to become eligible for the next step if that had been possible */
STATIC_OVL boolean
peaked_skill(skill)
int skill;
{
    if (P_RESTRICTED(skill))
        return FALSE;

    return (boolean) (P_SKILL(skill) >= P_MAX_SKILL(skill)
                      && ((int) P_ADVANCE(skill)
                          >= practice_needed_to_advance(skill, P_SKILL(skill))));
}

STATIC_OVL void
skill_advance(skill)
int skill;
{
    u.weapon_slots -= slots_required(skill);
    P_SKILL(skill)++;
    u.skill_record[u.skills_advanced++] = skill;
    /* subtly change the advance message to indicate no more advancement */
    You("are now %s skilled in %s.",
        P_SKILL(skill) >= P_MAX_SKILL(skill) ? "most" : "more",
        P_NAME(skill));
	update_can_advance_any_skill();
}

static const struct skill_range {
    short first, last;
    const char *name;
} skill_ranges[] = {
    { P_FIRST_H_TO_H, P_LAST_H_TO_H, "Fighting Skills" },
    { P_FIRST_WEAPON, P_LAST_WEAPON, "Weapon Skills" },
    { P_FIRST_SPELL, P_LAST_SPELL, "Spell Casting Skills" },
	{ P_FIRST_NONCOMBAT, P_LAST_NONCOMBAT, "Non-Combat Skills" },
};

/*
 * The `#enhance' extended command.  What we _really_ would like is
 * to keep being able to pick things to advance until we couldn't any
 * more.  This is currently not possible -- the menu code has no way
 * to call us back for instant action.  Even if it did, we would also need
 * to be able to update the menu since selecting one item could make
 * others unselectable.
 */
int
enhance_weapon_skill()
{
    int pass, i, n, len, longest, to_advance, eventually_advance, maxxed_cnt;
    char buf[BUFSZ], sklnambuf[BUFSZ];
    const char *prefix;
    menu_item *selected;
    anything any;
    winid win;
    boolean speedy = FALSE;
	boolean firstheader = TRUE;

    if (wizard && yn("Advance skills without practice?") == 'y')
        speedy = TRUE;

    do {
        /* find longest available skill name, count those that can advance */
        to_advance = eventually_advance = maxxed_cnt = 0;
        for (longest = 0, i = 0; i < P_NUM_SKILLS; i++)
		{
            if (P_RESTRICTED(i))
                continue;
            if ((len = strlen(P_NAME(i))) > longest)
                longest = len;
            if (can_advance(i, speedy))
                to_advance++;
            else if (could_advance(i))
                eventually_advance++;
            else if (peaked_skill(i))
                maxxed_cnt++;
        }

        win = create_nhwindow(NHW_MENU);
        start_menu(win);

        /* start with a legend if any entries will be annotated
           with "*" or "#" below */
		if (!speedy)
		{
			any = zeroany;

			Sprintf(buf, "Skill slot%s available: %d",
				plur(u.weapon_slots), u.weapon_slots);

			add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
				MENU_UNSELECTED);

			strcpy(buf, "");
			add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}

		if (eventually_advance > 0 || maxxed_cnt > 0)
		{
            any = zeroany;
            if (eventually_advance > 0)
			{
                Sprintf(buf, "*: Can be enhanced %s.",
                        (u.ulevel < MAXULEV)
                            ? "when you're more experienced"
                            : "if skill slots become available");
                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
                         MENU_UNSELECTED);
            }
            if (maxxed_cnt > 0)
			{
                Sprintf(buf, "#: Cannot be enhanced further.");

                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
                         MENU_UNSELECTED);
            }
            add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, "",
                     MENU_UNSELECTED);
        }

        /* List the skills, making ones that could be advanced
           selectable.  List the miscellaneous skills first.
           Possible future enhancement:  list spell skills before
           weapon skills for spellcaster roles. */
		longest++;

		char headerbuf[BUFSZ] = "";
		any = zeroany;
		prefix =
			(to_advance + eventually_advance + maxxed_cnt > 0)
			? "    "
			: "";

		if (wizard) {
			if (!iflags.menu_tab_sep)
				Sprintf(headerbuf, " %s%-*s %-12s %-12s %5s (%s)", prefix,
					longest, "Skill", "Current", "Maximum", "Point",
					"Next");
			else
				Sprintf(headerbuf, " %s%s\t%s\t%s\t%5s (%4s)", prefix, "Skill", "Current", "Maximum", "Point",
					"Next");
		}
		else
		{
			if (!iflags.menu_tab_sep)
				Sprintf(headerbuf, " %s %-*s %-12s %-12s", prefix, longest, "Skill", "Current", "Maximum");
			else
				Sprintf(headerbuf, " %s%s\t%s\t%s", prefix, "Skill", "Current", "Maximum");
		}

		add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
			headerbuf, MENU_UNSELECTED);
		
		for (pass = 0; pass < SIZE(skill_ranges); pass++)
            for (i = skill_ranges[pass].first; i <= skill_ranges[pass].last;
                 i++) {
                /* Print headings for skill types */
                any = zeroany;
                if (i == skill_ranges[pass].first)
				{
					/*
					if (!firstheader)
						add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE,
							"", MENU_UNSELECTED);
					*/
                    add_menu(win, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
                             skill_ranges[pass].name, MENU_UNSELECTED);
					firstheader = FALSE;
				}
                if (P_RESTRICTED(i))
                    continue;
                /*
                 * Sigh, this assumes a monospaced font unless
                 * iflags.menu_tab_sep is set in which case it puts
                 * tabs between columns.
                 * The 12 is the longest skill level name.
                 * The "    " is room for a selection letter and dash, "a - ".
                 */
                if (can_advance(i, speedy))
                    prefix = ""; /* will be preceded by menu choice */
                else if (could_advance(i))
                    prefix = "  * ";
                else if (peaked_skill(i))
                    prefix = "  # ";
                else
                    prefix =
                        (to_advance + eventually_advance + maxxed_cnt > 0)
                            ? "    "
                            : "";
                (void) skill_level_name(i, sklnambuf, FALSE);
				char skillmaxbuf[BUFSZ] = "";
				(void)skill_level_name(i, skillmaxbuf, TRUE);

				char skillnamebuf[BUFSZ] = "";
				strcpy(skillnamebuf, P_NAME(i));
				*skillnamebuf = highc(*skillnamebuf);


				if (wizard) {
                    if (!iflags.menu_tab_sep)
                        Sprintf(buf, " %s%-*s %-12s %-12s %5d (%d)", prefix,
                                longest, skillnamebuf, sklnambuf, skillmaxbuf, P_ADVANCE(i),
                                practice_needed_to_advance(i, P_SKILL(i)));
                    else
                        Sprintf(buf, " %s%s\t%s\t%s\t%5d (%d)", prefix, skillnamebuf, 
                                sklnambuf, skillmaxbuf, P_ADVANCE(i),
                                practice_needed_to_advance(i, P_SKILL(i)));
                } else {
                    if (!iflags.menu_tab_sep)
                        Sprintf(buf, " %s %-*s %-12s %-12s", prefix, longest,
							skillnamebuf, sklnambuf, skillmaxbuf);
                    else
                        Sprintf(buf, " %s%s\t%s\t%s", prefix, skillnamebuf,
                                sklnambuf, skillmaxbuf);
                }
                any.a_int = can_advance(i, speedy) ? i + 1 : 0;
                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf,
                         MENU_UNSELECTED);
            }

        Strcpy(buf, (to_advance > 0) ? "Pick a skill to advance:"
                                     : "Current skills:");
        end_menu(win, buf);
        n = select_menu(win, to_advance ? PICK_ONE : PICK_NONE, &selected);
        destroy_nhwindow(win);
        if (n > 0) {
            n = selected[0].item.a_int - 1; /* get item selected */
            free((genericptr_t) selected);
            skill_advance(n);
            /* check for more skills able to advance, if so then .. */
            for (n = i = 0; i < P_NUM_SKILLS; i++) {
                if (can_advance(i, speedy)) {
                    if (!speedy)
                        You_feel("you could be more dangerous!");
                    n++;
                    break;
                }
            }
        }
    } while (speedy && n > 0);
    return 0;
}

void
update_can_advance_any_skill()
{
	int i = 0;
	/* check for more skills able to advance, if so then .. */
	for (i = 0; i < P_NUM_SKILLS; i++) {
		if (can_advance(i, FALSE)) {
			if(!u.canadvanceskill);
				context.botl = TRUE;
			u.canadvanceskill = TRUE;
			return;
		}
	}
	if (u.canadvanceskill);
		context.botl = TRUE;

	u.canadvanceskill = FALSE;
	return;
}
/*
 * Change from restricted to unrestricted, allowing P_BASIC as max.  This
 * function may be called with with P_NONE.  Used in pray.c as well as below.
 */
void
unrestrict_weapon_skill(skill)
int skill;
{
    if (skill < P_NUM_SKILLS && P_RESTRICTED(skill)) {
        P_SKILL(skill) = P_UNSKILLED;
        P_MAX_SKILL(skill) = P_BASIC;
        P_ADVANCE(skill) = 0;
    }
}

void
use_skill(skill, degree)
int skill;
int degree;
{
    boolean advance_before;

    if (skill != P_NONE && !P_RESTRICTED(skill)) {
        advance_before = can_advance(skill, FALSE);
        P_ADVANCE(skill) += degree;
        if (!advance_before && can_advance(skill, FALSE))
            give_may_advance_msg(skill);
		update_can_advance_any_skill();
	}
}

void
add_weapon_skill(n)
int n; /* number of slots to gain; normally one */
{
    int i, before, after;

    for (i = 0, before = 0; i < P_NUM_SKILLS; i++)
        if (can_advance(i, FALSE))
            before++;
    u.weapon_slots += n;
    for (i = 0, after = 0; i < P_NUM_SKILLS; i++)
        if (can_advance(i, FALSE))
            after++;
    if (before < after)
        give_may_advance_msg(P_NONE);

	update_can_advance_any_skill();
}

void
lose_weapon_skill(n)
int n; /* number of slots to lose; normally one */
{
    int skill;

    while (--n >= 0) {
        /* deduct first from unused slots then from last placed one, if any */
        if (u.weapon_slots) {
            u.weapon_slots--;
        } else if (u.skills_advanced) {
            skill = u.skill_record[--u.skills_advanced];
            if (P_SKILL(skill) <= P_UNSKILLED)
                panic("lose_weapon_skill (%d)", skill);
            P_SKILL(skill)--; /* drop skill one level */
            /* Lost skill might have taken more than one slot; refund rest. */
            u.weapon_slots = slots_required(skill) - 1;
            /* It might now be possible to advance some other pending
               skill by using the refunded slots, but giving a message
               to that effect would seem pretty confusing.... */
        }
    }
	update_can_advance_any_skill();
}

int
weapon_type(obj)
struct obj *obj;
{
    /* KMH -- now uses the object table */
    int type;

    if (!obj)
        return P_BARE_HANDED_COMBAT; /* Not using a weapon */

	/* JG -- Now all items have a skill */
	/*
    if (obj->oclass != WEAPON_CLASS && obj->oclass != TOOL_CLASS
        && obj->oclass != GEM_CLASS)
        return P_NONE;*/
	/* Not a weapon, weapon-tool, or ammo */
    type = objects[obj->otyp].oc_skill;
    return (type < 0) ? -type : type;
}

int
uwep_skill_type()
{
    if (u.twoweap)
        return P_TWO_WEAPON_COMBAT;
    return weapon_type(uwep);
}

/*
 * Return hit bonus/penalty based on skill of weapon.
 * Treat restricted weapons as unskilled.
 */
int
weapon_hit_bonus(weapon)
struct obj *weapon;
{
    int type, wep_type, skill, bonus = 0;
    static const char bad_skill[] = "weapon_hit_bonus: bad skill %d";

    wep_type = weapon_type(weapon);
    /* use two weapon skill only if attacking with one of the wielded weapons
     */
    type = (u.twoweap && (!weapon || (weapon && !bimanual(weapon) && (weapon == uwep || weapon == uarms))))
               ? P_TWO_WEAPON_COMBAT
               : wep_type;
    if (type == P_NONE)
	{
        bonus = 0;
    }
	else if (type <= P_LAST_WEAPON)
	{
        switch (P_SKILL(type)) {
        default:
            impossible(bad_skill, P_SKILL(type)); /* fall through */
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -4;
            break;
        case P_BASIC:
            bonus = 0;
            break;
        case P_SKILLED:
            bonus = 2;
            break;
        case P_EXPERT:
            bonus = 3;
            break;
        }
    } else if (type == P_TWO_WEAPON_COMBAT) {
        skill = P_SKILL(P_TWO_WEAPON_COMBAT);
        if (wep_type != P_NONE && P_SKILL(wep_type) < skill)
            skill = P_SKILL(wep_type);
        switch (skill) {
        default:
            impossible(bad_skill, skill); /* fall through */
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -9;
            break;
        case P_BASIC:
            bonus = -7;
            break;
        case P_SKILLED:
            bonus = -5;
            break;
        case P_EXPERT:
            bonus = -3;
            break;
        }
    } else if (type == P_BARE_HANDED_COMBAT) {
        /*
         *        b.h. m.a.
         * unskl:  +1  n/a
         * basic:  +1   +3
         * skild:  +2   +4
         * exprt:  +2   +5
         * mastr:  +3   +6
         * grand:  +3   +7
         */
        bonus = P_SKILL(type);
        bonus = max(bonus, P_UNSKILLED) - 1; /* unskilled => 0 */
        bonus = ((bonus + 2) * (martial_bonus() ? 2 : 1)) / 2;
    }

    /* KMH -- It's harder to hit while you are riding */
    if (u.usteed) {
        switch (P_SKILL(P_RIDING)) {
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus -= 2;
            break;
        case P_BASIC:
            bonus -= 1;
            break;
        case P_SKILLED:
            break;
        case P_EXPERT:
            break;
        }
        if (u.twoweap)
            bonus -= 2;
    }

    return bonus;
}

/*
 * Return damage bonus/penalty based on skill of weapon.
 * Treat restricted weapons as unskilled.
 */
int
weapon_dam_bonus(weapon)
struct obj *weapon;
{
    int type, wep_type, skill, bonus = 0;

    wep_type = weapon_type(weapon);
    /* use two weapon skill only if attacking with one of the wielded weapons
     */
    type = (u.twoweap && (!weapon || (weapon && !bimanual(weapon) && (weapon == uwep || weapon == uarms))))
               ? P_TWO_WEAPON_COMBAT
               : wep_type;
    if (type == P_NONE) {
        bonus = 0;
    } else if (type <= P_LAST_WEAPON) {
        switch (P_SKILL(type)) {
        default:
            impossible("weapon_dam_bonus: bad skill %d", P_SKILL(type));
        /* fall through */
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -2;
            break;
        case P_BASIC:
            bonus = 0;
            break;
        case P_SKILLED:
            bonus = 1;
            break;
        case P_EXPERT:
            bonus = 2;
            break;
        }
    } else if (type == P_TWO_WEAPON_COMBAT) {
        skill = P_SKILL(P_TWO_WEAPON_COMBAT);
        if (P_SKILL(wep_type) < skill)
            skill = P_SKILL(wep_type);
        switch (skill) {
        default:
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            bonus = -3;
            break;
        case P_BASIC:
            bonus = -1;
            break;
        case P_SKILLED:
            bonus = 0;
            break;
        case P_EXPERT:
            bonus = 1;
            break;
        }
    } else if (type == P_BARE_HANDED_COMBAT) {
        /*
         *        b.h. m.a.
         * unskl:   0  n/a
         * basic:  +1   +3
         * skild:  +1   +4
         * exprt:  +2   +6
         * mastr:  +2   +7
         * grand:  +3   +9
         */
        bonus = P_SKILL(type);
        bonus = max(bonus, P_UNSKILLED) - 1; /* unskilled => 0 */
        bonus = ((bonus + 1) * (martial_bonus() ? 3 : 1)) / 2;
    }

    /* KMH -- Riding gives some thrusting damage */
    if (u.usteed && type != P_TWO_WEAPON_COMBAT) {
        switch (P_SKILL(P_RIDING)) {
        case P_ISRESTRICTED:
        case P_UNSKILLED:
            break;
        case P_BASIC:
            break;
        case P_SKILLED:
            bonus += 1;
            break;
        case P_EXPERT:
            bonus += 2;
            break;
        }
    }

    return bonus;
}

/*
 * Initialize weapon skill array for the game.  Start by setting all
 * skills to restricted, then set the skill for every weapon the
 * hero is holding, finally reading the given array that sets
 * maximums.
 */
void
skill_init(class_skill_initial, class_skill_max)
const struct def_skill *class_skill_initial;
const struct def_skill* class_skill_max;
{
    struct obj *obj;
    int sklvl, skill;

    /* initialize skill array; by default, everything is restricted */
    for (skill = 0; skill < P_NUM_SKILLS; skill++) {
        P_SKILL(skill) = P_ISRESTRICTED;
        P_MAX_SKILL(skill) = P_ISRESTRICTED;
        P_ADVANCE(skill) = 0;
    }

    /* walk through array to set skill maximums */
    for (; class_skill_max->skill != P_NONE; class_skill_max++) {
        sklvl = class_skill_max->sklvl;
        skill = class_skill_max->skill;

        P_MAX_SKILL(skill) = sklvl;
        if (P_SKILL(skill) == P_ISRESTRICTED) /* skill pre-set */
            P_SKILL(skill) = P_UNSKILLED;
    }


	/* walk through array to set skill initial levels */
	for (; class_skill_initial->skill != P_NONE; class_skill_initial++) {
		sklvl = class_skill_initial->sklvl;
		skill = class_skill_initial->skill;

		if(P_MAX_SKILL(skill) != P_ISRESTRICTED)
			P_SKILL(skill) = min(P_MAX_SKILL(skill), sklvl);
	}


	/* Set skill for all weapons in inventory to be basic */
	for (obj = invent; obj; obj = obj->nobj) {
		/* don't give skill just because of carried ammo, wait until
		   we see the relevant launcher (prevents an archeologist's
		   touchstone from inadvertently providing skill in sling) */
		if (is_ammo(obj))
			continue;

		skill = weapon_type(obj);
		if (skill != P_NONE && P_SKILL(skill) < P_BASIC && P_MAX_SKILL(skill) != P_ISRESTRICTED)
			P_SKILL(skill) = min(P_MAX_SKILL(skill), P_BASIC);
	}

#if 0
	/* High potential fighters already know how to use their hands. */
	if (P_MAX_SKILL(P_BARE_HANDED_COMBAT) > P_EXPERT)
		P_SKILL(P_BARE_HANDED_COMBAT) = P_BASIC;

	/* Roles that start with a horse know how to ride it */
	if (urole.petnum == PM_PONY)
		P_SKILL(P_RIDING) = P_BASIC;

	/* Rogues and rangers know how to disarm traps */
	if (Role_if(PM_ROGUE) || Role_if(PM_RANGER))
		P_SKILL(P_DISARM_TRAP) = P_BASIC;


	/* set skills for magic */
	if (Role_if(PM_HEALER) || Role_if(PM_MONK)) {
		P_SKILL(P_HEALING_SPELL) = P_BASIC;
	}
	else if (Role_if(PM_PRIEST))
	{
		if (u.ualign.type == A_CHAOTIC)
			P_SKILL(P_NECROMANCY_SPELL) = P_BASIC;
		else
			P_SKILL(P_HEALING_SPELL) = P_BASIC;

		P_SKILL(P_CLERIC_SPELL) = P_BASIC;
	}
	else if (Role_if(PM_WIZARD)) {
		P_SKILL(P_ARCANE_SPELL) = P_BASIC;
		P_SKILL(P_ENCHANTMENT_SPELL) = P_BASIC;
	}
#endif

	/*
     * Make sure we haven't missed setting the max on a skill
     * & set advance
     */
    for (skill = 0; skill < P_NUM_SKILLS; skill++) {
        if (!P_RESTRICTED(skill)) {
            if (P_MAX_SKILL(skill) < P_SKILL(skill)) {
                impossible("skill_init: curr > max: %s", P_NAME(skill));
                P_MAX_SKILL(skill) = P_SKILL(skill);
            }
            P_ADVANCE(skill) = practice_needed_to_advance(skill, P_SKILL(skill) - 1);
        }
    }

    /* each role has a special spell; allow at least basic for its type
       (despite the function name, this works for spell skills too) */
    unrestrict_weapon_skill(spell_skilltype(urole.spelspec));
}

void
setmnotwielded(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
    if (!obj)
        return;
    if ((artifact_light(obj) || (objects[obj->otyp].oc_flags2 & O2_SHINES_MAGICAL_LIGHT)) && obj->lamplit) {
        end_burn(obj, FALSE);
        if (canseemon(mon))
            pline("%s in %s %s %s shining.", The(xname(obj)),
                  s_suffix(mon_nam(mon)), mbodypart(mon, HAND),
                  otense(obj, "stop"));
    }
    if (MON_WEP(mon) == obj)
        MON_NOWEP(mon);
    obj->owornmask &= ~W_WEP;

}

/*weapon.c*/
