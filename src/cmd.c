/* GnollHack File Change Notice: This file has been changed from the original. Date of last change: 2023-05-22 */

/* GnollHack 4.0    cmd.c    $NHDT-Date: 1557088405 2019/05/05 20:33:25 $  $NHDT-Branch: GnollHack-3.6.2-beta01 $:$NHDT-Revision: 1.333 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2013. */
/* GnollHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include "func_tab.h"
#include <math.h>

/* Macros for meta and ctrl modifiers:
 *   M and C return the meta/ctrl code for the given character;
 *     e.g., (C('c') is ctrl-c
 */
#ifndef M
#ifndef NHSTDC
#define M(c) (0x80 | (c))
#else
#define M(c) ((c) - 128)
#endif /* NHSTDC */
#endif

#ifndef C
#define C(c) (0x40 & (c) ? 0x1f & (c) : (0x80 | (0x1f & (c))))
#endif

#ifdef ALTMETA
STATIC_VAR boolean alt_esc = FALSE;
#endif
STATIC_VAR boolean escape_sequence_key_start_allowed = FALSE;
STATIC_VAR NEARDATA int last_multi;

struct cmd Cmd = { 0 }; /* flag.h */

extern const char *hu_stat[];  /* hunger status from eat.c */
extern const char *enc_stat[]; /* encumbrance status from botl.c */

#ifdef UNIX
/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS 20
#endif
#endif

#define CMD_TRAVEL (char) 0xFC //0x90
#define CMD_CLICKLOOK (char) 0xFD //0x8F
#define CMD_TRAVEL_ATTACK (char) 0xFE
#define CMD_TRAVEL_WALK (char) 0xFB
#define CMD_CLICKFIRE (char) 0xE0 //(Meta-0x60)

#ifdef DEBUG
extern int NDECL(wiz_debug_cmd_bury);
#endif

#ifdef DUMB /* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply);            /**/
extern int NDECL(dorub);              /**/
extern int NDECL(dojump);             /**/
extern int NDECL(doextlist);          /**/
extern int NDECL(enter_explore_mode); /**/
extern int NDECL(dodrop);             /**/
extern int NDECL(doddrop);            /**/
extern int NDECL(dodown);             /**/
extern int NDECL(doup);               /**/
extern int NDECL(donull);             /**/
extern int NDECL(dowipe);             /**/
extern int NDECL(docallcnd);          /**/
extern int NDECL(dotakeoff);          /**/
extern int NDECL(doremring);          /**/
extern int NDECL(dowear);             /**/
extern int NDECL(doputon);            /**/
extern int NDECL(doddoremarm);        /**/
extern int NDECL(dokick);             /**/
extern int NDECL(dofire);             /**/
extern int NDECL(dothrow);            /**/
extern int NDECL(doeat);              /**/
extern int NDECL(done2);              /**/
extern int NDECL(vanquished);         /**/
extern int NDECL(doengrave);          /**/
extern int NDECL(dopickup);           /**/
extern int NDECL(ddoinv);             /**/
extern int NDECL(dotypeinv);          /**/
extern int NDECL(dolook);             /**/
extern int NDECL(doprgold);           /**/
extern int NDECL(doprwep);            /**/
extern int NDECL(doprarm);            /**/
extern int NDECL(doprring);           /**/
extern int NDECL(dopramulet);         /**/
extern int NDECL(doprtool);           /**/
extern int NDECL(dosuspend);          /**/
extern int NDECL(doforce);            /**/
extern int NDECL(doopen);             /**/
extern int NDECL(doclose);            /**/
extern int NDECL(dosh);               /**/
extern int NDECL(dodiscovered);       /**/
extern int NDECL(doclassdisco);       /**/
extern int NDECL(doset);              /**/
extern int NDECL(dotogglepickup);     /**/
extern int NDECL(dotoggledecorations);/**/
extern int NDECL(dowhatis);           /**/
extern int NDECL(doquickwhatis);      /**/
extern int NDECL(dowhatdoes);         /**/
extern int NDECL(dohelp);             /**/
extern int NDECL(dohistory);          /**/
extern int NDECL(doloot);             /**/
extern int NDECL(dodrink);            /**/
extern int NDECL(dodip);              /**/
extern int NDECL(dosacrifice);        /**/
extern int NDECL(dopray);             /**/
extern int NDECL(dotip);              /**/
extern int NDECL(doturn);             /**/
extern int NDECL(doredraw);           /**/
extern int NDECL(doread);             /**/
extern int NDECL(dosave);             /**/
extern int NDECL(dosearch);           /**/
extern int NDECL(doidtrap);           /**/
extern int NDECL(dopay);              /**/
extern int NDECL(dosit);              /**/
extern int NDECL(dotalk);             /**/
extern int NDECL(docast);             /**/
extern int NDECL(dovspell);           /**/
extern int NDECL(dosortspell);        /**/
extern int NDECL(dotelecmd);          /**/
extern int NDECL(dountrap);           /**/
extern int NDECL(doversion);          /**/
extern int NDECL(doextversion);       /**/
extern int NDECL(doswapweapon);       /**/
extern int NDECL(dowield);            /**/
extern int NDECL(dowieldquiver);      /**/
extern int NDECL(dounwield);          /**/
extern int NDECL(dostash);            /**/
extern int NDECL(dozap);              /**/
extern int NDECL(doorganize);         /**/
#endif /* DUMB */

#ifdef ANDROID
extern void NDECL(quit_possible);
#endif

STATIC_DCL int NDECL((*timed_occ_fn));

STATIC_PTR int NDECL(dosuspend_core);
STATIC_PTR int NDECL(dosh_core);
STATIC_PTR int NDECL(doherecmdmenu);
STATIC_PTR int NDECL(dotherecmdmenu);
STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(dolight);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
STATIC_PTR int NDECL(dotravel);
STATIC_PTR int NDECL(doterrain);
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_intrinsic);
STATIC_PTR int NDECL(wiz_map);
STATIC_PTR int NDECL(wiz_makemap);
STATIC_PTR int NDECL(wiz_files);
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
#if defined(DEBUG)
STATIC_PTR int NDECL(wiz_panic);
#endif
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_smell);
STATIC_PTR int NDECL(wiz_intrinsic);
STATIC_PTR int NDECL(wiz_show_wmodes);
STATIC_DCL void NDECL(wiz_map_levltyp);
#if !defined(GNH_MOBILE) && defined(DEBUG)
STATIC_DCL int NDECL(wiz_save_monsters);
STATIC_DCL int NDECL(wiz_save_tiledata);
STATIC_DCL int NDECL(wiz_count_tiles);
STATIC_DCL int NDECL(wiz_save_glyph2tiles);
STATIC_DCL int NDECL(wiz_save_quest_texts);
STATIC_DCL int NDECL(wiz_save_spells);
#endif
STATIC_DCL void NDECL(wiz_levltyp_legend);
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int NDECL(wiz_migrate_mons);
#endif
STATIC_DCL size_t FDECL(size_monst, (struct monst *, BOOLEAN_P));
STATIC_DCL size_t FDECL(size_obj, (struct obj *));
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, size_t *,
                                  BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *,
                                  BOOLEAN_P, long *, size_t *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *,
                                         long *, size_t *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *,
                                  BOOLEAN_P, long *, size_t *));
STATIC_DCL void FDECL(contained_stats, (winid, const char *, long *, size_t *));
STATIC_DCL void FDECL(misc_stats, (winid, long *, size_t *));
STATIC_PTR int NDECL(wiz_show_stats);
STATIC_DCL boolean FDECL(accept_menu_prefix, (int NDECL((*))));
STATIC_PTR int NDECL(wiz_rumor_check);
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(dopolyformstatistics);
STATIC_DCL int FDECL(doviewpetstatistics, (struct monst*));

STATIC_DCL void FDECL(enlght_out, (const char *, int));
STATIC_DCL void FDECL(enlght_line, (const char *, const char *, const char *,
                                    const char *, BOOLEAN_P));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *, int, int, char *));
STATIC_DCL void FDECL(enlght_halfdmg, (int, int));
STATIC_DCL boolean NDECL(walking_on_water);
STATIC_DCL boolean FDECL(cause_known, (int));
STATIC_DCL char *FDECL(attrval, (int, int, char *));
STATIC_DCL void FDECL(background_enlightenment, (int, int));
STATIC_DCL void FDECL(basics_enlightenment, (int, int));
STATIC_DCL void FDECL(characteristics_enlightenment, (int, int));
STATIC_DCL void FDECL(one_characteristic, (int, int, int));
STATIC_DCL void FDECL(status_enlightenment, (int, int));
STATIC_DCL void FDECL(attributes_enlightenment, (int, int));

STATIC_DCL void FDECL(add_herecmd_menuitem, (winid, int NDECL((*)),
                                             const char *));
STATIC_DCL char FDECL(here_cmd_menu, (BOOLEAN_P));
STATIC_DCL char FDECL(there_cmd_menu, (BOOLEAN_P, int, int));
STATIC_DCL char *NDECL(parse);
STATIC_DCL void FDECL(show_direction_keys, (winid, CHAR_P, BOOLEAN_P));
STATIC_DCL boolean FDECL(help_dir, (CHAR_P, int, const char *));
STATIC_DCL void FDECL(add_command_menu_items, (winid, int));
STATIC_DCL void NDECL(check_gui_special_effect);
STATIC_DCL void FDECL(print_monster_abilities, (winid, int*, BOOLEAN_P));
STATIC_DCL void FDECL(print_weapon_skill_line_core, (enum p_skills, BOOLEAN_P, int));
STATIC_DCL void FDECL(print_weapon_skill_line, (struct obj*, BOOLEAN_P, int));

STATIC_DCL const char* FDECL(get_spell_type, (int)); //needed for wiz_save_spells
STATIC_DCL const char* FDECL(get_attribute, (int)); //needed for wiz_save_spells
STATIC_DCL const char* FDECL(get_targeting, (unsigned int)); //needed for wiz_save_spells

STATIC_VAR const char *readchar_queue = "";
STATIC_VAR coord clicklook_cc;
//STATIC_VAR coord clickfire_cc;
STATIC_VAR boolean special_effect_shown = FALSE;

STATIC_PTR int
doprev_message(VOID_ARGS)
{
    return nh_doprev_message();
}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation(VOID_ARGS)
{
    (*timed_occ_fn)();
    if (multi > 0)
        multi--;
    return multi > 0;
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *      Currently:      Take off all armor.
 *                      Picking Locks / Forcing Chests.
 *                      Setting traps.
 */
void
reset_occupations()
{
    reset_remarm();
    reset_pick();
    reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, soundset_id, occ_type, sound_type, xtime)
int NDECL((*fn));
const char *txt;
enum object_soundset_types soundset_id;
enum object_occupation_types occ_type;
enum occupation_sound_types sound_type;
int xtime;
{
    if (xtime) 
    {
        occupation = timed_occupation;
        timed_occ_fn = fn;
    } 
    else
        occupation = fn;

    occtxt = txt;
    occtime = 0;
    occsoundset = soundset_id;
    occtyp = occ_type;
    if (soundset_id > 0)
    {
        if(sound_type < MAX_OCCUPATION_SOUND_TYPES)
            play_occupation_immediate_sound(soundset_id, occ_type, sound_type);

        start_occupation_ambient_sound(soundset_id, occ_type);
    }
    return;
}

STATIC_DCL char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
STATIC_VAR char pushq[BSIZE], saveq[BSIZE];
STATIC_VAR NEARDATA int phead, ptail, shead, stail;

STATIC_OVL char
popch(VOID_ARGS)
{
    /* If occupied, return '\0', letting tgetch know a character should
     * be read from the keyboard.  If the character read is not the
     * ABORT character (as checked in pcmain.c), that character will be
     * pushed back on the pushq.
     */
    if (occupation)
        return '\0';
    if (in_doagain)
        return (char) ((shead != stail) ? saveq[stail++] : '\0');
    else
        return (char) ((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar(VOID_ARGS) /* courtesy of aeb@cwi.nl */
{
    register int ch;

    if (iflags.debug_fuzzer)
        return randomkey();
    if (!(ch = popch()))
        ch = nhgetch();
    return (char) ch;
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
    if (!ch)
        phead = ptail = 0;
    if (phead < BSIZE)
        pushq[phead++] = ch;
    return;
}

/* A ch == 0 resets the saveq.  Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
    if (!in_doagain && !skip_savech) {
        if (!ch)
            phead = ptail = shead = stail = 0;
        else if (shead < BSIZE)
            saveq[shead++] = ch;
    }
    return;
}

/* here after # - now read a full-word command */
STATIC_PTR int
doextcmd(VOID_ARGS)
{
    int idx, retval;
    int NDECL((*func));

    /* keep repeating until we don't run help or quit */
    do {
        idx = get_ext_cmd();
        if (idx < 0)
            return 0; /* quit */

        func = extcmdlist[idx].ef_funct;
        if (!wizard && (extcmdlist[idx].flags & WIZMODECMD)) {
            play_sfx_sound(SFX_GENERAL_CANNOT);
            You_ex(ATR_NONE, CLR_MSG_FAIL, "can't do that.");
            return 0;
        }
        if (!(wizard || discover || CasualMode) && (extcmdlist[idx].flags & CASUALMODECMD)) {
            play_sfx_sound(SFX_GENERAL_CANNOT);
            You_ex(ATR_NONE, CLR_MSG_FAIL, "can't do that.");
            return 0;
        }
        if (iflags.menu_requested && !accept_menu_prefix(func)) {
            pline("'%s' prefix has no effect for the %s command.",
                  visctrl(Cmd.spkeys[NHKF_REQMENU]),
                  extcmdlist[idx].ef_txt);
            iflags.menu_requested = FALSE;
        }
        retval = (*func)();
    } while (func == doextlist);

    return retval;
}

/* here after #? - now list all full-word commands and provid
   some navigation capability through the long list */
int
doextlist(VOID_ARGS)
{
    register const struct ext_func_tab *efp;
    char buf[BUFSZ], searchbuf[BUFSZ], promptbuf[QBUFSZ];
    winid menuwin;
    anything any;
    menu_item* selected = (menu_item*)0;
    int n, pass;
    int menumode = 0, menushown[3], onelist = 0;
    boolean redisplay = TRUE, search = FALSE;
    static const char *headings[] = { "Extended commands",
                                      "Casual Extended Commands",
                                      "Debugging Extended Commands",
                                      };

    searchbuf[0] = '\0';
    menuwin = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_DISPLAY_FILE, NO_GLYPH, zerocreatewindowinfo);

    while (redisplay) {
        redisplay = FALSE;
        any = zeroany;
        start_menu_ex(menuwin, GHMENU_STYLE_CHOOSE_COMMAND);
#ifndef GNH_MOBILE
        add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_TITLE, NO_COLOR,
                 "Extended Commands List", MENU_UNSELECTED);
        add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_HALF_SIZE, NO_COLOR,
            " ", MENU_UNSELECTED);
#endif

        Strcpy(buf, menumode ? "Show" : "Hide");
        Strcat(buf, " commands that don't autocomplete");
        if (!menumode)
            Strcat(buf, " (those not marked with [A])");
        any.a_int = 1;
        add_menu(menuwin, NO_GLYPH, &any, 'a', 0, ATR_NONE, NO_COLOR, buf,
                 MENU_UNSELECTED);

        if (!*searchbuf) {
            any.a_int = 2;
            /* was 's', but then using ':' handling within the interface
               would only examine the two or three meta entries, not the
               actual list of extended commands shown via separator lines;
               having ':' as an explicit selector overrides the default
               menu behavior for it; we retain 's' as a group accelerator */
            add_menu(menuwin, NO_GLYPH, &any, ':', 's', ATR_NONE, NO_COLOR,
                     "Search extended commands", MENU_UNSELECTED);
        } else {
            Strcpy(buf, "Show all, clear search");
            if (strlen(buf) + strlen(searchbuf) + strlen(" (\"\")") < QBUFSZ)
                Sprintf(eos(buf), " (\"%s\")", searchbuf);
            any.a_int = 3;
            /* specifying ':' as a group accelerator here is mostly a
               statement of intent (we'd like to accept it as a synonym but
               also want to hide it from general menu use) because it won't
               work for interfaces which support ':' to search; use as a
               general menu command takes precedence over group accelerator */
            add_menu(menuwin, NO_GLYPH, &any, 's', ':', ATR_NONE, NO_COLOR,
                     buf, MENU_UNSELECTED);
        }
        if (wizard) {
            any.a_int = 4;
            add_menu(menuwin, NO_GLYPH, &any, 'z', 0, ATR_NONE, NO_COLOR,
                     onelist ? "Show debugging commands in separate section"
                     : "Show all alphabetically, including debugging commands",
                     MENU_UNSELECTED);
        }
        any = zeroany;
        add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_HALF_SIZE, NO_COLOR,
                 " ", MENU_UNSELECTED);
        menushown[0] = menushown[1] = menushown[2] = 0;
        n = 0;
        for (pass = 0; pass <= 2; ++pass) {
            /* skip second/third pass if not in wizard mode or wizard mode
               commands are being integrated into a single list */
            if (pass == 1 && (onelist || !(wizard || discover || CasualMode)))
                continue;
            if (pass == 2 && (onelist || !wizard))
                break;
            for (efp = extcmdlist; efp->ef_txt; efp++) {
                int wizc, casualc;

                if ((efp->flags & CMD_NOT_AVAILABLE) != 0)
                    continue;
                /* if hiding non-autocomplete commands, skip such */
                if (menumode == 1 && (efp->flags & AUTOCOMPLETE) == 0)
                    continue;
                /* if searching, skip this command if it doesn't match */
                if (*searchbuf
                    /* first try case-insensitive substring match */
                    && !strstri(efp->ef_txt, searchbuf)
                    && !strstri(efp->ef_desc, searchbuf)
                    /* wildcard support; most interfaces use case-insensitve
                       pmatch rather than regexp for menu searching */
                    && !pmatchi(searchbuf, efp->ef_txt)
                    && !pmatchi(searchbuf, efp->ef_desc))
                    continue;
                /* skip wizard mode commands if not in wizard mode;
                   when showing two sections, skip wizard mode commands
                   in pass==0 and skip other commands in pass==1 */
                wizc = (efp->flags & WIZMODECMD) != 0;
                casualc = (efp->flags & CASUALMODECMD) != 0;
                if (wizc && !wizard)
                    continue;
                if (casualc && !(wizard || discover || CasualMode))
                    continue;
                if (!onelist && ((pass != 1 && casualc) || (pass == 1 && !casualc)))
                    continue;
                if (!onelist && ((pass != 2 && wizc) || (pass == 2 && !wizc)))
                    continue;

                /* We're about to show an item, have we shown the menu yet?
                   Doing menu in inner loop like this on demand avoids a
                   heading with no subordinate entries on the search
                   results menu. */
                if (!menushown[pass]) {
                    Strcpy(buf, headings[pass]);
                    add_extended_menu(menuwin, NO_GLYPH, &any, 0, 0,
                             iflags.menu_headings | ATR_HEADING, NO_COLOR, buf, MENU_UNSELECTED, menu_heading_info());
                    menushown[pass] = 1;
                }
                Sprintf(buf, " %-14s %-3s %s",
                        efp->ef_txt,
                        (efp->flags & AUTOCOMPLETE) ? "[A]" : "  ",
                        efp->ef_desc);
                add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_INDENT_AT_BRACKET_OR_DOUBLE_SPACE, NO_COLOR,
                         buf, MENU_UNSELECTED);
                ++n;
            }
            if (n)
                add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_HALF_SIZE, NO_COLOR,
                         " ", MENU_UNSELECTED);
        }
        if (*searchbuf && !n)
            add_menu(menuwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
                     "no matches", MENU_UNSELECTED);
#ifndef GNH_MOBILE
        end_menu(menuwin, (char *) 0);
#else
        end_menu(menuwin, "Extended Commands List");
#endif
        n = select_menu(menuwin, PICK_ONE, &selected);
        if (n > 0 && selected)
        {
            switch (selected[0].item.a_int) 
            {
            case 1: /* 'a': toggle show/hide non-autocomplete */
                menumode = 1 - menumode;  /* toggle 0 -> 1, 1 -> 0 */
                redisplay = TRUE;
                break;
            case 2: /* ':' when not searching yet: enable search */
                search = TRUE;
                break;
            case 3: /* 's' when already searching: disable search */
                search = FALSE;
                searchbuf[0] = '\0';
                redisplay = TRUE;
                break;
            case 4: /* 'z': toggle showing wizard mode commands separately */
                search = FALSE;
                searchbuf[0] = '\0';
                onelist = 1 - onelist;  /* toggle 0 -> 1, 1 -> 0 */
                redisplay = TRUE;
                break;
            }
            free((genericptr_t) selected);
        } else {
            search = FALSE;
            searchbuf[0] = '\0';
        }
        if (search) {
            Strcpy(promptbuf, "Extended command list search phrase");
            Strcat(promptbuf, "?");
            getlin(promptbuf, searchbuf);
            (void) mungspaces(searchbuf);
            if (searchbuf[0] == '\033')
                searchbuf[0] = '\0';
            if (*searchbuf)
                redisplay = TRUE;
            search = FALSE;
        }
    }
    destroy_nhwindow(menuwin);
    return 0;
}

#if defined(TTY_GRAPHICS) || defined(CURSES_GRAPHICS)
#define MAX_EXT_CMD 200 /* Change if we ever have more ext cmds */

/*
 * This is currently used only by the tty interface and is
 * controlled via runtime option 'extmenu'.  (Most other interfaces
 * already use a menu all the time for extended commands.)
 *
 * ``# ?'' is counted towards the limit of the number of commands,
 * so we actually support MAX_EXT_CMD-1 "real" extended commands.
 *
 * Here after # - now show pick-list of possible commands.
 */
int
extcmd_via_menu()
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *) 0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD + 1];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret, len, biggest;
    int accelerator, prevaccelerator;
    int matchlevel = 0;
    boolean wastoolong, one_per_line;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
        i = n = 0;
        any = zeroany;
        /* populate choices */
        for (efp = extcmdlist; efp->ef_txt; efp++) {
            if ((efp->flags & CMD_NOT_AVAILABLE)
                || !(efp->flags & AUTOCOMPLETE)
                || (!wizard && (efp->flags & WIZMODECMD)))
                continue;
            if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
                choices[i] = efp;
                if ((len = (int) strlen(efp->ef_desc)) > biggest)
                    biggest = len;
                if (++i > MAX_EXT_CMD) {
#if defined(BETA)
                    impossible(
      "Exceeded %d extended commands in doextcmd() menu; 'extmenu' disabled.",
                               MAX_EXT_CMD);
#endif /* BETA */
                    iflags.extmenu = 0;
                    return -1;
                }
            }
        }
        choices[i] = (struct ext_func_tab *) 0;
        nchoices = i;
        /* if we're down to one, we have our selection so get out of here */
        if (nchoices  <= 1) {
            ret = (nchoices == 1) ? (int) (choices[0] - extcmdlist) : -1;
            break;
        }

        /* otherwise... */
        win = create_nhwindow(NHW_MENU);
        start_menu(win);
        Sprintf(fmtstr, "%%-%ds", biggest + 15);
        prompt[0] = '\0';
        wastoolong = FALSE; /* True => had to wrap due to line width
                             * ('w' in wizard mode) */
        /* -3: two line menu header, 1 line menu footer (for prompt) */
        one_per_line = (nchoices < ROWNO - 3);
        accelerator = prevaccelerator = 0;
        acount = 0;
        for (i = 0; choices[i]; ++i) {
            accelerator = choices[i]->ef_txt[matchlevel];
            if (accelerator != prevaccelerator || one_per_line)
                wastoolong = FALSE;
            if (accelerator != prevaccelerator || one_per_line
                || (acount >= 2
                    /* +4: + sizeof " or " - sizeof "" */
                    && (strlen(prompt) + 4 + strlen(choices[i]->ef_txt)
                        /* -6: enough room for 1 space left margin
                         *   + "%c - " menu selector + 1 space right margin */
                        >= min(sizeof prompt, COLNO - 6)))) {
                if (acount) {
                    /* flush extended cmds for that letter already in buf */
                    Sprintf(buf, fmtstr, prompt);
                    any.a_char = prevaccelerator;
                    add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, NO_COLOR,
                             buf, FALSE);
                    acount = 0;
                    if (!(accelerator != prevaccelerator || one_per_line))
                        wastoolong = TRUE;
                }
            }
            prevaccelerator = accelerator;
            if (!acount || one_per_line) {
                Sprintf(prompt, "%s%s [%s]", wastoolong ? "or " : "",
                        choices[i]->ef_txt, choices[i]->ef_desc);
            } else if (acount == 1) {
                Sprintf(prompt, "%s%s or %s", wastoolong ? "or " : "",
                        choices[i - 1]->ef_txt, choices[i]->ef_txt);
            } else {
                Strcat(prompt, " or ");
                Strcat(prompt, choices[i]->ef_txt);
            }
            ++acount;
        }
        if (acount) {
            /* flush buf */
            Sprintf(buf, fmtstr, prompt);
            any.a_char = prevaccelerator;
            add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, NO_COLOR, buf,
                     FALSE);
        }
        Sprintf(prompt, "Extended Command: %s", cbuf);
        end_menu(win, prompt);
        n = select_menu(win, PICK_ONE, &pick_list);
        destroy_nhwindow(win);
        if (n == 1) {
            if (matchlevel > (QBUFSZ - 2)) {
                free((genericptr_t) pick_list);
#if defined(BETA)
                impossible("Too many chars (%d) entered in extcmd_via_menu()",
                           matchlevel);
#endif
                ret = -1;
            } else {
                cbuf[matchlevel++] = pick_list[0].item.a_char;
                cbuf[matchlevel] = '\0';
                free((genericptr_t) pick_list);
            }
        } else {
            if (matchlevel) {
                ret = 0;
                matchlevel = 0;
            } else
                ret = -1;
        }
    }
    return ret;
}
#endif /* TTY_GRAPHICS */


#define MAXABILITYNUM 128
#define MAXNAMELENGTH 80

struct available_ability
{
    char name[MAXNAMELENGTH];
    int (*function_ptr)();
    int (*function_mtmp_ptr)(struct monst*);
    struct monst* target_mtmp;
};

STATIC_VAR struct available_ability available_ability_list[MAXABILITYNUM] = { {"", 0, 0, 0} };

int
doability(VOID_ARGS)
{
    int i = '\0';

    menu_item* pick_list = (menu_item*)0;
    winid win;
    anything any;
    int abilitynum = 0;
    int glyph = 0;
    glyph = player_to_glyph_index(urole.rolenum, urace.racenum, Upolyd ? u.mfemale : flags.female, u.ualign.type, 0) + GLYPH_PLAYER_OFF;
    int gui_glyph = maybe_get_replaced_glyph(glyph, u.ux, u.uy, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, &youmonst, 0UL, 0UL, MAT_NONE, 0));

    any = zeroany;
    win = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_CHARACTER_MENU_SCREEN, gui_glyph, extended_create_window_info_from_mon(&youmonst));
    start_menu_ex(win, GHMENU_STYLE_CHARACTER);

    /* CHARACTER ABILITY INFORMATION */
    any = zeroany;
    add_extended_menu(win, NO_GLYPH, &any,
        0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
        "Character Abilities                   ", MENU_UNSELECTED, menu_heading_info());

    strcpy(available_ability_list[abilitynum].name, "Statistics");
    available_ability_list[abilitynum].function_ptr = &docharacterstatistics;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;


    strcpy(available_ability_list[abilitynum].name, "Attributes");
    available_ability_list[abilitynum].function_ptr = &doattributes;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    strcpy(available_ability_list[abilitynum].name, "Skills");
    available_ability_list[abilitynum].function_ptr = &doskill;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    if (Upolyd)
    {
        strcpy(available_ability_list[abilitynum].name, "Polymorphed form");
        available_ability_list[abilitynum].function_ptr = &dopolyformstatistics;
        available_ability_list[abilitynum].target_mtmp = 0;

        any = zeroany;
        any.a_int = abilitynum + 1;
        glyph = flags.female ? female_monnum_to_glyph(u.umonnum) : monnum_to_glyph(u.umonnum);
        gui_glyph = maybe_get_replaced_glyph(glyph, u.ux, u.uy, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, &youmonst, 0UL, 0UL, MAT_NONE, 0));

        add_menu(win, gui_glyph, &any,
            0, 0, ATR_NONE, NO_COLOR,
            available_ability_list[abilitynum].name, MENU_UNSELECTED);

        abilitynum++;

        /* Monster abilities */
        //print_monster_abilities(win, &abilitynum, TRUE);

        strcpy(available_ability_list[abilitynum].name, "Monster abilities");
        available_ability_list[abilitynum].function_ptr = &domonsterability;
        available_ability_list[abilitynum].target_mtmp = 0;

        any = zeroany;
        any.a_int = abilitynum + 1;

        add_menu(win, NO_GLYPH, &any,
            0, 0, ATR_NONE, NO_COLOR,
            available_ability_list[abilitynum].name, MENU_UNSELECTED);

        abilitynum++;
    }

#ifndef GNH_MOBILE
    /* SKILL-BASED ABILITIES */
    any = zeroany;
    add_extended_menu(win, NO_GLYPH, &any,
        0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
        "Use Skill-Based Abilities             ", MENU_UNSELECTED, menu_heading_info());

    /* Ride */
    strcpy(available_ability_list[abilitynum].name, "Ride");
    available_ability_list[abilitynum].function_ptr = &doride;
    available_ability_list[abilitynum].target_mtmp = 0;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    /* Untrap */
    strcpy(available_ability_list[abilitynum].name, "Untrap");
    available_ability_list[abilitynum].function_ptr = &dountrap;
    available_ability_list[abilitynum].target_mtmp = 0;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

#endif

    /* Game Status */
    any = zeroany;
    add_extended_menu(win, NO_GLYPH, &any,
        0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
        "Game Status                           ", MENU_UNSELECTED, menu_heading_info());

    /*
    strcpy(available_ability_list[abilitynum].name, "Quests");
    available_ability_list[abilitynum].function_ptr = &doquests;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;
    */

    strcpy(available_ability_list[abilitynum].name, "Goals and achievements");
    available_ability_list[abilitynum].function_ptr = &doconduct;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    strcpy(available_ability_list[abilitynum].name, "Dungeon overview");
    available_ability_list[abilitynum].function_ptr = &dooverview;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    strcpy(available_ability_list[abilitynum].name, "Killed monsters");
    available_ability_list[abilitynum].function_ptr = &dokilledmonsters;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    strcpy(available_ability_list[abilitynum].name, "Genocided monsters");
    available_ability_list[abilitynum].function_ptr = &dogenocidedmonsters;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;

    strcpy(available_ability_list[abilitynum].name, "Discovered items");
    available_ability_list[abilitynum].function_ptr = &dodiscovered;

    any = zeroany;
    any.a_int = abilitynum + 1;

    add_menu(win, NO_GLYPH, &any,
        0, 0, ATR_NONE, NO_COLOR,
        available_ability_list[abilitynum].name, MENU_UNSELECTED);

    abilitynum++;



    /* Your pets' statistics */
    int petcount = 0;
    struct monst* mtmp;
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    {
        if (!DEADMONSTER(mtmp) && is_tame(mtmp))
        {
            petcount++;
        }
    }

    if (petcount > 0)
    {
        any = zeroany;
        add_extended_menu(win, NO_GLYPH, &any,
            0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
            "Companion Statistics                ", MENU_UNSELECTED, menu_heading_info());

        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
        {
            if (!DEADMONSTER(mtmp) && is_tame(mtmp))
            {
                char namebuf[MAXNAMELENGTH];
                if (UMNAME(mtmp))
                {
                    char umnbuf[BUFSIZ];
                    Strcpy(umnbuf, UMNAME(mtmp));
                    umnbuf[16] = '\0'; /* Limit the length of the name */
                    Strcpy(namebuf, umnbuf);
                }
                else if (MNAME(mtmp) && mtmp->u_know_mname)
                {
                    char mnbuf[BUFSIZ];
                    Strcpy(mnbuf, MNAME(mtmp));
                    mnbuf[16] = '\0'; /* Limit the length of the name */
                    Strcpy(namebuf, mnbuf);
                }
                else
                {
                    char buf[BUFSZ];
                    Strcpy(buf, mon_monster_name(mtmp));
                    *buf = highc(*buf);
                    strncpy(namebuf, buf, MAXNAMELENGTH - 1);
                    namebuf[MAXNAMELENGTH - 1] = '\0';
                }

                Sprintf(available_ability_list[abilitynum].name, "%s", namebuf);
                available_ability_list[abilitynum].function_mtmp_ptr = &doviewpetstatistics;
                available_ability_list[abilitynum].target_mtmp = mtmp;

                any = zeroany;
                any.a_int = abilitynum + 1;

                glyph = abs(any_mon_to_glyph(mtmp, rn2_on_display_rng));
                gui_glyph = maybe_get_replaced_glyph(glyph, mtmp->mx, mtmp->my, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, mtmp, 0UL, 0UL, MAT_NONE, 0));

                add_menu(win, gui_glyph, &any,
                    0, 0, ATR_NONE, NO_COLOR,
                    available_ability_list[abilitynum].name, MENU_UNSELECTED);

                abilitynum++;

                if (abilitynum >= MAXABILITYNUM)
                    break;
            }
        }
    }

    end_menu(win, "Character and Game Status");


    if (abilitynum <= 0)
    {
        You("don't have any special abilities.");
        destroy_nhwindow(win);
        return 0;
    }

    if (select_menu(win, PICK_ONE, &pick_list) > 0) 
    {
        i = pick_list->item.a_int;
        free((genericptr_t)pick_list);
    }
    destroy_nhwindow(win);

    if (i == '\0' || i > abilitynum || i < 1)
        return 0;

    int res = 0;
    int j = i - 1;
    if (available_ability_list[j].function_mtmp_ptr && available_ability_list[j].target_mtmp)
        res = (available_ability_list[j].function_mtmp_ptr)(available_ability_list[j].target_mtmp);
    else if (available_ability_list[j].function_ptr)
        res = (available_ability_list[j].function_ptr)();

    return res;
}

/* #monster command - use special monster ability while polymorphed */
int
domonsterability(VOID_ARGS)
{
    int i = '\0';

    menu_item* pick_list = (menu_item*)0;
    winid win;
    int abilitynum = 0;
    int glyph, gui_glyph;
    glyph = abs(u_to_glyph());
    gui_glyph = maybe_get_replaced_glyph(glyph, u.ux, u.uy, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, &youmonst, 0UL, 0UL, MAT_NONE, 0));

    struct extended_create_window_info createinfo = extended_create_window_info_from_mon(&youmonst);
    win = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_MONSTER_COMMAND_MENU, gui_glyph, createinfo);
    start_menu_ex(win, GHMENU_STYLE_MONSTER_ABILITY);
    print_monster_abilities(win, &abilitynum, FALSE);
    end_menu(win, "Monster Abilities");

    if (abilitynum <= 0)
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        You_ex(ATR_NONE, CLR_MSG_FAIL, "don't have any monster abilities at your disposal.");
        destroy_nhwindow(win);
        return 0;
    }

    if (select_menu(win, PICK_ONE, &pick_list) > 0)
    {
        i = pick_list->item.a_int;
        free((genericptr_t)pick_list);
    }
    destroy_nhwindow(win);

    if (i == '\0' || i > abilitynum || i < 1)
        return 0;

    int res = 0;
    int j = i - 1;
    if (available_ability_list[j].function_mtmp_ptr && available_ability_list[j].target_mtmp)
        res = (available_ability_list[j].function_mtmp_ptr)(available_ability_list[j].target_mtmp);
    else if (available_ability_list[j].function_ptr)
        res = (available_ability_list[j].function_ptr)();

    return res;
}

STATIC_OVL
void
print_monster_abilities(win, abilitiesnum_ptr, ischaractermenu)
winid win;
int* abilitiesnum_ptr;
boolean ischaractermenu;
{
    if (!abilitiesnum_ptr)
        return;

    anything any;
    int abilitynum = *abilitiesnum_ptr;
    int glyph = 0, gui_glyph = 0;
    const char* fmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ?
        "%s (&mana; %d)" : "%s (%d mana)";
    struct extended_menu_info menuinfo = zeroextendedmenuinfo;
    menuinfo.menu_flags |= MENU_FLAGS_USE_SPECIAL_SYMBOLS;

    /* Monster abilities */
    if (can_breathe(youmonst.data)
        || attacktype(youmonst.data, AT_SPIT)
        || youmonst.data->mlet == S_NYMPH
        || attacktype(youmonst.data, AT_GAZE)
        || is_were(youmonst.data)
        || webmaker(youmonst.data)
        || is_hider(youmonst.data)
        || is_tentacled_one(youmonst.data)
        || u.umonnum == PM_GREMLIN
        || is_unicorn(youmonst.data)
        || youmonst.data->msound == MS_SHRIEK
        || youmonst.data->mlet == S_VAMPIRE
        || lays_eggs(youmonst.data)
        )
    {
        if(ischaractermenu)
        {
            any = zeroany;
            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
                "Use Monster Abilities                 ", MENU_UNSELECTED, menu_heading_info());
        }
        else
        {
            any = zeroany;
            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, iflags.menu_headings | ATR_HEADING, NO_COLOR,
                "Your Abilities", MENU_UNSELECTED, menu_heading_info());
        }

        if (can_breathe(youmonst.data))
        {
            Sprintf(available_ability_list[abilitynum].name, fmt, "Use breath weapon", BREATH_WEAPON_MANA_COST);
            available_ability_list[abilitynum].function_ptr = &dobreathe;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (attacktype(youmonst.data, AT_SPIT))
        {
            strcpy(available_ability_list[abilitynum].name, "Spit");
            available_ability_list[abilitynum].function_ptr = &dospit;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (youmonst.data->mlet == S_NYMPH)
        {
            strcpy(available_ability_list[abilitynum].name, "Remove chained items");
            available_ability_list[abilitynum].function_ptr = &doremove;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (attacktype(youmonst.data, AT_GAZE))
        {
            int i, adtyp = 0;
            for (i = 0; i < NATTK; i++) {
                if (youmonst.data->mattk[i].aatyp == AT_GAZE) {
                    adtyp = youmonst.data->mattk[i].adtyp;
                    break;
                }
            }
            int gazemanacost = (adtyp == AD_CNCL ? CNCL_GAZE_MANA_COST : EYE_STALK_MANA_COST);
            const char* gazefmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ?
                "Gaze%s (&mana; %d)" : "Gaze%s (%d mana)";

            char gazebuf[BUFSIZ];
            Sprintf(gazebuf, gazefmt, youmonst.data->mlet == S_EYE ? " with central eye" : "", gazemanacost);
            strcpy(available_ability_list[abilitynum].name, gazebuf);
            available_ability_list[abilitynum].function_ptr = &dogaze;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (attacktype(youmonst.data, AT_EYES))
        {
            Sprintf(available_ability_list[abilitynum].name, fmt, "Gaze with one or more eyestalks", EYE_STALK_MANA_COST);
            available_ability_list[abilitynum].function_ptr = &doeyestalk;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (is_were(youmonst.data))
        {
            Sprintf(available_ability_list[abilitynum].name, fmt, "Summon monsters", WERE_SUMMON_MANA_COST);
            available_ability_list[abilitynum].function_ptr = &dosummon;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (webmaker(youmonst.data))
        {
            strcpy(available_ability_list[abilitynum].name, "Spin web");
            available_ability_list[abilitynum].function_ptr = &dospinweb;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (is_hider(youmonst.data))
        {
            strcpy(available_ability_list[abilitynum].name, "Hide");
            available_ability_list[abilitynum].function_ptr = &dohide;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (is_tentacled_one(youmonst.data))
        {
            Sprintf(available_ability_list[abilitynum].name, fmt, "Project a mind blast", MIND_BLAST_MANA_COST);
            available_ability_list[abilitynum].function_ptr = &domindblast;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (u.umonnum == PM_GREMLIN)
        {
            strcpy(available_ability_list[abilitynum].name, "Dry a fountain");
            available_ability_list[abilitynum].function_ptr = &dodryfountain;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (is_unicorn(youmonst.data))
        {
            Sprintf(available_ability_list[abilitynum].name, fmt, "Use your horn", UNICORN_HORN_MANA_COST);
            available_ability_list[abilitynum].function_ptr = &douseunicornhorn;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (youmonst.data->msound == MS_SHRIEK)
        {
            strcpy(available_ability_list[abilitynum].name, "Shriek");
            available_ability_list[abilitynum].function_ptr = &doshriek;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (youmonst.data->mlet == S_VAMPIRE)
        {
            strcpy(available_ability_list[abilitynum].name, "Assume another form");
            available_ability_list[abilitynum].function_ptr = &dopoly;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }

        if (lays_eggs(youmonst.data) && flags.female)
        {
            const char* layfmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ?
                "%s an egg (&food; %d)" : "%s an egg (%d nutrition)";

            Sprintf(available_ability_list[abilitynum].name, layfmt, eggs_in_water(youmonst.data) ? "Spawn" : "Lay", (int)objects[EGG].oc_nutrition);
            available_ability_list[abilitynum].function_ptr = &dolayegg;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;
            any.a_int = abilitynum + 1;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, NO_COLOR,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, menuinfo);

            abilitynum++;
        }
    }


    /* Your steed's abilities */
    if (u.usteed &&
        can_breathe(u.usteed->data)
        )
    {
        any = zeroany;
        glyph = abs(any_mon_to_glyph(u.usteed, rn2_on_display_rng));
        gui_glyph = maybe_get_replaced_glyph(glyph, u.ux, u.uy, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, u.usteed, 0UL, 0UL, MAT_NONE, 0));
        add_extended_menu(win, gui_glyph, &any,
            0, 0, iflags.menu_headings, NO_COLOR,
            ischaractermenu ? "Use Your Steed's Abilities            " : "Your Steed's Abilities", MENU_UNSELECTED, menu_heading_info());

        if (can_breathe(u.usteed->data))
        {
            struct extended_menu_info steedmenuinfo = zeroextendedmenuinfo;
            int mcolor = NO_COLOR;
            steedmenuinfo.menu_flags |= MENU_FLAGS_USE_SPECIAL_SYMBOLS;
            any = zeroany;
            if (u.usteed->mspec_used > 0)
            {
                Strcpy(available_ability_list[abilitynum].name, "(Breath weapon cooling down)");
                mcolor = CLR_GRAY;
            }
            else
            {
                const char* steedbreathefmt = ((windowprocs.wincap2 & WC2_SPECIAL_SYMBOLS) != 0) ?
                    "%s (&cool; %s)" : "%s (%s round cooldown)";
                Sprintf(available_ability_list[abilitynum].name, steedbreathefmt, "Command steed to use breath weapon", "1d10+4");
                any.a_int = abilitynum + 1;
                mcolor = NO_COLOR;
            }

            available_ability_list[abilitynum].function_ptr = &dosteedbreathe;
            available_ability_list[abilitynum].target_mtmp = 0;

            any = zeroany;

            add_extended_menu(win, NO_GLYPH, &any,
                0, 0, ATR_NONE, mcolor,
                available_ability_list[abilitynum].name, MENU_UNSELECTED, steedmenuinfo);

            abilitynum++;
        }
    }

    *abilitiesnum_ptr = abilitynum;
}


#undef MAXNAMELENGTH
#undef MAXABILITYNUM

/* #monster command - use special monster ability while polymorphed -- OBSOLETE --JG, use domonsterability instead */
int
domonability(VOID_ARGS)
{
    if (can_breathe(youmonst.data))
        return dobreathe();
    else if (attacktype(youmonst.data, AT_SPIT))
        return dospit();
    else if (youmonst.data->mlet == S_NYMPH)
        return doremove();
    else if (attacktype(youmonst.data, AT_GAZE))
        return dogaze();
    else if (is_were(youmonst.data))
        return dosummon();
    else if (webmaker(youmonst.data))
        return dospinweb();
    else if (is_hider(youmonst.data))
        return dohide();
    else if (is_tentacled_one(youmonst.data))
        return domindblast();
    else if (u.umonnum == PM_GREMLIN) {
        if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
            if (split_mon(&youmonst, (struct monst *) 0))
                dryup(u.ux, u.uy, TRUE);
        } else
            There("is no fountain here.");
    } else if (is_unicorn(youmonst.data)) {
        return douseunicornhorn();
    } else if (youmonst.data->msound == MS_SHRIEK) {
        You("shriek.");
        if (u.uburied)
            pline("Unfortunately sound does not carry well through rock.");
        else
            aggravate();
    } else if (youmonst.data->mlet == S_VAMPIRE)
        return dopoly();
    else if (Upolyd)
        pline("Any special ability you may have is purely reflexive.");
    else
        You("don't have a special ability in your normal form!");
    return 0;
}

int
enter_explore_mode(VOID_ARGS)
{
    if (wizard)
    {
        You("are in debug mode.");
    }
    else if (discover) 
    {
        You("are already in explore mode.");
    } 
    else 
    {
#if defined(GNH_MOBILE)
        You("cannot access explore mode.");
#else
#ifdef SYSCF
#if defined(UNIX)
        if (!sysopt.explorers || !sysopt.explorers[0]
            || !check_user_string(sysopt.explorers)) 
        {
            if (!wizard) 
            {
                You("cannot access explore mode.");
                return 0;
            }
            else 
            {
                pline1("Note: normally you wouldn't be allowed into explore mode.");
                /* keep going */
            }
        }
#endif
#endif
        pline_ex(ATR_NONE, CLR_MSG_WARNING,
        "Beware!  From explore mode there will be no return to normal game.");
        if (paranoid_query_ex(ATR_NONE, CLR_MSG_WARNING, ParanoidQuit, "Confirm Explore Mode",
                           "Do you want to enter explore mode?")) 
        {
            clear_nhwindow(WIN_MESSAGE);
            discover = TRUE;
            CasualMode = FALSE, ModernMode = FALSE;
            You_ex(ATR_NONE, CLR_MSG_HINT, "are now in %s mode.", get_game_mode_text(TRUE));
            context.botl = context.botlx = 1;
        } 
        else 
        {
            clear_nhwindow(WIN_MESSAGE);
            pline("Resuming normal game.");
        }
#endif
    }
    return 0;
}

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish(VOID_ARGS) /* Unlimited wishes for debug mode by Paul Polderman */
{
    if (wizard) {
        boolean save_verbose = flags.verbose;

        flags.verbose = FALSE;
        makewish(TRUE, FALSE);
        flags.verbose = save_verbose;
        (void) encumber_msg();
    } else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_wish)));
    return 0;
}

/* ^I command - reveal and optionally identify hero's inventory */
STATIC_PTR int
wiz_identify(VOID_ARGS)
{
    if (wizard) {
        iflags.override_ID = (int) cmd_from_func(wiz_identify);
        /* command remapping might leave #wizidentify as the only way
           to invoke us, in which case cmd_from_func() will yield NUL;
           it won't matter to display_inventory()/display_pickinv()
           if ^I invokes some other command--what matters is that
           display_pickinv() and xname() see override_ID as nonzero */
        if (!iflags.override_ID)
            iflags.override_ID = C('I');
        (void) display_inventory((char *) 0, FALSE, 0);
        iflags.override_ID = 0;
    } else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_identify)));
    return 0;
}

/* #wizmakemap - discard current dungeon level and replace with a new one */
STATIC_PTR int
wiz_makemap(VOID_ARGS)
{
    if (wizard) {
        struct monst *mtmp;
        boolean was_in_W_tower = In_W_tower(u.ux, u.uy, &u.uz);

        rm_mapseen(ledger_no(&u.uz));
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
            if (mtmp->isgd) { /* vault is going away; get rid of guard */
                mtmp->isgd = 0;
                mongone(mtmp);
            }
            if (DEADMONSTER(mtmp))
                continue;
            if (mtmp->isshk)
                setpaid(mtmp);
            /* TODO?
             *  Reduce 'born' tally for each monster about to be discarded
             *  by savelev(), otherwise replacing heavily populated levels
             *  tends to make their inhabitants become extinct.
             */
        }
        if (Punished) {
            ballrelease(FALSE);
            unplacebc();
        }
        /* reset lock picking unless it's for a carried container */
        maybe_reset_pick((struct obj *) 0);
        /* reset interrupted digging if it was taking place on this level */
        if (on_level(&context.digging.level, &u.uz))
            (void) memset((genericptr_t) &context.digging, 0,
                          sizeof (struct dig_info));
        /* reset cached targets */
        iflags.travelcc.x = iflags.travelcc.y = 0; /* travel destination */
        context.polearm.hitmon = (struct monst *) 0; /* polearm target */
        /* escape from trap */
        reset_utrap(FALSE);
        check_special_room(TRUE); /* room exit */
        u.ustuck = (struct monst *) 0;
        u.uswallow = 0;
        u.uinwater = 0;
        u.uundetected = 0; /* not hidden, even if means are available */
        dmonsfree(); /* purge dead monsters from 'fmon' */
        /* keep steed and other adjacent pets after releasing them
           from traps, stopping eating, &c as if hero were ascending */
        keepdogs(TRUE); /* (pets-only; normally we'd be using 'FALSE' here) */

        /* discard current level; "saving" is used to release dynamic data */
        savelev(-1, ledger_no(&u.uz), FREE_SAVE);
        /* create a new level; various things like bestowing a guardian
           angel on Astral or setting off alarm on Ft.Ludios are handled
           by goto_level(do.c) so won't occur for replacement levels */
        mklev();

        vision_reset();
        vision_full_recalc = 1;
        cls();
        /* was using safe_teleds() but that doesn't honor arrival region
           on levels which have such; we don't force stairs, just area */
        u_on_rndspot((u.uhave.amulet ? 1 : 0) /* 'going up' flag */
                     | (was_in_W_tower ? 2 : 0));
        losedogs();
        /* u_on_rndspot() might pick a spot that has a monster, or losedogs()
           might pick the hero's spot (only if there isn't already a monster
           there), so we might have to move hero or the co-located monster */
        if ((mtmp = m_at(u.ux, u.uy)) != 0)
            u_collide_m(mtmp);
        initrack();
        if (Punished) {
            unplacebc();
            placebc();
        }
        docrt();
        flush_screen(1);
        deliver_splev_message(); /* level entry */
        check_special_room(FALSE); /* room entry */
#ifdef INSURANCE
        save_currentstate();
#endif
    } else {
        pline(unavailcmd, "#wizmakemap");
    }
    return 0;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map(VOID_ARGS)
{
    if (wizard) {
        struct trap *t;
        long save_Hconf = HConfusion, save_Hhallu = HHallucination;

        HConfusion = HHallucination = 0L;
        for (t = ftrap; t != 0; t = t->ntrap) {
            t->tseen = 1;
            map_trap(t, TRUE);
        }
        do_mapping();
        HConfusion = save_Hconf;
        HHallucination = save_Hhallu;
    } else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_map)));
    return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis(VOID_ARGS)
{
    if (wizard)
        (void) create_particular();
    else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_genesis)));
    return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where(VOID_ARGS)
{
    if (wizard)
        (void) print_dungeon(FALSE, (schar *) 0, (xchar *) 0);
    else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_where)));
    return 0;
}

STATIC_PTR int
wiz_files(VOID_ARGS)
{
    if (wizard)
        list_files();
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_files)));
    return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect(VOID_ARGS)
{
    if (wizard)
        (void) findit();
    else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_detect)));
    return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele(VOID_ARGS)
{
    if (wizard)
        level_tele(1, TRUE, zerodlevel);
    else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_level_tele)));
    return 0;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change(VOID_ARGS)
{
    char buf[BUFSZ] = DUMMY;
    int newlevel = 0;
    int ret;

    getlin_ex(GETLINE_LEVEL_CHANGE, ATR_NONE, NO_COLOR, "To what experience level do you want to be set?", buf, (char*)0, (char*)0, (char*)0);
    (void) mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0')
        ret = 0;
    else
        ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
        pline1(Never_mind);
        return 0;
    }
    if (newlevel == u.ulevel) {
        You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
        if (u.ulevel == 1) {
            You("are already as inexperienced as you can get.");
            return 0;
        }
        if (newlevel < 1)
            newlevel = 1;
        while (u.ulevel > newlevel)
            losexp("#levelchange");
    } else {
        if (u.ulevel >= MAXULEV) {
            You("are already as experienced as you can get.");
            return 0;
        }
        if (newlevel > MAXULEV)
            newlevel = MAXULEV;
        while (u.ulevel < newlevel)
            pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
#if defined(DEBUG)
STATIC_PTR int
wiz_panic(VOID_ARGS)
{
    if (iflags.debug_fuzzer) {
        u.uhp = u.uhpmax = 1000;
        u.uen = u.uenmax = 1000;
        return 0;
    }
    if (yn_query("Do you want to call panic() and end your game?") == 'y')
        panic("Crash test.");
    return 0;
}
#endif

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself(VOID_ARGS)
{
    polyself(1);
    return 0;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv(VOID_ARGS)
{
    winid win;
    int x, y, v, startx, stopx, curx;
    char row[COLNO + 1];

    win = create_nhwindow(NHW_TEXT);
    /*
     * Each seenv description takes up 2 characters, so center
     * the seenv display around the hero.
     */
    startx = max(1, u.ux - (COLNO / 4));
    stopx = min(startx + (COLNO / 2), COLNO);
    /* can't have a line exactly 80 chars long */
    if (stopx - startx == COLNO / 2)
        startx++;

    for (y = 0; y < ROWNO; y++) {
        for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
            if (x == u.ux && y == u.uy) {
                row[curx] = row[curx + 1] = '@';
            } else {
                v = levl[x][y].seenv & 0xff;
                if (v == 0)
                    row[curx] = row[curx + 1] = ' ';
                else
                    Sprintf(&row[curx], "%02x", v);
            }
        }
        /* remove trailing spaces */
        for (x = curx - 1; x >= 0; x--)
            if (row[x] != ' ')
                break;
        row[x + 1] = '\0';

        putstr(win, 0, row);
    }
    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return 0;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision(VOID_ARGS)
{
    winid win;
    int x, y, v;
    char row[COLNO + 1];

    win = create_nhwindow(NHW_TEXT);
    Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
            COULD_SEE, IN_SIGHT, TEMP_LIT);
    putstr(win, 0, row);
    putstr(win, 0, "");
    for (y = 0; y < ROWNO; y++) {
        for (x = 1; x < COLNO; x++) {
            if (x == u.ux && y == u.uy)
                row[x] = '@';
            else {
                v = viz_array[y][x]; /* data access should be hidden */
                if (v == 0)
                    row[x] = ' ';
                else
                    row[x] = '0' + viz_array[y][x];
            }
        }
        /* remove trailing spaces */
        for (x = COLNO - 1; x >= 1; x--)
            if (row[x] != ' ')
                break;
        row[x + 1] = '\0';

        putstr(win, 0, &row[1]);
    }
    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return 0;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes(VOID_ARGS)
{
    winid win;
    int x, y;
    char row[COLNO + 1];
    struct rm *lev;
    boolean istty = WINDOWPORT("tty");

    win = create_nhwindow(NHW_TEXT);
    if (istty)
        putstr(win, 0, ""); /* tty only: blank top line */
    for (y = 0; y < ROWNO; y++) {
        for (x = 0; x < COLNO; x++) {
            lev = &levl[x][y];
            if (x == u.ux && y == u.uy)
                row[x] = '@';
            else if (IS_WALL_OR_SDOOR(lev->typ))
                row[x] = '0' + (lev->wall_info & WM_MASK);
            else if (lev->typ == CORR)
                row[x] = '#';
            else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
                row[x] = '.';
            else
                row[x] = 'x';
        }
        row[COLNO] = '\0';
        /* map column 0, levl[0][], is off the left edge of the screen */
        putstr(win, 0, &row[1]);
    }
    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return 0;
}

/* wizard mode variant of #terrain; internal levl[][].typ values in base-36 */
STATIC_OVL void
wiz_map_levltyp(VOID_ARGS)
{
    winid win;
    int x, y, terrain;
    char row[COLNO + 1];
    boolean istty = !strcmp(windowprocs.name, "tty");

    win = create_nhwindow(NHW_TEXT);
    /* map row 0, levl[][0], is drawn on the second line of tty screen */
    if (istty)
        putstr(win, 0, ""); /* tty only: blank top line */
    for (y = 0; y < ROWNO; y++) {
        /* map column 0, levl[0][], is off the left edge of the screen;
           it should always have terrain type "undiggable stone" */
        for (x = 1; x < COLNO; x++) {
            terrain = levl[x][y].typ;
            /* assumes there aren't more than 10+26+26 terrain types */
            row[x - 1] = (char) ((terrain == STONE && !may_dig(x, y))
                                    ? '*'
                                    : (terrain < 10)
                                       ? '0' + terrain
                                       : (terrain < 36)
                                          ? 'a' + terrain - 10
                                          : 'A' + terrain - 36);
        }
        x--;
        if (levl[0][y].typ != STONE || may_dig(0, y))
            row[x++] = '!';
        row[x] = '\0';
        putstr(win, 0, row);
    }

    {
        char dsc[BUFSZ];
        s_level *slev = Is_special(&u.uz);

        Sprintf(dsc, "D:%d,L:%d", u.uz.dnum, u.uz.dlevel);
        /* [dungeon branch features currently omitted] */
        /* special level features */
        if (slev) {
            Sprintf(eos(dsc), " \"%s\"", slev->name);
            /* special level flags (note: dungeon.def doesn't set `maze'
               or `hell' for any specific levels so those never show up) */
            if (slev->flags.maze_like)
                Strcat(dsc, " mazelike");
            if (slev->flags.hellish)
                Strcat(dsc, " hellish");
            if (slev->flags.town)
                Strcat(dsc, " town");
            if (slev->flags.rogue_like)
                Strcat(dsc, " roguelike");
            /* alignment currently omitted to save space */
        }
        /* level features */
        if (level.flags.nfountains)
            Sprintf(eos(dsc), " %c:%d", defsyms[S_fountain].sym,
                    (int) level.flags.nfountains);
        if (level.flags.nsinks)
            Sprintf(eos(dsc), " %c:%d", defsyms[S_sink].sym,
                    (int) level.flags.nsinks);
        if (level.flags.has_vault)
            Strcat(dsc, " vault");
        if (level.flags.has_shop)
            Strcat(dsc, " shop");
        if (level.flags.has_temple)
            Strcat(dsc, " temple");
        if (level.flags.has_smithy)
            Strcat(dsc, " smithy");
        if (level.flags.has_npc_room)
            Strcat(dsc, " residence");
        if (level.flags.has_court)
            Strcat(dsc, " throne");
        if (level.flags.has_zoo)
            Strcat(dsc, " zoo");
        if (level.flags.has_morgue)
            Strcat(dsc, " morgue");
        if (level.flags.has_barracks)
            Strcat(dsc, " barracks");
        if (level.flags.has_armory)
            Strcat(dsc, " armory");
        if (level.flags.has_beehive)
            Strcat(dsc, " hive");
        if (level.flags.has_garden)
            Strcat(dsc, " garden");
        if (level.flags.has_library)
            Strcat(dsc, " library");
        if (level.flags.has_dragonlair)
            Strcat(dsc, " dragonlair");
        if (level.flags.has_swamp)
            Strcat(dsc, " swamp");
        /* level flags */
        if (level.flags.noteleport)
            Strcat(dsc, " noTport");
        if (level.flags.hardfloor)
            Strcat(dsc, " noDig");
        if (level.flags.nommap)
            Strcat(dsc, " noMMap");
        if (!level.flags.hero_memory)
            Strcat(dsc, " noMem");
        if (level.flags.shortsighted)
            Strcat(dsc, " shortsight");
        if (level.flags.graveyard)
            Strcat(dsc, " graveyard");
        if (level.flags.is_maze_lev)
            Strcat(dsc, " maze");
        if (level.flags.is_cavernous_lev)
            Strcat(dsc, " cave");
        if (level.flags.arboreal)
            Strcat(dsc, " tree");
        if (Sokoban)
            Strcat(dsc, " sokoban-rules");
        /* non-flag info; probably should include dungeon branching
           checks (extra stairs and magic portals) here */
        if (Invocation_lev(&u.uz))
            Strcat(dsc, " invoke");
        if (On_W_tower_level(&u.uz))
            Strcat(dsc, " tower");
        /* append a branch identifier for completeness' sake */
        if (u.uz.dnum == main_dungeon_dnum)
            Strcat(dsc, " dungeon");
        else if (u.uz.dnum == mines_dnum)
            Strcat(dsc, " mines");
        else if (In_sokoban(&u.uz))
            Strcat(dsc, " sokoban");
        else if (u.uz.dnum == quest_dnum)
            Strcat(dsc, " quest");
        else if (Is_knox(&u.uz))
            Strcat(dsc, " ludios");
        else if (u.uz.dnum == gehennom_dnum)
            Strcat(dsc, " gehennom");
        else if (u.uz.dnum == tower_dnum)
            Strcat(dsc, " vlad");
        else if (In_endgame(&u.uz))
            Strcat(dsc, " endgame");
        else {
            /* somebody's added a dungeon branch we're not expecting */
            const char *brname = dungeons[u.uz.dnum].dname;

            if (!brname || !*brname)
                brname = "unknown";
            if (!strncmpi(brname, "the ", 4))
                brname += 4;
            Sprintf(eos(dsc), " %s", brname);
        }
        /* limit the line length to map width */
        if (strlen(dsc) >= COLNO)
            dsc[COLNO - 1] = '\0'; /* truncate */
        putstr(win, 0, dsc);
    }

    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return;
}

#ifndef GNH_MOBILE
/* Save monster list */
STATIC_PTR int
wiz_save_monsters(VOID_ARGS) /* Save a csv file for monsters */
{
    if (wizard) 
    {
        int i, j;
        pline("Starting writing monsters.csv...");
        const char* fq_save = "monsters.csv";
        int fd;

        (void)remove(fq_save);

#ifdef MAC
        fd = macopen(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, TEXT_TYPE);
#else
        fd = open(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, FCMASK);
#endif
        char buf[BUFSIZ];

        Sprintf(buf, "Name,Level,Move,AC,MC,MR,Alignment,GenoFlags,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < NATTK; j++)
        {
            Sprintf(buf, "Attack%d,Type,DmgType,DiceNum,DieSize,DmgPlus,MCAdj,", j+1);
            (void)write(fd, buf, strlen(buf));

        }

        Sprintf(buf, ",CorpseWeight,Nutrition,SoundType,Size,STR,DEX,CON,INT,WIS,CHA,");
        (void)write(fd, buf, strlen(buf));

        Sprintf(buf, "MResists,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < 32; j++)
        {
            Sprintf(buf, "%d,", j+1);
            (void)write(fd, buf, strlen(buf));
        }
        Sprintf(buf, "MConveys,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < 32; j++)
        {
            Sprintf(buf, "%d,", j + 1);
            (void)write(fd, buf, strlen(buf));
        }
        Sprintf(buf, "MFlags1,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < 32; j++)
        {
            Sprintf(buf, "%d,", j + 1);
            (void)write(fd, buf, strlen(buf));
        }
        Sprintf(buf, "MFlags2,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < 32; j++)
        {
            Sprintf(buf, "%d,", j + 1);
            (void)write(fd, buf, strlen(buf));
        }
        Sprintf(buf, "MFlags3,");
        (void)write(fd, buf, strlen(buf));
        for (j = 0; j < 32; j++)
        {
            Sprintf(buf, "%d,", j + 1);
            (void)write(fd, buf, strlen(buf));
        }
        Sprintf(buf, ",Difficulty,Color,");
        (void)write(fd, buf, strlen(buf));

        Sprintf(buf, ",STR-ToHit-Bonus,STR-DmgBonus,DEX-ToHit-Bonus,DEX-AC-Bonus,CON-HP-Bonus\n");
        (void)write(fd, buf, strlen(buf));


        for (i = LOW_PM; i < NUM_MONSTERS; i++)
        {
            Sprintf(buf, "%s,%d,%d,%d,%d,%d,%d,%lu,",
                mons[i].mname, 
                (int)mons[i].mlevel, (int)mons[i].mmove, 
                (int)mons[i].ac, (int)mons[i].mc, (int)mons[i].mr, 
                (int)mons[i].maligntyp, mons[i].geno);
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < NATTK; j++)
            {
                Sprintf(buf, ",%d,%d,%d,%d,%d,%d,", 
                    mons[i].mattk[j].aatyp, mons[i].mattk[j].adtyp, 
                    mons[i].mattk[j].damn, mons[i].mattk[j].damd, mons[i].mattk[j].damp, 
                    mons[i].mattk[j].mcadj);
                (void)write(fd, buf, strlen(buf));
            }

            Sprintf(buf, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", 
                mons[i].cwt, mons[i].cnutrit, mons[i].msound, mons[i].msize, 
                mons[i].str, mons[i].dex, mons[i].con, mons[i].intl, mons[i].wis, mons[i].cha);
            (void)write(fd, buf, strlen(buf));

            Sprintf(buf, ",");
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < 32; j++)
            {
                Sprintf(buf, "%d,", (mons[i].mresists & (1L << j)) ? 1 : 0);
                (void)write(fd, buf, strlen(buf));
            }
            Sprintf(buf, ",");
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < 32; j++)
            {
                Sprintf(buf, "%d,", (mons[i].mconveys & (1L << j)) ? 1 : 0);
                (void)write(fd, buf, strlen(buf));
            }
            Sprintf(buf, ",");
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < 32; j++)
            {
                Sprintf(buf, "%d,", (mons[i].mflags1 & (1L << j)) ? 1 : 0);
                (void)write(fd, buf, strlen(buf));
            }
            Sprintf(buf, ",");
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < 32; j++)
            {
                Sprintf(buf, "%d,", (mons[i].mflags2 & (1L << j)) ? 1 : 0);
                (void)write(fd, buf, strlen(buf));
            }
            Sprintf(buf, ",");
            (void)write(fd, buf, strlen(buf));
            for (j = 0; j < 32; j++)
            {
                Sprintf(buf, "%d,", (mons[i].mflags3 & (1L << j)) ? 1 : 0);
                (void)write(fd, buf, strlen(buf));
            }
            Sprintf(buf, ",%d,%d,",
                mons[i].difficulty, mons[i].mcolor);
            (void)write(fd, buf, strlen(buf));
            Sprintf(buf, ",%d,%d,%d,%d,%0.1f\n",
                strength_tohit_bonus(mons[i].str), strength_damage_bonus(mons[i].str), dexterity_tohit_bonus(mons[i].dex), dexterity_ac_bonus(mons[i].dex),constitution_hp_bonus(mons[i].con));
            (void)write(fd, buf, strlen(buf));

        }
        (void)close(fd);

        pline("Done!");
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_save_monsters)));
    return 0;
}


/* Save tile data */
STATIC_PTR int
wiz_save_tiledata(VOID_ARGS) /* Save a csv file for tile data */
{
#ifdef USE_TILES
    if (wizard)
    {
        const char* fq_save = "tile_definition.csv";
        pline("Starting writing %s...", fq_save);
        int cnt = process_tiledata(0, fq_save, (int*)0, (uchar*)0);
        pline("Done writing %s. %d tiles written.", fq_save, cnt);
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_save_tiledata)));
#endif
    return 0;
}

/* Count tiles */
STATIC_PTR int
wiz_count_tiles(VOID_ARGS) /* Save a csv file for tile data */
{
#ifdef USE_TILES
    if (wizard)
    {
        int cnt = process_tiledata(2, (const char*)0, (int*)0, (uchar*)0);
        pline("There are %d tiles.", cnt);
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_count_tiles)));
#endif
    return 0;
}

STATIC_PTR int
wiz_save_glyph2tiles(VOID_ARGS) /* Save a csv file for tile data */
{
#ifdef USE_TILES
    if (wizard)
    {
        const char* fq_save = "glyph2tile_out.csv";
        int fd;
        char buf[BUFSZ];
        pline("Starting writing %s...", fq_save);
        (void)remove(fq_save);

#ifdef MAC
        fd = macopen(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, TEXT_TYPE);
#else
        fd = open(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, FCMASK);
#endif
        int i;
        for (i = 0; i < MAX_GLYPH; i++)
        {
            const char* header = "";
            const char* special = "";
            switch (i)
            {
            case GLYPH_MON_OFF:
                header = "MON_OFF";
                break;
            case GLYPH_BODY_OFF:
                header = "MON_BODY_OFF";
                break;
            case GLYPH_ATTACK_OFF:
                header = "MON_ATTACK_OFF";
                break;
            case GLYPH_THROW_OFF:
                header = "MON_THROW_OFF";
                break;
            case GLYPH_FIRE_OFF:
                header = "MON_FIRE_OFF";
                break;
            case GLYPH_CAST_NODIR_OFF:
                header = "MON_CAST_NODIR_OFF";
                break;
            case GLYPH_CAST_DIR_OFF:
                header = "MON_CAST_DIR_OFF";
                break;
            case GLYPH_SPECIAL_ATTACK_OFF:
                header = "MON_SPECIAL_ATTACK_OFF";
                break;
            case GLYPH_KICK_OFF:
                header = "MON_KICK_OFF";
                break;
            case GLYPH_ITEM_USE_OFF:
                header = "MON_ITEM_USE_OFF";
                break;
            case GLYPH_DOOR_USE_OFF:
                header = "MON_DOOR_USE_OFF";
                break;
            case GLYPH_DEATH_OFF:
                header = "MON_DEATH_OFF";
                break;
            case GLYPH_FEMALE_MON_OFF:
                header = "FEMALE_MON_OFF";
                break;
            case GLYPH_FEMALE_BODY_OFF:
                header = "FEMALE_BODY_MON_OFF";
                break;
            case GLYPH_FEMALE_ATTACK_OFF:
                header = "FEMALE_ATTACK_OFF";
                break;
            case GLYPH_FEMALE_THROW_OFF:
                header = "MON_FEMALE_THROW_OFF";
                break;
            case GLYPH_FEMALE_FIRE_OFF:
                header = "MON_FEMALE_FIRE_OFF";
                break;
            case GLYPH_FEMALE_CAST_NODIR_OFF:
                header = "MON_FEMALE_CAST_NODIR_OFF";
                break;
            case GLYPH_FEMALE_CAST_DIR_OFF:
                header = "MON_FEMALE_CAST_DIR_OFF";
                break;
            case GLYPH_FEMALE_SPECIAL_ATTACK_OFF:
                header = "MON_FEMALE_SPECIAL_ATTACK_OFF";
                break;
            case GLYPH_FEMALE_KICK_OFF:
                header = "MON_FEMALE_KICK_OFF";
                break;
            case GLYPH_FEMALE_ITEM_USE_OFF:
                header = "MON_FEMALE_ITEM_USE_OFF";
                break;
            case GLYPH_FEMALE_DOOR_USE_OFF:
                header = "MON_FEMALE_DOOR_USE_OFF";
                break;
            case GLYPH_FEMALE_DEATH_OFF:
                header = "MON_FEMALE_DEATH_OFF";
                break;
            case GLYPH_INVIS_OFF:
                header = "INVIS_OFF";
                break;
            case GLYPH_OBJ_OFF:
                header = "OBJ_OFF";
                break;
            case GLYPH_OBJ_MISSILE_OFF:
                header = "OBJ_MISSILE_OFF";
                break;
            case GLYPH_CMAP_OFF:
                header = "CMAP_OFF";
                break;
            case GLYPH_BROKEN_CMAP_OFF:
                header = "BROKEN_CMAP_OFF";
                break;
            case GLYPH_CMAP_VARIATION_OFF:
                header = "CMAP_VARIATION_OFF";
                break;
            case GLYPH_BROKEN_CMAP_VARIATION_OFF:
                header = "BROKEN_CMAP_VARIATION_OFF";
                break;
            case GLYPH_EXPLODE_OFF:
                header = "EXPLODE_OFF";
                break;
            case GLYPH_ZAP_OFF:
                header = "ZAP_OFF";
                break;
            case GLYPH_SWALLOW_OFF:
                header = "SWALLOW_OFF";
                break;
            case GLYPH_WARNING_OFF:
                header = "WARNING_OFF";
                break;
            case GLYPH_WORM_OFF:
                header = "EXPLODE_OFF";
                break;
            case GLYPH_ARTIFACT_OFF:
                header = "ARTIFACT_OFF";
                break;
            case GLYPH_ARTIFACT_MISSILE_OFF:
                header = "ARTIFACT_MISSILE_OFF";
                break;
            case GLYPH_STATUE_OFF:
                header = "STATUE_OFF";
                break;
            case GLYPH_FEMALE_STATUE_OFF:
                header = "FEMALE_STATUE_OFF";
                break;
            case GLYPH_PLAYER_OFF:
                header = "PLAYER_OFF";
                break;
            case GLYPH_PLAYER_ATTACK_OFF:
                header = "PLAYER_ATTACK_OFF";
                break;
            case GLYPH_PLAYER_THROW_OFF:
                header = "PLAYER_THROW_OFF";
                break;
            case GLYPH_PLAYER_FIRE_OFF:
                header = "PLAYER_FIRE_OFF";
                break;
            case GLYPH_PLAYER_CAST_NODIR_OFF:
                header = "PLAYER_CAST_NODIR_OFF";
                break;
            case GLYPH_PLAYER_CAST_DIR_OFF:
                header = "PLAYER_CAST_DIR_OFF";
                break;
            case GLYPH_PLAYER_SPECIAL_ATTACK_OFF:
                header = "PLAYER_SPECIAL_ATTACK_OFF";
                break;
            case GLYPH_PLAYER_KICK_OFF:
                header = "PLAYER_KICK_OFF";
                break;
            case GLYPH_PLAYER_ITEM_USE_OFF:
                header = "PLAYER_ITEM_USE_OFF";
                break;
            case GLYPH_PLAYER_DOOR_USE_OFF:
                header = "PLAYER_DOOR_USE_OFF";
                break;
            case GLYPH_PLAYER_DEATH_OFF:
                header = "PLAYER_DEATH_OFF";
                break;
            case GLYPH_SPECIAL_EFFECT_OFF:
                header = "SPECIAL_EFFECT_OFF";
                break;
            case GLYPH_SIMPLE_DOODAD_OFF:
                header = "SIMPLE_DOODAD_OFF";
                break;
            case GLYPH_MIRRORABLE_DOODAD_OFF:
                header = "MIRRORABLE_DOODAD_OFF";
                break;
            case GLYPH_CURSOR_OFF:
                header = "CURSOR_OFF";
                break;
            case GLYPH_HIT_TILE_OFF:
                header = "GENERAL_HIT_TILE_OFF";
                break;
            case GLYPH_GENERAL_TILE_OFF:
                header = "GENERAL_TILE_OFF";
                break;
            case GLYPH_UI_TILE_OFF:
                header = "GENERAL_UI_TILE_OFF";
                break;
            case GLYPH_SPELL_TILE_OFF:
                header = "GENERAL_SPELL_TILE_OFF";
                break;
            case GLYPH_SKILL_TILE_OFF:
                header = "GENERAL_SKILL_TILE_OFF";
                break;
            case GLYPH_BUFF_OFF:
                header = "BUFF_OFF";
                break;
            case GLYPH_REPLACEMENT_OFF:
                header = "REPLACEMENT_OFF";
                break;
            case GLYPH_ANIMATION_OFF:
                header = "ANIMATION_OFF";
                break;
            case GLYPH_ENLARGEMENT_OFF:
                header = "ENLARGEMENT_OFF";
                break;
            }
            if (strcmp(header, ""))
            {
                Sprintf(buf, "%s\n", header);
                (void)write(fd, buf, strlen(buf));
            }

            if (i == abs(u_to_glyph()))
                special = ",YOU";

            Sprintf(buf, "glyph=%d,tile=%d,repl=%d,flags=%d%s\n", i, glyph2tile[i], glyph2replacement[i], glyphtileflags[i], special);
            (void)write(fd, buf, strlen(buf));
        }

        (void)close(fd);
        pline("Done writing %s. %d glyphs written.", fq_save, MAX_GLYPH);
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_save_glyph2tiles)));
#endif
    return 0;
}


/* Save monster list */
STATIC_PTR int
wiz_save_quest_texts(VOID_ARGS) /* Save a csv file for monsters */
{
    if (wizard)
    {
        pline("Starting writing quest_texts.txt...");
        const char* fq_save = "quest_texts.txt";
        int fd;

        (void)remove(fq_save);

#ifdef MAC
        fd = macopen(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, TEXT_TYPE);
#else
        fd = open(fq_save, O_WRONLY | O_TEXT | O_CREAT | O_TRUNC, FCMASK);
#endif
        write_quest_texts(fd);

        (void)close(fd);
        pline("Done writing %s.", fq_save);
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_save_quest_texts)));

    return 0;
}


/* Save spell list */
STATIC_PTR int
wiz_save_spells(VOID_ARGS) /* Save an md file of spells for wiki */
{
    if (wizard)
        export_spells_for_wiki();
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(wiz_save_spells)));

    return 0;
}
#endif /* ifndef GNH_MOBILE */

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
}

/* Mana cost*/
/*                                //TODO: further calculation for mana cost, copied from spell.c. Needed?
STATIC_OVL const double*
get_mana_cost(skill)
int skill;
{
    double manacost = get_spellbook_adjusted_mana_cost(skill);
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
}
*/

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

/* temporary? hack, since level type codes aren't the same as screen
   symbols and only the latter have easily accessible descriptions */
STATIC_VAR const char *levltyp[MAX_LEVTYPE + 2] = {
    "stone", 
    "vertical wall", 
    "horizontal wall", 
    "top-left corner wall",
    "top-right corner wall", 
    "bottom-left corner wall",
    "bottom-right corner wall", 
    "cross wall", "tee-up wall", 
    "tee-down wall",
    "tee-left wall", 
    "tee-right wall", 
    "drawbridge wall", 
    "tree",
    "secret door", 
    "secret corridor", 
    "pool", 
    "moat", 
    "water",
    "drawbridge up", 
    "lava pool", 
    "iron bars", 
    "door", 
    "corridor", 
    "room",
    "stairs", 
    "ladder", 
    "fountain", 
    "throne", 
    "sink", 
    "grave", 
    "brazier",
    "signpost",
    "anvil", 
    "altar", 
    "ice",
    "drawbridge down", 
    "air", 
    "cloud", 
    "grass", 
    "ground", 
    "unexplored",
    /* not a real terrain type, but used for undiggable stone
       by wiz_map_levltyp() */
    "unreachable/undiggable",
    /* padding in case the number of entries above is odd */
    ""
};

/* explanation of base-36 output from wiz_map_levltyp() */
STATIC_OVL void
wiz_levltyp_legend(VOID_ARGS)
{
    winid win;
    int i, j, last, c;
    const char *dsc, *fmt;
    char buf[BUFSZ];

    win = create_nhwindow(NHW_TEXT);
    putstr(win, 0, "#terrain encodings:");
    putstr(win, 0, "");
    fmt = " %c - %-28s"; /* TODO: include tab-separated variant for win32 */
    *buf = '\0';
    /* output in pairs, left hand column holds [0],[1],...,[N/2-1]
       and right hand column holds [N/2],[N/2+1],...,[N-1];
       N ('last') will always be even, and may or may not include
       the empty string entry to pad out the final pair, depending
       upon how many other entries are present in levltyp[] */
    last = SIZE(levltyp) & ~1;
    for (i = 0; i < last / 2; ++i)
        for (j = i; j < last; j += last / 2) {
            dsc = levltyp[j];
            c = !*dsc ? ' '
                   : !strncmp(dsc, "unreachable", 11) ? '*'
                      /* same int-to-char conversion as wiz_map_levltyp() */
                      : (j < 10) ? '0' + j
                         : (j < 36) ? 'a' + j - 10
                            : 'A' + j - 36;
            Sprintf(eos(buf), fmt, c, dsc);
            if (j > i) {
                putstr(win, 0, buf);
                *buf = '\0';
            }
        }
    display_nhwindow(win, TRUE);
    destroy_nhwindow(win);
    return;
}

/* #wizsmell command - test usmellmon(). */
STATIC_PTR int
wiz_smell(VOID_ARGS)
{
    int ans = 0;
    int mndx;  /* monster index */
    coord cc;  /* screen pos of unknown glyph */
    int glyph; /* glyph at selected position */

    cc.x = u.ux;
    cc.y = u.uy;
    mndx = 0; /* gcc -Wall lint */
    if (!olfaction(youmonst.data)) {
        You("are incapable of detecting odors in your present form.");
        return 0;
    }

    pline("You can move the cursor to a monster that you want to smell.");
    do {
        pline("Pick a monster to smell.");
        ans = getpos(&cc, TRUE, "a monster", CURSOR_STYLE_LOOK_CURSOR);
        if (ans < 0 || cc.x < 0) {
            return 0; /* done */
        }
        /* Convert the glyph at the selected position to a mndxbol. */
        glyph = glyph_at(cc.x, cc.y);
        if (glyph_is_monster(glyph))
            mndx = glyph_to_mon(glyph);
        else
            mndx = 0;
        /* Is it a monster? */
        if (mndx) {
            if (!usmellmon(&mons[mndx]))
                pline("That monster seems to give off no smell.");
        } else
            pline("That is not a monster.");
    } while (TRUE);
    return 0;
}

/* #wizinstrinsic command to set some intrinsics for testing */
STATIC_PTR int
wiz_intrinsic(VOID_ARGS)
{
    if (wizard) 
    {
        static const char wizintrinsic[] = "#wizintrinsic";
        static const char fmt[] = "You are%s %s.";
        winid win;
        anything any;
        char buf[BUFSZ];
        int i, j, n, p, amt;
        long oldtimeout, newtimeout;
        const char *propname;
        menu_item *pick_list = (menu_item *) 0;

        any = zeroany;
        win = create_nhwindow(NHW_MENU);
        start_menu(win);
        for (i = 0; (propname = propertynames[i].prop_name) != 0; ++i) 
        {
            p = propertynames[i].prop_num;
            if (p == HALLUC_RES) 
            {
                /* Grayswandir vs hallucination; ought to be redone to
                   use u.uprops[HALLUC].blocked instead of being treated
                   as a separate property; letting in be manually toggled
                   even only in wizard mode would be asking for trouble... */
                continue;
            }
            if (p == FIRE_IMMUNITY) 
            {
                any.a_int = 0;
                add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "--", FALSE);
            }
            any.a_int = i + 1; /* +1: avoid 0 */
            oldtimeout = u.uprops[p].intrinsic & TIMEOUT;
            if (oldtimeout)
                Sprintf(buf, "%-27s [%li]", propname, oldtimeout);
            else
                Sprintf(buf, "%s", propname);
            add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
        }
        end_menu(win, "Which intrinsics?");
        n = select_menu(win, PICK_ANY, &pick_list);
        destroy_nhwindow(win);

        amt = 30; /* TODO: prompt for duration */
        for (j = 0; j < n; ++j) 
        {
            i = pick_list[j].item.a_int - 1; /* -1: reverse +1 above */
            p = propertynames[i].prop_num;
            oldtimeout = u.uprops[p].intrinsic & TIMEOUT;
            newtimeout = oldtimeout + (long) amt;
            switch (p) 
            {
            case SICK:
            case FOOD_POISONED:
            case MUMMY_ROT:
            case SLIMED:
            case STONED:
                if (oldtimeout > 0L && newtimeout > oldtimeout)
                    newtimeout = oldtimeout;
                break;
            }

            switch (p) 
            {
            case BLINDED:
                make_blinded(newtimeout, TRUE);
                break;
#if 0       /* make_confused() only gives feedback when confusion is
             * ending so use the 'default' case for it instead */
            case CONFUSION:
                make_confused(newtimeout, TRUE);
                break;
#endif /*0*/
            case DEAF:
                make_deaf(newtimeout, TRUE);
                break;
            case HALLUC:
                make_hallucinated(newtimeout, TRUE, 0L);
                break;
            case SICK:
                make_sick(newtimeout, wizintrinsic, TRUE, 0);
                break;
            case FOOD_POISONED:
                make_food_poisoned(newtimeout, wizintrinsic, TRUE, 0);
                break;
            case MUMMY_ROT:
                make_mummy_rotted(newtimeout, wizintrinsic, TRUE, 0);
                break;
            case SLIMED:
                Sprintf(buf, fmt,
                        !Slimed ? "" : " still", "turning into slime");
                make_slimed(newtimeout, buf, KILLED_BY, wizintrinsic, 0);
                break;
            case STONED:
                Sprintf(buf, fmt,
                        !Stoned ? "" : " still", "turning into stone");
                make_stoned(newtimeout, buf, KILLED_BY, wizintrinsic, 0);
                break;
            case STUNNED:
                make_stunned(newtimeout, TRUE);
                break;
            case VOMITING:
                Sprintf(buf, fmt, !Vomiting ? "" : " still", "vomiting");
                make_vomiting(newtimeout, FALSE);
                pline1(buf);
                break;
            case WARN_OF_MON:
                if (!Warn_of_mon) {
                    context.warntype.speciesidx = PM_GRID_BUG;
                    context.warntype.species
                                         = &mons[context.warntype.speciesidx];
                }
                goto def_feedback;
            case LEVITATION:
            case FLYING:
                float_vs_flight();
                /*FALLTHRU*/
            default:
            def_feedback:
                pline("Timeout for %s %s %d.", propertynames[i].prop_name,
                      oldtimeout ? "increased by" : "set to", amt);
                incr_itimeout(&u.uprops[p].intrinsic, amt);
                refresh_u_tile_gui_info(TRUE);
                break;
            }
            context.botl = 1; /* probably not necessary... */
        }
        if (n >= 1)
            free((genericptr_t) pick_list);
        doredraw();
    } 
    else
        pline(unavailcmd, visctrl((int) cmd_from_func(wiz_intrinsic)));

    return 0;
}

/* #wizrumorcheck command - verify each rumor access */
STATIC_PTR int
wiz_rumor_check(VOID_ARGS)
{
    rumor_check();
    return 0;
}

/* #terrain command -- show known map, inspired by crawl's '|' command */
STATIC_PTR int
doterrain(VOID_ARGS)
{
    winid men;
    menu_item *sel;
    anything any;
    int n;
    int which;

    /*
     * normal play: choose between known map without mons, obj, and traps
     *  (to see underlying terrain only), or
     *  known map without mons and objs (to see traps under mons and objs), or
     *  known map without mons (to see objects under monsters);
     * explore mode: normal choices plus full map (w/o mons, objs, traps);
     * wizard mode: normal and explore choices plus
     *  a dump of the internal levl[][].typ codes w/ level flags, or
     *  a legend for the levl[][].typ codes dump
     */
    men = create_nhwindow(NHW_MENU);
    start_menu_ex(men, GHMENU_STYLE_CHOOSE_COMMAND);
    any = zeroany;
    any.a_int = 1;
    add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
             "known map without monsters, objects, and traps",
             MENU_SELECTED);
    any.a_int = 2;
    add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
             "known map without monsters and objects",
             MENU_UNSELECTED);
    any.a_int = 3;
    add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
             "known map without monsters",
             MENU_UNSELECTED);
    if (discover || wizard) {
        any.a_int = 4;
        add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
                 "full map without monsters, objects, and traps",
                 MENU_UNSELECTED);
        if (wizard) {
            any.a_int = 5;
            add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
                     "internal levl[][].typ codes in base-36",
                     MENU_UNSELECTED);
            any.a_int = 6;
            add_menu(men, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR,
                     "legend of base-36 levl[][].typ codes",
                     MENU_UNSELECTED);
        }
    }
    end_menu(men, "View which?");

    n = select_menu(men, PICK_ONE, &sel);
    destroy_nhwindow(men);
    /*
     * n <  0: player used ESC to cancel;
     * n == 0: preselected entry was explicitly chosen and got toggled off;
     * n == 1: preselected entry was implicitly chosen via <space>|<enter>;
     * n == 2: another entry was explicitly chosen, so skip preselected one.
     */
    which = (n < 0) ? -1 : (n == 0) ? 1 : sel[0].item.a_int;
    if (n > 1 && which == 1)
        which = sel[1].item.a_int;
    if (n > 0)
        free((genericptr_t) sel);

    switch (which) {
    case 1: /* known map */
        reveal_terrain(0, TER_MAP);
        break;
    case 2: /* known map with known traps */
        reveal_terrain(0, TER_MAP | TER_TRP);
        break;
    case 3: /* known map with known traps and objects */
        reveal_terrain(0, TER_MAP | TER_TRP | TER_OBJ);
        break;
    case 4: /* full map */
        reveal_terrain(1, TER_MAP);
        break;
    case 5: /* map internals */
        wiz_map_levltyp();
        break;
    case 6: /* internal details */
        wiz_levltyp_legend();
        break;
    default:
        break;
    }
    return 0; /* no time elapses */
}

/* -enlightenment and conduct- */
STATIC_VAR winid en_win = WIN_ERR;
STATIC_VAR boolean en_via_menu = FALSE;
STATIC_VAR const char You_[] = "You ", are[] = "are ", were[] = "were ",
                  have[] = "have ", had[] = "had ", can[] = "can ",
                  could[] = "could ", cannot[] = "cannot ", could_not[] = "could not ";
STATIC_VAR const char have_been[] = "have been ", have_never[] = "have never ", have_not[] = "have not ", had_not[] = "had not ",
                  never[] = "never ";

#define enl_msg(prefix, present, past, suffix, ps) \
    enlght_line(prefix, final ? past : present, suffix, ps, FALSE)
#define enl_msg2(prefix, present, past, suffix, ps) \
    enlght_line(prefix, final ? past : present, suffix, ps, TRUE)
#define you_are(attr, ps) enl_msg(You_, are, were, attr, ps)
#define you_have(attr, ps) enl_msg(You_, have, had, attr, ps)
#define you_have_not(attr, ps) enl_msg(You_, have_not, had_not, attr, ps)
#define you_have_not2(attr) enl_msg2(You_, have_not, had_not, attr, "")
#define you_can(attr, ps) enl_msg(You_, can, could, attr, ps)
#define you_cannot(attr, ps) enl_msg(You_, cannot, could_not, attr, ps)
#define you_have_been(goodthing) enl_msg(You_, have_been, were, goodthing, "")
#define you_have_never(badthing) \
    enl_msg(You_, have_never, never, badthing, "")
#define you_have_X(something) \
    enl_msg(You_, have, (const char *) "", something, "")

STATIC_OVL void
enlght_out(buf, attr)
const char *buf;
int attr;
{
    if (en_via_menu) {
        anything any;

        any = zeroany;
        add_menu(en_win, NO_GLYPH, &any, 0, 0, attr, NO_COLOR, buf, FALSE);
    } else
        putstr(en_win, attr, buf);
}

STATIC_OVL void
enlght_line(start, middle, end, ps, parentheses)
const char *start, *middle, *end, *ps;
boolean parentheses;
{
    char buf[BUFSZ];

    Sprintf(buf, " %s%s%s%s%s.%s", parentheses ? "(" : "", start, middle, end, ps, parentheses ? ")" : "");
    enlght_out(buf, ATR_NONE);
}

/* format increased chance to hit or damage or defense (Protection) */
STATIC_OVL char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
    const char *modif, *bonus;
    boolean invrt;
    int absamt;

    absamt = abs(incamt);
    /* Protection amount is typically larger than damage or to-hit;
       reduce magnitude by a third in order to stretch modifier ranges
       (small:1..5, moderate:6..10, large:11..19, huge:20+) */
    if (!strcmp(inctyp, "defense"))
        absamt = (absamt * 2) / 3;

    if (absamt <= 3)
        modif = "small";
    else if (absamt <= 6)
        modif = "moderate";
    else if (absamt <= 12)
        modif = "large";
    else
        modif = "huge";

    modif = !incamt ? "no" : an(modif); /* ("no" case shouldn't happen) */
    bonus = (incamt >= 0) ? "bonus" : "penalty";
    /* "bonus <foo>" (to hit) vs "<bar> bonus" (damage, defense) */
    invrt = strcmp(inctyp, "to hit") ? TRUE : FALSE;

    Sprintf(outbuf, "%s %s %s", modif, invrt ? inctyp : bonus,
            invrt ? bonus : inctyp);
    if (final || wizard)
        Sprintf(eos(outbuf), " (%s%d)", (incamt > 0) ? "+" : "", incamt);

    return outbuf;
}

/* report half physical or half spell damage */
STATIC_OVL void
enlght_halfdmg(category, final)
int category;
int final;
{
    const char *category_name;
    char buf[BUFSZ];

    switch (category) {
    case HALF_PHYSICAL_DAMAGE:
        category_name = "physical";
        break;
    case HALF_PHYSICAL_DAMAGE_AGAINST_UNDEAD_AND_DEMONS:
        category_name = "physical";
        break;
    case HALF_SPELL_DAMAGE:
        category_name = "spell";
        break;
    default:
        category_name = "unknown";
        break;
    }
    Sprintf(buf, " %s %s damage%s", (final || wizard) ? "half" : "reduced",
            category_name, category == HALF_PHYSICAL_DAMAGE_AGAINST_UNDEAD_AND_DEMONS ? " from undead and demons" : "");
    enl_msg(You_, "take", "took", buf, from_what(category));
}

/* is hero actively using water walking capability on water (or lava)? */
STATIC_OVL boolean
walking_on_water()
{
    if (u.uinwater || Levitation || Flying)
        return FALSE;
    return (boolean) (Walks_on_water
                      && (is_pool(u.ux, u.uy) || is_lava(u.ux, u.uy)));
}

/* check whether hero is wearing something that player definitely knows
   confers the target property; item must have been seen and its type
   discovered but it doesn't necessarily have to be fully identified */
STATIC_OVL boolean
cause_known(propindx)
int propindx; /* index of a property which can be conveyed by worn item */
{
    register struct obj *o;

    /* simpler than from_what()/what_gives(); we don't attempt to
       handle artifacts and we deliberately ignore wielded items */

    for (o = invent; o; o = o->nobj) 
    {
        if (!object_stats_known(o))
            continue;

        if (item_is_giving_power(o, propindx))
            return TRUE;
    }
    return FALSE;
}

boolean 
object_stats_known(obj)
struct obj* obj;
{
    if (!obj)
        return FALSE;

    boolean statsknown = ((!obj->oartifact && objects[obj->otyp].oc_name_known) || (obj->oartifact && obj->aknown && obj->nknown));
    boolean dknown = obj->dknown;
    return (statsknown && dknown);
}


/* format a characteristic value, accommodating Strength's strangeness */
STATIC_OVL char *
attrval(attrindx, attrvalue, resultbuf)
int attrindx, attrvalue;
char resultbuf[]; /* should be at least [7] to hold "18/100\0" */
{
    if (attrindx != A_STR || attrvalue <= 18)
        Sprintf(resultbuf, "%d", attrvalue);
    else if (attrvalue > STR18(100)) /* 19 to 25 */
        Sprintf(resultbuf, "%d", attrvalue - 100);
    else /* simplify "18/ **" to be "18/100" */
        Sprintf(resultbuf, "18/%02d", attrvalue - 18);
    return resultbuf;
}

void
enlightenment(mode, final)
int mode;  /* BASICENLIGHTENMENT | MAGICENLIGHTENMENT (| both) */
int final; /* ENL_GAMEINPROGRESS:0, ENL_GAMEOVERALIVE, ENL_GAMEOVERDEAD */
{
    char buf[BUFSZ], tmpbuf[BUFSZ];
    int glyph = player_to_glyph_index(urole.rolenum, urace.racenum, Upolyd ? u.mfemale : flags.female, u.ualign.type, 0) + GLYPH_PLAYER_OFF;
    int gui_glyph = maybe_get_replaced_glyph(glyph, u.ux, u.uy, data_to_replacement_info(glyph, LAYER_MONSTER, (struct obj*)0, &youmonst, 0UL, 0UL, MAT_NONE, 0));

    en_win = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_ENLIGHTENMENT_SCREEN, gui_glyph, extended_create_window_info_from_mon(&youmonst));
    en_via_menu = !final;
    if (en_via_menu)
        start_menu_ex(en_win, GHMENU_STYLE_ATTRIBUTES);

    Strcpy(tmpbuf, plname);
    *tmpbuf = highc(*tmpbuf); /* same adjustment as bottom line */
    /* as in background_enlightenment, when poly'd we need to use the saved
       gender in u.mfemale rather than the current you-as-monster gender */
#ifndef GNH_MOBILE
    Sprintf(buf, "%s the %s's attributes:", tmpbuf,
            ((Upolyd ? u.mfemale : flags.female) && urole.name.f)
                ? urole.name.f
                : urole.name.m);

    /* title */
    enlght_out(buf, ATR_HEADING); /* "Conan the Archaeologist's attributes:" */
#endif
    /* background and characteristics; ^X or end-of-game disclosure */
    if (mode & BASICENLIGHTENMENT) {
        /* role, race, alignment, deities, dungeon level, time, experience */
        background_enlightenment(mode, final);
        /* hit points, energy points, armor class, gold */
        basics_enlightenment(mode, final);
        /* strength, dexterity, &c */
        characteristics_enlightenment(mode, final);
    }
    /* expanded status line information, including things which aren't
       included there due to space considerations--such as obvious
       alternative movement indicators (riding, levitation, &c), and
       various troubles (turning to stone, trapped, confusion, &c);
       shown for both basic and magic enlightenment */
    status_enlightenment(mode, final);
    /* remaining attributes; shown for potion,&c or wizard mode and
       explore mode ^X or end of game disclosure */
    if (mode & MAGICENLIGHTENMENT) {
        /* intrinsics and other traditional enlightenment feedback */
        attributes_enlightenment(mode, final);
    }

    if (!en_via_menu) {
        display_nhwindow(en_win, TRUE);
    } else {
        menu_item *selected = 0;
#ifdef GNH_MOBILE
        Sprintf(buf, "%s the %s", tmpbuf,
            ((Upolyd ? u.mfemale : flags.female) && urole.name.f)
            ? urole.name.f
            : urole.name.m);

        /* title */
        //enlght_out(buf, ATR_TITLE); /* "Conan the Archaeologist" */
    //    Sprintf(buf, "Attributes");
    //    enlght_out(buf, ATR_SUBTITLE); /* "Attributes" */

        end_menu_ex(en_win, "Attributes", buf);
#else
        end_menu(en_win, (char *) 0);
#endif
        if (select_menu(en_win, PICK_NONE, &selected) > 0)
            free((genericptr_t) selected);
        en_via_menu = FALSE;
    }
    destroy_nhwindow(en_win);
    en_win = WIN_ERR;
}

/*ARGSUSED*/
/* display role, race, alignment and such to en_win */
STATIC_OVL void
background_enlightenment(unused_mode, final)
int unused_mode UNUSED;
int final;
{
    const char *role_titl, *rank_titl;
    int innategend, difgend, difalgn;
    char buf[BUFSZ], tmpbuf[BUFSZ];

    /* note that if poly'd, we need to use u.mfemale instead of flags.female
       to access hero's saved gender-as-human/elf/&c rather than current one */
    innategend = (Upolyd ? u.mfemale : flags.female) ? 1 : 0;
    role_titl = (innategend && urole.name.f) ? urole.name.f : urole.name.m;
    rank_titl = rank_of(u.ulevel, Role_switch, innategend);
#ifndef GNH_MOBILE
    enlght_out("", ATR_NONE); /* separator after title */
#endif
    enlght_out("Background:", ATR_SUBHEADING);

    /* if polymorphed, report current shape before underlying role;
       will be repeated as first status: "you are transformed" and also
       among various attributes: "you are in beast form" (after being
       told about lycanthropy) or "you are polymorphed into <a foo>"
       (with countdown timer appended for wizard mode); we really want
       the player to know he's not a samurai at the moment... */
    if (Upolyd) {
        struct permonst *uasmon = youmonst.data;

        tmpbuf[0] = '\0';
        /* here we always use current gender, not saved role gender */
        if (!is_male(uasmon) && !is_female(uasmon) && !is_neuter(uasmon))
            Sprintf(tmpbuf, "%s ", genders[flags.female ? 1 : 0].adj);
        Sprintf(buf, "%sin %s%s form", !final ? "currently " : "", tmpbuf,
                pm_monster_name(uasmon, flags.female));
        you_are(buf, "");
    }

    /* report role; omit gender if it's redundant (eg, "female priestess") */
    tmpbuf[0] = '\0';
    if (!urole.name.f
        && ((urole.allow & ROLE_GENDMASK) == (ROLE_MALE | ROLE_FEMALE)
            || innategend != flags.initgend))
        Sprintf(tmpbuf, "%s ", genders[innategend].adj);
    buf[0] = '\0';
    if (Upolyd)
        Strcpy(buf, "actually "); /* "You are actually a ..." */
    if (!strcmpi(rank_titl, role_titl)) {
        /* omit role when rank title matches it */
        Sprintf(eos(buf), "%s, level %d %s%s", an(rank_titl), u.ulevel,
                tmpbuf, urace.noun);
    } else {
        Sprintf(eos(buf), "%s, a level %d %s%s %s", an(rank_titl), u.ulevel,
                tmpbuf, urace.adj, role_titl);
    }
    you_are(buf, "");

    /* report alignment (bypass you_are() in order to omit ending period);
       adverb is used to distinguish between temporary change (helm of opp.
       alignment), permanent change (one-time conversion), and original */
    Sprintf(buf, " %s%s%s, %son a mission for %s",
            You_, !final ? are : were,
            align_str(u.ualign.type),
            /* helm of opposite alignment (might hide conversion) */
            (u.ualign.type != u.ualignbase[A_CURRENT])
               /* what's the past tense of "currently"? if we used "formerly"
                  it would sound like a reference to the original alignment */
               ? (!final ? "currently " : "temporarily ")
               /* permanent conversion */
               : (u.ualign.type != u.ualignbase[A_ORIGINAL])
                  /* and what's the past tense of "now"? certainly not "then"
                     in a context like this...; "belatedly" == weren't that
                     way sooner (in other words, didn't start that way) */
                  ? (!final ? "now " : "belatedly ")
                  /* atheist (ignored in very early game) */
                  : (!u.uconduct.gnostic && moves > 1000L)
                     ? "nominally "
                     /* lastly, normal case */
                     : "",
            u_gname());

    boolean usenextrow = TRUE;
#ifdef GNH_MOBILE
    usenextrow = (boolean)final;
#endif
    if (usenextrow)
    {
        enlght_out(buf, ATR_NONE);
        strcpy(buf, "");
    }

    /* show the rest of this game's pantheon (finishes previous sentence)
       [appending "also Moloch" at the end would allow for straightforward
       trailing "and" on all three aligned entries but looks too verbose] */
    Sprintf(eos(buf), " who %s opposed by", !final ? "is" : "was");
    if (u.ualign.type != A_LAWFUL)
        Sprintf(eos(buf), " %s (%s) and", align_gname(A_LAWFUL),
                align_str(A_LAWFUL));
    if (u.ualign.type != A_NEUTRAL)
        Sprintf(eos(buf), " %s (%s)%s", align_gname(A_NEUTRAL),
                align_str(A_NEUTRAL),
                (u.ualign.type != A_CHAOTIC) ? " and" : "");
    if (u.ualign.type != A_CHAOTIC)
        Sprintf(eos(buf), " %s (%s)", align_gname(A_CHAOTIC),
                align_str(A_CHAOTIC));
    Strcat(buf, "."); /* terminate sentence */
    enlght_out(buf, ATR_NONE);

    /* show original alignment,gender,race,role if any have been changed;
       giving separate message for temporary alignment change bypasses need
       for tricky phrasing otherwise necessitated by possibility of having
       helm of opposite alignment mask a permanent alignment conversion */
    difgend = (innategend != flags.initgend);
    difalgn = (((u.ualign.type != u.ualignbase[A_CURRENT]) ? 1 : 0)
               + ((u.ualignbase[A_CURRENT] != u.ualignbase[A_ORIGINAL])
                  ? 2 : 0));
    if (difalgn & 1) { /* have temporary alignment so report permanent one */
        Sprintf(buf, "actually %s", align_str(u.ualignbase[A_CURRENT]));
        you_are(buf, "");
        difalgn &= ~1; /* suppress helm from "started out <foo>" message */
    }
    if (difgend || difalgn) { /* sex change or perm align change or both */
        Sprintf(buf, " You started out %s%s%s.",
                difgend ? genders[flags.initgend].adj : "",
                (difgend && difalgn) ? " and " : "",
                difalgn ? align_str(u.ualignbase[A_ORIGINAL]) : "");
        enlght_out(buf, ATR_NONE);
    }

    /* 3.6.2: dungeon level, so that ^X really has all status info as
       claimed by the comment below; this reveals more information than
       the basic status display, but that's one of the purposes of ^X;
       similar information is revealed by #overview; the "You died in
       <location>" given by really_done() is more rudimentary than this */
    *buf = *tmpbuf = '\0';
    if (In_endgame(&u.uz)) {
        int egdepth = observable_depth(&u.uz);

        (void) endgamelevelname(tmpbuf, egdepth);
        Sprintf(buf, "in the endgame, on the %s%s",
                !strncmp(tmpbuf, "Plane", 5) ? "Elemental " : "", tmpbuf);
    } else if (Is_knox(&u.uz)) {
        /* this gives away the fact that the knox branch is only 1 level */
        Sprintf(buf, "on the %s level", dungeons[u.uz.dnum].dname);
        /* TODO? maybe phrase it differently when actually inside the fort,
           if we're able to determine that (not trivial) */
    } else {
        char dgnbuf[QBUFSZ];

        Strcpy(dgnbuf, dungeons[u.uz.dnum].dname);
        if (!strncmpi(dgnbuf, "The ", 4))
            *dgnbuf = lowc(*dgnbuf);
        Sprintf(tmpbuf, "level %d",
                In_quest(&u.uz) ? dunlev(&u.uz) : depth(&u.uz));
        /* TODO? maybe extend this bit to include various other automatic
           annotations from the dungeon overview code */
        if (Is_really_rogue_level(&u.uz))
            Strcat(tmpbuf, ", a primitive area");
        else if (Is_bigroom(&u.uz) && !Blind)
            Strcat(tmpbuf, ", a very big room");
        Sprintf(buf, "in %s, on %s", dgnbuf, tmpbuf);
    }
    you_are(buf, "");

    /* this is shown even if the 'time' option is off */
    if (moves == 1L) {
        you_have("just started your adventure", "");
    } else {
        /* 'turns' grates on the nerves in this context... */
        Sprintf(buf, "the dungeon %ld turn%s ago", moves, plur(moves));
        /* same phrasing for current and final: "entered" is unconditional */
        enlght_line(You_, "entered ", buf, "", FALSE);
    }
    if (!Upolyd) {
        /* flags.showexp does not matter */
        /* experience level is already shown above */
        Sprintf(buf, "%-1ld experience point%s", u.uexp, plur(u.uexp));
        if (wizard) {
            if (u.ulevel < 30) {
                int ulvl = (int) u.ulevel;
                long nxtlvl = newuexp(ulvl);
                /* long oldlvl = (ulvl > 1) ? newuexp(ulvl - 1) : 0; */

                Sprintf(eos(buf), ", %ld %s%sneeded to attain level %d",
                        (nxtlvl - u.uexp), (u.uexp > 0) ? "more " : "",
                        !final ? "" : "were ", (ulvl + 1));
            }
        }
        you_have(buf, "");
    }
}

/* hit points, energy points, armor class -- essential information which
   doesn't fit very well in other categories */
/*ARGSUSED*/
STATIC_OVL void
basics_enlightenment(mode, final)
int mode UNUSED;
int final;
{
    static const char Power[] = "mana";
    char buf[BUFSZ];
    int pw = u.uen, hp = (Upolyd ? u.mh : u.uhp),
        pwmax = u.uenmax, hpmax = (Upolyd ? u.mhmax : u.uhpmax);

    enlght_out(" ", ATR_HALF_SIZE); /* separator after background */
    enlght_out("Basics:", ATR_SUBHEADING);

    if (hp < 0)
        hp = 0;
    /* "1 out of 1" rather than "all" if max is only 1; should never happen */
    if (hp == hpmax && hpmax > 1)
        Sprintf(buf, "%d hit points in total", hpmax);
    else
        Sprintf(buf, "%d out of %d hit point%s", hp, hpmax, plur(hpmax));
    you_have(buf, "");

    /* low max energy is feasible, so handle couple of extra special cases */
    if (pwmax == 0 || (pw == pwmax && pwmax == 2)) /* both: "all 2" is silly */
        Sprintf(buf, "%s %s", !pwmax ? "no" : "both", Power);
    else if (pw == pwmax && pwmax > 2)
        Sprintf(buf, "%d %s in total", pwmax, Power);
    else
        Sprintf(buf, "%d out of %d %s", pw, pwmax, Power);
    you_have(buf, "");

    if (Upolyd) {
        switch (mons[u.umonnum].mlevel) {
        case 0:
            /* status line currently being explained shows "HD:0" */
            Strcpy(buf, "0 hit dice (actually 1/2)");
            break;
        case 1:
            Strcpy(buf, "1 hit die");
            break;
        default:
            Sprintf(buf, "%d hit dice", mons[u.umonnum].mlevel);
            break;
        }
        you_have(buf, "");
    }

    Sprintf(buf, "%d", u.uac);
    enl_msg("Your armor class ", "is ", "was ", buf, "");

    Sprintf(buf, "%d (%d%% chance)", u.umc, magic_negation_percentage(u.umc));
    enl_msg("Your magic cancellation ", "is ", "was ", buf, "");

    /* gold; similar to doprgold(#seegold) but without shop billing info;
       same amount as shown on status line which ignores container contents */
    {
        static const char Your_wallet[] = "Your wallet ";
        long umoney = money_cnt(invent);

        if (!umoney) {
            enl_msg(Your_wallet, "is ", "was ", "empty", "");
        } else {
            Sprintf(buf, "%ld %s", umoney, currency(umoney));
            enl_msg(Your_wallet, "contains ", "contained ", buf, "");
        }
    }

    if (flags.pickup) {
        char ocl[MAX_OBJECT_CLASSES + 1];

        Strcpy(buf, "on");
        oc_to_str(flags.pickup_types, ocl);
        Sprintf(eos(buf), " for %s%s%s",
                *ocl ? "'" : "", *ocl ? ocl : "all types", *ocl ? "'" : "");
        if (flags.pickup_thrown && *ocl) /* *ocl: don't show if 'all types' */
            Strcat(buf, " plus thrown");
        if (iflags.autopickup_exceptions[AP_GRAB]
            || iflags.autopickup_exceptions[AP_LEAVE])
            Strcat(buf, ", with exceptions");
    } 
    else if (flags.pickup_thrown)
    {
        Strcpy(buf, "on for thrown only");
    }
    else
        Strcpy(buf, "off");
    enl_msg("Autopickup ", "is ", "was ", buf, "");

    const char* game_dif_text = get_game_difficulty_text(context.game_difficulty);
    strcpy(buf, game_dif_text);
    double monster_damage_mult = 1;
    double monster_hp_mult = 1;
    get_game_difficulty_multipliers(&monster_damage_mult, &monster_hp_mult);
    int pct_monster_dmg_mult = (int)(monster_damage_mult * 100);
    int pct_player_dmg_mult = (int)((1 / monster_hp_mult) * 100);
    char difficultybuf[BUFSIZ];
    Sprintf(difficultybuf, " (%d%% damage by monsters, %d%% by player)", pct_monster_dmg_mult, pct_player_dmg_mult);
    enl_msg("Your game difficulty ", "is ", "was ", buf, difficultybuf);

    char modebuf[BUFSIZ];
    Sprintf(modebuf, " mode (%s)", get_game_mode_description());
    enl_msg("You ", "are playing in ", "were playing in ", get_game_mode_text(TRUE), modebuf);

    Sprintf(buf, "%ld", get_current_game_score());
    enl_msg("Your game score ", "is ", "was ", buf, "");

    print_realtime(modebuf, get_current_game_duration());
    Sprintf(buf, "%s", modebuf);
    enl_msg("You ", "have been playing the game for ", "had been playing the game for ", buf, "");
}

/* characteristics: expanded version of bottom line strength, dexterity, &c */
STATIC_OVL void
characteristics_enlightenment(mode, final)
int mode;
int final;
{
    char buf[BUFSZ];

    enlght_out(" ", ATR_HALF_SIZE);
    Sprintf(buf, "%s Characteristics:", !final ? "Current" : "Final");
    enlght_out(buf, ATR_SUBHEADING);

    /* bottom line order */
    one_characteristic(mode, final, A_STR); /* strength */
    one_characteristic(mode, final, A_DEX); /* dexterity */
    one_characteristic(mode, final, A_CON); /* constitution */
    one_characteristic(mode, final, A_INT); /* intelligence */
    one_characteristic(mode, final, A_WIS); /* wisdom */
    one_characteristic(mode, final, A_CHA); /* charisma */
}

/* display one attribute value for characteristics_enlightenment() */
STATIC_OVL void
one_characteristic(mode, final, attrindx)
int mode, final, attrindx;
{
    extern const char *const attrname[]; /* attrib.c */
    boolean hide_innate_value = FALSE, interesting_alimit;
    int acurrent, abase, apeak, alimit;
    const char *paren_pfx;
    char subjbuf[BUFSZ], valubuf[BUFSZ], valstring[32];
    boolean parenthesis_open = FALSE;

    /* being polymorphed or wearing certain cursed items prevents
       hero from reliably tracking changes to characteristics so
       we don't show base & peak values then; when the items aren't
       cursed, hero could take them off to check underlying values
       and we show those in such case so that player doesn't need
       to actually resort to doing that */
    if (Upolyd) {
        hide_innate_value = TRUE;
    } else if (Fixed_abil) {
        if (stuck_ring(uleft, RIN_SUSTAIN_ABILITY)
            || stuck_ring(uright, RIN_SUSTAIN_ABILITY))
            hide_innate_value = TRUE;
    }

    if (AFIXMIN(attrindx) > 0 || AFIXMAX(attrindx) > 0)
        hide_innate_value = TRUE;

    /* note: final disclosure includes MAGICENLIGHTENTMENT */
    if ((mode & MAGICENLIGHTENMENT) && !Upolyd)
        hide_innate_value = FALSE;

    acurrent = ACURR(attrindx);
    (void) attrval(attrindx, acurrent, valubuf); /* Sprintf(valubuf,"%d",) */
    Sprintf(subjbuf, "Your %s ", attrname[attrindx]);

    if (!hide_innate_value) {
        /* show abase, amax, and/or attrmax if acurr doesn't match abase
           (a magic bonus or penalty is in effect) or abase doesn't match
           amax (some points have been lost to poison or exercise abuse
           and are restorable) or attrmax is different from normal human
           (while game is in progress; trying to reduce dependency on
           spoilers to keep track of such stuff) or attrmax was different
           from abase (at end of game; this attribute wasn't maxed out) */
        abase = ABASE(attrindx);
        apeak = AMAX(attrindx);
        alimit = ATTRMAX(attrindx);
        /* criterium for whether the limit is interesting varies */
        interesting_alimit =
            final ? TRUE /* was originally `(abase != alimit)' */
                  : (alimit != (attrindx != A_STR ? 18 : STR18(100)));
        paren_pfx = final ? " (" : " (current, ";
        if (acurrent != abase) {
            parenthesis_open = TRUE;
            Sprintf(eos(valubuf), "%sbase:%s", paren_pfx,
                    attrval(attrindx, abase, valstring));
            paren_pfx = ", ";
        }
        if (abase != apeak) {
            parenthesis_open = TRUE;
            Sprintf(eos(valubuf), "%speak:%s", paren_pfx,
                    attrval(attrindx, apeak, valstring));
            paren_pfx = ", ";
        }
        if (interesting_alimit) {
            parenthesis_open = TRUE;
            Sprintf(eos(valubuf), "%s%slimit:%s", paren_pfx,
                    /* more verbose if exceeding 'limit' due to magic bonus */
                    (acurrent > alimit) ? "innate " : "",
                    attrval(attrindx, alimit, valstring));
            /* paren_pfx = ", "; */
        }
    }

    if (attrindx == A_STR)
    {
        int currstr = ACURR(A_STR);
        int tohitbonus_constant = (currstr < STR18(100) ? strength_tohit_bonus(min(18, currstr)) : strength_tohit_bonus(currstr));
        int tohitbonus_random = ((currstr > 18 && currstr < STR18(100)) ? 1 : 0);
        int dmgbonus_constant = (currstr < STR18(100) ? strength_damage_bonus(min(18, currstr)) : strength_damage_bonus(currstr));
        int dmgbonus_random = ((currstr > 18 && currstr < STR18(100)) ? 2 : 0);
        //int random_chance = ((currstr > 18 && currstr < STR18(100)) ? currstr - 18 : 0);
        char tohitbuf[BUFSIZ];
        char dmgbuf[BUFSIZ];

        Sprintf(tohitbuf, "-%d", tohitbonus_constant + tohitbonus_random);
        Sprintf(dmgbuf, "-%d", dmgbonus_constant + dmgbonus_random);

        Sprintf(eos(valubuf), "%s%s%d%s to-hit %s, %s%d%s damage %s", 
            parenthesis_open ? ", " : " (",
            tohitbonus_constant >= 0 ? "+" : "",
            tohitbonus_constant,
            tohitbonus_random ? tohitbuf : "",
            tohitbonus_constant >= 0 ? "bonus" : "penalty",
            dmgbonus_constant >= 0 ? "+" : "",
            dmgbonus_constant,
            dmgbonus_random ? dmgbuf : "",
            dmgbonus_constant >= 0 ? "bonus" : "penalty"
            );

        parenthesis_open = TRUE;
    }
    else if (attrindx == A_DEX)
    {
        int tohitbonus = dexterity_tohit_bonus(ACURR(A_DEX));
        int acbonus = -dexterity_ac_bonus(ACURR(A_DEX));

        Sprintf(eos(valubuf), "%s%s%d to-hit %s, %s%d AC %s",
            parenthesis_open ? ", " : " (",
            tohitbonus >= 0 ? "+" : "", tohitbonus, tohitbonus >= 0 ? "bonus" : "penalty",
            acbonus >= 0 ? "+" : "", acbonus, acbonus <= 0 ? "bonus" : "penalty");

        parenthesis_open = TRUE;
    }
    else if (attrindx == A_CON)
    {
        double hpbonus = constitution_hp_bonus(ACURR(A_CON));

        Sprintf(eos(valubuf), "%s%s%0.1f HP %s per level",
            parenthesis_open ? ", " : " (",
            hpbonus >= 0 ? "+" : "", hpbonus, hpbonus >= 0 ? "bonus" : "penalty");

        parenthesis_open = TRUE;
    }

    if (parenthesis_open)
        Strcat(valubuf, ")");

    enl_msg(subjbuf, "is ", "was ", valubuf, "");
}

/* status: selected obvious capabilities, assorted troubles */
STATIC_OVL void
status_enlightenment(mode, final)
int mode;
int final;
{
    boolean magic = (mode & MAGICENLIGHTENMENT) ? TRUE : FALSE;
    int cap;
    enum p_skills wtype;
    char buf[BUFSZ], youtoo[BUFSZ];
    boolean Riding = (u.usteed
                      /* if hero dies while dismounting, u.usteed will still
                         be set; we want to ignore steed in that situation */
                      && !(final == ENL_GAMEOVERDEAD
                           && !strcmp(killer.name, "riding accident")));
    const char *steedname = (!Riding ? (char *) 0
                      : x_monnam(u.usteed, is_tame(u.usteed) ? ARTICLE_YOUR : ARTICLE_THE, (char *) 0,
                        (SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION), FALSE));

    /*\
     * Status (many are abbreviated on bottom line; others are or
     *     should be discernible to the hero hence to the player)
    \*/
    enlght_out(" ", ATR_HALF_SIZE); /* separator after title or characteristics */
    enlght_out(final ? "Final Status:" : "Current Status:", ATR_SUBHEADING);

    Strcpy(youtoo, You_);
    /* not a traditional status but inherently obvious to player; more
       detail given below (attributes section) for magic enlightenment */
    if (Upolyd) {
        Strcpy(buf, "transformed");
        if (ugenocided())
            Sprintf(eos(buf), " and %s %s inside",
                    final ? "felt" : "feel", udeadinside());
        you_are(buf, "");
    }
    /* not a trouble, but we want to display riding status before maybe
       reporting steed as trapped or hero stuck to cursed saddle */
    if (Riding) {
        Sprintf(buf, "riding %s", steedname);
        you_are(buf, "");
        Sprintf(eos(youtoo), "and %s ", steedname);
    }
    /* other movement situations that hero should always know */
    if (Levitation) {
        if (Lev_at_will && magic)
            you_are("levitating, at will", "");
        else
            enl_msg(youtoo, are, were, "levitating", !(magic || is_innate(LEVITATION) || cause_known(LEVITATION)) ? "" : from_what(LEVITATION));
    } else if (Flying) { /* can only fly when not levitating */
        enl_msg(youtoo, are, were, "flying", !(magic || is_innate(FLYING) || cause_known(FLYING)) ? "" : from_what(FLYING));
    }

    if (Underwater) {
        you_are("underwater", "");
    } else if (u.uinwater) {
        you_are(Swimming ? "swimming" : "in water", !(magic || is_innate(SWIMMING) || cause_known(SWIMMING)) ? "" : from_what(SWIMMING));
    } else if (walking_on_water()) {
        /* show active Wwalking here, potential Wwalking elsewhere */
        Sprintf(buf, "walking on %s",
                is_pool(u.ux, u.uy) ? "water"
                : is_lava(u.ux, u.uy) ? "lava"
                  : surface(u.ux, u.uy)); /* catchall; shouldn't happen */
        you_are(buf, !(magic || is_innate(WATER_WALKING) || cause_known(WATER_WALKING)) ? "" : from_what(WATER_WALKING));
    }
    if (Upolyd && (u.uundetected || U_AP_TYPE != M_AP_NOTHING))
        youhiding(TRUE, final);

    /* internal troubles, mostly in the order that prayer ranks them */
    if (Stoned)
        you_are("turning to stone", "");
    if (Slimed)
        you_are("turning into slime", "");
    if (Strangled) {
        Strcpy(buf, "being strangled");
        if (wizard)
            Sprintf(eos(buf), " (%ld)", (Strangled & TIMEOUT));
        you_are(buf, !(magic || cause_known(STRANGLED)) ? "" : from_what(STRANGLED));
    }

    if (u.uburied)
    {
        you_are("buried", "");
    }
    if (Airless_environment)
    {
        Strcpy(buf, "in an airless environment");
            if (wizard)
                Sprintf(eos(buf), " (%ld)", (HAirless_environment & TIMEOUT));
            you_are(buf, from_what(AIRLESS_ENVIRONMENT));
    }

    if (Sick) 
        you_are("terminally sick from illness", "");
    if (FoodPoisoned)
        you_are("terminally sick from food poisoning", "");
    if (MummyRot)
        you_are("sick from mummy rot", "");
    if (Vomiting)
        you_are("nauseated", "");
    if (Stunned)
        you_are("stunned", "");
    if (Confusion)
        you_are("confused", "");
    if (Hallucination)
        you_are("hallucinating", "");
    if (Blind) {
        /* from_what() (currently wizard-mode only) checks !haseyes()
           before u.uroleplay.blind, so we should too */
        Sprintf(buf, "%s blind",
                !haseyes(youmonst.data) ? "innately"
                : u.uroleplay.blind ? "permanently"
                  /* better phrasing desperately wanted... */
                  : Blind_because_of_blindfold_only ? "deliberately"
                    : "temporarily");
        if (wizard && (Blinded & TIMEOUT) != 0L
            && !u.uroleplay.blind && haseyes(youmonst.data))
            Sprintf(eos(buf), " (%ld)", (Blinded & TIMEOUT));
        /* !haseyes: avoid "you are innately blind innately" */
        you_are(buf, !haseyes(youmonst.data) ? "" : from_what(BLINDED));
    }
    if (Deaf)
        you_are("deaf", !(magic || is_innate(DEAF) || cause_known(DEAF)) ? "" : from_what(DEAF));

    /* external troubles, more or less */
    if (Punished) {
        if (uball) {
            Sprintf(buf, "chained to %s", ansimpleoname(uball));
        } else {
            impossible("Punished without uball?");
            Strcpy(buf, "punished");
        }
        you_are(buf, "");
    }
    if (u.utrap) {
        char predicament[BUFSZ];
        struct trap *t;
        boolean anchored = (u.utraptype == TT_BURIEDBALL);

        if (anchored) {
            Strcpy(predicament, "tethered to something buried");
        } else if (u.utraptype == TT_INFLOOR || u.utraptype == TT_LAVA) {
            Sprintf(predicament, "stuck in %s", the(surface(u.ux, u.uy)));
        } else {
            Strcpy(predicament, "trapped");
            if ((t = t_at(u.ux, u.uy)) != 0)
                Sprintf(eos(predicament), " in %s",
                        an(get_trap_explanation(t)));
        }
        if (u.usteed) { /* not `Riding' here */
            Sprintf(buf, "%s%s ", anchored ? "you and " : "", steedname);
            *buf = highc(*buf);
            enl_msg(buf, (anchored ? "are " : "is "),
                    (anchored ? "were " : "was "), predicament, "");
        } else
            you_are(predicament, "");
    } /* (u.utrap) */
    if (u.uswallow) {
        Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
        if (wizard)
            Sprintf(eos(buf), " (%u)", u.uswldtim);
        you_are(buf, "");
    } else if (u.ustuck) {
        Sprintf(buf, "%s %s",
                (Upolyd && sticks(youmonst.data)) ? "holding" : "held by",
                a_monnam(u.ustuck));
        you_are(buf, "");
    }
    if (Riding) {
        struct obj *saddle = which_armor(u.usteed, W_SADDLE);

        if (saddle && saddle->cursed) {
            Sprintf(buf, "stuck to %s %s", s_suffix(steedname),
                    simpleonames(saddle));
            you_are(buf, "");
        }
    }
    if (Wounded_legs) {
        /* when mounted, Wounded_legs applies to steed rather than to
           hero; we only report steed's wounded legs in wizard mode */
        if (u.usteed) { /* not `Riding' here */
            if (wizard && steedname) {
                Strcpy(buf, steedname);
                *buf = highc(*buf);
                enl_msg(buf, " has", " had", " wounded legs", "");
            }
        } else {
            Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
            you_have(buf, "");
        }
    }
    if (Glib) {
        Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
        you_have(buf, "");
    }
    if (Fumbling) {
        if (magic || cause_known(FUMBLING))
            enl_msg(You_, "fumble", "fumbled", "", from_what(FUMBLING));
    }
    if (Laughing) {
        if (magic || cause_known(LAUGHING))
            enl_msg(You_, "laugh uncontrollably", "laughed uncontrollably", "", from_what(LAUGHING));
    }
    if (Tottering) {
        if (magic || cause_known(TOTTERING))
            enl_msg(You_, "shake and totter", "shaked and tottered", "", from_what(TOTTERING));
    }
    if (OddIdeas) {
        if (magic || cause_known(ODD_IDEAS))
            enl_msg(You_, "get visionary ideas", "got visionary ideas", "", from_what(ODD_IDEAS));
    }
    if (Sleepy) {
        if (magic || cause_known(SLEEPY)) {
            Strcpy(buf, from_what(SLEEPY));
            if (wizard)
                Sprintf(eos(buf), " (%ld)", (HSleepy & TIMEOUT));
            enl_msg("You ", "fall", "fell", " asleep uncontrollably", buf);
        }
    }
    /* hunger/nutrition */
    if (Hunger) {
        if (magic || cause_known(HUNGER))
            enl_msg(You_, "hunger", "hungered", " rapidly",
                    from_what(HUNGER));
    }
    Strcpy(buf, hu_stat[u.uhs]); /* hunger status; omitted if "normal" */
    mungspaces(buf);             /* strip trailing spaces */
    if (*buf) {
        *buf = lowc(*buf); /* override capitalization */
        if (!strcmp(buf, "weak"))
            Strcat(buf, " from severe hunger");
        else if (!strncmp(buf, "faint", 5)) /* fainting, fainted */
            Strcat(buf, " due to starvation");
        you_are(buf, "");
    }
    /* encumbrance */
    if ((cap = near_capacity()) > UNENCUMBERED) {
        const char *adj = "?_?"; /* (should always get overridden) */

        Strcpy(buf, enc_stat[cap]);
        *buf = lowc(*buf);
        switch (cap) {
        case SLT_ENCUMBER:
            adj = "slightly";
            break; /* burdened */
        case MOD_ENCUMBER:
            adj = "moderately";
            break; /* stressed */
        case HVY_ENCUMBER:
            adj = "very";
            break; /* strained */
        case EXT_ENCUMBER:
            adj = "extremely";
            break; /* overtaxed */
        case OVERLOADED:
            adj = "not possible";
            break;
        }
        Sprintf(eos(buf), "; movement %s %s%s", !final ? "is" : "was", adj,
                (cap < OVERLOADED) ? " slowed" : "");
        you_are(buf, "");
    } else {
        /* last resort entry, guarantees Status section is non-empty
           (no longer needed for that purpose since weapon status added;
           still useful though) */
        you_are("unencumbered", "");
    }

    /* report being weaponless; distinguish whether gloves are worn */
    if (!uwep) 
    {
        if (!uarms)
        {
            you_are(uarmg ? "empty handed" /* gloves imply hands */
                : humanoid(youmonst.data)
                /* hands but no weapon and no gloves */
                ? "bare handed"
                /* alternate phrasing for paws or lack of hands */
                : "not wielding anything",
                "");
        }
        else
        {
            if (is_shield(uarms))
            {
                Sprintf(buf, "wielding a shield in your left %s", body_part(HAND));
                you_are(buf, "");
            }
            else
            {
                const char* what = weapon_descr(uarms);
                Sprintf(buf, "wielding %s in your left %s",
                    (uarms->quan == 1L) ? an(what) : makeplural(what), body_part(HAND));
                you_are(buf, "");
            }

        }
    /* two-weaponing implies hands (can't be polymorphed) and
       a weapon or wep-tool (not other odd stuff) in each hand */
    } else if (u.twoweap) {
        you_are("engaged in two-weapon fighting", "");
    /* report most weapons by their skill class (so a katana will be
       described as a long sword, for instance; mattock and hook are
       exceptions), or wielded non-weapon item by its object class */
    } else {
        const char *what = weapon_descr(uwep);

        if (!strcmpi(what, "armor") || !strcmpi(what, "food")
            || !strcmpi(what, "venom"))
            Sprintf(buf, "wielding some %s", what);
        else
            Sprintf(buf, "wielding %s",
                    (uwep->quan == 1L) ? an(what) : makeplural(what));

        if (uarms && is_shield(uarms))
            Strcat(buf, " and a shield");

        you_are(buf, "");
    }
    /*
     * Skill with current weapon.  Might help players who've never
     * noticed #enhance or decided that it was pointless.
     *
     * TODO?  Maybe merge wielding line and skill line into one sentence.
     */
    print_weapon_skill_line(uwep, TRUE, final);

    if (two_handed_bonus_applies(uwep))
        print_weapon_skill_line_core(P_TWO_HANDED_WEAPON, TRUE, final);

    if (!uwep && (P_SKILL_LEVEL(P_MARTIAL_ARTS) > P_UNSKILLED || Martial_prowess))
    {
        wtype = P_MARTIAL_ARTS;
        char sklvlbuf[20];
        int sklvl = P_SKILL_LEVEL(wtype);
        boolean hav = (sklvl != P_UNSKILLED && sklvl != P_SKILLED);

        if (sklvl == P_ISRESTRICTED)
            Strcpy(sklvlbuf, "no");
        else
            (void)lcase(skill_level_name(wtype, sklvlbuf, FALSE));
        /* "you have no/basic/expert/master/grand-master skill with <skill>"
           or "you are unskilled/skilled in <skill>" */

        int hitbonus = weapon_skill_hit_bonus(uwep, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */
        int dmgbonus = weapon_skill_dmg_bonus(uwep, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */

        Sprintf(buf, "%s %s %s%s (%s%d to hit and %s%d to damage)", sklvlbuf,
            hav ? "skill with" : "in", skill_name(wtype, TRUE), 
            Martial_prowess ? (final ? " and had martial prowess" : " and have martial prowess") : "",
            hitbonus >= 0 ? "+" : "", hitbonus, dmgbonus >= 0 ? "+" : "", dmgbonus);

        if (can_advance(wtype, FALSE))
            Sprintf(eos(buf), " and %s that",
                !final ? "can enhance" : "could have enhanced");
        if (hav)
            you_have(buf, "");
        else
            you_are(buf, "");
    }

    if (!uwep)
    {
        if (martial_bonus())
            wtype = P_MARTIAL_ARTS;
        else
            wtype = P_BARE_HANDED_COMBAT;

        char mbuf[BUFSZ];
        Strcpy(mbuf, "");
        if (martial_bonus())
        {
            int multihitchance = martial_arts_multishot_percentage_chance(adjusted_limited_skill_level(wtype, FALSE, TRUE));
            Sprintf(mbuf, ", %d%% double-hit chance", multihitchance); // due to% s", multihitchance, skill_name(wtype, TRUE));
        }
        Sprintf(buf, "fighting weaponless (%s%s)", martial_bonus() ? "full strength bonus" : "half strength damage bonus", mbuf);
        you_are(buf, "");
    }

    if (uarms && (!uwep || (weapon_skill_type(uwep) != weapon_skill_type(uarms))))
        print_weapon_skill_line(uarms, u.twoweap && is_weapon(uarms), final);

    if (u.twoweap) 
    {
        wtype = P_DUAL_WEAPON_COMBAT;
        char sklvlbuf[20];
        int sklvl = P_SKILL_LEVEL(wtype);
        boolean hav = (sklvl != P_UNSKILLED && sklvl != P_SKILLED);

        if (sklvl == P_ISRESTRICTED)
            Strcpy(sklvlbuf, "no");
        else
            (void)lcase(skill_level_name(wtype, sklvlbuf, FALSE));
        /* "you have no/basic/expert/master/grand-master skill with <skill>"
           or "you are unskilled/skilled in <skill>" */

        int hitbonus = weapon_skill_hit_bonus(uwep, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */
        int dmgbonus = weapon_skill_dmg_bonus(uwep, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */

        Sprintf(buf, "%s %s %s (%s%d to hit and %s%d to damage)", sklvlbuf,
            hav ? "skill with" : "in", skill_name(wtype, TRUE), hitbonus >= 0 ? "+" : "", hitbonus, dmgbonus >= 0 ? "+" : "", dmgbonus);

        if (can_advance(wtype, FALSE))
            Sprintf(eos(buf), " and %s that",
                !final ? "can enhance" : "could have enhanced");
        if (hav)
            you_have(buf, "");
        else
            you_are(buf, "");
    }

    /* report 'nudity' */
    if (!uarm && !uarmu && !uarmc && !uarms && !uarmg && !uarmf && !uarmh && !uarmo && !uarmb) {
        if (u.uroleplay.nudist)
            enl_msg(You_, "do", "did", " not wear any armor", "");
        else
            you_are("not wearing any armor", "");
    }

    if (u.ublessed > 0)
    {
        char protbuf[BUFSZ];
        Sprintf(protbuf, "endowed with divine protection (-%d to AC and +%d to MC)", u.ublessed, u.ublessed / 3);
        you_are(protbuf, "");
    }
}

STATIC_OVL
void
print_weapon_skill_line_core(wtype, printweaponstats, final)
enum p_skills wtype;
boolean printweaponstats;
int final;
{
    char buf[BUFSZ];
    if (wtype != P_NONE)
    {
        if (wtype == P_MARTIAL_ARTS)
            wtype = P_BARE_HANDED_COMBAT; /* Martial arts is separately below */

        char sklvlbuf[20];
        int sklvl = P_SKILL_LEVEL(wtype);
        boolean hav = (sklvl != P_UNSKILLED && sklvl != P_SKILLED);

        if (sklvl == P_ISRESTRICTED)
            Strcpy(sklvlbuf, "no");
        else
            (void)lcase(skill_level_name(wtype, sklvlbuf, FALSE));
        /* "you have no/basic/expert/master/grand-master skill with <skill>"
           or "you are unskilled/skilled in <skill>" */

        char ebuf[BUFSZ] = "";
        if (printweaponstats)
        {
            int hitbonus = weapon_skill_hit_bonus((struct obj*)0, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */
            int dmgbonus = weapon_skill_dmg_bonus((struct obj*)0, wtype, FALSE, FALSE, FALSE, 0, TRUE); /* Gives only pure skill bonuses */
            Sprintf(ebuf, "%s%d to hit%s%s%d to damage",
                hitbonus >= 0 ? "+" : "", hitbonus,
                wtype == P_SHIELD ? ", " : " and ",
                dmgbonus >= 0 ? "+" : "", dmgbonus);
            if (wtype == P_TWO_HANDED_WEAPON)
            {
                int multishotchance = two_handed_weapon_multishot_percentage_chance(P_SKILL_LEVEL(wtype));
                Strcat(ebuf, ", ");
                Sprintf(eos(ebuf), "%d%% double-hit chance", multishotchance);
            }
        }
        if (wtype == P_SHIELD)
        {
            int acbonus = -shield_skill_ac_bonus(P_SKILL_LEVEL(wtype));
            int mcbonus = shield_skill_mc_bonus(P_SKILL_LEVEL(wtype));
            if (printweaponstats)
                Strcat(ebuf, ", ");
            Sprintf(eos(ebuf), "%s%d to AC and %s%d to MC", acbonus >= 0 ? "+" : "", acbonus, mcbonus >= 0 ? "+" : "", mcbonus);
        }
        char pbuf[BUFSZ] = "";
        if (*ebuf)
            Sprintf(pbuf, " (%s)", ebuf);

        Sprintf(buf, "%s %s %s%s%s", sklvlbuf, hav ? "skill with" : "in",
            skill_name(wtype, TRUE),
            (wtype == P_BARE_HANDED_COMBAT || wtype == P_MARTIAL_ARTS) && Martial_prowess ? (final ? " and had martial prowess" : " and have martial prowess") : "",
            pbuf);

        if (can_advance(wtype, FALSE))
            Sprintf(eos(buf), " and %s that",
                !final ? "can enhance" : "could have enhanced");
        if (hav)
            you_have(buf, "");
        else
            you_are(buf, "");
    }
}

STATIC_OVL
void
print_weapon_skill_line(wep, printweaponstats, final)
struct obj* wep;
boolean printweaponstats;
int final;
{
    if (!wep)
        return;
    enum p_skills wtype = weapon_skill_type(wep);
    print_weapon_skill_line_core(wtype, printweaponstats, final);
}


/* attributes: intrinsics and the like, other non-obvious capabilities */
void
attributes_enlightenment(unused_mode, final)
int unused_mode UNUSED;
int final;
{
    static NEARDATA const char if_surroundings_permitted[] =
        " if surroundings permitted";
    int ltmp;
    char buf[BUFSZ];

    /*\
     *  Attributes
    \*/
    enlght_out(" ", ATR_HALF_SIZE);
    enlght_out(final ? "Final Attributes:" : "Current Attributes:", ATR_SUBHEADING);

    if (u.uevent.uhand_of_elbereth) {
        you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1], "");
    }

    Sprintf(buf, "%s", piousness(TRUE, "aligned"));
    if (u.ualign.record >= 0)
        you_are(buf, "");
    else
        you_have(buf, "");

    if (wizard) {
        Sprintf(buf, " %d", u.ualign.record);
        enl_msg("Your alignment ", "is", "was", buf, "");
    }

    /*** Resistances to troubles ***/
    if (Invulnerable)
        you_are("invulnerable", from_what(INVULNERABLE));
    if (Antimagic)
        you_are("magic-protected", from_what(ANTIMAGIC));

    if (Magic_missile_immunity)
        you_are("fully magic missile resistant", from_what(MAGIC_MISSILE_IMMUNITY));
    if (Improved_magic_missile_resistance)
    {
        if (Magic_missile_immunity)
            you_are("latently strongly (75%) magic missile resistant", from_what(IMPROVED_MAGIC_MISSILE_RESISTANCE));
        else
            you_are("strongly (75%) magic missile resistant", from_what(IMPROVED_MAGIC_MISSILE_RESISTANCE));
    }
    if (Magic_missile_resistance)
    {
        if (Magic_missile_immunity || Improved_magic_missile_resistance)
            you_are("latently weakly (50%) magic missile resistant", from_what(MAGIC_MISSILE_RESISTANCE));
        else
            you_are("weakly (50%) magic missile resistant", from_what(MAGIC_MISSILE_RESISTANCE));
    }

    if (Fire_immunity)
        you_are("fully fire resistant", from_what(FIRE_IMMUNITY));
    if (Improved_fire_resistance)
    {
        if (Fire_immunity)
            you_are("latently strongly (75%) fire resistant", from_what(IMPROVED_FIRE_RESISTANCE));
        else
            you_are("strongly (75%) fire resistant", from_what(IMPROVED_FIRE_RESISTANCE));
    }
    if (Fire_resistance)
    {
        if (Fire_immunity || Improved_fire_resistance)
            you_are("latently weakly (50%) fire resistant", from_what(FIRE_RESISTANCE));
        else
            you_are("weakly (50%) fire resistant", from_what(FIRE_RESISTANCE));
    }

    if (Cold_immunity)
        you_are("fully cold resistant", from_what(COLD_IMMUNITY));
    if (Improved_cold_resistance)
    {
        if (Cold_immunity)
            you_are("latently strongly (75%) cold resistant", from_what(IMPROVED_COLD_RESISTANCE));
        else
            you_are("strongly (75%) cold resistant", from_what(IMPROVED_COLD_RESISTANCE));
    }
    if (Cold_resistance)
    {
        if (Cold_immunity || Improved_cold_resistance)
            you_are("latently weakly (50%) cold resistant", from_what(COLD_RESISTANCE));
        else
            you_are("weakly (50%) cold resistant", from_what(COLD_RESISTANCE));
    }

    if (Shock_immunity)
        you_are("fully shock resistant", from_what(SHOCK_IMMUNITY));
    if (Improved_shock_resistance)
    {
        if (Shock_immunity)
            you_are("latently strongly (75%) shock resistant", from_what(IMPROVED_SHOCK_RESISTANCE));
        else
            you_are("strongly (75%) shock resistant", from_what(IMPROVED_SHOCK_RESISTANCE));
    }
    if (Shock_resistance)
    {
        if (Shock_immunity || Improved_shock_resistance)
            you_are("latently weakly (50%) shock resistant", from_what(SHOCK_RESISTANCE));
        else
            you_are("weakly (50%) shock resistant", from_what(SHOCK_RESISTANCE));
    }

    if (Sleep_resistance)
        you_are("sleep resistant", from_what(SLEEP_RESISTANCE));
    if (Disint_resistance)
        you_are("disintegration resistant", from_what(DISINTEGRATION_RESISTANCE));
    if (Death_resistance)
        you_are("immune to death attacks", from_what(DEATH_RESISTANCE));
    if (Charm_resistance)
        you_are("immune to charming", from_what(CHARM_RESISTANCE));
    if (Fear_resistance)
        you_are("immune to fear", from_what(FEAR_RESISTANCE));
    if (Mind_shielding)
        you_are("mind-shielded", from_what(MIND_SHIELDING));
    if (Lycanthropy_resistance)
        you_are("immune to lycanthropy", from_what(LYCANTHROPY_RESISTANCE));
    if (Curse_resistance)
        you_are("protected from curses", from_what(CURSE_RESISTANCE));
    if (Poison_resistance)
        you_are("poison resistant", from_what(POISON_RESISTANCE));

    if (Acid_immunity)
        you_are("fully acid resistant", from_what(ACID_IMMUNITY));
    if (Improved_acid_resistance)
    {
        if(Acid_immunity)
            you_are("latently strongly (75%) acid resistant", from_what(IMPROVED_ACID_RESISTANCE));
        else
            you_are("strongly (75%) acid resistant", from_what(IMPROVED_ACID_RESISTANCE));
    }
    if (Acid_resistance)
    {
        if (Acid_immunity || Improved_acid_resistance)
            you_are("latently weakly (50%) acid resistant", from_what(ACID_RESISTANCE));
        else
            you_are("weakly (50%) acid resistant", from_what(ACID_RESISTANCE));
    }

    if (Drain_resistance)
        you_are("level-drain resistant", from_what(DRAIN_RESISTANCE));
    if (Sick_resistance)
        you_are("immune to sickness", from_what(SICK_RESISTANCE));
    if (Stone_resistance)
        you_are("petrification resistant", from_what(STONE_RESISTANCE));
    if (Stun_resistance)
        you_are("stun resistant", from_what(STUN_RESISTANCE));
    if (Slime_resistance)
        you_are("sliming resistant", from_what(SLIME_RESISTANCE));
    if (Bisection_resistance)
        you_cannot("be bisected", from_what(BISECTION_RESISTANCE));
    if (Halluc_resistance)
        enl_msg(You_, "resist", "resisted", " hallucinations",
                from_what(HALLUC_RES));
    if (u.uedibility || maybe_polyd(is_gnoll(youmonst.data), Race_if(PM_GNOLL)))
        you_can("recognize detrimental food", "");


    /*** Vision and senses ***/
    if (See_invisible) {
        if (!Blind)
            enl_msg(You_, "see", "saw", " invisible", from_what(SEE_INVISIBLE));
        else
            enl_msg(You_, "will see", "would have seen",
                    " invisible when not blind", from_what(SEE_INVISIBLE));
    }
    if (Enhanced_vision)
        you_have("enhanced vision", from_what(ENHANCED_VISION));
    if (Eye_protection)
        you_are("protected from eye damage", from_what(EYE_PROTECTION));
    if (Blocks_Blindness)
        you_are("protected against blindness", from_what(BLOCKS_BLINDNESS));
    if (Flash_resistance)
        you_are("protected from bright light", from_what(FLASH_RESISTANCE));
    if (Brain_protection)
        you_are("protected from brain damage", from_what(BRAIN_PROTECTION));
    if (Unblind_telepat)
        you_are("telepathic", from_what(TELEPAT));
    if (Blind_telepat)
        you_are("telepathic when blind", from_what(BLIND_TELEPATHY));
    if (XRay_vision)
        you_have("X-ray vision", from_what(XRAY_VISION));
    if (Warning)
        you_are("warned", from_what(WARNING));
    if (Warn_of_mon && context.warntype.obj) {
        Sprintf(buf, "aware of the presence of %s",
                (context.warntype.obj & M2_ORC) ? "orcs"
                : (context.warntype.obj & M2_ELF) ? "elves"
                : (context.warntype.obj & M2_ANGEL) ? "angels"
                : (context.warntype.obj & M2_DEMON) ? "demons" : something);
        you_are(buf, from_what(WARN_OF_MON));
    }
    if (Warn_of_mon && context.warntype.polyd) {
        Sprintf(buf, "aware of the presence of %s",
                ((context.warntype.polyd & (M2_HUMAN | M2_ELF))
                 == (M2_HUMAN | M2_ELF))
                    ? "humans and elves"
                    : (context.warntype.polyd & M2_HUMAN)
                          ? "humans"
                          : (context.warntype.polyd & M2_ELF)
                                ? "elves"
                                : (context.warntype.polyd & M2_ORC)
                                      ? "orcs"
                                      : (context.warntype.polyd & M2_DEMON)
                                            ? "demons"
                                      : (context.warntype.polyd & M2_ANGEL)
                                            ? "angels"
                                            : "certain monsters");
        you_are(buf, "");
    }
    if (Warn_of_mon && context.warntype.speciesidx >= LOW_PM) {
        Sprintf(buf, "aware of the presence of %s",
                pm_plural_name(&mons[context.warntype.speciesidx], 0));
        you_are(buf, from_what(WARN_OF_MON));
    }
    if (Undead_warning)
        you_are("warned of undead", from_what(WARN_UNDEAD));
    if (Demon_warning)
        you_are("warned of demons", from_what(WARN_DEMON));
    if (Orc_warning)
        you_are("warned of orcs", from_what(WARN_ORC));
    if (Troll_warning)
        you_are("warned of trolls", from_what(WARN_TROLL));
    if (Giant_warning)
        you_are("warned of giants", from_what(WARN_GIANT));
    if (Dragon_warning)
        you_are("warned of dragons", from_what(WARN_DRAGON));
    if (Elf_warning)
        you_are("warned of elves", from_what(WARN_ELF));
    if (Dwarf_warning)
        you_are("warned of dwarves", from_what(WARN_DWARF));
    if (Gnoll_warning)
        you_are("warned of gnolls", from_what(WARN_GNOLL));
    if (Gnome_warning)
        you_are("warned of gnomes", from_what(WARN_GNOME));
    if (Ogre_warning)
        you_are("warned of ogres", from_what(WARN_OGRE));
    if (Human_warning)
        you_are("warned of human beings", from_what(WARN_HUMAN));
    if (Lycanthrope_warning)
        you_are("warned of lycanthropes", from_what(WARN_LYCANTHROPE));
    if (Angel_warning)
        you_are("warned of angels", from_what(WARN_ANGEL));
    if (Searching)
        you_have("automatic searching", from_what(SEARCHING));
    if (Clairvoyant)
        you_are("clairvoyant", from_what(CLAIRVOYANT));
    if (Blocks_Clairvoyance)
        you_are("blocking clairvoyance", from_what(BLOCKS_CLAIRVOYANCE));
    if (Infravision)
        you_have("infravision", from_what(INFRAVISION));
    if (Detect_monsters)
        you_are("sensing the presence of monsters", "");
    if (Enhanced_untrap)
        you_have("enhanced untrapping", from_what(ENHANCED_UNTRAP));
    if (u.umconf)
        you_are("going to confuse monsters", "");

    /*** Appearance and behavior ***/
    if (Adornment) {
        int adorn = 0;

        if (uleft && uleft->otyp == RIN_ADORNMENT)
            adorn += uleft->enchantment;
        if (uright && uright->otyp == RIN_ADORNMENT)
            adorn += uright->enchantment;
        /* the sum might be 0 (+0 ring or two which negate each other);
           that yields "you are charismatic" (which isn't pointless
           because it potentially impacts seduction attacks) */
        Sprintf(buf, "%scharismatic",
                (adorn > 0) ? "more " : (adorn < 0) ? "less " : "");
        you_are(buf, from_what(ADORNED));
    }
    if (Invisib)
        you_are("invisible", from_what(INVISIBILITY));
    if (Blocks_Invisibility)
        you_are("blocking invisibility ", from_what(BLOCKS_INVISIBILITY));
    else if (Invis)
        you_are("invisible to others", from_what(INVISIBILITY));
    /* ordinarily "visible" is redundant; this is a special case for
       the situation when invisibility would be an expected attribute */
    if (Displaced)
        you_are("displaced", from_what(DISPLACED));
    if (Stealth)
        you_are("stealthy", from_what(STEALTH));
    if (Magical_kicking)
        you_can("kick magically", from_what(MAGICAL_KICKING));
    if (Aggravate_monster)
        enl_msg("You aggravate", "", "d", " monsters",
                from_what(AGGRAVATE_MONSTER));
    if (Conflict)
        enl_msg("You cause", "", "d", " conflict", from_what(CONFLICT));

    /*** Transportation ***/
    if (Jumping)
        you_can("jump", from_what(JUMPING));
    if (Teleportation)
        you_can("teleport", from_what(TELEPORT));
    if (Teleport_control)
        you_have("teleport control", from_what(TELEPORT_CONTROL));
    /* actively levitating handled earlier as a status condition */
    if (Blocks_Levitation) { /* levitation is blocked */
        if (ELevitation || HLevitation) {
            /* either trapped in the floor or inside solid rock
               (or both if chained to buried iron ball and have
               moved one step into solid rock somehow) */
            boolean trapped = (HBlocks_Levitation & I_SPECIAL) != 0L,
                    terrain = (HBlocks_Levitation & FROM_ACQUIRED) != 0L;

            Sprintf(buf, "%s%s%s",
                    trapped ? " if not trapped" : "",
                    (trapped && terrain) ? " and" : "",
                    terrain ? if_surroundings_permitted : "");
            enl_msg(You_, "would levitate", "would have levitated", buf, "");
        }
    }
    if (Levitation_control)
        you_have("levitation control", from_what(LEVITATION_CONTROL));
    /* actively flying handled earlier as a status condition */
    if (Blocks_Flying) { /* flight is blocked */
        if (HFlying || EFlying) {
            enl_msg(You_, "would fly", "would have flown",
                    /* wording quibble: for past tense, "hadn't been"
                       would sound better than "weren't" (and
                       "had permitted" better than "permitted"), but
                       "weren't" and "permitted" are adequate so the
                       extra complexity to handle that isn't worth it */
                    Levitation
                       ? " if you weren't levitating"
                       : (HBlocks_Flying == I_SPECIAL)
                          /* this is an oversimpliction; being trapped
                             might also be blocking levitation so flight
                             would still be blocked after escaping trap */
                          ? " if you weren't trapped"
                          : (HBlocks_Flying == FROM_ACQUIRED)
                             ? if_surroundings_permitted
                             /* two or more of levitation, surroundings,
                                and being trapped in the floor */
                             : " if circumstances permitted",
                    "");
        }
    }
    /* actively walking on water handled earlier as a status condition */
    if (Wwalking)
        you_can("walk on water", from_what(WATER_WALKING));
    /* actively swimming (in water but not under it) handled earlier */
    if (Swimming && (Underwater || !u.uinwater))
        you_can("swim", from_what(SWIMMING));
    if (Breathless)
        you_can("survive without air", from_what(MAGICAL_BREATHING));
    else if (Amphibious)
        you_can("breathe water", from_what(MAGICAL_BREATHING));
    if (Passes_walls)
        you_can("walk through walls", from_what(PASSES_WALLS));

    /*** Physical attributes ***/
    if (Divine_regeneration)
    {
        enl_msg("You extremely rapidly regenerate", "", "d", " hit points", from_what(DIVINE_REGENERATION));
    }
    if (Rapidest_regeneration)
    {
        if (Divine_regeneration)
            enl_msg("You latently very very rapidly regenerate", "", "d", " hit points", from_what(RAPIDEST_REGENERATION));
        else
            enl_msg("You very very rapidly regenerate", "", "d", " hit points", from_what(RAPIDEST_REGENERATION));
    }
    if (Rapider_regeneration)
    {
        if (Rapidest_regeneration || Divine_regeneration)
            enl_msg("You latently very rapidly regenerate", "", "d", " hit points", from_what(RAPIDER_REGENERATION));
        else
            enl_msg("You very rapidly regenerate", "", "d", " hit points", from_what(RAPIDER_REGENERATION));
    }
    if (Rapid_regeneration)
    {
        if (Rapider_regeneration || Rapidest_regeneration || Divine_regeneration)
            enl_msg("You latently rapidly regenerate", "", "d", " hit points", from_what(RAPID_REGENERATION));
        else
            enl_msg("You rapidly regenerate", "", "d", " hit points", from_what(RAPID_REGENERATION));
    }
    if (Regeneration)
    {
        if(Rapid_regeneration || Rapider_regeneration || Rapidest_regeneration || Divine_regeneration)
            enl_msg("You latently regenerate", "", "d", " hit points", from_what(REGENERATION));
        else
            enl_msg("You regenerate", "", "d", " hit points", from_what(REGENERATION));
    }

    if (Rapidest_energy_regeneration)
    {
        enl_msg("You very very rapidly regenerate", "", "d", " mana", from_what(RAPIDEST_ENERGY_REGENERATION));
    }
    if (Rapider_energy_regeneration)
    {
        if (Rapidest_energy_regeneration)
            enl_msg("You latently very rapidly regenerate", "", "d", " mana", from_what(RAPIDER_ENERGY_REGENERATION));
        else
            enl_msg("You very rapidly regenerate", "", "d", " mana", from_what(RAPIDER_ENERGY_REGENERATION));
    }
    if (Rapid_energy_regeneration)
    {
        if (Rapider_energy_regeneration || Rapidest_energy_regeneration)
            enl_msg("You latently rapidly regenerate", "", "d", " mana", from_what(RAPID_ENERGY_REGENERATION));
        else
            enl_msg("You rapidly regenerate", "", "d", " mana", from_what(RAPID_ENERGY_REGENERATION));
    }
    if (Energy_regeneration)
    {
        if (Rapid_energy_regeneration || Rapider_energy_regeneration || Rapidest_energy_regeneration)
            enl_msg("You latently regenerate", "", "d", " mana", from_what(ENERGY_REGENERATION));
        else
            enl_msg("You regenerate", "", "d", " mana", from_what(ENERGY_REGENERATION));
    }

    if (Slow_digestion)
        you_have("slower digestion", from_what(SLOW_DIGESTION));
    if (Half_slow_digestion)
    {
        if(Slow_digestion)
            enl_msg("You latently ", "have", "had", " half slower digestion", from_what(HALF_SLOW_DIGESTION));
        else
            you_have("half slower digestion", from_what(HALF_SLOW_DIGESTION));
    }
    if (u.ubasehitinc + u.uhitinc)
        you_have(enlght_combatinc("to hit", u.ubasehitinc + u.uhitinc, final, buf), "");
    if (u.ubasedaminc + u.udaminc)
        you_have(enlght_combatinc("damage", u.ubasedaminc + u.udaminc, final, buf), "");
    if (Super_heroism)
    {
        enl_msg("You cause", "", "d", " double damage", from_what(SUPER_HEROISM));
    }
    if (Heroism)
    {
        if (Super_heroism)
            enl_msg("You latently cause", "", "d", " 150% damage", from_what(HEROISM));
        else
            enl_msg("You cause", "", "d", " 150% damage", from_what(HEROISM));
    }
    if (Melee_life_leech)
        enl_msg("You leech", "", "d", " life in melee", from_what(MELEE_LIFE_LEECH));


    int role_ac_bonus = get_role_AC_bonus();
    int role_mc_bonus = get_role_MC_bonus();
    if (role_ac_bonus != 0 || role_mc_bonus != 0)
    {
        char protbuf[BUFSZ];
        Sprintf(protbuf, "innate protection (-%d to AC and +%d to MC)", role_ac_bonus, role_mc_bonus);
        you_have(protbuf, "");
    }

    if (Magical_protection)
        you_have("magical protection (-3 to AC and +1 to MC)", from_what(MAGICAL_PROTECTION));
    if (Magical_shielding)
        you_have("magical shielding (-6 to AC and +2 to MC)", from_what(MAGICAL_SHIELDING));
    if (Magical_barkskin)
        you_have("magical barkskin (-12 to AC and +4 to MC)", from_what(MAGICAL_BARKSKIN));
    if (Magical_stoneskin)
        you_have("magical stoneskin (-18 to AC and +6 to MC)", from_what(MAGICAL_STONESKIN));
    if (Titan_strength)
        you_are("magically as strong as a titan", from_what(TITAN_STRENGTH));
    if (Divine_dexterity)
        you_have("your dexterity magically increased to equivalent that of a demigod", from_what(DIVINE_DEXTERITY));
    if (Divine_endurance)
        you_have("your constitution magically increased to equivalent that of a demigod", from_what(DIVINE_ENDURANCE));
    if (Divine_intellect)
        you_have("your intelligence magically increased to equivalent that of a demigod", from_what(DIVINE_INTELLECT));
    if (Divine_wisdom)
        you_have("your wisdom magically increased to equivalent that of a demigod", from_what(DIVINE_WISDOM));
    if (Divine_charisma)
        you_have("your charisma magically increased to equivalent that of a demigod", from_what(DIVINE_CHARISMA));

    if (Half_physical_damage)
        enlght_halfdmg(HALF_PHYSICAL_DAMAGE, final);
    if (Half_physical_damage_against_undead_and_demons)
        enlght_halfdmg(HALF_PHYSICAL_DAMAGE_AGAINST_UNDEAD_AND_DEMONS, final);
    if (Half_spell_damage)
        enlght_halfdmg(HALF_SPELL_DAMAGE, final);
    if (Fire_vulnerability)
        you_are("vulnerable to fire", from_what(FIRE_VULNERABILITY));
    if (Cold_vulnerability)
        you_are("vulnerable to cold", from_what(COLD_VULNERABILITY));
    if (Elec_vulnerability)
        you_are("vulnerable to electricity", from_what(SHOCK_VULNERABILITY));
    if (Magic_damage_vulnerability)
        you_are("vulnerable to magical damage", from_what(MAGIC_MISSILE_VULNERABILITY));
    if (Double_physical_damage)
        you_are("receiving double physical damage", from_what(DOUBLE_PHYSICAL_DAMAGE));
    if (Double_spell_damage)
        you_are("receiving double spell damage", from_what(DOUBLE_SPELL_DAMAGE));
    /* polymorph and other shape change */
    if (Protection_from_shape_changers)
        you_are("protected from shape changers",
                from_what(PROT_FROM_SHAPE_CHANGERS));
    if (Unchanging) {
        const char *what = 0;

        if (!Upolyd) /* Upolyd handled below after current form */
            you_can("not change from your current form",
                    from_what(UNCHANGING));
        /* blocked shape changes */
        if (Polymorph)
            what = !final ? "polymorph" : "have polymorphed";
        else if (u.ulycn >= LOW_PM)
            what = !final ? "change shape" : "have changed shape";
        if (what) {
            Sprintf(buf, "would %s periodically", what);
            /* omit from_what(UNCHANGING); too verbose */
            enl_msg(You_, buf, buf, " if not locked into your current form",
                    "");
        }
    } else if (Polymorph) {
        you_are("polymorphing periodically", from_what(POLYMORPH));
    }
    if (Polymorph_control)
        you_have("polymorph control", from_what(POLYMORPH_CONTROL));
    if (Upolyd && u.umonnum != u.ulycn
        /* if we've died from turning into slime, we're polymorphed
           right now but don't want to list it as a temporary attribute
           [we need a more reliable way to detect this situation] */
        && !(final == ENL_GAMEOVERDEAD
             && u.umonnum == PM_GREEN_SLIME && !Unchanging)) {
        /* foreign shape (except were-form which is handled below) */
        Sprintf(buf, "polymorphed into %s", an(pm_monster_name(youmonst.data, flags.female)));
        if (wizard)
            Sprintf(eos(buf), " (%d)", u.mtimedone);
        you_are(buf, "");
    }
    if (lays_eggs(youmonst.data) && flags.female) /* Upolyd */
        you_can("lay eggs", "");
    if (u.ulycn >= LOW_PM) {
        /* "you are a werecreature [in beast form]" */
        Strcpy(buf, an(pm_monster_name(&mons[u.ulycn], flags.female)));
        if (u.umonnum == u.ulycn) {
            Strcat(buf, " in beast form");
            if (wizard)
                Sprintf(eos(buf), " (%d)", u.mtimedone);
        }
        you_are(buf, "");
    }
    if (Unchanging && Upolyd) /* !Upolyd handled above */
        you_can("not change from your current form", from_what(UNCHANGING));
    if (Hate_silver)
        you_are("harmed by silver", "");
    /* movement and non-armor-based protection */
    if (Lightning_fast)
    {
        you_are("lightning fast", from_what(LIGHTNING_FAST));
    }
    if (Super_fast)
    {
        if (Lightning_fast)
            you_have("latent super-fastness", from_what(SUPER_FAST));
        else
            you_are("super fast", from_what(SUPER_FAST));
    }
    if (Ultra_fast)
    {
        if (Super_fast || Lightning_fast)
            you_have("latent ultra-fastness", from_what(ULTRA_FAST));
        else
            you_are("ultra fast", from_what(ULTRA_FAST));
    }
    if (Very_fast)
    {
        if (Ultra_fast || Super_fast || Lightning_fast)
            you_have("latent very-fastness", from_what(VERY_FAST));
        else
            you_are("very fast", from_what(VERY_FAST));
    }
    if (Fast)
    {
        if (Very_fast || Ultra_fast || Super_fast || Lightning_fast)
            you_have("latent fastness", from_what(FAST));
        else
            you_are("fast", from_what(FAST));
    }
    if (Slowed)
        you_are("slowed", from_what(SLOWED));
    if (Silenced)
        you_are("silenced", from_what(SILENCED));
    if (Paralyzed)
        you_are("paralyzed", from_what(PARALYZED));
    if (Undead_immobility)
        you_are("magically held immobile when undead", from_what(UNDEAD_IMMOBILITY));
    if (Fearful)
        you_are("magically frightened", from_what(FEARFUL));
    if (Sleeping)
        you_are("in a magically induced sleep", from_what(SLEEPING));
    if (Cancelled)
        you_are("cancelled", from_what(CANCELLED));
    if (Cancellation_resistance)
    {
        if(Cancelled)
            you_cannot("be cancelled again", from_what(CANCELLATION_RESISTANCE));
        else
            you_cannot("be cancelled", from_what(CANCELLATION_RESISTANCE));
    }
    if (Summon_forbidden)
        you_have("summoning forbidden", from_what(SUMMON_FORBIDDEN));
    if (Crazed)
        you_are("crazed", from_what(CRAZED));

    if (No_magic_resistance)
        you_have("no magic resistance", from_what(NO_MAGIC_RESISTANCE));
    else if (One_fourth_magic_resistance)
        you_have("25% of normal magic resistance", from_what(ONE_FOURTH_MAGIC_RESISTANCE));
    else if (Half_magic_resistance)
        you_have("half normal magic resistance", from_what(HALVED_MAGIC_RESISTANCE));
    else if (Three_fourths_magic_resistance)
        you_have("75% of normal magic resistance", from_what(THREE_FOURTHS_MAGIC_RESISTANCE));

    if (Charmed)
        you_are("charmed", from_what(CHARMED));
    if (Undead_control)
    {
        if(is_undead(youmonst.data))
            you_are("under undead control", from_what(UNDEAD_CONTROL));
        else
            you_are("latently under undead control", from_what(UNDEAD_CONTROL));
    }

    if (Reflecting)
        you_have("reflection", from_what(REFLECTING));
    if (Free_action)
        you_have("free action", from_what(FREE_ACTION));
    if (Fixed_abil)
        you_have("fixed abilities", from_what(FIXED_ABIL));
    if (Lifesaved)
        enl_msg("Your life ", "will be", "would have been", " saved", "");


    /*** Miscellany ***/
    if (Luck) {
        ltmp = abs((int) Luck);
        Sprintf(buf, "%s%slucky",
                ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
                Luck < 0 ? "un" : "");
        if (wizard)
            Sprintf(eos(buf), " (%d)", Luck);
        you_are(buf, "");
    } else if (wizard)
        enl_msg("Your luck ", "is", "was", " zero", "");
    if (u.moreluck > 0)
        you_have("extra luck", "");
    else if (u.moreluck < 0)
        you_have("reduced luck", "");
    if (u.unluck_does_not_timeout)
        enl_msg("Bad luck ", "does", "did", " not time out for you", "");
    if (u.luck_does_not_timeout)
        enl_msg("Good luck ", "does", "did", " not time out for you", "");

    if (u.ugangr) {
        Sprintf(buf, " %sangry with you",
                u.ugangr > 6 ? "extremely " : u.ugangr > 3 ? "very " : "");
        if (wizard)
            Sprintf(eos(buf), " (%d)", u.ugangr);
        enl_msg(u_gname(), " is", " was", buf, "");
    } else {
        /*
         * We need to suppress this when the game is over, because death
         * can change the value calculated by can_pray(), potentially
         * resulting in a false claim that you could have prayed safely.
         */
        if (!final) {
#if 0
            /* "can [not] safely pray" vs "could [not] have safely prayed" */
            Sprintf(buf, "%s%ssafely pray%s", can_pray(FALSE) ? "" : "not ",
                    final ? "have " : "", final ? "ed" : "");
#else
            Sprintf(buf, "safely pray");
#endif
            if (wizard)
                Sprintf(eos(buf), " (%d)", u.uprayer_timeout);

            if(can_pray(FALSE))
                you_can(buf, "");
            else
                you_cannot(buf, "");
        }
    }

#ifdef DEBUG
    /* named fruit debugging (doesn't really belong here...); to enable,
       include 'fruit' in DEBUGFILES list (even though it isn't a file...) */
    if (wizard && explicitdebug("fruit")) {
        struct fruit *f;

        reorder_fruit(TRUE); /* sort by fruit index, from low to high;
                              * this modifies the ffruit chain, so could
                              * possibly mask or even introduce a problem,
                              * but it does useful sanity checking */
        for (f = ffruit; f; f = f->nextf) {
            Sprintf(buf, "Fruit #%d ", f->fid);
            enl_msg(buf, "is ", "was ", f->fname, "");
        }
        enl_msg("The current fruit ", "is ", "was ", pl_fruit, "");
        Sprintf(buf, "%d", flags.made_fruit);
        enl_msg("The made fruit flag ", "is ", "was ", buf, "");
    }
#endif

    {
        const char *p;

        buf[0] = '\0';
        if (final < 2) { /* still in progress, or quit/escaped/ascended */
            p = "survived after being killed ";
            switch (u.umortality) {
            case 0:
                p = !final ? (char *) 0 : "survived";
                break;
            case 1:
                Strcpy(buf, "once");
                break;
            case 2:
                Strcpy(buf, "twice");
                break;
            case 3:
                Strcpy(buf, "thrice");
                break;
            default:
                Sprintf(buf, "%d times", u.umortality);
                break;
            }
        } else { /* game ended in character's death */
            p = "are dead";
            switch (u.umortality) {
            case 0:
                impossible("dead without dying?");
            case 1:
                break; /* just "are dead" */
            default:
                Sprintf(buf, " (%d%s time!)", u.umortality,
                        ordin(u.umortality));
                break;
            }
        }
        if (p)
            enl_msg(You_, "have been killed ", p, buf, "");
    }
}

#if 0  /* no longer used */
STATIC_DCL boolean NDECL(minimal_enlightenment);

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL boolean
minimal_enlightenment()
{
    winid tmpwin;
    menu_item *selected;
    anything any;
    int genidx, n;
    char buf[BUFSZ], buf2[BUFSZ];
    static const char untabbed_fmtstr[] = "%-15s: %-12s";
    static const char untabbed_deity_fmtstr[] = "%-17s%s";
    static const char tabbed_fmtstr[] = "%s:\t%-12s";
    static const char tabbed_deity_fmtstr[] = "%s\t%s";
    static const char *fmtstr;
    static const char *deity_fmtstr;

    fmtstr = iflags.menu_tab_sep ? tabbed_fmtstr : untabbed_fmtstr;
    deity_fmtstr = iflags.menu_tab_sep ? tabbed_deity_fmtstr
                                       : untabbed_deity_fmtstr;
    any = zeroany;
    buf[0] = buf2[0] = '\0';
    tmpwin = create_nhwindow(NHW_MENU);
    start_menu(tmpwin);
    add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR,
             "Starting", FALSE, menu_heading_info());

    /* Starting name, race, role, gender */
    Sprintf(buf, fmtstr, "name", plname);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    Sprintf(buf, fmtstr, "race", urace.noun);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    Sprintf(buf, fmtstr, "role",
            (flags.initgend && urole.name.f) ? urole.name.f : urole.name.m);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    Sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    /* Starting alignment */
    Sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL]));
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    /* Current name, race, role, gender */
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "", FALSE);
    add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR,
             "Current", FALSE, menu_heading_info());
    Sprintf(buf, fmtstr, "race", Upolyd ? youmonst.data->mname : urace.noun);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    if (Upolyd) {
        Sprintf(buf, fmtstr, "role (base)",
                (u.mfemale && urole.name.f) ? urole.name.f
                                            : urole.name.m);
        add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    } else {
        Sprintf(buf, fmtstr, "role",
                (flags.female && urole.name.f) ? urole.name.f
                                               : urole.name.m);
        add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    }
    /* don't want poly_gender() here; it forces `2' for non-humanoids */
    genidx = is_neuter(youmonst.data) ? 2 : flags.female;
    Sprintf(buf, fmtstr, "gender", genders[genidx].adj);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    if (Upolyd && (int) u.mfemale != genidx) {
        Sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj);
        add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);
    }

    /* Current alignment */
    Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type));
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    /* Deity list */
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, "", FALSE);
    add_extended_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, NO_COLOR,
             "Deities", FALSE, menu_heading_info());
    Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
            (u.ualignbase[A_ORIGINAL] == u.ualign.type
             && u.ualign.type == A_CHAOTIC)               ? " (s,c)"
                : (u.ualignbase[A_ORIGINAL] == A_CHAOTIC) ? " (s)"
                : (u.ualign.type   == A_CHAOTIC)          ? " (c)" : "");
    Sprintf(buf, fmtstr, "Chaotic", buf2);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
            (u.ualignbase[A_ORIGINAL] == u.ualign.type
             && u.ualign.type == A_NEUTRAL)               ? " (s,c)"
                : (u.ualignbase[A_ORIGINAL] == A_NEUTRAL) ? " (s)"
                : (u.ualign.type   == A_NEUTRAL)          ? " (c)" : "");
    Sprintf(buf, fmtstr, "Neutral", buf2);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
            (u.ualignbase[A_ORIGINAL] == u.ualign.type
             && u.ualign.type == A_LAWFUL)                ? " (s,c)"
                : (u.ualignbase[A_ORIGINAL] == A_LAWFUL)  ? " (s)"
                : (u.ualign.type   == A_LAWFUL)           ? " (c)" : "");
    Sprintf(buf, fmtstr, "Lawful", buf2);
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, buf, FALSE);

    end_menu(tmpwin, "Base Attributes");
    n = select_menu(tmpwin, PICK_NONE, &selected);
    destroy_nhwindow(tmpwin);
    return (boolean) (n != -1);
}
#endif /*0*/


STATIC_PTR int
dopolyformstatistics(VOID_ARGS)
{
    return monsterdescription(&youmonst);
}

STATIC_PTR int
doviewpetstatistics(struct monst* mon)
{
    return monsterdescription(mon);
}


/* ^X command */
STATIC_PTR int
doattributes(VOID_ARGS)
{
    int mode = BASICENLIGHTENMENT;

    /* show more--as if final disclosure--for wizard and explore modes */
    if ((wizard && yn_query("Enforce magic enlightenment?") == 'y') || discover)
        mode |= MAGICENLIGHTENMENT;

    enlightenment(mode, ENL_GAMEINPROGRESS);
    return 0;
}

/* M('c') command */
int
docommandmenu(VOID_ARGS)
{
    winid menuwin;
    menu_item* selected = (menu_item*)0;
    int n = 0;

    menuwin = create_nhwindow(NHW_MENU);
    start_menu_ex(menuwin, GHMENU_STYLE_ITEM_COMMAND);

    add_command_menu_items(menuwin, INCMDMENU);

    end_menu(menuwin, "Choose an action:");
    n = select_menu(menuwin, PICK_ONE, &selected);
    if (n > 0)
    {
        if (selected->item.a_nfunc)
        {
            (void)(selected->item.a_nfunc)();
        }
        free((genericptr_t)selected);
    }

    destroy_nhwindow(menuwin);
    return 0;

}


/* '+' command */
int
dospellmainmenu(VOID_ARGS)
{
    winid menuwin;
    menu_item* selected = (menu_item*)0;
    int n = 0;

    menuwin = create_nhwindow(NHW_MENU);
    start_menu_ex(menuwin, GHMENU_STYLE_ITEM_COMMAND);

    add_command_menu_items(menuwin, INSPELLMENU);

    end_menu(menuwin, "Choose a command for spells:");
    n = select_menu(menuwin, PICK_ONE, &selected);
    if (n > 0)
    {
        if (selected->item.a_nfunc)
        {
            (void)(selected->item.a_nfunc)();
        }
        free((genericptr_t)selected);
    }

    destroy_nhwindow(menuwin);
    return 0;
}

STATIC_OVL
void
add_command_menu_items(menuwin, cmdflag)
winid menuwin;
int cmdflag;
{
    register const struct ext_func_tab* efp;
    int n = 0;
    size_t maxcommandlength = 0;
    size_t maxdesclength = 0;

    for (efp = extcmdlist; efp->ef_txt; efp++)
    {
        if ((efp->flags & CMD_NOT_AVAILABLE) != 0 || (efp->flags & cmdflag) == 0)
            continue;

        if (strlen(efp->ef_txt) > maxcommandlength)
            maxcommandlength = strlen(efp->ef_txt);

        if (strlen(efp->ef_desc) > maxdesclength)
            maxdesclength = strlen(efp->ef_desc);
    }

    anything any;

    for (efp = extcmdlist; efp->ef_txt; efp++)
    {
        any = zeroany;
        char descbuf[BUFSZ] = "";
        char cmdbuf[BUFSZ] = "";
        char buf[BUFSZ] = "";

        int cmdchar = 'a' + n;
        size_t cmdlen = 0;

        any.a_nfunc = efp->ef_funct;

        strcpy(cmdbuf, efp->ef_txt);
        cmdlen = strlen(cmdbuf);
        if (cmdlen < maxcommandlength)
        {
            size_t i;
            for (i = 0; i < (maxcommandlength - cmdlen); i++)
                Sprintf(eos(cmdbuf), " ");
        }

        if ((efp->flags & CMD_NOT_AVAILABLE) != 0 || (efp->flags & cmdflag) == 0)
            continue;

        strcpy(descbuf, efp->ef_desc);
        *descbuf = highc(*descbuf);

#ifndef GNH_MOBILE
        char shortcutbuf[BUFSZ] = "";
        size_t desclen = 0;
        desclen = strlen(descbuf);
        if (desclen < maxdesclength)
        {
            for (size_t i = 0; i < (maxdesclength - desclen); i++)
                Sprintf(eos(descbuf), " ");
        }

        uchar altmask = 0x80;
        uchar ctrlmask = 0x20 | 0x40;

        if (efp->bound_key != '\0' && !(efp->bound_key >= (uchar)M(0) && efp->bound_key <= (uchar)M(9)))
            Sprintf(shortcutbuf, "  (%s%c)",
            (efp->bound_key & ctrlmask) == 0 ? "Ctrl-" : (efp->bound_key & altmask) == altmask ? "Alt-" : "",
                (efp->bound_key & ctrlmask) == 0 ? efp->bound_key | ctrlmask : (efp->bound_key & altmask) == altmask ? efp->bound_key & ~altmask : efp->bound_key);
        else
            strcpy(shortcutbuf, "");
#endif
        Sprintf(buf, "%s  %s", cmdbuf, descbuf);

        add_menu(menuwin, NO_GLYPH, &any, cmdchar, 0, ATR_INDENT_AT_DOUBLE_SPACE, NO_COLOR,
            buf, MENU_UNSELECTED);
        n++;
    }
}


void
youhiding(via_enlghtmt, msgflag)
boolean via_enlghtmt; /* englightment line vs topl message */
int msgflag;          /* for variant message phrasing */
{
    char *bp, buf[BUFSZ];

    Strcpy(buf, "hiding");
    if (U_AP_TYPE != M_AP_NOTHING) {
        /* mimic; hero is only able to mimic a strange object or gold
           or hallucinatory alternative to gold, so we skip the details
           for the hypothetical furniture and monster cases */
        bp = eos(strcpy(buf, "mimicking"));
        if (U_AP_TYPE == M_AP_OBJECT) {
            Sprintf(bp, " %s", an(simple_typename(youmonst.mappearance)));
        } else if (U_AP_TYPE == M_AP_FURNITURE) {
            Strcpy(bp, " something");
        } else if (U_AP_TYPE == M_AP_MONSTER) {
            Strcpy(bp, " someone");
        } else {
            ; /* something unexpected; leave 'buf' as-is */
        }
    } else if (u.uundetected) {
        bp = eos(buf); /* points past "hiding" */
        if (youmonst.data->mlet == S_EEL) {
            if (is_pool(u.ux, u.uy))
                Sprintf(bp, " in the %s", waterbody_name(u.ux, u.uy));
        } else if (hides_under(youmonst.data)) {
            struct obj *o = level.objects[u.ux][u.uy];

            if (o)
                Sprintf(bp, " underneath %s", ansimpleoname(o));
        } else if (is_clinger(youmonst.data) || Flying) {
            /* Flying: 'lurker above' hides on ceiling but doesn't cling */
            Sprintf(bp, " on the %s", ceiling(u.ux, u.uy));
        } else {
            /* on floor; is_hider() but otherwise not special: 'trapper' */
            if (u.utrap && u.utraptype == TT_PIT) {
                struct trap *t = t_at(u.ux, u.uy);

                Sprintf(bp, " in a %spit",
                        (t && t->ttyp == SPIKED_PIT) ? "spiked " : "");
            } else
                Sprintf(bp, " on the %s", surface(u.ux, u.uy));
        }
    } else {
        ; /* shouldn't happen; will result in generic "you are hiding" */
    }

    if (via_enlghtmt) {
        int final = msgflag; /* 'final' is used by you_are() macro */

        you_are(buf, "");
    } else {
        /* for dohide(), when player uses '#monster' command */
        You("are %s %s.", msgflag ? "already" : "now", buf);
    }
}

int
dodeletesavedgame(VOID_ARGS)
{
    if (wizard || discover || CasualMode)
    {
        boolean has_existing_save_file = check_existing_save_file();
        if (has_existing_save_file)
        {
            if (yn_query_ex(ATR_NONE, CLR_MSG_NEGATIVE, "Delete Save File", "Are you sure to delete the save file?") == 'y')
            {
                delete_savefile();
                pline1("Save file has been deleted.");
            }
            else
                pline1(Never_mind);
        }
        else
        {
            pline1("There is no save file to delete.");
        }
    }
    else
        pline(unavailcmd, visctrl((int)cmd_from_func(dodeletesavedgame)));

    return 0;
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
int
doconduct(VOID_ARGS)
{
    show_conduct(0);
    return 0;
}

void
show_conduct(final)
int final;
{
    char buf[BUFSZ];
    int ngenocided;
    int dumpwin;

    /* Create the conduct window */
    en_win = create_nhwindow_ex(NHW_MENU, GHWINDOW_STYLE_SEMI_WIDE_LIST, NO_GLYPH, zerocreatewindowinfo);

#if defined (DUMPLOG) || defined(DUMPHTML)
    if(iflags.in_dumplog)
        dumpwin = NHW_DUMPTXT;
    else
        dumpwin = en_win;
#else
    dumpwin = en_win;
#endif

    if (!u.uachieve.ascended || !u.uachieve.amulet || !u.uachieve.role_achievement)
    {
        putstr(en_win, ATR_TITLE, "Goals:");
        if (!final)
            putstr(en_win, ATR_HALF_SIZE, " ");

        if (!u.uachieve.amulet)
        {
            char goalbuf[BUFSZ];
            Sprintf(goalbuf, "on a mission to recover the Amulet of Yendor for %s", u_gname());
            you_are(goalbuf, "");
        }
        else if (!u.uachieve.ascended)
        {
            char goalbuf[BUFSZ];
            Sprintf(goalbuf, "on a mission to sacrifice the Amulet of Yendor on the high altar to %s on the Astral Plane", u_gname());
            you_are(goalbuf, "");
            if (!u.uachieve.entered_elemental_planes && !u.uachieve.entered_astral_plane)
            {
                Strcpy(goalbuf, "seeking to exit the Dungeons of Doom on level 1 in order to enter the Elemental Planes");
                you_are(goalbuf, "");
            }
        }
        if (!u.uachieve.role_achievement)
        {
            char goalbuf[BUFSZ];
            Sprintf(goalbuf, "an optional quest to %s", get_role_achievement_description(FALSE));
            you_have(goalbuf, "");
        }
    }

    int num_achievements = 0;
    putstr(dumpwin, ATR_NONE, "");
    putstr(en_win, ATR_TITLE, "Achievements:");
    if (!final)
        putstr(en_win, ATR_HALF_SIZE, " ");
    if (u.uachieve.ascended)
    {
        you_have("ascended to demigodhood", "");
        num_achievements++;
    }
    if (u.uachieve.amulet)
    {
        you_have("found the Amulet of Yendor", "");
        num_achievements++;
    }
    if (u.uachieve.crowned && u.uevent.uhand_of_elbereth > 0)
    {
        char achbuf[BUFSZ];
        Sprintf(achbuf, "become %s", hofe_titles[u.uevent.uhand_of_elbereth - 1]);
        you_have(achbuf, "");
        num_achievements++;
    }
    if (u.uachieve.role_achievement)
    {
        you_have(get_role_achievement_description(TRUE), "");
        num_achievements++;
    }
    if (u.uachieve.bell)
    {
        you_have("found the Bell of Opening", "");
        num_achievements++;
    }
    if (u.uachieve.book)
    {
        you_have("found the Book of the Dead", "");
        num_achievements++;
    }
    if (u.uachieve.menorah)
    {
        you_have("found the Candelabrum of Invocation", "");
        num_achievements++;
    }
    if (u.uachieve.prime_codex)
    {
        you_have("found the Prime Codex", "");
        num_achievements++;
    }
    if (u.uachieve.entered_astral_plane)
    {
        you_have("entered the Astral Plane", "");
        num_achievements++;
    }
    if (u.uachieve.entered_elemental_planes)
    {
        you_have("entered the Elemental Planes", "");
        num_achievements++;
    }
    if (u.uachieve.enter_gehennom)
    {
        you_have("entered Gehennom", "");
        num_achievements++;
    }
    if (u.uachieve.killed_medusa)
    {
        you_have("defeated Medusa", "");
        num_achievements++;
    }
    if (u.uachieve.consulted_oracle)
    {
        you_have("consulted the Oracle", "");
        num_achievements++;
    }
    if (u.uachieve.read_discworld_novel)
    {
        you_have("read a Discworld novel", "");
        num_achievements++;
    }
    if (u.uachieve.entered_gnomish_mines)
    {
        you_have("descended to the Gnomish Mines", "");
        num_achievements++;
    }
    if (u.uachieve.entered_mine_town)
    {
        you_have("visited Mine Town", "");
        num_achievements++;
    }
    if (u.uachieve.mines_luckstone)
    {
        you_have("found the Gladstone", "");
        num_achievements++;
    }
    if (u.uachieve.entered_shop)
    {
        you_have("visited a shop", "");
        num_achievements++;
    }
    if (u.uachieve.entered_temple)
    {
        you_have("visited a temple", "");
        num_achievements++;
    }
    if (u.uachieve.entered_sokoban)
    {
        you_have("found Sokoban", "");
        num_achievements++;
    }
    if (u.uachieve.finish_sokoban)
    {
        you_have("solved Sokoban", "");
        num_achievements++;
    }
    if (u.uachieve.entered_bigroom)
    {
        you_have("found the Big Room", "");
        num_achievements++;
    }
    if (u.uachieve.learned_castle_tune)
    {
        you_have("learned the castle tune", "");
        num_achievements++;
    }
    if (u.uachieve.entered_large_circular_dungeon)
    {
        you_have("entered the Large Circular Dungeon", "");
        num_achievements++;
    }
    if (u.uachieve.entered_plane_of_modron)
    {
        you_have("entered the Plane of the Modron", "");
        num_achievements++;
    }
    if (u.uachieve.entered_hellish_pastures)
    {
        you_have("entered Hellish Pastures", "");
        num_achievements++;
    }
    if (u.uachieve.killed_yacc)
    {
        you_have("defeated Yacc, the Demon Lord of Bovines", "");
        num_achievements++;
    }
    if (!num_achievements)
    {
        you_have_not("earned any achievements", "");
    }

    putstr(dumpwin, ATR_NONE, "");
    putstr(en_win, ATR_TITLE, "Voluntary challenges:");
    if(!final)
        putstr(en_win, ATR_HALF_SIZE, " ");

    if (u.uroleplay.blind)
        you_have_been("blind from birth");
    if (u.uroleplay.nudist)
        you_have_been("faithfully nudist");

    if (!u.uconduct.food)
        enl_msg(You_, "have gone", "went", " without food", "");
        /* but beverages are okay */
    else if (!u.uconduct.unvegan)
        you_have_X("followed a strict vegan diet");
    else if (!u.uconduct.unvegetarian)
        you_have_been("vegetarian");

    if (!u.uconduct.gnostic)
        you_have_been("an atheist");

    if (!u.uconduct.weaphit) {
        you_have_never("hit with a wielded weapon");
    } else if (wizard) {
        Sprintf(buf, "used a wielded weapon %ld time%s", u.uconduct.weaphit,
                plur(u.uconduct.weaphit));
        you_have_X(buf);
    }
    if (!u.uconduct.killer)
        you_have_been("a pacifist");

    if (!u.uconduct.literate) {
        you_have_been("illiterate");
    } else if (wizard) {
        Sprintf(buf, "read items or engraved %ld time%s", u.uconduct.literate,
                plur(u.uconduct.literate));
        you_have_X(buf);
    }

    ngenocided = num_genocides();
    if (ngenocided == 0) {
        you_have_never("genocided any monsters");
    } else {
        Sprintf(buf, "genocided %d type%s of monster%s", ngenocided,
                plur(ngenocided), plur(ngenocided));
        you_have_X(buf);
    }

    if (!u.uconduct.polypiles) {
        you_have_never("polymorphed an object");
    } else if (wizard) {
        Sprintf(buf, "polymorphed %ld item%s", u.uconduct.polypiles,
                plur(u.uconduct.polypiles));
        you_have_X(buf);
    }

    if (!u.uconduct.polyselfs) {
        you_have_never("changed form");
    } else if (wizard) {
        Sprintf(buf, "changed form %ld time%s", u.uconduct.polyselfs,
                plur(u.uconduct.polyselfs));
        you_have_X(buf);
    }

    if (!u.uconduct.wishes) {
        you_have_X("used no wishes");
    } else {
        Sprintf(buf, "used %ld wish%s", u.uconduct.wishes,
                (u.uconduct.wishes > 1L) ? "es" : "");
        if (u.uconduct.wisharti) {
            /* if wisharti == wishes
             *  1 wish (for an artifact)
             *  2 wishes (both for artifacts)
             *  N wishes (all for artifacts)
             * else (N is at least 2 in order to get here; M < N)
             *  N wishes (1 for an artifact)
             *  N wishes (M for artifacts)
             */
            if (u.uconduct.wisharti == u.uconduct.wishes)
                Sprintf(eos(buf), " (%s",
                        (u.uconduct.wisharti > 2L) ? "all "
                          : (u.uconduct.wisharti == 2L) ? "both " : "");
            else
                Sprintf(eos(buf), " (%ld ", u.uconduct.wisharti);

            Sprintf(eos(buf), "for %s)",
                    (u.uconduct.wisharti == 1L) ? "an artifact"
                                                : "artifacts");
        }
        you_have_X(buf);

        if (!u.uconduct.wisharti)
            enl_msg(You_, "have not wished", "did not wish",
                    " for any artifacts", "");
    }

    if (Role_if(PM_ROGUE))
    {
        char mbuf[BUFSZ];
        long valuableworth = money_cnt(invent) + hidden_gold() + carried_gem_value();
        putstr(dumpwin, ATR_NONE, "");
        Sprintf(mbuf, "You %s %ld %s worth of %svaluables with you.", final ? "had" : "have", valuableworth, currency(valuableworth), program_state.gameover ? "" : "known ");
        putstr(en_win, ATR_TITLE, mbuf);
    }
    else if (Role_if(PM_TOURIST))
    {
        putstr(dumpwin, ATR_NONE, "");
        putstr(en_win, ATR_TITLE, "Selfies taken with:");
        if (!final)
            putstr(en_win, ATR_HALF_SIZE, " ");

        print_selfies(en_win);
    }

    /* Pop up the window and wait for a key */
    display_nhwindow(en_win, TRUE);
    destroy_nhwindow(en_win);
    en_win = WIN_ERR;
}

/* ordered by command name */
struct ext_func_tab extcmdlist[] = {
    { '#', "#", "perform an extended command",
            doextcmd, IFBURIED | GENERALCMD },
    { M('?'), "?", "list all extended commands",
            doextlist, IFBURIED | GENERALCMD },
    { M('a'), "adjust", "adjust inventory letters",
            doorganize, IFBURIED | AUTOCOMPLETE | INCMDMENU },
    { M('A'), "annotate", "name current level",
            donamelevel, IFBURIED | AUTOCOMPLETE },
    { 'a', "apply", "apply (use) a tool (pick-axe, key, lamp...)",
            doapply, SINGLE_OBJ_CMD_SPECIFIC, 0, 0, "use or apply" },
    { 'A', "ability", "view and use skills and abilities",
            doability, IFBURIED | AUTOCOMPLETE },
    { M(2), "attributes", "show your attributes",
            doattributes, IFBURIED | AUTOCOMPLETE },
    { '@', "autopickup", "toggle the pickup option on/off",
            dotogglepickup, IFBURIED },
#if defined (USE_TILES) && !defined(GNH_MOBILE)
    { M('b'), "bars", "toggle tile hit point bars on/off",
            dotogglehpbars, IFBURIED | AUTOCOMPLETE },
#endif
    { C('b'), "break", "break something", dobreak, AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_GENERAL, 0, 0, "break" },
#if defined (USE_TILES) && !defined(GNH_MOBILE)
    { M('y'), "bufftimers", "toggle tile buff timers on/off",
            dotogglebufftimers, IFBURIED | AUTOCOMPLETE },
#endif
    { 'c', "close", "close a door", doclose },
    { 'C', "chat", "talk to someone", dotalk, IFBURIED | AUTOCOMPLETE },
    { M(10), "chatsteed", "talk to steed", dotalksteed, IFBURIED },
    { M(11), "chatnearby", "talk to someone nearby", dotalknearby, IFBURIED },
    { '\0',   "chronicle", "show journal of major events",
              do_gamelog, IFBURIED | AUTOCOMPLETE | GENERALCMD },
    { C('c'), "call", "call (name) something", docallcmd, IFBURIED | AUTOCOMPLETE, 0, getobj_callable, "call" },
    { M('c'), "commands", "list of additional actions",
            docommandmenu, IFBURIED | GENERALCMD | AUTOCOMPLETE },
    { M(3) /*M('c')*/, "conduct", "list voluntary challenges you have maintained",
            doconduct, IFBURIED | AUTOCOMPLETE },
    { '\0', "decorations", "toggle display of decorations on and off", dotoggledecorations, IFBURIED | AUTOCOMPLETE },
    { M(6), "deletesavedgame", "delete saved game if it exists",
            dodeletesavedgame, IFBURIED | CASUALMODECMD | AUTOCOMPLETE },
    { C('g'), "dig", "dig the ground", dodig, INCMDMENU },
    { M('d'), "dip", "dip an object into something", dodip, AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_GENERAL, 0, getobj_allowall, "dip" },
    { '>', "down", "go down a staircase", dodown },
    { 'd', "drop", "drop an item", dodrop, SINGLE_OBJ_CMD_GENERAL, 0, getobj_drop_types, "drop"},
    { '%', "dropmany", "drop many items", dodropmany },
    { 'D', "droptype", "drop specific item types", doddrop },
    { 'e', "eat", "eat something", doeat, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_allobj, "eat" },
    { 'E', "engrave", "engrave writing on the floor", doengrave, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_styluses, "write with" },
    { '\0', "enhance", "advance or check weapon and spell skills",
            enhance_weapon_skill, IFBURIED | AUTOCOMPLETE },
    { '\0', "exploremode", "enter explore (discovery) mode",
            enter_explore_mode, IFBURIED },
    { 'f', "fire", "fire ammunition from quiver", dofire, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_allowall, "fire" },
    { M('f'), "force", "force a lock", doforce, AUTOCOMPLETE | INCMDMENU },
    { ';', "glance", "show what type of thing a map symbol corresponds to",
            doquickwhatis, IFBURIED | GENERALCMD },
    { M('g'), "genocided", "list genocided monsters",
            dogenocidedmonsters, IFBURIED },
#if defined (USE_TILES) && !defined(GNH_MOBILE)
    { M('_'), "grid", "toggle tile grid on/off",
            dotogglegrid, IFBURIED | AUTOCOMPLETE },
#endif
    { M('h'), "handedness", "toggle swap weapon handedness",
            doswaphandedness, IFBURIED | AUTOCOMPLETE | INCMDMENU },
    { '?', "help", "give a help message", dohelp, IFBURIED | GENERALCMD },
    { '\0', "herecmdmenu", "show menu of commands you can do here",
            doherecmdmenu, IFBURIED },
    { 'i', "inventory", "show your inventory", ddoinv, IFBURIED },
    { 'I', "inventtype", "inventory specific item types",
            dotypeinv, IFBURIED },
    { M('i'), "invoke", "invoke an object's special powers",
            doinvoke, IFBURIED | AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_invoke_types, "invoke" },
    { M('j'), "jump", "jump to another location", dojump, AUTOCOMPLETE | INCMDMENU },
    { C('d'), "kick", "kick something", dokick, AUTOCOMPLETE | INCMDMENU },
    { M('k'), "killed", "list killed monsters",
            dokilledmonsters, IFBURIED },
    { '\\', "known", "show what object types have been discovered",
            dodiscovered, IFBURIED | GENERALCMD },
    { '`', "knownclass", "show discovered types for one class of objects",
            doclassdisco, IFBURIED | GENERALCMD },
    { M(0), "levelchange", "change experience level", /* Special hotkey launchable from GUI */
            wiz_level_change, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { C('l'), "light", "light or snuff out something around you", dolight, AUTOCOMPLETE | INCMDMENU },
    { ':', "look", "look at what is here", dolook, IFBURIED },
    { 'L', "lookat", "show what type of thing a symbol corresponds to",
            dowhatis, IFBURIED | GENERALCMD },
    { M('l'), "loot", "loot a box on the floor", doloot, AUTOCOMPLETE },
#ifdef DEBUG_MIGRATING_MONS
    { '\0', "migratemons", "migrate N random monsters",
            wiz_migrate_mons, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#endif
    { M(4), "monster", "use monster ability or skill",
            domonsterability, IFBURIED | AUTOCOMPLETE },
    { 'N', "name", "name a monster or an object",
            docallcmd, IFBURIED | AUTOCOMPLETE },
    { '\0', "namespecificitem", "name a specific object",
            doname_specific_object, IFBURIED | AUTOCOMPLETE | SINGLE_OBJ_CMD_GENERAL, 0, getobj_callable, "name", "name this specific object" },
    { '\0', "nameitemtype", "name a type of object",
            doname_type_of_object, IFBURIED | AUTOCOMPLETE | SINGLE_OBJ_CMD_GENERAL, 0, getobj_callable, "name the type for", "name the type of this object" },
    { M('o'), "offer", "offer a sacrifice to the gods",
            dosacrifice, AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_offerfodder, "sacrifice" },
    { 'o', "open", "open a door", doopen },
    { 'O', "options", "show option settings, possibly change them",
            doset, IFBURIED | GENERALCMD },
    { C('o'), "overview", "show a summary of the explored dungeon",
            dooverview, IFBURIED | AUTOCOMPLETE },
#if defined(DEBUG)
    { '\0', "panic", "test panic routine (fatal to game)",
            wiz_panic, IFBURIED },
#endif
    { 'p', "pay", "pay your shopping bill", dopay },
    { ',', "pickup", "pick up things at the current location", dopickup },
    { M(1), "polyself", "polymorph self", /* Special hotkey launchable from GUI */
            wiz_polyself, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { M('p'), "pray", "pray to the gods for help",
            dopray, IFBURIED | AUTOCOMPLETE | INCMDMENU },
    { C('p'), "prevmsg", "view recent game messages",
            doprev_message, IFBURIED | GENERALCMD },
    { 'P', "puton", "put on an accessory (ring, amulet, etc)", doputon, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_accessories, "put on", "put on" },
    { 'q', "quaff", "quaff (drink) something", dodrink, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_beverages, "drink", "drink" },
    { M('q'), "quit", "exit without saving current game",
            done2, IFBURIED | AUTOCOMPLETE | GENERALCMD },
    { 'Q', "quiver", "select ammunition for quiver", dowieldquiver, SINGLE_OBJ_CMD_SPECIFIC, 0, 0, "ready" },
    { 'r', "read", "read a scroll or spellbook", doread, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_readable, "read" },
    { C('r'), "redraw", "redraw screen", doredraw, IFBURIED | GENERALCMD },
    { 'R', "remove", "remove an accessory (ring, amulet, etc)", doremring, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_accessories, "remove" },
    { M('R'), "ride", "mount or dismount a saddled steed",
            doride, AUTOCOMPLETE },
    { M(12), "ridenearby", "mount or dismount a saddled steed nearby",
            doridenearby, 0 },
    { M('r'), "rub", "rub a lamp or a stone", dorub, AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_cuddly, "rub" },
    { 's', "search", "search for traps and secret doors",
            dosearch, IFBURIED, "searching" },
    { 'S', "skill", "view and advance skills",
            doskill, IFBURIED | AUTOCOMPLETE },
    { M('s'), "save", "save the game and exit", dosave, IFBURIED | AUTOCOMPLETE | GENERALCMD },
    { '*', "seeall", "show all equipment in use", doprinuse, IFBURIED },
    { AMULET_SYM, "seeamulet", "show the amulet currently worn",
            dopramulet, IFBURIED },
    { ARMOR_SYM, "seearmor", "show the armor currently worn",
            doprarm, IFBURIED },
    { GOLD_SYM, "seegold", "count your gold", doprgold, IFBURIED },
    { '\0', "seenv", "show seen vectors",
            wiz_show_seenv, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { RING_SYM, "seerings", "show the ring(s) currently worn",
            doprring, IFBURIED },
    { SPBOOK_SYM, "spellmenu", "spell main menu",
            dospellmainmenu, AUTOCOMPLETE | IFBURIED | INCMDMENU },
    { TOOL_SYM, "seetools", "show the tools currently in use",
            doprtool, IFBURIED },
    { '^', "seetrap", "show the type of adjacent trap", doidtrap, IFBURIED },
    { WEAPON_SYM, "seeweapon", "show the weapon currently wielded",
            doprwep, IFBURIED },
    { ILLOBJ_SYM, "seeworn", "show the currently worn items",
            doseeworn, IFBURIED },
    { '!', "shell", "do a shell escape",
            dosh_core, IFBURIED | GENERALCMD
#ifndef SHELL
                       | CMD_NOT_AVAILABLE
#endif /* SHELL */
    },
    { C('s'), "sit", "sit down", dosit, AUTOCOMPLETE | INCMDMENU },
    { M(7), "stash", "stash an item into a container", dostash, SINGLE_OBJ_CMD_GENERAL, 0, getobj_stash_objs, "stash", "stash into a container" },
    { '\0', "stats", "show memory statistics",
            wiz_show_stats, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { C('z'), "suspend", "suspend the game",
            dosuspend_core, IFBURIED | GENERALCMD
#ifndef SUSPEND
                            | CMD_NOT_AVAILABLE
#endif /* SUSPEND */
    },

    { 'T', "takeoff", "take off one piece of armor", dotakeoff, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_clothes, "take off", "take off" },
    { M('t')/*'A'*/, "takeoffall", "remove all armor", doddoremarm },
#if defined (USE_TILES) && !defined(GNH_MOBILE)
    { M(';'), "targeting", "toggle tile targeting graphics on/off",
            dotogglemonstertargeting, IFBURIED | AUTOCOMPLETE },
    { M(':'), "umark", "toggle tile player mark graphics on/off",
            dotoggleumark, IFBURIED | AUTOCOMPLETE },
#endif
    { C('t'), "teleport", "teleport around the level", dotelecmd, IFBURIED },
    { '\0', "terrain", "show map without obstructions",
            doterrain, IFBURIED | AUTOCOMPLETE },
    { '\0', "therecmdmenu",
            "menu of commands you can do from here to adjacent spot",
            dotherecmdmenu },
    { 't', "throw", "throw something", dothrow, SINGLE_OBJ_CMD_GENERAL, 0, getobj_toss_objs, "throw" },
    { '\0', "timeout", "look at timeout queue and hero's timed intrinsics",
            wiz_timeout_queue, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { M('T'), "tip", "empty a container", dotip, AUTOCOMPLETE | INCMDMENU | SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_tippables, "tip" },
    { '_', "travel", "travel to a specific location on the map", dotravel },
 //   { M('t'), "turn", "turn undead away", doturn, IFBURIED | AUTOCOMPLETE }, //Replaced by holy symbol
    { C('x'), "twoweapon", "toggle two-weapon combat",
            dotwoweapon, AUTOCOMPLETE | INCMDMENU },
    { M('u'), "untrap", "untrap something", dountrap, AUTOCOMPLETE },
    { '<', "up", "go up a staircase", doup },
    { M('<'), "lastpickeditem", "use the last picked item", dolastpickeditem, IFBURIED },
    { '\0', "vanquished", "list vanquished monsters",
            dovanquished, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { M('v'), "version",
            "list compile time options for this version of GnollHack",
            doextversion, IFBURIED | AUTOCOMPLETE | GENERALCMD },
    { 'v', "versionshort", "show version", doversion, IFBURIED | GENERALCMD },
    { 'V', "history", "show long version and game history",
            dohistory, IFBURIED | GENERALCMD },
    { '\0', "vision", "show vision array",
            wiz_show_vision, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '.', "wait", "rest one move while doing nothing",
            donull, IFBURIED, "waiting" },
    { 'W', "wear", "wear a piece of armor", dowear, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_clothes, "wear" },
    { '&', "whatdoes", "tell what a command does", dowhatdoes, IFBURIED },
    { '/', "whatis", "show what type of thing a symbol corresponds to",
            dowhatis, IFBURIED | GENERALCMD },
    { 'w', "wield", "wield (put in use) a weapon", dowield, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_wield_objs, "wield" },
    { M('w'), "wipe", "wipe off your face", dowipe, AUTOCOMPLETE | INCMDMENU },
    { 'x', "swap", "swap wielded and secondary weapons", doswapweapon_right_or_both, INCMDMENU },
    { M('x'), "examine", "describe an item", doitemdescriptions, IFBURIED | AUTOCOMPLETE | SINGLE_OBJ_CMD_INFO, 0, getobj_allobj, "examine" },
    { M(5), "unwield", "unwield a weapon", dounwield, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_unwield_objs, "unwield"},
    { '}', "you", "describe your character", docharacterstatistics, IFBURIED | AUTOCOMPLETE },
    { C('y'), "yell", "yell for your companions",
            doyell, IFBURIED | AUTOCOMPLETE | INCMDMENU },
    { 'z', "zap", "zap a wand", dozap, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_zap_syms, "zap" },
    { M(8), "markautostash", "mark a container as auto-stash", domarkautostash, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_mark_autostashs, "mark as auto-stash", "mark as auto-stash"  },
    { M(9), "unmarkautostash", "unmark a container as auto-stash", dounmarkautostash, SINGLE_OBJ_CMD_SPECIFIC, 0, getobj_unmark_autostashs, "unmark as auto-stash", "unmark as auto-stash" },
    { 'Z', "cast", "cast a spell", docast, AUTOCOMPLETE | IFBURIED | INSPELLMENU },
    { 'X', "mix", "prepare a spell from material components",
            domix, AUTOCOMPLETE | INSPELLMENU },
    { M('z'), "viewspell", "view spells", dospellview, IFBURIED | AUTOCOMPLETE | INSPELLMENU },
    { '\0', "managespell", "manage spells",
            dospellmanage, AUTOCOMPLETE | IFBURIED | INSPELLMENU },
    { '\0', "sortspells", "sort known spells",
            dosortspell, AUTOCOMPLETE | IFBURIED | INSPELLMENU },
    { '\0', "reorderspells", "reorder known spells",
            dovspell, AUTOCOMPLETE | IFBURIED | INSPELLMENU },
#if defined (USE_TILES) && !defined(GNH_MOBILE)
    { M('.'), "zoomnormal", "revert to normal zoom level",
            dozoomnormal, IFBURIED | AUTOCOMPLETE },
    { M('+'), "zoomin", "zoom map out",
            dozoomin, IFBURIED | AUTOCOMPLETE },
    { M('-'), "zoomout", "zoom map in",
            dozoomout, IFBURIED | AUTOCOMPLETE },
    { M(','), "zoommini", "zoom map to fit to screen",
            dozoommini, IFBURIED | AUTOCOMPLETE },
    { C(','), "zoomhalf", "zoom map out to 50% of normal",
            dozoomhalf, IFBURIED | AUTOCOMPLETE },
#endif
#ifdef GNH_MOBILE
    { '{', "viewpet", "view currently active pet",
            doviewpet, IFBURIED },
#endif

#ifdef DEBUG
    { '\0', "wizbury", "bury objs under and around you",
            wiz_debug_cmd_bury, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#endif
    { C('e'), "wizdetect", "reveal hidden things within a small radius",
            wiz_detect, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { M('m'), "wizgenesis", "create a monster",
            wiz_genesis, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { C('i'), "wizidentify", "identify all items in inventory",
            wiz_identify, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizintrinsic", "set an intrinsic",
            wiz_intrinsic, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { C('v'), "wizlevelport", "teleport to another level",
            wiz_level_tele, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizlightsources", "show mobile light sources",
            wiz_light_sources, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#if defined (DUMPLOG) || defined (DUMPHTML)
    { '\0', "wizdumplog", "write the dumplog",
            wiz_dumplog, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#endif
    { '\0', "wizmakemap", "recreate the current level",
            wiz_makemap, IFBURIED | WIZMODECMD },
    { C('f'), "wizmap", "map the level",
            wiz_map, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#if !defined(GNH_MOBILE) && defined(DEBUG)
    { '\0', "wizsavemon", "save monsters into a file",
            wiz_save_monsters, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsaveenc", "save encounters into a file",
            wiz_save_encounters, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsavetiledata", "save tile data into a file",
            wiz_save_tiledata, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizcounttiles", "count the number of tiles",
            wiz_count_tiles, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsaveglyph2tiles", "save glyph2tile into a file",
            wiz_save_glyph2tiles, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsavequesttexts", "save quest texts into a file",
            wiz_save_quest_texts, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsavespells", "save spells into a .md-file for wiki",
            wiz_save_spells, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
#endif
    { '\0', "wizcrown", "make the god crown you",
            wiz_crown, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizfiles", "browse through files in current directory",
            wiz_files, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizrumorcheck", "verify rumor boundaries",
            wiz_rumor_check, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizsmell", "smell monster",
            wiz_smell, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wizwhere", "show locations of special levels",
            wiz_where, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { C('w'), "wizwish", "wish for something",
            wiz_wish, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', "wmode", "show wall modes",
            wiz_show_wmodes, IFBURIED | AUTOCOMPLETE | WIZMODECMD },
    { '\0', (char *) 0, (char *) 0, donull, 0, (char *) 0 } /* sentinel */
};

int extcmdlist_length = SIZE(extcmdlist) - 1;

const char *
key2extcmddesc(key)
uchar key;
{
    if (Cmd.commands[key] && Cmd.commands[key]->ef_txt)
        return Cmd.commands[key]->ef_desc;
    return (char *) 0;
}

uchar
cmd_from_txt(command)
const char* command;
{
    if (!command)
        return '\0';

    int i;
    for (i = 0; i < 256; ++i)
        if (Cmd.commands[i] && Cmd.commands[i]->ef_txt && !strcmp(Cmd.commands[i]->ef_txt, command))
            return (uchar)i;

    return '\0';
}

int
ext_cmd_from_txt(ext_command)
const char* ext_command;
{
    if (!ext_command)
        return -1;

    int i;
    for (i = 0; extcmdlist[i].ef_txt != 0; i++)
        if (!strcmp(extcmdlist[i].ef_txt, ext_command))
            return i;

    return -1;
}


boolean
bind_key(key, command)
uchar key;
const char *command;
{
    struct ext_func_tab *extcmd;

    /* special case: "nothing" is reserved for unbinding */
    if (!strcmp(command, "nothing")) {
        Cmd.commands[key] = (struct ext_func_tab *) 0;
        return TRUE;
    }

    for (extcmd = extcmdlist; extcmd->ef_txt; extcmd++) {
        if (strcmp(command, extcmd->ef_txt))
            continue;
        Cmd.commands[key] = extcmd;
#if 0 /* silently accept key binding for unavailable command (!SHELL,&c) */
        if ((extcmd->flags & CMD_NOT_AVAILABLE) != 0) {
            char buf[BUFSZ];

            Sprintf(buf, cmdnotavail, extcmd->ef_txt);
            config_error_add("%s", buf);
        }
#endif
        return TRUE;
    }

    return FALSE;
}

/* initialize all keyboard commands */
void
commands_init(VOID_ARGS)
{
    struct ext_func_tab *extcmd;

    for (extcmd = extcmdlist; extcmd->ef_txt; extcmd++)
        if (extcmd->key)
            Cmd.commands[extcmd->key] = extcmd;

    //(void) bind_key(C('l'), "redraw"); /* if number_pad is set */
    /*       'b', 'B' : go sw */
    /*       'F' : fight (one time) */
    /*       'g', 'G' : multiple go */
    /*       'h', 'H' : go west */
    (void) bind_key('h',    "help"); /* if number_pad is set */
    (void) bind_key('j',    "jump"); /* if number_pad is on */
    /*       'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' move commands */
    (void) bind_key('k',    "kick"); /* if number_pad is on */
    (void) bind_key('l',    "loot"); /* if number_pad is on */
    (void) bind_key(C('n'), "annotate"); /* if number_pad is on */
    (void) bind_key(M('n'), "name");
    (void) bind_key(M('N'), "name");
    (void) bind_key('u',    "untrap"); /* if number_pad is on */

#ifdef USE_TILES
    (void) bind_key(C('0'), "zoommini");
    (void) bind_key(C('1'), "zoomnormal");
    (void) bind_key(C('.'), "zoomnormal");
    (void) bind_key(C('+'), "zoomin");
    (void) bind_key(C('-'), "zoomout");
#endif

    /* alt keys: */
    (void) bind_key(M('O'), "overview");
    (void) bind_key(M('1'), "handedness");
    (void) bind_key(M('2'), "twoweapon");
#ifdef USE_TILES
    (void) bind_key(M('3'), "bars");
    (void) bind_key(M('4'), "targeting");
    (void) bind_key(M('*'), "targeting");
#endif

    update_bindings_list();
}

int
dokeylist_putcmds(datawin, docount, cmdflags, exflags, keys_used)
winid datawin;
boolean docount;
int cmdflags, exflags;
boolean *keys_used; /* boolean keys_used[256] */
{
    int i;
    char buf[BUFSZ];
    char buf2[BUFSZ];
    int count = 0;

    for (i = 0; i < 256; i++) {
        const struct ext_func_tab *extcmd;
        uchar key = (uchar) i;

        if (keys_used[i])
            continue;
        if (key == ' ' && !flags.rest_on_space)
            continue;
        if ((extcmd = Cmd.commands[i]) != (struct ext_func_tab *) 0) {
            if ((cmdflags && !(extcmd->flags & cmdflags))
                || (exflags && (extcmd->flags & exflags)))
                continue;
            if (docount) {
                count++;
                continue;
            }
            Sprintf(buf, "%-8s %-12s %s", key2txt(key, buf2),
                    extcmd->ef_txt,
                    extcmd->ef_desc);
            putstr(datawin, 0, buf);
            keys_used[i] = TRUE;
        }
    }
    return count;
}

/* list all keys and their bindings, like dat/hh but dynamic */
void
dokeylist(VOID_ARGS)
{
    char buf[BUFSZ], buf2[BUFSZ];
    uchar key;
    boolean keys_used[256] = {0};
    winid datawin;
    int i;
    static const char
        run_desc[] = "Prefix: run until something very interesting is seen",
        forcefight_desc[] =
                     "Prefix: force fight even if you don't see a monster";
    static const struct {
        int nhkf;
        const char *desc;
        boolean numpad;
    } misc_keys[] = {
        { NHKF_ESC, "escape from the current query/action", FALSE },
        { NHKF_RUSH,
          "Prefix: rush until something interesting is seen", FALSE },
        { NHKF_RUN, run_desc, FALSE },
        { NHKF_RUN2, run_desc, TRUE },
        { NHKF_FIGHT, forcefight_desc, FALSE },
        { NHKF_FIGHT2, forcefight_desc, TRUE } ,
        { NHKF_NOPICKUP,
          "Prefix: move without picking up objects/fighting", FALSE },
        { NHKF_RUN_NOPICKUP,
          "Prefix: run without picking up objects/fighting", FALSE },
        { NHKF_DOINV, "view inventory", TRUE },
        { NHKF_DOSEEWORN, "view worn items", TRUE },
        { NHKF_REQMENU, "Prefix: request a menu", FALSE },
#ifdef REDO
        { NHKF_DOAGAIN , "re-do: perform the previous command again", FALSE },
        { NHKF_DOAGAIN2 , "alternate re-do: perform the previous command again", FALSE },
#endif
        { 0, (const char *) 0, FALSE }
    };

    datawin = create_nhwindow(NHW_TEXT);
    putstr(datawin, 0, "");
    putstr(datawin, 0, "            Full Current Key Bindings List");

    /* directional keys */
    putstr(datawin, 0, "");
    putstr(datawin, 0, "Directional keys:");
    show_direction_keys(datawin, '.', FALSE); /* '.'==self in direction grid */

    keys_used[(uchar) Cmd.move_NW] = keys_used[(uchar) Cmd.move_N]
        = keys_used[(uchar) Cmd.move_NE] = keys_used[(uchar) Cmd.move_W]
        = keys_used[(uchar) Cmd.move_E] = keys_used[(uchar) Cmd.move_SW]
        = keys_used[(uchar) Cmd.move_S] = keys_used[(uchar) Cmd.move_SE]
        = TRUE;

    if (!iflags.num_pad) {
        keys_used[(uchar) highc(Cmd.move_NW)]
            = keys_used[(uchar) highc(Cmd.move_N)]
            = keys_used[(uchar) highc(Cmd.move_NE)]
            = keys_used[(uchar) highc(Cmd.move_W)]
            = keys_used[(uchar) highc(Cmd.move_E)]
            = keys_used[(uchar) highc(Cmd.move_SW)]
            = keys_used[(uchar) highc(Cmd.move_S)]
            = keys_used[(uchar) highc(Cmd.move_SE)] = TRUE;
        keys_used[(uchar) C(Cmd.move_NW)]
            = keys_used[(uchar) C(Cmd.move_N)]
            = keys_used[(uchar) C(Cmd.move_NE)]
            = keys_used[(uchar) C(Cmd.move_W)]
            = keys_used[(uchar) C(Cmd.move_E)]
            = keys_used[(uchar) C(Cmd.move_SW)]
            = keys_used[(uchar) C(Cmd.move_S)]
            = keys_used[(uchar) C(Cmd.move_SE)] = TRUE;
        putstr(datawin, 0, "");
        putstr(datawin, 0,
          "Shift-<direction> will move in specified direction until you hit");
        putstr(datawin, 0, "        a wall or run into something.");
        putstr(datawin, 0,
          "Ctrl-<direction> will run in specified direction until something");
        putstr(datawin, 0, "        very interesting is seen.");
    }

    putstr(datawin, 0, "");
    putstr(datawin, 0, "Miscellaneous keys:");
    for (i = 0; misc_keys[i].desc; i++) {
        key = Cmd.spkeys[misc_keys[i].nhkf];
        if (key && ((misc_keys[i].numpad && iflags.num_pad)
                    || !misc_keys[i].numpad)) {
            keys_used[(uchar) key] = TRUE;
            Sprintf(buf, "%-8s %s", key2txt(key, buf2), misc_keys[i].desc);
            putstr(datawin, 0, buf);
        }
    }
#ifndef NO_SIGNAL
    putstr(datawin, 0, "^c       break out of GnollHack (SIGINT)");
    keys_used[(uchar) C('c')] = TRUE;
#endif

    putstr(datawin, 0, "");
    show_menu_controls(datawin, TRUE);

    if (dokeylist_putcmds(datawin, TRUE, GENERALCMD, WIZMODECMD, keys_used)) {
        putstr(datawin, 0, "");
        putstr(datawin, 0, "General commands:");
        (void) dokeylist_putcmds(datawin, FALSE, GENERALCMD, WIZMODECMD | CASUALMODECMD,
                                 keys_used);
    }

    if (dokeylist_putcmds(datawin, TRUE, 0, WIZMODECMD, keys_used)) {
        putstr(datawin, 0, "");
        putstr(datawin, 0, "Game commands:");
        (void) dokeylist_putcmds(datawin, FALSE, 0, WIZMODECMD | CASUALMODECMD, keys_used);
    }

    if (wizard
        && dokeylist_putcmds(datawin, TRUE, WIZMODECMD, 0, keys_used)) {
        putstr(datawin, 0, "");
        putstr(datawin, 0, "Wizard-mode commands:");
        (void) dokeylist_putcmds(datawin, FALSE, WIZMODECMD, 0, keys_used);
    }

    if ((wizard || discover || CasualMode)
        && dokeylist_putcmds(datawin, TRUE, CASUALMODECMD, 0, keys_used)) {
        putstr(datawin, 0, "");
        putstr(datawin, 0, "Casual-mode commands:");
        (void)dokeylist_putcmds(datawin, FALSE, CASUALMODECMD, 0, keys_used);
    }

    display_nhwindow(datawin, FALSE);
    destroy_nhwindow(datawin);
}

char
cmd_from_func(fn)
int NDECL((*fn));
{
    int i;

    for (i = 0; i < 256; ++i)
        if (Cmd.commands[i] && Cmd.commands[i]->ef_funct == fn)
            return (char) i;
    return '\0';
}

/*
 * wizard mode sanity_check code
 */

STATIC_VAR const char template[] = "%-27s  %4ld  %6zu";
STATIC_VAR const char stats_hdr[] = "                             count  bytes";
STATIC_VAR const char stats_sep[] = "---------------------------  ----- -------";

STATIC_OVL size_t
size_obj(otmp)
struct obj *otmp;
{
    size_t sz = sizeof (struct obj);

    if (otmp->oextra) {
        sz += sizeof (struct oextra);
        if (ONAME(otmp))
            sz += strlen(ONAME(otmp)) + 1;
        if (UONAME(otmp))
            sz += strlen(UONAME(otmp)) + 1;
        if (OMONST(otmp))
            sz += size_monst(OMONST(otmp), FALSE);
        if (OMID(otmp))
            sz += sizeof (unsigned);
        if (OLONG(otmp))
            sz += sizeof (long);
        if (OMAILCMD(otmp))
            sz += strlen(OMAILCMD(otmp)) + 1;
    }
    return sz;
}

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
struct obj *chain;
long *total_count;
size_t *total_size;
boolean top;
boolean recurse;
{
    long count;
    size_t size;
    struct obj *obj;

    for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
        if (top) {
            count++;
            size += size_obj(obj);
        }
        if (recurse && obj->cobj)
            count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
    }
    *total_count += count;
    *total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, force, total_count, total_size)
winid win;
const char *src;
struct obj *chain;
boolean force;
long *total_count;
size_t *total_size;
{
    char buf[BUFSZ];
    long count = 0L;
    size_t size = 0;

    count_obj(chain, &count, &size, TRUE, FALSE);

    if (count || size || force) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, src, count, size);
        putstr(win, 0, buf);
    }
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
winid win;
const char *src;
struct monst *chain;
long *total_count;
size_t *total_size;
{
    char buf[BUFSZ];
    long count = 0;
    size_t size = 0;
    struct monst *mon;

    for (mon = chain; mon; mon = mon->nmon)
        count_obj(mon->minvent, &count, &size, TRUE, FALSE);

    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, src, count, size);
        putstr(win, 0, buf);
    }
}

STATIC_OVL void
contained_stats(win, src, total_count, total_size)
winid win;
const char *src;
long *total_count;
size_t *total_size;
{
    char buf[BUFSZ];
    long count = 0;
    size_t size = 0;
    struct monst *mon;

    count_obj(invent, &count, &size, FALSE, TRUE);
    count_obj(fobj, &count, &size, FALSE, TRUE);
    count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
    count_obj(migrating_objs, &count, &size, FALSE, TRUE);
    /* DEADMONSTER check not required in this loop since they have no
     * inventory */
    for (mon = fmon; mon; mon = mon->nmon)
        count_obj(mon->minvent, &count, &size, FALSE, TRUE);
    for (mon = migrating_mons; mon; mon = mon->nmon)
        count_obj(mon->minvent, &count, &size, FALSE, TRUE);

    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, src, count, size);
        putstr(win, 0, buf);
    }
}

STATIC_OVL size_t
size_monst(mtmp, incl_wsegs)
struct monst *mtmp;
boolean incl_wsegs;
{
    size_t sz = sizeof (struct monst);

    if (mtmp->wormno && incl_wsegs)
        sz += size_wseg(mtmp);

    if (mtmp->mextra) {
        sz += sizeof (struct mextra);
        if (MNAME(mtmp))
            sz += strlen(MNAME(mtmp)) + 1;
        if (UMNAME(mtmp))
            sz += strlen(UMNAME(mtmp)) + 1;
        if (EGD(mtmp))
            sz += sizeof (struct egd);
        if (EPRI(mtmp))
            sz += sizeof (struct epri);
        if (ESMI(mtmp))
            sz += sizeof(struct esmi);
        if (ENPC(mtmp))
            sz += sizeof(struct enpc);
        if (ESHK(mtmp))
            sz += sizeof (struct eshk);
        if (EMIN(mtmp))
            sz += sizeof (struct emin);
        if (EDOG(mtmp))
            sz += sizeof (struct edog);
        if (MMONST(mtmp))
            sz += size_monst(MMONST(mtmp), FALSE);
        if (MOBJ(mtmp))
            sz += size_obj(MOBJ(mtmp));
        /* mextra->mcorpsenm doesn't point to more memory */
    }
    return sz;
}

STATIC_OVL void
mon_chain(win, src, chain, force, total_count, total_size)
winid win;
const char *src;
struct monst *chain;
boolean force;
long *total_count;
size_t *total_size;
{
    char buf[BUFSZ];
    long count = 0L;
    size_t size = 0;
    struct monst *mon;
    /* mon->wormno means something different for migrating_mons and mydogs */
    boolean incl_wsegs = !strcmpi(src, "fmon");

    for (mon = chain; mon; mon = mon->nmon) {
        count++;
        size += size_monst(mon, incl_wsegs);
    }
    if (count || size || force) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, src, count, size);
        putstr(win, 0, buf);
    }
}

STATIC_OVL void
misc_stats(win, total_count, total_size)
winid win;
long *total_count;
size_t *total_size;
{
    char buf[BUFSZ], hdrbuf[QBUFSZ];
    long count = 0L;
    size_t size = 0;
    int idx;
    struct trap *tt;
    struct damage *sd; /* shop damage */
    struct kinfo *k; /* delayed killer */
    struct cemetery *bi; /* bones info */

    /* traps and engravings are output unconditionally;
     * others only if nonzero
     */
    for (tt = ftrap; tt; tt = tt->ntrap) {
        ++count;
        size += sizeof *tt;
    }
    *total_count += count;
    *total_size += size;
    Sprintf(hdrbuf, "traps, size %ld", (long) sizeof (struct trap));
    Sprintf(buf, template, hdrbuf, count, size);
    putstr(win, 0, buf);

    count = 0L;
    size = 0;
    engr_stats("engravings, size %ld+text", hdrbuf, &count, &size);
    *total_count += count;
    *total_size += size;
    Sprintf(buf, template, hdrbuf, count, size);
    putstr(win, 0, buf);

    count = 0L;
    size = 0;
    light_stats("light sources, size %ld", hdrbuf, &count, &size);
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    sound_stats("sound sources, size %ld", hdrbuf, &count, &size);
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    timer_stats("timers, size %ld", hdrbuf, &count, &size);
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    for (sd = level.damagelist; sd; sd = sd->next) {
        ++count;
        size += sizeof *sd;
    }
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(hdrbuf, "shop damage, size %ld",
                (long) sizeof (struct damage));
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    region_stats("regions, size %ld+%ld*rect+N", hdrbuf, &count, &size);
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    for (k = killer.next; k; k = k->next) {
        ++count;
        size += sizeof *k;
    }
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(hdrbuf, "delayed killer%s, size %ld",
                plur(count), (long) sizeof (struct kinfo));
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    for (bi = level.bonesinfo; bi; bi = bi->next) {
        ++count;
        size += (long) sizeof *bi;
    }
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Sprintf(hdrbuf, "bones history, size %ld",
                (long) sizeof (struct cemetery));
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }

    count = 0L;
    size = 0;
    for (idx = 0; idx < NUM_OBJECTS; ++idx)
        if (objects[idx].oc_uname) {
            ++count;
            size += strlen(objects[idx].oc_uname) + 1;
        }
    if (count || size) {
        *total_count += count;
        *total_size += size;
        Strcpy(hdrbuf, "object type names, text");
        Sprintf(buf, template, hdrbuf, count, size);
        putstr(win, 0, buf);
    }
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
STATIC_OVL int
wiz_show_stats(VOID_ARGS)
{
    char buf[BUFSZ];
    winid win;
    long total_obj_count, total_mon_count, total_ovr_count, total_misc_count;
    size_t total_obj_size, total_mon_size, total_ovr_size, total_misc_size;
    
    win = create_nhwindow(NHW_TEXT);
    putstr(win, 0, "Current memory statistics:");

    total_obj_count = total_obj_size = 0L;
    putstr(win, 0, stats_hdr);
    Sprintf(buf, "  Objects, base size %ld", (long) sizeof (struct obj));
    putstr(win, 0, buf);
    obj_chain(win, "invent", invent, TRUE, &total_obj_count, &total_obj_size);
    obj_chain(win, "fobj", fobj, TRUE, &total_obj_count, &total_obj_size);
    obj_chain(win, "buried", level.buriedobjlist, FALSE,
              &total_obj_count, &total_obj_size);
    obj_chain(win, "migrating obj", migrating_objs, FALSE,
              &total_obj_count, &total_obj_size);
    obj_chain(win, "billobjs", billobjs, FALSE,
              &total_obj_count, &total_obj_size);
    obj_chain(win, "memoryobjs", memoryobjs, FALSE,
              &total_obj_count, &total_obj_size);
    mon_invent_chain(win, "minvent", fmon, &total_obj_count, &total_obj_size);
    mon_invent_chain(win, "migrating minvent", migrating_mons,
                     &total_obj_count, &total_obj_size);
    contained_stats(win, "contained", &total_obj_count, &total_obj_size);
    putstr(win, 0, stats_sep);
    Sprintf(buf, template, "  Obj total", total_obj_count, total_obj_size);
    putstr(win, 0, buf);

    total_mon_count = total_mon_size = 0L;
    putstr(win, 0, "");
    Sprintf(buf, "  Monsters, base size %ld", (long) sizeof (struct monst));
    putstr(win, 0, buf);
    mon_chain(win, "fmon", fmon, TRUE, &total_mon_count, &total_mon_size);
    mon_chain(win, "migrating", migrating_mons, FALSE,
              &total_mon_count, &total_mon_size);
    /* 'mydogs' is only valid during level change or end of game disclosure,
       but conceivably we've been called from within debugger at such time */
    if (mydogs) /* monsters accompanying hero */
        mon_chain(win, "mydogs", mydogs, FALSE,
                  &total_mon_count, &total_mon_size);
    putstr(win, 0, stats_sep);
    Sprintf(buf, template, "  Mon total", total_mon_count, total_mon_size);
    putstr(win, 0, buf);

    total_ovr_count = total_ovr_size = 0L;
    putstr(win, 0, "");
    putstr(win, 0, "  Overview");
    overview_stats(win, template, &total_ovr_count, &total_ovr_size);
    putstr(win, 0, stats_sep);
    Sprintf(buf, template, "  Over total", total_ovr_count, total_ovr_size);
    putstr(win, 0, buf);

    total_misc_count = total_misc_size = 0L;
    putstr(win, 0, "");
    putstr(win, 0, "  Miscellaneous");
    misc_stats(win, &total_misc_count, &total_misc_size);
    putstr(win, 0, stats_sep);
    Sprintf(buf, template, "  Misc total", total_misc_count, total_misc_size);
    putstr(win, 0, buf);

    putstr(win, 0, "");
    putstr(win, 0, stats_sep);
    Sprintf(buf, template, "  Grand total",
            (total_obj_count + total_mon_count
             + total_ovr_count + total_misc_count),
            (total_obj_size + total_mon_size
             + total_ovr_size + total_misc_size));
    putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
    show_borlandc_stats(win);
#endif

    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);
    return 0;
}

void
sanity_check(VOID_ARGS)
{
    obj_sanity_check();
    timer_sanity_check();
    mon_sanity_check();
    light_sources_sanity_check();
    sound_sources_sanity_check();
    bc_sanity_check();
}

#ifdef DEBUG_MIGRATING_MONS
STATIC_OVL int
wiz_migrate_mons()
{
    int mcount = 0;
    char inbuf[BUFSZ] = DUMMY;
    struct permonst *ptr;
    struct monst *mtmp;
    d_level tolevel;

    getlin_ex(GETLINE_NUMBERS_ONLY, ATR_NONE, NO_COLOR, "How many random monsters to migrate?", inbuf, (char*)0, "[0]", (char*)0);
    if (*inbuf == '\033')
        return 0;
    mcount = atoi(inbuf);
    if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
        return 0;
    while (mcount > 0) {
        if (Is_stronghold(&u.uz))
            assign_level(&tolevel, &valley_level);
        else
            get_level(&tolevel, depth(&u.uz) + 1);
        ptr = rndmonst();
        mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
        if (mtmp)
            migrate_to_level(mtmp, ledger_no(&tolevel), MIGR_RANDOM,
                             (coord *) 0);
        mcount--;
    }
    return 0;
}
#endif

#define unctrl(c) ((((c) & 0xE0) == 0) ? (0x60 | (c)) : (((c) & 0x80) != 0 && ((c) & 0x40) == 0) ? (0x20 | (0x7f & (c))) : (c))
#define unmeta(c) (0x7f & (c))

struct {
    int nhkf;
    char key;
    const char *name;
} const spkeys_binds[] = {
    { NHKF_ESC,              '\033', (char *) 0 }, /* no binding */
    { NHKF_DOAGAIN,          DOAGAIN, "repeat" },
    { NHKF_DOAGAIN2,         -89 /*'�'*/, "repeat.alternate" },
    { NHKF_REQMENU,          'm', "reqmenu" },
    { NHKF_RUN,              'G', "run" },
    { NHKF_RUN2,             '5', "run.numpad" },
    { NHKF_RUSH,             'g', "rush" },
    { NHKF_FIGHT,            'F', "fight" },
    { NHKF_FIGHT2,           '-', "fight.numpad" },
    { NHKF_NOPICKUP,         'm', "nopickup" },
    { NHKF_RUN_NOPICKUP,     'M', "run.nopickup" },
    { NHKF_DOINV,            '0', "doinv" },
    { NHKF_DOSEEWORN,        ']', "doseeworn" },
    { NHKF_TRAVEL,           CMD_TRAVEL, (char *) 0 }, /* no binding */
    { NHKF_TRAVEL_ATTACK,    CMD_TRAVEL_ATTACK, (char*)0 }, /* no binding */
    { NHKF_TRAVEL_WALK,      CMD_TRAVEL_WALK, (char*)0 }, /* no binding */
    { NHKF_CLICKFIRE,        CMD_CLICKFIRE, (char*)0 }, /* no binding */
    { NHKF_CLICKLOOK,        CMD_CLICKLOOK, (char *) 0 }, /* no binding */
    { NHKF_REDRAW,           C('r'), "redraw" },
    { NHKF_REDRAW2,          C('l'), "redraw.numpad" },
    { NHKF_GETDIR_SELF,      '.', "getdir.self" },
    { NHKF_GETDIR_SELF2,     's', "getdir.self2" },
    { NHKF_GETDIR_HELP,      '?', "getdir.help" },
    { NHKF_COUNT,            'n', "count" },
    { NHKF_GETPOS_SELF,      '@', "getpos.self" },
    { NHKF_GETPOS_PICK,      '.', "getpos.pick" },
    { NHKF_GETPOS_PICK_Q,    ',', "getpos.pick.quick" },
    { NHKF_GETPOS_PICK_O,    ';', "getpos.pick.once" },
    { NHKF_GETPOS_PICK_V,    ':', "getpos.pick.verbose" },
    { NHKF_GETPOS_SHOWVALID, '$', "getpos.valid" },
    { NHKF_GETPOS_AUTODESC,  '#', "getpos.autodescribe" },
    { NHKF_GETPOS_MON_NEXT,  'm', "getpos.mon.next" },
    { NHKF_GETPOS_MON_PREV,  'M', "getpos.mon.prev" },
    { NHKF_GETPOS_OBJ_NEXT,  'o', "getpos.obj.next" },
    { NHKF_GETPOS_OBJ_PREV,  'O', "getpos.obj.prev" },
    { NHKF_GETPOS_DOOR_NEXT, 'd', "getpos.door.next" },
    { NHKF_GETPOS_DOOR_PREV, 'D', "getpos.door.prev" },
    { NHKF_GETPOS_UNEX_NEXT, 'x', "getpos.unexplored.next" },
    { NHKF_GETPOS_UNEX_PREV, 'X', "getpos.unexplored.prev" },
    { NHKF_GETPOS_VALID_NEXT, 'z', "getpos.valid.next" },
    { NHKF_GETPOS_VALID_PREV, 'Z', "getpos.valid.prev" },
    { NHKF_GETPOS_INTERESTING_NEXT, 'a', "getpos.all.next" },
    { NHKF_GETPOS_INTERESTING_PREV, 'A', "getpos.all.prev" },
    { NHKF_GETPOS_HELP,      '?', "getpos.help" },
    { NHKF_GETPOS_LIMITVIEW, '"', "getpos.filter" },
    { NHKF_GETPOS_MOVESKIP,  '*', "getpos.moveskip" },
    { NHKF_GETPOS_MENU,      '!', "getpos.menu" }
};

boolean
bind_specialkey(key, command)
uchar key;
const char *command;
{
    int i;
    for (i = 0; i < SIZE(spkeys_binds); i++) {
        if (!spkeys_binds[i].name || strcmp(command, spkeys_binds[i].name))
            continue;
        Cmd.spkeys[spkeys_binds[i].nhkf] = key;
        return TRUE;
    }
    return FALSE;
}

/* returns a one-byte character from the text (it may massacre the txt
 * buffer) */
char
txt2key(txt)
char *txt;
{
    txt = trimspaces(txt);
    if (!*txt)
        return '\0';

    /* simple character */
    if (!txt[1])
        return txt[0];

    /* a few special entries */
    if (!strcmp(txt, "<enter>"))
        return '\n';
    if (!strcmp(txt, "<space>"))
        return ' ';
    if (!strcmp(txt, "<esc>"))
        return '\033';

    /* control and meta keys */
    switch (*txt) {
    case 'm': /* can be mx, Mx, m-x, M-x */
    case 'M':
        txt++;
        if (*txt == '-' && txt[1])
            txt++;
        if (txt[1])
            return '\0';
        return M(*txt);
    case 'c': /* can be cx, Cx, ^x, c-x, C-x, ^-x */
    case 'C':
    case '^':
        txt++;
        if (*txt == '-' && txt[1])
            txt++;
        if (txt[1])
            return '\0';
        return C(*txt);
    }

    /* ascii codes: must be three-digit decimal */
    if (*txt >= '0' && *txt <= '9') {
        uchar key = 0;
        int i;

        for (i = 0; i < 3; i++) {
            if (txt[i] < '0' || txt[i] > '9')
                return '\0';
            key = 10 * key + txt[i] - '0';
        }
        return key;
    }

    return '\0';
}

/* returns the text for a one-byte encoding;
 * must be shorter than a tab for proper formatting */
char *
key2txt(c, txt)
uchar c;
char *txt; /* sufficiently long buffer */
{
    /* should probably switch to "SPC", "ESC", "RET"
       since GnollHack's documentation uses ESC for <escape> */
    if (c == ' ')
        Sprintf(txt, "<space>");
    else if (c == '\033')
        Sprintf(txt, "<esc>");
    else if (c == '\n')
        Sprintf(txt, "<enter>");
    else if (c == '\177')
        Sprintf(txt, "<del>"); /* "<delete>" won't fit */
    else
        Strcpy(txt, visctrl((char) c));
    return txt;
}


void
parseautocomplete(autocomplete, condition)
char *autocomplete;
boolean condition;
{
    struct ext_func_tab *efp;
    register char *autoc;

    /* break off first autocomplete from the rest; parse the rest */
    if ((autoc = index(autocomplete, ',')) != 0
        || (autoc = index(autocomplete, ':')) != 0) {
        *autoc++ = '\0';
        parseautocomplete(autoc, condition);
    }

    /* strip leading and trailing white space */
    autocomplete = trimspaces(autocomplete);

    if (!*autocomplete)
        return;

    /* take off negation */
    if (*autocomplete == '!') {
        /* unlike most options, a leading "no" might actually be a part of
         * the extended command.  Thus you have to use ! */
        autocomplete++;
        autocomplete = trimspaces(autocomplete);
        condition = !condition;
    }

    /* find and modify the extended command */
    for (efp = extcmdlist; efp->ef_txt; efp++) {
        if (!strcmp(autocomplete, efp->ef_txt)) {
            if (condition)
                efp->flags |= AUTOCOMPLETE;
            else
                efp->flags &= ~AUTOCOMPLETE;
            return;
        }
    }

    /* not a real extended command */
    raw_printf("Bad autocomplete: invalid extended command '%s'.",
               autocomplete);
    wait_synch();
}

STATIC_VAR boolean backed_dir_cmd = FALSE;

/* called at startup and after number_pad is twiddled */
void
reset_commands(initial)
boolean initial;
{
    static const char sdir[] = "hykulnjb><",
                      sdir_swap_yz[] = "hzkulnjb><",
                      ndir[] = "47896321><",
                      ndir_phone_layout[] = "41236987><",
                      gnhdir[] = "4y8u6j2h><";
    static const int ylist[] = {
        'y', 'Y', C('y'), M('y'), M('Y'), M(C('y'))
    };
    static struct ext_func_tab *back_dir_cmd[8];
    const struct ext_func_tab *cmdtmp;
    boolean flagtemp;
    int c, i, updated = 0;

    if (initial)
    {
#ifdef ALTMETA
        alt_esc = FALSE;
#endif
        escape_sequence_key_start_allowed = FALSE;
        last_multi = 0;
        phead = ptail = shead = stail = 0;
        backed_dir_cmd = FALSE;
        en_via_menu = FALSE;
        special_effect_shown = FALSE;
        timed_occ_fn = 0;
        readchar_queue = "";
        memset((genericptr_t)&clicklook_cc, 0, sizeof(clicklook_cc));
        //memset((genericptr_t)&clickfire_cc, 0, sizeof(clickfire_cc));
        struct ext_func_tab* efp;
        for (efp = extcmdlist; efp->ef_txt; efp++)
            efp->bound_key = 0;
        Cmd.num_pad = FALSE;
        Cmd.pcHack_compat = Cmd.phone_layout = Cmd.swap_yz = FALSE;
        for (i = 0; i < SIZE(spkeys_binds); i++)
            Cmd.spkeys[spkeys_binds[i].nhkf] = spkeys_binds[i].key;
        commands_init();
        updated = 1;
    }
    else
    {

        if (backed_dir_cmd) 
        {
            for (i = 0; i < 8; i++)
            {
                Cmd.commands[(uchar) Cmd.dirchars[i]] = back_dir_cmd[i];
            }
        }

        /* basic num_pad */
        flagtemp = iflags.num_pad;
        if (flagtemp != Cmd.num_pad)
        {
            Cmd.num_pad = flagtemp;
            ++updated;
        }
        /* swap_yz mode (only applicable for !num_pad); intended for
           QWERTZ keyboard used in Central Europe, particularly Germany */
        flagtemp = (iflags.num_pad_mode & 1) ? !Cmd.num_pad : FALSE;
        if (flagtemp != Cmd.swap_yz)
        {
            Cmd.swap_yz = flagtemp;
            ++updated;
            /* Cmd.swap_yz has been toggled;
               perform the swap (or reverse previous one) */
            for (i = 0; i < SIZE(ylist); i++)
            {
                c = ylist[i] & 0xff;
                cmdtmp = Cmd.commands[c];              /* tmp = [y] */
                Cmd.commands[c] = Cmd.commands[c + 1]; /* [y] = [z] */
                Cmd.commands[c + 1] = cmdtmp;          /* [z] = tmp */
            }
        }

        flagtemp = (iflags.num_pad_mode & 4) ? !Cmd.num_pad : FALSE;
        if (flagtemp != Cmd.gnh_layout)
        {
            Cmd.gnh_layout = flagtemp;
            ++updated;
        }

        /* MSDOS compatibility mode (only applicable for num_pad) */
        flagtemp = (iflags.num_pad_mode & 1) ? Cmd.num_pad : FALSE;
        if (flagtemp != Cmd.pcHack_compat) 
        {
            Cmd.pcHack_compat = flagtemp;
            ++updated;
            /* pcHack_compat has been toggled */
            c = M('5') & 0xff;
            cmdtmp = Cmd.commands['5'];
            Cmd.commands['5'] = Cmd.commands[c];
            Cmd.commands[c] = cmdtmp;
            c = M('0') & 0xff;
            Cmd.commands[c] = Cmd.pcHack_compat ? Cmd.commands['I'] : 0;
        }
        /* phone keypad layout (only applicable for num_pad) */
        flagtemp = (iflags.num_pad_mode & 2) ? Cmd.num_pad : FALSE;
        if (flagtemp != Cmd.phone_layout) 
        {
            Cmd.phone_layout = flagtemp;
            ++updated;
            /* phone_layout has been toggled */
            for (i = 0; i < 3; i++) {
                c = '1' + i;             /* 1,2,3 <-> 7,8,9 */
                cmdtmp = Cmd.commands[c];              /* tmp = [1] */
                Cmd.commands[c] = Cmd.commands[c + 6]; /* [1] = [7] */
                Cmd.commands[c + 6] = cmdtmp;          /* [7] = tmp */
                c = (M('1') & 0xff) + i; /* M-1,M-2,M-3 <-> M-7,M-8,M-9 */
                cmdtmp = Cmd.commands[c];              /* tmp = [M-1] */
                Cmd.commands[c] = Cmd.commands[c + 6]; /* [M-1] = [M-7] */
                Cmd.commands[c + 6] = cmdtmp;          /* [M-7] = tmp */
            }
        }
    } /*?initial*/

    if (updated)
        Cmd.serialno++;
    Cmd.dirchars = !Cmd.num_pad
                       ? (Cmd.gnh_layout ? gnhdir : (!Cmd.swap_yz ? sdir : sdir_swap_yz))
                       : (!Cmd.phone_layout ? ndir : ndir_phone_layout);
    Cmd.alphadirchars = !Cmd.num_pad ? Cmd.dirchars : sdir;

    Cmd.move_W = Cmd.dirchars[0];
    Cmd.move_NW = Cmd.dirchars[1];
    Cmd.move_N = Cmd.dirchars[2];
    Cmd.move_NE = Cmd.dirchars[3];
    Cmd.move_E = Cmd.dirchars[4];
    Cmd.move_SE = Cmd.dirchars[5];
    Cmd.move_S = Cmd.dirchars[6];
    Cmd.move_SW = Cmd.dirchars[7];

    if (!initial)
    {
        for (i = 0; i < 8; i++)
        {
            back_dir_cmd[i] =
                (struct ext_func_tab *) Cmd.commands[(uchar) Cmd.dirchars[i]];
            Cmd.commands[(uchar) Cmd.dirchars[i]] = (struct ext_func_tab *) 0;
        }
        backed_dir_cmd = TRUE;
        for (i = 0; i < 8; i++)
            (void) bind_key(Cmd.dirchars[i], "nothing");

        update_bindings_list();
    }
}

void
update_bindings_list(VOID_ARGS)
{
    struct ext_func_tab* efp;
    for (efp = extcmdlist; efp->ef_txt; efp++) 
    {
        efp->bound_key = 0;
        if (efp->key && Cmd.commands[efp->key] == efp)
        {
            efp->bound_key = efp->key;
            continue;
        }

        int i;
        for (i = 0; i < 256; i++)
        {
            if (Cmd.commands[i] == efp)
            {
                efp->bound_key = i;
                break;
            }
        }
    }
}

/* non-movement commands which accept 'm' prefix to request menu operation */
STATIC_OVL boolean
accept_menu_prefix(cmd_func)
int NDECL((*cmd_func));
{
    if (cmd_func == dopickup || cmd_func == dotip
        /* eat, #offer, and apply tinning-kit all use floorfood() to pick
           an item on floor or in invent; 'm' skips picking from floor
           (ie, inventory only) rather than request use of menu operation */
        || cmd_func == doeat || cmd_func == dosacrifice || cmd_func == doapply
        /* 'm' for removing saddle from adjacent monster without checking
           for containers at <u.ux,u.uy> */
        || cmd_func == doloot
        /* travel: pop up a menu of interesting targets in view */
        || cmd_func == dotravel
        /* wizard mode ^V and ^T */
        || cmd_func == wiz_level_tele || cmd_func == dotelecmd
        /* 'm' prefix allowed for some extended commands */
        || cmd_func == doextcmd || cmd_func == doextlist)
        return TRUE;
    return FALSE;
}

char
randomkey(VOID_ARGS)
{
    static int i = 0;
    char c;

    switch (rn2(12)) {
    default: c = '\033'; break;
    case 0: c = '\n'; break;
    case 1:
    case 2:
    case 3:
    case 4: c = (char)(' ' + rn2((int)('~' - ' '))); break;
    case 5: c = '\t'; break;
    case 6: c = (char)('a' + rn2((int)('z' - 'a'))); break;
    case 7: c = (char)('A' + rn2((int)('Z' - 'A'))); break;
    case 8: c = extcmdlist[(i++) % SIZE(extcmdlist)].key; break;
    case 9: c = '#'; break;
    }

    return c;
}

void
random_response(buf, sz)
char *buf;
int sz;
{
    char c;
    int count = 0;

    for (;;) {
        c = randomkey();
        if (c == '\n')
            break;
        if (c == '\033') {
            count = 0;
            break;
        }
        if (count < sz - 1)
            buf[count++] = c;
    }
    buf[count] = '\0';
}

int
rnd_extcmd_idx(VOID_ARGS)
{
    return rn2(extcmdlist_length + 1) - 1;
}

int
ch2spkeys(c, start, end)
char c;
int start,end;
{
    int i;

    for (i = start; i <= end; i++)
        if (Cmd.spkeys[i] == c)
            return i;
    return NHKF_ESC;
}

void
rhack(cmd)
register char *cmd;
{
    int spkey;
    boolean prefix_seen, bad_command,
        firsttime = (cmd == 0);

    create_context_menu(CREATE_CONTEXT_MENU_NORMAL);
    update_here_window();
    issue_simple_gui_command(GUI_CMD_CLEAR_PET_DATA);

    //reset_all_monster_origin_coordinates();
    check_gui_special_effect();
    check_mobbed_hint();

    iflags.menu_requested = FALSE;
#ifdef SAFERHANGUP
    if (program_state.done_hup)
        end_of_input();
#endif
    if (firsttime) {
        context.nopick = 0;
        cmd = parse();
    }
    if (*cmd == Cmd.spkeys[NHKF_ESC]) {
#ifdef ANDROID
        quit_possible();
#endif
        context.move = FALSE;
        return;
    }
    if ((*cmd == DOAGAIN || *cmd == Cmd.spkeys[NHKF_DOAGAIN] || *cmd == Cmd.spkeys[NHKF_DOAGAIN2]) && !in_doagain && saveq[0]) {
        in_doagain = TRUE;
        stail = 0;
        rhack((char *) 0); /* read and execute command */
        in_doagain = FALSE;
        return;
    }
    /* Special case of *cmd == ' ' handled better below */
    if (!*cmd || *cmd == (char) 0377) {
        nhbell();
        context.move = FALSE;
        return; /* probably we just had an interrupt */
    }

    /* handle most movement commands */
    prefix_seen = FALSE;
    context.travel = context.travel1 = context.travel_mode = context.mv = context.run = 0;
    spkey = ch2spkeys(*cmd, NHKF_RUN, NHKF_CLICKLOOK);

    if (flags.prefer_fast_move)
    {
        switch (spkey) {
        case NHKF_RUSH:
            spkey = NHKF_RUN;
            break;
        case NHKF_RUN:
            spkey = NHKF_RUSH;
            break;
        case NHKF_NOPICKUP:
            spkey = NHKF_RUN_NOPICKUP;
            break;
        case NHKF_RUN_NOPICKUP:
            spkey = NHKF_NOPICKUP;
            break;
        }
    }

    switch (spkey) {
    case NHKF_RUSH:
        if (movecmd(cmd[1])) {
            context.run = 2;
            domove_attempting |= DOMOVE_RUSH;
        } else
            prefix_seen = TRUE;
        break;
    case NHKF_RUN2:
        if (!Cmd.num_pad)
            break;
        /*FALLTHRU*/
    case NHKF_RUN:
        if (movecmd(lowc(cmd[1]))) {
            context.run = 3;
            domove_attempting |= DOMOVE_RUSH;
        } else
            prefix_seen = TRUE;
        break;
    case NHKF_FIGHT2:
        if (!Cmd.num_pad)
            break;
        /*FALLTHRU*/
    /* Effects of movement commands and invisible monsters:
     * m: always move onto space (even if 'I' remembered)
     * F: always attack space (even if 'I' not remembered)
     * normal movement: attack if 'I', move otherwise.
     */
    case NHKF_FIGHT:
        if (movecmd(cmd[1])) {
            context.forcefight = 1;
            domove_attempting |= DOMOVE_WALK;
        } else
            prefix_seen = TRUE;
        break;
    case NHKF_NOPICKUP:
        if (movecmd(cmd[1]) || u.dz) {
            context.run = 0;
            context.nopick = 1;
            if (!u.dz)
                domove_attempting |= DOMOVE_WALK;
            else
                cmd[0] = cmd[1]; /* "m<" or "m>" */
        } else
            prefix_seen = TRUE;
        break;
    case NHKF_RUN_NOPICKUP:
        if (movecmd(lowc(cmd[1]))) {
            context.run = 1;
            context.nopick = 1;
            domove_attempting |= DOMOVE_RUSH;
        } else
            prefix_seen = TRUE;
        break;
    case NHKF_DOINV:
        if (!Cmd.num_pad)
            break;
        (void) ddoinv(); /* a convenience borrowed from the PC */
        context.move = FALSE;
        return;
    case NHKF_DOSEEWORN:
        if (!Cmd.num_pad)
            break;
        (void)doseeworn();
        context.move = FALSE;
        return;
    case NHKF_CLICKLOOK:
        if (iflags.clicklook) {
            context.move = FALSE;
            do_look(2, &clicklook_cc);
        }
        return;
    case NHKF_CLICKFIRE:
        if (iflags.clickfire) {
            //context.move = FALSE;
            int fireres = dofire();
            if (!fireres)
                readchar_queue = ""; //Prevent movement if firing failed.
        }
        return;
    case NHKF_TRAVEL:
    case NHKF_TRAVEL_ATTACK:
    case NHKF_TRAVEL_WALK:
        if (flags.travelcmd) {
            context.travel = 1;
            context.travel1 = 1;
            context.run = 8;
            context.nopick = (spkey == NHKF_TRAVEL ? 1 : 0);
            domove_attempting |= DOMOVE_RUSH;
            context.travel_mode = (spkey == NHKF_TRAVEL_WALK) ? TRAVEL_MODE_WALK : TRAVEL_MODE_NORMAL;
            context.tmid = 0;
            context.toid = 0;
            if (isok(u.tx, u.ty))
            {
                if (spkey == NHKF_TRAVEL_ATTACK)
                {
                    context.travel_mode = TRAVEL_MODE_ATTACK_AND_WALK;
                    struct monst* tmtmp = m_at(u.tx, u.ty);
                    if (tmtmp && canspotmon(tmtmp))
                    {
                        context.tmid = tmtmp->m_id;
                    }
                }
            }
            break;
        }
        /*FALLTHRU*/
    default:
        if (movecmd(*cmd))
        { /* ordinary movement */
            context.run = 0; /* only matters here if it was 8 */
            domove_attempting |= DOMOVE_WALK;
        } 
        else if (Cmd.gnh_layout ? ((!digit(*cmd) && movecmd(lowc(*cmd))) || (digit(unmeta(*cmd)) && movecmd(unmeta(*cmd)))) :
            movecmd(Cmd.num_pad ? unmeta(*cmd) : lowc(*cmd))) 
        {
            context.run = 1;
            domove_attempting |= DOMOVE_RUSH;
        } 
        else if (Cmd.gnh_layout ? (digit(unctrl(*cmd)) && movecmd(unctrl(*cmd))) :
            movecmd(unctrl(*cmd))) 
        {
            context.run = 3;
            domove_attempting |= DOMOVE_RUSH;
        }
        break;
    }

    /* some special prefix handling */
    /* overload 'm' prefix to mean "request a menu" */
    if (prefix_seen && cmd[0] == Cmd.spkeys[NHKF_REQMENU]) 
    {
        /* (for func_tab cast, see below) */
        const struct ext_func_tab *ft = Cmd.commands[cmd[1] & 0xff];
        int NDECL((*func)) = ft ? ((struct ext_func_tab *) ft)->ef_funct : 0;

        if (func && accept_menu_prefix(func)) 
        {
            iflags.menu_requested = TRUE;
            ++cmd;
        }
    }

    if (((domove_attempting & (DOMOVE_RUSH | DOMOVE_WALK)) != 0L)
                            && !context.travel && !dxdy_moveok()) {
        /* trying to move diagonally as a grid bug;
           this used to be treated by movecmd() as not being
           a movement attempt, but that didn't provide for any
           feedback and led to strangeness if the key pressed
           ('u' in particular) was overloaded for num_pad use */
        You_cant("get there from here...");
        context.run = 0;
        context.nopick = context.forcefight = FALSE;
        context.move = context.mv = FALSE;
        multi = 0;
        return;
    }

    if ((domove_attempting & DOMOVE_WALK) != 0L) {
        if (multi)
            context.mv = TRUE;
        domove();
        context.forcefight = 0;
        return;
    } else if ((domove_attempting & DOMOVE_RUSH) != 0L) {
        if (firsttime) {
            if (!multi)
                multi = max(COLNO, ROWNO);
            u.last_str_turn = 0;
        }
        context.mv = TRUE;
        domove();
        return;
    } else if (prefix_seen && cmd[1] == Cmd.spkeys[NHKF_ESC]) {
        /* <prefix><escape> */
        /* don't report "unknown command" for change of heart... */
        bad_command = FALSE;
    }
    else if ((*cmd == ' ' && !flags.rest_on_space) || *cmd == 13) // )
    {
        bad_command = FALSE;//TRUE; /* skip cmdlist[] loop */

    /* handle all other commands */
    } 
    else
    {
        register const struct ext_func_tab *tlist;
        int res = 0, NDECL((*func));

        /* current - use *cmd to directly index cmdlist array */
        if ((tlist = Cmd.commands[(unsigned char)(*cmd & 0xff)]) != 0)
        {
            if (!wizard && (tlist->flags & WIZMODECMD))
            {
                play_sfx_sound(SFX_GENERAL_CANNOT);
                You_cant_ex(ATR_NONE, CLR_MSG_FAIL, "do that!");
                res = 0;
            }
            else if (!(wizard || discover || CasualMode) && (tlist->flags & CASUALMODECMD))
            {
                play_sfx_sound(SFX_GENERAL_CANNOT);
                You_cant_ex(ATR_NONE, CLR_MSG_FAIL, "do that!");
                res = 0;
            }
            else if (u.uburied && !(tlist->flags & IFBURIED))
            {
                play_sfx_sound(SFX_GENERAL_CANNOT);
                You_cant_ex(ATR_NONE, CLR_MSG_FAIL, "do that while you are buried!");
                res = 0;
            } 
            else 
            {
                /* we discard 'const' because some compilers seem to have
                   trouble with the pointer passed to set_occupation() */
                func = ((struct ext_func_tab *) tlist)->ef_funct;
                if (func)
                {
                    context.first_time_cmd = TRUE;
                    if (tlist->f_text && !occupation && multi)
                        set_occupation(func, tlist->f_text, 0, 0, 0, multi);
                    res = (*func)(); /* perform the command */
                    context.first_time_cmd = FALSE;
                }
            }
            if (!res)
            {
                context.move = FALSE;
                multi = 0;
            }
            return;
        }
        /* if we reach here, cmd wasn't found in cmdlist[] */
        bad_command = TRUE;
    }

    if (bad_command)
    {
        char expcmd[20]; /* we expect 'cmd' to point to 1 or 2 chars */
        char c, c1 = cmd[1];

        expcmd[0] = '\0';
        while ((c = *cmd++) != '\0')
            Strcat(expcmd, visctrl(c)); /* add 1..4 chars plus terminator */

        if (!prefix_seen || !help_dir(c1, spkey, "Invalid direction key!"))
            Norep("Unknown command '%s'.", expcmd);
    }
    /* didn't move */
    context.move = FALSE;
    multi = 0;
    return;
}


STATIC_OVL void
check_gui_special_effect()
{
    if (u.uz.dnum != mines_dnum && depth(&u.uz) > GUI_SPECIAL_EFFECT_LEVEL_DEPTH_THRESHOLD)
    {
        if (!special_effect_shown)
        {
            struct special_view_info info = { 0 };
            info.viewtype = SPECIAL_VIEW_SHOW_SPECIAL_EFFECT;
            info.text = 0;
            (void)open_special_view(info);
            special_effect_shown = TRUE;
        }
    }
}

/* convert an x,y pair into a direction code */
int
xytod(x, y)
schar x, y;
{
    register int dd;

    for (dd = 0; dd < 8; dd++)
        if (x == xdir[dd] && y == ydir[dd])
            return dd;
    return -1;
}

/* convert a direction code into an x,y pair */
void
dtoxy(cc, dd)
coord *cc;
register int dd;
{
    cc->x = xdir[dd];
    cc->y = ydir[dd];
    return;
}

/* also sets u.dz, but returns false for <> */
int
movecmd(sym)
char sym;
{
    register const char *dp = index(Cmd.dirchars, sym);

    u.dz = 0;
    if (!dp || !*dp)
        return 0;
    u.dx = xdir[dp - Cmd.dirchars];
    u.dy = ydir[dp - Cmd.dirchars];
    u.dz = zdir[dp - Cmd.dirchars];
#if 0 /* now handled elsewhere */
    if (u.dx && u.dy && NODIAG(u.umonnum)) {
        u.dx = u.dy = 0;
        return 0;
    }
#endif
    return !u.dz;
}

/* grid bug handling which used to be in movecmd() */
int
dxdy_moveok(VOID_ARGS)
{
    if (u.dx && u.dy && NODIAG(u.umonnum))
        u.dx = u.dy = 0;
    return u.dx || u.dy;
}

/* decide whether a character (user input keystroke) requests screen repaint */
boolean
redraw_cmd(c)
char c;
{
    return (boolean) (c == Cmd.spkeys[NHKF_REDRAW]
                      || (Cmd.num_pad && c == Cmd.spkeys[NHKF_REDRAW2]));
}

boolean
prefix_cmd(c)
char c;
{
    return (c == Cmd.spkeys[NHKF_RUSH]
            || c == Cmd.spkeys[NHKF_RUN]
            || c == Cmd.spkeys[NHKF_NOPICKUP]
            || c == Cmd.spkeys[NHKF_RUN_NOPICKUP]
            || c == Cmd.spkeys[NHKF_FIGHT]
            || (Cmd.num_pad && (c == Cmd.spkeys[NHKF_RUN2]
                                || c == Cmd.spkeys[NHKF_FIGHT2])));
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int
get_adjacent_loc(prompt, emsg, x, y, cc)
const char *prompt, *emsg;
xchar x, y;
coord *cc;
{
    xchar new_x, new_y;
    if (!getdir(prompt)) {
        pline1(Never_mind);
        return 0;
    }
    new_x = x + u.dx;
    new_y = y + u.dy;
    if (cc && isok(new_x, new_y)) {
        cc->x = new_x;
        cc->y = new_y;
    } else {
        if (emsg)
            pline1(emsg);
        return 0;
    }
    return 1;
}

int
getdir(s)
const char *s;
{
    char dirsym;
    int is_mov;

retry:
    create_context_menu(CREATE_CONTEXT_MENU_IN_GETDIR);
    escape_sequence_key_start_allowed = 1;
    if (in_doagain || *readchar_queue)
        dirsym = readchar();
    else
        dirsym = yn_function((s && *s != '^') ? s : "In what direction?",
                             (char *) 0, '\0', (char*)0);
    
    escape_sequence_key_start_allowed = 0;
    /* remove the prompt string so caller won't have to */
    clear_nhwindow(WIN_MESSAGE);

    if (redraw_cmd(dirsym)) 
    { /* ^R */
        docrt();              /* redraw */
        goto retry;
    }
    savech(dirsym);

    if (dirsym == Cmd.spkeys[NHKF_GETDIR_SELF]
        || dirsym == Cmd.spkeys[NHKF_GETDIR_SELF2])
    {
        u.dx = u.dy = u.dz = 0;
    } else if (!(is_mov = movecmd(dirsym)) && !u.dz) 
    {
        boolean did_help = FALSE, help_requested;

        if (!index(quitchars, dirsym))
        {
            help_requested = (dirsym == Cmd.spkeys[NHKF_GETDIR_HELP]);
            if (help_requested || iflags.cmdassist) 
            {
                did_help = help_dir((s && *s == '^') ? dirsym : '\0',
                                    NHKF_ESC,
                                    help_requested ? (const char *) 0
                                                  : "Invalid direction key!");
                if (help_requested)
                    goto retry;
            }
            if (!did_help)
                pline("What a strange direction!");
        }
        create_context_menu(CREATE_CONTEXT_MENU_NORMAL);
        return 0;
    } 
    else if (is_mov && !dxdy_moveok()) 
    {
        You_cant("orient yourself that direction.");
        create_context_menu(CREATE_CONTEXT_MENU_NORMAL);
        return 0;
    }
    if (!u.dz && (Stunned || (Confusion && !rn2(5))))
        confdir();

    create_context_menu(CREATE_CONTEXT_MENU_NORMAL);
    return 1;
}

STATIC_OVL void
show_direction_keys(win, centerchar, nodiag)
winid win; /* should specify a window which is using a fixed-width font... */
char centerchar; /* '.' or '@' or ' ' */
boolean nodiag;
{
    char buf[BUFSZ];

    if (!centerchar)
        centerchar = ' ';

    if (nodiag) {
        Sprintf(buf, "             %c   ", Cmd.move_N);
        putstr(win, 0, buf);
        putstr(win, 0, "             |   ");
        Sprintf(buf, "          %c- %c -%c",
                Cmd.move_W, centerchar, Cmd.move_E);
        putstr(win, 0, buf);
        putstr(win, 0, "             |   ");
        Sprintf(buf, "             %c   ", Cmd.move_S);
        putstr(win, 0, buf);
    } else {
        Sprintf(buf, "          %c  %c  %c",
                Cmd.move_NW, Cmd.move_N, Cmd.move_NE);
        putstr(win, 0, buf);
        putstr(win, 0, "           \\ | / ");
        Sprintf(buf, "          %c- %c -%c",
                Cmd.move_W, centerchar, Cmd.move_E);
        putstr(win, 0, buf);
        putstr(win, 0, "           / | \\ ");
        Sprintf(buf, "          %c  %c  %c",
                Cmd.move_SW, Cmd.move_S, Cmd.move_SE);
        putstr(win, 0, buf);
    };
}

/* explain choices if player has asked for getdir() help or has given
   an invalid direction after a prefix key ('F', 'g', 'm', &c), which
   might be bogus but could be up, down, or self when not applicable */
STATIC_OVL boolean
help_dir(sym, spkey, msg)
char sym;
int spkey; /* NHKF_ code for prefix key, if one was used, or for ESC */
const char *msg;
{
    static const char wiz_only_list[] = "EFGIVW";
    char ctrl;
    winid win;
    char buf[BUFSZ], buf2[BUFSZ], *explain;
    const char *dothat, *how;
    boolean prefixhandling, viawindow;

    /* NHKF_ESC indicates that player asked for help at getdir prompt */
    viawindow = (spkey == NHKF_ESC || iflags.cmdassist);
    prefixhandling = (spkey != NHKF_ESC);
    /*
     * Handling for prefix keys that don't want special directions.
     * Delivered via pline if 'cmdassist' is off, or instead of the
     * general message if it's on.
     */
    dothat = "do that";
    how = " at"; /* for "<action> at yourself"; not used for up/down */
    switch (spkey) 
    {
    case NHKF_NOPICKUP:
        dothat = "move";
        break;
    case NHKF_RUSH:
        dothat = "rush";
        break;
    case NHKF_RUN2:
        if (!Cmd.num_pad)
            break;
        /*FALLTHRU*/
    case NHKF_RUN:
    case NHKF_RUN_NOPICKUP:
        dothat = "run";
        break;
    case NHKF_FIGHT2:
        if (!Cmd.num_pad)
            break;
        /*FALLTHRU*/
    case NHKF_FIGHT:
        dothat = "fight";
        how = ""; /* avoid "fight at yourself" */
        break;
    default:
        prefixhandling = FALSE;
        break;
    }

    buf[0] = '\0';
    /* for movement prefix followed by '.' or (numpad && 's') to mean 'self';
       note: '-' for hands (inventory form of 'self') is not handled here */
    if (prefixhandling
        && (sym == Cmd.spkeys[NHKF_GETDIR_SELF]
            || (Cmd.num_pad && sym == Cmd.spkeys[NHKF_GETDIR_SELF2]))) 
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        Sprintf(buf, "You can't %s%s yourself.", dothat, how);
    /* for movement prefix followed by up or down */
    } 
    else if (prefixhandling && (sym == '<' || sym == '>')) 
    {
        play_sfx_sound(SFX_GENERAL_CANNOT);
        Sprintf(buf, "You can't %s %s.", dothat,
                /* was "upwards" and "downwards", but they're considered
                   to be variants of canonical "upward" and "downward" */
                (sym == '<') ? "upward" : "downward");
    }

    /* if '!cmdassist', display via pline() and we're done (note: asking
       for help at getdir() prompt forces cmdassist for this operation) */
    if (!viawindow) 
    {
        if (prefixhandling) 
        {
            if (!*buf)
                Sprintf(buf, "Invalid direction for '%s' prefix.",
                        visctrl(Cmd.spkeys[spkey]));
            pline_ex(ATR_NONE, CLR_MSG_FAIL, "%s", buf);
            return TRUE;
        }
        /* when 'cmdassist' is off and caller doesn't insist, do nothing */
        return FALSE;
    }

    /* open_special_view may open a more appropriate screen; if not, do a normal window */
    struct special_view_info info = { 0 };
    info.viewtype = SPECIAL_VIEW_HELP_DIR;
    info.text = *buf ? buf : msg;
    info.title = "Invalid Direction";
    if (!open_special_view(info))
    {

        win = create_nhwindow(NHW_TEXT);
        if (!win)
            return FALSE;

        if (*buf) 
        {
            /* show bad-prefix message instead of general invalid-direction one */
            putstr(win, 0, buf);
            putstr(win, 0, "");
        }
        else if (msg) 
        {
            Sprintf(buf, "cmdassist: %s", msg);
            putstr(win, 0, buf);
            putstr(win, 0, "");
        }

        if (!prefixhandling && (letter(sym) || sym == '[')) 
        {
            /* '[': old 'cmdhelp' showed ESC as ^[ */
            sym = highc(sym); /* @A-Z[ (note: letter() accepts '@') */
            ctrl = (sym - 'A') + 1; /* 0-27 (note: 28-31 aren't applicable) */
            if ((explain = dowhatdoes_core(ctrl, buf2)) != 0
                && (!index(wiz_only_list, sym) || wizard)) 
            {
                Sprintf(buf, "Are you trying to use ^%c%s?", sym,
                    index(wiz_only_list, sym) ? ""
                    : " as specified in the Guidebook");
                putstr(win, 0, buf);
                putstr(win, 0, "");
                putstr(win, 0, explain);
                putstr(win, 0, "");
                putstr(win, 0,
                    "To use that command, hold down the <Ctrl> key as a shift");
                Sprintf(buf, "and press the <%c> key.", sym);
                putstr(win, 0, buf);
                putstr(win, 0, "");
            }
        }

        Sprintf(buf, "Valid direction keys%s%s%s are:",
            prefixhandling ? " to " : "", prefixhandling ? dothat : "",
            NODIAG(u.umonnum) ? " in your current form" : "");
        putstr(win, 0, buf);
        show_direction_keys(win, !prefixhandling ? '.' : ' ', NODIAG(u.umonnum));

        if (!prefixhandling || spkey == NHKF_NOPICKUP) 
        {
            /* NOPICKUP: unlike the other prefix keys, 'm' allows up/down for
               stair traversal; we won't get here when "m<" or "m>" has been
               given but we include up and down for 'm'+invalid_direction;
               self is excluded as a viable direction for every prefix */
            putstr(win, 0, "");
            putstr(win, 0, "          <  up");
            putstr(win, 0, "          >  down");
            if (!prefixhandling) {
                int selfi = Cmd.num_pad ? NHKF_GETDIR_SELF2 : NHKF_GETDIR_SELF;

                Sprintf(buf, "       %4s  direct at yourself",
                    visctrl(Cmd.spkeys[selfi]));
                putstr(win, 0, buf);
            }
        }

        if (msg) 
        {
            /* non-null msg means that this wasn't an explicit user request */
            putstr(win, 0, "");
            putstr(win, 0,
                "(Suppress this message with !cmdassist in config file.)");
        }
        display_nhwindow(win, FALSE);
        destroy_nhwindow(win);
    }
    return TRUE;
}

void
confdir(VOID_ARGS)
{
    register int x = NODIAG(u.umonnum) ? 2 * rn2(4) : rn2(8);

    u.dx = xdir[x];
    u.dy = ydir[x];
    return;
}

const char *
directionname(dir)
int dir;
{
    static NEARDATA const char *const dirnames[] = {
        "west",      "northwest", "north",     "northeast", "east",
        "southeast", "south",     "southwest", "down",      "up",
    };

    static NEARDATA const char invalidname[] = "invalid";

    if (dir < 0 || dir >= SIZE(dirnames))
        return invalidname;
    return dirnames[dir];
}

int
isok(x, y)
register int x, y;
{
    /* x corresponds to curx, so x==1 is the first column. Ach. %% */
    return x >= 1 && x <= COLNO - 1 && y >= 0 && y <= ROWNO - 1;
}

/* #herecmdmenu command */
STATIC_PTR int
doherecmdmenu(VOID_ARGS)
{
    char ch = here_cmd_menu(TRUE);

    return ch ? 1 : 0;
}

/* #therecmdmenu command, a way to test there_cmd_menu without mouse */
STATIC_PTR int
dotherecmdmenu(VOID_ARGS)
{
    char ch;

    if (!getdir((const char *) 0) || !isok(u.ux + u.dx, u.uy + u.dy))
        return 0;

    if (u.dx || u.dy)
        ch = there_cmd_menu(TRUE, u.ux + u.dx, u.uy + u.dy);
    else
        ch = here_cmd_menu(TRUE);

    return ch ? 1 : 0;
}

STATIC_OVL void
add_herecmd_menuitem(win, func, text)
winid win;
int NDECL((*func));
const char *text;
{
    char ch;
    anything any;

    if ((ch = cmd_from_func(func)) != '\0') {
        any = zeroany;
        any.a_nfunc = func;
        add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, NO_COLOR, text, MENU_UNSELECTED);
    }
}

STATIC_OVL char
there_cmd_menu(doit, x, y)
boolean doit;
int x, y;
{
    winid win;
    char ch;
    char buf[BUFSZ];
    schar typ = levl[x][y].typ;
    int npick, K = 0;
    menu_item *picks = (menu_item *) 0;
    struct trap *ttmp;
    struct monst *mtmp;

    win = create_nhwindow(NHW_MENU);
    start_menu_ex(win, GHMENU_STYLE_CHOOSE_COMMAND);

    if (IS_DOOR(typ)) {
        boolean key_or_pick, card;
        int dm = levl[x][y].doormask;

        if ((dm & (D_CLOSED | D_LOCKED))) {
            add_herecmd_menuitem(win, doopen, "Open the door"), ++K;
            /* unfortunately there's no lknown flag for doors to
               remember the locked/unlocked state */
            key_or_pick = (carrying(SKELETON_KEY) || carrying(LOCK_PICK));
            card = (carrying(CREDIT_CARD) != 0);
            if (key_or_pick || card) {
                Sprintf(buf, "%sunlock the door",
                        key_or_pick ? "lock or " : "");
                add_herecmd_menuitem(win, doapply, upstart(buf)), ++K;
            }
            /* unfortunately there's no tknown flag for doors (or chests)
               to remember whether a trap had been found */
            add_herecmd_menuitem(win, dountrap,
                                 "Search the door for a trap"), ++K;
            /* [what about #force?] */
            add_herecmd_menuitem(win, dokick, "Kick the door"), ++K;
        } else if ((dm & D_ISOPEN)) {
            add_herecmd_menuitem(win, doclose, "Close the door"), ++K;
        }
    }

    if (typ <= SCORR)
        add_herecmd_menuitem(win, dosearch, "Search for secret doors"), ++K;

    if ((ttmp = t_at(x, y)) != 0 && ttmp->tseen) {
        char tbuf[BUFSZ];
        Sprintf(tbuf, "Examine %s", trap_type_definitions[ttmp->ttyp].type_name);
        add_herecmd_menuitem(win, doidtrap, tbuf), ++K;
        if (!(trap_type_definitions[ttmp->ttyp].tdflags & TRAPDEF_FLAGS_NOT_DISARMABLE))
        {
            Sprintf(tbuf, "Attempt to disarm the %s", trap_type_definitions[ttmp->ttyp].type_name);
            add_herecmd_menuitem(win, dountrap, tbuf), ++K;
        }
    }

    mtmp = m_at(x, y);
    if (mtmp && !canspotmon(mtmp))
        mtmp = 0;
    if (mtmp && which_armor(mtmp, W_SADDLE)) {
        char *mnam = x_monnam(mtmp, ARTICLE_THE, (char *) 0,
                              SUPPRESS_SADDLE, FALSE);

        if (!u.usteed) {
            Sprintf(buf, "Ride %s", mnam);
            add_herecmd_menuitem(win, doride, buf), ++K;
        }
        Sprintf(buf, "Remove saddle from %s", mnam);
        add_herecmd_menuitem(win, doloot, buf), ++K;
    }
    if (mtmp && can_saddle(mtmp) && !which_armor(mtmp, W_SADDLE)
        && carrying(SADDLE)) {
        Sprintf(buf, "Put saddle on %s", mon_nam(mtmp)), ++K;
        add_herecmd_menuitem(win, doapply, buf);
    }
#if 0
    if (mtmp || glyph_is_invisible(glyph_at(x, y))) {
        /* "Attack %s", mtmp ? mon_nam(mtmp) : "unseen creature" */
    } else {
        /* "Move %s", direction */
    }
#endif

    if (K) {
        end_menu(win, "What do you want to do?");
        npick = select_menu(win, PICK_ONE, &picks);
    } else {
        pline("No applicable actions.");
        npick = 0;
    }
    destroy_nhwindow(win);
    ch = '\0';
    if (npick > 0) {
        int NDECL((*func)) = picks->item.a_nfunc;
        free((genericptr_t) picks);

        if (doit) {
            int ret = (*func)();

            ch = (char) ret;
        } else {
            ch = cmd_from_func(func);
        }
    }
    return ch;
}

STATIC_OVL char
here_cmd_menu(doit)
boolean doit;
{
    winid win;
    char ch;
    char buf[BUFSZ];
    schar typ = levl[u.ux][u.uy].typ;
    int npick;
    menu_item *picks = (menu_item *) 0;

    win = create_nhwindow(NHW_MENU);
    start_menu_ex(win, GHMENU_STYLE_CHOOSE_COMMAND);

    if (IS_FOUNTAIN(typ) || IS_SINK(typ)) 
    {
        //int ftyp = levl[u.ux][u.uy].subtyp; // (levl[u.ux][u.uy].fountainmask & FOUNTAIN_TYPE_MASK);
        Sprintf(buf, "Drink from the %s", IS_FOUNTAIN(typ) ? get_fountain_name(u.ux, u.uy) : defsyms[S_sink].explanation);
        add_herecmd_menuitem(win, dodrink, buf);
    }
    if (IS_FOUNTAIN(typ))
    {
        //int ftyp = levl[u.ux][u.uy].subtyp; //  (levl[u.ux][u.uy].fountainmask& FOUNTAIN_TYPE_MASK);
        Sprintf(buf, "Dip something into the %s", get_fountain_name(u.ux, u.uy));
        add_herecmd_menuitem(win, dodip, buf);
    }
    if (IS_THRONE(typ))
        add_herecmd_menuitem(win, dosit,
                             "Sit on the throne");

    if ((u.ux == xupstair && u.uy == yupstair)
        || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
        || (u.ux == xupladder && u.uy == yupladder)) {
        Sprintf(buf, "Go up the %s",
                (u.ux == xupladder && u.uy == yupladder)
                ? "ladder" : "stairs");
        add_herecmd_menuitem(win, doup, buf);
    }
    if ((u.ux == xdnstair && u.uy == ydnstair)
        || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
        || (u.ux == xdnladder && u.uy == ydnladder)) {
        Sprintf(buf, "Go down the %s",
                (u.ux == xdnladder && u.uy == ydnladder)
                ? "ladder" : "stairs");
        add_herecmd_menuitem(win, dodown, buf);
    }
    if (u.usteed) { /* another movement choice */
        Sprintf(buf, "Dismount %s",
                x_monnam(u.usteed, ARTICLE_THE, (char *) 0, SUPPRESS_SADDLE, FALSE));
        add_herecmd_menuitem(win, doride, buf);
    }

#if 0
    if (Upolyd) { /* before objects */
        Sprintf(buf, "Use %s special ability",
                s_suffix(mons[u.umonnum].mname));
        add_herecmd_menuitem(win, domonability, buf);
    }
#endif

    if (OBJ_AT(u.ux, u.uy)) {
        struct obj *otmp = level.objects[u.ux][u.uy];

        Sprintf(buf, "Pick up %s", otmp->nexthere ? "items" : doname_with_weight_last(otmp, objects[LOADSTONE].oc_name_known));
        add_herecmd_menuitem(win, dopickup, buf);

        if (Is_container(otmp)) {
            Sprintf(buf, "Loot %s", doname(otmp));
            add_herecmd_menuitem(win, doloot, buf);
        }
        if (otmp->oclass == FOOD_CLASS) {
            Sprintf(buf, "Eat %s", doname(otmp));
            add_herecmd_menuitem(win, doeat, buf);
        }
    }

    if (invent)
        add_herecmd_menuitem(win, dodrop, "Drop items");

    add_herecmd_menuitem(win, donull, "Rest one turn");
    add_herecmd_menuitem(win, dosearch, "Search around you");
    add_herecmd_menuitem(win, dolook, "Look at what is here");

    end_menu(win, "What do you want to do?");
    npick = select_menu(win, PICK_ONE, &picks);
    destroy_nhwindow(win);
    ch = '\0';
    if (npick > 0) {
        int NDECL((*func)) = picks->item.a_nfunc;
        free((genericptr_t) picks);

        if (doit) {
            int ret = (*func)();

            ch = (char) ret;
        } else {
            ch = cmd_from_func(func);
        }
    }
    return ch;
}


/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1] = 0;
    int target_x = x;
    int target_y = y;
    memset(cmd, 0, sizeof(cmd));

    if (iflags.clicklook && mod == CLICK_2) 
    {
        clicklook_cc.x = target_x;
        clicklook_cc.y = target_y;
        cmd[0] = Cmd.spkeys[NHKF_CLICKLOOK];
        return cmd;
    }

    x -= u.ux;
    y -= u.uy;

    if (flags.travelcmd) 
    {
        if (abs(x) <= 1 && abs(y) <= 1)
        {
            x = sgn(x), y = sgn(y);
        }
        else if (mod == CLICK_3)
        {
            if (abs(y) <= (max(0, abs(x) - 1) / 2))
                y = 0;

            if (abs(x) <= (max(0, abs(y) - 1) / 2))
                x = 0;
            
            x = sgn(x), y = sgn(y);
        }
        else 
        {
            struct monst* mtmp = 0;
            if (isok(target_x, target_y))
            {
                mtmp = m_at(target_x, target_y);
            }

            if (mtmp && canspotmon(mtmp) && !is_peaceful(mtmp) && !is_tame(mtmp))
            {
                if (iflags.clickfire && uwep && (is_launcher(uwep) || is_thrown_weapon_only(uwep)) && dist2(u.ux, u.uy, target_x, target_y) > 2)
                {
                    if (!x || !y || abs(x) == abs(y)) /* straight line or diagonal */
                    {
                        boolean path_is_clear = clear_path(u.ux, u.uy, target_x, target_y);
                        struct monst* mtmpinway = spotted_linedup_monster_in_way(u.ux, u.uy, target_x, target_y);
                        if (path_is_clear && !mtmpinway)
                        {
                            //clickfire_cc.x = target_x;
                            //clickfire_cc.y = target_y;
                            cmd[0] = Cmd.spkeys[NHKF_CLICKFIRE];
                            x = sgn(x), y = sgn(y);
                            dir = xytod(x, y);
                            cmd[1] = dir >= 0 ? Cmd.dirchars[dir] : '\0';
                            cmd[2] = '\0';
                        }
                        else if(!path_is_clear)
                        {
                            play_sfx_sound(SFX_GENERAL_CANNOT);
                            pline_ex(ATR_NONE, CLR_MSG_FAIL, "You cannot %s at %s; the path to it is not clear.", is_launcher(uwep) ? "fire" : "throw", mon_nam(mtmp));
                            cmd[0] = '\0';
                        }
                        else if (mtmpinway)
                        {
                            play_sfx_sound(SFX_GENERAL_CANNOT);
                            pline_ex(ATR_NONE, CLR_MSG_FAIL, "You cannot %s at %s; %s is in the way.", is_launcher(uwep) ? "fire" : "throw", mon_nam(mtmp), mon_nam(mtmpinway));
                            cmd[0] = '\0';
                        }
                        else
                        {
                            play_sfx_sound(SFX_GENERAL_CANNOT);
                            pline_ex(ATR_NONE, CLR_MSG_FAIL, "You cannot %s at %s; there is something in the way.", is_launcher(uwep) ? "fire" : "throw", mon_nam(mtmp));
                            cmd[0] = '\0';
                        }
                    }
                    else
                    {
                        play_sfx_sound(SFX_GENERAL_CANNOT);
                        pline_ex(ATR_NONE, CLR_MSG_FAIL, "You cannot %s at %s; it is not lined up.", is_launcher(uwep) ? "fire" : "throw", mon_nam(mtmp));
                        cmd[0] = '\0';
                    }
                }
                else
                {
                    u.tx = target_x;
                    u.ty = target_y;
                    cmd[0] = Cmd.spkeys[NHKF_TRAVEL_ATTACK];
                }
            }
            else
            {
                /* Normal travel */
                u.tx = target_x;
                u.ty = target_y;
                //struct obj* otmp = 0;
                cmd[0] = Cmd.spkeys[NHKF_TRAVEL_WALK];
            }
            return cmd;
        }

        if (x == 0 && y == 0)
        {
            if (!flags.self_click_action)
            {
                cmd[0] = '\0';
                return cmd;
            }

            if (u.uswallow)
            {
                cmd[0] = '\0';
                return cmd;
            }

            if (iflags.herecmd_menu) 
            {
                cmd[0] = here_cmd_menu(FALSE);
                return cmd;
            }

            /* here */
            if (IS_FOUNTAIN(levl[u.ux][u.uy].typ)
                || IS_SINK(levl[u.ux][u.uy].typ)) 
            {
                cmd[0] = cmd_from_func(mod == CLICK_1 || mod == CLICK_3 ? dodrink : dodip);
                return cmd;
            } 
            else if (IS_THRONE(levl[u.ux][u.uy].typ)) 
            {
                cmd[0] = cmd_from_func(dosit);
                return cmd;
            } 
            else if ((u.ux == xupstair && u.uy == yupstair)
                       || (u.ux == sstairs.sx && u.uy == sstairs.sy
                           && sstairs.up)
                       || (u.ux == xupladder && u.uy == yupladder)) 
            {
                cmd[0] = cmd_from_func(doup);
                return cmd;
            } 
            else if ((u.ux == xdnstair && u.uy == ydnstair)
                       || (u.ux == sstairs.sx && u.uy == sstairs.sy
                           && !sstairs.up)
                       || (u.ux == xdnladder && u.uy == ydnladder)) 
            {
                cmd[0] = cmd_from_func(dodown);
                return cmd;
            } 
            else if (OBJ_AT(u.ux, u.uy)) 
            {
                cmd[0] = cmd_from_func(Is_container(level.objects[u.ux][u.uy])
                                       ? doloot : dopickup);
                return cmd;
            }
            else 
            {
                cmd[0] = cmd_from_func(donull); /* just rest */
                return cmd;
            }
        }

        /* directional commands */

        dir = xytod(x, y);

        if (!u.uswallow && !m_at(u.ux + x, u.uy + y)
            && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) 
        {
            cmd[1] = Cmd.dirchars[dir];
            cmd[2] = '\0';
            if (iflags.herecmd_menu)
            {
                cmd[0] = there_cmd_menu(FALSE, u.ux + x, u.uy + y);
                if (cmd[0] == '\0')
                    cmd[1] = '\0';
                return cmd;
            }

            if (IS_DOOR(levl[u.ux + x][u.uy + y].typ))
            {
                /* slight assistance to the player: choose kick/open for them
                    */
                struct rm* door = &levl[u.ux + x][u.uy + y];
                boolean has_fitting_key = FALSE;
                if (flags.autounlock)
                {
                    struct obj* carried_key = 0;
                    if ((carried_key = carrying_fitting_unlocking_tool_for_door(door)) != 0)
                    {
                        has_fitting_key = TRUE;
                    }
                }
                if (door->doormask & D_LOCKED)
                {
                    if (has_fitting_key || !door->click_kick_ok)
                    {
                        if (!door->click_kick_ok)
                            context.click_kick_query = 1;

                        cmd[0] = cmd_from_func(doopen);
                    }
                    else
                        cmd[0] = cmd_from_func(dokick);
                    return cmd;
                }
                if (door->doormask & D_CLOSED)
                {
                    cmd[0] = cmd_from_func(doopen);
                    return cmd;
                }
            }
            if (levl[u.ux + x][u.uy + y].typ <= SCORR)
            {
                cmd[0] = cmd_from_func(dosearch);
                cmd[1] = 0;
                return cmd;
            }
        }
    } 
    else 
    {
        /* convert without using floating point, allowing sloppy clicking */
        if (x > 2 * abs(y))
            x = 1, y = 0;
        else if (y > 2 * abs(x))
            x = 0, y = 1;
        else if (x < -2 * abs(y))
            x = -1, y = 0;
        else if (y < -2 * abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if (x == 0 && y == 0) 
        {
            /* map click on player to "rest" command */
            if (flags.self_click_action)
                cmd[0] = cmd_from_func(donull);
            else
                cmd[0] = '\0';
            return cmd;
        }
        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = '\0';
    if (mod == CLICK_1 || mod == CLICK_3) 
    {
        cmd[0] = Cmd.dirchars[dir];
    } 
    else 
    {
        cmd[0] = (Cmd.num_pad
                     ? M(Cmd.dirchars[dir])
                     : (Cmd.dirchars[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

char
get_count(allowchars, inkey, maxcount, count, historical)
char *allowchars;
char inkey;
long maxcount;
long *count;
boolean historical; /* whether to include in message history: True => yes */
{
    char qbuf[QBUFSZ];
    int key;
    long cnt = 0L;
    boolean backspaced = FALSE;
    /* this should be done in port code so that we have erase_char
       and kill_char available; we can at least fake erase_char */
#define STANDBY_erase_char '\177'

    for (;;) {
        if (inkey) {
            key = inkey;
            inkey = '\0';
        } else
            key = readchar();

        if (digit(key)) {
            cnt = 10L * cnt + (long) (key - '0');
            if (cnt < 0)
                cnt = 0;
            else if (maxcount > 0 && cnt > maxcount)
                cnt = maxcount;
        } else if (cnt && (key == '\b' || key == STANDBY_erase_char)) {
            cnt = cnt / 10;
            backspaced = TRUE;
        } else if (key == Cmd.spkeys[NHKF_ESC]) {
            break;
        } else if (!allowchars || index(allowchars, key)) {
            *count = cnt;
            break;
        }

        if (cnt > 9 || backspaced) {
            clear_nhwindow(WIN_MESSAGE);
            if (backspaced && !cnt) {
                Sprintf(qbuf, "Count: ");
            } else {
                Sprintf(qbuf, "Count: %ld", cnt);
                backspaced = FALSE;
            }
            custompline(SUPPRESS_HISTORY, "%s", qbuf);
            mark_synch();
        }
    }

    if (historical) {
        Sprintf(qbuf, "Count: %ld ", *count);
        (void) key2txt((uchar) key, eos(qbuf));
        putmsghistory(qbuf, FALSE);
    }

    return key;
}


STATIC_OVL char *
parse(VOID_ARGS)
{
#ifdef LINT /* static char in_line[COLNO]; */
    char in_line[COLNO];
#else
    static char in_line[COLNO];
#endif
    /*register int*/ char foo = 0;

    iflags.in_parse = TRUE;
    multi = 0;
    context.move = 1;
    flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

#ifdef ALTMETA
    alt_esc = iflags.altmeta; /* readchar() hack */
#endif
    escape_sequence_key_start_allowed = 1;

    if ((!Cmd.num_pad && !Cmd.gnh_layout) || ((foo = readchar()) == Cmd.spkeys[NHKF_COUNT])) {
        long tmpmulti = (long)multi;

        foo = get_count((char *) 0, '\0', LARGEST_INT, &tmpmulti, FALSE);
        last_multi = multi = (int)tmpmulti;
    }
#ifdef ALTMETA
    alt_esc = FALSE; /* readchar() reset */
#endif
    escape_sequence_key_start_allowed = 0;

    if (iflags.debug_fuzzer /* if fuzzing, override '!' and ^Z */
        && (Cmd.commands[foo & 0x0ff]
            && (Cmd.commands[foo & 0x0ff]->ef_funct == dosuspend_core
                || Cmd.commands[foo & 0x0ff]->ef_funct == dosh_core)))
        foo = Cmd.spkeys[NHKF_ESC];

    if (foo == Cmd.spkeys[NHKF_ESC]) 
    { /* esc cancels count (TH) */
        clear_nhwindow(WIN_MESSAGE);
        multi = last_multi = 0;
    }
    else if (foo == Cmd.spkeys[NHKF_DOAGAIN] || foo == Cmd.spkeys[NHKF_DOAGAIN2] || in_doagain) {
        multi = last_multi;
    }
    else
    {
        last_multi = multi;
        if (!(foo == ' ' && !flags.rest_on_space) && foo != 13)
        {
            savech(0); /* reset input queue */
            savech((char)foo);
        }
    }

    if (multi) {
        multi--;
        save_cm = in_line;
    } else {
        save_cm = (char *) 0;
    }
    /* in 3.4.3 this was in rhack(), where it was too late to handle M-5 */
    if (Cmd.pcHack_compat) {
        /* This handles very old inconsistent DOS/Windows behaviour
           in a different way: earlier, the keyboard handler mapped
           these, which caused counts to be strange when entered
           from the number pad. Now do not map them until here. */
        switch (foo) {
        case (char)'5':
            foo = Cmd.spkeys[NHKF_RUSH];
            break;
        case (char)M('5'):
            foo = Cmd.spkeys[NHKF_RUN];
            break;
        case (char)M('0'):
            foo = Cmd.spkeys[NHKF_DOINV];
            break;
        default:
            break; /* as is */
        }
    }

    in_line[0] = foo;
    in_line[1] = '\0';
    if (prefix_cmd(foo)) {
        escape_sequence_key_start_allowed = 1;
        foo = readchar();
        escape_sequence_key_start_allowed = 0;
        savech((char) foo);
        in_line[1] = foo;
        in_line[2] = 0;
    }
    clear_nhwindow(WIN_MESSAGE);

    iflags.in_parse = FALSE;
    return in_line;
}

#ifdef HANGUPHANDLING
/* some very old systems, or descendents of such systems, expect signal
   handlers to have return type `int', but they don't actually inspect
   the return value so we should be safe using `void' unconditionally */
/*ARGUSED*/
void
hangup(sig_unused) /* called as signal() handler, so sent at least one arg */
int sig_unused UNUSED;
{
    if (program_state.exiting)
        program_state.in_moveloop = 0;
    nhwindows_hangup();
#ifdef SAFERHANGUP
    /* When using SAFERHANGUP, the done_hup flag it tested in rhack
       and a couple of other places; actual hangup handling occurs then.
       This is 'safer' because it disallows certain cheats and also
       protects against losing objects in the process of being thrown,
       but also potentially riskier because the disconnected program
       must continue running longer before attempting a hangup save. */
    program_state.done_hup++;
    /* defer hangup iff game appears to be in progress */
    if (program_state.in_moveloop && program_state.something_worth_saving)
        return;
#endif /* SAFERHANGUP */
    end_of_input();
}

void
end_of_input(VOID_ARGS)
{
#ifdef NOSAVEONHANGUP
#ifdef INSURANCE
    if (flags.ins_chkpt && program_state.something_worth_saving)
        program_statue.preserve_locks = 1; /* keep files for recovery */
#endif
    program_state.something_worth_saving = 0; /* don't save */
#endif

#ifndef SAFERHANGUP
    if (!program_state.done_hup++)
#endif
        if (program_state.something_worth_saving)
        {
            (void)dosave0(TRUE);
        }
    if (iflags.window_inited)
        exit_nhwindows((char *) 0);
    clearlocks();
    nh_terminate(EXIT_SUCCESS);
    /*NOTREACHED*/ /* not necessarily true for vms... */
    return;
}
#endif /* HANGUPHANDLING */

char
readchar(VOID_ARGS)
{
    register int sym;
    int x = u.ux, y = u.uy, mod = 0;

    if (iflags.debug_fuzzer)
        return randomkey();
    if (*readchar_queue)
        sym = *readchar_queue++;
    else
        sym = in_doagain ? pgetchar() : nh_poskey(&x, &y, &mod);

#ifdef NR_OF_EOFS
    if (sym == EOF) {
        register int cnt = NR_OF_EOFS;
        /*
         * Some SYSV systems seem to return EOFs for various reasons
         * (?like when one hits break or for interrupted systemcalls?),
         * and we must see several before we quit.
         */
        do {
            clearerr(stdin); /* omit if clearerr is undefined */
            sym = pgetchar();
        } while (--cnt && sym == EOF);
    }
#endif /* NR_OF_EOFS */

    if (sym == EOF) {
#ifdef HANGUPHANDLING
        hangup(0); /* call end_of_input() or set program_state.done_hup */
#endif
        sym = '\033';
#ifdef ALTMETA
    }
    else if (sym == '\033' && alt_esc) {
        /* iflags.altmeta: treat two character ``ESC c'' as single `M-c' */
        sym = *readchar_queue ? *readchar_queue++ : pgetchar();
        if (sym == EOF || sym == 0)
            sym = '\033';
#ifdef UNIX
        else if (sym == 91 && tty_arrow_key_support_enabled())
            goto sym91here;
#endif
        else if (sym != '\033')
            sym |= 0200; /* force 8th bit on */
#endif /*ALTMETA*/

#ifdef UNIX
    } else if (sym == '\033' && tty_arrow_key_support_enabled()) {
        sym = *readchar_queue ? *readchar_queue++ : pgetchar();
        if (sym == EOF || sym == 0 || sym == '\033')
            sym = '\033';
        else if (sym == 91)
        {
        sym91here:
            sym = *readchar_queue ? *readchar_queue++ : pgetchar();
            switch (sym)
            {
            case 0: /* Alt+[ */
                sym = '['; //91
                sym |= 0200; /* force 8th bit on */
                break;
            case 65:
                sym = Cmd.move_N;
                break;
            case 66:
                sym = Cmd.move_S;
                break;
            case 67:
                sym = Cmd.move_E;
                break;
            case 68:
                sym = Cmd.move_W;
                break;
            default:
                sym = '\033';
                break;
            }
        }
        else
        {
            /* Extra handling here, but currently they all give ESC */
            sym = '\033';
        }
#endif
    } else if (sym == 0) {
        /* click event */
        readchar_queue = click_to_cmd(x, y, mod);
        sym = *readchar_queue++;
    }
    return (char) sym;
}

/* '_' command, #travel, via keyboard rather than mouse click */
STATIC_PTR int
dotravel(VOID_ARGS)
{
    static char cmd[2];
    coord cc;

    /* [FIXME?  Supporting the ability to disable traveling via mouse
       click makes some sense, depending upon overall mouse usage.
       Disabling '_' on a user by user basis makes no sense at all since
       even if it is typed by accident, aborting when picking a target
       destination is trivial.  Travel via mouse predates travel via '_',
       and this use of OPTION=!travel is probably just a mistake....] */
    if (!flags.travelcmd)
        return 0;

    cmd[1] = 0;
    cc.x = iflags.travelcc.x;
    cc.y = iflags.travelcc.y;
    if (cc.x == 0 && cc.y == 0) {
        /* No cached destination, start attempt from current position */
        cc.x = u.ux;
        cc.y = u.uy;
    }
    iflags.getloc_travelmode = TRUE;
    if (iflags.menu_requested) {
        int gf = iflags.getloc_filter;
        iflags.getloc_filter = GFILTER_VIEW;
        if (!getpos_menu(&cc, GLOC_INTERESTING)) {
            iflags.getloc_filter = gf;
            iflags.getloc_travelmode = FALSE;
            return 0;
        }
        iflags.getloc_filter = gf;
    } else {
        pline("Where do you want to travel to?");
        if (getpos(&cc, TRUE, "the desired destination", CURSOR_STYLE_TRAVEL_CURSOR) < 0) {
            /* user pressed ESC */
            iflags.getloc_travelmode = FALSE;
            return 0;
        }
    }
    iflags.getloc_travelmode = FALSE;
    iflags.travelcc.x = u.tx = cc.x;
    iflags.travelcc.y = u.ty = cc.y;
    cmd[0] = Cmd.spkeys[NHKF_TRAVEL];
    readchar_queue = cmd;
    return 0;
}

/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function_ex(style, attr, color, glyph, title, query, resp, def, resp_desc, introline, ynflags)
int style, attr, color, glyph;
const char *title, *query, *resp, *resp_desc, * introline;
char def;
unsigned long ynflags; /* 1 means use upper side for half-sized tile */
{
    char res, qbuf[QBUFSZ];
#if defined(DUMPLOG) || defined(DUMPHTML)
    extern unsigned saved_pline_index; /* pline.c */
    unsigned idx = saved_pline_index;
    /* buffer to hold query+space+formatted_single_char_response */
    char dumplog_buf[QBUFSZ + 1 + 15]; /* [QBUFSZ+1+7] should suffice */
#endif

    iflags.last_msg = PLNMSG_UNKNOWN; /* most recent pline is clobbered */

    /* maximum acceptable length is QBUFSZ-1 */
    if (strlen(query) >= QBUFSZ) {
        /* caller shouldn't have passed anything this long */
        paniclog("Query truncated: ", query);
        (void) strncpy(qbuf, query, QBUFSZ - 1 - 3);
        Strcpy(&qbuf[QBUFSZ - 1 - 3], "...");
        query = qbuf;
    }
    res = (*windowprocs.win_yn_function_ex)(style, attr, color, glyph, title, query, resp, def, resp_desc, introline, ynflags);
#if defined(DUMPLOG) || defined(DUMPHTML)
    if (idx == saved_pline_index) {
        /* when idx is still the same as saved_pline_index, the interface
           didn't put the prompt into saved_plines[]; we put a simplified
           version in there now (without response choices or default) */
        Sprintf(dumplog_buf, "%s ", query);
        (void) key2txt((uchar) res, eos(dumplog_buf));
        dumplogmsg(dumplog_buf, (char*)0, (char*)0, attr, color);
    }
#endif
    return res;
}

char
yn_function(query, resp, def, resp_desc)
const char* query, *resp, *resp_desc;
char def;
{
    return yn_function_ex(YN_STYLE_GENERAL, ATR_NONE, NO_COLOR, NO_GLYPH, (const char*)0, query, resp, def, resp_desc, (const char*)0, 0UL);
}

char
yn_function_mon(mtmp, query, chars, def, descs)
struct monst* mtmp;
const char* query;
const char* chars, *descs;
char def;
{
    if (!mtmp)
        return yn_query(query);

    int glyph = any_mon_to_glyph(mtmp, rn2_on_display_rng);
    char namebuf[BUFSZ];
    strcpy_capitalized_for_title(namebuf, Monnam(mtmp));

    return yn_function_ex(YN_STYLE_MONSTER_QUESTION, ATR_NONE, NO_COLOR, glyph, namebuf, query, chars, def, descs, (const char*)0, 0UL);
}

char
yn_query_mon(mtmp, query)
struct monst* mtmp;
const char* query;
{
    return yn_function_mon(mtmp, query, ynchars, 'n', yndescs);
}

char
ynq_mon(mtmp, query)
struct monst* mtmp;
const char* query;
{
    return yn_function_mon(mtmp, query, ynqchars, 'q', ynqdescs);
}

char
ynaq_mon(mtmp, query)
struct monst* mtmp;
const char* query;
{
    return yn_function_mon(mtmp, query, ynaqchars, 'q', ynaqdescs);
}

/* for paranoid_confirm:quit,die,attack prompting */
boolean
paranoid_query(be_paranoid, prompt)
boolean be_paranoid;
const char* prompt;
{
    return paranoid_query_ex(ATR_NONE, CLR_MSG_WARNING, be_paranoid, (char*)0, prompt);
}

/* for paranoid_confirm:quit,die,attack prompting */
boolean
paranoid_query_ex(attr, color, be_paranoid, title, prompt)
int attr, color;
boolean be_paranoid;
const char *prompt, *title;
{
    boolean confirmed_ok;

    /* when paranoid, player must respond with "yes" rather than just 'y'
       to give the go-ahead for this query; default is "no" unless the
       ParanoidConfirm flag is set in which case there's no default */
    if (be_paranoid) {
        char qbuf[QBUFSZ], ans[BUFSZ] = DUMMY;
        const char *promptprefix = "", *responsetype = ParanoidConfirm
                                                           ? "(yes|no)"
                                                           : "(yes) [no]";
        int trylimit = 6; /* 1 normal, 5 more with "Yes or No:" prefix */

        /* in addition to being paranoid about this particular
           query, we might be even more paranoid about all paranoia
           responses (ie, ParanoidConfirm is set) in which case we
           require "no" to reject in addition to "yes" to confirm
           (except we won't loop if response is ESC; it means no) */
        do {
            Sprintf(qbuf, "%s%s", promptprefix, prompt);
            getlin_ex(GETLINE_PARANOID, attr, color, qbuf, ans, (char*)0, responsetype, (char*)0);
            (void) mungspaces(ans);
            confirmed_ok = !strcmpi(ans, "yes");
            if (confirmed_ok || *ans == '\033')
                break;
            promptprefix = "\"Yes\" or \"No\": ";
        } while (ParanoidConfirm && strcmpi(ans, "no") && --trylimit);
    } else
        confirmed_ok = (yn_query_ex(attr, color, title, prompt) == 'y');

    return confirmed_ok;
}

/* ^Z command, #suspend */
STATIC_PTR int
dosuspend_core(VOID_ARGS)
{
#if defined(SUSPEND) && !defined(GNH_MOBILE)
    /* Does current window system support suspend? */
    if ((*windowprocs.win_can_suspend)()) {
        /* NB: SYSCF SHELLERS handled in port code. */
        dosuspend();
    } else
#endif
        Norep(cmdnotavail, "#suspend");
    return 0;
}

/* '!' command, #shell */
STATIC_PTR int
dosh_core(VOID_ARGS)
{
#ifdef SHELL
    /* access restrictions, if any, are handled in port code */
    dosh();
#else
    Norep(cmdnotavail, "#shell");
#endif
    return 0;
}


int
dozoomnormal(VOID_ARGS)
{
    flags.screen_scale_adjustment = flags.preferred_screen_scale <= 0 ? 0.0 
        : max(MIN_SCREEN_SCALE_ADJUSTMENT, min(MAX_SCREEN_SCALE_ADJUSTMENT, (((double)flags.preferred_screen_scale) / 100.0 - 1.0) ));

    stretch_window();

    return 0;
}

int
dozoomin(VOID_ARGS)
{
    double scale_level = round(flags.screen_scale_adjustment / KEYBOARD_SCREEN_SCALE_ADJUSTMENT_STEP);
    flags.screen_scale_adjustment = (scale_level + 1) * KEYBOARD_SCREEN_SCALE_ADJUSTMENT_STEP;
    if(flags.screen_scale_adjustment > MAX_SCREEN_SCALE_ADJUSTMENT)
        flags.screen_scale_adjustment = MAX_SCREEN_SCALE_ADJUSTMENT;
    
    stretch_window();

    return 0;
}

int
dozoomout(VOID_ARGS)
{
    double scale_level = round(flags.screen_scale_adjustment / KEYBOARD_SCREEN_SCALE_ADJUSTMENT_STEP);
    flags.screen_scale_adjustment = (scale_level - 1) * KEYBOARD_SCREEN_SCALE_ADJUSTMENT_STEP;
    if (flags.screen_scale_adjustment < MIN_SCREEN_SCALE_ADJUSTMENT)
        flags.screen_scale_adjustment = MIN_SCREEN_SCALE_ADJUSTMENT;

    stretch_window();

    return 0;
}

int
dozoommini(VOID_ARGS)
{
    flags.screen_scale_adjustment = -1.0; /* In fact fit-to-screen */

    stretch_window();

    return 0;
}

int
dozoomhalf(VOID_ARGS)
{
    flags.screen_scale_adjustment = -0.5;

    stretch_window();

    return 0;
}

void
zoomtoscale(scale)
double scale;
{
    flags.screen_scale_adjustment = scale;
    stretch_window();
}

int
doviewpet(VOID_ARGS)
{
    if (context.view_pet_mid > 0)
    {
        struct monst* mtmp;
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
        {
            if (mtmp->m_id == context.view_pet_mid && is_tame(mtmp))
            {
                if (abs(mtmp->mx - u.ux) <= 1 && abs(mtmp->my - u.uy) <= 1 && !mtmp->meating && mon_can_move(mtmp))
                    return dochatmon(mtmp);
                else
                    return doviewpetstatistics(mtmp);
            }
        }
    }

    return 0;
}


STATIC_PTR int
dolight(VOID_ARGS)
{
    if (get_location_light_range(u.ux, u.uy) != 0)
    {
        if (levl[u.ux][u.uy].lamplit == 0)
        {
            char qbuf[BUFSZ];
            if (levl[u.ux][u.uy].typ == ALTAR)
                Sprintf(qbuf, "The fires on the %s are unlit here. Light them up?", dfeature_at(u.ux, u.uy));
            else
               Sprintf(qbuf, "There is an unlit %s here. Light it up?", dfeature_at(u.ux, u.uy));

            char ans = yn_query(qbuf);
            if (ans == 'y')
            {
                play_simple_location_sound(u.ux, u.uy, LOCATION_SOUND_TYPE_APPLY);
                maybe_create_location_light_source(u.ux, u.uy);
                newsym(u.ux, u.uy);
                return 1;
            }
        }
        else
        {
            char qbuf[BUFSZ];

            if (levl[u.ux][u.uy].typ == ALTAR)
                Sprintf(qbuf, "The fires on the %s are lit here. Snuff them out?", dfeature_at(u.ux, u.uy));
            else
                Sprintf(qbuf, "There is a lit %s here. Snuff it out?", dfeature_at(u.ux, u.uy));

            char ans = yn_query(qbuf);
            if (ans == 'y')
            {
                levl[u.ux][u.uy].lamplit = FALSE;
                del_light_source(LS_LOCATION, xy_to_any(u.ux, u.uy));
                newsym(u.ux, u.uy);
                play_simple_location_sound(u.ux, u.uy, LOCATION_SOUND_TYPE_APPLY2);
                return 1;
            }
        }
    }
    else if(OBJ_AT(u.ux, u.uy))
    {
        struct obj* otmp;
        char qbuf[QBUFSZ];
        char c;
        int cnt = 0;

        for (otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere)
            if (is_obj_ignitable(otmp))
                cnt++;

        if (cnt > 0)
        {
            c = 'y';
            if (cnt > 1)
            {
                Sprintf(qbuf, "There are several ignitable objects here. Do you want to light them up or snuff them out?");
                c = yn_function(qbuf, ynqchars, 'n', ynqdescs);
            }

            if (c == 'q')
                return  FALSE;
            else if (c == 'y')
            {
                for (otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere)
                {
                    if (is_obj_ignitable(otmp))
                    {
                        char qsfx[QBUFSZ];
                        boolean one = (otmp->quan == 1L);
                        boolean snuffout = (otmp->lamplit);

                        /* "There is <an object> here; <verb> it?" or
                            "There are <N objects> here; <verb> one?" */

                        Sprintf(qbuf, "There %s ", otense(otmp, "are"));
                        Sprintf(qsfx, " here; %s %s %s?", snuffout ? "snuff" : "light", one ? "it" : "them", snuffout ? "out" : "up");

                        (void)safe_qbuf(qbuf, qbuf, qsfx, otmp, doname_in_text, ansimpleoname,
                            one ? something : (const char*)"things");

                        if ((c = yn_function(qbuf, ynqchars, 'n', ynqdescs)) == 'y')
                        {
                            if (is_candle(otmp))
                                return use_candle(&otmp);
                            else if (is_torch(otmp))
                                return use_torch(&otmp);
                            else if (is_obj_candelabrum(otmp))
                                use_candelabrum(otmp);
                            else if (otmp->otyp == POT_OIL)
                                light_cocktail(&otmp);
                            else
                                use_lamp(otmp);
                            return TRUE;
                        }
                        else if (c == 'q')
                            return FALSE;
                    }
                }
            }
        }
    }


    /* Light in a direction */
    coord cc;
    if (!get_adjacent_loc((char*)0, (char*)0, u.ux, u.uy, &cc))
        return 0;

    if (get_location_light_range(cc.x, cc.y) != 0)
    {
        char ebuf[BUFSZ];
        strcpy(ebuf, "");
        if(levl[cc.x][cc.y].typ == ALTAR)
            strcpy(ebuf, "fires on the ");

        if (levl[cc.x][cc.y].lamplit == 0)
        {
            maybe_create_location_light_source(cc.x, cc.y);
            newsym(cc.x, cc.y);
            play_simple_location_sound(cc.x, cc.y, LOCATION_SOUND_TYPE_APPLY);
            You("light the %s%s up.", ebuf, dfeature_at(cc.x, cc.y));
            return 1;
        }
        else
        {
            levl[cc.x][cc.y].lamplit = FALSE;
            del_light_source(LS_LOCATION, xy_to_any(cc.x, cc.y));
            newsym(cc.x, cc.y);
            play_simple_location_sound(cc.x, cc.y, LOCATION_SOUND_TYPE_APPLY2);
            You("snuff the %s%s out.", ebuf, dfeature_at(cc.x, cc.y));
            return 1;
        }
    }
    else
    {
        play_sfx_sound(SFX_GENERAL_NOTHING_THERE);
        pline_ex(ATR_NONE, CLR_MSG_FAIL, "There's nothing to light or snuff out.");
    }

    return 0;
}

void
create_context_menu(menu_type)
enum create_context_menu_types menu_type;
{
    clear_context_menu();

    switch (menu_type)
    {
    case CREATE_CONTEXT_MENU_IN_GETPOS_GENERAL:
        add_context_menu('a', Cmd.spkeys[NHKF_GETPOS_INTERESTING_NEXT], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Next",
            0, 0, NO_COLOR);

        add_context_menu('A', Cmd.spkeys[NHKF_GETPOS_INTERESTING_PREV], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Previous",
            0, 0, NO_COLOR);

        add_context_menu('.', Cmd.spkeys[NHKF_GETPOS_PICK], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Select",
            0, 0, NO_COLOR);
        break;
    case CREATE_CONTEXT_MENU_IN_GETPOS_MONSTER:
        add_context_menu('m', Cmd.spkeys[NHKF_GETPOS_MON_NEXT], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Next",
            0, 0, NO_COLOR);

        add_context_menu('M', Cmd.spkeys[NHKF_GETPOS_MON_PREV], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Previous",
            0, 0, NO_COLOR);

        add_context_menu('.', Cmd.spkeys[NHKF_GETPOS_PICK], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Select",
            0, 0, NO_COLOR);
        break;
    case CREATE_CONTEXT_MENU_IN_GETPOS_TRAVEL_MODE:
        add_context_menu('a', Cmd.spkeys[NHKF_GETPOS_INTERESTING_NEXT], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Next",
            0, 0, NO_COLOR);

        add_context_menu('A', Cmd.spkeys[NHKF_GETPOS_INTERESTING_PREV], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Previous",
            0, 0, NO_COLOR);

        add_context_menu('<', cmd_from_func(doup), CONTEXT_MENU_STYLE_GETPOS, cmap_to_glyph(S_upstair), "Stairs Up",
            "Stairs", 0, NO_COLOR);

        add_context_menu('>', cmd_from_func(dodown), CONTEXT_MENU_STYLE_GETPOS, cmap_to_glyph(S_dnstair), "Stairs Down",
            "Stairs", 0, NO_COLOR);

        add_context_menu('.', Cmd.spkeys[NHKF_GETPOS_PICK], CONTEXT_MENU_STYLE_GETPOS, NO_GLYPH, "Select",
            0, 0, NO_COLOR);
        break;
    case CREATE_CONTEXT_MENU_IN_GETDIR:
        add_context_menu('<', cmd_from_func(doup), CONTEXT_MENU_STYLE_GETDIR, cmap_to_glyph(S_upstair), "Upwards",
            0, 0, NO_COLOR);

        add_context_menu('>', cmd_from_func(dodown), CONTEXT_MENU_STYLE_GETDIR, cmap_to_glyph(S_dnstair), "Downwards",
            0, 0, NO_COLOR);

        add_context_menu('.', Cmd.spkeys[NHKF_GETDIR_SELF], CONTEXT_MENU_STYLE_GETDIR, u_to_glyph(), "Self",
            0, 0, NO_COLOR);
        break;
    case CREATE_CONTEXT_MENU_BLOCKING_WINDOW:
        add_context_menu(27, Cmd.spkeys[NHKF_ESC], CONTEXT_MENU_STYLE_CLOSE_DISPLAY, NO_GLYPH, "Close",
            0, 0, NO_COLOR);
        break;
    default:
    case CREATE_CONTEXT_MENU_NORMAL:
    {

        /* Normal context menu */
        if (!isok(u.ux, u.uy))
            return;

        struct obj* otmp = level.objects[u.ux][u.uy];
        struct rm* lev = &levl[u.ux][u.uy];
        struct trap* t = t_at(u.ux, u.uy);
        if (IS_ALTAR(lev->typ))
        {
            add_context_menu(M('o'), cmd_from_func(dosacrifice), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Offer", 0, 0, NO_COLOR);
            add_context_menu(M('p'), cmd_from_func(dopray), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Pray", 0, 0, NO_COLOR);
        }
        else if (IS_FOUNTAIN(lev->typ) || IS_SINK(lev->typ))
        {
            add_context_menu('q', cmd_from_func(dodrink), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Drink", 0, 0, NO_COLOR);
            add_context_menu(M('d'), cmd_from_func(dodip), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Dip", 0, 0, NO_COLOR);
        }
        else if (IS_POOL(lev->typ))
        {
            add_context_menu(M('d'), cmd_from_func(dodip), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Dip", 0, 0, NO_COLOR);
        }
        else if (IS_THRONE(lev->typ))
        {
            add_context_menu(C('s'), cmd_from_func(dosit), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Sit", "on Throne", 0, NO_COLOR);
        }
        else if ((u.ux == xupstair && u.uy == yupstair)
            || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
            || (u.ux == xupladder && u.uy == yupladder))
        {
            add_context_menu('<', cmd_from_func(doup), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Go Up",
                (u.ux == xupladder && u.uy == yupladder) ? "Ladder" : "Stairs", 0, NO_COLOR);
        }
        else if ((u.ux == xdnstair && u.uy == ydnstair)
            || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
            || (u.ux == xdnladder && u.uy == ydnladder))
        {
            add_context_menu('>', cmd_from_func(dodown), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Go Down",
                (u.ux == xdnladder && u.uy == ydnladder) ? "Ladder" : "Stairs", 0, NO_COLOR);
        }
        else if ((Flying || (Levitation && Levitation_control)) && t && t->tseen && is_hole(t->ttyp)
            && Can_fall_thru(&u.uz))
        {
            add_context_menu('>', cmd_from_func(dodown), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), "Go Down",
                t->ttyp == HOLE ? "Hole" : "Trap Door", 0, NO_COLOR);
        }
        else if (t && t->tseen && is_pit(t->ttyp))
        {
            if (u.utrap > 0)
            {
                /* Climb out of the pit */
                add_context_menu('<', cmd_from_func(doup), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy), 
                    Flying ? "Fly out" : (Levitation && Levitation_control) ? "Levitate out" : "Climb out",
                    "Pit", 0, NO_COLOR);
            }
            else
            {
                /* Go down to the pit */
                add_context_menu('>', cmd_from_func(dodown), CONTEXT_MENU_STYLE_GENERAL, back_to_glyph(u.ux, u.uy),
                    Flying ? "Fly down" : (Levitation && Levitation_control) ? "Levitate down" : has_pitwalk(youmonst.data) ? "Climb down" : "Jump down",
                    "Pit", 0, NO_COLOR);
            }
        }

        struct monst* shkp = can_pay_to_shkp();
        if (shkp && has_eshk(shkp))
        {
            struct eshk* eshkp = ESHK(shkp);
            if (eshkp->robbed || eshkp->debit || eshkp->billct)
            {
                add_context_menu('p', cmd_from_func(dopay), CONTEXT_MENU_STYLE_GENERAL, any_mon_to_glyph(shkp, rn2_on_display_rng), "Pay", 0, 0, NO_COLOR);
            }
        }

        int x, y;
        boolean addchatmenu = FALSE;
        boolean addridemenu = FALSE;
        //boolean addkickmenu = FALSE;
        struct monst* mtmp = 0;
        struct monst* steedmtmp = 0;
        struct monst* chatmtmp = 0;
        //struct obj* kickotmp = 0;
        if (!Hallucination)
        {
            for (x = u.ux - 1; x <= u.ux + 1; x++)
            {
                for (y = u.uy - 1; y <= u.uy + 1; y++)
                    if (!(x == u.ux && y == u.uy) && isok(x, y))
                    {
                        mtmp = m_at(x, y);
                        if (mtmp && !DEADMONSTER(mtmp))
                        {
                            if (monster_invokes_context_chat(mtmp))
                            {
                                chatmtmp = mtmp;
                                addchatmenu = TRUE;
                            }
                            if (!u.usteed && is_tame(mtmp) && is_steed(mtmp->data) && canspotmon(mtmp) && which_armor(mtmp, W_SADDLE))
                            {
                                steedmtmp = mtmp;
                                addridemenu = TRUE;
                            }
                        }
                        //otmp = level.objects[x][y];
                        //if (otmp)
                        //{
                        //    if (Is_proper_container(otmp) && otmp->olocked && !otmp->obroken && otmp->lknown)
                        //    {
                        //        kickotmp = otmp;
                        //        addkickmenu = TRUE;
                        //    }
                        //}
                    }
            }
        }

        if (addchatmenu && chatmtmp)
        {
            add_context_menu('C', cmd_from_func(dotalknearby), CONTEXT_MENU_STYLE_GENERAL, any_mon_to_glyph(chatmtmp, rn2_on_display_rng), "Chat", Monnam(chatmtmp), 0, NO_COLOR);
        }
        if (addridemenu && steedmtmp)
        {
            add_context_menu(M('R'), cmd_from_func(doridenearby), CONTEXT_MENU_STYLE_GENERAL, any_mon_to_glyph(steedmtmp, rn2_on_display_rng), "Mount", Monnam(steedmtmp), 0, NO_COLOR);
        }
        //if (addkickmenu)
        //{
        //    if(kickotmp)
        //        add_context_menu(C('d'), cmd_from_func(dokicknearby), CONTEXT_MENU_STYLE_GENERAL, obj_to_glyph(kickotmp, rn2_on_display_rng), "Kick", The(cxname(kickotmp)), 0, NO_COLOR);
        //}

        if (u.usteed)
        {
            add_context_menu(M('R'), cmd_from_func(doride), CONTEXT_MENU_STYLE_GENERAL, any_mon_to_glyph(u.usteed, rn2_on_display_rng), "Dismount", Monnam(u.usteed), 0, NO_COLOR);
            add_context_menu('C', cmd_from_func(dotalksteed), CONTEXT_MENU_STYLE_GENERAL, any_mon_to_glyph(u.usteed, rn2_on_display_rng), "Steed", Monnam(u.usteed), 0, NO_COLOR);
        }

        if (otmp)
        {
            add_context_menu(',', cmd_from_func(dopickup), CONTEXT_MENU_STYLE_GENERAL, otmp->gui_glyph, "Pick Up", cxname(otmp), 0, NO_COLOR);
            struct obj* otmp_here;
            boolean eat_added = FALSE;
            boolean loot_added = FALSE;
            for (otmp_here = otmp; otmp_here; otmp_here = otmp_here->nexthere)
            {
                if (!eat_added && is_edible(otmp_here))
                {
                    add_context_menu('e', cmd_from_func(doeat), CONTEXT_MENU_STYLE_GENERAL, otmp_here->gui_glyph, "Eat", cxname(otmp_here), 0, NO_COLOR);
                    eat_added = TRUE;
                }

                if (!loot_added && Is_container(otmp_here))
                {
                    add_context_menu('l', cmd_from_func(doloot), CONTEXT_MENU_STYLE_GENERAL, otmp_here->gui_glyph, "Loot", cxname(otmp_here), 0, NO_COLOR);
                    loot_added = TRUE;
                }
            }
        }

        const char* dfeature = adjusted_dfeature_at(u.ux, u.uy);
        struct engr* ep = engr_at(u.ux, u.uy);
        int displ_style = here_window_display_style(dfeature, ep, otmp);

        if (Blind || displ_style == 2)
        {
            add_context_menu(':', cmd_from_func(dolook), CONTEXT_MENU_STYLE_GENERAL, NO_GLYPH, "Look Here", "", 0, NO_COLOR);
        }

        if (context.last_picked_obj_oid > 0 && context.last_picked_obj_show_duration_left > 0)
        {
            struct obj* lpobj;
            if ((lpobj = o_on(context.last_picked_obj_oid, invent)) != 0)
                add_context_menu(M('<'), cmd_from_func(dolastpickeditem), CONTEXT_MENU_STYLE_GENERAL, lpobj ? lpobj->gui_glyph : 0, "Last Item", lpobj ? cxname(lpobj) : "", 0, NO_COLOR);
        }
        break;
    }
    }
}

int
domarkautostash(VOID_ARGS)
{
    struct obj* obj = getobj(getobj_mark_autostashs, "mark as auto-stash", 0, "");
    if (!obj)
        return 0;

    if (obj && (Is_proper_container(obj) || (Is_container(obj) && !objects[obj->otyp].oc_name_known)))
    {
        if (obj->speflags & SPEFLAGS_AUTOSTASH)
        {
            pline("%s is already an auto-stash.", The(cxname(obj)));
        }
        else
        {
            obj->speflags |= SPEFLAGS_AUTOSTASH;
            pline("%s is now marked as an auto-stash.", The(cxname(obj)));
        }
    }
    else
    {
        pline("%s is not an auto-stashable item.", The(cxname(obj)));
    }

    return 0;
}

int
dounmarkautostash(VOID_ARGS)
{
    struct obj* obj = getobj(getobj_unmark_autostashs, "unmark as auto-stash", 0, "");
    if (!obj)
        return 0;

    if (!(obj->speflags & SPEFLAGS_AUTOSTASH))
    {
        pline("%s is not an auto-stash.", The(cxname(obj)));
    }
    else
    {
        obj->speflags &= ~SPEFLAGS_AUTOSTASH;
        pline("%s was unmarked as an auto-stash.", The(cxname(obj)));
    }

    return 0;
}

struct monst*
spotted_linedup_monster_in_way(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
    if(!isok(x1, y1) || !isok(x2,y2))
        return 0;

    if (x1 == x2 && y1 == y2)
        return 0;

    int dx = x2 - x1;
    int dy = y2 - y1;
    if (!(!dx || !dy || abs(dx) == abs(dy)))
        return 0;

    int sx = sgn(dx);
    int sy = sgn(dy);

    int x = x1;
    int y = y1;
    struct monst* mtmp = 0;

    while(TRUE)
    {
        x += sx;
        y += sy;
        if (x == x2 && y == y2)
            break;
        if (!isok(x, y))
            break;

        mtmp = m_at(x, y);
        if (mtmp && canspotmon(mtmp))
        {
            return mtmp;
        }
    };

    return 0;
}

/* #chronicle command */
int
do_gamelog(VOID_ARGS)
{
    if (gamelog) {
        show_gamelog(ENL_GAMEINPROGRESS);
    }
    else {
        pline("No chronicled events.");
    }
    return 0;
}

/*cmd.c*/
