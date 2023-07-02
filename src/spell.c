/* GnollHack File Change Notice: This file has been changed from the original. Date of last change: 2023-05-22 */

/* GnollHack 4.0    spell.c    $NHDT-Date: 1546565814 2019/01/04 01:36:54 $  $NHDT-Branch: GnollHack-3.6.2-beta01 $:$NHDT-Revision: 1.88 $ */
/*      Copyright (c) M. Stephenson 1988                          */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include <math.h>


/* spellmenu arguments; 0 thru n-1 used as spl_book[] index when swapping */
#define SPELLMENU_DETAILS (-4)
#define SPELLMENU_PREPARE (-3)
#define SPELLMENU_CAST (-2)
#define SPELLMENU_REORDER (-1)
#define SPELLMENU_SORT (MAXSPELL) /* special menu entry */


#define spellev(spell) spl_book[spell].sp_lev
#define spellamount(spell) spl_book[spell].sp_amount
#define spellmatcomp(spell) spl_book[spell].sp_matcomp
#define spellcooldownlength(spell) spl_book[spell].sp_cooldownlength
#define spellcooldownleft(spell) spl_book[spell].sp_cooldownleft
#define spellskillchance(spell) spl_book[spell].sp_skillchance
#define spellhotkey(spell) spl_book[spell].sp_hotkey
#define spellname(spell) OBJ_NAME(objects[spellid(spell)])
#define spellet(spell) \
    ((char) ((spell < 26) ? ('a' + spell) : ('A' + spell - 26)))  /* Obsolete! Do not use! */
#define spell_to_glyph(spell) (spellid(spell) - FIRST_SPELL + GLYPH_SPELL_TILE_OFF)

STATIC_DCL void FDECL(print_spell_level_symbol, (char*, int));
STATIC_DCL int FDECL(spell_let_to_idx, (CHAR_P));
STATIC_DCL boolean FDECL(cursed_book, (struct obj * bp));
STATIC_DCL boolean FDECL(confused_book, (struct obj *));
STATIC_DCL void FDECL(deadbook, (struct obj *));
STATIC_DCL void FDECL(modronbook, (struct obj*));
STATIC_PTR int NDECL(learn);
STATIC_DCL boolean NDECL(rejectcasting);
STATIC_DCL boolean FDECL(reject_specific_spell_casting, (int));
STATIC_DCL boolean FDECL(getspell, (int *, int));
STATIC_PTR int FDECL(CFDECLSPEC spell_cmp, (const genericptr,
                                            const genericptr));
STATIC_DCL boolean NDECL(spellsortmenu);
STATIC_DCL boolean FDECL(dospellmenu, (const char *, int, int *));
STATIC_DCL boolean FDECL(dotradspellmenu, (const char*, int, int*));
STATIC_DCL boolean FDECL(doaltspellmenu, (const char*, int, int*));
STATIC_DCL int FDECL(percent_success, (int, BOOLEAN_P));
STATIC_DCL int FDECL(attribute_value_for_spellbook, (int));
#if 0
STATIC_DCL char *FDECL(spellretention, (int, char *));
STATIC_DCL int FDECL(throwspell, (int));
STATIC_DCL void FDECL(spell_backfire, (int));
STATIC_DCL boolean FDECL(spell_aim_step, (genericptr_t, int, int));
#endif
STATIC_DCL const char* FDECL(get_spell_type, (int));
STATIC_DCL const char* FDECL(get_attribute, (int));
STATIC_DCL const char* FDECL(get_targeting, (unsigned int));
STATIC_DCL const char* FDECL(get_spell_level_bonus, (int));
STATIC_DCL const char* FDECL(get_components, (int));
STATIC_DCL const char* FDECL(get_material_components, (int));
STATIC_DCL const char *FDECL(spelltypemnemonic, (int));
STATIC_DCL const char* FDECL(spelltypesymbol, (int));
STATIC_DCL int FDECL(domaterialcomponentsmenu, (int));
STATIC_DCL void FDECL(add_spell_cast_menu_item, (winid, int, int, int, BOOLEAN_P));
STATIC_DCL void FDECL(add_spell_cast_menu_heading, (winid, int, BOOLEAN_P));
STATIC_DCL void FDECL(add_spell_prepare_menu_item, (winid, int, int, int, int, BOOLEAN_P));
STATIC_DCL void FDECL(add_spell_prepare_menu_heading, (winid, int, int, BOOLEAN_P));
STATIC_DCL void FDECL(add_alt_spell_cast_menu_item, (winid, int, int));
STATIC_DCL void FDECL(add_alt_spell_prepare_menu_item, (winid, int, int));
STATIC_DCL boolean FDECL(is_acceptable_component_object_type, (struct materialcomponent*, int));
STATIC_DCL boolean FDECL(is_acceptable_component_monster_type, (struct materialcomponent*, int));
STATIC_DCL uchar FDECL(is_obj_acceptable_component, (struct materialcomponent*, struct obj* otmp, BOOLEAN_P));
STATIC_DCL int FDECL(count_matcomp_alternatives, (struct materialcomponent*));
STATIC_DCL struct extended_create_window_info FDECL(extended_create_window_info_from_spell, (int, BOOLEAN_P));

/* since the spellbook itself doesn't blow up, don't say just "explodes" */
STATIC_VAR const char explodes[] = "radiates explosive energy";

NEARDATA const char* spl_sortchoices[NUM_SPELL_SORTBY] = {
    "no sorting, by casting letter",
    "alphabetically",
    "by level, low to high",
    "by level, high to low",
    "by skill group, alphabetized within each group",
    "by skill group, low to high level within group",
    "by skill group, high to low level within group",
    "maintain current ordering",
    /* a menu choice rather than a sort choice */
    "reassign casting letters to retain current order",
};

NEARDATA short spl_orderindx[MAXSPELL] = { 0 }; /* array of spl_book[] indices */

STATIC_OVL struct extended_create_window_info
extended_create_window_info_from_spell(spell_id, active)
int spell_id UNUSED;
boolean active;
{
    struct extended_create_window_info info = { 0 };
    if (active)
        info.create_flags |= WINDOW_CREATE_FLAGS_ACTIVE;
    return info;
}

/* convert a letter into a number in the range 0..51, or -1 if not a letter */
STATIC_OVL int
spell_let_to_idx(ilet)
char ilet;
{
    int indx;

    indx = ilet - 'a';
    if (indx >= 0 && indx < 26)
        return indx;
    indx = ilet - 'A';
    if (indx >= 0 && indx < 26)
        return indx + 26;
    return -1;
}

/* TRUE: book should be destroyed by caller */
STATIC_OVL boolean
cursed_book(bp)
struct obj *bp;
{
    boolean was_in_use;
    int lev = (int)objects[bp->otyp].oc_spell_level;
    int dmg = 0;

    switch (rn2(max(1, (lev + 4) / 2))) {
    case 0:
        You_feel_ex(ATR_NONE, CLR_MSG_WARNING, "a wrenching sensation.");
        tele(); /* teleport him */
        break;
    case 1:
        You_feel_ex(ATR_NONE, CLR_MSG_WARNING, "threatened.");
        play_sfx_sound(SFX_AGGRAVATE_MONSTER);
        aggravate();
        break;
    case 2:
        if (!Blinded)
            play_sfx_sound(SFX_ACQUIRE_BLINDNESS);
        make_blinded(Blinded + rn1(100, 250), TRUE);
        break;
    case 3:
        take_gold();
        break;
    case 4:
        pline_ex(ATR_NONE, CLR_MSG_NEGATIVE, "These runes were just too much to comprehend.");
        if (!Confusion)
            play_sfx_sound(SFX_ACQUIRE_CONFUSION);
        make_confused(itimeout_incr(HConfusion, rn1(7, 16)), FALSE);
        break;
    case 5:
        pline_The_ex(ATR_NONE, CLR_MSG_NEGATIVE, "book was coated with contact poison!");
        if (uarmg) {
            erode_obj(uarmg, "gloves", ERODE_CORRODE, EF_GREASE | EF_VERBOSE);
            break;
        }
        /* temp disable in_use; death should not destroy the book */
        was_in_use = bp->in_use;
        bp->in_use = FALSE;
        losestr(Poison_resistance ? rn1(2, 1) : rn1(4, 3));
        losehp(adjust_damage(rnd(10), (struct monst*)0, &youmonst, AD_DRST, ADFLAGS_NONE), "contact-poisoned spellbook",
               KILLED_BY_AN);
        losehp(rnd(Poison_resistance ? 6 : 10), "contact-poisoned spellbook",
            KILLED_BY_AN);
        bp->in_use = was_in_use;
        break;
    case 6:
        if (Magic_missile_immunity) {
            u_shieldeff();
            pline_The_ex(ATR_NONE, CLR_MSG_NEGATIVE, "book %s, but you are unharmed!", explodes);
        } else {
            pline_ex(ATR_NONE, CLR_MSG_NEGATIVE, "As you read the book, it %s in your %s!", explodes,
                  body_part(FACE));
            dmg = 2 * rnd(10) + 5;
            losehp(adjust_damage(dmg, (struct monst*)0, &youmonst, AD_MAGM, ADFLAGS_NONE), "exploding rune", KILLED_BY_AN);
        }
        return TRUE;
    default:
        rndcurse();
        break;
    }
    return FALSE;
}

/* study while confused: returns TRUE if the book is destroyed */
STATIC_OVL boolean
confused_book(spellbook)
struct obj *spellbook;
{
    boolean gone = FALSE;

    if (!rn2(3) && spellbook->otyp != SPE_BOOK_OF_THE_DEAD && spellbook->otyp != SPE_BOOK_OF_MODRON)
    {
        char dcbuf[BUFSZ] = "";
        char dcbuf2[BUFSZ] = "";
        char dcbuf3[BUFSZ] = "";
        spellbook->in_use = TRUE; /* in case called from learn */
        Strcpy(dcbuf2, "Being confused you have difficulties in controlling your actions.");
        pline_ex1(ATR_NONE, CLR_MSG_WARNING, dcbuf2);
        display_nhwindow(WIN_MESSAGE, FALSE);
        Strcpy(dcbuf3, "You accidentally tear the spellbook to pieces.");
        pline_ex1(ATR_NONE, CLR_MSG_WARNING, dcbuf3);
        Sprintf(dcbuf, "%s %s", dcbuf2, dcbuf3);
        if (!objects[spellbook->otyp].oc_name_known
            && !objects[spellbook->otyp].oc_uname)
            docall(spellbook, dcbuf);
        useup(spellbook);
        gone = TRUE;
    } else {
        You_ex(ATR_NONE, CLR_MSG_ATTENTION, "find yourself reading the %s line over and over again.",
            spellbook == context.spbook.book ? "next" : "first");
    }
    return gone;
}

/* special effects for The Book of the Dead */
STATIC_OVL void
deadbook(book2)
struct obj *book2;
{
    struct monst *mtmp, *mtmp2;
    coord mm;

    You("turn the pages of the Book of the Dead...");
    makeknown(SPE_BOOK_OF_THE_DEAD);
    /* KMH -- Need ->known to avoid "_a_ Book of the Dead" */
    book2->known = 1;
    if (invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy)) {
        register struct obj *otmp;
        register boolean arti1_primed = FALSE, arti2_primed = FALSE,
                         arti_cursed = FALSE;

        if (book2->cursed) {
            play_sfx_sound(SFX_GENERAL_NOT_IN_THE_RIGHT_CONDITION);
            pline_The_ex(ATR_NONE, CLR_MSG_WARNING, "runes appear scrambled.  You can't read them!");
            return;
        }

        if (!u.uhave.bell || !u.uhave.menorah) {
            pline_ex(ATR_NONE, CLR_MSG_WARNING, "A chill runs down your %s.", body_part(SPINE));
            if (!u.uhave.bell)
                You_hear("a faint chime...");
            if (!u.uhave.menorah)
                pline("Vlad's doppelganger is amused.");
            return;
        }

        for (otmp = invent; otmp; otmp = otmp->nobj) {
            if (otmp->otyp == CANDELABRUM_OF_INVOCATION && otmp->special_quality >= objects[otmp->otyp].oc_special_quality
                && otmp->lamplit) {
                if (!otmp->cursed)
                    arti1_primed = TRUE;
                else
                    arti_cursed = TRUE;
            }
            if (otmp->otyp == BELL_OF_OPENING
                && (moves - otmp->age) < 5L) { /* you rang it recently */
                if (!otmp->cursed)
                    arti2_primed = TRUE;
                else
                    arti_cursed = TRUE;
            }
        }

        if (arti_cursed) {
            play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
            pline_The_ex(ATR_NONE, CLR_MSG_FAIL, "invocation fails!");
            pline_ex(ATR_NONE, CLR_MSG_HINT, "At least one of your artifacts is cursed...");
        } else if (arti1_primed && arti2_primed) {
            unsigned soon =
                (unsigned) d(2, 6); /* time til next intervene() */
            
            /* successful invocation */
            mkinvokearea();
            if (!u.uevent.invoked)
            {
                achievement_gained("Performed the Invocation Ritual");
                livelog_printf(LL_ACHIEVE, "%s", "performed the invocation");
                u.uevent.invoked = 1;
            }
            /* in case you haven't killed the Wizard yet, behave as if
               you just did */
            u.uevent.ukilled_wizard = 1; /* wizdead() */
            if (!u.uintervene_timer || u.uintervene_timer > soon)
                u.uintervene_timer = soon;
        } else { /* at least one artifact not prepared properly */
            You_ex(ATR_NONE, CLR_MSG_ATTENTION, "have a feeling that %s is amiss...", something);
            goto raise_dead;
        }
        return;
    }

    /* when not an invocation situation */
    if (book2->cursed)
    {
    raise_dead:
        You_ex(ATR_NONE, CLR_MSG_WARNING, "raised the dead!");
        /* first maybe place a dangerous adversary */
        if (!rn2(3) && ((mtmp = makemon(&mons[PM_MASTER_LICH], u.ux, u.uy,
                                        MM_NO_MONSTER_INVENTORY | MM_PLAY_SUMMON_ANIMATION | MM_UNDEAD_SUMMON_ANIMATION | MM_PLAY_SUMMON_SOUND | MM_ANIMATION_WAIT_UNTIL_END)) != 0
                        || (mtmp = makemon(&mons[PM_NALFESHNEE], u.ux, u.uy,
                                           MM_NO_MONSTER_INVENTORY | MM_PLAY_SUMMON_ANIMATION | MM_CHAOTIC_SUMMON_ANIMATION | MM_PLAY_SUMMON_SOUND | MM_ANIMATION_WAIT_UNTIL_END)) != 0)) {
            mtmp->mpeaceful = 0;
            set_mhostility(mtmp);
            newsym(mtmp->mx, mtmp->my);
        }
        /* next handle the affect on things you're carrying */
        (void) revive_from_inventory(&youmonst);
        /* last place some monsters around you */
        mm.x = u.ux;
        mm.y = u.uy;
        mkundead(&mm, TRUE, MM_NO_MONSTER_INVENTORY | MM_PLAY_SUMMON_ANIMATION | MM_UNDEAD_SUMMON_ANIMATION);

    } 
    else if (book2->blessed)
    {
        for (mtmp = fmon; mtmp; mtmp = mtmp2) 
        {
            mtmp2 = mtmp->nmon; /* tamedog() changes chain */
            if (DEADMONSTER(mtmp))
                continue;

            if ((is_undead(mtmp->data) || is_vampshifter(mtmp))
                && cansee(mtmp->mx, mtmp->my))
            {
                mtmp->mpeaceful = TRUE;

                if (sgn(mtmp->data->maligntyp) == sgn(u.ualign.type)
                    && distu(mtmp->mx, mtmp->my) < 4)
                {
                    if (mtmp->mtame)
                    {
                        if (mtmp->mtame < 20)
                            mtmp->mtame++;
                    }
                    else
                        (void)tamedog(mtmp, (struct obj*) 0, TAMEDOG_NO_FORCED_TAMING, FALSE, 0, FALSE, FALSE);
                }
                else 
                {
                    monflee(mtmp, 0, FALSE, TRUE);
                    newsym(mtmp->mx, mtmp->my);
                }
            }
        }
    }
    else
    {
        switch (rn2(3))
        {
        case 0:
            Your_ex(ATR_NONE, CLR_MSG_WARNING, "ancestors are annoyed with you!");
            break;
        case 1:
            pline_The_ex(ATR_NONE, CLR_MSG_WARNING, "headstones in the cemetery begin to move!");
            break;
        default:
            pline_ex(ATR_NONE, CLR_MSG_WARNING, "Oh my!  Your name appears in the book!");
        }
    }
    return;
}

/* special effects for The Prime Codex */
STATIC_OVL void
modronbook(book2)
struct obj* book2;
{
    You("finish reading one paragraph of the Prime Codex...");
    book2->aknown = book2->nknown = book2->dknown = 1;
    makeknown(SPE_BOOK_OF_MODRON);

    if (Hallucination || Stunned || Confusion)
        pline_ex(ATR_NONE, CLR_MSG_POSITIVE, "The machinations of the multiverse suddenly start to make a lot more sense to you.");
    else
        pline_ex(ATR_NONE, CLR_MSG_POSITIVE, "The machinations of the multiverse are a bit clearer to you now.");

    play_sfx_sound(SFX_CURE_AILMENT);

    (void)make_hallucinated(0L, TRUE, 0L);
    make_stunned(0L, TRUE);
    make_confused(0L, TRUE);
}



/* 'book' has just become cursed; if we're reading it and realize it is
   now cursed, interrupt */
void
book_cursed(book)
struct obj *book;
{
    if (occupation == learn && context.spbook.book == book
        && book->cursed && book->bknown && multi >= 0)
        stop_occupation();
}

STATIC_PTR int
learn(VOID_ARGS)
{
    int i;
    short booktype;
    char splname[BUFSZ];
    boolean costly = TRUE;
    struct obj *book = context.spbook.book;
    boolean learnsuccess = FALSE;
    boolean gone = FALSE;
    struct monst* shkp = 0;
    boolean billable_book = FALSE;

    if (book->unpaid && costly_spot(u.ux, u.uy))
    {
        char* o_shop = in_rooms(u.ux, u.uy, SHOPBASE);
        shkp = shop_keeper(*o_shop);
        if (shkp && inhishop(shkp) && is_obj_on_shk_bill(book, shkp))
        {
            billable_book = TRUE;
        }
    }

    /* JDS: lenses give 50% faster reading; 33% smaller read time */
    if (context.spbook.delay && Enhanced_vision && rn2(2))
        context.spbook.delay++;
    if (Confusion && book->otyp != SPE_BOOK_OF_THE_DEAD && book->otyp != SPE_BOOK_OF_MODRON)
    { /* became confused while learning */
        context.spbook.reading_result = READING_RESULT_CONFUSED;
#if 0
        (void) confused_book(book);
        context.spbook.book = 0; /* no longer studying */
        context.spbook.o_id = 0;
        nomul(context.spbook.delay); /* remaining delay is uninterrupted */
        multi_reason = "reading a book";
        nomovemsg = 0;
        context.spbook.delay = 0;
        return 0;
#endif
    }
    if (context.spbook.delay) {
        /* not if (context.spbook.delay++), so at end delay == 0 */
        context.spbook.delay++;
        return 1; /* still busy */
    }


    /* STUDY RESULT */
    booktype = book->otyp;

    /* Book of the Dead always has the same result */
    if (booktype == SPE_BOOK_OF_THE_DEAD) {
        deadbook(book);
        return 0;
    }
    else if (booktype == SPE_BOOK_OF_MODRON)
    {
        modronbook(book);
        return 0 ;
    }

    /* Possible failures */
    boolean reading_result = context.spbook.reading_result;
    if (reading_result == READING_RESULT_FAIL)
    {

        if (book->cursed)
            gone = cursed_book(book);
        else if (!book->blessed && !rn2(2))
        {
            play_sfx_sound(SFX_SPELL_LEARN_FAIL);
            pline_ex(ATR_NONE, CLR_MSG_NEGATIVE, "These runes were just too much to comprehend.");
            play_sfx_sound(SFX_ACQUIRE_CONFUSION);
            make_confused(itimeout_incr(HConfusion, rnd(4) + 5), FALSE);
        }
        else
        {
            play_sfx_sound(SFX_SPELL_LEARN_FAIL);
            pline_ex(ATR_NONE, CLR_MSG_FAIL, "Despite your best efforts, you fail to understand the spell in %s.", the(cxname(book)));
        }

        if (gone || !rn2(2)) 
        {
            if (!gone)
            {
                if (iflags.using_gui_sounds)
                    delay_output_milliseconds(100); /* Some time so that the sounds do not come exactly at the same time */
                play_sfx_sound(SFX_ITEM_CRUMBLES_TO_DUST);
                pline_The_ex(ATR_NONE, CLR_MSG_ATTENTION, "spellbook crumbles to dust!");
            }
            if (!objects[book->otyp].oc_name_known
                && !objects[book->otyp].oc_uname)
                docall(book, (char*)0);
            useup(book);
        }
        else
            book->in_use = FALSE;

        goto check_added_to_your_bill_here;
    }
    else if (reading_result == READING_RESULT_CONFUSED)
    {
        gone = confused_book(book);
        if (!gone)
        {
            book->in_use = FALSE;
        }
        goto check_added_to_your_bill_here;
    }


    /* SUCCESS */
    exercise(A_WIS, TRUE); /* you're studying. */

    Sprintf(splname, "%s", OBJ_NAME(objects[booktype]));
    for (i = 0; i < MAXSPELL; i++)
        if (spellid(i) == booktype || spellid(i) == NO_SPELL)
            break;
    
    int initialamount = i < MAXSPELL ? spl_book[i].sp_amount : 0;
    int addedamount = 0;

    if (i == MAXSPELL)
    {
        impossible("Too many spells memorized!");
    }
    else if (spellid(i) == booktype)
    {
        /* normal book can be read and re-read a total of 4 times */
        if (book->spestudied > MAX_SPELL_STUDY)
        {
            play_sfx_sound(SFX_SPELL_TOO_FAINT);
            pline_ex(ATR_NONE, CLR_MSG_FAIL, "This spellbook is too faint to be read any more.");
            book->otyp = booktype = SPE_BLANK_PAPER;
            book->material = objects[book->otyp].oc_material;
            /* reset spestudied as if polymorph had taken place */
            book->spestudied = rn2(book->spestudied);
        }
        //else if (spellknow(i) > SPELL_IS_KEEN / 10)
        //{
        //    play_sfx_sound(SFX_SPELL_KNOWN_ALREADY);
        //    You_ex(ATR_NONE, CLR_MSG_ATTENTION, "know %s quite well already.", splname);
        //    costly = FALSE;
        //}
        else
        { /* spellknow(i) <= SPELL_IS_KEEN/10 */
            play_sfx_sound(SFX_SPELL_KEENER);
            int multicolors[2] = { CLR_MSG_SPELL, NO_COLOR };
            Your_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, "knowledge of \'%s\' is %s.", splname, spellknow(i) ? "keener" : "restored");
            incr_spell_nknow(i, 1);
            book->spestudied++;
            exercise(A_WIS, TRUE); /* extra study */

            if (spl_book[i].sp_matcomp > 0)
            {
                addedamount = matlists[spl_book[i].sp_matcomp].spellsgained;
                spl_book[i].sp_amount += addedamount;
            }
            learnsuccess = TRUE;
        }
        /* make book become known even when spell is already
           known, in case amnesia made you forget the book */
        makeknown((int) booktype);
    }
    else
    { /* (spellid(i) == NO_SPELL) */
        /* for a normal book, spestudied will be zero, but for
           a polymorphed one, spestudied will be non-zero and
           one less reading is available than when re-learning */
        if (book->spestudied >= MAX_SPELL_STUDY) {
            /* pre-used due to being the product of polymorph */
            play_sfx_sound(SFX_SPELL_TOO_FAINT);
            pline_ex(ATR_NONE, CLR_MSG_FAIL, "This spellbook is too faint to read even once.");
            book->otyp = booktype = SPE_BLANK_PAPER;
            book->material = objects[book->otyp].oc_material;
            /* reset spestudied as if polymorph had taken place */
            book->spestudied = rn2(book->spestudied);
        } 
        else 
        {
            spl_book[i].sp_id = (short)booktype;
            spl_book[i].sp_lev = (xchar)objects[booktype].oc_spell_level;
            spl_book[i].sp_matcomp = objects[booktype].oc_material_components;
            if(spl_book[i].sp_matcomp)
                spl_book[i].sp_amount = 0; //How many times material components have been mixed
            else
                spl_book[i].sp_amount = -1; //Infinite
            spl_book[i].sp_cooldownlength = (int)objects[booktype].oc_spell_cooldown;
            spl_book[i].sp_cooldownleft = 0;
            spl_book[i].sp_skillchance = (int)objects[booktype].oc_spell_skill_chance;

            incr_spell_nknow(i, 1);
            book->spestudied++;

            if (spl_book[i].sp_matcomp > 0)
            {
                addedamount = matlists[spl_book[i].sp_matcomp].spellsgained;
                spl_book[i].sp_amount += addedamount;
            }

            sortspells();

            play_sfx_sound(SFX_SPELL_LEARN_SUCCESS);
            int multicolors[1] = { CLR_MSG_SPELL };
            You_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, i > 0 ? "add \'%s\' to your repertoire." : "learn \'%s\'.", splname);
            learnsuccess = TRUE;
        }
        makeknown((int) booktype);
    }

    if (addedamount > 0)
    {
        int multicolors[3] = { CLR_BRIGHT_CYAN, NO_COLOR, CLR_MSG_SPELL };
        if (addedamount == 1)
            You_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, "now have %s %scasting of \'%s\' prepared.", "one", !initialamount ? "" : "more ", splname);
        else
            You_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, "now have %d %scastings of \'%s\' prepared.", addedamount, !initialamount ? "" : "more ", splname);
    }

    if (book->cursed) 
    { /* maybe a demon cursed it */
        if (cursed_book(book)) 
        {
            useup(book);
            gone = TRUE;
            context.spbook.book = 0;
            context.spbook.o_id = 0;
            goto check_added_to_your_bill_here;
        }
    }
    if (costly)
        check_unpaid(book);

    if (learnsuccess)
    {
        play_sfx_sound(SFX_ITEM_CRUMBLES_TO_DUST);
        pline_The_ex(ATR_NONE, CLR_MSG_ATTENTION, "spellbook crumbles to dust.");
        useup(book);
        gone = TRUE;
    }

    context.spbook.book = 0;
    context.spbook.o_id = 0;

check_added_to_your_bill_here:
    if (gone && billable_book && shkp && !Deaf && !muteshk(shkp))
        play_voice_shopkeeper_simple_line(shkp, SHOPKEEPER_LINE_ILL_ADD_THAT_TO_YOUR_BILL);

    return 0;

}

void
print_spell_level_text(buf, booktype, includeschool, capitalize_style)
char* buf;
int booktype;
boolean includeschool;
uchar capitalize_style;
{
    if (!buf)
        return;

    char lvlbuf[BUFSZ];
    if (includeschool)
    {
        if (objects[booktype].oc_spell_level == -1)
            Sprintf(lvlbuf, "minor %s cantrip", spelltypemnemonic(objects[booktype].oc_skill));
        else if (objects[booktype].oc_spell_level == 0)
            Sprintf(lvlbuf, "major %s cantrip", spelltypemnemonic(objects[booktype].oc_skill));
        else if (objects[booktype].oc_spell_level > 0)
            Sprintf(lvlbuf, "level %ld %s spell", objects[booktype].oc_spell_level, spelltypemnemonic(objects[booktype].oc_skill));
        else
            Strcpy(lvlbuf, "spell of inappropriate level");
    }
    else
    {
        if (objects[booktype].oc_spell_level == -1)
            Sprintf(lvlbuf, "minor cantrip");
        else if (objects[booktype].oc_spell_level == 0)
            Sprintf(lvlbuf, "major cantrip");
        else if (objects[booktype].oc_spell_level > 0)
            Sprintf(lvlbuf, "%ld", objects[booktype].oc_spell_level);
        else
            Strcpy(lvlbuf, "inappropriate level");
    }
    switch(capitalize_style)
    {
    case 1:
        *lvlbuf = highc(*lvlbuf);
        /*Fall through*/
    default:
    case 0:
        strcpy(buf, lvlbuf);
        break;
    case 2:
        strcpy_capitalized_for_title(buf, lvlbuf);
        break;
    }

}

STATIC_OVL
void
print_spell_level_symbol(buf, booktype)
char* buf;
int booktype;
{
    if (!buf)
        return;

    char lvlbuf[BUFSZ];
    if (objects[booktype].oc_spell_level == -1)
        strcpy(lvlbuf, "c");
    else if (objects[booktype].oc_spell_level == 0)
        strcpy(lvlbuf, "C");
    else if (objects[booktype].oc_spell_level > 0)
        Sprintf(lvlbuf, "%ld", objects[booktype].oc_spell_level);
    else
        strcpy(lvlbuf, "*");

    Sprintf(buf, "%s %s", spelltypesymbol(objects[booktype].oc_skill), lvlbuf);
}

int
study_book(spellbook)
register struct obj *spellbook;
{
    if (!spellbook)
        return 0;

    int booktype = spellbook->otyp;
    boolean confused = (Confusion != 0);
    boolean hallucinated = (Hallucination != 0);
    boolean too_hard = FALSE;
    char lvlbuf[BUFSZ] = "";
    char buf[BUFSZ] = "";
    char namebuf[BUFSZ] = "";
    char Namebuf2[BUFSZ] = "";
    boolean takeround = 0;
    boolean perusetext = 0;

    if (!(context.spbook.delay && spellbook == context.spbook.book)
        && !(objects[spellbook->otyp].oc_flags & O1_NON_SPELL_SPELLBOOK))
    {
        strcpy(namebuf, OBJ_NAME(objects[booktype]));
        strcpy(Namebuf2, OBJ_NAME(objects[booktype]));
        *Namebuf2 = highc(*Namebuf2);

        print_spell_level_text(lvlbuf, booktype, TRUE, FALSE);

        if (!confused && !hallucinated)
        {
            if (!objects[spellbook->otyp].oc_name_known)
            {
                if (objects[spellbook->otyp].oc_spell_flags & S1_SPELLBOOK_MUST_BE_READ_TO_IDENTIFY)
                {
                    if(!OBJ_CONTENT_DESC(spellbook->otyp) || strcmp(OBJ_CONTENT_DESC(spellbook->otyp), "") == 0)
                        Sprintf(buf, "The topic of %s is unclear. Read it?", the(cxname(spellbook)));
                    else
                        Sprintf(buf, "This spellbook contains %s. Read it?", OBJ_CONTENT_DESC(spellbook->otyp));
                    takeround = 1;
                    perusetext = 1;
                }
                else
                {
                    int multicolors[1] = { CLR_MSG_SPELL };
                    pline_multi_ex(ATR_NONE, CLR_MSG_HINT, no_multiattrs, multicolors, "This spellbook contains \'%s\'.", namebuf);
                    makeknown(spellbook->otyp);
                    takeround = 1;
                    Sprintf(buf, "\'%s\' is %s. Continue?", Namebuf2, an(lvlbuf));
                }
            }
            else
            {
                Sprintf(buf, "\'%s\' is %s. Continue?", Namebuf2, an(lvlbuf));
            }
        }
        else if (hallucinated)
        {
            Sprintf(buf, "Whoa! This book contains some real deep stuff. Continue?");
            takeround = 1;
        }
        else //Confused
        {
            takeround = 1;
            if (!rn2(3))
                Sprintf(buf, "This spellbook contains %s. Read it?", an(lvlbuf));
            else
                Sprintf(buf, "The runes in %s seem to be all over the place. Continue?", the(cxname(spellbook)));
        }
        if (yn_query(buf) != 'y')
            return takeround; // Takes one round to read the header if not known in advance

        if (perusetext)
            You("start perusing %s...", the(cxname(spellbook)));

        /* attempting to read dull book may make hero fall asleep */
        if (!confused && !Sleep_resistance
            && !strcmp(OBJ_DESCR(objects[booktype]), "dull")) {
            const char *eyes;
            int dullbook = rnd(25) - ACURR(A_WIS);

            /* adjust chance if hero stayed awake, got interrupted, retries */
            if (context.spbook.delay && spellbook == context.spbook.book)
                dullbook -= rnd((int)objects[booktype].oc_spell_level + 2);

            if (dullbook > 0) {
                eyes = body_part(EYE);
                if (eyecount(youmonst.data) > 1)
                    eyes = makeplural(eyes);
                play_sfx_sound(SFX_ACQUIRE_SLEEP);
                pline_ex(ATR_NONE, CLR_MSG_WARNING, "This book is so dull that you can't keep your %s open.",
                      eyes);
                dullbook += rnd(2 * ((int)objects[booktype].oc_spell_level + 2));
                fall_asleep(-dullbook, TRUE);
                return 1;
            }
        }
    }

    boolean resume = FALSE;
    if (context.spbook.delay && !confused && spellbook == context.spbook.book
        /* handle the sequence: start reading, get interrupted, have
           context.spbook.book become erased somehow, resume reading it */
        && booktype != SPE_BLANK_PAPER) {
        You("continue your efforts to %s the %s.",
            (objects[booktype].oc_subtyp != BOOKTYPE_SPELLBOOK) ? "read" : "memorize",
            (objects[booktype].oc_subtyp != BOOKTYPE_SPELLBOOK) ? book_type_names[objects[booktype].oc_subtyp] : "spell");
        resume = TRUE;
    } 
    else 
    {
        /* KMH -- Simplified this code */
        if (booktype == SPE_BLANK_PAPER) {
            pline("This spellbook is all blank.");
            makeknown(booktype);
            return 1;
        }

        /* 3.6 tribute */
        if (objects[booktype].oc_subtyp == BOOKTYPE_NOVEL) 
        {
            /* Obtain current Terry Pratchett book title */
            const char *tribtitle = noveltitle(&spellbook->novelidx, 0UL, 0UL);

            if (read_tribute("books", tribtitle, 0, (char *) 0, 0,
                             spellbook->o_id))
            {
                if (!u.uconduct.literate++)
                    livelog_printf(LL_CONDUCT,
                        "became literate by reading %s", tribtitle);
                check_unpaid(spellbook);
                makeknown(booktype);
                if (!u.uevent.read_tribute) 
                {
                    /* give bonus of 20 xp and 4*20+0 pts */
                    more_experienced(20, 0);
                    newexplevel();
                    u.uevent.read_tribute = 1; /* only once */
                }
                if (!u.uachieve.read_discworld_novel)
                {
                    achievement_gained("Read a Discworld Novel");
                    u.uachieve.read_discworld_novel = 1;
                }
            }
            return 1;
        }
        else if (objects[booktype].oc_subtyp == BOOKTYPE_MANUAL)
        {
            read_manual(spellbook);
            return 1;
        }

        context.spbook.delay = -min(8, max(1, (schar)(objects[booktype].oc_spell_level)))
            * objects[booktype].oc_delay;

        /* Books are often wiser than their readers (Rus.) */
        spellbook->in_use = TRUE;
        if (!spellbook->blessed && spellbook->otyp != SPE_BOOK_OF_THE_DEAD && spellbook->otyp != SPE_BOOK_OF_MODRON) {
            if (spellbook->cursed)
            {
                too_hard = TRUE;
            }
            else
            {

                /* uncursed - chance to fail */
                int read_ability = attribute_value_for_spellbook(spellbook->otyp)
                    + 8 
                    + u.ulevel
                    + 4 * max(0, P_SKILL_LEVEL(spell_skilltype(spellbook->otyp)) - 1)
                    - 2 * (int)objects[booktype].oc_spell_level
                    + (Enhanced_vision ? 2 : 0);

                if (read_ability < 20 && !confused) //Role_if(PM_WIZARD) && 
                {
                    char qbuf[QBUFSZ];
                    char descbuf[BUFSZ] = "difficult";

                    if (read_ability <= 0)
                        Sprintf(descbuf, "%sseems impossible", perusetext ? "still " : "");
                    else if (read_ability <= 4)
                        Sprintf(descbuf, "%sseems extremely difficult", perusetext ? "still " : "");
                    else if (read_ability <= 8)
                        Sprintf(descbuf, "%sseems very difficult", perusetext ? "still " : "");
                    else if (read_ability <= 12)
                        Sprintf(descbuf, "%sseems difficult", perusetext ? "still " : "");
                    else if (read_ability <= 16)
                        Sprintf(descbuf, "%sseems somewhat difficult", perusetext ? "still " : "");
                    else
                        Sprintf(descbuf, "%sseems a bit difficult", perusetext ? "still " : "");

                    Sprintf(qbuf, "This spellbook %s to comprehend. Continue?", descbuf);

                    if (yn_query(qbuf) != 'y') 
                    {
                        spellbook->in_use = FALSE;
                        return 1;
                    }
                }
                /* its up to random luck now */
                if (rnd(20) > read_ability) {
                    too_hard = TRUE;
                }
            }
        }

        /* Now, check the result */
        context.spbook.reading_result = READING_RESULT_SUCCESS;
        if (too_hard)
        {
            context.spbook.reading_result = READING_RESULT_FAIL;
#if 0
            boolean gone = FALSE;
            
            if(spellbook->cursed)
                gone = cursed_book(spellbook);
            else if (!spellbook->blessed && !rn2(2))
            {
                pline("These runes were just too much to comprehend.");
                make_confused(itimeout_incr(HConfusion, rnd(4) + 5), FALSE);
            }

            nomul(context.spbook.delay); /* study time */
            multi_reason = "reading a book";
            nomovemsg = 0;
            context.spbook.delay = 0;
            if (gone || !rn2(2)) {
                if (!gone)
                    pline_The("spellbook crumbles to dust!");
                if (!objects[spellbook->otyp].oc_name_known
                    && !objects[spellbook->otyp].oc_uname)
                    docall(spellbook, (char*)0);
                useup(spellbook);
            } else
                spellbook->in_use = FALSE;
            return 1;
#endif
        } 
        else if (confused) 
        {
            context.spbook.reading_result = READING_RESULT_CONFUSED;
#if 0
            if (!confused_book(spellbook))
            {
                spellbook->in_use = FALSE;
            }
            nomul(context.spbook.delay);
            multi_reason = "reading a book";
            nomovemsg = 0;
            context.spbook.delay = 0;
            return 1;
#endif
        }
        spellbook->in_use = FALSE;

//        if (perusetext)
//            pline("The spellbook seems comprehensible enough.");

        play_simple_object_sound(spellbook, OBJECT_SOUND_TYPE_READ);
        You("begin to %s the runes.",
            spellbook->otyp == SPE_BOOK_OF_THE_DEAD ? "recite" : spellbook->otyp == SPE_BOOK_OF_MODRON ? "decipher" : "memorize");
        if (iflags.using_gui_sounds)
            delay_output_milliseconds(100); /* Some time between sounds */
    }

    context.spbook.book = spellbook;
    if (context.spbook.book)
        context.spbook.o_id = context.spbook.book->o_id;

    set_occupation(learn, "studying", objects[spellbook->otyp].oc_soundset, OCCUPATION_STUDYING, resume ? OCCUPATION_SOUND_TYPE_RESUME : OCCUPATION_SOUND_TYPE_START, 0);
    return 1;
}

/* a spellbook has been destroyed or the character has changed levels;
   the stored address for the current book is no longer valid */
void
book_disappears(obj)
struct obj *obj;
{
    if (obj == context.spbook.book) {
        context.spbook.book = (struct obj *) 0;
        context.spbook.o_id = 0;
    }
}

/* renaming an object usually results in it having a different address;
   so the sequence start reading, get interrupted, name the book, resume
   reading would read the "new" book from scratch */
void
book_substitution(old_obj, new_obj)
struct obj *old_obj, *new_obj;
{
    if (old_obj == context.spbook.book) {
        context.spbook.book = new_obj;
        if (context.spbook.book)
            context.spbook.o_id = context.spbook.book->o_id;
    }
}

/* called from moveloop() */
void
age_spells()
{
    int i;
    /*
     * The time relative to the hero (a pass through move
     * loop) causes all spell knowledge to be decremented.
     * The hero's speed, rest status, conscious status etc.
     * does not alter the loss of memory.
     */
    for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
        if (spellknow(i))
            decrnknow(i);
    return;
}

/* return True if spellcasting is inhibited;
   only covers a small subset of reasons why casting won't work */
STATIC_OVL boolean
rejectcasting()
{
    /* rejections which take place before selecting a particular spell */
    if (Stunned)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "are too impaired to cast a spell.");
        return TRUE;
    } 
    else if (Cancelled)
    {
        play_sfx_sound(SFX_CANCELLATION_IN_FORCE);
        Your_ex(ATR_NONE, CLR_MSG_FAIL, "magic is not flowing properly to allow for casting a spell.");
        return TRUE;
    }
    return FALSE;
}

/* return True if spellcasting is inhibited;
   only covers a small subset of reasons why casting won't work */
STATIC_OVL boolean
reject_specific_spell_casting(spell)
int spell;
{
    /* rejections which take place before selecting a particular spell */
    int spellbookid = spellid(spell);

    if (spellbookid == NO_SPELL)
        return TRUE;

    if (!(objects[spellbookid].oc_spell_flags & S1_NO_VERBAL_COMPONENT) && !can_chant(&youmonst))
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "are unable to chant the incantation for a spell with a verbal component!");
        return TRUE;
    }
    else if (!(objects[spellbookid].oc_spell_flags & S1_NO_SOMATIC_COMPONENT) && nohands(youmonst.data))
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "do not have hands to cast a spell with a somatic component!");
        return TRUE;
    }
#if 0 /* Removed as excessively dangerous for wizards using staffs */
    else if (!(objects[spellbookid].oc_spell_flags & S1_NO_SOMATIC_COMPONENT) && !freehand())
    {
        /* Note: !freehand() occurs when weapon and shield (or two-handed
         * weapon) are welded to hands, so "arms" probably doesn't need
         * to be makeplural(bodypart(ARM)).
         */
        play_sfx_sound(SFX_GENERAL_CANNOT);
        Your_ex(ATR_NONE, CLR_MSG_WARNING, "arms are not free to cast a spell with a somatic component!");
        return TRUE;
    }
#endif
    else if (!(objects[spellbookid].oc_spell_flags & S1_NO_SOMATIC_COMPONENT) && u.uburied) {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "cannot cast a spell with a somatic component while being buried!");
        return TRUE;
    }
    return FALSE;
}

#define MAX_SPELL_LIST_TYPES 6
/*
 * Return TRUE if a spell was picked, with the spell index in the return
 * parameter.  Otherwise return FALSE.
 */
STATIC_OVL boolean
getspell(spell_no, spell_list_type)
int *spell_no;
int spell_list_type;
{
    if (spell_list_type < 0 || spell_list_type >= MAX_SPELL_LIST_TYPES)
        return FALSE;

    int nspells, idx;
    char ilet, lets[BUFSZ], qbuf[QBUFSZ];
    const char* const verbs[MAX_SPELL_LIST_TYPES] = { "cast", "prepare", "manage", "view", "set a hotkey for", "forget" };
    char titlebuf[BUFSZ];
    char verbbufC[BUFSZ];
    char verbtitlebufC[BUFSZ];
    strcpy(verbbufC, verbs[spell_list_type]);
    *verbbufC = highc(*verbbufC);
    strcpy_capitalized_for_title(verbtitlebufC, verbs[spell_list_type]);
    Sprintf(titlebuf, "Choose a Spell to %s", verbtitlebufC);

    if (spellid(0) == NO_SPELL) 
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "don't know any spells right now.");
        return FALSE;
    }

    if (spell_list_type == 0 && rejectcasting())
        return FALSE; /* no spell chosen */

    if (flags.menu_style == MENU_TRADITIONAL)
    {
        /* we know there is at least 1 known spell */
        for (nspells = 1; nspells < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(nspells) != NO_SPELL;
            nspells++)
            continue;

        if (nspells == 1)
            Strcpy(lets, "a");
        else if (nspells < 27)
            Sprintf(lets, "a-%c", 'a' + nspells - 1);
        else if (nspells == 27)
            Sprintf(lets, "a-zA");
        /* this assumes that there are at most 52 spells... */
        else
            Sprintf(lets, "a-zA-%c", 'A' + nspells - 27);

        for (;;)
        {
            Sprintf(qbuf, "%s which spell? [%s *?]",
                verbbufC, lets);
            ilet = yn_function(qbuf, (char*)0, '\0', (char*)0);
            if (ilet == '*' || ilet == '?')
                break; /* use menu mode */
            if (index(quitchars, ilet))
                return FALSE;

            idx = spell_let_to_idx(ilet);
            if (idx < 0 || idx >= nspells)
            {
                play_sfx_sound(SFX_GENERAL_CANNOT);
                You_ex(ATR_NONE, CLR_MSG_FAIL, "don't know that spell.");
                continue; /* ask again */
            }
            *spell_no = idx;
            return TRUE;
        }
    }

    int splaction = (spell_list_type >= 2 ? SPELLMENU_DETAILS : spell_list_type == 1 ? SPELLMENU_PREPARE : SPELLMENU_CAST);
    return dospellmenu(titlebuf, splaction, spell_no);
}

STATIC_OVL boolean
dospellmenu(prompt, splaction, spell_no)
const char* prompt;
int splaction; /* SPELLMENU_CAST, SPELLMENU_REORDER, or spl_book[] index */
int* spell_no;
{
    if (iflags.spell_table_format)
    {
        return dotradspellmenu(prompt, splaction, spell_no);
    }
    else
    {
        return doaltspellmenu(prompt, splaction, spell_no);
    }
}

/* an alternative implementation of the '+' command, designed to work better on mobile phones */
STATIC_OVL boolean
doaltspellmenu(prompt, splaction, spell_no)
const char* prompt;
int splaction; /* SPELLMENU_CAST, SPELLMENU_REORDER, or spl_book[] index */
int* spell_no;
{
    winid tmpwin;
    int i, n, how, splnum;
    char buf[BUFSIZ], descbuf[BUFSZ];
    menu_item* selected;
    anything any;
    const char* nodesc = "(No short description)";

    tmpwin = create_nhwindow(NHW_MENU);
    start_menu_ex(tmpwin, GHMENU_STYLE_SPELLS_ALTERNATE);
    any = zeroany; /* zero out all bits */

    if (splaction == SPELLMENU_DETAILS || splaction == SPELLMENU_REORDER || splaction == SPELLMENU_SORT || splaction >= 0)
    {
        for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) 
        {
            splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
            int glyph = spell_to_glyph(splnum);
            char fullname[BUFSZ] = "";
            Sprintf(fullname, "%s", spellname(splnum));
            *fullname = highc(*fullname);

            if (OBJ_ITEM_DESC(spellid(splnum)))
            {
                Strcpy(descbuf, OBJ_ITEM_DESC(spellid(splnum)));
            }
            else
                Strcpy(descbuf, nodesc);

            boolean inactive = FALSE;
            struct extended_menu_info info = zeroextendedmenuinfo;
            int mcolor = NO_COLOR;
            info.menu_flags |= MENU_FLAGS_USE_SPECIAL_SYMBOLS;
            if (spellknow(splnum) <= 0)
            {
                Sprintf(buf, "%s %s", fullname, "(You cannot recall this spell)");
                mcolor = CLR_BLACK;
                info.menu_flags |= MENU_FLAGS_USE_COLOR_FOR_SUFFIXES;
                inactive = TRUE;
            }
            else
            {
                Sprintf(buf, "%s (%s)", fullname, descbuf);
                mcolor = NO_COLOR;
                info.menu_flags |= MENU_FLAGS_ACTIVE;
            }
            if (!inactive)
                info.menu_flags |= MENU_FLAGS_ACTIVE;

            any.a_int = inactive ? 0 : splnum + 1; /* must be non-zero */
            add_extended_menu(tmpwin, glyph, &any, 0, 0, ATR_INDENT_AT_DOUBLE_SPACE, mcolor, buf,
                (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED, info);
        }
    }
    else if (splaction == SPELLMENU_PREPARE)
    {
        for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
        {
            add_alt_spell_prepare_menu_item(tmpwin, i, splaction);
        }
    }
    else
    {
        int splcnt = 0;
        for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
        {
            add_alt_spell_cast_menu_item(tmpwin, i, splaction);
            splcnt++;
        }
        if (splcnt > 0)
        {
            add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
                "", MENU_UNSELECTED);
            any.a_int = -1;
            int glyph = MAXSPELL + GLYPH_SPELL_TILE_OFF;
            struct extended_menu_info info = zeroextendedmenuinfo;
            info.menu_flags |= MENU_FLAGS_ACTIVE;
            add_extended_menu(tmpwin, glyph, &any, '?', 0, ATR_NONE, NO_COLOR,
                "View Spells", MENU_UNSELECTED, info);
            any.a_int = -2;
            glyph = MAXSPELL + 1 + GLYPH_SPELL_TILE_OFF;
            add_extended_menu(tmpwin, glyph, &any, '!', 0, ATR_NONE, NO_COLOR,
                "Mix Spells", MENU_UNSELECTED, info);
        }
    }

    how = PICK_ONE;
    if (splaction == SPELLMENU_REORDER) 
    {
        if (spellid(1) == NO_SPELL) 
        {
            /* only one spell => nothing to swap with */
            how = PICK_NONE;
        }
        else 
        {
            /* more than 1 spell, add an extra menu entry */
            any.a_int = SPELLMENU_SORT + 1;
            add_menu(tmpwin, NO_GLYPH, &any, '+', 0, ATR_NONE, NO_COLOR,
                "[sort spells]", MENU_UNSELECTED);
        }
    }

#ifdef GNH_MOBILE
    if ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0 && (splaction == SPELLMENU_PREPARE || splaction == SPELLMENU_CAST))
    {
        char subbuf[BUFSZ];
        if (splaction == SPELLMENU_CAST)
        {
            Strcpy(subbuf, "&success; Success %, &mana; Mana Cost, &cool; Cooldown, &casts; Casts Left");
        }
        else
        {
            Strcpy(subbuf, "&casts; Casts Left, &adds; Casts Added");
        }
        end_menu_ex(tmpwin, prompt, subbuf);

    }
    else
    {
        end_menu(tmpwin, prompt);
    }
#else 
    end_menu(tmpwin, prompt);
#endif

    //Show menu
    n = select_menu(tmpwin, how, &selected);
    destroy_nhwindow(tmpwin);

    if (n > 0) 
    {
        if (selected[0].item.a_int <= 0)
        {
            int res = selected[0].item.a_int;
            free((genericptr_t)selected);
            switch (res)
            {
            case -1:
                return dospellview();
            case -2:
                return domix();
            default:
                return 0;
            }
        }

        *spell_no = selected[0].item.a_int - 1;
        /* menu selection for `PICK_ONE' does not
           de-select any preselected entry */
        if (n > 1 && *spell_no == splaction)
            *spell_no = selected[1].item.a_int - 1;
        free((genericptr_t)selected);
        /* default selection of preselected spell means that
           user chose not to swap it with anything */
        if (*spell_no == splaction)
            return FALSE;
        return TRUE;
    }
    else if (splaction >= 0) 
    {
        /* explicit de-selection of preselected spell means that
           user is still swapping but not for the current spell */
        *spell_no = splaction;
        return TRUE;
    }
    return FALSE; 
}




STATIC_VAR int docast_spell_no = -1;

/* the 'Z' command -- cast a spell */
int
docast()
{
    if (in_doagain && docast_spell_no > -1)
    {
        return spelleffects(docast_spell_no, FALSE, &youmonst);
    }
    else
    {
        docast_spell_no = -1;
        if (getspell(&docast_spell_no, 0))
            return spelleffects(docast_spell_no, FALSE, &youmonst);
    }
    docast_spell_no = -1;
    return 0;
}


int
dospellmanage()
{
    int spell_no;
    int action = dospellmanagemenu();

    if (action <= 0)
        return 0;

    if (getspell(&spell_no, action))
    {
        if(action == 4)
            return setspellhotkey(spell_no);
        else if (action == 5)
            return forgetspell(spell_no);
    }
    return 0;
}


/* the M('z') command -- spell info / descriptions */
int
dospellview()
{
    int spell_no;
    boolean didselect = FALSE;
    do
    { 
        didselect = getspell(&spell_no, 3);
        if(didselect)
            (void)spelldescription(spell_no);
    } while (didselect);
    return 0;
}

STATIC_OVL const char *
spelltypemnemonic(skill)
int skill;
{
    switch (skill) {
    case P_ARCANE_SPELL:
        return "arcane";
    case P_HEALING_SPELL:
        return "healing";
    case P_DIVINATION_SPELL:
        return "divination";
    case P_ENCHANTMENT_SPELL:
        return "enchantment";
    case P_CLERIC_SPELL:
        return "clerical";
    case P_MOVEMENT_SPELL:
        return "movement";
    case P_TRANSMUTATION_SPELL:
        return "transmutation";
    case P_CONJURATION_SPELL:
        return "conjuration";
    case P_ABJURATION_SPELL:
        return "abjuration";
    case P_CELESTIAL_SPELL:
        return "celestial";
    case P_NATURE_SPELL:
        return "nature";
    case P_NECROMANCY_SPELL:
        return "necromancy";
    default:
        impossible("Unknown spell skill, %d;", skill);
        return empty_string;
    }
}

STATIC_OVL const char*
spelltypesymbol(skill)
int skill;
{
    if ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0)
    {
        switch (skill) {
        case P_ARCANE_SPELL:
            return "&sparc;";
        case P_HEALING_SPELL:
            return "&sphea;";
        case P_DIVINATION_SPELL:
            return "&spdiv;";
        case P_ENCHANTMENT_SPELL:
            return "&spenc;";
        case P_CLERIC_SPELL:
            return "&spcle;";
        case P_MOVEMENT_SPELL:
            return "&spmov;";
        case P_TRANSMUTATION_SPELL:
            return "&sptra;";
        case P_CONJURATION_SPELL:
            return "&spcon;";
        case P_ABJURATION_SPELL:
            return "&spabj;";
        case P_CELESTIAL_SPELL:
            return "&spcel;";
        case P_NATURE_SPELL:
            return "&spnat;";
        case P_NECROMANCY_SPELL:
            return "&spnec;";
        default:
            impossible("Unknown spell skill, %d;", skill);
            return empty_string;
        }
    }
    else
    {
        switch (skill) {
        case P_ARCANE_SPELL:
            return "Arc";
        case P_HEALING_SPELL:
            return "Hea";
        case P_DIVINATION_SPELL:
            return "Div";
        case P_ENCHANTMENT_SPELL:
            return "Enc";
        case P_CLERIC_SPELL:
            return "Cle";
        case P_MOVEMENT_SPELL:
            return "Mov";
        case P_TRANSMUTATION_SPELL:
            return "Tra";
        case P_CONJURATION_SPELL:
            return "Con";
        case P_ABJURATION_SPELL:
            return "Abj";
        case P_CELESTIAL_SPELL:
            return "Cel";
        case P_NATURE_SPELL:
            return "Nat";
        case P_NECROMANCY_SPELL:
            return "Nec";
        default:
            impossible("Unknown spell skill, %d;", skill);
            return empty_string;
        }
    }
}

int
spell_skilltype(booktype)
int booktype;
{
    return objects[booktype].oc_skill;
}

#if 0
/* attempting to cast a forgotten spell will cause disorientation */
STATIC_OVL void
spell_backfire(spell)
int spell;
{
    long duration = (long) ((spellev(spell) + 1) * 3), /* 6..24 */
         old_stun = (HStun & TIMEOUT), old_conf = (HConfusion & TIMEOUT);

    /* Prior to 3.4.1, only effect was confusion; it still predominates.
     *
     * 3.6.0: this used to override pre-existing confusion duration
     * (cases 0..8) and pre-existing stun duration (cases 4..9);
     * increase them instead.   (Hero can no longer cast spells while
     * Stunned, so the potential increment to stun duration here is
     * just hypothetical.)
     */
    switch (rn2(10)) {
    case 0:
    case 1:
    case 2:
    case 3:
        play_sfx_sound(SFX_ACQUIRE_CONFUSION);
        make_confused(old_conf + duration, FALSE); /* 40% */
        break;
    case 4:
    case 5:
    case 6:
        play_sfx_sound(SFX_ACQUIRE_CONFUSION);
        make_confused(old_conf + 2L * duration / 3L, FALSE); /* 30% */
        make_stunned(old_stun + duration / 3L, FALSE);
        break;
    case 7:
    case 8:
        play_sfx_sound(SFX_ACQUIRE_CONFUSION);
        make_stunned(old_stun + 2L * duration / 3L, FALSE); /* 20% */
        make_confused(old_conf + duration / 3L, FALSE);
        break;
    case 9:
        play_sfx_sound(SFX_ACQUIRE_STUN);
        make_stunned(old_stun + duration, FALSE); /* 10% */
        break;
    }
    return;
}
#endif

double
get_spell_mana_cost(spell)
int spell;
{
    if (spell < 0)
        return 0;

    double energy = get_spellbook_adjusted_mana_cost(spellid(spell));

    return energy;
}

double
get_spellbook_adjusted_mana_cost(otyp)
int otyp;
{
    if (otyp <= STRANGE_OBJECT || otyp >= NUM_OBJECTS)
        return 0;

    int skill = objects[otyp].oc_skill;
    int skill_level = P_SKILL_LEVEL(skill);
    double multiplier = spell_skill_mana_cost_multiplier(skill_level);
    double energy = max(0.1, ((double)objects[otyp].oc_spell_mana_cost * multiplier));

    if (energy >= 100.0)
        energy = floor(energy);

    return energy;
}

double
spell_skill_mana_cost_multiplier(skill_level)
int skill_level;
{
    double multiplier = 1.1;
    switch (skill_level)
    {
    case P_ISRESTRICTED:
    case P_UNSKILLED:
        multiplier = 1.1;
        break;
    case P_BASIC:
        multiplier = 1.0;
        break;
    case P_SKILLED:
        multiplier = 0.9;
        break;
    case P_EXPERT:
        multiplier = 0.8;
        break;
    case P_MASTER:
        multiplier = 0.7;
        break;
    case P_GRAND_MASTER:
        multiplier = 0.6;
        break;
    default:
        break;
    }

    return multiplier;
}

const char*
get_obj_saving_throw_description(obj)
struct obj* obj;
{
    if (!obj)
        return "";

    return get_otyp_saving_throw_description(obj->otyp);
}

const char*
get_otyp_saving_throw_description(otyp)
int otyp;
{
    if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_MAGIC_CANCELLATION)
        return "magic cancellation";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_STR)
        return "strength";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_DEX)
        return "dexterity";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_CON)
        return "constitution";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_INT)
        return "intelligence";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_WIS)
        return "wisdom";
    else if (objects[otyp].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_VS_CHA)
        return "charisma";
    else
        return "unknown type";
}

STATIC_OVL const char*
get_spell_type(skill)
int skill;
{
    switch (skill) {
    case P_ARCANE_SPELL:
        return "arcane";
    case P_HEALING_SPELL:
        return "healing";
    case P_DIVINATION_SPELL:
        return "divination";
    case P_ENCHANTMENT_SPELL:
        return "enchantment";
    case P_CLERIC_SPELL:
        return "clerical";
    case P_MOVEMENT_SPELL:
        return "movement";
    case P_TRANSMUTATION_SPELL:
        return "transmutation";
    case P_CONJURATION_SPELL:
        return "conjuration";
    case P_ABJURATION_SPELL:
        return "abjuration";
    case P_CELESTIAL_SPELL:
        return "celestial";
    case P_NATURE_SPELL:
        return "nature";
    case P_NECROMANCY_SPELL:
        return "necromancy";
    default:
        //impossible("Unknown spell skill, %d;", skill);    //TODO: quoted out because generating error in wiz-mode. Find out why switching to default.
        return "Not applicable";
    }
}

STATIC_OVL const char*
get_attribute(attrno)
int attrno;
{
    if (attrno >= 0)
    {
        switch (attrno)
        {
        case A_STR:
            return "Strength";
            break;
        case A_DEX:
            return "Dexterity";
            break;
        case A_CON:
            return "Constitution";
            break;
        case A_INT:
            return "Intelligence";
            break;
        case A_WIS:
            return "Wisdom";
            break;
        case A_CHA:
            return "Charisma";
            break;
        case A_MAX_INT_WIS:
            return "Higher of intelligence and wisdom";
            break;
        case A_MAX_INT_CHA:
            return "Higher of intelligence and charisma";
            break;
        case A_MAX_WIS_CHA:
            return "Higher of wisdom and charisma";
            break;
        case A_MAX_INT_WIS_CHA:
            return "Higher of intelligence, wisdom, and charisma";
            break;
        case A_AVG_INT_WIS:
            return "Average of intelligence and wisdom";
            break;
        case A_AVG_INT_CHA:
            return "Average of intelligence and charisma";
            break;
        case A_AVG_WIS_CHA:
            return "Average of wisdom and charisma";
            break;
        case A_AVG_INT_WIS_CHA:
            return "Average of intelligence, wisdom, and charisma";
            break;
        default:
            return "Not applicable";
            break;
        }
    }
    return "None";
}

STATIC_OVL const char*
get_targeting(skill)
unsigned int skill;
{
    if (skill > 0)
    {
        switch (skill)
        {
        case NODIR:
            return "None";
            break;
        case IMMEDIATE:
            return "One target in selected direction";
            break;
        case RAY:
            if (skill & S1_SPELL_EXPLOSION_EFFECT)                  // TODO: S1_SPELL_EXPLOSION_EFFECT necessary? Bitwise and?
                return "Ray that explodes on hit";
            else
                return "Ray in selected direction";
            break;
        case TARGETED:
            return "Target selected on screen";
            break;
        case TOUCH:
            return "Touch";
            break;
        case IMMEDIATE_MULTIPLE_TARGETS:
            return "Multiple targets in selected direction";
            break;
        case IMMEDIATE_ONE_TO_THREE_TARGETS:
            return "1/2/3 targets in selected direction depending on blessedness";
            break;
        case IMMEDIATE_ONE_TO_SEVEN_TARGETS:
            return "1/4/7 targets in selected direction depending on blessedness";
            break;
        case IMMEDIATE_TWO_TO_SIX_TARGETS:
            return "2/4/6 targets in selected direction depending on blessedness";
            break;
        default:
            return "Targeting not applicable";
            break;
        }
    }
    else
        return "Targeting skill was <=0, not applicable";
}

STATIC_OVL const char*
get_spell_level_bonus(skill)
int skill; 
{
    char buf[BUFSIZ];
    char plusbuf[BUFSIZ];
    boolean maindiceprinted = FALSE;
    
    Sprintf(buf, "- **Level bonus:**  ");

    if ((objects[skill].oc_spell_flags & S1_LDMG_IS_PER_LEVEL_DMG_INCREASE) && objects[skill].oc_spell_per_level_step > 0)      // TODO: Bitwise AND needed here?
    {
        if (objects[skill].oc_spell_per_level_dice > 0 && objects[skill].oc_spell_per_level_diesize > 0)
        {
            maindiceprinted = TRUE;
            Sprintf(plusbuf, "            %dd%d", objects[skill].oc_spell_per_level_dice, objects[skill].oc_spell_per_level_diesize);
            Strcat(buf, plusbuf);
        }

        if (objects[skill].oc_spell_per_level_plus != 0)
        {
            if (maindiceprinted && objects[skill].oc_spell_per_level_plus > 0)
            {
                Sprintf(plusbuf, "+");
                Strcat(buf, plusbuf);
            }
            Sprintf(plusbuf, "%d", objects[skill].oc_spell_per_level_plus);
            Strcat(buf, plusbuf);
        }

        if (objects[skill].oc_spell_per_level_step == 1)
            Sprintf(plusbuf, " per caster level");
        else
            Sprintf(plusbuf, " per %ld caster levels", objects[skill].oc_spell_per_level_step);

        Strcat(buf, plusbuf);
    }
    else
    {
        Sprintf(buf, "");
    }
    
    return buf;
}

STATIC_OVL const char*
get_components(skill)
int skill;
{
    char buf[BUFSIZ];
    char buf2[BUFSIZ];

    if (objects[skill].oc_spell_flags & S1_SPELL_BYPASSES_MAGIC_RESISTANCE)
    {
        if (objects[skill].oc_spell_flags & S1_SPELL_BYPASSES_UNIQUE_MONSTER_MAGIC_RESISTANCE)
            Sprintf(buf, "Other:            %s", "Bypasses magic resistance for all monsters");
        else
            Sprintf(buf, "Other:            %s", "Bypasses magic resistance for non-unique monsters");
    }

    Strcpy(buf2, "");
    if (!(objects[skill].oc_spell_flags & S1_NO_VERBAL_COMPONENT))
        Strcpy(buf2, "Verbal");

    if (!(objects[skill].oc_spell_flags & S1_NO_SOMATIC_COMPONENT))
    {
        if (strcmp(buf2, ""))
            Strcpy(eos(buf2), ", ");

        Strcpy(eos(buf2), "Somatic");
    }

    if (objects[skill].oc_material_components > 0)
    {
        if (strcmp(buf2, ""))
            Strcpy(eos(buf2), ", ");

        Strcpy(eos(buf2), "Material");
    }

    if (!strcmp(buf2, ""))
        Strcpy(buf2, "None");

    Sprintf(buf, "Components:               %s", buf2);
    
    return buf;
}

STATIC_OVL const char*
get_material_components(skill)
int skill;
{
    char buf[BUFSIZ];
    char buf2[BUFSIZ];

    if (objects[skill].oc_material_components > 0)
    {
        Sprintf(buf2, "%d casting%s", matlists[objects[skill].oc_material_components].spellsgained,
            matlists[objects[skill].oc_material_components].spellsgained == 1 ? "" : "s");

        Sprintf(buf, "Material components - %s:  \n", buf2);

        for (int i = 0; matlists[objects[skill].oc_material_components].matcomp[i].objectid[0] > 0; i++)
        {
            Sprintf(buf2, "%d\. %s%s  \n", (i+1), domatcompname(&matlists[objects[skill].oc_material_components].matcomp[i]),
                ((matlists[objects[skill].oc_material_components].matcomp[i].flags & MATCOMP_NOT_SPENT) ? " as a catalyst" : ""));
            
            (void)strcat(buf, buf2);
        }
    }
    else
        Sprintf(buf, "%s  \n", "No material components");

    return buf;
}

int
export_spells_for_wiki() {                              // Copied mostly from spelldescription(), below

    pline("Starting writing spells...");

    (void)mkdir("\\spells_for_wiki\\");                 // TODO: Is it needed to have something checking if the folder exists?

    char* cwd[BUFSIZ];
    char fq_save[BUFSIZ];
    char name[BUFSIZ];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        (void)strcat(cwd, "\\spells_for_wiki\\");
        //pline("Current working dir: %s\n", cwd);
    }
    else {
        pline("getcwd() error");
        return 0;
    }

    for (int i = FIRST_SPELL; i <= SPE_BOOK_OF_THE_DEAD; i++) {

        name[0] = '\0';
        (void)strcat(name, obj_descr[objects[i].oc_name_idx].oc_name);
        for (int j = 0; j < strlen(name); j++) {

            if (name[j] == ' ') name[j] = '-';
        }

        *name = highc(*name);

        fq_save[0] = '\0';
        (void)strcat(fq_save, cwd);
        (void)strcat(fq_save, name);
        (void)strcat(fq_save, ".md");

        int fd;

        (void)remove(fq_save);

#ifdef MAC
        fd = macopen(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, TEXT_TYPE);
#else
        fd = open(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, FCMASK);
#endif
        char buf[BUFSIZ];
        char plusbuf[BUFSIZ];

        // Spell level
        if (objects[i].oc_spell_level == -1) {
            Sprintf(buf, "## Minor %s cantrip\n\n", get_spell_type(objects[i].oc_skill));
            (void)write(fd, buf, strlen(buf));
        }
        else if (objects[i].oc_spell_level == 0) {
            Sprintf(buf, "## Major %s cantrip\n\n", get_spell_type(objects[i].oc_skill));
            (void)write(fd, buf, strlen(buf));
        }
        else if (objects[i].oc_spell_level > 0) {
            Sprintf(buf, "## Level %ld %s spell\n\n", objects[i].oc_spell_level, get_spell_type(objects[i].oc_skill));
            (void)write(fd, buf, strlen(buf));
        }
        else {
            Sprintf(buf, "## Spell of inappropriate level\n\n");
            (void)write(fd, buf, strlen(buf));
        }

        // Spell primary casting attribute
        Sprintf(buf, "- **Attribute:**                %s  \n", get_attribute(objects[i].oc_spell_attribute));
        (void)write(fd, buf, strlen(buf));

        // Spell mana cost
        Sprintf(buf, "- **Mana cost:**                %.1f  \n", (double)objects[i].oc_spell_mana_cost);    // TODO: does this need further calculation?        
        (void)write(fd, buf, strlen(buf));

        // Spell casting time
        if (objects[i].oc_spell_flags & S1_DOES_NOT_TAKE_A_TURN)                                            // TODO: S1_DOES_NOT_TAKE_A_TURN necessary?
            Sprintf(buf, "- **Casting time:**             0 rounds  \n");
        else
            Sprintf(buf, "- **Casting time:**             1 round  \n");

        (void)write(fd, buf, strlen(buf));

        // Spell cooldown
        if (objects[i].oc_spell_cooldown > 0)
            Sprintf(buf, "- **Spell cooldown:**        %ld round%s  \n", objects[i].oc_spell_cooldown, objects[i].oc_spell_cooldown == 1 ? "" : "s");
        else
            Sprintf(buf, "- **Spell cooldown:**           None  \n");

        (void)write(fd, buf, strlen(buf));

        // Spell targeting direction
        Sprintf(buf, "- **Targeting:**                %s  \n", get_targeting(objects[i].oc_dir));
        (void)write(fd, buf, strlen(buf));

        // Spell range
        if (objects[i].oc_spell_range > 0)
            Sprintf(buf, "- **Range:**                    %ld'  \n", objects[i].oc_spell_range * 5L);
        else
            Sprintf(buf, "- **Range:**                     0'  \n");
        (void)write(fd, buf, strlen(buf));

        // Damage or healing
        if (objects[i].oc_spell_dmg_dice != 0 && objects[i].oc_spell_dmg_diesize != 0) {
            if (objects[i].oc_skill == P_HEALING_SPELL)
                Sprintf(buf, "- **Healing:**                  ");
            else
                Sprintf(buf, "- **Damage:**                   ");

            Sprintf(plusbuf, "%dd%d  \n", objects[i].oc_spell_dmg_dice, objects[i].oc_spell_dmg_diesize);
            (void)strcat(buf, plusbuf);
            (void)write(fd, buf, strlen(buf));
        }

        // Spell level bonus
        Sprintf(buf, get_spell_level_bonus(i));
        Sprintf(plusbuf, "  \n");
        (void)strcat(buf, plusbuf);
        (void)write(fd, buf, strlen(buf));

        // Spell damage type
        if (objects[i].oc_damagetype != AD_NONE)
        {
            char dmgttext[BUFSIZ] = "";
            Strcpy(dmgttext, get_damage_type_text(objects[i].oc_damagetype));
            *dmgttext = highc(*dmgttext);
            if (strcmp(dmgttext, "") != 0)
            {
                Sprintf(buf, "- **Damage type:**              %s  \n", dmgttext);
                (void)write(fd, buf, strlen(buf));
            }
        }

        // Spell skill chance
        Sprintf(buf, "- **Train chance:**             %ld%%  \n", objects[i].oc_spell_skill_chance);
        (void)write(fd, buf, strlen(buf));

        // Spell components
        Sprintf(buf, "### %s  \n", get_components(i));
        (void)write(fd, buf, strlen(buf));

        // Spell material components
        Sprintf(buf, "### ");
        Sprintf(plusbuf, "%s", get_material_components(i));
        (void)strcat(buf, plusbuf);
        (void)write(fd, buf, strlen(buf));

        // Spell description
        Sprintf(buf, "## Description:  \n");
        Sprintf(plusbuf, "%s", obj_descr[objects[i].oc_name_idx].oc_item_description);
        (void)write(fd, buf, strlen(buf));
        (void)write(fd, plusbuf, strlen(plusbuf));
        
        (void)close(fd);
    }

    pline("Done!");

    return 0;
}

int
spelldescription(spell)
int spell;
{

    if (spellknow(spell) <= 0)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_WARNING, "cannot recall this spell anymore.");
        return 0;
    }

    winid datawin = WIN_ERR;
    int glyph = spell_to_glyph(spell);
    datawin = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_SPELL_DESCRIPTION_SCREEN, glyph, extended_create_window_info_from_spell(spell, TRUE));

    int booktype = spellid(spell);
    char buf[BUFSZ];
    char buf2[BUFSZ];
    char buf3[BUFSZ];

    /* Name */
    Strcpy(buf, spellname(spell));
    *buf = highc(*buf);    
    putstr(datawin, ATR_TITLE, buf);

    /* Level & category*/
    print_spell_level_text(buf, booktype, TRUE, TRUE);    
    putstr(datawin, ATR_SUBTITLE, buf);

    /* One empty line here */
    Strcpy(buf, "");    
    putstr(datawin, 0, buf);

    /* Attributes*/
    if (objects[booktype].oc_spell_attribute >= 0)
    {
        char statbuf[BUFSZ];
        switch (objects[booktype].oc_spell_attribute)
        {
        case A_STR:
            Strcpy(statbuf, "Strength");
            break;
        case A_DEX:
            Strcpy(statbuf, "Dexterity");
            break;
        case A_CON:
            Strcpy(statbuf, "Constitution");
            break;
        case A_INT:
            Strcpy(statbuf, "Intelligence");
            break;
        case A_WIS:
            Strcpy(statbuf, "Wisdom");
            break;
        case A_CHA:
            Strcpy(statbuf, "Charisma");
            break;
        case A_MAX_INT_WIS:
            Strcpy(statbuf, "Higher of intelligence and wisdom");
            break;
        case A_MAX_INT_CHA:
            Strcpy(statbuf, "Higher of intelligence and charisma");
            break;
        case A_MAX_WIS_CHA:
            Strcpy(statbuf, "Higher of wisdom and charisma");
            break;
        case A_MAX_INT_WIS_CHA:
            Strcpy(statbuf, "Higher of intelligence, wisdom, and charisma");
            break;
        case A_AVG_INT_WIS:
            Strcpy(statbuf, "Average of intelligence and wisdom");
            break;
        case A_AVG_INT_CHA:
            Strcpy(statbuf, "Average of intelligence and charisma");
            break;
        case A_AVG_WIS_CHA:
            Strcpy(statbuf, "Average of wisdom and charisma");
            break;
        case A_AVG_INT_WIS_CHA:
            Strcpy(statbuf, "Average of intelligence, wisdom, and charisma");
            break;
        default:
            Strcpy(statbuf, "Not applicable");
            break;
        }

        Sprintf(buf, "Attributes:       %s", statbuf);        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Success percentage */
    int successpct = percent_success(spell, TRUE);
    int successpct_unlimited = percent_success(spell, FALSE);
    Strcpy(buf2, "");
    if(successpct_unlimited < 0 || successpct_unlimited > 100)
        Sprintf(buf2, " (base %d%%)", successpct_unlimited);
    Sprintf(buf, "Success chance:   %d%%%s", successpct, buf2);
    putstr(datawin, ATR_INDENT_AT_COLON, buf);

    /* Mana cost*/
    double manacost = get_spellbook_adjusted_mana_cost(booktype);
    if (manacost > 0)
    {
        double displayed_manacost = ceil(10 * manacost) / 10;
        Sprintf(buf2, "%.*f", displayed_manacost >= 100 ? 0 : 1, displayed_manacost);
    }
    else
    {
        Strcpy(buf2, "None");
    }
    Sprintf(buf, "Mana cost:        %s", buf2);    
    putstr(datawin, ATR_INDENT_AT_COLON, buf);

    /* Casting time*/
    if(objects[booktype].oc_spell_flags & S1_DOES_NOT_TAKE_A_TURN)
        Strcpy(buf2, "0 rounds");
    else
        Strcpy(buf2, "1 round");

    Sprintf(buf, "Casting time:     %s", buf2);    
    putstr(datawin, ATR_INDENT_AT_COLON, buf);

    /* Cooldown */
    if (objects[booktype].oc_spell_cooldown > 0)
    {
        Sprintf(buf2, "%ld round%s", objects[booktype].oc_spell_cooldown, objects[booktype].oc_spell_cooldown == 1 ? "" : "s");
    }
    else
    {
        Sprintf(buf2, "None");
    }

    Sprintf(buf, "Cooldown:         %s", buf2);    
    putstr(datawin, ATR_INDENT_AT_COLON, buf);

    /* DirType */
    if (objects[booktype].oc_dir > 0)
    {
        Strcpy(buf2, "");
        switch (objects[booktype].oc_dir)
        {
        case NODIR:
            Strcpy(buf2, "None");
            break;
        case IMMEDIATE:
            Strcpy(buf2, "One target in selected direction");
            break;
        case RAY:
            if(objects[booktype].oc_spell_flags & S1_SPELL_EXPLOSION_EFFECT)
                Strcpy(buf2, "Ray that explodes on hit");
            else
                Strcpy(buf2, "Ray in selected direction");
            break;
        case TARGETED:
            Strcpy(buf2, "Target selected on screen");
            break;
        case TOUCH:
            Strcpy(buf2, "Touch");
            break;
        case IMMEDIATE_MULTIPLE_TARGETS:
            Strcpy(buf2, "Multiple targets in selected direction");
            break;
        case IMMEDIATE_ONE_TO_THREE_TARGETS:
            Strcpy(buf2, "1/2/3 targets in selected direction depending on blessedness");
            break;
        case IMMEDIATE_ONE_TO_SEVEN_TARGETS:
            Strcpy(buf2, "1/4/7 targets in selected direction depending on blessedness");
            break;
        case IMMEDIATE_TWO_TO_SIX_TARGETS:
            Strcpy(buf2, "2/4/6 targets in selected direction depending on blessedness");
            break;
        default:
            break;
        }
        Sprintf(buf, "Targeting:        %s", buf2);
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Range */
    if (objects[booktype].oc_spell_range > 0)
    {
        Sprintf(buf, "Range:            %ld'", objects[booktype].oc_spell_range * 5L);        
        putstr(datawin, 0, buf);
    }

    /* Radius */
    if (objects[booktype].oc_spell_radius > 0)
    {
        Sprintf(buf, "Radius:           %ld'", objects[booktype].oc_spell_radius * 5L);        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Damage or Healing */
    boolean damageprinted = FALSE;
    double baseavg = 0;
    double perlevelavg = 0;
    int used_level = 0;
    int used_bonuses = 1;

    if (objects[booktype].oc_spell_dmg_dice > 0 || objects[booktype].oc_spell_dmg_diesize > 0 || objects[booktype].oc_spell_dmg_plus > 0)
    {
        damageprinted = TRUE;

        char plusbuf[BUFSZ];
        boolean maindiceprinted = FALSE;

        if (objects[booktype].oc_skill == P_HEALING_SPELL)
            Sprintf(buf, "Healing:          ");
        else
            Sprintf(buf, "Damage:           ");

        if (objects[booktype].oc_spell_dmg_dice > 0 && objects[booktype].oc_spell_dmg_diesize > 0)
        {
            maindiceprinted = TRUE;
            Sprintf(plusbuf, "%dd%d", objects[booktype].oc_spell_dmg_dice, objects[booktype].oc_spell_dmg_diesize);
            Strcat(buf, plusbuf);
            baseavg += (double)objects[booktype].oc_spell_dmg_dice * (double)(1.0 + objects[booktype].oc_spell_dmg_diesize) / 2.0;
        }

        if (objects[booktype].oc_spell_dmg_plus != 0)
        {
            if (maindiceprinted && objects[booktype].oc_spell_dmg_plus > 0)
            {
                Sprintf(plusbuf, "+");
                Strcat(buf, plusbuf);
            }
            Sprintf(plusbuf, "%d", objects[booktype].oc_spell_dmg_plus);
            Strcat(buf, plusbuf);
            baseavg += (double)objects[booktype].oc_spell_dmg_plus;
        }

        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);

        /* Per level */
        if ((objects[booktype].oc_spell_flags & S1_LDMG_IS_PER_LEVEL_DMG_INCREASE) && objects[booktype].oc_spell_per_level_step > 0)
        {
            Sprintf(buf, "Level bonus:      ");
            if (objects[booktype].oc_spell_per_level_dice > 0 && objects[booktype].oc_spell_per_level_diesize > 0)
            {
                maindiceprinted = TRUE;
                Sprintf(plusbuf, "%dd%d", objects[booktype].oc_spell_per_level_dice, objects[booktype].oc_spell_per_level_diesize);
                Strcat(buf, plusbuf);
                perlevelavg += (double)objects[booktype].oc_spell_per_level_dice * (double)(1.0 + objects[booktype].oc_spell_per_level_diesize) / 2.0;
            }

            if (objects[booktype].oc_spell_per_level_plus != 0)
            {
                if (maindiceprinted && objects[booktype].oc_spell_per_level_plus > 0)
                {
                    Sprintf(plusbuf, "+");
                    Strcat(buf, plusbuf);
                }
                Sprintf(plusbuf, "%d", objects[booktype].oc_spell_per_level_plus);
                Strcat(buf, plusbuf);
                perlevelavg += (double)objects[booktype].oc_spell_per_level_plus;
            }

            if(objects[booktype].oc_spell_per_level_step == 1)
                Sprintf(eos(buf), " per caster level");
            else
                Sprintf(eos(buf), " per %ld caster levels", objects[booktype].oc_spell_per_level_step);
            
            putstr(datawin, ATR_INDENT_AT_COLON, buf);

            int max_level = get_maximum_applicable_spell_damage_level(booktype, &youmonst);
            used_level = min(max_level, u.ulevel);
            used_bonuses = used_level / (max(1, objects[booktype].oc_spell_per_level_step));

            if(max_level < MAXULEV)
            {
                int skill_type = objects[booktype].oc_skill;
                char skillnamebuf[BUFSZ];
                char skilllevelnamebuf[BUFSZ];

                (void)skill_level_name(skill_type, skilllevelnamebuf, FALSE);
                Strcpy(skillnamebuf, skill_name(skill_type, TRUE));
                //*skilllevelnamebuf = lowc(*skilllevelnamebuf);

                Sprintf(buf, "Level limit:      %d (%s at %s)", max_level, skilllevelnamebuf, skillnamebuf);                
                putstr(datawin, ATR_INDENT_AT_COLON, buf);
            }
        }
    }

    boolean damagetypeprinted = FALSE;
    if (objects[booktype].oc_damagetype != AD_NONE && damageprinted)
    {
        damagetypeprinted = TRUE;
        char dmgttext[BUFSZ] = "";
        Strcpy(dmgttext, get_damage_type_text(objects[booktype].oc_damagetype));
        *dmgttext = highc(*dmgttext);
        if (strcmp(dmgttext, "") != 0)
        {
            Sprintf(buf, "Damage type:      %s", dmgttext);            
            putstr(datawin, ATR_INDENT_AT_COLON, buf);
        }
    }
    
    if (objects[booktype].oc_dir_subtype > 0)
    {
        if (objects[booktype].oc_dir == RAY && !damagetypeprinted)
        {
            Strcpy(buf2, "");
            Strcpy(buf3, "Effect");
            switch (objects[booktype].oc_dir_subtype)
            {
            case RAY_MAGIC_MISSILE:
                Strcpy(buf2, "Force damage");
                Strcpy(buf3, "Damage");
                break;
            case RAY_FIRE:
                Strcpy(buf2, "Fire damage");
                Strcpy(buf3, "Damage");
                break;
            case RAY_COLD:
                Strcpy(buf2, "Cold damage");
                Strcpy(buf3, "Damage");
                break;
            case RAY_SLEEP:
                Strcpy(buf2, "Sleeping");
                Strcpy(buf3, "Effect");
                break;
            case RAY_DISINTEGRATION:
                Strcpy(buf2, "Disintegration");
                Strcpy(buf3, "Effect");
                break;
            case RAY_LIGHTNING:
                Strcpy(buf2, "Lightning damage");
                Strcpy(buf3, "Damage");
                break;
            case RAY_POISON_GAS:
                Strcpy(buf2, "Poison gas");
                Strcpy(buf3, "Damage");
                break;
            case RAY_ACID:
                Strcpy(buf2, "Acid damage");
                Strcpy(buf3, "Damage");
                break;
            case RAY_DEATH:
                Strcpy(buf2, "Death");
                Strcpy(buf3, "Effect");
                break;
            case RAY_DIGGING:
                Strcpy(buf2, "Digs stone");
                Strcpy(buf3, "Effect");
                break;
            case RAY_EVAPORATION:
                Strcpy(buf2, "Evaporates water");
                Strcpy(buf3, "Effect");
                break;
            default:
                break;
            }
            Sprintf(buf, "%s type:  %s", buf3, buf2);
            putstr(datawin, ATR_INDENT_AT_COLON, buf);
        }
        else if (objects[booktype].oc_dir == NODIR)
        {
            Strcpy(buf2, "");
            Strcpy(buf3, "Effect");
            int j;
            for(j = 0; propertynames[j].prop_num; j++)
            { 
                if (propertynames[j].prop_num == objects[booktype].oc_dir_subtype)
                {
                    Strcpy(buf2, propertynames[j].prop_name);
                    *buf2 = highc(*buf2);
                    break;
                }
            }
            if (strcmp(buf2, "") != 0) // Something else than ""
            {
                Sprintf(buf, "%s type:      %s", buf3, buf2);
                putstr(datawin, ATR_INDENT_AT_COLON, buf);
            }
        }
    }

    /* Duration */
    if (!has_spell_otyp_per_level_bonus(booktype) && (objects[booktype].oc_spell_dur_dice > 0 || objects[booktype].oc_spell_dur_diesize > 0 || objects[booktype].oc_spell_dur_plus > 0))
    {
        char plusbuf[BUFSZ];
        boolean maindiceprinted = FALSE;

        Sprintf(buf, "Duration:         ");

        if (objects[booktype].oc_spell_dur_dice > 0 && objects[booktype].oc_spell_dur_diesize > 0)
        {
            maindiceprinted = TRUE;
            Sprintf(plusbuf, "%dd%d", objects[booktype].oc_spell_dur_dice, objects[booktype].oc_spell_dur_diesize);
            Strcat(buf, plusbuf); 
        }

        if (objects[booktype].oc_spell_dur_plus != 0)
        {
            if (maindiceprinted && objects[booktype].oc_spell_dur_plus > 0)
            {
                Sprintf(plusbuf, "+");
                Strcat(buf, plusbuf);
            }
            Sprintf(plusbuf, "%d", objects[booktype].oc_spell_dur_plus);
            Strcat(buf, plusbuf);
        }
        Sprintf(plusbuf, " round%s", (objects[booktype].oc_spell_dur_dice == 0 && objects[booktype].oc_spell_dur_diesize == 0 && objects[booktype].oc_spell_dur_plus == 1) ? "" : "s");
        Strcat(buf, plusbuf);        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Saving throw adjustment */
    if (objects[booktype].oc_spell_flags & S1_FLAGS_EFFECT_USES_SAVING_THROW_MASK)
    {
        Sprintf(buf, "Saving throw:     Against %s", get_otyp_saving_throw_description(booktype));        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);

        char baseadjbuf[BUFSZ];
        int baseadj = objects[booktype].oc_spell_saving_throw_adjustment;
        Sprintf(baseadjbuf, "%s%d", baseadj >= 0 ? "+" : "", baseadj);

        int skill_level = P_SKILL_LEVEL(objects[booktype].oc_skill);
        int skilladj = get_spell_skill_level_saving_throw_adjustment(skill_level);
        char skill_level_namebuf[BUFSZ] = "";
        (void)skill_level_name(objects[booktype].oc_skill, skill_level_namebuf, FALSE);
        *skill_level_namebuf = lowc(*skill_level_namebuf);

        char skilladjbuf[BUFSZ];
        Sprintf(skilladjbuf, "%s%d", skilladj >= 0 ? "+" : "", skilladj);

        int totaladj = baseadj + skilladj;
        char totaladjbuf[BUFSZ];
        Sprintf(totaladjbuf, "%s%d", totaladj >= 0 ? "+" : "", totaladj);

        Sprintf(buf, "Save adjustment:  %s (%s base, %s %s skill)", totaladjbuf, baseadjbuf, skilladjbuf, skill_level_namebuf);        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Magic resistance */
    if (objects[booktype].oc_spell_flags & S1_FLAGS_EFFECT_USES_MAGIC_RESISTANCE_MASK)
    {

        Sprintf(buf, "Magic resistance: %s by magic resistance", 
            (objects[booktype].oc_spell_flags & S1_FLAGS_EFFECT_USES_MAGIC_RESISTANCE) ? "Affected" : 
            (objects[booktype].oc_spell_flags & S1_FLAGS_EFFECT_DOES_NOT_USE_MAGIC_RESISTANCE) ? "Unaffected" : "Unknown if affected");
        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    /* Skill chance */
    Sprintf(buf, "Train chance:     %ld%%", objects[booktype].oc_spell_skill_chance);    
    putstr(datawin, ATR_INDENT_AT_COLON, buf);

    /* Flags */
    if (objects[booktype].oc_spell_flags & S1_SPELL_BYPASSES_MAGIC_RESISTANCE)
    {
        if (objects[booktype].oc_spell_flags & S1_SPELL_BYPASSES_UNIQUE_MONSTER_MAGIC_RESISTANCE)
            Sprintf(buf, "Other:            %s", "Bypasses magic resistance for all monsters");
        else
            Sprintf(buf, "Other:            %s", "Bypasses magic resistance for non-unique monsters");
        
        putstr(datawin, ATR_INDENT_AT_COLON, buf);
    }

    Strcpy(buf2, "");
    if (!(objects[booktype].oc_spell_flags & S1_NO_VERBAL_COMPONENT))
        Strcpy(buf2, "Verbal");

    if (!(objects[booktype].oc_spell_flags & S1_NO_SOMATIC_COMPONENT))
    {
        if (strcmp(buf2,""))
            Strcpy(eos(buf2), ", ");

        Strcpy(eos(buf2), "Somatic");
    }

    if (objects[booktype].oc_material_components > 0)
    {
        if (strcmp(buf2, ""))
            Strcpy(eos(buf2), ", ");

        Strcpy(eos(buf2), "Material");
    }

    if (!strcmp(buf2, ""))
        Strcpy(buf2, "None");

    Sprintf(buf, "Components:       %s", buf2);    
    putstr(datawin, ATR_INDENT_AT_COLON, buf);


    /* Material components */
    if (objects[booktype].oc_material_components > 0)
    {
        Sprintf(buf2, "%d casting%s", matlists[objects[booktype].oc_material_components].spellsgained,
            matlists[objects[booktype].oc_material_components].spellsgained == 1 ? "" : "s");

        Sprintf(buf, "Material components - %s:", buf2);        
        putstr(datawin, ATR_HEADING, buf);

        int j;
        for (j = 0; matlists[objects[booktype].oc_material_components].matcomp[j].objectid[0] > 0; j++)
        {
            Sprintf(buf, " %2d - %s%s", (j + 1), domatcompname(&matlists[objects[booktype].oc_material_components].matcomp[j]),
                ((matlists[objects[booktype].oc_material_components].matcomp[j].flags & MATCOMP_NOT_SPENT) ? " as a catalyst": ""));
            
            putstr(datawin, ATR_INDENT_AT_DASH, buf);
        }
    }

    /* Statistics */
    if (damageprinted)
    {
        int cnt = 1;
        Strcpy(buf, "Spell statistics:");        
        putstr(datawin, ATR_HEADING, buf);

        if (damageprinted)
        {
            double avg = baseavg + perlevelavg * ((double)used_bonuses);
            Sprintf(buf, "  %d - Average damage is %.1f", cnt, avg);            
            putstr(datawin, ATR_INDENT_AT_DASH, buf);
            cnt++;
        }
    }

    /* Description*/
    if (OBJ_ITEM_DESC(booktype))
    {
#if 0
        /* One empty line here */
        Strcpy(buf, "");        
        putstr(datawin, 0, buf);
#endif
        Strcpy(buf, "Description:");
        putstr(datawin, ATR_INDENT_AT_DOUBLE_SPACE | ATR_HEADING, buf);
        //Sprintf(buf, "  %s", OBJ_ITEM_DESC(booktype));
        //putstr(datawin, 0, buf);
        char descbuf[8 * BUFSZ];
        Strcpy(descbuf, OBJ_ITEM_DESC(booktype));
        char* bp = descbuf;
        char* ebp;
        while (bp && *bp)
        {
            ebp = strstr(bp, "\n");
            if (ebp)
                *ebp = 0;

            if (strlen(bp) > 0)
            {
                Sprintf(buf, "  %s", bp);
                putstr(datawin, ATR_INDENT_AT_DOUBLE_SPACE, buf);
            }

            if (!ebp)
                break;
            else
                bp = ebp + 1;
        }
    }

    display_nhwindow(datawin, FALSE);
    destroy_nhwindow(datawin), datawin = WIN_ERR;

    return 0;
}

int
spelleffects(spell, atme, targetmonst)
int spell;
boolean atme;
struct monst* targetmonst;
{
    double damage = 0;
    int chance, n; // , intell;
    int otyp, skill, role_skill, res = 0;
    boolean confused = (Confusion != 0);
    boolean stunned = (Stunned != 0);
    struct obj *pseudo;
    boolean effect_happened = 1;
    //coord cc;

    /*
     * Reject attempting to cast while stunned or with no free hands.
     * Already done in getspell() to stop casting before choosing
     * which spell, but duplicated here for cases where spelleffects()
     * gets called directly for ^T without intrinsic teleport capability
     * or #turn for non-priest/non-knight.
     * (There's no duplication of messages; when the rejection takes
     * place in getspell(), we don't get called.)
     */
    if (rejectcasting()) {
        return 0; /* no time elapses */
    }

    if (reject_specific_spell_casting(spell)) {
        return 0; /* no time elapses */
    }

    if (spellamount(spell) == 0) {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "do not have the spell's material components prepared.");
        skip_savech = 1;
        char ans = yn_query_ex(ATR_NONE, CLR_MSG_FAIL, "No Components Mixed", "Do you want to mix the spell's material components now?");
        if (ans == 'y')
        {
            in_doagain = 0;
            res = domaterialcomponentsmenu(spell);
            skip_savech = 0;
            return res;
        }

        skip_savech = 0;
        return 0; /* no time elapses */

    }

    if (spellcooldownleft(spell) > 0) {
        play_sfx_sound(SFX_NOT_READY_YET);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "cannot cast the spell before the cooldown has expired.");
        return 0; /* no time elapses */
    }

    //This might happen with amnesia etc., the spells no longer "age"
    if (spellknow(spell) <= 0)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "cannot recall this spell anymore.");
        return 0;
    }

    /*
     * Spell casting no longer affects knowledge of the spell. A
     * decrement of spell knowledge is done every turn.
     */
    /*
    if (spellknow(spell) <= 0) {
        Your("knowledge of this spell is twisted.");
        pline("It invokes nightmarish images in your mind...");
        spell_backfire(spell);
        return 1;
    } else if (spellknow(spell) <= SPELL_IS_KEEN / 200) { // 100 turns left
        You("strain to recall the spell.");
    } else if (spellknow(spell) <= SPELL_IS_KEEN / 40) { // 500 turns left
        You("have difficulty remembering the spell.");
    } else if (spellknow(spell) <= SPELL_IS_KEEN / 20) { // 1000 turns left
        Your("knowledge of this spell is growing faint.");
    } else if (spellknow(spell) <= SPELL_IS_KEEN / 10) { // 2000 turns left
        Your("recall of this spell is gradually fading.");
    }
    */
    /*
     *  Note: dotele() also calculates energy use and checks nutrition
     *  and strength requirements; it any of these change, update it too.
     */
    //energy = (spellev(spell) * 5);
    /* 5 <= energy <= 35 */

    double denergy = get_spell_mana_cost(spell);

    /*if (u.uhunger <= 10 && spellid(spell) != SPE_DETECT_FOOD) {
        You("are too hungry to cast that spell.");
        return 0;
    } else*/
    if (ACURR(A_STR) < 4 && spellid(spell) != SPE_RESTORE_ABILITY) {
        play_sfx_sound(SFX_GENERAL_NOT_ENOUGH_STAMINA);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "lack the strength to cast spells.");
        return 0;
    } else if (check_capacity(
                "Your concentration falters while carrying so much stuff.")) {
        play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
        return 1;
    }

#if 0
    /* if the cast attempt is already going to fail due to insufficient
       energy (ie, u.uen < energy), the Amulet's drain effect won't kick
       in and no turn will be consumed; however, when it does kick in,
       the attempt may fail due to lack of energy after the draining, in
       which case a turn will be used up in addition to the energy loss */

    if (u.uhave.amulet && u.uen >= energy) {
        You_feel("the amulet draining your energy away.");
        /* this used to be 'energy += rnd(2 * energy)' (without 'res'),
           so if amulet-induced cost was more than u.uen, nothing
           (except the "don't have enough energy" message) happened
           and player could just try again (and again and again...);
           now we drain some energy immediately, which has a
           side-effect of not increasing the hunger aspect of casting */
        u.uen -= rnd(2 * energy);
        if (u.uen < 0)
            u.uen = 0;
        context.botl = 1;
        res = 1; /* time is going to elapse even if spell doesn't get cast */
    }
#endif
    double dumana = (double)u.uen + ((double)u.uen_fraction) / 10000;
    if (denergy > dumana)
    {
        play_sfx_sound(SFX_NOT_ENOUGH_MANA);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "don't have enough mana to cast that spell.");
        return res;
    } 
    //else {
        /* If hero is a wizard, their current intelligence
             * (bonuses + temporary + current)
             * affects hunger reduction in casting a spell.
             * 1. int = 17-18 no reduction
             * 2. int = 16    1/4 hungr
             * 3. int = 15    1/2 hungr
             * 4. int = 1-14  normal reduction
             * The reason for this is:
             * a) Intelligence affects the amount of exertion
             * in thinking.
             * b) Wizards have spent their life at magic and
             * understand quite well how to cast spells.
             */
        /*
        if (spellid(spell) != SPE_DETECT_FOOD) {
            int hungr = energy * 2;

            
            intell = acurr(A_INT);
            if (!Role_if(PM_WIZARD))
                intell = 10;
            switch (intell) {
            case 25:
            case 24:
            case 23:
            case 22:
            case 21:
            case 20:
            case 19:
            case 18:
            case 17:
                hungr = 0;
                break;
            case 16:
                hungr /= 4;
                break;
            case 15:
                hungr /= 2;
                break;
            }
            */
            /* don't put player (quite) into fainting from
             * casting a spell, particularly since they might
             * not even be hungry at the beginning; however,
             * this is low enough that they must eat before
             * casting anything else except detect food
             */
            //if (hungr > u.uhunger - 3)
            //    hungr = u.uhunger - 3;
            //morehungry(hungr);
        //}
    //}

    //One spell preparation is used, successful casting or not
    if(spellamount(spell) > 0)
        spellamount(spell)--;

    //Also, it goes under cooldown, successful or not
    int splcd = getspellcooldown(spell);
    if(splcd > 0)
        spellcooldownleft(spell) = splcd;
    else
        spellcooldownleft(spell) = 0;

    //Now check if successful
    chance = percent_success(spell, TRUE);
    if (confused || stunned || (rnd(100) > chance)) 
    {
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        u_wait_until_action();
        play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "fail to cast the spell correctly.");
        deduct_mana_cost(denergy / 2);
        context.botl = 1;
        update_u_action_revert(ACTION_TILE_NO_ACTION);
        return 1;
    }

    deduct_mana_cost(denergy);
    context.botl = 1;
    exercise(A_WIS, TRUE);
    /* pseudo is a temporary "false" object containing the spell stats */
    pseudo = mksobj(spellid(spell), FALSE, FALSE, FALSE);
    pseudo->blessed = pseudo->cursed = 0;
    pseudo->quan = 20L; /* do not let useup get it */
    /*
     * Find the skill the hero has in a spell type category.
     * See spell_skilltype for categories.
     */
    otyp = pseudo->otyp;
    skill = spell_skilltype(otyp);
    role_skill = P_SKILL_LEVEL(skill);

    if (!u.uachieve.role_achievement && (
        (Role_if(PM_WIZARD) && spellev(spell) >= 10)
        || (Role_if(PM_PRIEST) && spellev(spell) >= 10)
        || (Role_if(PM_HEALER) && (skill == P_HEALING_SPELL || skill == P_ABJURATION_SPELL) && spellev(spell) >= 9)))
    {
        u.uachieve.role_achievement = 1;
        char abuf[BUFSZ];
        const char* ra_desc = get_role_achievement_description(TRUE);
        strcpy_capitalized_for_title(abuf, ra_desc);
        achievement_gained(abuf);
        livelog_printf(LL_ACHIEVE, "%s", ra_desc);
    }

    switch (otyp) {
    /*
     * At first spells act as expected.  As the hero increases in skill
     * with the appropriate spell type, some spells increase in their
     * effects, e.g. more damage, further distance, and so on, without
     * additional cost to the spellcaster.
     */
    /* these spells are all duplicates of wand effects */
    case SPE_MAGIC_ARROW:
    case SPE_ARROW_OF_DIANA:
    case SPE_FORCE_BOLT:
    case SPE_FORCE_STRIKE:
    case SPE_FIREBALL:
    case SPE_FIRE_STORM:
    case SPE_ICE_STORM:
    case SPE_THUNDERSTORM:
    case SPE_MAGICAL_IMPLOSION:
    case SPE_MAGIC_STORM:
    case SPE_CELESTIAL_STORM:
    case SPE_WRATH_OF_GOD:
    case SPE_FIRE_BOLT:
    case SPE_LIGHTNING_BOLT:
    case SPE_CONE_OF_COLD:
    case SPE_FLAME_BURST:
    case SPE_THUNDERSTRIKE:
    case SPE_GREATER_FROST_RAY:
    case SPE_DISINTEGRATE:
    case SPE_SLEEP:
    case SPE_MAGIC_MISSILE:
    case SPE_GREATER_MAGIC_MISSILE:
    case SPE_KNOCK:
    case SPE_SILENCE:
    case SPE_SLOW_MONSTER:
    case SPE_MASS_SLOW:
    case SPE_HASTE_MONSTER:
    case SPE_HOLD_MONSTER:
    case SPE_HOLD_UNDEAD:
    case SPE_MASS_HOLD:
    case SPE_WIZARD_LOCK:
    case SPE_DIG:
    case SPE_LOWER_WATER:
    case SPE_FEAR:
    case SPE_TURN_UNDEAD:
    case SPE_NEGATE_UNDEATH:
    case SPE_BANISH_DEMON:
    case SPE_POLYMORPH:
    case SPE_TELEPORT_MONSTER:
    case SPE_CANCELLATION:
    case SPE_DISJUNCTION:
    case SPE_LOWER_MAGIC_RESISTANCE:
    case SPE_DIMINISH_MAGIC_RESISTANCE:
    case SPE_ABOLISH_MAGIC_RESISTANCE:
    case SPE_NEGATE_MAGIC_RESISTANCE:
    case SPE_FORBID_SUMMONING:
    case SPE_FINGER_OF_DEATH:
    case SPE_TOUCH_OF_DEATH:
    case SPE_TOUCH_OF_PETRIFICATION:
    case SPE_OBLITERATE:
    case SPE_FLESH_TO_STONE:
    case SPE_GAZE_OF_PETRIFICATION:
    case SPE_SHOCKING_TOUCH:
    case SPE_BURNING_HANDS:
    case SPE_FREEZING_TOUCH:
    case SPE_INCINERATE:
    case SPE_ELECTROCUTE:
    case SPE_GLACIAL_GRASP:
    case SPE_HEAVENLY_TOUCH:
    case SPE_TOUCH_OF_DIVINITY:
    case SPE_CHARM_MONSTER:
    case SPE_CONTROL_UNDEAD:
    case SPE_DOMINATE_MONSTER:
    case SPE_POWER_WORD_KILL:
    case SPE_POWER_WORD_STUN:
    case SPE_POWER_WORD_BLIND:
    case SPE_SUNLIGHT_BEAM:
    case SPE_RESURRECTION:
    case SPE_RAISE_MINOR_ZOMBIE:
    case SPE_CREATE_MINOR_MUMMY:
    case SPE_RAISE_GIANT_ZOMBIE:
    case SPE_CREATE_GIANT_MUMMY:
    case SPE_CREATE_DRACOLICH:
    case SPE_CREATE_ELDER_DRACOLICH:
    case SPE_RAISE_SKELETON:
    case SPE_RAISE_SKELETON_WARRIOR:
    case SPE_RAISE_SKELETON_LORD:
    case SPE_RAISE_SKELETON_KING:
    case SPE_RAISE_GIANT_SKELETON:
    case SPE_DETECT_UNSEEN:
    case SPE_LIGHT:
    case SPE_BLACK_BLADE_OF_DISASTER:
    case SPE_MAGE_ARMOR:
    case SPE_CREATE_FOOD:
    case SPE_CREATE_FRUITS:
    case SPE_CREATE_WATER:
    case SPE_ARMAGEDDON:
    case SPE_WISH:
    case SPE_TIME_STOP:
    case SPE_ANIMATE_AIR:
    case SPE_ANIMATE_EARTH:
    case SPE_ANIMATE_FIRE:
    case SPE_ANIMATE_WATER:
    case SPE_GREATER_ANIMATE_AIR:
    case SPE_GREATER_ANIMATE_EARTH:
    case SPE_GREATER_ANIMATE_FIRE:
    case SPE_GREATER_ANIMATE_WATER:
    case SPE_SUMMON_DRAGON:
    case SPE_SUMMON_ANCIENT_DRAGON:
    case SPE_CREATE_GOLD_GOLEM:
    case SPE_CREATE_GLASS_GOLEM:
    case SPE_CREATE_GEMSTONE_GOLEM:
    case SPE_CREATE_SILVER_GOLEM:
    case SPE_CREATE_CLAY_GOLEM:
    case SPE_CREATE_STONE_GOLEM:
    case SPE_CREATE_IRON_GOLEM:
    case SPE_CREATE_WOOD_GOLEM:
    case SPE_CREATE_PAPER_GOLEM:
    case SPE_SUMMON_DEMON:
    case SPE_FAITHFUL_HOUND:
    case SPE_CALL_DEMOGORGON:
    case SPE_CELESTIAL_DOVE:
    case SPE_CELESTIAL_EAGLE:
    case SPE_SUMMON_PHOENIX:
    case SPE_SUMMON_GOLD_DRAGON:
    case SPE_SUMMON_ANCIENT_GOLD_DRAGON:
    case SPE_CALL_BAHAMUT:
    case SPE_SUMMON_TREANT:
    case SPE_SUMMON_ELDER_TREANT:
    case SPE_SUMMON_GIANT_ANT:
    case SPE_SUMMON_SOLDIER_ANT:
    case SPE_SUMMON_DIREWOLF:
    case SPE_SUMMON_BISON:
    case SPE_SUMMON_PEGASUS:
    case SPE_SUMMON_ROC:
    case SPE_SUMMON_GRIZZLY_BEAR:
    case SPE_SUMMON_OWLBEAR:
    case SPE_SUMMON_OWLBEAR_PATRIARCH:
    case SPE_SUMMON_COCKATRICE:
    case SPE_SUMMON_GIANT_COCKATRICE:
    case SPE_SUMMON_GARGANTUAN_COCKATRICE:
    case SPE_SUMMON_GIANT_SPIDER:
    case SPE_SUMMON_PHASE_SPIDER:
    case SPE_SUMMON_PURPLE_WORM:
    case SPE_SUMMON_GARGANTUAN_BEETLE:
    case SPE_SUMMON_RAVEN:
    case SPE_SUMMON_WINTER_WOLF:
    case SPE_HEAVENLY_SERPENT:
    case SPE_GUARDIAN_ANGEL:
    case SPE_SUMMON_ARCHON:
    case SPE_DIVINE_MOUNT:
    case SPE_HEAVENLY_ARMY:
    case SPE_STICK_TO_SNAKE:
    case SPE_STICK_TO_COBRA:
    case SPE_STICK_TO_PYTHON:
    case SPE_STICK_TO_BOA:
    case SPE_STICK_TO_GIANT_ANACONDA:
    case SPE_CALL_HIERARCH_MODRON:
    case SPE_GREAT_YENDORIAN_SUMMONING:
    case SPE_CALL_GHOUL:
    case SPE_MASS_RAISE_ZOMBIE:
    case SPE_MASS_CREATE_MUMMY:
    case SPE_MASS_CREATE_DRACOLICH:
    case SPE_SPHERE_OF_ANNIHILATION:
    case SPE_CIRCLE_OF_SUNLIGHT:
    case SPE_CIRCLE_OF_RADIANCE:
    case SPE_CIRCLE_OF_FIRE:
    case SPE_CIRCLE_OF_FROST:
    case SPE_CIRCLE_OF_LIGHTNING:
    case SPE_CIRCLE_OF_MAGIC:
    case SPE_REPLENISH_UNDEATH:
    case SPE_GREATER_UNDEATH_REPLENISHMENT:
    case SPE_CURE_BLINDNESS:
    case SPE_CURE_SICKNESS:
    case SPE_CURE_PETRIFICATION:
    case SPE_MINOR_HEALING:
    case SPE_HEALING:
    case SPE_EXTRA_HEALING:
    case SPE_GREATER_HEALING:
    case SPE_PRODIGIOUS_HEALING:
    case SPE_FULL_HEALING:
    case SPE_DRAIN_LEVEL:
    case SPE_PROBE:
    case SPE_STONE_TO_FLESH:
    case SPE_COMMUNE:
    case SPE_PRAYER:
    case SPE_ABSOLUTION:
    case SPE_ENLIGHTENMENT:
    case SPE_TELEPORT_SELF:
    case SPE_CONTROLLED_TELEPORT:
    case SPE_CIRCLE_OF_TELEPORTATION:
    case SPE_LEVEL_TELEPORT:
    case SPE_CONTROLLED_LEVEL_TELEPORT:
    case SPE_PORTAL:
        if (objects[otyp].oc_dir != NODIR)
        {
            if (atme) 
            {
                u.dx = u.dy = u.dz = 0;
            } 
            else if (!getdir((char *) 0))
            {
                /* getdir cancelled, re-use previous direction */
                /*
                 * FIXME:  reusing previous direction only makes sense
                 * if there is an actual previous direction.  When there
                 * isn't one, the spell gets cast at self which is rarely
                 * what the player intended.  Unfortunately, the way
                 * spelleffects() is organized means that aborting with
                 * "nevermind" is not an option.
                 */
                play_sfx_sound(SFX_MAGICAL_ENERGY_RELEASED);
                pline_The_ex(ATR_NONE, CLR_MSG_SPELL, "magical energy is released!");
            }

            if (!u.dx && !u.dy && !u.dz)
            {
                //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
                update_u_action(ACTION_TILE_CAST_NODIR);
                play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
                u_wait_until_action();

                if ((damage = zapyourself(pseudo, TRUE)) != 0)
                {
                    char buf[BUFSZ];

                    Sprintf(buf, "zapped %sself with a spell", uhim());
                    losehp(damage, buf, NO_KILLER_PREFIX);
                }
            }
            else
            {
                update_u_facing(TRUE);
                //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
                update_u_action(ACTION_TILE_CAST_DIR);
                play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
                u_wait_until_action();
                weffects(pseudo);

            }
        } 
        else
        {
            /* No dir */
            //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
            update_u_action(ACTION_TILE_CAST_NODIR);
            play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
            u_wait_until_action();
            weffects(pseudo);
        }

        update_inventory(); /* spell may modify inventory */
        break;
    case SPE_METEOR_SWARM:
    {
        int shots = NUM_METEOR_SWARM_METEORS;
        coord cc = { u.ux, u.uy };
        pline("Where do you want to center the meteor swarm?");

        int trycnt = 0;
        while (trycnt < 10)
        {
            (void)getpos(&cc, TRUE, "the desired position", CURSOR_STYLE_SPELL_CURSOR);
            if (!get_valid_targeted_position(cc.x, cc.y, otyp))
            {
                play_sfx_sound(SFX_GENERAL_NOT_AT_RIGHT_LOCATION);
                pline_ex(ATR_NONE, CLR_MSG_FAIL, "Not a valid target position.");
                if (trycnt > 4)
                {
                    cc.x = u.ux;
                    cc.y = u.uy;
                    break;
                }
            }
            else
                break;

            trycnt++;
        }

        if (isok(cc.x, cc.y))
        {
            int expl_type = EXPL_METEOR_SWARM;
            for (n = 0; n < shots; n++)
            {
                You_ex(ATR_NONE, CLR_MSG_ATTENTION, "shoot a meteor!");
                explode(cc.x, cc.y, objects[otyp].oc_dir_subtype, &youmonst, objects[otyp].oc_spell_dmg_dice, objects[otyp].oc_spell_dmg_diesize, objects[otyp].oc_spell_dmg_plus, otyp, SPBOOK_CLASS, expl_type);
            }
        }
        else
        {
            play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
            pline_ex(ATR_NONE, CLR_MSG_FAIL, "The spell fails!");
        }
        break;
    }
    /* these are all duplicates of scroll effects */
    case SPE_REMOVE_CURSE:
    case SPE_CONFUSE_MONSTER:
    case SPE_DETECT_FOOD:
    case SPE_MASS_FEAR:
    case SPE_IDENTIFY:
    case SPE_DETECT_BLESSEDNESS:
    case SPE_BLESS:
    case SPE_CURSE:
    case SPE_ENCHANT_ARMOR:
    case SPE_ENCHANT_WEAPON:
    case SPE_PROTECT_ARMOR:
    case SPE_PROTECT_WEAPON:
    case SPE_SPHERE_OF_CHARMING:
    case SPE_SPHERE_OF_DOMINATION:
    case SPE_MASS_CHARM:
    case SPE_MASS_DOMINATION:
    case SPE_MASS_SLEEP:
    case SPE_HOLY_WORD:
    case SPE_MAGIC_MAPPING:
    case SPE_DETECT_TRAPS:
    case SPE_FLAME_STRIKE:
    case SPE_DEATHSPELL:
    case SPE_STINKING_CLOUD:
    case SPE_CREATE_MONSTER:
        if (objects[otyp].oc_dir != TARGETED)
        {
            //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
            update_u_action(ACTION_TILE_CAST_NODIR);
            play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
            u_wait_until_action();
        }
        (void) seffects(pseudo, &effect_happened, targetmonst);
        break;

    /* these are all duplicates of potion effects */
    case SPE_HASTE_SELF:
    case SPE_DETECT_TREASURE:
    case SPE_DETECT_MONSTERS:
    case SPE_IMPROVED_MONSTER_DETECTION:
    case SPE_LEVITATION:
    case SPE_RESTORE_ABILITY:
    case SPE_INVISIBILITY:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        (void) peffects(pseudo);
        break;
    /* end of potion-like spells */

    case SPE_CREATE_FAMILIAR:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        (void) make_familiar((struct obj *) 0, u.ux, u.uy, FALSE);
        break;
    case SPE_CONGREGATE:
    case SPE_SUMMONING_CALL:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        if (iflags.using_gui_sounds)
            delay_output_milliseconds(200);
        use_magic_whistle((struct obj*) 0);
        break;
    case SPE_CLAIRVOYANCE:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        play_special_effect_at(SPECIAL_EFFECT_GENERIC_SPELL, 0, u.ux, u.uy, FALSE);
        special_effect_wait_until_action(0);
        if (!Blocks_Clairvoyance) {
            if (role_skill >= P_SKILLED)
                pseudo->blessed = 1; /* detect monsters as well as map */
            do_vicinity_map(pseudo);
        /* at present, only one thing blocks clairvoyance */
        } else if (uarmh && uarmh->otyp == CORNUTHAUM)
            You_ex(ATR_NONE, CLR_MSG_ATTENTION, "sense a pointy hat on top of your %s.", body_part(HEAD));
        special_effect_wait_until_end(0);
        break;
    case SPE_MINOR_CONSULTATION:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        outrumor(&youmonst, pseudo, 1, BY_SPELL);
        break;
    case SPE_MAJOR_CONSULTATION:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        outoracle(&youmonst, pseudo, FALSE, 2);
        break;
    case SPE_PROTECTION:
    case SPE_SHIELD:
    case SPE_BARKSKIN:
    case SPE_STONESKIN:
    case SPE_REFLECTION:
    case SPE_ANTI_MAGIC_SHELL:
    case SPE_PROTECTION_FROM_FIRE:
    case SPE_PROTECTION_FROM_COLD:
    case SPE_PROTECTION_FROM_LIGHTNING:
    case SPE_PROTECTION_FROM_ACID:
    case SPE_PROTECTION_FROM_POISON:
    case SPE_PROTECTION_FROM_CHARM:
    case SPE_PROTECTION_FROM_LIFE_DRAINING:
    case SPE_PROTECTION_FROM_DEATH_MAGIC:
    case SPE_PROTECTION_FROM_DISINTEGRATION:
    case SPE_PROTECTION_FROM_SICKNESS:
    case SPE_PROTECTION_FROM_PETRIFICATION:
    case SPE_PROTECTION_FROM_LYCANTHROPY:
    case SPE_PROTECTION_FROM_CURSES:
    case SPE_TITAN_STRENGTH:
    case SPE_DIVINE_STRENGTH:
    case SPE_DIVINE_ENDURANCE:
    case SPE_WATER_BREATHING:
    case SPE_WATER_WALKING:
    case SPE_TELEPATHY:
    case SPE_MIRROR_IMAGE:
    case SPE_MASS_CONFLICT:
    case SPE_GLOBE_OF_INVULNERABILITY:
    case SPE_DIVINE_INTERVENTION:
    {
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        play_special_effect_at(SPECIAL_EFFECT_GENERIC_SPELL, 0, u.ux, u.uy, FALSE);
        play_sfx_sound_at_location(SFX_GENERAL_GAIN_ABILITY_SPELL, u.ux, u.uy);
        special_effect_wait_until_action(0);
        switch (otyp)
        {
        case SPE_MASS_CONFLICT:
            play_sfx_sound(SFX_CONFLICT);
            break;
        case SPE_MIRROR_IMAGE:
            play_sfx_sound(SFX_MIRROR_IMAGE);
            break;
        default:
            break;
        }
        int multicolors[1] = { CLR_MSG_HINT };
        You_multi_ex(ATR_NONE, CLR_MSG_SPELL, no_multiattrs, multicolors, "successfully cast \'%s\'.", spellname(spell));
        addspellintrinsictimeout(otyp);
        special_effect_wait_until_end(0);
        break;
    }
    case SPE_JUMPING:
        //play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
        update_u_action(ACTION_TILE_CAST_NODIR);
        play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
        u_wait_until_action();
        if (!jump(max(role_skill, 1)))
            pline1(nothing_happens);
        break;
    case SPE_COLD_ENCHANT_ITEM:
    case SPE_FIRE_ENCHANT_ITEM:
    case SPE_LIGHTNING_ENCHANT_ITEM:
    case SPE_DEATH_ENCHANT_ITEM:
    {
        const char enchant_objects[] = { ALL_CLASSES, ALLOW_NONE, 0 };
        struct obj* otmp;
        char buf[BUFSZ] = "";
        strcpy(buf, otyp == SPE_COLD_ENCHANT_ITEM ? "cold-enchant" :
            otyp == SPE_FIRE_ENCHANT_ITEM ? "fire-enchant" :
            otyp == SPE_LIGHTNING_ENCHANT_ITEM ? "lightning-enchant" :
            otyp == SPE_DEATH_ENCHANT_ITEM ? "death-enchant" :
            "enchant");

        otmp = getobj(enchant_objects, buf, 0, "");
        if (!otmp || inaccessible_equipment(otmp, buf, FALSE, FALSE))
        {
            return 0;
        }

        if (otmp && otmp != &zeroobj) 
        {
            play_simple_monster_sound(&youmonst, MONSTER_SOUND_TYPE_CAST);
            update_u_action(ACTION_TILE_CAST_NODIR);
            play_sfx_sound_at_location(SFX_GENERIC_CAST_EFFECT, u.ux, u.uy);
            u_wait_until_action();

            switch (otyp)
            {
            case SPE_DEATH_ENCHANT_ITEM:
                if (is_death_enchantable(otmp))
                {
                    You_ex(ATR_NONE, CLR_MSG_POSITIVE, "enchant %s with death magic.", yname(otmp));
                    otmp = elemental_enchant_quan(otmp, DEATH_ENCHANTMENT_QUANTITY_NORMAL, DEATH_ENCHANTMENT);
                    prinv((char*)0, otmp, 0L);
                    //otmp->elemental_enchantment = DEATH_ENCHANTMENT;
                }
                else
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in black energy for a moment.", Tobjnam(otmp, "flicker"));
                }
                break;
            case SPE_COLD_ENCHANT_ITEM:
                if (otmp->elemental_enchantment == DEATH_ENCHANTMENT)
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in blue for a moment, but then glows black.", Tobjnam(otmp, "flicker"));
                    break;
                }
                if (otmp->elemental_enchantment == FIRE_ENCHANTMENT)
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "The cold energies dispel the flaming enchantment on %s.", yname(otmp));
                    otmp->elemental_enchantment = 0;
                    update_inventory();
                    break;
                }

                if (is_elemental_enchantable(otmp))
                {
                    You_ex(ATR_NONE, CLR_MSG_SUCCESS, "enchant %s with freezing magic.", yname(otmp));
                    otmp = elemental_enchant_quan(otmp, ELEMENTAL_ENCHANTMENT_QUANTITY_NORMAL, COLD_ENCHANTMENT);
                    prinv((char*)0, otmp, 0L);
                    //otmp->elemental_enchantment = COLD_ENCHANTMENT;
                }
                else
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in blue for a moment.", Tobjnam(otmp, "flicker"));
                }
                break;
            case SPE_FIRE_ENCHANT_ITEM:
                if (otmp->elemental_enchantment == DEATH_ENCHANTMENT)
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in red for a moment, but then glows black.", Tobjnam(otmp, "flicker"));
                    break;
                }
                if (otmp->elemental_enchantment == COLD_ENCHANTMENT)
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "The fiery energies dispel the freezing enchantment on %s.", yname(otmp));
                    otmp->elemental_enchantment = 0;
                    update_inventory();
                    break;
                }

                if (is_elemental_enchantable(otmp))
                {
                    You_ex(ATR_NONE, CLR_MSG_SUCCESS, "enchant %s with fire magic.", yname(otmp));
                    otmp = elemental_enchant_quan(otmp, ELEMENTAL_ENCHANTMENT_QUANTITY_NORMAL, FIRE_ENCHANTMENT);
                    prinv((char*)0, otmp, 0L);
                    //otmp->elemental_enchantment = FIRE_ENCHANTMENT;
                }
                else
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in red for a moment.", Tobjnam(otmp, "flicker"));
                }
                break;
            case SPE_LIGHTNING_ENCHANT_ITEM:
                if (otmp->elemental_enchantment == DEATH_ENCHANTMENT)
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in blue for a moment, but then glows black.", Tobjnam(otmp, "flicker"));
                    break;
                }
                if (is_elemental_enchantable(otmp))
                {
                    You_ex(ATR_NONE, CLR_MSG_SUCCESS, "enchant %s with lightning magic.", yname(otmp));
                    otmp = elemental_enchant_quan(otmp, ELEMENTAL_ENCHANTMENT_QUANTITY_NORMAL, LIGHTNING_ENCHANTMENT);
                    prinv((char*)0, otmp, 0L);
                    //otmp->elemental_enchantment = LIGHTNING_ENCHANTMENT;
                }
                else
                {
                    pline_ex(ATR_NONE, CLR_MSG_SPELL, "%s in blue for a moment.", Tobjnam(otmp, "flicker"));
                }
                break;
            }
        }
        break;
    }
    default:
        impossible("Unknown spell %d attempted.", spell);
        obfree(pseudo, (struct obj *) 0);
        return 0;
    }


    /* gain skill for successful cast */
    int pointsmultiplier = max(0, spellskillchance(spell) / 100);
    int randomizedchance = spellskillchance(spell) - pointsmultiplier * 100;

    if (rn2(100) < randomizedchance)
        pointsmultiplier++;

    if (pointsmultiplier > 0)
        use_skill(skill, (spellev(spell) + 2) * pointsmultiplier);

    int result = effect_happened;
    if (pseudo->otyp > STRANGE_OBJECT && objects[pseudo->otyp].oc_spell_flags & S1_DOES_NOT_TAKE_A_TURN)
        result = 0;

    obfree(pseudo, (struct obj *) 0); /* now, get rid of it */
    u_wait_until_end();
    update_u_action_revert(ACTION_TILE_NO_ACTION);

    return result;
}

void
addspellintrinsictimeout(otyp)
int otyp;
{
    if (otyp <= 0)
        return;

    if (objects[otyp].oc_dir_subtype <= 0 || objects[otyp].oc_dir_subtype > LAST_PROP)
        return;

    boolean hadbefore = u.uprops[objects[otyp].oc_dir_subtype].intrinsic || u.uprops[objects[otyp].oc_dir_subtype].extrinsic;
    long duration = d(objects[otyp].oc_spell_dur_dice, objects[otyp].oc_spell_dur_diesize) + objects[otyp].oc_spell_dur_plus;
    long oldtimeout = u.uprops[objects[otyp].oc_dir_subtype].intrinsic & TIMEOUT;
    long oldprop = u.uprops[objects[otyp].oc_dir_subtype].intrinsic & ~TIMEOUT;

    if (oldtimeout > duration || duration <= 0)
        return;

    u.uprops[objects[otyp].oc_dir_subtype].intrinsic = oldprop | duration;
    if (!hadbefore)
    {
        switch(objects[otyp].oc_dir_subtype)
        {
        case REFLECTING:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels more reflecting than before.");
            break;
        case FIRE_IMMUNITY:
        case IMPROVED_FIRE_RESISTANCE:
        case FIRE_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels less prone to burning than before.");
            break;
        case COLD_IMMUNITY:
        case IMPROVED_COLD_RESISTANCE:
        case COLD_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels less prone to frostbites than before.");
            break;
        case SHOCK_IMMUNITY:
        case IMPROVED_SHOCK_RESISTANCE:
        case SHOCK_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels less prone to electricity than before.");
            break;
        case DISINTEGRATION_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "body feels firmer than before.");
            break;
        case POISON_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more healthy than before.");
            break;
        case ACID_RESISTANCE:
        case IMPROVED_ACID_RESISTANCE:
        case ACID_IMMUNITY:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels less prone to acid than before.");
            break;
        case STONE_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels more limber than before.");
            break;
        case DRAIN_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "less suspectible to draining than before.");
            break;
        case SICK_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "unbothered by disease agents.");
            break;
        case INVULNERABLE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels less prone to damage than before.");
            break;
        case ANTIMAGIC:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more protected from magic.");
            break;
        case MAGIC_MISSILE_IMMUNITY:
        case IMPROVED_MAGIC_MISSILE_RESISTANCE:
        case MAGIC_MISSILE_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more protected from magic missiles.");
            break;
        case DEATH_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "soul's silver cord feels thicker than before.");
            break;
        case CHARM_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more firm about your own motivations.");
            break;
        case FEAR_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more courageous.");
            break;
        case MIND_SHIELDING:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "shielded from mental detection.");
            break;
        case LYCANTHROPY_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more protected from lycanthropy.");
            break;
        case CURSE_RESISTANCE:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "more protected from curses.");
            break;
        case LIFESAVED:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "less mortal than before.");
            break;
        case DETECT_MONSTERS:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "sensitive to the presence of monsters.");
            break;
        case BLIND_TELEPATHY:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "telepathic when blind.");
            break;
        case TELEPAT:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "telepathic.");
            break;
        case XRAY_VISION:
            You_ex(ATR_NONE, CLR_MSG_POSITIVE, "can see through walls.");
            break;
        case WATER_WALKING:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "like you could walk on water.");
            break;
        case MAGICAL_BREATHING:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "like you breathe in water.");
            break;
        case DISPLACED:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "image is duplicated by a displaced double.");
            break;
        case CONFLICT:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "neighborhood feels more quarrelsome than before.");
            break;
        case MAGICAL_PROTECTION:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "protected!");
            break;
        case MAGICAL_SHIELDING:
            You_feel_ex(ATR_NONE, CLR_MSG_POSITIVE, "shielded!");
            break;
        case MAGICAL_BARKSKIN:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin hardens into bark!");
            break;
        case MAGICAL_STONESKIN:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin hardens into stone!");
            break;
        case BISECTION_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels strong as steel!");
            break;
        case SLIME_RESISTANCE:
            Your_ex(ATR_NONE, CLR_MSG_POSITIVE, "skin feels fiery!");
            break;
        default:
            break;
        }
    }

    
    return;
}

int
subdirtype2explosiontype(subdir)
int subdir;
{
    int expltype = 0;
    switch (subdir)
    {
    case RAY_COLD:
    case RAY_WND_COLD:
        expltype = EXPL_FROSTY;
        break;
    case RAY_FIRE:
    case RAY_WND_FIRE:
        expltype = EXPL_FIERY;
        break;
    case RAY_POISON_GAS:
    case RAY_WND_POISON_GAS:
    case RAY_ACID:
    case RAY_WND_ACID:
        expltype = EXPL_NOXIOUS;
        break;
    case RAY_SLEEP:
    case RAY_WND_SLEEP:
    case RAY_DEATH:
    case RAY_WND_DEATH:
    case RAY_DISINTEGRATION:
    case RAY_WND_DISINTEGRATION:
    default:
        expltype = EXPL_MAGICAL;
        break;
    }

    return expltype;
}

void
deduct_mana_cost(double dmana)
{

    int integer_mana = (int)dmana;
    double df_mana = dmana - (double)integer_mana;
    int fractional_mana = (int)(10000 * df_mana);

    u.uen -= integer_mana;
    u.uen_fraction -= fractional_mana;
    if (u.uen_fraction < 0)
    {
        int multiple = (abs(u.uen_fraction) / 10000) + 1;
        u.uen -= multiple;
        u.uen_fraction += multiple * 10000;
    }
}

#if 0
/*ARGSUSED*/
STATIC_OVL boolean
spell_aim_step(arg, x, y)
genericptr_t arg UNUSED;
int x, y;
{
    if (!isok(x,y))
        return FALSE;
    if (!ZAP_POS(levl[x][y].typ)
        && !(IS_DOOR(levl[x][y].typ) && (levl[x][y].doormask & D_ISOPEN)))
        return FALSE;
    return TRUE;
}

/* Choose location where spell takes effect. */
STATIC_OVL int
throwspell(otyp)
int otyp;
{
    coord cc, uc;
    struct monst *mtmp;
    int range = 10;
    if (otyp >= 0 && objects[otyp].oc_spell_range > 0)
        range = objects[otyp].oc_spell_range;

    if (u.uinwater) {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        pline("You're joking!  In this weather?");
        return 0;
    } else if (Is_waterlevel(&u.uz)) {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You("had better wait for the sun to come out.");
        return 0;
    }

    pline("Where do you want to cast the spell?");
    cc.x = u.ux;
    cc.y = u.uy;
    if (getpos(&cc, TRUE, "the desired position", CURSOR_STYLE_SPELL_CURSOR) < 0)
        return 0; /* user pressed ESC */
    /* The number of moves from hero to where the spell drops.*/
    if (distmin(u.ux, u.uy, cc.x, cc.y) > range) {
        play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
        pline_The("spell dissipates over the distance!");
        return 0;
    } else if (u.uswallow) {
        play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
        pline_The("spell is cut short!");
        exercise(A_WIS, FALSE); /* What were you THINKING! */
        u.dx = 0;
        u.dy = 0;
        return 1;
    } else if ((!cansee(cc.x, cc.y)
                && (!(mtmp = m_at(cc.x, cc.y)) || !canspotmon(mtmp)))
               || IS_STWALL(levl[cc.x][cc.y].typ)) {
        play_sfx_sound(SFX_FAIL_TO_CAST_CORRECTLY);
        Your("mind fails to lock onto that location!");
        return 0;
    }

    uc.x = u.ux;
    uc.y = u.uy;

    walk_path(&uc, &cc, spell_aim_step, (genericptr_t) 0);

    u.dx = cc.x;
    u.dy = cc.y;
    return 1;
}
#endif

STATIC_VAR struct tport_hideaway {
    struct spell savespell;
    int tport_indx;
} save_tport;

/* add/hide/remove/unhide teleport-away on behalf of dotelecmd() to give
   more control to behavior of ^T when used in wizard mode */
int
tport_spell(what)
int what;
{
    int i;
/* also defined in teleport.c */
#define NOOP_SPELL  0
#define HIDE_SPELL  1
#define ADD_SPELL   2
#define UNHIDESPELL 3
#define REMOVESPELL 4

    for (i = 0; i < MAXSPELL; i++)
        if (spellid(i) == SPE_TELEPORT_MONSTER || spellid(i) == NO_SPELL)
            break;
    if (i == MAXSPELL) {
        impossible("tport_spell: spellbook full");
        /* wizard mode ^T is not able to honor player's menu choice */
    } else if (spellid(i) == NO_SPELL) {
        if (what == HIDE_SPELL || what == REMOVESPELL) {
            save_tport.tport_indx = MAXSPELL;
        } else if (what == UNHIDESPELL) {
            /*assert( save_tport.savespell.sp_id == SPE_TELEPORT_MONSTER );*/
            spl_book[save_tport.tport_indx] = save_tport.savespell;
            save_tport.tport_indx = MAXSPELL; /* burn bridge... */
        } else if (what == ADD_SPELL) {
            save_tport.savespell = spl_book[i];
            save_tport.tport_indx = i;
            spl_book[i].sp_id = SPE_TELEPORT_MONSTER;
            spl_book[i].sp_lev = (xchar)objects[SPE_TELEPORT_MONSTER].oc_spell_level;
            spl_book[i].sp_matcomp = objects[SPE_TELEPORT_MONSTER].oc_material_components;
            spl_book[i].sp_cooldownlength = (int)objects[SPE_TELEPORT_MONSTER].oc_spell_cooldown;
            spl_book[i].sp_cooldownleft = 0;
            spl_book[i].sp_skillchance = (int)objects[SPE_TELEPORT_MONSTER].oc_spell_skill_chance;
            spl_book[i].sp_amount = -1; //Infinite??
            spl_book[i].sp_know = SPELL_IS_KEEN;
            sortspells();
            return REMOVESPELL; /* operation needed to reverse */
        }
    } else { /* spellid(i) == SPE_TELEPORT_MONSTER */
        if (what == ADD_SPELL || what == UNHIDESPELL) {
            save_tport.tport_indx = MAXSPELL;
        } else if (what == REMOVESPELL) {
            /*assert( i == save_tport.tport_indx );*/
            spl_book[i] = save_tport.savespell;
            save_tport.tport_indx = MAXSPELL;
        } else if (what == HIDE_SPELL) {
            save_tport.savespell = spl_book[i];
            save_tport.tport_indx = i;
            spl_book[i].sp_id = NO_SPELL;
            sortspells();
            return UNHIDESPELL; /* operation needed to reverse */
        }
    }
    return NOOP_SPELL;
}

/* forget a random selection of known spells due to amnesia;
   they used to be lost entirely, as if never learned, but now we
   just set the memory retention to zero so that they can't be cast */
void
losespells()
{
    int n, nzap, i;

    /* in case reading has been interrupted earlier, discard context */
    context.spbook.book = 0;
    context.spbook.o_id = 0;
    /* count the number of known spells */
    for (n = 0; n < MAXSPELL; ++n)
        if (spellid(n) == NO_SPELL)
            break;

    /* lose anywhere from zero to all known spells;
       if confused, use the worse of two die rolls */
    nzap = rn2(n + 1);
    if (Confusion) {
        i = rn2(n + 1);
        if (i > nzap)
            nzap = i;
    }
    /* good Luck might ameliorate spell loss */
    if (nzap > 1 && !rnl(7))
        nzap = rnd(nzap);

    /*
     * Forget 'nzap' out of 'n' known spells by setting their memory
     * retention to zero.  Every spell has the same probability to be
     * forgotten, even if its retention is already zero.
     *
     * Perhaps we should forget the corresponding book too?
     *
     * (3.4.3 removed spells entirely from the list, but always did
     * so from its end, so the 'nzap' most recently learned spells
     * were the ones lost by default.  Player had sort control over
     * the list, so could move the most useful spells to front and
     * only lose them if 'nzap' turned out to be a large value.
     *
     * Discarding from the end of the list had the virtue of making
     * casting letters for lost spells become invalid and retaining
     * the original letter for the ones which weren't lost, so there
     * was no risk to the player of accidentally casting the wrong
     * spell when using a letter that was in use prior to amnesia.
     * That wouldn't be the case if we implemented spell loss spread
     * throughout the list of known spells; every spell located past
     * the first lost spell would end up with new letter assigned.)
     */
    for (i = 0; nzap > 0; ++i) {
        /* when nzap is small relative to the number of spells left,
           the chance to lose spell [i] is small; as the number of
           remaining candidates shrinks, the chance per candidate
           gets bigger; overall, exactly nzap entries are affected */
        if (rn2(n - i) < nzap) {
            /* lose access to spell [i] */
            spellknow(i) = 0;
#if 0
            /* also forget its book */
            forget_single_object(spellid(i));
#endif
            /* and abuse wisdom */
            exercise(A_WIS, FALSE);
            /* there's now one less spell slated to be forgotten */
            --nzap;
        }
    }
}

/*
 * Allow player to sort the list of known spells.  Manually swapping
 * pairs of them becomes very tedious once the list reaches two pages.
 *
 * Possible extensions:
 *      provide means for player to control ordering of skill classes;
 *      provide means to supply value N such that first N entries stick
 *      while rest of list is being sorted;
 *      make chosen sort order be persistent such that when new spells
 *      are learned, they get inserted into sorted order rather than be
 *      appended to the end of the list?
 */

/* qsort callback routine */
STATIC_PTR int CFDECLSPEC
spell_cmp(vptr1, vptr2)
const genericptr vptr1;
const genericptr vptr2;
{
    /*
     * gather up all of the possible parameters except spell name
     * in advance, even though some might not be needed:
     *  indx. = spl_orderindx[] index into spl_book[];
     *  otyp. = spl_book[] index into objects[];
     *  levl. = spell level;
     *  skil. = skill group aka spell class.
     */
    short indx1 = *(short*)vptr1, indx2 = *(short*)vptr2;
    int otyp1 = spl_book[indx1].sp_id, otyp2 = spl_book[indx2].sp_id,
        levl1 = (int)objects[otyp1].oc_spell_level, levl2 = (int)objects[otyp2].oc_spell_level,
        skil1 = objects[otyp1].oc_skill, skil2 = objects[otyp2].oc_skill;

    switch (flags.spellorder) {
    case SORTBY_LETTER:
        return indx1 - indx2;
    case SORTBY_ALPHA:
        break;
    case SORTBY_LVL_LO:
        if (levl1 != levl2)
            return levl1 - levl2;
        break;
    case SORTBY_LVL_HI:
        if (levl1 != levl2)
            return levl2 - levl1;
        break;
    case SORTBY_SKL_AL:
        if (skil1 != skil2)
            return skil1 - skil2;
        break;
    case SORTBY_SKL_LO:
        if (skil1 != skil2)
            return skil1 - skil2;
        if (levl1 != levl2)
            return levl1 - levl2;
        break;
    case SORTBY_SKL_HI:
        if (skil1 != skil2)
            return skil1 - skil2;
        if (levl1 != levl2)
            return levl2 - levl1;
        break;
    default:
        return (vptr1 < vptr2) ? -1
                               : (vptr1 > vptr2); /* keep current order */
    }
    /* tie-breaker for most sorts--alphabetical by spell name */
    return strcmpi(OBJ_NAME(objects[otyp1]), OBJ_NAME(objects[otyp2]));
}

/* sort the index used for display order of the "view known spells"
   list (sortmode == SORTBY_xxx), or sort the spellbook itself to make
   the current display order stick (sortmode == SORTRETAINORDER) */
void
sortspells(VOID_ARGS)
{
    short i;
#if defined(SYSV) || defined(DGUX)
    unsigned n;
#else
    int n;
#endif

    if (flags.spellorder == SORTBY_CURRENT)
        return;

    if (flags.spellorder != SORTRETAINORDER)
    {
        /* Initialize sort order */
        for (i = 0; i < MAXSPELL; i++)
            spl_orderindx[i] = i;
    }

    if (flags.spellorder == SORTBY_NONE)
        return;

    /* Find the number of spells */
    for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; ++n)
        continue;

    if (n < 2)
        return; /* not enough entries to need sorting */

    if (flags.spellorder == SORTRETAINORDER) 
    {
        struct spell tmp_book[MAXSPELL];

        /* sort spl_book[] rather than spl_orderindx[];
           this also updates the index to reflect the new ordering (we
           could just free it since that ordering becomes the default) */
        for (i = 0; i < MAXSPELL; i++)
            tmp_book[i] = spl_book[spl_orderindx[i]];
        for (i = 0; i < MAXSPELL; i++)
            spl_book[i] = tmp_book[i], spl_orderindx[i] = i;
        flags.spellorder = SORTBY_NONE; /* reset */
        return;
    }

    /* usual case, sort the index rather than the spells themselves */
    qsort((genericptr_t)spl_orderindx, n, sizeof spl_orderindx[0], spell_cmp);
    return;
}

/* called if the [sort spells] entry in the view spells menu gets chosen */
STATIC_OVL boolean
spellsortmenu()
{
    winid tmpwin;
    menu_item *selected;
    anything any;
    char let;
    int i, n, choice;

    tmpwin = create_nhwindow(NHW_MENU);
    start_menu(tmpwin);
    any = zeroany; /* zero out all bits */

    for (i = 0; i < SIZE(spl_sortchoices); i++) {
        if (i == SORTRETAINORDER) {
            let = 'z'; /* assumes fewer than 26 sort choices... */
            /* separate final choice from others with a blank line */
            any.a_int = 0;
            add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "",
                     MENU_UNSELECTED);
        } else {
            let = 'a' + i;
        }
        any.a_int = i + 1;
        add_menu(tmpwin, NO_GLYPH, &any, let, 0, ATR_NONE, NO_COLOR, spl_sortchoices[i],
                 (i == flags.spellorder) ? MENU_SELECTED : MENU_UNSELECTED);
    }
    end_menu(tmpwin, "View known spells list sorted");

    n = select_menu(tmpwin, PICK_ONE, &selected);
    destroy_nhwindow(tmpwin);
    if (n > 0) {
        choice = selected[0].item.a_int - 1;
        /* skip preselected entry if we have more than one item chosen */
        if (n > 1 && choice == flags.spellorder)
            choice = selected[1].item.a_int - 1;
        free((genericptr_t) selected);
        flags.spellorder = choice;
        return TRUE;
    }
    return FALSE;
}

int
dosortspell()
{
    if (spellsortmenu())
        sortspells();

    return 0;
}

int
dovspell()
{
    char qbuf[QBUFSZ];
    int splnum, othnum;
    struct spell spl_tmp;

    if (spellid(0) == NO_SPELL)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "don't know any spells right now.");
    } 
    else 
    {
        while (dospellmenu("Choose a spell to reorder", SPELLMENU_REORDER, &splnum))
        {
            if (splnum == SPELLMENU_SORT) 
            {
                if (spellsortmenu())
                    sortspells();
            } 
            else 
            {
                Sprintf(qbuf, "Reordering spells; swap '%s' with", spellname(splnum));
                if (!dospellmenu(qbuf, splnum, &othnum))
                    break;

                spl_tmp = spl_book[splnum];
                spl_book[splnum] = spl_book[othnum];
                spl_book[othnum] = spl_tmp;

                sortspells();
            }
        }
    }
    return 0;
}

/* the '+' command -- view known spells */
STATIC_OVL boolean
dotradspellmenu(prompt, splaction, spell_no)
const char *prompt;
int splaction; /* SPELLMENU_CAST, SPELLMENU_REORDER, or spl_book[] index */
int *spell_no;
{
    winid tmpwin;
    int i, n, how, splnum;
    char buf[BUFSZ], descbuf[BUFSZ], fmt[BUFSZ];
    //char* colorbufs[MAXSPELL];
    //int colorbufcnt = 0;
    //const char *fmt;
    menu_item *selected;
    anything any;
    const char* nodesc = "(No short description)";

    //for (j = 0; j < MAXSPELL; j++)
    //{
    //    colorbufs[j] = (char*)malloc(BUFSZ * sizeof(char));
    //    strcpy(colorbufs[j], "");
    //}

    tmpwin = create_nhwindow(NHW_MENU);
    start_menu_ex(tmpwin, GHMENU_STYLE_SPELLS);
    any = zeroany; /* zero out all bits */

    int hotkeys[11] = { 0 };

    /* -1 means no hotkey; otherwise, values can be 0... MAXSPELL - 1, indicating the spell learnt */
    for (i = 0; i <= 10; i++)
        hotkeys[i] = -1;

    for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
    {
        if (spellhotkey(i) > 0 && spellhotkey(i) <= 10)
            hotkeys[spellhotkey(i)] = i;
    }

    /*
     * The correct spacing of the columns when not using
     * tab separation depends on the following:
     * (1) that the font is monospaced, and
     * (2) that selection letters are pre-pended to the
     * given string and are of the form "a - ".
     */
    if (splaction == SPELLMENU_DETAILS || splaction == SPELLMENU_REORDER || splaction == SPELLMENU_SORT || splaction >= 0)
    {
        int maxlen = 15;
        int maxnamelen = 0;

        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++)
        {
            int desclen = 0;
            splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
            if (OBJ_ITEM_DESC(spellid(splnum)))
                desclen = (int)strlen(OBJ_ITEM_DESC(spellid(splnum)));
            else
                desclen = (int)strlen(nodesc);
            if (desclen > maxlen)
                maxlen = desclen;

            int namelen = 0;
            namelen = (int)strlen(spellname(splnum));
            if (namelen > maxnamelen)
                maxnamelen = namelen;

        }

        int extraspaces = maxlen - 15;
        if (extraspaces > 42)
            extraspaces = 42;

        int extraleftforname_and_hotkey = 42 - extraspaces;
        int hotkey_chars = 2;
        int namelength = max(10, min(maxnamelen, 20 + extraleftforname_and_hotkey - hotkey_chars));


        char spacebuf[BUFSZ] = "";

        for (i = 0; i < extraspaces; i++)
            Strcat(spacebuf, " ");

        if (!iflags.menu_tab_sep) {
#if defined (GNH_MOBILE)
            Sprintf(fmt, "%%-%ds  #  Description    %%s", namelength);
#else
            Sprintf(fmt, "    %%-%ds  #  Description    %%s", namelength);
#endif
            Sprintf(buf, fmt, "Name", spacebuf);
        }
        else {
            Sprintf(buf, "Name\tH\tDescription");
        }

        add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR, buf,
            MENU_UNSELECTED, menu_heading_info());


        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++) {
            splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
            char shortenedname[BUFSZ] = "";
            char fullname[BUFSZ] = "";

            if (!iflags.menu_tab_sep) {
                Sprintf(fmt, "%%-%ds  %%c  %%s", namelength);
            }
            else {
                strcpy(fmt, "%s\t%c\t%s");
            }

            Sprintf(fullname, "%s", spellname(splnum));

            //Spell name
            if (strlen(fullname) > (size_t)(namelength))
                strncpy(shortenedname, fullname, (size_t)(namelength));
            else
                strcpy(shortenedname, fullname);


            //Shorten description, if needed
            char shorteneddesc[BUFSZ] = "";
            char fulldesc[BUFSZ];

            if(OBJ_ITEM_DESC(spellid(splnum)))
            {
                strcpy(fulldesc, OBJ_ITEM_DESC(spellid(splnum)));

                if (strlen(fulldesc) > 57)
                    strncpy(shorteneddesc, fulldesc, 57);
                else
                    strcpy(shorteneddesc, fulldesc);

                strcpy(descbuf, shorteneddesc);
            }
            else
                strcpy(descbuf, nodesc);

            char hotkeychar = ' ';
            if (spellhotkey(i) == 10)
                hotkeychar = '0';
            else if(spellhotkey(i) > 0)
                hotkeychar = spellhotkey(i) + '0';

            //Finally print
            if (spellknow(splnum) <= 0)
                Sprintf(buf, fmt, shortenedname, hotkeychar,  "(You cannot recall this spell)");
            else
                Sprintf(buf, fmt, shortenedname, hotkeychar, descbuf);

            any.a_int = splnum + 1; /* must be non-zero */
            add_menu(tmpwin, NO_GLYPH, &any, 0 /*spellet(splnum)*/, 0, ATR_INDENT_AT_DOUBLE_SPACE, NO_COLOR, buf,
                (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED);

        }
    }
    else if (splaction == SPELLMENU_PREPARE)
    {
        int maxlen = 23;
        int maxnamelen = 0;

        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++)
        {
            int desclen = 0;
            int namelen = 0;
            splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
            desclen = (int)strlen(matlists[spellmatcomp(splnum)].description_short);
            namelen = (int)strlen(spellname(splnum));
            if (desclen > maxlen)
                maxlen = desclen;
            if (namelen > maxnamelen)
                maxnamelen = namelen;
        }

        int extraspaces = maxlen - 23;
        if (extraspaces > 16)
            extraspaces = 16;

        int extraleftforname = 16 - extraspaces;
        int namelength = max(10, min(maxnamelen, 20 + extraleftforname));


        boolean hotkeyfound = FALSE;
        add_spell_prepare_menu_heading(tmpwin, namelength, extraspaces, FALSE);

        for (i = 1; i <= 10; i++)
        {
            if (hotkeys[i] >= 0)
            {
                add_spell_prepare_menu_item(tmpwin, hotkeys[i], splaction, namelength, extraspaces, TRUE);
                hotkeyfound = TRUE;
            }
        }
        if (hotkeyfound)
        {
            add_spell_prepare_menu_heading(tmpwin, namelength, extraspaces, TRUE);
        }

        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++)
        {
            add_spell_prepare_menu_item(tmpwin, i, splaction, namelength, extraspaces, FALSE);
        }
    }
    else
    {
        int maxnamelen = 0;

        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++)
        {
            int namelen = 0;
            splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
            namelen = (int)strlen(spellname(splnum));
            if (namelen > maxnamelen)
                maxnamelen = namelen;
        }

        int namelength = max(10, min(maxnamelen, 27));

        boolean hotkeyfound = FALSE;
        add_spell_cast_menu_heading(tmpwin, namelength, FALSE);
        for (i = 1; i <= 10; i++)
        {
            if (hotkeys[i] >= 0)
            {
                add_spell_cast_menu_item(tmpwin, hotkeys[i], splaction, namelength, TRUE);
                hotkeyfound = TRUE;
            }
        }
        if (hotkeyfound)
        {
            add_spell_cast_menu_heading(tmpwin, namelength, TRUE);
        }

        for (i = 0; i < MAXSPELL /*min(MAXSPELL, 52)*/ && spellid(i) != NO_SPELL; i++)
        {
            add_spell_cast_menu_item(tmpwin, i, splaction, namelength, FALSE);
        }
    }

    how = PICK_ONE;
    if (splaction == SPELLMENU_REORDER) {
        if (spellid(1) == NO_SPELL) {
            /* only one spell => nothing to swap with */
            how = PICK_NONE;
        } else {
            /* more than 1 spell, add an extra menu entry */
            any.a_int = SPELLMENU_SORT + 1;
            add_menu(tmpwin, NO_GLYPH, &any, '+', 0, ATR_NONE, NO_COLOR,
                     "[sort spells]", MENU_UNSELECTED);
        }
    }
    end_menu(tmpwin, prompt);

    //Show menu
    n = select_menu(tmpwin, how, &selected);
    destroy_nhwindow(tmpwin);
    
    //Remove menucolors
    //for (j = 0; j < colorbufcnt; j++)
    //{
    //    free_menu_coloring_str(colorbufs[j]);
    //}

    //for (j = 0; j < MAXSPELL; j++)
    //{
    //    free(colorbufs[j]);
    //}


    if (n > 0) {
        *spell_no = selected[0].item.a_int - 1;
        /* menu selection for `PICK_ONE' does not
           de-select any preselected entry */
        if (n > 1 && *spell_no == splaction)
            *spell_no = selected[1].item.a_int - 1;
        free((genericptr_t) selected);
        /* default selection of preselected spell means that
           user chose not to swap it with anything */
        if (*spell_no == splaction)
            return FALSE;
        return TRUE;
    } else if (splaction >= 0) {
        /* explicit de-selection of preselected spell means that
           user is still swapping but not for the current spell */
        *spell_no = splaction;
        return TRUE;
    }
    return FALSE;
} 


STATIC_OVL
void
add_spell_prepare_menu_heading(tmpwin, namelength, extraspaces, addemptyline)
winid tmpwin;
int namelength;
int extraspaces;
boolean addemptyline;
{
    char buf[BUFSZ], fmt[BUFSZ];
    anything any = zeroany;


    char spacebuf[BUFSZ] = "";

    int i;
    for (i = 0; i < extraspaces; i++)
        Strcat(spacebuf, " ");

    if (!iflags.menu_tab_sep)
    {
#if defined (GNH_MOBILE)
        Sprintf(fmt, "%%-%ds  Casts  Adds  Material components    %%s", namelength);
#else
        Sprintf(fmt, "    %%-%ds  Casts  Adds  Material components    %%s", namelength);
#endif
        Sprintf(buf, fmt, "Name", spacebuf);
    }
    else
    {
        Sprintf(buf, "Name\tCasts\tAdds\tMaterial components");
    }

    if (addemptyline)
    {
        any = zeroany;
        add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "", MENU_UNSELECTED);
    }

    any = zeroany;
    add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR, buf,
        MENU_UNSELECTED, menu_heading_info());

}

STATIC_OVL
void
add_spell_prepare_menu_item(tmpwin, i, splaction, namelength, extraspaces, usehotkey)
winid tmpwin;
int i;
int splaction;
int namelength;
int extraspaces;
boolean usehotkey;
{
    int splnum = !flags.spellorder || usehotkey ? i : (int)spl_orderindx[i];
    char buf[BUFSZ], availablebuf[BUFSZ], matcompbuf[BUFSZ], levelbuf[10], fmt[BUFSZ];
    anything any = zeroany;

    if (!iflags.menu_tab_sep) {
        if (spellknow(splnum) <= 0)
            Sprintf(fmt, "%%-%ds  %%s", namelength);
        else
        {
            char lengthbuf[BUFSZ] = "";
            Sprintf(lengthbuf, "%ds", 23 + extraspaces);
            Sprintf(fmt, "%%-%ds  %%5s  %%4s  %%-", namelength);
            Strcat(fmt, lengthbuf);
            //fmt = "%-20s  %s   %5s  %-35s";
            //        fmt = "%-20s  %2d   %-12s %4d %3d%% %9s";
        }
    }
    else
    {
        if (spellknow(splnum) <= 0)
            strcpy(fmt, "%s\t%s");
        else
            strcpy(fmt, "%s\t%s\t%s\t%s");
        //        fmt = "%s\t%-d\t%s\t%-d\t%-d%%\t%s";
    }

    //Shorten spell name if need be
    char shortenedname[BUFSZ] = "";
    char fullname[BUFSZ];
    char addsbuf[BUFSZ];

    strcpy(fullname, spellname(splnum));

    if (strlen(fullname) > (size_t)namelength)
        strncpy(shortenedname, fullname, namelength);
    else
        strcpy(shortenedname, fullname);

    //Print spell level
    if (spellev(splnum) < -1)
        strcpy(levelbuf, " *");
    else if (spellev(splnum) == -1)
        strcpy(levelbuf, " c");
    else if (spellev(splnum) == 0)
        strcpy(levelbuf, " C");
    else
        Sprintf(levelbuf, "%2d", spellev(splnum));

    //Print spell amount
    if (spellamount(splnum) >= 0)
        Sprintf(availablebuf, "%d", spellamount(splnum));
    else
        strcpy(availablebuf, "Inf.");

    //Print spells gained
    if (spellmatcomp(splnum) > 0)
        Sprintf(addsbuf, "%d", matlists[spellmatcomp(splnum)].spellsgained);
    else
        strcpy(addsbuf, "N/A");

    //Shorten matcomp description, if needed
    char shortenedmatcompdesc[BUFSZ];
    char fullmatcompdesc[BUFSZ];

    strcpy(shortenedmatcompdesc, "");
    strcpy(fullmatcompdesc, "");

    strcpy(fullmatcompdesc, matlists[spellmatcomp(splnum)].description_short);

    if (strlen(fullmatcompdesc) > 39)
    {
        strncpy(shortenedmatcompdesc, fullmatcompdesc, 39);
        shortenedmatcompdesc[39] = '\0';
    }
    else
        strcpy(shortenedmatcompdesc, fullmatcompdesc);

    if (spellmatcomp(splnum))
        strcpy(matcompbuf, shortenedmatcompdesc);
    else
        strcpy(matcompbuf, "(Not required)");

    //Finally print everything to buf
    if (spellknow(splnum) <= 0)
    {
        Sprintf(buf, fmt, shortenedname, "(You cannot recall this spell)");
    }
    else
    {
        Sprintf(buf, fmt, shortenedname,
            availablebuf, addsbuf, matcompbuf);
    }

    char letter = '\0';
    if (usehotkey)
    {
        if (spellhotkey(i) == 10)
            letter = '0';
        else
            letter = spellhotkey(i) + '0';
    }
    else
        letter = 0; // spellet(splnum);


    any.a_int = splnum + 1; /* must be non-zero */
    add_menu(tmpwin, NO_GLYPH, &any, letter, 0, ATR_NONE, NO_COLOR, buf,
        (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED);

}




STATIC_OVL
void
add_spell_cast_menu_heading(tmpwin, namelength, addemptyline)
winid tmpwin;
int namelength;
boolean addemptyline;
{
    char buf[BUFSZ], fmt[BUFSZ];
    anything any = zeroany;

    if (!iflags.menu_tab_sep)
    {
#if defined(GNH_MOBILE)
        Sprintf(fmt, "%%-%ds Level %%-13s Mana Stat Fail Cool Casts", namelength);
#else
        Sprintf(fmt, "    %%-%ds Level %%-13s Mana Stat Fail Cool Casts", namelength);
#endif
        Sprintf(buf, fmt, "Name", "Category");
    }
    else {
        Sprintf(buf, "Name\tLevel\tCategory\tMana\tStat\tFail\tCool\tCasts");
    }

    if (addemptyline)
    {
        any = zeroany;
        add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "", MENU_UNSELECTED);
    }

    any = zeroany;
    add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR, buf,
        MENU_UNSELECTED, menu_heading_info());

}

STATIC_OVL
void
add_alt_spell_cast_menu_item(tmpwin, i, splaction)
winid tmpwin;
int i;
int splaction;
{
    int splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
    char buf[BUFSIZ], availablebuf[BUFSZ], descbuf[BUFSZ], levelbuf[BUFSZ] = "";
    char fullname[BUFSZ] = "";
    anything any = zeroany;
    strcpy(fullname, spellname(splnum));
    *fullname = highc(*fullname);

    int glyph = spell_to_glyph(splnum);
    print_spell_level_symbol(levelbuf, spellid(splnum));
    if (OBJ_ITEM_DESC(spellid(splnum)))
    {
        Sprintf(descbuf, " (%s)", OBJ_ITEM_DESC(spellid(splnum)));
    }
    else
        strcpy(descbuf, "");

    if (spellamount(splnum) >= 0)
        Sprintf(availablebuf, "%d", spellamount(splnum));
    else
        strcpy(availablebuf, "Inf.");

    double spellmanacost = get_spell_mana_cost(splnum);
    double displayed_manacost = ceil(10 * spellmanacost) / 10;

    if (spellknow(splnum) <= 0)
    {
        Sprintf(buf, "%s %s", fullname, "(You cannot recall this spell)");
    }
    else
    {
        char extrabuf[BUFSZ] = "";
        if (spellcooldownleft(splnum) > 0)
            Sprintf(extrabuf, "%d/", spellcooldownleft(splnum));
        const char* fmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ? 
            "%s%s {%s &success; %d%% &mana; %.1f &cool; %s%d &casts; %s}" : "%s%s {%s Success %d%% Mana %.1f Cool %s%d Casts %s}";
        Sprintf(buf, fmt, fullname, descbuf,
            levelbuf,
            percent_success(splnum, TRUE),
            displayed_manacost,
            extrabuf, getspellcooldown(splnum),
            availablebuf);
    }

    boolean inactive = FALSE;
    struct extended_menu_info info = zeroextendedmenuinfo;
    int mcolor = NO_COLOR;
    info.menu_flags |= MENU_FLAGS_USE_SPECIAL_SYMBOLS;
    if (spellcooldownleft(splnum) > 0 || spellknow(splnum) <= 0)
    {
        mcolor = CLR_GRAY;
        info.menu_flags |= MENU_FLAGS_USE_COLOR_FOR_SUFFIXES;
        inactive = TRUE;
    }
    if(!inactive)
        info.menu_flags |= MENU_FLAGS_ACTIVE;

    any.a_int = inactive ? 0 : splnum + 1; /* must be non-zero */

    add_extended_menu(tmpwin, glyph, &any, 0, 0, ATR_NONE, mcolor, buf,
        (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED, info);

}

STATIC_OVL
void
add_alt_spell_prepare_menu_item(tmpwin, i, splaction)
winid tmpwin;
int i;
int splaction;
{
    int splnum = !flags.spellorder ? i : (int)spl_orderindx[i];
    char buf[BUFSIZ], availablebuf[BUFSZ], matcompbuf[BUFSZ];
    anything any = zeroany;
    char fullname[BUFSZ];
    char addsbuf[BUFSZ];
    strcpy(fullname, spellname(splnum));
    *fullname = highc(*fullname);

    int glyph = NO_GLYPH;
    glyph = spellid(splnum) - FIRST_SPELL + GLYPH_SPELL_TILE_OFF;

    //Print spell amount
    if (spellamount(splnum) >= 0)
        Sprintf(availablebuf, "%d", spellamount(splnum));
    else
        strcpy(availablebuf, "Inf.");

    //Print spells gained
    if (spellmatcomp(splnum) > 0)
        Sprintf(addsbuf, "%d", matlists[spellmatcomp(splnum)].spellsgained);
    else
        strcpy(addsbuf, "N/A");

    char fullmatcompdesc[BUFSZ];
    strcpy(fullmatcompdesc, matlists[spellmatcomp(splnum)].description_short);

    if (spellmatcomp(splnum))
        strcpy(matcompbuf, fullmatcompdesc);
    else
        strcpy(matcompbuf, "No components");

    boolean inactive = FALSE;
    struct extended_menu_info info = zeroextendedmenuinfo;
    int mcolor = NO_COLOR;
    info.menu_flags |= MENU_FLAGS_USE_SPECIAL_SYMBOLS;
    if (spellknow(splnum) <= 0)
    {
        Sprintf(buf, "%s %s", fullname, "(You cannot recall this spell)");
        mcolor = CLR_GRAY;
        info.menu_flags |= MENU_FLAGS_USE_COLOR_FOR_SUFFIXES;
        inactive = TRUE;
    }
    else
    {
        const char* fmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ? "%s (%s) {&casts; %s &adds; %s}" : "%s (%s) {Casts %s Adds %s}";
        Sprintf(buf, fmt, fullname, matcompbuf,
            availablebuf, addsbuf);
    }
    if (!inactive)
        info.menu_flags |= MENU_FLAGS_ACTIVE;

    any.a_int = inactive ? 0 : splnum + 1; /* must be non-zero */
    add_extended_menu(tmpwin, glyph, &any, 0, 0, ATR_NONE, mcolor, buf,
        (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED, info);
}

STATIC_OVL
void
add_spell_cast_menu_item(tmpwin, i, splaction, namelength, usehotkey)
winid tmpwin;
int i;
int splaction;
int namelength;
boolean usehotkey;
{
    int splnum = !flags.spellorder || usehotkey ? i : (int)spl_orderindx[i];
    char buf[BUFSZ], availablebuf[BUFSZ], levelbuf[10], statbuf[10], fmt[BUFSZ];
    char shortenedname[BUFSZ] = "";
    char fullname[BUFSZ] = "";
    char categorybuf[BUFSZ] = "";
    anything any = zeroany;

    if (!iflags.menu_tab_sep) 
    {
        if (spellknow(splnum) <= 0)
            Sprintf(fmt, "%%-%ds  %%s", namelength);
        else
            Sprintf(fmt, "%%-%ds  %%s   %%-13s %%4.*f  %%s %%3d%%%% %%4d  %%4s", namelength);
        //        fmt = "%-20s  %2d   %-12s %4d %3d%% %8s";
    }
    else 
    {
        if (spellknow(splnum) <= 0)
            Sprintf(fmt, "%%-%ds\t%%s", namelength);
        else
            Sprintf(fmt, "%%-%ds\t%%s\t%%-13s\t%%4.*f\t%%s\t%%3d%%%%\t%%4d\t%%4s", namelength);
#if 0
        if (spellknow(splnum) <= 0)
            strcpy(fmt, "%s\t%s");
        else
            strcpy(fmt, "%s\t%s\t%s\t%.*f\t%-d%%\t%-d\t%s");
        //        fmt = "%s\t%-d\t%s\t%-d\t%-d%%\t%s";
#endif
    }

    Sprintf(fullname, "%s%s", spellcooldownleft(splnum) > 0 ? "[" : "",
        spellname(splnum));

    //Spell name
    if (strlen(fullname) > (size_t)(spellcooldownleft(splnum) > 0 ? namelength - 1 : namelength))
        strncpy(shortenedname, fullname, (size_t)(spellcooldownleft(splnum) > 0 ? namelength - 1 : namelength));
    else
        strcpy(shortenedname, fullname);

    if (spellcooldownleft(splnum) > 0)
        Strcat(shortenedname, "]");

    //Spell level
    if (spellev(splnum) < -1)
        strcpy(levelbuf, " *");
    else if (spellev(splnum) == -1)
        strcpy(levelbuf, " c");
    else if (spellev(splnum) == 0)
        strcpy(levelbuf, " C");
    else
        Sprintf(levelbuf, "%2d", spellev(splnum));

    strcpy(categorybuf, spelltypemnemonic(spell_skilltype(spellid(splnum))));


    if (spellamount(splnum) >= 0)
        Sprintf(availablebuf, "%d", spellamount(splnum));
    else
        strcpy(availablebuf, "Inf.");

    switch (objects[spellid(splnum)].oc_spell_attribute)
    {
    case A_STR:
        strcpy(statbuf, "Str");
        break;
    case A_DEX:
        strcpy(statbuf, "Dex");
        break;
    case A_CON:
        strcpy(statbuf, "Con");
        break;
    case A_INT:
        strcpy(statbuf, "Int");
        break;
    case A_WIS:
        strcpy(statbuf, "Wis");
        break;
    case A_CHA:
        strcpy(statbuf, "Cha");
        break;
    case A_MAX_INT_WIS:
        strcpy(statbuf, "I/W");
        break;
    case A_MAX_INT_CHA:
        strcpy(statbuf, "I/C");
        break;
    case A_MAX_WIS_CHA:
        strcpy(statbuf, "W/C");
        break;
    case A_MAX_INT_WIS_CHA:
        strcpy(statbuf, "Any");
        break;
    case A_AVG_INT_WIS:
        strcpy(statbuf, "I+W");
        break;
    case A_AVG_INT_CHA:
        strcpy(statbuf, "I+C");
        break;
    case A_AVG_WIS_CHA:
        strcpy(statbuf, "W+C");
        break;
    case A_AVG_INT_WIS_CHA:
        strcpy(statbuf, "All");
        break;
    default:
        strcpy(statbuf, "N/A");
        break;
    }

    double spellmanacost = get_spell_mana_cost(splnum);
    double displayed_manacost = ceil(10 * spellmanacost) / 10;

    //Category
    if (spellknow(splnum) <= 0)
        Sprintf(buf, fmt, shortenedname, "(You cannot recall this spell)");
    else
        Sprintf(buf, fmt, shortenedname, levelbuf,//spellev(splnum),
            categorybuf,
            displayed_manacost >= 100 ? 0 : 1, displayed_manacost,
            statbuf,
            100 - percent_success(splnum, TRUE),
            spellcooldownleft(splnum) > 0 ? spellcooldownleft(splnum) : getspellcooldown(splnum),
            availablebuf);  //spellretention(splnum, retentionbuf));

    any.a_int = splnum + 1; /* must be non-zero */

    char letter = '\0';
    if (usehotkey)
    {
        if (spellhotkey(i) == 10)
            letter = '0';
        else
            letter = spellhotkey(i) + '0';
    }
    else
        letter = 0; // spellet(splnum);

    add_menu(tmpwin, NO_GLYPH, &any, letter, 0, ATR_NONE, (spellcooldownleft(splnum) > 0 && splaction != SPELLMENU_PREPARE) || spellknow(splnum) <= 0 ? CLR_BLACK : NO_COLOR, buf,
        (splnum == splaction) ? MENU_SELECTED : MENU_UNSELECTED);

    //Strcat(shortenedname, "=black");
    //if (spellcooldownleft(splnum) > 0 && splaction != SPELLMENU_PREPARE)
    //{
    //    add_menu_coloring(shortenedname);
    //    strcpy(colorbufs, shortenedname);
    //    (*colorbufcnt)++;
    //}
    /* //Should not be needed
    else
    {
        free_menu_coloring_str(buf);
    }
    */
}

int
dospellmanagemenu()
{
    int i = '\0';

    menu_item* pick_list = (menu_item*)0;
    winid win;
    anything any;

    any = zeroany;
    win = create_nhwindow(NHW_MENU);
    start_menu_ex(win, GHMENU_STYLE_CHOOSE_COMMAND);


    struct available_selection_item
    {
        int charnum;
        char name[BUFSZ];
        int action;
        //int (*function_ptr)(int);
    };
    struct available_selection_item available_selection_item[10] = { { 0 } };
    int selnum = 0;

#if 0
    /* Spell description */
    strcpy(available_selection_item[selnum].name, "Detailed spell description");
    available_selection_item[selnum].function_ptr = &spelldescription;
    available_selection_item[selnum].charnum = 'a' + selnum;

    any = zeroany;
    any.a_char = available_selection_item[selnum].charnum;

    add_menu(win, NO_GLYPH, &any,
        any.a_char, 0, ATR_NONE, NO_COLOR,
        available_selection_item[selnum].name, MENU_UNSELECTED);

    selnum++;
#endif

#ifndef GNH_MOBILE
    strcpy(available_selection_item[selnum].name, "Set or clear a hotkey for a spell");
    //available_selection_item[selnum].function_ptr = &setspellhotkey;
    available_selection_item[selnum].action = 4;
    available_selection_item[selnum].charnum = 'a' + selnum;

    any = zeroany;
    any.a_char = available_selection_item[selnum].charnum;

    add_menu(win, NO_GLYPH, &any,
        any.a_char, 0, ATR_NONE, NO_COLOR,
        available_selection_item[selnum].name, MENU_UNSELECTED);

    selnum++;
#endif

    /* Forget spell */
    strcpy(available_selection_item[selnum].name, "Forget a spell");
    //available_selection_item[selnum].function_ptr = &forgetspell;
    available_selection_item[selnum].action = 5;
    available_selection_item[selnum].charnum = 'a' + selnum;

    any = zeroany;
    any.a_char = available_selection_item[selnum].charnum;

    add_menu(win, NO_GLYPH, &any,
        any.a_char, 0, ATR_NONE, NO_COLOR,
        available_selection_item[selnum].name, MENU_UNSELECTED);

    selnum++;


    /* Finalize the menu */
    char qbuf[BUFSZ] = "";
    strcpy(qbuf, "What do you want to do?");

    /* Finish the menu */
    end_menu(win, qbuf);


    if (selnum <= 0)
    {
        You("do not have management actions available.");
        destroy_nhwindow(win);
        return 0;
    }


    /* Now generate the menu */
    if (select_menu(win, PICK_ONE, &pick_list) > 0)
    {
        i = pick_list->item.a_char;
        free((genericptr_t)pick_list);
    }
    destroy_nhwindow(win);

    if (i == '\0')
        return 0;

    int res = 0, j;
    for (j = 0; j < selnum; j++)
    {
        if (available_selection_item[j].charnum == i)
        {
            if (i != '\0')
            {
                res = available_selection_item[j].action; // (available_selection_item[j].function_ptr)(spell);
            }
            break;
        }
    }

    return res;
}


int
setspellhotkey(spell)
int spell;
{
    char promptbuf[BUFSZ] = "";
    //char answerbuf[BUFSZ] = "";
    char answerchar = '\0';
    char buf[BUFSZ] = "";
    const char* letters = "1234567890-q";

    /* Set or clear hotkey */
    char hotbuf[BUFSZ] = "";
    if (spellhotkey(spell))
        Sprintf(hotbuf, "(\'%d\')", (spellhotkey(spell) % 10));

    Sprintf(promptbuf, "Which hotkey do you want to use for \'%s\'? %s", spellname(spell), hotbuf);
    //getlin(promptbuf, answerbuf);
    answerchar = yn_function(promptbuf, letters, '-', (char*)0);
    //(void)mungspaces(answerbuf);
    if (answerchar == '\033' || answerchar == 'q')
    {
        pline1(Never_mind);
        return 0;
    }
    else if (answerchar == '\0' || answerchar == '-')
    {
        spellhotkey(spell) = 0;
        int multicolors[1] = { CLR_MSG_HINT };
        pline_multi_ex(ATR_NONE, NO_COLOR, no_multiattrs, multicolors, buf, "Hotkey for \'%s\' cleared.", spellname(spell));
    }
    else if (answerchar >= '0' && answerchar <= '9')
    {
        int selected_hotkey = 0;
        if(answerchar == '0')
            selected_hotkey = 10;
        else
            selected_hotkey = answerchar - '0';

        /* clear the existing hotkey */
        int n;
        for (n = 0; n < MAXSPELL; n++)
        {
            if(spl_book[n].sp_hotkey == selected_hotkey)
            {
                spl_book[n].sp_hotkey = 0;
                break;
            }
            if (spellid(n) == NO_SPELL)
                break;
        }

        spellhotkey(spell) = selected_hotkey;
        int multicolors[2] = { CLR_MSG_HINT, CLR_MSG_HINT };
        pline_multi_ex(ATR_NONE, NO_COLOR, no_multiattrs, multicolors, buf, "Hotkey for \'%s\' set to \'%c\'.", spellname(spell), answerchar);
    }
    else
        pline("Illegal hotkey.");

    return 0;
}

int
forgetspell(spell)
int spell;
{
    char qbuf[BUFSZ] = "";
    char spellnamebuf[BUFSZ] = "";
    strcpy(spellnamebuf, spellname(spell));

    Sprintf(qbuf, "Are you sure you want to forget \'%s\' permanently?", spellnamebuf);
    if (yn_query_ex(ATR_NONE, CLR_MSG_WARNING, (char*)0, qbuf) == 'y')
    {
        struct spell empty_spell = { 0 };
        int n;
        for (n = spell + 1; n <= MAXSPELL; n++)
        {
            if (n == MAXSPELL)
                spl_book[n - 1] = empty_spell;
            else
            { 
                spl_book[n - 1] = spl_book[n];
                if (spellid(n) == NO_SPELL)
                    break;
            }
        }
        sortspells();
        char buf[BUFSZ] = "";
        int multicolors[1] = { CLR_MSG_HINT };
        pline_multi_ex(ATR_NONE, NO_COLOR, no_multiattrs, multicolors, buf, "You removed \'%s\' from your memory permanently.", spellnamebuf);
    }

    return 0;
}

long
get_object_spell_casting_penalty(obj)
struct obj* obj;
{
    if (!obj)
        return 0;

    long res = objects[obj->otyp].oc_spell_casting_penalty;

    if (obj->material != objects[obj->otyp].oc_material && is_armor(obj))
    {
        res += material_definitions[obj->material].spell_casting_penalty_armor[objects[obj->otyp].oc_armor_category];
    }

    if (res > 0 && has_obj_mythic_spellcasting(obj))
        return 0;

    return res;
}

STATIC_OVL int
percent_success(spell, limited)
int spell;
boolean limited;
{
    /* Intrinsic and learned ability are combined to calculate
     * the probability of player's success at cast a given spell.
     */
    int chance, statused = A_INT;
    long armor_penalty;
    int skill;

    /* Calculate intrinsic ability (armor_penalty) */
    armor_penalty = 0L; // urole.spelbase;
    statused = attribute_value_for_spellbook(spellid(spell));

    if (!(objects[spellid(spell)].oc_spell_flags & S1_NO_SOMATIC_COMPONENT))
    {
        if (uarm)
            armor_penalty += get_object_spell_casting_penalty(uarm);
        if (uarms)
            armor_penalty += get_object_spell_casting_penalty(uarms);
        if (uarmh)
            armor_penalty += get_object_spell_casting_penalty(uarmh);
        if (uarmg)
            armor_penalty += get_object_spell_casting_penalty(uarmg);
        if (uarmf)
            armor_penalty += get_object_spell_casting_penalty(uarmf);
        if (uarmu)
            armor_penalty += get_object_spell_casting_penalty(uarmu);
        if (uarmo)
            armor_penalty += get_object_spell_casting_penalty(uarmo);
        if (uarmb)
            armor_penalty += get_object_spell_casting_penalty(uarmb);
        if (umisc)
            armor_penalty += get_object_spell_casting_penalty(umisc);
        if (umisc2)
            armor_penalty += get_object_spell_casting_penalty(umisc2);
        if (umisc3)
            armor_penalty += get_object_spell_casting_penalty(umisc3);
        if (umisc4)
            armor_penalty += get_object_spell_casting_penalty(umisc4);
        if (umisc5)
            armor_penalty += get_object_spell_casting_penalty(umisc5);
        if (uamul)
            armor_penalty += get_object_spell_casting_penalty(uamul);
        if (uleft)
            armor_penalty += get_object_spell_casting_penalty(uleft);
        if (uright)
            armor_penalty += get_object_spell_casting_penalty(uright);
        if (ublindf)
            armor_penalty += get_object_spell_casting_penalty(ublindf);
        if (uwep)
            armor_penalty += get_object_spell_casting_penalty(uwep);
    }

    /* Calculate success chance */
    chance = -130;
    chance += -60 * (spellev(spell) + 0);

    skill = P_SKILL_LEVEL(spell_skilltype(spellid(spell)));

    long bonus = 0L;
    bonus += 15L * (long)statused;
    bonus += (long)spell_skill_base_success_bonus(skill);
    bonus += (long)spell_skill_ulevel_success_bonus(skill);
    bonus += (skill == P_ISRESTRICTED ? 0L : 5L * (long)u.uspellcastingbonus_unrestricted); /* items */
    bonus += 5L * (long)u.uspellcastingbonus_all; /* items */

    chance += (int)bonus;
    chance -= (int)(ARMOR_SPELL_CASTING_PENALTY_MULTIPLIER * armor_penalty);

    if (limited)
    {
        /* Clamp to percentile */
        if (chance > 100)
            chance = 100;
        if (chance < 0)
            chance = 0;
    }

    return chance;
}

int spell_skill_base_success_bonus(skill_level)
int skill_level;
{
    return 80 * max(0, skill_level - 1);
}

double spell_skill_ulevel_success_bonus_per_level(skill_level)
int skill_level;
{
    return ((double)max(1, skill_level)) / 4.0f;
}

int spell_skill_ulevel_success_bonus(skill_level)
int skill_level;
{
    double bonusperlevel = spell_skill_ulevel_success_bonus_per_level(skill_level);
    return (int)((double)u.ulevel * bonusperlevel);
}

STATIC_OVL
int
attribute_value_for_spellbook(objectid)
int objectid;
{
    int statused = 0;

    if (objects[objectid].oc_spell_attribute >= 0 && objects[objectid].oc_spell_attribute < A_MAX)
        statused = ACURR((int)objects[objectid].oc_spell_attribute); //ACURR(urole.spelstat);
    else if (objects[objectid].oc_spell_attribute == A_MAX_INT_WIS)
        statused = max(ACURR(A_INT), ACURR(A_WIS));
    else if (objects[objectid].oc_spell_attribute == A_MAX_INT_CHA)
        statused = max(ACURR(A_INT), ACURR(A_CHA));
    else if (objects[objectid].oc_spell_attribute == A_MAX_WIS_CHA)
        statused = max(ACURR(A_CHA), ACURR(A_WIS));
    else if (objects[objectid].oc_spell_attribute == A_MAX_INT_WIS_CHA)
        statused = max(max(ACURR(A_INT), ACURR(A_WIS)), ACURR(A_CHA));
    else if (objects[objectid].oc_spell_attribute == A_AVG_INT_WIS)
        statused = (ACURR(A_INT) + ACURR(A_WIS)) / 2;
    else if (objects[objectid].oc_spell_attribute == A_AVG_INT_CHA)
        statused = (ACURR(A_INT) + ACURR(A_CHA)) / 2;
    else if (objects[objectid].oc_spell_attribute == A_AVG_WIS_CHA)
        statused = (ACURR(A_CHA) + ACURR(A_WIS)) / 2;
    else if (objects[objectid].oc_spell_attribute == A_AVG_INT_WIS_CHA)
        statused = (ACURR(A_INT) + ACURR(A_WIS) + ACURR(A_CHA)) / 3;

    return statused;

}

#if 0
STATIC_OVL char *
spellretention(idx, outbuf)
int idx;
char *outbuf;
{
    long turnsleft, percent, accuracy;
    int skill;

    skill = P_SKILL_LEVEL(spell_skilltype(spellid(idx)));
    skill = max(skill, P_UNSKILLED); /* restricted same as unskilled */
    turnsleft = spellknow(idx);
    *outbuf = '\0'; /* lint suppression */

    if (turnsleft < 1L) {
        /* spell has expired; hero can't successfully cast it anymore */
        Strcpy(outbuf, "(gone)");
    } else if (turnsleft >= (long) SPELL_IS_KEEN) {
        /* full retention, first turn or immediately after reading book */
        Strcpy(outbuf, "100%");
    } else {
        /*
         * Retention is displayed as a range of percentages of
         * amount of time left until memory of the spell expires;
         * the precision of the range depends upon hero's skill
         * in this spell.
         *    expert:  2% intervals; 1-2,   3-4,  ...,   99-100;
         *   skilled:  5% intervals; 1-5,   6-10, ...,   95-100;
         *     basic: 10% intervals; 1-10, 11-20, ...,   91-100;
         * unskilled: 25% intervals; 1-25, 26-50, 51-75, 76-100.
         *
         * At the low end of each range, a value of N% really means
         * (N-1)%+1 through N%; so 1% is "greater than 0, at most 200".
         * SPELL_IS_KEEN is a multiple of 100; SPELL_IS_KEEN/100 loses no precision.
         */
        percent = (turnsleft - 1L) / ((long) SPELL_IS_KEEN / 100L) + 1L;
        accuracy =
            (skill >= P_EXPERT) ? 2L : (skill == P_SKILLED)
                                           ? 5L
                                           : (skill == P_BASIC) ? 10L : 25L;
        /* round up to the high end of this range */
        percent = accuracy * ((percent - 1L) / accuracy + 1L);
        Sprintf(outbuf, "%ld%%-%ld%%", percent - accuracy + 1L, percent);
    }
    return outbuf;
}
#endif

boolean
already_learnt_spell_type(otyp)
int otyp;
{
    if (otyp <= STRANGE_OBJECT || otyp >= NUM_OBJECTS)
        return FALSE;

    if (objects[otyp].oc_class != SPBOOK_CLASS)
        return FALSE;

    int i;
    for (i = 0; i < MAXSPELL; i++)
        if (spellid(i) == NO_SPELL)
            break;
        else if (spellid(i) == otyp)
            return TRUE;

    return FALSE;
}


/* Learn a spell during creation of the initial inventory */
void
initialspell(obj)
struct obj *obj;
{
    int i, otyp = obj->otyp;

    for (i = 0; i < MAXSPELL; i++)
        if (spellid(i) == NO_SPELL || spellid(i) == otyp)
            break;

    if (i == MAXSPELL) {
        impossible("Too many spells memorized!");
    } else if (spellid(i) != NO_SPELL) {
        /* initial inventory shouldn't contain duplicate spellbooks */
        impossible("Spell %s already known.", OBJ_NAME(objects[otyp]));
    } else {
        spl_book[i].sp_id = (short)otyp;
        spl_book[i].sp_lev = (xchar)objects[otyp].oc_spell_level;
        spl_book[i].sp_matcomp = objects[otyp].oc_material_components;
        if(spl_book[i].sp_matcomp)
            spl_book[i].sp_amount = matlists[spl_book[i].sp_matcomp].spellsgained; /* Some amount in the beginning */
        else
            spl_book[i].sp_amount = -1;
        spl_book[i].sp_cooldownlength = (int)objects[otyp].oc_spell_cooldown;
        spl_book[i].sp_cooldownleft = 0;
        spl_book[i].sp_skillchance = (int)objects[otyp].oc_spell_skill_chance;

        incr_spell_nknow(i, 0);
    }
    return;
}


/* Mixing starts here*/
STATIC_VAR int domix_spell_no = -1;
/* the 'X' command, two-weapon moved to M(x) */
int
domix()
{
    if (in_doagain && domix_spell_no > -1)
    {
        return domaterialcomponentsmenu(domix_spell_no);
    }
    else
    {
        domix_spell_no = -1;
        if (getspell(&domix_spell_no, 1))
        {
            //Open mixing menu and explain what components are needed
            return domaterialcomponentsmenu(domix_spell_no);
        }
    }
    domix_spell_no = -1;
    return 0;
}


STATIC_OVL int
domaterialcomponentsmenu(spell)
int spell;
{
    int i;
    int j;

    //This might happen with amnesia etc., the spells no longer "age"
    if (spellknow(spell) <= 0)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "cannot recall this spell or its material components anymore.");
        return 0;
    }

    if (!spellmatcomp(spell))
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        pline_ex(ATR_NONE, CLR_MSG_FAIL, "That spell does not require material components.");
        return 0;
    }

    if (!invent)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "have nothing to prepare spells with.");
        return 0;
    }

    //Start assuming components are ok, unless we find otherwise below
    int result = 1;

    if (*u.ushops)
        sellobj_state(SELL_DELIBERATE);

    struct obj* selcomps[MAX_MATERIALS];
    struct obj* difcomps[MAX_MATERIALS];
    int difcomp_req_amt[MAX_MATERIALS];
    for (j = 0; j < MAX_MATERIALS; j++)
    {
        selcomps[j] = (struct obj*)0;
        difcomps[j] = (struct obj*)0;
        difcomp_req_amt[j] = 0;
    }

    char spellname[BUFSZ] = "";
    char capspellname[BUFSZ] = "";
    strcpy(spellname, obj_descr[spellid(spell)].oc_name);
    strcpy(capspellname, spellname);
    *capspellname = highc(*capspellname); //Make first letter capital

    int matcnt = 0;
    int difmatcnt = 0;

    /* Check the material components here */
    for(j = 0; matlists[spellmatcomp(spell)].matcomp[j].amount != 0; j++)
    {
        matcnt++;
        struct obj* otmp = (struct obj*)0;
        char buf[BUFSZ], buf3[BUFSZ], buf5[BUFSZ];
        struct materialcomponent* mc = &matlists[spellmatcomp(spell)].matcomp[j];
        strcpy(buf3, domatcompname(mc));

        Sprintf(buf, "You need %s%s. ",
            buf3, ((mc->flags & MATCOMP_NOT_SPENT) ? " as a catalyst" : " as a component"));

        Sprintf(buf5, "prepare \'%s\' with", spellname);

        char allclassletters[MAX_MATCOMP_ALTERNATIVES + 1];

        int c = 0;
        for (c = 0; c < MAX_MATCOMP_ALTERNATIVES; c++)
        {
            if (mc->objectid[c] == STRANGE_OBJECT)
                break;

            char classletter = FOOD_CLASS;
            if (mc->objectid[c] != CORPSE && mc->objectid[c] != TIN && mc->objectid[c] != EGG)
                classletter = objects[mc->objectid[c]].oc_class;

            allclassletters[c] = classletter;
        }
        allclassletters[c] = '\0';

        otmp = getobj(allclassletters, buf5, 0, buf);

        if (!otmp)
            return 0;

        //Save the material component
        selcomps[j] = otmp;

        //Check if acceptable
        boolean acceptable = !!is_obj_acceptable_component(mc, otmp, FALSE);

        //Check quantity
        if (otmp->quan < mc->amount)
        {
            play_sfx_sound(SFX_MIXING_NOT_ENOUGH_COMPONENTS);
            pline("%s requires %s%s, but you have only %ld.",
                spellname,
                buf3,
                ((mc->flags & MATCOMP_NOT_SPENT) ? " as a catalyst" : " as a component"),
                otmp->quan);
            return 0;
        }

        //int quan_mult = mc->amount > 0 ? otmp->quan / mc->amount : 1;
        boolean previous_found = FALSE;
        int k;
        for (k = 0; k < difmatcnt; k++)
        {
            if (difcomps[k] == selcomps[j])
            {
                previous_found = TRUE;

                if ((mc->flags & MATCOMP_NOT_SPENT))
                    difcomp_req_amt[k] += 0;
                else
                    difcomp_req_amt[k] += mc->amount;

                break;
            }

        }
        if (!previous_found)
        {
            difcomps[difmatcnt] = selcomps[j];

            if ((mc->flags & MATCOMP_NOT_SPENT))
                difcomp_req_amt[difmatcnt] = 0;
            else
                difcomp_req_amt[difmatcnt] = mc->amount;

            difmatcnt++;
        }

        //Note: You might ask for another pick from another type (e.g., using both blessed and uncursed items), but this gets a bit too complicated
        if (acceptable)
        {
            //Correct ingredient
        }
        else
        {
            //Incorrect ingredient
            result = 0;
        }

    }

    /* Check that the same object has not been selected more than one time */
    boolean same_found = FALSE;
    for (j = 0; j < matcnt; j++)
    {
        for (i = j + 1; i < matcnt; i++)
        {
            if (selcomps[i] == selcomps[j])
            {
                same_found = TRUE;
                break;
            }
        }
    }

    if (same_found)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_cant_ex1(ATR_NONE, CLR_MSG_FAIL, "use the same material component more than once.");
        return 0;
    }

    /* Determine the amount to be mixed */
    boolean failure = !result || ((Confusion || Stunned) && spellev(spell) > 1 && rn2(spellev(spell)));
    int spells_gained_per_mixing = matlists[spellmatcomp(spell)].spellsgained;
    int selected_multiplier = 1;
    int lowest_multiplier = 999;
    int quan_mult = 0;
    int k;
    for (k = 0; k < difmatcnt; k++)
    {
        if (difcomps[k] && difcomp_req_amt[k] > 0)
        {
            quan_mult = (int)difcomps[k]->quan / difcomp_req_amt[k];
            if (quan_mult < lowest_multiplier)
                lowest_multiplier = quan_mult;
        }
    }

    if (lowest_multiplier > 1)
    {
        char qbuf[BUFSZ] = "";
        char ebuf[BUFSZ] = "";
        char buf[BUFSZ] = "";
        Sprintf(qbuf, "You get %d casting%s per mixing. How many times to mix?", spells_gained_per_mixing, plur(spells_gained_per_mixing));
        Sprintf(ebuf, "[max %d] (1)", lowest_multiplier);
        getlin_ex(GETLINE_NUMBERS_ONLY, ATR_NONE, NO_COLOR, qbuf, buf, (char*)0, ebuf, (char*)0);
        (void)mungspaces(buf);

        if (buf[0] == '\033')
        {
            return 0;
        }
        else if (buf[0] == ' ' || buf[0] == '\0')
        {
            selected_multiplier = 1;
        }
        else
        {
            int count = atoi(buf);
            if (count > 0)
            {
                selected_multiplier = min(lowest_multiplier, count);
            }
        }
    }

    /* Now go through the selected material components */
    for (j = 0; j < matcnt; j++)
    {
        struct obj* otmp = selcomps[j];
        struct materialcomponent* mc = &matlists[spellmatcomp(spell)].matcomp[j];

        if (!otmp || !mc)
            continue;

        int mc_alternatives = count_matcomp_alternatives(mc);

        if (!failure && mc_alternatives == 1)
            makeknown(otmp->otyp);

        boolean usecomps = !(mc->flags & MATCOMP_NOT_SPENT);

        //Resisting objects cannot be consumed
        if (obj_resists(otmp, 0, 100))
            usecomps = FALSE;

        if (otmp->otyp != mc->objectid[0])
        {
            //Wrong item
            if(otmp->oclass != objects[mc->objectid[0]].oc_class
                && otmp->oclass != POTION_CLASS && otmp->oclass != SCROLL_CLASS && otmp->oclass != FOOD_CLASS)
            {
                //The same class may get consumed, but not a different class, unless it is a potion, scroll, or food
                usecomps = FALSE;
            }

            //Disintegration resistant items, indestructible items, and fire-resistant items won't get destroyed
            //Loadstone will not get destroyed, nor do containers or worn items, which can be cursed
            if(oresist_disintegration(otmp)
                || oresist_fire(otmp)
                || is_obj_indestructible(otmp)
                || Is_container(otmp)
                || (objects[otmp->otyp].oc_flags & O1_CANNOT_BE_DROPPED_IF_CURSED)
                || (objects[otmp->otyp].oc_flags & O1_BECOMES_CURSED_WHEN_PICKED_UP_AND_DROPPED)
                || (otmp->owornmask & ~W_WEAPON)
                || (otmp->oclass == WEAPON_CLASS && (otmp->owornmask & W_WEAPON)))
                usecomps = FALSE;
        }

        //Use them all up
        if (usecomps /*!(mc->flags & MATCOMP_NOT_SPENT) && !obj_resists(otmp,0,100) && otmp->oclass == objects[mc->objectid[0]].oc_class */)
        {
            int used_amount = (failure ? 1 : selected_multiplier) * mc->amount;
            if(otmp->quan >= used_amount)
            {
                for (i = 0; i < used_amount; i++)
                    useup(otmp);

                //Note: otmp cannot appear twice in the selcomps list due to the same_found check, so we are good to continue
            }
            else
            {
                impossible("There should always be enough material components at this stage");
                useupall(otmp);
                failure = TRUE;
            }
        }
    }

    /* And now the result */
    if (failure)
    {
        //Explosion
        int dmg = d(max(1, spellev(spell)), 6);
        if (spellev(spell) == 0)
            dmg = rnd(4);
        else if (spellev(spell) == -1)
            dmg = rnd(2);
        else if (spellev(spell) < -1)
            dmg = 1;

        char buf[BUFSZ];
        Sprintf(buf, "killed by a failed spell preparation");

        int explosionchance = min(100, max(0, (spellev(spell) - 2) * 10 + dmg * 4));

        if ((99-rnl(100)) < explosionchance)//If lucky explosions happen less frequently
        {
            //One more damage
            Your_ex(ATR_NONE, CLR_MSG_NEGATIVE, "concoction explodes in a large ball of fire!");
            losehp(adjust_damage(dmg, (struct monst*)0, &youmonst, AD_FIRE, ADFLAGS_NONE), buf, NO_KILLER_PREFIX);
            explode(u.ux, u.uy, RAY_FIRE, &youmonst, 0, 0, 1, 0, 0, EXPL_FIERY);
        }
        else
        {
            play_sfx_sound(SFX_MIXING_FLARES_UP);
            Your_ex(ATR_NONE, CLR_MSG_NEGATIVE, "concoction flares up, burning you!");
            display_u_being_hit(HIT_ON_FIRE, dmg, 0UL);
            losehp(adjust_damage(dmg, (struct monst*)0, &youmonst, AD_FIRE, ADFLAGS_NONE), buf, NO_KILLER_PREFIX);
        }
    }
    else
    {
        //Success
        int addedamount = spells_gained_per_mixing * selected_multiplier;
        spellamount(spell) += addedamount;
        play_sfx_sound(SFX_MIXING_SUCCESS);
        You_ex(ATR_NONE, CLR_MSG_SUCCESS, "successfully prepared the material components.");
        int multicolors[2] = { CLR_MSG_HINT, CLR_MSG_SPELL };
        if (addedamount == 1)
            You_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, "now have %s more casting of \'%s\' prepared.", "one", spellname);
        else
            You_multi_ex(ATR_NONE, CLR_MSG_SUCCESS, no_multiattrs, multicolors, "now have %d more castings of \'%s\' prepared.", addedamount, spellname);

#if 0
        /* gain skill for successful preparation */
        int otyp = spellid(spell);
        int skill = spell_skilltype(otyp);
        int skillchance = spellskillchance(spell);
        int skillpoints = max(0, max(spellev(spell) + 2, 1) * addedamount * skillchance / 100);

        if(skillpoints > 0)    
            use_skill(skill, skillpoints);
#endif
    }


    if (*u.ushops)
        sellobj_state(SELL_NORMAL);
    if (result)
        reset_occupations();

    return result;

}

STATIC_OVL
int
count_matcomp_alternatives(mc)
struct materialcomponent* mc;
{
    int cnt = 0;
    int i;
    for (i = 0; i < MAX_MATCOMP_ALTERNATIVES; i++)
    {
        if (mc->objectid[i] == STRANGE_OBJECT)
            break;

        cnt++;
    }

    return cnt;
}


STATIC_OVL
boolean
is_acceptable_component_object_type(mc, otyp)
struct materialcomponent *mc;
int otyp;
{
    int i;
    for (i = 0; i < MAX_MATCOMP_ALTERNATIVES; i++)
    {
        if (mc->objectid[i] == STRANGE_OBJECT)
            break;

        if (mc->objectid[i] == otyp)
            return TRUE;
    }
    return FALSE;
}

STATIC_OVL
boolean
is_acceptable_component_monster_type(mc, mnum)
struct materialcomponent* mc;
int mnum;
{
    int i;
    for (i = 0; i < MAX_MATCOMP_MONSTER_ALTERNATIVES; i++)
    {
        if (mc->monsterid[i] == NOT_APPLICABLE)
            break;

        if (mc->monsterid[i] == mnum)
            return TRUE;
    }
    return FALSE;
}

STATIC_OVL
uchar
is_obj_acceptable_component(mc, otmp, also_possible)
struct materialcomponent* mc;
struct obj* otmp;
boolean also_possible;
{
    boolean acceptable = FALSE;
    if (is_acceptable_component_object_type(mc, otmp->otyp))
        acceptable = TRUE;

    if ((mc->flags & MATCOMP_BLESSED_REQUIRED) && !otmp->blessed)
        acceptable = otmp->bknown || !also_possible ? FALSE : 2;

    if ((mc->flags & MATCOMP_CURSED_REQUIRED) && !otmp->cursed)
        acceptable = otmp->bknown || !also_possible ? FALSE : 2;

    if ((mc->flags & MATCOMP_NOT_CURSED) && otmp->cursed)
        acceptable = otmp->bknown || !also_possible ? FALSE : 2;

    if ((mc->flags & MATCOMP_DEATH_ENCHANTMENT_REQUIRED) && otmp->elemental_enchantment != DEATH_ENCHANTMENT)
        acceptable = FALSE;

    if ((is_acceptable_component_object_type(mc, CORPSE) || is_acceptable_component_object_type(mc, TIN) || is_acceptable_component_object_type(mc, EGG))
        && mc->monsterid[0] >= 0 && !is_acceptable_component_monster_type(mc, otmp->corpsenm))
        acceptable = FALSE;

    return acceptable;
}

uchar
is_obj_component_for(spell, otmp)
int spell;
struct obj* otmp;
{
    int j;
    uchar res;
    for (j = 0; matlists[spellmatcomp(spell)].matcomp[j].amount != 0; j++)
    {
        struct materialcomponent* mc = &matlists[spellmatcomp(spell)].matcomp[j];
        res = is_obj_acceptable_component(mc, otmp, TRUE);
        if (res)
            return res;
    }
    return 0;
}

const char*
domatcompname(mc)
struct materialcomponent* mc;
{
    /* in general, use the description for complicated (e.g., multialternative) cases */
    if(mc->description && strcmp(mc->description, ""))
        return mc->description;

    /* otherwise construct the description based on the first component */
    struct objclass* perobj = (struct objclass*)0;
    if (mc->objectid[0] >= 0)
        perobj = &objects[mc->objectid[0]];

    struct permonst* permon = (struct permonst*)0;
    if (mc->monsterid[0] >= 0)
        permon = &mons[mc->monsterid[0]];

    if (!perobj || !mc || mc->amount == 0)
        return empty_string;

    static char buf3[BUFSZ];
    char buf2[BUFSZ], buf4[BUFSZ];
    strcpy(buf2, "");
    strcpy(buf4, "");

    if (permon && perobj)
    {
        if (mc->objectid[0] == CORPSE || mc->objectid[0] == EGG)
            //Add "lizard" to "corpse" to get "lizard corpse" (or lizard egg)
            Sprintf(buf4, "%s %s", permon->mname, obj_descr[perobj->oc_name_idx].oc_name);
        else if (mc->objectid[0] == TIN)
            //Add "lizard" to "tin" to get "lizard corpse"
            Sprintf(buf4, "%s of %s meat", obj_descr[perobj->oc_name_idx].oc_name, permon->mname);
        else
            Sprintf(buf4, "%s%s", obj_descr[perobj->oc_name_idx].oc_name, GemStone(mc->objectid[0]) ? " stone" : "");
    }
    else
    {
        Sprintf(buf4, "%s%s%s",
            objects[mc->objectid[0]].oc_class == SCROLL_CLASS ? "scroll of " :
            objects[mc->objectid[0]].oc_class == POTION_CLASS ? "potion of " :
            objects[mc->objectid[0]].oc_class == WAND_CLASS ? "wand of " :
            objects[mc->objectid[0]].oc_class == SPBOOK_CLASS ? "spellbook of " : "",
            obj_descr[perobj->oc_name_idx].oc_name,
            GemStone(mc->objectid[0]) ? " stone" : "");
    }

    //Correct type of component
    Sprintf(buf2, "%s%s%s",
        (mc->flags & MATCOMP_BLESSED_REQUIRED ? "blessed " : mc->flags & MATCOMP_CURSED_REQUIRED ? "cursed " : (mc->flags & MATCOMP_NOT_CURSED ? "noncursed " : "")),
        (mc->flags & MATCOMP_DEATH_ENCHANTMENT_REQUIRED ? "death-magical " : ""),
        buf4);

    //Indicate how many
    if (mc->amount == 1)
        strcpy(buf3, an(buf2));
    else
        Sprintf(buf3, "%d %s", mc->amount, makeplural(buf2));

    return buf3;
}

int
getspellcooldown(spell)
int spell;
{
    if (spell < 0)
        return 0;

    //int cooldown = objects[spellid(spell)].oc_spell_cooldown;
    int cooldown = spellcooldownlength(spell);

    /*
    int spell_level = spellev(spell);
    int cooldown = spell_level + 1;

    if (spell_level < -1)
        cooldown = 0;

    if (spell_level == 8)
        cooldown = 10;
    else if (spell_level == 9)
        cooldown = 12;
    else if (spell_level == 10)
        cooldown = 14;
        */
    return cooldown;
}

#if defined (DUMPLOG) || defined (DUMPHTML)
void
dump_spells()
{
    if (spellid(0) == NO_SPELL)
    {
        putstr(0, ATR_HEADING, "You did not know any spells.");
    }
    else
    {
        int i;
        char buf[BUFSZ];
        char spellnamebuf[BUFSZ];
        char castingsbuf[BUFSZ] = "";
        char successbuf[BUFSZ] = "";
        putstr(0, ATR_HEADING, "Spells in your repertoire:");
        for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
        {
            int pct_lim = percent_success(i, TRUE);

            if (spellamount(i) >= 0)
                Sprintf(castingsbuf, " %d casting%s left", spellamount(i), plur(spellamount(i)));
            else
                Strcpy(castingsbuf, "");

            Strcpy(spellnamebuf, spellname(i));
            *spellnamebuf = highc(*spellnamebuf);
            Sprintf(successbuf, "%d%% success", pct_lim);

            Sprintf(buf, " %-34s %-13s%s", spellnamebuf, successbuf, castingsbuf);
            putstr(0, ATR_PREFORM, buf);
        }
    }
}
#endif

void
reset_spells(VOID_ARGS)
{
    docast_spell_no = -1;
    domix_spell_no = -1;
    memset((genericptr_t)&save_tport, 0, sizeof(save_tport));
}

/*spell.c*/
