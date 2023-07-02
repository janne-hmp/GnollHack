/* GnollHack File Change Notice: This file has been changed from the original. Date of last change: 2023-05-22 */

/* GnollHack 4.0    extern.h    $NHDT-Date: 1557088399 2019/05/05 20:33:19 $  $NHDT-Branch: GnollHack-3.6.2-beta01 $:$NHDT-Revision: 1.703 $ */
/* Copyright (c) Steve Creps, 1988.                  */
/* GnollHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#include "soundset.h"
#include "lev.h"
#include "general.h"

#define E extern

/* ### alloc.c ### */

#if 0
E long *FDECL(alloc, (size_t));
#endif
E char *FDECL(fmt_ptr, (const genericptr));

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void FDECL(moveloop, (BOOLEAN_P));
E void NDECL(stop_occupation);
E void NDECL(display_gamewindows);
E void NDECL(newgame);
E void NDECL(choose_game_difficulty);
E const char* FDECL(get_game_difficulty_text, (int));
E const char* FDECL(get_game_difficulty_symbol, (int));
E const char* FDECL(get_game_mode_text, (BOOLEAN_P));
E const char* FDECL(get_game_mode_text_core, (BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E const char* NDECL(get_game_mode_description);
E const char* FDECL(get_game_mode_description_core, (BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void NDECL(show_gui_tips);
E void FDECL(welcome, (BOOLEAN_P));
E time_t NDECL(get_realtime);
E int FDECL(argcheck, (int, char **, enum earlyarg));
E void FDECL(reset_monster_origin_coordinates, (struct monst*));
E void NDECL(reset_all_monster_origin_coordinates);
E void FDECL(reset_object_origin_coordinates, (struct obj*));
E void NDECL(lock_thread_lock);
E void NDECL(unlock_thread_lock);
E void NDECL(reset_allmain);

/* ### animation.c ### */

E void NDECL(init_animations);
E short FDECL(get_player_replacement, (enum action_tile_types, int, int, int, int, int));
E short FDECL(get_player_animation, (enum action_tile_types, int, int, int, int, int));
E short FDECL(get_player_enlargement, (enum action_tile_types, int, int, int, int, int));
E int FDECL(get_player_action_glyph_offset, (enum action_tile_types));
E int FDECL(get_monster_action_glyph_offset, (enum action_tile_types, int));
#if 0
E int FDECL(maybe_get_replaced_tile, (int, int, int, struct replacement_info, enum autodraw_types*));
#endif
E int FDECL(maybe_get_replaced_glyph, (int, int, int, struct replacement_info));
E int FDECL(maybe_get_animated_tile, (int, int, enum animation_play_types, long, int*, int*, char*, enum autodraw_types*));
E int FDECL(get_tile_animation_index_from_glyph, (int));
E int FDECL(get_replacement_base_tile, (SHORT_P));
E int FDECL(get_animation_base_tile, (SHORT_P));
E int FDECL(get_enlargement_base_tile, (SHORT_P, SHORT_P));
E struct replacement_info FDECL(data_to_replacement_info, (int, int, struct obj*, struct monst*, unsigned long, unsigned long, UCHAR_P, SHORT_P));
E int FDECL(get_animation_frame_with_tile, (int, int));
E void FDECL(play_special_effect_at, (enum special_effect_types, int, int, int, BOOLEAN_P));
E void FDECL(special_effect_wait_until_action, (int));
E void FDECL(special_effect_wait_until_end, (int));
E int FDECL(get_u_move_speed, (BOOLEAN_P));
E void reduce_counters(int);
E void reduce_counters_intervals(int);
E void NDECL(stop_animations);
E boolean FDECL(glyph_is_specific_cmap_or_its_variation, (int, int));
E boolean FDECL(no_wall_end_autodraw, (int, int));

/* ### apply.c ### */

E int NDECL(doapply);
E int NDECL(dobreak);
E int NDECL(dorub);
E int NDECL(dojump);
E int FDECL(jump, (int));
E int NDECL(number_leashed);
E void FDECL(o_unleash, (struct obj *));
E void FDECL(m_unleash, (struct monst *, BOOLEAN_P));
E void NDECL(unleash_all);
E boolean NDECL(next_to_u);
E struct obj *FDECL(get_mleash, (struct monst *));
E const char *NDECL(beautiful);
E void FDECL(check_leash, (XCHAR_P, XCHAR_P));
E boolean FDECL(um_dist, (XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(snuff_candle, (struct obj *));
E boolean FDECL(snuff_torch, (struct obj*));
E boolean FDECL(snuff_lit, (struct obj *));
E boolean FDECL(catch_lit, (struct obj *));
E void FDECL(use_unicorn_horn, (struct obj *));
E int FDECL(use_golf_club, (struct obj*));
E boolean FDECL(tinnable, (struct obj *));
E void NDECL(reset_trapset);
E void FDECL(fig_transform, (ANY_P *, long));
E int FDECL(unfixable_trouble_count, (BOOLEAN_P));
E int FDECL(uthitm, (struct monst*, struct obj*, struct monst*));
E int FDECL(uthito, (struct obj*, struct obj*, struct monst*));
E int FDECL(endlessarrows, (struct obj*, int, int));
E struct obj* FDECL(elemental_enchant_quan, (struct obj*, int, UCHAR_P));
E void FDECL(use_magic_whistle, (struct obj*));
E int FDECL(count_other_containers, (struct obj*, struct obj*, struct obj**, BOOLEAN_P));
E struct obj* FDECL(select_other_container, (struct obj*, struct obj*, BOOLEAN_P, BOOLEAN_P));
E void FDECL(get_pole_type_weapon_min_max_distances, (struct obj*, struct monst*, int*, int*));
E int FDECL(use_grail, (struct obj*, BOOLEAN_P));
E int FDECL(use_salve, (struct obj*, BOOLEAN_P));
E int NDECL(floorapply);
E int FDECL(use_lever, (struct trap*));
E void FDECL(setapplyclasses, (char*));
E void FDECL(setbreakclasses, (char*));
E void FDECL(use_candelabrum, (struct obj*));
E int FDECL(use_candle, (struct obj**));
E int FDECL(use_torch, (struct obj**));
E void FDECL(use_lamp, (struct obj*));
E void FDECL(light_cocktail, (struct obj**));

/* ### artifact.c ### */

E void NDECL(init_artifacts);
E void FDECL(save_artifacts, (int));
E void NDECL(reset_artifacts);
E void FDECL(restore_artifacts, (int));
E const char *FDECL(artiname, (SHORT_P));
E struct obj *FDECL(mk_artifact, (struct obj *, ALIGNTYP_P, UCHAR_P));
E const char *FDECL(artifact_name, (const char *, short *));
E boolean FDECL(exist_artifact, (int, const char *));
E void FDECL(artifact_exists, (struct obj *, const char *, BOOLEAN_P));
E int FDECL(get_artifact_id, (int, const char*));
E int NDECL(nartifact_exist);
E boolean FDECL(arti_immune, (struct obj *, int));
E boolean FDECL(artifact_has_flag, (struct obj *, unsigned long));
E boolean FDECL(artifact_confers_monster_power, (struct monst*, struct obj*, int));
E boolean FDECL(confers_luck, (struct obj *));
E boolean FDECL(confers_unluck, (struct obj*));
E boolean FDECL(artifact_confers_luck, (struct obj*));
E boolean FDECL(artifact_confers_unluck, (struct obj*));
E boolean FDECL(arti_reflects, (struct obj *));
E boolean FDECL(shade_glare, (struct obj *));
E boolean FDECL(restrict_name, (struct obj *, const char *));
E void FDECL(set_artifact_intrinsic, (struct obj *, long));
E int FDECL(touch_artifact, (struct obj *, struct monst *));
E int FDECL(spec_abon, (struct obj *, struct monst *));
E double FDECL(spec_dbon, (struct obj *, struct monst *, double));
E void FDECL(discover_artifact, (SHORT_P));
E boolean FDECL(undiscovered_artifact, (SHORT_P));
E int FDECL(disp_artifact_discoveries, (winid));
E int FDECL(artifact_hit, (struct monst *, struct monst *, struct obj *, double *, boolean*, int));
E int FDECL(pseudo_artifact_hit, (struct monst*, struct monst*, struct obj*, int, int, int, short*));
E int NDECL(doinvoke);
E boolean FDECL(finesse_ahriman, (struct obj *));
E void FDECL(arti_speak, (struct obj *));
E boolean FDECL(artifact_light, (struct obj *));
E long FDECL(spec_m2, (struct obj *));
E boolean FDECL(artifact_has_invprop, (struct obj *, UCHAR_P));
E long FDECL(arti_cost, (struct obj *));
E struct obj *FDECL(what_gives, (int));
E const char *FDECL(glow_color, (int));
E const char *FDECL(glow_verb, (int, BOOLEAN_P));
E void FDECL(Sting_effects, (struct obj*, int));
E boolean FDECL(item_has_specific_monster_warning, (struct obj*));
E int FDECL(retouch_object, (struct obj **, BOOLEAN_P));
E void FDECL(retouch_equipment, (int));
E void NDECL(mkot_trap_warn);
E boolean FDECL(is_magic_key, (struct monst *, struct obj *));
E struct obj *FDECL(has_magic_key, (struct monst *));
E boolean FDECL(carried_item_is_giving_monster_power, (struct monst*, struct obj*, int));
E boolean FDECL(worn_item_is_giving_monster_power, (struct monst*, struct obj*, int));
E boolean FDECL(item_is_giving_monster_power, (struct monst*, struct obj*, int));
E boolean FDECL(item_is_giving_power, (struct obj*, int));
E boolean FDECL(carried_artifact_is_giving_monster_power, (struct monst*, struct obj*, int));
E unsigned long FDECL(prop_to_spfx, (int));
E int FDECL(spfx_to_prop, (unsigned long));
E uchar FDECL(prop_to_adtyp, (int));
E int FDECL(adtyp_to_prop, (UCHAR_P));
E struct obj* FDECL(what_gives_monster, (struct monst*, int));
E boolean FDECL(is_immune, (struct monst*, int));
E int NDECL(create_portal);
E const char* FDECL(get_artifact_invoke_name, (int));
E boolean FDECL(is_artifact_applicable_as_axe, (struct obj*));
E int FDECL(artifact_to_obj, (int));
E void FDECL(artifact_taken_away, (int));
E void NDECL(save_initial_artifact_values);


/* ### attrib.c ### */

E uchar FDECL(adjattrib, (int, int, int));
E uchar FDECL(m_adjattrib, (struct monst*, int, int, BOOLEAN_P));
E void FDECL(gainstr, (struct obj *, int, BOOLEAN_P));
E void FDECL(losestr, (int));
E void FDECL(poisontell, (int, BOOLEAN_P));
E void FDECL(poisoned, (const char *, int, const char *, int, BOOLEAN_P, int));
E void FDECL(extra_enchantment_damage, (const char*, UCHAR_P, const char*, BOOLEAN_P));
E void FDECL(change_luck, (int, BOOLEAN_P));
E int FDECL(stone_luck, (BOOLEAN_P));
E boolean FDECL(object_uses_spellbook_wand_flags_and_properties, (struct obj*));
E boolean FDECL(inappropriate_character_type, (struct obj*));
E boolean FDECL(inappropriate_monster_character_type, (struct monst*, struct obj*));
E boolean FDECL(inappropriate_exceptionality, (struct monst*, struct obj*));
E void NDECL(update_extrinsics);
E void NDECL(set_moreluck);
E void NDECL(restore_attrib);
E void FDECL(exercise, (int, BOOLEAN_P));
E void NDECL(exerchk);
E void FDECL(init_attr, (int));
E void NDECL(redist_attr);
E void FDECL(adjabil, (int, int));
E int NDECL(newhp);
E int FDECL(hpmaxadjustment, (BOOLEAN_P));
E int FDECL(m_hpmaxadjustment, (struct monst*, BOOLEAN_P));
E void NDECL(updatemaxhp);
E void FDECL(update_mon_maxhp, (struct monst*));
E void NDECL(updateabon);
E void FDECL(update_mon_abon, (struct monst*));
E schar FDECL(acurr, (int));
E schar FDECL(m_acurr, (struct monst*, int));
E schar NDECL(acurrstr);
E schar FDECL(m_acurrstr, (struct monst*));
E schar FDECL(acurrstr_base, (int));
E boolean FDECL(extremeattr, (int));
E void FDECL(adjalign, (int));
E int FDECL(is_innate, (int));
E char *FDECL(from_what, (int));
E void FDECL(uchangealign, (int, int));
E const struct innate* FDECL(race_abil, (int));
E const struct innate* FDECL(role_abil, (int));
E boolean FDECL(is_obj_worn, (struct obj*));

/* ### ball.c ### */

E void FDECL(ballrelease, (BOOLEAN_P));
E void NDECL(ballfall);
E void NDECL(placebc);
E void NDECL(unplacebc);
E void FDECL(set_bc, (int));
E void FDECL(move_bc, (int, int, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(drag_ball, (XCHAR_P, XCHAR_P, int *, xchar *, xchar *,
                            xchar *, xchar *, boolean *, BOOLEAN_P));
E void FDECL(drop_ball, (XCHAR_P, XCHAR_P));
E void NDECL(drag_down);
E void NDECL(bc_sanity_check);

/* ### bones.c ### */

E void FDECL(sanitize_name, (char *));
E void FDECL(drop_upon_death, (struct monst *, struct obj *, int, int));
E boolean NDECL(can_make_bones);
E void FDECL(savebones, (int, time_t, struct obj *));
E int NDECL(getbones);

/* ### botl.c ### */

E void NDECL(reset_blstats);
E char* FDECL(get_strength_string, (int));
E const char* NDECL(rank);
E char *NDECL(do_statusline1);
E void NDECL(check_gold_symbol);
E char *NDECL(do_statusline2);
E char* NDECL(do_statusline3);
E void NDECL(bot);
E void NDECL(timebot);
E int FDECL(xlev_to_rank, (int));
E const char *FDECL(rank_of, (int, SHORT_P, BOOLEAN_P));
E int FDECL(title_to_mon, (const char *, int *, size_t*));
E void NDECL(max_rank_sz);
E long NDECL(botl_score);
E int FDECL(describe_level, (char *));
E int FDECL(describe_mode, (char*));
E void FDECL(status_initialize, (BOOLEAN_P));
E void NDECL(status_finish);
E int NDECL(stat_cap_indx);
E int NDECL(stat_hunger_indx);
E const char *FDECL(bl_idx_to_fldname, (int));
#ifdef STATUS_HILITES
E void NDECL(status_eval_next_unhilite);
E void NDECL(reset_status_hilites);
E boolean FDECL(parse_status_hl1, (char *op, BOOLEAN_P));
E void FDECL(status_notify_windowport, (BOOLEAN_P));
E void NDECL(clear_status_hilites);
E int NDECL(count_status_hilites);
E boolean NDECL(status_hilite_menu);
E void NDECL(botl_save_hilites);
E void NDECL(botl_restore_hilites);
#endif
E const char* FDECL(get_condition_name, (unsigned long));
E unsigned long NDECL(get_u_condition_bits);
E unsigned long FDECL(get_m_status_bits, (struct monst*, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E unsigned long FDECL(get_m_condition_bits, (struct monst*));
E void FDECL(get_m_buff_bits, (struct monst*, unsigned long*, BOOLEAN_P));
E void NDECL(status_reassess);
E char* NDECL(botl_realtime);
E long NDECL(get_current_game_duration);
E size_t FDECL(print_conditions, (char*));
E void FDECL(compose_partystatline, (char*, char*, char*, char*, char*));


/* ### cmd.c ### */

E char NDECL(randomkey);
E void FDECL(random_response, (char *, int));
E int NDECL(rnd_extcmd_idx);
E int NDECL(dodeletesavedgame);
E int NDECL(doconduct);
E int NDECL(domonsterability);
E int NDECL(domonability);
E int NDECL(doability);
E int NDECL(dospellmainmenu);
E int NDECL(docommandmenu);
E char FDECL(cmd_from_func, (int NDECL((*))));
E uchar FDECL(cmd_from_txt, (const char*));
E int FDECL(ext_cmd_from_txt, (const char*));
E boolean FDECL(redraw_cmd, (CHAR_P));
#ifdef USE_TRAMPOLI
E int NDECL(doextcmd);
E int NDECL(domonability);
E int NDECL(doprev_message);
E int NDECL(timed_occupation);
E int NDECL(doattributes);
E int NDECL(dopolyformstatistics);
E int NDECL(doviewpetstatistics);
E int NDECL(wiz_detect);
E int NDECL(wiz_files);
E int NDECL(wiz_genesis);
E int NDECL(wiz_identify);
E int NDECL(wiz_level_tele);
E int NDECL(wiz_map);
E int NDECL(wiz_where);
E int NDECL(wiz_wish);
#endif /* USE_TRAMPOLI */
E void NDECL(reset_occupations);
E void FDECL(set_occupation, (int (*)(void), const char *, enum object_soundset_types, enum object_occupation_types, enum occupation_sound_types, int));
E char NDECL(pgetchar);
E void FDECL(pushch, (CHAR_P));
E void FDECL(savech, (CHAR_P));
E const char *FDECL(key2extcmddesc, (UCHAR_P));
E boolean FDECL(bind_specialkey, (UCHAR_P, const char *));
E char FDECL(txt2key, (char *));
E void FDECL(parseautocomplete, (char *, BOOLEAN_P));
E void FDECL(reset_commands, (BOOLEAN_P));
E void FDECL(rhack, (char *));
E void NDECL(reduce_last_item_show_duration);
E int NDECL(doextlist);
E int NDECL(extcmd_via_menu);
E int NDECL(enter_explore_mode);
E void FDECL(enlightenment, (int, int));
E void FDECL(youhiding, (BOOLEAN_P, int));
E void FDECL(show_conduct, (int));
E boolean FDECL(bind_key, (UCHAR_P, const char *));
E void NDECL(dokeylist);
E int FDECL(xytod, (SCHAR_P, SCHAR_P));
E void FDECL(dtoxy, (coord *, int));
E int FDECL(movecmd, (CHAR_P));
E int NDECL(dxdy_moveok);
E int FDECL(getdir, (const char *));
E void NDECL(confdir);
E const char *FDECL(directionname, (int));
E int FDECL(isok, (int, int));
E int FDECL(get_adjacent_loc,
            (const char *, const char *, XCHAR_P, XCHAR_P, coord *));
E const char *FDECL(click_to_cmd, (int, int, int));
E char FDECL(get_count, (char *, CHAR_P, long, long *, BOOLEAN_P));
#ifdef HANGUPHANDLING
E void FDECL(hangup, (int));
E void NDECL(end_of_input);
#endif
E char NDECL(readchar);
E void NDECL(sanity_check);
E char* FDECL(key2txt, (UCHAR_P, char *));
E char FDECL(yn_function, (const char*, const char*, CHAR_P, const char*));
E char FDECL(yn_function_ex, (int, int, int, int, const char*, const char *, const char *, CHAR_P, const char*, const char*, unsigned long));
E char FDECL(yn_function_mon, (struct monst*, const char*, const char*, CHAR_P, const char*));
E char FDECL(yn_query_mon, (struct monst*, const char*));
E char FDECL(ynq_mon, (struct monst*, const char*));
E char FDECL(ynaq_mon, (struct monst*, const char*));
E boolean FDECL(paranoid_query, (BOOLEAN_P, const char *));
E boolean FDECL(paranoid_query_ex, (int, int, BOOLEAN_P, const char*, const char*));
E boolean FDECL(object_stats_known, (struct obj*));
E int NDECL(dozoomin);
E int NDECL(dozoomout);
E int NDECL(dozoomnormal);
E int NDECL(dozoommini);
E int NDECL(dozoomhalf);
E void FDECL(zoomtoscale, (double));
E void NDECL(update_bindings_list);
E void FDECL(create_context_menu, (enum create_context_menu_types));
E int NDECL(doviewpet);
E int NDECL(domarkautostash);
E int NDECL(dounmarkautostash);
E struct monst* FDECL(spotted_linedup_monster_in_way, (int, int, int, int));
E int NDECL(do_gamelog);

/* ### dbridge.c ### */

E boolean FDECL(is_pool, (int, int));
E boolean FDECL(is_lava, (int, int));
E boolean FDECL(is_pool_or_lava, (int, int));
E boolean FDECL(is_ice, (int, int));
E boolean FDECL(is_moat, (int, int));
E schar FDECL(db_under_typ, (int));
E int FDECL(is_drawbridge_wall, (int, int));
E boolean FDECL(is_db_wall, (int, int));
E boolean FDECL(find_drawbridge, (int *, int *));
E boolean FDECL(create_drawbridge, (int, int, int, BOOLEAN_P));
E void FDECL(open_drawbridge, (int, int));
E void FDECL(maybe_close_drawbridge, (int, int));
E void FDECL(close_drawbridge, (int, int));
E void FDECL(destroy_drawbridge, (int, int, BOOLEAN_P));
E void NDECL(reset_drawbridge);

/* ### decl.c ### */

E void NDECL(decl_init);

/* ### detect.c ### */

E boolean FDECL(trapped_chest_at, (int, int, int));
E boolean FDECL(trapped_door_at, (int, int, int));
E struct obj *FDECL(o_in, (struct obj *, CHAR_P));
E struct obj *FDECL(o_material, (struct obj *, unsigned));
E int FDECL(gold_detect, (struct obj *));
E int FDECL(food_detect, (struct obj *));
E int FDECL(object_detect, (struct obj *, int));
E int FDECL(monster_detect, (struct obj *, int));
E int FDECL(trap_detect, (struct obj *));
E const char *FDECL(level_distance, (d_level *));
E void FDECL(use_crystal_ball, (struct obj **));
E void NDECL(do_mapping);
E void FDECL(do_vicinity_map, (struct obj *));
E void FDECL(cvt_sdoor_to_door, (int, int));
#ifdef USE_TRAMPOLI
E void FDECL(findone, (int, int, genericptr_t));
E void FDECL(openone, (int, int, genericptr_t));
#endif
E int NDECL(findit);
E int NDECL(openit);
E boolean FDECL(detecting, (void (*)(int, int, genericptr)));
E void FDECL(find_trap, (struct trap *));
E void NDECL(warnreveal);
E int FDECL(dosearch0, (int));
E int NDECL(dosearch);
E void NDECL(sokoban_detect);
#if defined(DUMPLOG) || defined(DUMPHTML)
E void NDECL(dump_map);
#endif
E void FDECL(reveal_terrain, (int, int));

/* ### dig.c ### */

E int NDECL(dodig);
E int FDECL(dig_typ, (struct obj *, XCHAR_P, XCHAR_P));
E boolean NDECL(is_digging);
#ifdef USE_TRAMPOLI
E int NDECL(dig);
#endif
E int NDECL(holetime);
E boolean FDECL(dig_check, (struct monst *, BOOLEAN_P, int, int));
E void FDECL(digactualhole, (int, int, struct monst *, int));
E boolean FDECL(dighole, (BOOLEAN_P, BOOLEAN_P, coord *));
E int FDECL(use_pick_axe, (struct obj *));
E int FDECL(use_pick_axe2, (struct obj *));
E boolean FDECL(mdig_tunnel, (struct monst *));
E void FDECL(draft_message, (BOOLEAN_P));
E void FDECL(watch_dig, (struct monst *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(zap_dig, (struct obj*));
E void FDECL(zap_evaporation, (struct obj*));
E struct obj *FDECL(bury_an_obj, (struct obj *, boolean *));
E void FDECL(bury_objs, (int, int));
E int FDECL(unearth_objs, (struct monst*, int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(rot_organic, (ANY_P *, long));
E void FDECL(rot_corpse, (ANY_P *, long));
E struct obj *FDECL(buried_ball, (coord *));
E void NDECL(buried_ball_to_punishment);
E void NDECL(buried_ball_to_freedom);
E schar FDECL(fillholetyp, (int, int, BOOLEAN_P));
E void FDECL(liquid_flow,
             (XCHAR_P, XCHAR_P, SCHAR_P, struct trap *, const char *));
E boolean FDECL(conjoined_pits, (struct trap *, struct trap *, BOOLEAN_P));
E void FDECL(uncatch_tree_objects, (int, int));
E void NDECL(reset_dig);

#if 0
E void FDECL(bury_monst, (struct monst *));
E void NDECL(bury_you);
E void NDECL(unearth_you);
E void NDECL(escape_tomb);
E void FDECL(bury_obj, (struct obj *));
#endif

/* ### display.c ### */

E void FDECL(magic_map_background, (XCHAR_P, XCHAR_P, int));
E void FDECL(map_background, (XCHAR_P, XCHAR_P, int));
E void FDECL(map_trap, (struct trap *, int));
E void FDECL(map_object, (struct obj *, int));
E void FDECL(map_object_for_detection, (struct obj*, int));
E void FDECL(map_object_no_chain_check, (struct obj*, int));
E void FDECL(map_object_no_chain_check_choose_ascii, (struct obj*, int, BOOLEAN_P));
E void FDECL(map_object_core, (struct obj*, int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(map_invisible, (XCHAR_P, XCHAR_P));
E boolean FDECL(unmap_invisible, (int, int));
E void FDECL(unmap_object, (int, int));
E void FDECL(clear_monster_layerinfo, (struct layer_info*));
E void FDECL(map_location, (int, int, int));
E void FDECL(feel_newsym, (XCHAR_P, XCHAR_P));
E void FDECL(feel_location, (XCHAR_P, XCHAR_P));
E void FDECL(newsym, (int, int));
E void FDECL(newsym_with_flags, (int, int, unsigned long));
E void FDECL(newsym_with_extra_info, (int, int, unsigned long, int, int));
E void FDECL(newsym_with_extra_info_and_flags, (int, int, unsigned long, int, int, unsigned long));
E void FDECL(newsym_force, (int, int));
E void FDECL(force_redraw_at, (int, int));
E void FDECL(shieldeff, (XCHAR_P, XCHAR_P));
E void FDECL(talkeff, (XCHAR_P, XCHAR_P));
E void FDECL(tmp_at, (int, int));
E void FDECL(tmp_at_with_missile_flags, (int, int, unsigned long, UCHAR_P, SHORT_P));
E void FDECL(tmp_at_with_obj, (int, int, struct obj*, unsigned long, UCHAR_P, SHORT_P));
E void FDECL(swallowed, (int));
E void FDECL(under_ground, (int));
E void FDECL(under_water, (int));
E void NDECL(see_monsters);
E void NDECL(set_mimic_blocking);
E void NDECL(see_objects);
E void NDECL(see_traps);
E void NDECL(curs_on_u);
E int NDECL(doredraw);
E void NDECL(docrt);
E void NDECL(clear_memory_object_detection_marks);
E void NDECL(redraw_map);
E void FDECL(show_glyph, (int, int, int));
E void FDECL(clear_current_and_show_glyph, (int, int, int));
E void FDECL(maybe_clear_and_show_glyph, (int, int, int, BOOLEAN_P));
E void FDECL(show_gui_glyph_on_layer, (int, int, int, int, enum layer_types));
E void FDECL(show_glyph_on_layer, (int, int, int, enum layer_types));
E void FDECL(show_glyph_ascii, (int, int, int));
E void FDECL(show_glyph_on_layer_and_ascii, (int, int, int, enum layer_types));
E void FDECL(show_gui_glyph_on_layer_and_ascii, (int, int, int, int, enum layer_types));
E void FDECL(clear_glyph_buffer_at, (int, int));
E void FDECL(show_monster_glyph_with_extra_info, (int, int, int, struct monst*, unsigned long, int, int));
E void FDECL(show_monster_glyph_with_extra_info_choose_ascii, (int, int, int, struct monst*, int, int, unsigned long, int, int, BOOLEAN_P));
E void FDECL(clear_monster_extra_info, (int, int));
E void FDECL(clear_monster_layer_at, (int, int));
E void FDECL(clear_monster_layer_memory_at, (int, int));
E void FDECL(show_extra_info, (int, int, unsigned long, int, int));
E void FDECL(show_missile_info, (int, int, UCHAR_P, UCHAR_P, SHORT_P, UCHAR_P, UCHAR_P, UCHAR_P, UCHAR_P, UCHAR_P, UCHAR_P, unsigned long, SHORT_P, XCHAR_P, XCHAR_P));
E unsigned long FDECL(get_missile_flags, (struct obj*, BOOLEAN_P));
E void FDECL(show_leash_info, (int, int, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E void FDECL(clear_layer_info, (struct layer_info*));
E void FDECL(clear_all_glyphs_at, (int, int));
E void FDECL(clear_hero_memory_at, (int, int));
E void FDECL(clear_object_glyphs_at, (int, int));
E void FDECL(change_layer_damage_displayed, (int, int, int));
E void FDECL(change_layer_hit_tile, (int, int, int));
E void FDECL(add_glyph_buffer_layer_flags, (int, int, unsigned long));
E void FDECL(remove_glyph_buffer_layer_flags, (int, int, unsigned long));
E void FDECL(set_glyph_buffer_object_height, (int, int, SHORT_P));
E void FDECL(set_glyph_buffer_feature_doodad_height, (int, int, SCHAR_P));
E void FDECL(set_glyph_buffer_oid, (int, int, int));
E void FDECL(add_or_remove_glyph_to_layer, (int, int, int, BOOLEAN_P));
E void FDECL(add_glyph_to_layer, (int, int, int));
E void FDECL(remove_current_glyph_from_layer, (int, int));
E void NDECL(clear_glyph_buffer);
E void FDECL(row_refresh, (int, int, int));
E void NDECL(cls);
E void NDECL(show_memory_everywhere);
E void NDECL(show_detection_everywhere);
E void NDECL(show_memory_and_detection_everywhere);
E void FDECL(flush_screen, (int));
E int FDECL(back_to_glyph, (XCHAR_P, XCHAR_P));
E int FDECL(back_to_broken_glyph, (XCHAR_P, XCHAR_P));
E int FDECL(zapdir_to_glyph, (int, int, int));
E int FDECL(zapbounce_to_glyph, (int, int, int, int, int));
E int FDECL(zap_glyph_to_corner_glyph, (int, unsigned long, int));
E int FDECL(dir_to_beam_index, (int, int));
E int FDECL(glyph_at, (XCHAR_P, XCHAR_P));
E struct layer_info FDECL(layers_at, (XCHAR_P, XCHAR_P));
E void NDECL(set_wall_state);
E void FDECL(unset_seenv, (struct rm *, int, int, int, int));
E int FDECL(warning_of, (struct monst *));
E int NDECL(get_current_cmap_type_index);
E int FDECL(get_missile_index, (int, int));
E void FDECL(display_self_with_extra_info_choose_ascii, (unsigned long, int, int, BOOLEAN_P));
E int FDECL(get_location_light_range, (XCHAR_P, XCHAR_P));
E int FDECL(get_location_light_sidedness, (XCHAR_P, XCHAR_P));
E void NDECL(u_shieldeff);
E void FDECL(m_shieldeff, (struct monst*));
E boolean NDECL(use_extra_special_staircase);
E int FDECL(wall_angle, (struct rm*));
E short FDECL(get_obj_height, (struct obj*));
E boolean FDECL(generic_has_floor_tile, (struct obj*));
E void FDECL(update_tile_gui_info, (BOOLEAN_P, struct monst*, int, int, unsigned long));
E void FDECL(refresh_u_tile_gui_info, (BOOLEAN_P));
E void FDECL(refresh_m_tile_gui_info, (struct monst*, BOOLEAN_P));
E void FDECL(set_obj_glyph, (struct obj*));
E int FDECL(get_seen_monster_glyph, (struct monst*));
E void NDECL(reset_display);
E const char* FDECL(get_decoration_description, (int, int));
E const char* FDECL(get_carpet_description, (int, int));

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(drop, (struct obj *));
E int NDECL(wipeoff);
#endif
E int NDECL(dodrop);
E int NDECL(doitemdescriptions);
E int NDECL(docharacterstatistics);
E const char* FDECL(get_damage_type_text, (SHORT_P));
E const char* FDECL(get_defense_type_text, (int));
E const char* FDECL(get_monster_size_text, (int));
E const char* FDECL(get_attack_type_text, (int));
E boolean FDECL(boulder_hits_pool, (struct obj *, int, int, BOOLEAN_P));
E boolean FDECL(flooreffects, (struct obj *, int, int, const char *));
E void FDECL(doaltarobj, (struct obj *));
E boolean FDECL(canletgo, (struct obj *, const char *));
E int FDECL(drop, (struct obj*));
E void FDECL(dropx, (struct obj *));
E void FDECL(dropy, (struct obj *));
E void FDECL(dropz, (struct obj *, BOOLEAN_P));
E void FDECL(obj_no_longer_held, (struct obj *));
E int NDECL(doddrop);
E int NDECL(dodropmany);
E int NDECL(dodown);
E int NDECL(doup);
#ifdef INSURANCE
E void NDECL(save_currentstate);
#endif
E void FDECL(u_collide_m, (struct monst *));
E void FDECL(goto_level, (d_level *, UCHAR_P, BOOLEAN_P, XCHAR_P));
E void FDECL(schedule_goto, (d_level *, UCHAR_P, BOOLEAN_P, BOOLEAN_P, long,
                             const char *, const char *));
E void NDECL(deferred_goto);
E boolean FDECL(revive_corpse, (struct obj *));
E boolean FDECL(animate_corpse, (struct obj*, int));
E void FDECL(revive_mon, (ANY_P *, long));
E int NDECL(donull);
E int NDECL(dowipe);
E void FDECL(set_wounded_legs, (long, int));
E void FDECL(heal_legs, (int));
E boolean NDECL(floorexamine);
E int FDECL(itemdescription, (struct obj*));
E int FDECL(corpsedescription, (struct obj*));
E void FDECL(printweight, (char*, int, BOOLEAN_P, BOOLEAN_P));
E int FDECL(monsterdescription, (struct monst*));
E int NDECL(dotogglehpbars);
E int NDECL(dotogglegrid);
E int NDECL(dotogglemonstertargeting);
E int NDECL(dotoggleumark);
E int NDECL(dotogglebufftimers);
E void FDECL(delete_location, (XCHAR_P, XCHAR_P));
E void FDECL(delete_decoration, (XCHAR_P, XCHAR_P));
E void FDECL(full_location_transform, (XCHAR_P, XCHAR_P, int, int, int, UNSIGNED_SHORT_P, SCHAR_P, SCHAR_P, UCHAR_P, SCHAR_P, SCHAR_P, SCHAR_P, UCHAR_P, int, int, int, int, BOOLEAN_P, BOOLEAN_P, SHORT_P, SHORT_P, BOOLEAN_P));
E void FDECL(full_initial_location_transform, (XCHAR_P, XCHAR_P, int, UNSIGNED_SHORT_P, SCHAR_P, SCHAR_P, UCHAR_P, SCHAR_P, SCHAR_P, SCHAR_P, UCHAR_P, int, int, BOOLEAN_P, BOOLEAN_P, SHORT_P, SHORT_P, BOOLEAN_P));
E void FDECL(create_simple_location, (XCHAR_P, XCHAR_P, int, int, int, UNSIGNED_SHORT_P, int, int, int, int, BOOLEAN_P));
E void FDECL(create_simple_initial_location, (XCHAR_P, XCHAR_P, int, UNSIGNED_SHORT_P, int, int, BOOLEAN_P));
E void FDECL(create_location_with_current_floor, (XCHAR_P, XCHAR_P, int, int, int, UNSIGNED_SHORT_P, int, BOOLEAN_P));
E void FDECL(create_initial_location_with_current_floor, (XCHAR_P, XCHAR_P, int, UNSIGNED_SHORT_P, int, BOOLEAN_P));
E void FDECL(create_current_floor_location, (XCHAR_P, XCHAR_P, UNSIGNED_SHORT_P, int, BOOLEAN_P));
E void FDECL(create_basic_floor_location, (XCHAR_P, XCHAR_P, int, int, UNSIGNED_SHORT_P, BOOLEAN_P));
E void FDECL(transform_location_type, (XCHAR_P, XCHAR_P, int, int));
E void FDECL(transform_location_type_and_flags, (XCHAR_P, XCHAR_P, int, int, UNSIGNED_SHORT_P));
E void FDECL(transform_location_type_and_flags_and_set_broken, (XCHAR_P, XCHAR_P, int, int, UNSIGNED_SHORT_P, int));
E const char* FDECL(get_obj_subtype_name, (struct obj*));
E const char* FDECL(get_otyp_subtype_name, (int));
E struct extended_create_window_info FDECL(extended_create_window_info_from_obj, (struct obj*));
E struct extended_create_window_info FDECL(extended_create_window_info_from_mon, (struct monst*));
E struct extended_create_window_info FDECL(extended_create_window_info_from_mon_with_flags, (struct monst*, unsigned long));
E void FDECL(hint_via_pline, (const char*));
E void FDECL(standard_hint, (const char*, boolean*));
E void FDECL(reviver_hint, (struct monst*));
E void FDECL(brain_hint, (struct monst*));
E void FDECL(grab_hint, (struct monst*));
E void FDECL(pray_hint, (const char*, const char*, boolean*));
E void NDECL(death_hint);
E void NDECL(check_mobbed_hint);
E void FDECL(item_destruction_hint, (int, BOOLEAN_P));
E void NDECL(heal_ailments_upon_revival);

/* ### do_name.c ### */

E char *FDECL(coord_desc, (int, int, char *, CHAR_P));
E boolean FDECL(getpos_menu, (coord *, int));
E int FDECL(getpos, (coord *, BOOLEAN_P, const char *, enum game_cursor_types));
E void FDECL(getpos_sethilite, (void (*f)(int), boolean (*d)(int,int)));
E void FDECL(new_mname, (struct monst *, size_t));
E void FDECL(free_mname, (struct monst *));
E void FDECL(new_umname, (struct monst*, size_t));
E void FDECL(free_umname, (struct monst*));
E void FDECL(new_oname, (struct obj *, size_t));
E void FDECL(free_oname, (struct obj *));
E const char *FDECL(safe_oname, (struct obj *));
E void FDECL(new_uoname, (struct obj*, size_t));
E void FDECL(free_uoname, (struct obj*));
E const char* FDECL(safe_uoname, (struct obj*));
E struct monst *FDECL(christen_monst, (struct monst *, const char *));
E struct monst* FDECL(u_name_monst, (struct monst*, const char*));
E struct obj *FDECL(oname, (struct obj *, const char *));
E struct obj* FDECL(uoname, (struct obj*, const char*));
E boolean FDECL(objtyp_is_callable, (int));
E int NDECL(docallcmd);
E int NDECL(doname_specific_object);
E int NDECL(doname_type_of_object);
E void FDECL(docall, (struct obj *, const char*));
E const char *NDECL(rndghostname);
E char *FDECL(x_monnam, (struct monst *, int, const char *, int, BOOLEAN_P));
E char *FDECL(l_monnam, (struct monst *));
E char *FDECL(mon_nam, (struct monst *));
E char *FDECL(noit_mon_nam, (struct monst *));
E char* FDECL(noittame_mon_nam, (struct monst*));
E char *FDECL(Monnam, (struct monst *));
E char *FDECL(noit_Monnam, (struct monst *));
E char* FDECL(noittame_Monnam, (struct monst*));
E char *FDECL(noname_monnam, (struct monst *, int));
E char *FDECL(m_monnam, (struct monst *));
E char *FDECL(y_monnam, (struct monst *));
E char* FDECL(Ymonnam, (struct monst*));
E char *FDECL(Adjmonnam, (struct monst *, const char *));
E char *FDECL(Amonnam, (struct monst *));
E char *FDECL(a_monnam, (struct monst *));
E char *FDECL(distant_monnam, (struct monst *, int, char *));
E char *FDECL(rndmonnam, (char *));
E const char *FDECL(hcolor, (const char *));
E const char *NDECL(rndcolor);
E const char *FDECL(hliquid, (const char *));
E const char *NDECL(roguename);
E struct obj *FDECL(realloc_obj, (struct obj *, int, genericptr_t, int, const char *));
E char *FDECL(coyotename, (struct monst *, char *));
E char *FDECL(rndorcname, (char *));
E struct monst *FDECL(christen_orc, (struct monst *, const char *, const char *));
E const char *FDECL(noveltitle, (short *, unsigned long, unsigned long));
E const char* FDECL(manualtitle, (short*, unsigned long, unsigned long));
E const char *FDECL(lookup_novel, (const char *, short*));
E const char* FDECL(lookup_manual, (const char*, short*));
E char* FDECL(randomize_dwarf_name, (char*));
E char* FDECL(randomize_elf_name, (char*));
E char* FDECL(randomize_gnome_name, (char*));
E char* FDECL(randomize_hobbit_name, (char*));
E char* FDECL(randomize_gnoll_name, (char*));
E char* FDECL(randomize_flind_name, (char*));
E char* FDECL(randomize_male_human_name, (char*));
E char* FDECL(randomize_female_human_name, (char*));
E char* FDECL(randomize_demon_name, (char*));
E char* FDECL(randomize_undead_spellcaster_name, (char*));
E char* FDECL(randomize_angel_name, (char*));
E char* FDECL(randomize_modron_name, (char*));
#ifdef ANDROID
E void FDECL(and_getlin_log, (const char*, char*));
#endif
E void FDECL(read_manual, (struct obj*));
E void NDECL(reset_doname);

/* ### do_wear.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(Armor_on);
E int NDECL(Boots_on);
E int NDECL(Gloves_on);
E int NDECL(Helmet_on);
E int FDECL(select_off, (struct obj *));
E int NDECL(take_off);
#endif
E void FDECL(off_msg, (struct obj *));
E void FDECL(set_wear, (struct obj *));
E boolean FDECL(donning, (struct obj *));
E boolean FDECL(doffing, (struct obj *));
E void FDECL(cancel_doff, (struct obj *, long));
E void NDECL(cancel_don);
E int FDECL(stop_donning, (struct obj *));
E int NDECL(Armor_off);
E int NDECL(Armor_gone);
E int NDECL(Helmet_off);
E int NDECL(Gloves_off);
E int NDECL(Boots_on);
E int NDECL(Boots_off);
E int NDECL(Cloak_off);
E int NDECL(Shield_off);
E int NDECL(Shirt_off);
E int NDECL(Robe_off);
E int NDECL(Bracers_off);
E int FDECL(MiscellaneousItem_off, (struct obj *));
E void NDECL(Amulet_off);
E void FDECL(Ring_on, (struct obj *));
E void FDECL(Ring_off, (struct obj *));
E void FDECL(Ring_gone, (struct obj *));
E void FDECL(Blindf_on, (struct obj *));
E void FDECL(Blindf_off, (struct obj *));
E int NDECL(dotakeoff);
E int NDECL(doremring);
E int FDECL(cursed, (struct obj *));
E int FDECL(armoroff, (struct obj *));
E int FDECL(canwearobj, (struct obj *, long *, BOOLEAN_P, BOOLEAN_P));
E int NDECL(dowear);
E int NDECL(doputon);
E void NDECL(find_ac);
E void NDECL(find_mc);
E void NDECL(glibr);
E struct obj *FDECL(some_armor, (struct monst *));
E struct obj *FDECL(stuck_ring, (struct obj *, int));
E struct obj *NDECL(unchanger);
E void NDECL(reset_remarm);
E int NDECL(doddoremarm);
E int FDECL(destroy_arm, (struct obj *));
E void FDECL(adj_abon, (struct obj *, SCHAR_P));
E boolean FDECL(inaccessible_equipment, (struct obj *, const char *, BOOLEAN_P, BOOLEAN_P));
E int NDECL(get_role_AC_bonus);
E int NDECL(get_role_MC_bonus);
E int FDECL(armor_or_accessory_off, (struct obj*));
E void NDECL(reset_dowear);

/* ### dog.c ### */

E void FDECL(newedog, (struct monst *));
E void FDECL(free_edog, (struct monst *));
E void FDECL(initedog, (struct monst *, BOOLEAN_P));
E struct monst *FDECL(make_familiar,
                      (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E struct monst *NDECL(makedog);
E void NDECL(update_mlstmv);
E void NDECL(losedogs);
E void FDECL(mon_arrive, (struct monst *, BOOLEAN_P));
E void FDECL(mon_catchup_elapsed_time, (struct monst *, long));
E void FDECL(keepdogs, (BOOLEAN_P));
E void FDECL(migrate_to_level, (struct monst *, XCHAR_P, XCHAR_P, coord *));
E int FDECL(dogfood, (struct monst *, struct obj *));
E boolean FDECL(tamedog, (struct monst *, struct obj *, UCHAR_P, int, UNSIGNED_SHORT_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(abuse_dog, (struct monst *));
E void FDECL(wary_dog, (struct monst *, BOOLEAN_P));
E void FDECL(break_charm, (struct monst*, BOOLEAN_P));
E void NDECL(reset_dogs);

/* ### dogmove.c ### */

E struct obj *FDECL(droppables, (struct monst *));
E struct obj* FDECL(m_has_wearable_armor_or_accessory, (struct monst*));
E struct obj* FDECL(m_has_worn_armor_or_accessory, (struct monst*));
E int FDECL(dog_nutrition, (struct monst *, struct obj *));
E int FDECL(dog_eat, (struct monst *, struct obj *, int, int, BOOLEAN_P));
E int FDECL(dog_move, (struct monst *, int));
#ifdef USE_TRAMPOLI
E void FDECL(wantdoor, (int, int, genericptr_t));
#endif
E void FDECL(finish_meating, (struct monst *));
E boolean FDECL(could_reach_item, (struct monst*, XCHAR_P, XCHAR_P));
E void FDECL(m_gainstr, (struct monst*, struct obj*, int, BOOLEAN_P));
E void FDECL(dog_food_after_effect, (struct monst*, struct obj*, BOOLEAN_P));
E boolean FDECL(dog_wants_to_eat, (struct monst*));
E void NDECL(reset_dogmove);

/* ### dokick.c ### */

E boolean FDECL(ghitm, (struct monst *, struct obj *, uchar *));
E void FDECL(container_impact_dmg, (struct obj *, XCHAR_P, XCHAR_P));
E int NDECL(dokick);
#ifdef ANDROID
E void NDECL(autokick);
E int FDECL(dokick_indir, (BOOLEAN_P));
#endif
E boolean FDECL(ship_object, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(obj_delivery, (BOOLEAN_P));
E void FDECL(deliver_obj_to_mon, (struct monst *mtmp, int, unsigned long));
E schar FDECL(down_gate, (XCHAR_P, XCHAR_P));
E void FDECL(impact_drop, (struct obj *, XCHAR_P, XCHAR_P, XCHAR_P));
E int FDECL(kick_object, (XCHAR_P, XCHAR_P, char *, BOOLEAN_P));
E void NDECL(reset_kick);

/* ### dothrow.c ### */

E int NDECL(dothrow);
E int NDECL(dofire);
E void FDECL(endmultishot, (BOOLEAN_P));
E void FDECL(hitfloor, (struct obj *, BOOLEAN_P));
E void FDECL(hurtle, (int, int, int, BOOLEAN_P));
E void FDECL(mhurtle, (struct monst *, int, int, int));
E boolean FDECL(throwing_weapon, (struct obj *));
E boolean FDECL(nonmelee_throwing_weapon, (struct obj*));
E void FDECL(throwit, (struct obj *, long));
E int FDECL(omon_adj, (struct monst *, struct obj *, BOOLEAN_P));
E int FDECL(thitmonst, (struct monst *, struct obj *, BOOLEAN_P, uchar *));
E int FDECL(hero_breaks, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E int FDECL(breaks, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(release_camera_demon, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(breakobj, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(breaktest, (struct obj *));
E boolean FDECL(walk_path, (coord *, coord *,
                            boolean (*)(genericptr, int, int), genericptr_t));
E boolean FDECL(hurtle_jump, (genericptr_t, int, int));
E boolean FDECL(hurtle_step, (genericptr_t, int, int));

E struct multishot_result FDECL(get_multishot_stats, (struct monst*, struct obj*, struct obj*, BOOLEAN_P));
E void FDECL(check_shop_obj, (struct obj*, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E void NDECL(reset_throw);

/* ### do_wear.c ### */
E void FDECL(learnring, (struct obj*, BOOLEAN_P));



/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int FDECL(def_char_to_objclass, (CHAR_P));
E int FDECL(def_char_to_monclass, (CHAR_P));
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(switch_symbols, (int));
E void FDECL(assign_graphics, (int));
E void NDECL(init_r_symbols);
E void NDECL(init_symbols);
E void NDECL(update_bouldersym);
E void NDECL(init_showsyms);
E void NDECL(init_l_symbols);
E void FDECL(clear_symsetentry, (int, BOOLEAN_P));
E void FDECL(update_l_symset, (struct symparse *, nhsym));
E void FDECL(update_r_symset, (struct symparse *, nhsym));
E boolean FDECL(cursed_object_at, (int, int));
E const char* FDECL(get_cmap_tilename, (int));

/* ### dungeon.c ### */

E void FDECL(save_dungeon, (int, BOOLEAN_P, BOOLEAN_P));
E void NDECL(reset_dungeon);
E void FDECL(restore_dungeon, (int));
E void FDECL(insert_branch, (branch *, BOOLEAN_P));
E void NDECL(init_dungeons);
E s_level *FDECL(find_level, (const char *));
E s_level *FDECL(Is_special, (d_level *));
E branch *FDECL(Is_branchlev, (d_level *));
E boolean FDECL(builds_up, (d_level *));
E xchar FDECL(ledger_no, (d_level *));
E xchar NDECL(maxledgerno);
E schar FDECL(depth, (d_level *));
E xchar FDECL(dunlev, (d_level *));
E xchar FDECL(dunlevs_in_dungeon, (d_level *));
E xchar FDECL(ledger_to_dnum, (XCHAR_P));
E xchar FDECL(ledger_to_dlev, (XCHAR_P));
E xchar FDECL(deepest_lev_reached, (BOOLEAN_P));
E boolean FDECL(on_level, (d_level *, d_level *));
E void FDECL(next_level, (BOOLEAN_P));
E void FDECL(prev_level, (BOOLEAN_P));
E void FDECL(u_on_newpos, (int, int));
E void FDECL(u_on_rndspot, (int));
E void FDECL(u_on_sstairs, (int));
E void NDECL(u_on_upstairs);
E void NDECL(u_on_dnstairs);
E boolean FDECL(On_stairs, (XCHAR_P, XCHAR_P));
E void FDECL(get_level, (d_level *, int));
E void FDECL(get_level_in_dungeon, (d_level*, int, XCHAR_P));
E boolean FDECL(Is_botlevel, (d_level *));
E boolean FDECL(Can_fall_thru, (d_level *));
E boolean FDECL(Can_dig_down, (d_level *));
E boolean FDECL(Can_rise_up, (int, int, d_level *));
E boolean FDECL(has_ceiling, (d_level *));
E boolean FDECL(In_quest, (d_level *));
E boolean FDECL(In_mines, (d_level *));
E boolean FDECL(In_modron_level, (d_level*));
E boolean FDECL(In_bovine_level, (d_level*));
E boolean FDECL(In_large_circular_dgn_level, (d_level*));
E branch *FDECL(dungeon_branch, (const char *));
E boolean FDECL(at_dgn_entrance, (const char *));
E boolean FDECL(In_hell, (d_level *));
E boolean FDECL(In_V_tower, (d_level *));
E boolean FDECL(On_W_tower_level, (d_level *));
E boolean FDECL(In_W_tower, (int, int, d_level *));
E void FDECL(find_hell, (d_level *));
E void FDECL(goto_hell, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(assign_level, (d_level *, d_level *));
E void FDECL(assign_rnd_level, (d_level *, d_level *, int));
E int FDECL(induced_align, (int));
E boolean FDECL(Invocation_lev, (d_level *));
E xchar NDECL(level_difficulty);
E schar FDECL(lev_by_name, (const char *));
E schar FDECL(print_dungeon, (BOOLEAN_P, schar *, xchar *));
E char *FDECL(get_annotation, (d_level *));
E int NDECL(donamelevel);
E int NDECL(dooverview);
E void FDECL(show_overview, (int, int));
E void FDECL(forget_mapseen, (int));
E void FDECL(rm_mapseen, (int));
E void FDECL(init_mapseen, (d_level *));
E void NDECL(recalc_mapseen);
E void FDECL(mapseen_temple, (struct monst *));
E void FDECL(room_discovered, (int));
E void FDECL(recbranch_mapseen, (d_level *, d_level *));
E void FDECL(overview_stats, (winid, const char *, long *, size_t *));
E void FDECL(remdun_mapseen, (int));
E const char *FDECL(endgamelevelname, (char *, int));
E mapseen* FDECL(find_mapseen, (d_level*));
E void FDECL(check_special_level_naming_by_mon, (struct monst*));
E void FDECL(set_special_level_seen, (d_level*, BOOLEAN_P));

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(eatmdone);
E int NDECL(eatfood);
E int NDECL(opentin);
E int NDECL(unfaint);
#endif
E void NDECL(eatmupdate);
E boolean FDECL(is_edible, (struct obj *));
E void NDECL(init_uhunger);
E int NDECL(Hear_again);
E void NDECL(reset_eat);
E int NDECL(doeat);
E int FDECL(use_tin_opener, (struct obj *));
E void NDECL(gethungry);
E void FDECL(morehungry, (int));
E void FDECL(lesshungry, (int));
E boolean NDECL(is_fainted);
E void NDECL(reset_faint);
E void NDECL(violated_vegetarian);
E void FDECL(update_hunger_status, (BOOLEAN_P));
E struct obj *FDECL(floorfood, (const char *, int));
E void NDECL(vomit);
E int FDECL(eaten_stat, (int, struct obj *));
E void FDECL(food_disappears, (struct obj *));
E void FDECL(food_substitution, (struct obj *, struct obj *));
E void FDECL(eating_conducts, (struct permonst *));
E int FDECL(eat_brains, (struct monst *, struct monst *, BOOLEAN_P, double *));
E void NDECL(fix_petrification);
E void FDECL(consume_oeaten, (struct obj *, int));
E boolean FDECL(maybe_finished_meal, (BOOLEAN_P));
E void FDECL(set_tin_variety, (struct obj *, int));
E int FDECL(tin_variety_txt, (char *, int *));
E void FDECL(tin_details, (struct obj *, int, char *));
E boolean FDECL(Popeye, (int));
E unsigned FDECL(obj_nutrition, (struct obj*, struct monst*));
E unsigned FDECL(mon_nutrition_size_multiplier, (struct monst*));
E void FDECL(display_nutrition_floating_text, (int, int, int));
E void FDECL(display_famine_floating_text, (int, int, int));
E long FDECL(get_rotted_status, (struct obj*));
E void NDECL(reset_hunger_status);
E boolean FDECL(intrinsic_possible, (int, struct permonst*));

/* ### encounter.c ### */

E void NDECL(encounter_init);
E void FDECL(randomize_encounter, (int, int));
E void FDECL(create_encounter, (int, int, int, int));
E int NDECL(wiz_save_encounters);


/* ### end.c ### */

E void FDECL(done1, (int));
E int NDECL(done2);
#ifdef USE_TRAMPOLI
E void FDECL(done_intr, (int));
#endif
E void FDECL(done_in_by, (struct monst *, int));
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E void VDECL(panic, (const char *, ...)) PRINTF_F(1, 2) NORETURN;
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(done, (int));
E void FDECL(container_contents, (struct obj *, BOOLEAN_P,
                                  BOOLEAN_P, BOOLEAN_P, int));
#ifdef VMS
E void FDECL(nh_terminate, (int));
E void FDECL(nh_bail, (int, const char*, BOOLEAN_P));
#else
E void FDECL(nh_terminate, (int)) NORETURN;
E void FDECL(nh_bail, (int, const char*, BOOLEAN_P)) NORETURN;
#endif
E int NDECL(dovanquished);
E int NDECL(dokilledmonsters);
E int NDECL(dogenocidedmonsters);
E int NDECL(num_genocides);
E void FDECL(delayed_killer, (int, int, const char *, int));
E struct kinfo *FDECL(find_delayed_killer, (int));
E void FDECL(dealloc_killer, (struct kinfo *));
E void FDECL(save_killers, (int, int));
E void NDECL(reset_killers);
E void FDECL(restore_killers, (int));
E char *FDECL(build_english_list, (char *));
#if defined(PANICTRACE) && !defined(NO_SIGNAL)
E void FDECL(panictrace_setsignals, (BOOLEAN_P));
#endif
#ifdef ANDROID
void and_you_die();
#endif
E long NDECL(get_current_game_score);
E int NDECL(wiz_dumplog);
E void FDECL(print_selfies, (winid));
E void NDECL(reset_game);
E void NDECL(reset_gamestate_ex);
E void NDECL(tally_realtime);


/* ### engrave.c ### */

E char *FDECL(random_engraving, (char *));
E void FDECL(wipeout_text, (char *, int, unsigned));
E boolean FDECL(can_reach_floor, (BOOLEAN_P));
E void FDECL(cant_reach_floor, (int, int, BOOLEAN_P, BOOLEAN_P));
E const char *FDECL(surface, (int, int));
E const char *FDECL(ceiling, (int, int));
E struct engr *FDECL(engr_at, (XCHAR_P, XCHAR_P));
E int FDECL(sengr_at, (const char *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(u_wipe_engr, (int));
E void FDECL(wipe_engr_at, (XCHAR_P, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(read_engr_at, (int, int));
E void FDECL(make_engr_at, (int, int, const char *, long, XCHAR_P, UNSIGNED_SHORT_P));
E void FDECL(del_engr_at, (int, int));
E int NDECL(freehand);
E int NDECL(doengrave);
E void NDECL(sanitize_engravings);
E void FDECL(save_engravings, (int, int));
E void NDECL(reset_engravings);
E void FDECL(rest_engravings, (int));
E void FDECL(engr_stats, (const char *, char *, long *, size_t *));
E void FDECL(del_engr, (struct engr *));
E void FDECL(rloc_engr, (struct engr *));
E void FDECL(make_grave, (int, int, const char *, BOOLEAN_P));
E void FDECL(make_signpost, (int, int, const char*, BOOLEAN_P));

/* ### exper.c ### */

E long FDECL(newuexp, (int));
E int NDECL(newpw);
E int NDECL(enmaxadjustment);
E void NDECL(updatemaxen);
E int FDECL(experience, (struct monst *, int));
E void FDECL(more_experienced, (int, int));
E long FDECL(game_score_difficulty_adjustment, (long));

E void FDECL(losexp, (const char *));
E void NDECL(newexplevel);
E void FDECL(pluslvl, (BOOLEAN_P));
E long FDECL(rndexp, (BOOLEAN_P));

/* ### explode.c ### */

E void FDECL(explode, (int, int, int, struct monst*, int, int, int, int, CHAR_P, int));
E long FDECL(scatter, (int, int, int, unsigned int, struct obj *));
E void FDECL(splatter_burning_oil, (int, int, BOOLEAN_P));
E void FDECL(explode_oil, (struct obj *, int, int));
E void FDECL(play_explosion_animation_at, (int, int, enum explosion_types));
E void NDECL(explosion_wait_until_action);
E void NDECL(explosion_wait_until_end);

/* ### extralev.c ### */

E void NDECL(makeroguerooms);
E void FDECL(corr, (int, int));
E void NDECL(makerogueghost);

/* ### files.c ### */

E char *FDECL(fname_encode, (const char *, CHAR_P, char *, char *, int));
E char *FDECL(fname_decode, (CHAR_P, char *, char *, int));
E const char *FDECL(fqname, (const char *, int, int));
E FILE *FDECL(fopen_datafile, (const char *, const char *, int));
#ifdef MFLOPPY
E void NDECL(set_lock_and_bones);
#endif
E void FDECL(set_levelfile_name, (char *, int));
E int FDECL(create_levelfile, (int, char *));
E int FDECL(open_levelfile, (int, char *));
E void FDECL(delete_levelfile, (int));
E void NDECL(clearlocks);
E int FDECL(create_bonesfile, (d_level *, char **, char *));
#ifdef MFLOPPY
E void NDECL(cancel_bonesfile);
#endif
E void FDECL(commit_bonesfile, (d_level *));
E int FDECL(open_bonesfile, (d_level *, char **));
E int FDECL(delete_bonesfile, (d_level *));
E void NDECL(compress_bonesfile);
E void FDECL(set_savefile_name, (BOOLEAN_P));
#ifdef INSURANCE
E void FDECL(save_savefile_name, (int));
#endif
#ifndef MICRO
E void NDECL(set_error_savefile);
E void NDECL(set_imported_savefile);
#endif
E int NDECL(create_savefile);
E int NDECL(open_savefile);
E int NDECL(delete_savefile);
E int NDECL(open_and_validate_saved_game);
E void NDECL(mode_message);
E void NDECL(create_gamestate_levelfile);
E int FDECL(load_saved_game, (int));
E void FDECL(nh_compress, (const char *));
E void FDECL(nh_uncompress, (const char *));
E boolean FDECL(lock_file, (const char *, int, int));
E void FDECL(unlock_file, (const char *));
#ifdef USER_SOUNDS
E boolean FDECL(can_read_file, (const char *));
#endif
E void FDECL(config_error_init, (BOOLEAN_P, const char *, BOOLEAN_P));
E void FDECL(config_erradd, (const char *));
E int NDECL(config_error_done);
E boolean FDECL(read_config_file, (const char *, int));
E void FDECL(check_recordfile, (const char *));
E void NDECL(read_wizkit);
E int FDECL(read_sym_file, (int));
E int FDECL(parse_sym_line, (char *, int));
E void FDECL(paniclog, (const char *, const char *));
E void FDECL(testinglog, (const char *, const char *, const char *));
E int FDECL(validate_prefix_locations, (char *));
#ifdef SELECTSAVED
E char *FDECL(plname_from_file, (const char *, struct save_game_stats*));
E struct save_game_data FDECL(newsavegamedata, (char*, char*, struct save_game_stats, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
#endif
E struct save_game_data *NDECL(get_saved_games);
E void FDECL(free_saved_games, (struct save_game_data *));
E boolean NDECL(check_saved_game_exists);

#ifdef SELF_RECOVER
E boolean NDECL(recover_savefile);
#endif
#ifdef SYSCF_FILE
E void NDECL(assure_syscf_file);
#endif
E int FDECL(nhclose, (int));
#ifdef HOLD_LOCKFILE_OPEN
E void NDECL(really_close);
#endif
#ifdef DEBUG
E boolean FDECL(debugcore, (const char *, BOOLEAN_P));
#endif
E boolean FDECL(read_tribute, (const char *, const char *, int,
                               char *, int, unsigned));
E boolean FDECL(Death_quote, (char *, int));
E void NDECL(list_files);
E void FDECL(reset_save_file_name, (BOOLEAN_P));
E void NDECL(reset_files);
E void NDECL(reset_gamelog);
E void FDECL(show_gamelog, (int));
E void FDECL(gamelog_add, (long, long, const char*));
E void VDECL(livelog_printf, (unsigned int, const char*, ...)) PRINTF_F(2, 3);
E void FDECL(livelog_dump_url, (unsigned int));
#ifdef WHEREIS_FILE
E void NDECL(touch_whereis);
E void NDECL(delete_whereis);
E void FDECL(signal_whereis, (int));
#endif
#ifdef EXTRAINFO_FN
E void NDECL(mk_dgl_extrainfo);
#endif
#ifdef WISH_TRACKER
E void FDECL(trackwish, (char*));
#endif


/* ### fountain.c ### */

E void FDECL(floating_above, (const char *));
E void FDECL(dogushforth, (int));
#ifdef USE_TRAMPOLI
E void FDECL(gush, (int, int, genericptr_t));
#endif
E void FDECL(dryup, (XCHAR_P, XCHAR_P, BOOLEAN_P));
E void NDECL(drinkfountain);
E void FDECL(dipfountain, (struct obj *));
E void FDECL(breaksink, (int, int));
E void NDECL(drinksink);
E const char* FDECL(fountain_type_text, (int));
E const char* FDECL(get_fountain_name, (int, int));
E void NDECL(init_fountains);

/* ### gnhmain.c ### */

#if defined(UNIX) && defined(GNH_MOBILE)
#ifdef PORT_HELP
E void NDECL(port_help);
#endif
E boolean NDECL(authorize_wizard_mode);
#endif /* UNIX && GNH_MOBILE */

/* ### gnhunix.c ### */

#if defined(UNIX) && defined(GNH_MOBILE)
E void NDECL(getlock);
E void NDECL(check_crash);
E void NDECL(thread_lock_init);
E void NDECL(thread_lock_destroy);
E void NDECL(thread_lock_lock);
E void NDECL(thread_lock_unlock);
#endif /* UNIX && GNH_MOBILE */

/* ### hack.c ### */

E boolean FDECL(is_valid_travelpt, (int,int));
E anything *FDECL(uint_to_any, (unsigned));
E anything *FDECL(long_to_any, (long));
E anything *FDECL(monst_to_any, (struct monst *));
E anything *FDECL(obj_to_any, (struct obj *));
E anything* FDECL(trap_to_any, (struct trap*));
E anything* FDECL(coord_to_any, (coord));
E anything* FDECL(region_to_any, (struct nhregion*));
E anything* FDECL(xy_to_any, (XCHAR_P, XCHAR_P));
E boolean FDECL(revive_nasty, (int, int, const char *));
E void FDECL(movobj, (struct obj *, XCHAR_P, XCHAR_P));
E boolean FDECL(may_dig, (XCHAR_P, XCHAR_P));
E boolean FDECL(may_passwall, (XCHAR_P, XCHAR_P));
E boolean FDECL(bad_rock, (struct permonst *, XCHAR_P, XCHAR_P));
E int FDECL(cant_squeeze_thru, (struct monst *));
E boolean FDECL(invocation_pos, (XCHAR_P, XCHAR_P));
E boolean FDECL(test_move, (int, int, int, int, int));
#ifdef DEBUG
E int NDECL(wiz_debug_cmd_traveldisplay);
#endif
E boolean NDECL(u_rooted);
E void NDECL(domove);
E boolean NDECL(overexertion);
E void NDECL(invocation_message);
E void NDECL(switch_terrain);
E boolean FDECL(pooleffects, (BOOLEAN_P));
E void FDECL(spoteffects, (BOOLEAN_P));
E char *FDECL(in_rooms, (XCHAR_P, XCHAR_P, int));
E boolean FDECL(in_town, (int, int));
E void FDECL(check_special_room, (BOOLEAN_P));
E int NDECL(dopickup);
E void NDECL(lookaround);
E boolean FDECL(crawl_destination, (int, int));
E int NDECL(monster_nearby);
E void FDECL(nomul, (int));
E void FDECL(unmul, (const char *));
E void FDECL(you_die, (const char*, int));
E void FDECL(kill_player, (const char*, int));
E void FDECL(losehp, (double, const char *, int));
E int NDECL(weight_cap);
E int NDECL(inv_weight);
E int NDECL(near_capacity);
E int FDECL(calc_capacity, (int));
E int NDECL(max_capacity);
E boolean FDECL(check_capacity, (const char *));
E int FDECL(inv_cnt, (BOOLEAN_P));
E int FDECL(inv_cnt_ex, (BOOLEAN_P, BOOLEAN_P));
E long FDECL(money_cnt, (struct obj *));
E struct extended_menu_info FDECL(obj_to_extended_menu_info, (struct obj*));
E struct extended_menu_info NDECL(menu_heading_info);
E struct extended_menu_info FDECL(menu_group_heading_info, (CHAR_P));
E struct extended_menu_info FDECL(menu_special_mark_info, (CHAR_P));
E const char* FDECL(get_cmap_or_cmap_variation_glyph_explanation, (int));
E void NDECL(reset_hack);

/* ### hacklib.c ### */

E boolean FDECL(digit, (CHAR_P));
E boolean FDECL(letter, (CHAR_P));
E char FDECL(highc, (CHAR_P));
E char FDECL(lowc, (CHAR_P));
E char *FDECL(lcase, (char *));
E char *FDECL(ucase, (char *));
E char *FDECL(upstart, (char *));
E char *FDECL(mungspaces, (char *));
E char *FDECL(trimspaces, (char *));
E char *FDECL(strip_newline, (char *));
E char *FDECL(stripchars, (char *, const char *, const char *));
E char *FDECL(eos, (char *));
E boolean FDECL(str_end_is, (const char *, const char *));
E char *FDECL(strkitten, (char *, CHAR_P));
E void FDECL(copynchars, (char *, const char *, int));
E char FDECL(chrcasecpy, (int, int));
E char *FDECL(strcasecpy, (char *, const char *));
E char* FDECL(strcpy_capitalized_for_title, (char*, const char*));
E char *FDECL(s_suffix, (const char *));
E char *FDECL(ing_suffix, (const char *));
E char *FDECL(xcrypt, (const char *, char *));
E boolean FDECL(onlyspace, (const char *));
E char *FDECL(tabexpand, (char *));
E char *FDECL(visctrl, (CHAR_P));
E char *FDECL(strsubst, (char *, const char *, const char *));
E int FDECL(strNsubst, (char *, const char *, const char *, int));
E const char *FDECL(ordin, (int));
E char *FDECL(sitoa, (int));
E int FDECL(sgn, (int));
E int FDECL(rounddiv, (int, int));
E int FDECL(dist2, (int, int, int, int));
E int FDECL(isqrt, (int));
E int FDECL(distmin, (int, int, int, int));
E boolean FDECL(online2, (int, int, int, int));
E boolean FDECL(pmatch, (const char *, const char *));
E boolean FDECL(pmatchi, (const char *, const char *));
E boolean FDECL(pmatchz, (const char *, const char *));
#ifndef STRNCMPI
E int FDECL(strncmpi, (const char *, const char *, size_t));
#endif
#ifndef STRSTRI
E char *FDECL(strstri, (const char *, const char *));
#endif
E boolean
FDECL(fuzzymatch, (const char *, const char *, const char *, BOOLEAN_P));
E void FDECL(init_random, (int FDECL((*fn), (int))));
E void FDECL(reseed_random, (int FDECL((*fn), (int))));
E time_t NDECL(getnow);
E int NDECL(getyear);
#if 0
E char *FDECL(yymmdd, (time_t));
#endif
E void FDECL(print_realtime, (char*, long));
E long FDECL(yyyymmdd, (time_t));
E long FDECL(hhmmss, (time_t));
E char *FDECL(yyyymmddhhmmss, (time_t));
E time_t FDECL(time_from_yyyymmddhhmmss, (char *));
E int NDECL(phase_of_the_moon);
E boolean NDECL(friday_13th);
E int NDECL(night);
E int NDECL(midnight);
E void FDECL(strbuf_init, (strbuf_t *));
E void FDECL(strbuf_append, (strbuf_t *, const char *));
E void FDECL(strbuf_reserve, (strbuf_t *, size_t));
E void FDECL(strbuf_empty, (strbuf_t *));
E void FDECL(strbuf_nl_to_crlf, (strbuf_t *));
E void NDECL(adjusted_delay_output);
E void FDECL(copyUTF8toCP437, (char*, size_t, char*, size_t));
E void FDECL(convertUTF8toCP437, (char*, size_t));
E char FDECL(unicode_to_CP437, (nhsym));
E boolean NDECL(use_utf8_encoding);
E int FDECL(converted_nh_poskey, (int*, int*, int*));
E int NDECL(tty_arrow_key_support_enabled);
E void FDECL(write_nhsym_utf8, (char**, nhsym, BOOLEAN_P));
E void FDECL(write_text2buf_utf8, (char*, size_t, const char*));
E void FDECL(write_CP437_to_buf_unicode, (char*, size_t, const char*));
E void FDECL(write_gui_debuglog, (const char*));
E int FDECL(hl_attridx_to_attrmask, (int));
E int FDECL(hl_attrmask_to_attridx, (int));
E int FDECL(hl_attridx_to_atr, (int));
E int FDECL(hl_attrmask_to_atr, (int));

/* ### invent.c ### */

E Loot *FDECL(sortloot, (struct obj **, unsigned, BOOLEAN_P,
                         boolean (*)(OBJ_P)));
E void FDECL(unsortloot, (Loot **));
E void FDECL(assigninvlet, (struct obj *));
E struct obj *FDECL(merge_choice, (struct obj *, struct obj *));
E int FDECL(merged, (struct obj **, struct obj **));
#ifdef USE_TRAMPOLI
E int FDECL(ckunpaid, (struct obj *));
#endif
E void FDECL(addinv_core1, (struct obj *));
E void FDECL(addinv_core2, (struct obj *));
E void FDECL(update_all_character_properties, (struct obj*, BOOLEAN_P));
E struct obj *FDECL(addinv, (struct obj *));
E struct obj *FDECL(hold_another_object,
                    (struct obj *, const char *, const char *, const char *));
E void FDECL(useupall, (struct obj *));
E void FDECL(useup, (struct obj *));
E void FDECL(consume_obj_charge, (struct obj *, BOOLEAN_P));
E void FDECL(freeinv_core, (struct obj *));
E void FDECL(freeinv, (struct obj *));
E void FDECL(delallobj, (int, int));
E void FDECL(delobj, (struct obj *));
E void FDECL(delobj_with_flags, (struct obj*, unsigned long));
E struct obj *FDECL(sobj_at, (int, int, int));
E struct obj* FDECL(noncursed_sobj_at, (int, int, int));
E struct obj* FDECL(any_obj_at, (int, int, int));
E struct obj* FDECL(otyp_in_objchn, (int, struct obj*));
E struct obj *FDECL(nxtobj, (struct obj *, int, BOOLEAN_P));
E struct obj *FDECL(carrying, (int));
E struct obj* NDECL(carrying_leashed_leash);
E struct obj* FDECL(carrying_fitting_unlocking_tool_for_box, (struct obj*));
E struct obj* FDECL(carrying_fitting_unlocking_tool_for_door, (struct rm*));
E boolean NDECL(have_lizard);
E struct obj *NDECL(u_have_novel);
E struct obj *FDECL(o_on, (unsigned int, struct obj *));
E struct obj* FDECL(o_on_memory, (unsigned int, struct obj*));
E struct obj* FDECL(m_on_memory, (unsigned int, struct obj*));
E void FDECL(reduce_item_cooldown, (struct obj*));
E boolean FDECL(obj_here, (struct obj *, int, int));
E boolean NDECL(wearing_armor);
E boolean FDECL(is_worn, (struct obj *));
E boolean FDECL(is_worn_correctly, (struct obj*));
E struct obj *FDECL(g_at, (int, int));
E boolean FDECL(splittable, (struct obj *));
E struct obj *FDECL(getobj, (const char *, const char *, int, const char*));
E struct obj* FDECL(getobj_ex, (const char*, const char*, int, const char*, boolean (*)(struct obj*)));
void FDECL(construct_getobj_letters, (const char*, const char*, boolean(*)(struct obj*), char*, char*, char*, size_t, size_t, size_t, int*, xchar*, char**, xchar*, boolean*, boolean*, boolean*, boolean*, struct obj*));

E boolean FDECL(acceptable_getobj_obj, (struct obj*, const char*, const char*));
E int FDECL(ggetobj, (const char *, int (*)(OBJ_P), int,
                      BOOLEAN_P, unsigned *, int));
E int FDECL(askchain, (struct obj **, const char *, int, int (*)(OBJ_P),
                       int (*)(OBJ_P), int, const char *));
E void FDECL(fully_identify_obj, (struct obj *));
E int FDECL(identify, (struct obj *));
E int FDECL(count_unidentified, (struct obj *, boolean(*)(OBJ_P), BOOLEAN_P));
E int FDECL(identify_pack, (int, BOOLEAN_P));
E void NDECL(learn_unseen_invent);
E void FDECL(prinv, (const char *, struct obj *, long));
E void FDECL(prinv_ex, (const char*, struct obj*, long, int, int, int, int, BOOLEAN_P, BOOLEAN_P));
E char *FDECL(xprname,
              (struct obj *, const char *, CHAR_P, BOOLEAN_P, long, long));
E int NDECL(ddoinv);
E int NDECL(doseeworn);
E int NDECL(dolastpickeditem);
E int FDECL(display_item_command_menu_by_invlet, (CHAR_P, long));
E int FDECL(display_item_command_menu, (struct obj*, long));
E char FDECL(display_inventory, (const char *, BOOLEAN_P, int));
E char FDECL(display_inventory_with_header, (const char*, BOOLEAN_P, long*, int, BOOLEAN_P));
E int FDECL(display_binventory, (int, int, BOOLEAN_P));
E struct obj *FDECL(display_cinventory, (struct obj *));
E struct obj *FDECL(display_minventory, (struct monst *, int, char *));
E int NDECL(dotypeinv);
E const char *FDECL(dfeature_at, (int, int));
E const char* FDECL(adjusted_dfeature_at, (int, int));
int FDECL(here_window_display_style, (const char*, struct engr*, struct obj*));
E const char* FDECL(get_floor_doodad_explanation_at, (int, int));
E int FDECL(look_here, (int, BOOLEAN_P, BOOLEAN_P));
E int NDECL(dolook);
E boolean FDECL(will_feel_cockatrice, (struct obj *, BOOLEAN_P));
E void FDECL(feel_cockatrice, (struct obj *, BOOLEAN_P));
E void FDECL(stackobj, (struct obj *));
E boolean FDECL(mergable, (struct obj *, struct obj *));
E int NDECL(doprgold);
E int NDECL(doprwep);
E int NDECL(doprarm);
E int NDECL(doprring);
E int NDECL(dopramulet);
E int NDECL(doprtool);
E int NDECL(doprinuse);
E void FDECL(useupf, (struct obj *, long));
E void FDECL(useupf_with_flags, (struct obj*, long, unsigned long));
E char *FDECL(let_to_name, (CHAR_P, UCHAR_P, BOOLEAN_P));
E const char* FDECL(get_class_name, (CHAR_P));
E void NDECL(free_invbuf);
E void NDECL(reassign);
E int NDECL(doorganize);
E void NDECL(free_pickinv_cache);
E int FDECL(count_unpaid, (struct obj *, boolean(*)(OBJ_P), BOOLEAN_P));
E int FDECL(count_objects, (struct obj*, boolean(*)(OBJ_P), BOOLEAN_P));
E int FDECL(count_buc, (struct obj *, int, boolean (*)(OBJ_P), BOOLEAN_P));
E void FDECL(tally_BUCX, (struct obj *, BOOLEAN_P,
                          int *, int *, int *, int *, int *, int *));
E long FDECL(count_contents, (struct obj *, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(carry_obj_effects, (struct obj *));
E const char *FDECL(currency, (long));
E void FDECL(silly_thing, (const char *, struct obj *));
E void NDECL(print_things_here_to_window);
E void NDECL(update_here_window);
E void NDECL(reset_inventory);
E struct obj* FDECL(contains_otyp, (struct obj*, int));

/* ### ioctl.c ### */

#if (defined(UNIX) || defined(__BEOS__))
E void NDECL(getwindowsz);
E void NDECL(getioctls);
E void NDECL(setioctls);
#ifdef SUSPEND
E int NDECL(dosuspend);
#endif /* SUSPEND */
#endif /* UNIX || __BEOS__ */


/* ### light.c ### */

E void FDECL(new_light_source, (XCHAR_P, XCHAR_P, int, int, ANY_P *, int));
E void FDECL(del_light_source, (int, ANY_P *));
E void FDECL(do_light_sources, (char **));
E struct monst* FDECL(find_mid_ew, (unsigned));
E struct monst *FDECL(find_mid, (unsigned, unsigned));
E void FDECL(save_light_sources, (int, int, int));
E void NDECL(reset_light_sources);
E void FDECL(restore_light_sources, (int));
E void FDECL(light_stats, (const char *, char *, long *, size_t *));
E void FDECL(relink_light_sources, (BOOLEAN_P));
E void NDECL(light_sources_sanity_check);
E void FDECL(obj_move_light_source, (struct obj *, struct obj *));
E boolean NDECL(any_light_source);
E void FDECL(snuff_light_source, (int, int));
E boolean FDECL(obj_sheds_light, (struct obj *));
E boolean FDECL(obj_is_burning, (struct obj *));
E void FDECL(obj_split_light_source, (struct obj *, struct obj *));
E void FDECL(obj_merge_light_sources, (struct obj *, struct obj *));
E void FDECL(obj_adjust_light_radius, (struct obj *, int));
E int FDECL(candle_light_range, (struct obj *));
E int FDECL(artifact_light_range, (struct obj*));
E int FDECL(current_arti_light_radius, (struct obj *));
E const char *FDECL(arti_light_description, (struct obj *));
E int NDECL(wiz_light_sources);
E int FDECL(obj_light_radius, (struct obj*));
E long FDECL(obj_light_maximum_burn_time, (struct obj*));
E long FDECL(obj_light_burn_time_left, (struct obj*));

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(forcelock);
E int NDECL(picklock);
#endif
E boolean FDECL(picking_lock, (int *, int *));
E boolean FDECL(picking_at, (int, int));
E void FDECL(breakchestlock, (struct obj *, BOOLEAN_P));
E void NDECL(reset_pick);
E void FDECL(maybe_reset_pick, (struct obj *));
E int FDECL(pick_lock, (struct obj *));
E int FDECL(pick_lock_core, (struct obj*, int, int, BOOLEAN_P));
#ifdef ANDROID
E boolean NDECL(can_try_force);
#endif
E int NDECL(doforce);
E boolean FDECL(boxlock, (struct obj *, struct obj *));
E boolean FDECL(doorlock, (struct obj *, int, int));
E int NDECL(doopen);
E boolean FDECL(stumble_on_door_mimic, (int, int));
E int FDECL(doopen_indir, (int, int));
E int NDECL(doclose);
#ifdef ANDROID
E void FDECL(lock_mouse_cursor, (BOOLEAN_P));
#endif
E boolean FDECL(key_fits_the_box_lock, (struct obj*, struct obj*));
E boolean FDECL(key_fits_the_door_lock, (struct obj*, struct rm*));

#ifdef MAC
/* These declarations are here because the main code calls them. */

/* ### macfile.c ### */

E int FDECL(maccreat, (const char *, long));
E int FDECL(macopen, (const char *, int, long));
E int FDECL(macclose, (int));
E int FDECL(macread, (int, void *, unsigned));
E int FDECL(macwrite, (int, void *, unsigned));
E long FDECL(macseek, (int, long, SHORT_P));
E int FDECL(macunlink, (const char *));

/* ### macmain.c ### */

E boolean NDECL(authorize_wizard_mode);

/* ### macsnd.c ### */

E void FDECL(mac_speaker, (struct obj *, char *));

/* ### macunix.c ### */

E void FDECL(regularize, (char *));
E void NDECL(getlock);

/* ### macwin.c ### */

E void FDECL(lock_mouse_cursor, (Boolean));
E int NDECL(SanePositions);

/* ### mttymain.c ### */

E void FDECL(getreturn, (char *));
E void VDECL(msmsg, (const char *, ...));
E void NDECL(gettty);
E void NDECL(setftty);
E void FDECL(settty, (const char *));
E int NDECL(tgetch);
E void FDECL(cmov, (int x, int y));
E void FDECL(nocmov, (int x, int y));

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
#ifdef UNIX
E void NDECL(free_maildata);
E void NDECL(getmailstatus);
E void NDECL(ck_server_admin_msg);
#endif
E void NDECL(ckmailstatus);
E void FDECL(readmail, (struct obj *));
#endif /* MAIL */

/* ### makemon.c ### */

E void FDECL(dealloc_monst, (struct monst *));
E boolean FDECL(is_home_elemental, (struct permonst *));
E struct monst* FDECL(clone_mon, (struct monst*, XCHAR_P, XCHAR_P, BOOLEAN_P));
E double FDECL(monhp_per_lvl, (struct monst *));
E int FDECL(monbasehp_per_lvl, (struct monst*));
E double FDECL(monhpadj_per_lvl, (struct monst*));
E void FDECL(newmonhp, (struct monst *, int, int, unsigned long));
E int FDECL(monhp_difficulty_adjustment, (int, int));
E struct mextra *NDECL(newmextra);
E void FDECL(copy_mextra, (struct monst *, struct monst *));
E void FDECL(dealloc_mextra, (struct monst *));
E struct monst *FDECL(makemon, (struct permonst *, int, int, unsigned long));
E struct monst* FDECL(makemon2, (struct permonst*, int, int, unsigned long, unsigned long));
E struct monst* FDECL(makemon_ex, (struct permonst*, int, int, unsigned long, unsigned long, UNSIGNED_SHORT_P, int, int));
E struct monst* FDECL(makemon_limited, (struct permonst*, int, int, unsigned long, unsigned long, UNSIGNED_SHORT_P, int, int, int, int, ALIGNTYP_P));
E boolean FDECL(create_critters, (int, struct permonst *, BOOLEAN_P));
E struct permonst *NDECL(rndmonst);
E struct permonst* FDECL(rndmonst_limited, (int));
E void FDECL(reset_rndmonst, (int));
E void FDECL(get_generated_monster_minmax_levels, (int, int*, int*, int));
E struct permonst *FDECL(mkclass, (CHAR_P, int));
E struct permonst *FDECL(mkclass_aligned, (CHAR_P, int, ALIGNTYP_P));
E struct permonst* FDECL(mkclass_core, (CHAR_P, int, ALIGNTYP_P, int));
E int FDECL(mkclass_poly, (int));
E int FDECL(adj_lev, (struct permonst *, int));
E struct permonst *FDECL(grow_up, (struct monst *, struct monst *));
E void FDECL(change_mon_ability_scores, (struct monst*, int, int));
E struct obj* FDECL(mongets, (struct monst *, int));
E struct obj* FDECL(mongets_with_material, (struct monst*, int, UCHAR_P));
E struct obj* FDECL(mongets_spellbook, (struct monst*));
E int FDECL(mongets_return_enchantment, (struct monst*, int));
E struct obj* FDECL(mongets_noinit_item, (struct monst*, int,  int));
E long FDECL(mongetsgold, (struct monst*, long));
E int FDECL(golemhp, (int));
E boolean FDECL(peace_minded, (struct permonst *));
E void FDECL(set_mhostility, (struct monst *));
E void FDECL(newmcorpsenm, (struct monst *));
E void FDECL(freemcorpsenm, (struct monst *));
E void FDECL(set_mimic_sym, (struct monst *));
E int FDECL(mbirth_limit, (int));
E void FDECL(mimic_hit_msg, (struct monst *, SHORT_P));
E void FDECL(mkmonmoney, (struct monst *, long));
E int FDECL(bagotricks, (struct obj *, BOOLEAN_P, int *));
E boolean FDECL(propagate, (int, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(usmellmon, (struct permonst *));
E boolean FDECL(wrong_elem_type, (struct permonst*));
E void FDECL(m_initthrow, (struct monst*, int, int, int, BOOLEAN_P, int, int, int));
E boolean FDECL(randomize_monster_gender, (struct permonst*));
E void NDECL(makemon_animation_wait_until_end);
E struct monst* FDECL(make_level_monster, (int, int, unsigned long, unsigned long));
E struct monst* NDECL(make_level_monster_anywhere);
E void NDECL(reset_makemon);
E void FDECL(newmmonst, (struct monst*));
E void FDECL(free_mmonst, (struct monst*));
E void FDECL(newmobj, (struct monst*));
E void FDECL(free_mobj, (struct monst*));
E struct monst* FDECL(get_mmonst, (struct monst*, BOOLEAN_P));
E void FDECL(save_mmonst, (struct monst*, struct monst*));
E struct obj* FDECL(get_mobj, (struct monst*, BOOLEAN_P));
E void FDECL(save_mobj, (struct monst*, struct obj*));

/* ### mapglyph.c ### */

E int FDECL(mapglyph, (struct layer_info, nhsym *, int *, unsigned long*, int, int));
E char *FDECL(encglyph, (int));
E char *FDECL(decode_mixed, (char *, const char *));
E void FDECL(genl_putmixed_ex, (winid, const char *, int, int, int));
E int FDECL(zap_glyph_to_cmap, (int));

/* ### mcastu.c ### */

E int FDECL(castmu, (struct monst *, struct attack *, BOOLEAN_P, BOOLEAN_P));
E int FDECL(buzzmu, (struct monst *, struct attack *));

/* ### mhitm.c ### */

E int FDECL(fightm, (struct monst *, BOOLEAN_P));
E int FDECL(mattackm, (struct monst *, struct monst *));
E boolean FDECL(engulf_target, (struct monst *, struct monst *));
E int FDECL(mdisplacem, (struct monst *, struct monst *, BOOLEAN_P));
E void FDECL(paralyze_monst, (struct monst *, int, BOOLEAN_P));
E int FDECL(sleep_monst, (struct monst *, struct obj *, struct monst*, int, int, int));
E void FDECL(slept_monst, (struct monst *));
E void FDECL(xdrainenergym, (struct monst *, BOOLEAN_P));
E long FDECL(attk_protection, (int));
E void FDECL(rustm, (struct monst *, struct obj *));
E void NDECL(reset_mhitm);

/* ### mhitu.c ### */

E const char *FDECL(mpoisons_subj, (struct monst *, struct attack *));
E void NDECL(u_slow_down);
E struct monst *NDECL(cloneu);
E void FDECL(expels, (struct monst *, struct permonst *, BOOLEAN_P));
E struct attack *FDECL(getmattk, (struct monst *, struct monst *,
                                  int, int *, struct attack *));
E int FDECL(mattacku, (struct monst *));
E boolean FDECL(check_magic_cancellation_success, (struct monst*, int));
E boolean FDECL(check_ability_resistance_success, (struct monst*, int, int));
E int FDECL(magic_negation, (struct monst *));
E int FDECL(magic_negation_percentage, (int));
E boolean NDECL(gulp_blnd_check);
E int FDECL(gazemu, (struct monst *, struct attack *));
E void FDECL(mdamageu, (struct monst *, double, BOOLEAN_P));
E void FDECL(mdamageu_with_hit_tile, (struct monst*, double, BOOLEAN_P, enum hit_tile_types));
E int FDECL(could_seduce, (struct monst *, struct monst *, struct attack *));
E int FDECL(doseduce, (struct monst *));
E void FDECL(update_m_facing, (struct monst*, int, BOOLEAN_P));
E int FDECL(get_pm_attack_index, (struct permonst*, struct attack*));
E enum hit_tile_types FDECL(get_hit_tile_by_adtyp, (int));
boolean FDECL(should_display_m_action_tile, (struct monst*, enum action_tile_types action));
E void NDECL(reset_mhitu);

/* ### minion.c ### */

E void FDECL(newemin, (struct monst *));
E void FDECL(free_emin, (struct monst *));
E int FDECL(monster_census, (BOOLEAN_P));
E int FDECL(msummon, (struct monst *));
E int FDECL(yeenaghu_gnoll_summon, (struct monst*));
E int FDECL(yeenaghu_ghoul_summon, (struct monst*));
E int NDECL(yacc_bison_summon);
E int NDECL(orcus_undead_summon);
E void FDECL(summon_minion, (ALIGNTYP_P, BOOLEAN_P));
E int FDECL(demon_talk, (struct monst *, boolean*));
E long FDECL(bribe, (struct monst *));
E int FDECL(dprince, (ALIGNTYP_P));
E int FDECL(dlord, (ALIGNTYP_P));
E int NDECL(llord);
E int FDECL(ndemon, (ALIGNTYP_P));
E int NDECL(lminion);
E void FDECL(lose_guardian_angel, (struct monst *));
E void FDECL(gain_guardian_angel, (BOOLEAN_P));

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(do_comp, (genericptr_t, genericptr_t));
#endif
E void NDECL(sort_rooms);
E void FDECL(add_room, (int, int, int, int, BOOLEAN_P, SCHAR_P, BOOLEAN_P, int, int, int, int, SHORT_P));
E void FDECL(add_subroom, (struct mkroom *, int, int, int, int, BOOLEAN_P,
                           SCHAR_P, BOOLEAN_P, int, int, int, int, SHORT_P));
E void NDECL(makecorridors);
E void FDECL(add_door, (int, int, struct mkroom *));
E void NDECL(mklev);
#ifdef SPECIALIZATION
E void FDECL(topologize, (struct mkroom *, BOOLEAN_P));
#else
E void FDECL(topologize, (struct mkroom *));
#endif
E void FDECL(place_branch, (branch *, XCHAR_P, XCHAR_P));
E boolean FDECL(occupied, (XCHAR_P, XCHAR_P));
E int FDECL(okdoor, (XCHAR_P, XCHAR_P));
E void FDECL(dodoor, (int, int, struct mkroom *));
E struct trap* FDECL(mktrap, (int, int, struct mkroom *, coord *));
E void FDECL(mkmodronportal, (int, coord*, coord*, unsigned long));
E void FDECL(mkstairs, (XCHAR_P, XCHAR_P, CHAR_P, struct mkroom *, int));
E void NDECL(mkinvokearea);
E void FDECL(mineralize, (int, int, int, int, BOOLEAN_P));
E void FDECL(maybe_create_location_light_source, (XCHAR_P, XCHAR_P));
E void FDECL(maybe_create_location_sound_source, (XCHAR_P, XCHAR_P));
E void FDECL(maybe_create_location_light_and_sound_sources, (XCHAR_P, XCHAR_P));
E void NDECL(create_level_light_sources);
E void NDECL(create_level_sound_sources);
E void FDECL(define_level_location_floor_types, (BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));

/* ### mkmap.c ### */

E void FDECL(flood_fill_rm, (int, int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(remove_rooms, (int, int, int, int));
E void NDECL(wallify_entire_map);
/* E void FDECL(mkmap, (lev_init *)); -- need sp_lev.h for lev_init */

/* ### mkmaze.c ### */

E void NDECL(maze_init);
E void FDECL(wallification, (int, int, int, int));
E void FDECL(fix_wall_spines, (int, int, int, int));
E void FDECL(walkfrom, (int, int, SCHAR_P));
E void FDECL(makemaz, (const char *));
E void FDECL(mazexy, (coord *));
E void NDECL(bound_digging);
E void FDECL(mkportal, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E boolean FDECL(bad_location, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                               XCHAR_P, XCHAR_P));
E void FDECL(place_lregion, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                             XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, d_level *));
E void NDECL(fixup_special);
E void NDECL(fumaroles);
E void NDECL(movebubbles);
E void NDECL(water_friction);
E void FDECL(save_waterlevel, (int, int));
E void NDECL(reset_waterlevel);
E void FDECL(restore_waterlevel, (int));
E const char *FDECL(waterbody_name, (XCHAR_P, XCHAR_P));

/* ### mkobj.c ### */

E struct oextra *NDECL(newoextra);
E void FDECL(copy_oextra, (struct obj *, struct obj *));
E void FDECL(dealloc_oextra, (struct obj *));
E void FDECL(newomonst, (struct obj *));
E void FDECL(free_omonst, (struct obj *));
E void FDECL(newomid, (struct obj *));
E void FDECL(free_omid, (struct obj *));
E void FDECL(newolong, (struct obj *));
E void FDECL(free_olong, (struct obj *));
E void FDECL(new_omailcmd, (struct obj *, const char *));
E void FDECL(free_omailcmd, (struct obj *));
E struct obj *FDECL(mkobj_at, (CHAR_P, int, int, BOOLEAN_P));
E struct obj* FDECL(mkobj_at_with_flags, (CHAR_P, int, int, BOOLEAN_P, UCHAR_P, long, long, unsigned long));
E struct obj *FDECL(mksobj_at, (int, int, int, BOOLEAN_P, BOOLEAN_P));
E struct obj* FDECL(mksobj_at_with_flags, (int, int, int, BOOLEAN_P, BOOLEAN_P, int, struct monst*, UCHAR_P, long, long, unsigned long));
E struct obj *FDECL(mksobj_migr_to_species, (int, unsigned, BOOLEAN_P, BOOLEAN_P));
E struct obj *FDECL(mkobj, (CHAR_P, BOOLEAN_P, int));
E struct obj* FDECL(mkobj_with_flags, (CHAR_P, BOOLEAN_P, int, struct monst*, UCHAR_P, long, long, unsigned long));
E int FDECL(random_objectid_from_class, (CHAR_P, struct monst*, unsigned long));
E int FDECL(random_spellbook_objectid, (struct monst*, unsigned long));
E int NDECL(rndmonnum);
E boolean FDECL(bogon_is_pname, (CHAR_P));
E struct obj *FDECL(splitobj, (struct obj *, long));
E struct obj *FDECL(unsplitobj, (struct obj *));
E void NDECL(clear_splitobjs);
E void FDECL(replace_object, (struct obj *, struct obj *));
E struct obj *FDECL(unknwn_contnr_contents, (struct obj *));
E void FDECL(bill_dummy_object, (struct obj *));
E struct obj* FDECL(memory_dummy_object, (struct obj*));
E void FDECL(add_memory_object_contents, (struct obj*, struct obj*));
E void FDECL(add_to_memoryobjs, (struct obj*));
E void NDECL(clear_memoryobjs);
E void FDECL(clear_hero_object_memory_at, (int, int));
E void FDECL(costly_alteration, (struct obj *, int));
E struct obj *FDECL(mksobj, (int, BOOLEAN_P, BOOLEAN_P, int));
E struct obj* FDECL(mksobj_ex, (int, BOOLEAN_P, BOOLEAN_P, int, struct monst*, UCHAR_P));
E struct obj* FDECL(mksobj_with_flags, (int, BOOLEAN_P, BOOLEAN_P, int, struct monst*, UCHAR_P, long, long, unsigned long));
E int FDECL(bcsign, (struct obj *));
E int FDECL(weight, (struct obj *));
E struct obj *FDECL(mkgold, (long, int, int));
E struct obj *FDECL(mkcorpstat, (int, struct monst *, struct permonst *, int,
                                 int, unsigned));
E int FDECL(corpse_revive_type, (struct obj *));
E struct obj *FDECL(obj_attach_mid, (struct obj *, unsigned));
E struct monst *FDECL(get_mtraits, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(mk_tt_object, (int, int, int));
E struct obj *FDECL(mk_named_object,
                    (int, struct permonst *, int, int, const char *));
E struct obj *FDECL(rnd_treefruit_at, (int, int));
E long FDECL(get_multigen_quan, (UCHAR_P));
E short FDECL(get_init_charge, (UCHAR_P));
E short FDECL(get_obj_init_charge, (struct obj*));
E short FDECL(get_max_charge, (UCHAR_P));
E const char* FDECL(get_recharge_text, (enum recharging_types));
E short FDECL(get_obj_max_charge, (struct obj*));
E int FDECL(get_init_enchantment, (int));
E int FDECL(get_obj_init_enchantment, (struct obj*));
E int FDECL(get_max_enchantment, (int));
E int FDECL(get_obj_max_enchantment, (struct obj*));
E void FDECL(set_corpsenm, (struct obj *, int));
E void FDECL(start_corpse_timeout, (struct obj *));
E void FDECL(bless, (struct obj *));
E void FDECL(unbless, (struct obj *));
E void FDECL(curse, (struct obj *));
E void FDECL(uncurse, (struct obj *));
E void FDECL(blessorcurse, (struct obj *, int));
E boolean FDECL(is_flammable, (struct obj *));
E boolean FDECL(is_rottable, (struct obj *));
E void FDECL(place_object, (struct obj *, int, int));
E void FDECL(remove_object, (struct obj *));
E void FDECL(place_memory_object, (struct obj*, int, int));
E void FDECL(remove_memory_object, (struct obj*));
E void FDECL(discard_minvent, (struct monst *));
E void FDECL(obj_extract_self, (struct obj *));
E void FDECL(extract_nobj, (struct obj *, struct obj **));
E void FDECL(extract_nexthere, (struct obj *, struct obj **));
E int FDECL(add_to_minv, (struct monst *, struct obj *));
E struct obj *FDECL(add_to_container, (struct obj *, struct obj *));
E void FDECL(add_to_migration, (struct obj *));
E void FDECL(add_to_buried, (struct obj *));
E void FDECL(dealloc_obj, (struct obj *));
E void FDECL(obj_ice_effects, (int, int, BOOLEAN_P));
E long FDECL(peek_at_iced_corpse_age, (struct obj *));
E int FDECL(hornoplenty, (struct obj *, BOOLEAN_P));
E void NDECL(obj_sanity_check);
E struct obj *FDECL(obj_nexto, (struct obj *));
E struct obj *FDECL(obj_nexto_xy, (struct obj *, int, int, BOOLEAN_P));
E struct obj *FDECL(obj_absorb, (struct obj **, struct obj **));
E struct obj *FDECL(obj_meld, (struct obj **, struct obj **));
E void FDECL(pudding_merge_message, (struct obj *, struct obj *));
E unsigned int FDECL(get_item_base_weight, (struct obj*));

/* ### mkroom.c ### */

E int FDECL(make_room, (int));
E void FDECL(fill_zoo, (struct mkroom *));
E struct permonst *NDECL(antholemon);
E boolean FDECL(nexttodoor, (int, int));
E boolean FDECL(nexttotree, (int, int));
E boolean FDECL(has_dnstairs, (struct mkroom *));
E boolean FDECL(has_upstairs, (struct mkroom *));
E int FDECL(somex, (struct mkroom *));
E int FDECL(somey, (struct mkroom *));
E boolean FDECL(inside_room, (struct mkroom *, XCHAR_P, XCHAR_P));
E struct mkroom* FDECL(which_room, (XCHAR_P, XCHAR_P));
E struct mkroom* FDECL(which_room_by_list, (struct mkroom**, int, XCHAR_P, XCHAR_P));
E boolean FDECL(somexy, (struct mkroom *, coord *));
E boolean FDECL(somexy_within_distance, (struct mkroom*, coord, int, coord*));
E void FDECL(mkundead, (coord *, BOOLEAN_P, int));
E struct permonst *NDECL(courtmon);
E void FDECL(save_rooms, (int));
E void FDECL(rest_rooms, (int));
E void NDECL(reset_rooms);
E struct mkroom *FDECL(search_special, (SCHAR_P));
E int FDECL(cmap_to_type, (int));
E int FDECL(mknpcroom, (int));
E void FDECL(set_room_tileset, (struct mkroom*));

/* ### mon.c ### */

E void NDECL(mon_sanity_check);
E int FDECL(undead_to_corpse, (int));
E int FDECL(genus, (int, int));
E int FDECL(pm_to_cham, (int));
E int FDECL(minliquid, (struct monst *));
E int NDECL(movemon);
E int FDECL(meatmetal, (struct monst *));
E int FDECL(meatrock, (struct monst*));
E int FDECL(meatobj, (struct monst *));
E void FDECL(mpickgold, (struct monst *));
E boolean FDECL(mpickstuff, (struct monst *, const char *));
E int FDECL(curr_mon_load, (struct monst *));
E int FDECL(max_mon_load, (struct monst *));
E int FDECL(can_carry, (struct monst *, struct obj *));
E int FDECL(mfndpos, (struct monst *, coord *, long *, long));
E int FDECL(mfndpos_xy, (struct monst*, XCHAR_P, XCHAR_P, coord*, long*, long));
E boolean FDECL(monnear, (struct monst *, int, int));
E boolean FDECL(mon_somewhat_near, (struct monst*, int, int));
E void NDECL(dmonsfree);
E int FDECL(mcalcmove, (struct monst *));
E void NDECL(update_monster_timeouts);
E void FDECL(replmon, (struct monst *, struct monst *));
E void FDECL(relmon, (struct monst *, struct monst **));
E struct obj *FDECL(mlifesaver, (struct monst *));
E boolean FDECL(corpse_chance, (struct monst *, struct monst *, BOOLEAN_P));
E void FDECL(mondead, (struct monst *));
E void FDECL(mondead_with_flags, (struct monst*, unsigned long));
E void FDECL(mondied, (struct monst *));
E void FDECL(mongone, (struct monst *));
E void FDECL(monstone, (struct monst *));
E void FDECL(monkilled, (struct monst *, const char *, int));
E void FDECL(unstuck, (struct monst *));
E void FDECL(killed, (struct monst *));
E void FDECL(xkilled, (struct monst *, int));
E void FDECL(mon_to_stone, (struct monst *));
E void FDECL(m_into_limbo, (struct monst *, BOOLEAN_P));
E void FDECL(mnexto, (struct monst *));
E void FDECL(mnexto2, (struct monst*, BOOLEAN_P));
E void FDECL(maybe_mnexto, (struct monst *));
E int FDECL(mnearto, (struct monst *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(m_respond, (struct monst *));
E void FDECL(setmangry, (struct monst *, BOOLEAN_P));
E void FDECL(wakeup, (struct monst *, BOOLEAN_P));
E void NDECL(wake_nearby);
E void FDECL(wake_nearto, (int, int, int));
E void FDECL(seemimic, (struct monst *));
E void NDECL(rescham);
E void NDECL(restartcham);
E void FDECL(restore_cham, (struct monst *));
E boolean FDECL(hideunder, (struct monst *));
E void FDECL(hide_monst, (struct monst *));
E void FDECL(mon_animal_list, (BOOLEAN_P));
E boolean FDECL(validvamp, (struct monst *, int *, int));
E int FDECL(select_newcham_form, (struct monst *));
E void FDECL(mgender_from_permonst, (struct monst *, struct permonst *));
E int FDECL(newcham,
            (struct monst *, struct permonst *, UNSIGNED_SHORT_P, BOOLEAN_P, BOOLEAN_P));
E int FDECL(can_be_hatched, (int));
E int FDECL(egg_type_from_parent, (int, BOOLEAN_P));
E boolean FDECL(dead_species, (int, BOOLEAN_P));
E void NDECL(kill_genocided_monsters);
E void FDECL(golemeffects, (struct monst *, int, double));
E boolean FDECL(angry_guards, (BOOLEAN_P));
E void NDECL(pacify_guards);
E void FDECL(decide_to_shapeshift, (struct monst *, int));
E boolean FDECL(vamp_stone, (struct monst *));
E void FDECL(m_detach, (struct monst*, struct permonst*, BOOLEAN_P));
E int NDECL(randomtruegem);
E int FDECL(random_reagent_otyp, (BOOLEAN_P, BOOLEAN_P, int));
E int FDECL(randomore, (BOOLEAN_P));
E const char* FDECL(pm_monster_name, (struct permonst*, BOOLEAN_P));
E const char* FDECL(mon_monster_name, (struct monst*));
E const char* FDECL(corpse_monster_name, (struct obj*));
E const char* FDECL(pm_female_name, (struct permonst*));
E const char* FDECL(pm_common_name, (struct permonst*));
E const char* FDECL(mon_common_name, (struct monst*));
E const char* FDECL(corpse_common_name, (struct obj*));
E const char* FDECL(pm_plural_name, (struct permonst*, UCHAR_P));
E const char* FDECL(pm_general_name, (struct permonst*, UCHAR_P));
E boolean FDECL(is_female_corpse_or_statue, (struct obj*));
E boolean FDECL(is_corpse_or_statue_facing_right, (struct obj*));
E int FDECL(count_mon_runeswords, (struct monst*));
E int FDECL(revert_mon_polymorph, (struct monst*, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void NDECL(reset_mon);

/* ### mondata.c ### */

E void FDECL(set_mon_data, (struct monst *, struct permonst *, UNSIGNED_SHORT_P));
E struct attack *FDECL(attacktype_fordmg, (struct permonst *, int, int));
E boolean FDECL(attacktype, (struct permonst *, int));
E boolean FDECL(noattacks, (struct permonst *));
E boolean FDECL(poly_when_stoned, (struct permonst *));
E boolean FDECL(can_blnd, (struct monst *, struct monst *, UCHAR_P, struct obj *));
E boolean FDECL(ranged_attk, (struct permonst *));
E boolean FDECL(hates_silver, (struct permonst *));
E boolean FDECL(mon_hates_silver, (struct monst *));
E boolean FDECL(hates_blessed, (struct permonst*));
E boolean FDECL(mon_hates_blessed, (struct monst*));
E boolean FDECL(hates_cursed, (struct permonst*));
E boolean FDECL(mon_hates_cursed, (struct monst*));
E boolean FDECL(eschews_cursed, (struct permonst*));
E boolean FDECL(mon_eschews_cursed, (struct monst*));
E boolean FDECL(eschews_blessed, (struct permonst*));
E boolean FDECL(mon_eschews_blessed, (struct monst*));
E boolean FDECL(mon_hates_light, (struct monst*));
E boolean FDECL(mon_eschews_light, (struct monst*));
E boolean FDECL(eschews_silver, (struct permonst*));
E boolean FDECL(mon_eschews_silver, (struct monst*));
E boolean FDECL(passes_bars, (struct permonst *));
E boolean FDECL(can_blow, (struct monst *));
E boolean FDECL(can_chant, (struct monst *));
E boolean FDECL(can_be_strangled, (struct monst *));
E boolean FDECL(can_track, (struct permonst *));
E boolean FDECL(breakarm, (struct permonst *));
E boolean FDECL(sliparm, (struct permonst *));
E boolean FDECL(sticks, (struct permonst *));
E boolean FDECL(cantvomit, (struct permonst *));
E int FDECL(num_horns, (struct permonst *));
/* E boolean FDECL(canseemon, (struct monst *)); */
E struct attack *FDECL(dmgtype_fromattack, (struct permonst *, int, int));
E boolean FDECL(dmgtype, (struct permonst *, int));
E int FDECL(max_passive_dmg, (struct monst *, struct monst *));
E boolean FDECL(same_race, (struct permonst *, struct permonst *));
E int FDECL(monsndx, (struct permonst *));
E int FDECL(name_to_mon, (const char *));
E int FDECL(name_to_mon_ex, (const char*, int*));
E int FDECL(name_to_monclass, (const char *, int *));
E int FDECL(gender, (struct monst *));
E int FDECL(pronoun_gender, (struct monst *, BOOLEAN_P));
E boolean FDECL(levl_follower, (struct monst *));
E int FDECL(little_to_big, (int));
E int FDECL(big_to_little, (int));
E boolean FDECL(big_little_match, (int, int));
E const char *FDECL(locomotion, (const struct permonst *, const char *));
E const char *FDECL(stagger, (const struct permonst *, const char *));
E const char *FDECL(on_fire, (struct permonst *, struct attack *));
E const struct permonst *FDECL(raceptr, (struct monst *));
E boolean FDECL(olfaction, (struct permonst *));
E unsigned long FDECL(prop_to_innate, (int));
E unsigned long FDECL(prop_to_innate2, (int));
E unsigned long FDECL(prop_to_conveyed, (int));
E int FDECL(innate_to_prop, (unsigned long));
E int FDECL(innate2_to_prop, (unsigned long));
E int FDECL(conveyed_to_prop, (unsigned long));
E const char* FDECL(get_mflag_description, (unsigned long, BOOLEAN_P, UCHAR_P));
E int FDECL(get_worm_tail_mnum, (struct permonst*));
boolean FDECL(befriend_with_obj, (struct permonst*, struct obj*));


/* ### monmove.c ### */

E boolean FDECL(itsstuck, (struct monst *));
E boolean FDECL(mb_trapped, (struct monst *));
E boolean FDECL(monhaskey, (struct monst *, BOOLEAN_P));
E void FDECL(monster_regeneration_and_timeout, (struct monst *, BOOLEAN_P));
E int FDECL(dochugw, (struct monst *));
E boolean FDECL(onscary, (int, int, struct monst *));
E boolean FDECL(onnopickup, (int, int, struct monst*));
E void FDECL(monflee, (struct monst *, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(make_mon_fearful, (struct monst*, int));
E void FDECL(mon_yells, (struct monst *, const char *, const char *, const char*, BOOLEAN_P, BOOLEAN_P));
E void FDECL(check_mon_talk, (struct monst*));
E int FDECL(dochug, (struct monst *));
E boolean FDECL(m_digweapon_check, (struct monst *, XCHAR_P, XCHAR_P));
E int FDECL(m_move, (struct monst *, int));
E void FDECL(dissolve_bars, (int, int));
E boolean FDECL(closed_door, (int, int));
E boolean FDECL(accessible, (int, int));
E void FDECL(set_apparxy, (struct monst *));
E boolean FDECL(can_ooze, (struct monst *));
E boolean FDECL(can_fog, (struct monst *));
E boolean FDECL(should_displace,
                (struct monst *, coord *, long *, int, XCHAR_P, XCHAR_P));
E boolean FDECL(undesirable_disp, (struct monst *, XCHAR_P, XCHAR_P));
E boolean FDECL(m_findtravelpath, (struct monst*, int, xchar*, xchar*, long));
E boolean FDECL(m_test_move, (struct monst*, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, int, long));
E void FDECL(check_boss_fight, (struct monst*));

/* ### monst.c ### */

E void NDECL(monst_init);

/* ### monstr.c ### */

E void NDECL(monstr_init);

/* ### mplayer.c ### */

E struct monst *FDECL(mk_mplayer,
                      (struct permonst *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(create_mplayers, (int, BOOLEAN_P));
E void FDECL(mplayer_talk, (struct monst *));

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#ifndef WIN32
E int NDECL(tgetch);
#endif
#ifndef TOS
E char NDECL(switchar);
#endif
#ifndef __GO32__
E long FDECL(freediskspace, (char *));
#ifdef MSDOS
E int FDECL(findfirst_file, (char *));
E int NDECL(findnext_file);
E long FDECL(filesize_nh, (char *));
#else
E int FDECL(findfirst, (char *));
E int NDECL(findnext);
E long FDECL(filesize, (char *));
#endif /* MSDOS */
E char *NDECL(foundfile_buffer);
#endif /* __GO32__ */
E void FDECL(chdrive, (char *));
#ifndef TOS
E void NDECL(disable_ctrlP);
E void NDECL(enable_ctrlP);
#endif
#if defined(MICRO) && !defined(WINNT)
E void NDECL(get_scr_size);
#ifndef TOS
E void FDECL(gotoxy, (int, int));
#endif
#endif
#ifdef TOS
E int FDECL(_copyfile, (char *, char *));
E int NDECL(kbhit);
E void NDECL(set_colors);
E void NDECL(restore_colors);
#if defined(SUSPEND) && !defined(GNH_MOBILE)
E int NDECL(dosuspend);
#endif
#endif /* TOS */
#ifdef WIN32
E char *FDECL(get_username, (size_t *));
E void FDECL(nt_regularize, (char *));
E int NDECL((*nt_kbhit));
E void FDECL(Delay, (int));
#endif /* WIN32 */
#endif /* MICRO || WIN32 */

/* ### mthrowu.c ### */

E int FDECL(thitu, (int, int, struct obj **, const char *));
E int FDECL(ohitmon, (struct monst *, struct obj *, int, BOOLEAN_P));
E void FDECL(thrwmu, (struct monst *));
E int FDECL(spitmu, (struct monst *, struct attack *));
E int FDECL(breamu, (struct monst *, struct attack *));
E int FDECL(eyesmu, (struct monst*, struct attack*));
E boolean FDECL(linedup, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, int, BOOLEAN_P, int));
E boolean FDECL(lined_up, (struct monst *, BOOLEAN_P, int, BOOLEAN_P, int));
E struct obj *FDECL(m_carrying, (struct monst *, int));
E int FDECL(thrwmm, (struct monst *, struct monst *));
E int FDECL(spitmm, (struct monst *, struct attack *, struct monst *));
E int FDECL(breamm, (struct monst *, struct attack *, struct monst *));
E int FDECL(eyesmm, (struct monst*, struct attack*, struct monst*));
E int FDECL(buzzmm, (struct monst*, struct attack*, struct monst*));
E void FDECL(m_useupall, (struct monst *, struct obj *));
E void FDECL(m_useup, (struct monst *, struct obj *));
E void FDECL(m_throw, (struct monst *, int, int, int, int, int, struct obj *));
E void FDECL(hit_bars, (struct obj **, int, int, int, int,
                        BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(hits_bars, (struct obj **, int, int, int, int, int, int));
E const char* FDECL(get_eyestalk_ray_name, (int));
E const char* FDECL(get_breath_weapon_name, (int));
E void FDECL(set_m_ray_spell_stats, (struct monst*, struct attack*, struct monst*, int*, int*, int*, int*));
E void NDECL(reset_mthrowu);

/* ### muse.c ### */

E boolean FDECL(set_defensive_potion, (struct monst*, struct obj*));
E boolean FDECL(find_defensive, (struct monst *));
E int FDECL(use_defensive, (struct monst *));
E int FDECL(rnd_defensive_item, (struct monst *));
E boolean FDECL(find_offensive, (struct monst *));
#ifdef USE_TRAMPOLI
E int FDECL(mbhitm, (struct monst *, struct obj *, struct monst*));
#endif
E int FDECL(use_offensive, (struct monst *));
E int FDECL(rnd_offensive_item, (struct monst *));
E boolean FDECL(set_misc_potion, (struct monst *, struct obj *));
E boolean FDECL(find_misc, (struct monst*));
E int FDECL(use_misc, (struct monst *));
E int FDECL(rnd_misc_item, (struct monst *));
E boolean FDECL(searches_for_item, (struct monst *, struct obj *));
E boolean FDECL(mon_reflects, (struct monst *, const char *));
E boolean FDECL(ureflects, (const char *, const char *));
E void FDECL(mcureblindness, (struct monst *, BOOLEAN_P));
E void FDECL(mcuresickness, (struct monst*, BOOLEAN_P));
E void FDECL(mcurehallucination, (struct monst*, BOOLEAN_P));
E void FDECL(mcurestun, (struct monst*, BOOLEAN_P));
E void FDECL(mcureconfusion, (struct monst*, BOOLEAN_P));
E boolean FDECL(munstone, (struct monst *, BOOLEAN_P));
E boolean FDECL(munslime, (struct monst *, BOOLEAN_P));

/* ### music.c ### */

E void FDECL(awaken_soldiers, (struct monst *, BOOLEAN_P));
E int FDECL(do_play_instrument, (struct obj *));

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void NDECL(init_lan_features);
E char *NDECL(lan_username);
#endif

/* ### nhregex.c ### */
E struct nhregex *NDECL(regex_init);
E boolean FDECL(regex_compile, (const char *, struct nhregex *));
E const char *FDECL(regex_error_desc, (struct nhregex *));
E boolean FDECL(regex_match, (const char *, struct nhregex *));
E void FDECL(regex_free, (struct nhregex *));

/* ### npc.c ### */
E void FDECL(forget_npc_entry, (struct monst*));
E int FDECL(npc_move, (struct monst*));
E char FDECL(npc_room_occupied, (char*));
E struct monst* FDECL(findnpc, (CHAR_P));
E boolean FDECL(in_his_npc_room, (struct monst*));
E void FDECL(in_npc_room, (int));
E void FDECL(npcini, (d_level*, struct mkroom*, int, int, UCHAR_P, int));
E void FDECL(newenpc, (struct monst*));
E void FDECL(free_enpc, (struct monst*));
E void NDECL(clearnpcs);
E void FDECL(restnpc, (struct monst*, BOOLEAN_P));
E schar NDECL(min_npc_appearance_depth);

/* ### nttty.c ### */

#ifdef WIN32
E void NDECL(get_scr_size);
E int NDECL(nttty_kbhit);
E void FDECL(nttty_open, (int));
E void NDECL(nttty_rubout);
E int NDECL(tgetch);
E int FDECL(ntposkey, (int *, int *, int *));
E void FDECL(set_output_mode, (int));
E void NDECL(synch_cursor);
E void NDECL(GnollHack_enter_nttty);
E void NDECL(nttty_exit);
#endif

/* ### o_init.c ### */

E void NDECL(save_initial_objects_values);
E void NDECL(init_objects);
E void FDECL(obj_shuffle_range, (int, int *, int *));
E int NDECL(find_skates);
E void NDECL(oinit);
E void FDECL(savenames, (int, int));
E void NDECL(reset_names);
E void FDECL(restnames, (int));
E void FDECL(discover_object, (int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(undiscover_object, (int));
E int NDECL(dodiscovered);
E int NDECL(doclassdisco);
E void NDECL(rename_disco);
E void FDECL(randomize_mythic_quality, (struct obj*, UCHAR_P, uchar*, uchar*));
E boolean FDECL(is_mythic_affix_ok, (UCHAR_P, UCHAR_P, struct obj*, UCHAR_P));
E double FDECL(get_mythic_dmg_multiplier, (struct obj*, struct monst*, struct monst*));
E int FDECL(get_obj_exceptionality_ac_bonus, (struct obj*));
E int FDECL(get_obj_exceptionality_mc_bonus, (struct obj*));
E int FDECL(get_obj_material_and_exceptionality_ac_bonus, (struct obj*));
E int FDECL(get_obj_material_and_exceptionality_mc_bonus, (struct obj*));
E int FDECL(get_object_base_ac, (struct obj*));
E int FDECL(get_object_base_mc, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags2, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags3, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags4, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags5, (struct obj*));
E unsigned long FDECL(get_obj_oc_flags6, (struct obj*));


/* ### objects.c ### */

E void NDECL(objects_init);

/* ### matcomps.c ### */

E void NDECL(matcomps_init);

/* ### objnam.c ### */

E char *FDECL(obj_typename, (int));
E char *FDECL(simple_typename, (int));
E boolean FDECL(obj_is_pname, (struct obj *));
E char *FDECL(distant_name, (struct obj *, char *(*)(OBJ_P)));
E char *FDECL(fruitname, (BOOLEAN_P));
E struct fruit *FDECL(fruit_from_indx, (int));
E struct fruit *FDECL(fruit_from_name, (const char *, BOOLEAN_P, int *));
E void FDECL(reorder_fruit, (BOOLEAN_P));
E char *FDECL(xname, (struct obj *));
E char *FDECL(mshot_xname, (struct obj *));
E boolean FDECL(the_unique_obj, (struct obj *));
E boolean FDECL(the_unique_pm, (struct permonst *));
E boolean FDECL(erosion_matters, (struct obj *));
E char *FDECL(doname, (struct obj *));
E char* FDECL(doname_in_text, (struct obj*));
E char* FDECL(doname_in_text_with_price_and_weight_last, (struct obj*));
E char *FDECL(doname_with_price, (struct obj *));
E char *FDECL(doname_vague_quan, (struct obj *));
E char* FDECL(doname_with_weight_first_true, (struct obj*));
E char* FDECL(doname_with_weight_last_true, (struct obj*));
E char* FDECL(doname_with_weight_first, (struct obj*, BOOLEAN_P));
E char* FDECL(doname_with_weight_last, (struct obj*, BOOLEAN_P));
E char* FDECL(doname_with_price_and_weight_first, (struct obj*, BOOLEAN_P));
E char* FDECL(doname_with_price_and_weight_last, (struct obj*, BOOLEAN_P));
E boolean FDECL(not_fully_identified, (struct obj *));
E char *FDECL(corpse_xname, (struct obj *, const char *, unsigned));
E char *FDECL(cxname, (struct obj *));
E char* FDECL(acxname, (struct obj*));
E char *FDECL(cxname_singular, (struct obj *));
E char *FDECL(killer_xname, (struct obj *));
E char* FDECL(killer_xname_flags, (struct obj*, unsigned));
E char *FDECL(short_oname,
              (struct obj *, char *(*)(OBJ_P), char *(*)(OBJ_P), size_t));
E const char *FDECL(singular, (struct obj *, char *(*)(OBJ_P)));
E char *FDECL(an, (const char *));
E char *FDECL(An, (const char *));
E char *FDECL(The, (const char *));
E char *FDECL(the, (const char *));
E char *FDECL(aobjnam, (struct obj *, const char *));
E char *FDECL(yobjnam, (struct obj *, const char *));
E char *FDECL(Yobjnam2, (struct obj *, const char *));
E char *FDECL(Tobjnam, (struct obj *, const char *));
E char *FDECL(otense, (struct obj *, const char *));
E char *FDECL(vtense, (const char *, const char *));
E char *FDECL(Doname2, (struct obj *));
E char *FDECL(yname, (struct obj *));
E char *FDECL(Yname2, (struct obj *));
E char *FDECL(ysimple_name, (struct obj *));
E char *FDECL(Ysimple_name2, (struct obj *));
E char *FDECL(simpleonames, (struct obj *));
E char *FDECL(ansimpleoname, (struct obj *));
E char *FDECL(thesimpleoname, (struct obj *));
E char* FDECL(actualoname, (struct obj*));
E char *FDECL(bare_artifactname, (struct obj *));
E char *FDECL(makeplural, (const char *));
E char *FDECL(makesingular, (const char *));
E struct obj *FDECL(readobjnam, (char *, struct obj *, BOOLEAN_P, boolean*));
E int FDECL(rnd_class, (int, int));
E const char *FDECL(suit_simple_name, (struct obj *));
E const char *FDECL(cloak_simple_name, (struct obj *));
E const char* FDECL(robe_simple_name, (struct obj*));
E const char* FDECL(miscitem_simple_name, (struct obj*));
E const char *FDECL(helm_simple_name, (struct obj *));
E const char* FDECL(armor_class_simple_name, (struct obj*));
E const char *FDECL(mimic_obj_name, (struct monst *));
E char *FDECL(safe_qbuf, (char *, const char *, const char *, struct obj *,
                          char *(*)(OBJ_P), char *(*)(OBJ_P), const char *));
E int FDECL(shiny_obj, (CHAR_P));
E void FDECL(add_erosion_words, (struct obj*, char*));
E const char* FDECL(get_key_special_quality_description, (struct obj*));
E const char* FDECL(get_key_special_quality_description_by_otyp, (int, int));
E const char* FDECL(get_lock_description_by_otyp, (int, int, BOOLEAN_P));
E boolean FDECL(find_key_otyp_by_description, (char*, int*, int*));
E void FDECL(print_lock_with_buf, (char*, int, int, BOOLEAN_P));
E char* NDECL(nextobuf);
E char* FDECL(str_upper_start, (const char*));

/* ### options.c ### */

E void NDECL(reglyph_darkroom);
E boolean FDECL(match_optname, (const char *, const char *, size_t, BOOLEAN_P));
E void NDECL(process_options_file);
E void NDECL(init_options);
E void NDECL(finish_options);
E boolean FDECL(parseoptions, (char *, BOOLEAN_P, BOOLEAN_P));
E int NDECL(doset);
E int NDECL(dotoggledecorations);
E int NDECL(dotogglepickup);
E void NDECL(option_help);
E void FDECL(next_opt, (winid, const char *));
E int FDECL(fruitadd, (char *, struct fruit *));
E int FDECL(choose_classes_menu, (const char *, int, BOOLEAN_P,
                                  char *, char *));
E boolean FDECL(parsebindings, (char *));
E void FDECL(oc_to_str, (char *, char *));
E void FDECL(add_menu_cmd_alias, (CHAR_P, CHAR_P));
E char FDECL(get_menu_cmd_key, (CHAR_P));
E char FDECL(map_menu_cmd, (CHAR_P));
E void FDECL(show_menu_controls, (winid, BOOLEAN_P));
E void FDECL(assign_warnings, (nhsym*));
E char *FDECL(nh_getenv, (const char *));
E void FDECL(set_duplicate_opt_detection, (int));
E void FDECL(set_wc_option_mod_status, (unsigned long, int));
E void FDECL(set_wc2_option_mod_status, (unsigned long, int));
E void FDECL(set_option_mod_status, (const char *, int));
E int FDECL(add_autopickup_exception, (const char *));
E void NDECL(free_autopickup_exceptions);
E int FDECL(load_symset, (const char *, int));
E void NDECL(free_symsets);
E boolean FDECL(parsesymbols, (char *));
E struct symparse *FDECL(match_sym, (char *));
E void NDECL(set_playmode);
E nhsym FDECL(sym_val, (const char *));
E int FDECL(query_color, (const char *));
E int FDECL(query_attr, (const char *));
E const char *FDECL(clr2colorname, (int));
E int FDECL(match_str2clr, (char *));
E int FDECL(match_str2attr, (const char *, BOOLEAN_P));
E boolean FDECL(add_menu_coloring, (char *));
E boolean FDECL(get_menu_coloring, (const char *, int *, int *));
E int FDECL(get_menu_coloring_index, (const char*));
E boolean FDECL(free_menu_coloring_str, (const char*));
E void NDECL(free_menu_coloring);
E boolean FDECL(menu_style_allows_menu_coloring, (int));
E boolean FDECL(msgtype_parse_add, (char *));
E int FDECL(msgtype_type, (const char *, BOOLEAN_P));
E void FDECL(hide_unhide_msgtypes, (BOOLEAN_P, int));
E void NDECL(msgtype_free);

/* ### pager.c ### */

E char *FDECL(self_lookat, (char *));
E void FDECL(mhidden_description, (struct monst *, BOOLEAN_P, char *));
E boolean FDECL(object_from_map, (int,int,int,struct obj **));
E int FDECL(do_screen_description, (coord, BOOLEAN_P, nhsym, char *,
                                    const char **, struct permonst **));
E int FDECL(do_look, (int, coord *));
E int NDECL(dowhatis);
E int NDECL(doquickwhatis);
E int NDECL(doidtrap);
E int NDECL(dowhatdoes);
E char *FDECL(dowhatdoes_core, (CHAR_P, char *));
E int NDECL(dohelp);
E int NDECL(dohistory);

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
#ifdef CHDIR
E void FDECL(chdirx, (char *, BOOLEAN_P));
#endif /* CHDIR */
E boolean NDECL(authorize_wizard_mode);
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(flushout);
E int NDECL(dosh);
#ifdef MFLOPPY
E void FDECL(eraseall, (const char *, const char *));
E void FDECL(copybones, (int));
E void NDECL(playwoRAMdisk);
E int FDECL(saveDiskPrompt, (int));
E void NDECL(gameDiskPrompt);
#endif
E void FDECL(append_slash, (char *));
E void FDECL(getreturn, (const char *));
#ifndef AMIGA
E void VDECL(msmsg, (const char *, ...));
#endif
E FILE *FDECL(fopenp, (const char *, const char *));
#elif defined(UNIX) && defined(NOCWD_ASSUMPTIONS)
E void FDECL(append_slash, (char*));
#endif /* MICRO || WIN32 */

/* ### pctty.c ### */

#if defined(MICRO) || defined(WIN32)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void VDECL(error, (const char *, ...));
#if defined(TIMED_DELAY) && defined(_MSC_VER)
E void FDECL(msleep, (unsigned));
#endif
#endif /* MICRO || WIN32 */

/* ### pcunix.c ### */

#if defined(MICRO)
E void FDECL(regularize, (char *));
#endif /* MICRO */
#if defined(PC_LOCKING)
E void NDECL(getlock);
#endif

/* ### pickup.c ### */

E int FDECL(collect_obj_classes, (char *, struct obj *, BOOLEAN_P,
                                  boolean FDECL((*), (OBJ_P)), int *));
E boolean FDECL(rider_corpse_revival, (struct obj *, BOOLEAN_P));
E boolean FDECL(menu_class_present, (int));
E void FDECL(add_valid_menu_class, (int));
E boolean FDECL(allow_all, (struct obj *));
E boolean FDECL(allow_category, (struct obj *));
E boolean FDECL(is_potion_of_water, (struct obj*));
E boolean FDECL(is_scroll_of_remove_curse, (struct obj*));
E boolean FDECL(is_worn_by_type, (struct obj *));
E int FDECL(ck_bag, (struct obj *));
E int FDECL(stash_obj_in_container, (struct obj*, struct obj*));
#ifdef USE_TRAMPOLI
E int FDECL(in_container, (struct obj *));
E int FDECL(out_container, (struct obj *));
#endif
E int FDECL(pickup, (int));
E int FDECL(pickup_object, (struct obj *, long, BOOLEAN_P));
E int FDECL(query_category, (const char *, struct obj *, int,
                             menu_item **, int));
E int FDECL(query_objlist, (const char *, struct obj **, int,
                            menu_item **, int, boolean (*)(OBJ_P), int));
E void FDECL(add_weight_summary, (winid, int, int));
E void FDECL(add_weight_summary_putstr, (winid, int, int));
E int FDECL(enclevelmaximumweight, (enum encumbrance_types));
E int FDECL(enclevelminimumweight, (enum encumbrance_types));
E struct obj *FDECL(pick_obj, (struct obj *));
E int NDECL(encumber_msg);
E int FDECL(container_at, (int, int, BOOLEAN_P));
E int NDECL(doloot);
E void FDECL(observe_quantum_cat, (struct obj *, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(container_gone, (int (*)(OBJ_P)));
E boolean NDECL(u_handsy);
E int FDECL(use_container, (struct obj **, int, BOOLEAN_P));
E int FDECL(loot_mon, (struct monst *, int *, boolean *));
E int NDECL(dotip);
E boolean FDECL(is_autopickup_exception, (struct obj *, BOOLEAN_P));
E boolean FDECL(autopick_testobj, (struct obj *, BOOLEAN_P));
E boolean FDECL(maybe_disturb_container_monster, (struct obj* ));
E int NDECL(handle_knapsack_full);
E int NDECL(dostash);
E boolean NDECL(can_stash_objs);
E void NDECL(set_current_container_to_dummyobj);
E void NDECL(set_current_container_to_null);
E void NDECL(reset_pickup);

/* ### pline.c ### */

#if defined(DUMPLOG) || defined(DUMPHTML)
E void FDECL(dumplogmsg, (const char *, const char*, const char*, int, int));
E void NDECL(dumplogfreemessages);
#endif
E void VDECL(pline, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(custompline, (unsigned, const char *, ...)) PRINTF_F(2, 3);
E void VDECL(custompline_ex, (int, int, unsigned, const char*, ...)) PRINTF_F(4, 5);
E void VDECL(custompline_ex_prefix, (int, int, const char*, int, int, const char*, int, int, unsigned, const char*, ...)) PRINTF_F(10, 11);
E void VDECL(pline_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(pline_multi_ex, (int, int, int*, int*, const char*, ...)) PRINTF_F(5, 6);
E void VDECL(Norep, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(Norep_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void NDECL(free_youbuf);
E void VDECL(You, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(Your, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_feel, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_cant, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_hear, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_see, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(pline_The, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(There, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(You_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(You_multi_ex, (int, int, int*, int*, const char*, ...)) PRINTF_F(5, 6);
E void VDECL(Your_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(Your_multi_ex, (int, int, int*, int*, const char*, ...)) PRINTF_F(5, 6);
E void VDECL(You_feel_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(You_cant_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(You_hear_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(You_see_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(pline_The_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(There_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(verbalize_ex, (int, int, const char*, ...)) PRINTF_F(3, 4);
E void VDECL(verbalize, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(raw_printf, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(impossible, (const char *, ...)) PRINTF_F(1, 2);
E void VDECL(config_error_add, (const char *, ...)) PRINTF_F(1, 2);
E void NDECL(reset_pline);

/* ### polyself.c ### */

E void NDECL(set_uasmon);
E void NDECL(float_vs_flight);
E void NDECL(change_sex);
E void FDECL(polyself, (int));
E int FDECL(polymon, (int));
E int FDECL(monster_attribute_maximum, (struct permonst *, int));
E int FDECL(monster_attribute_minimum, (struct permonst*, int));
E void NDECL(rehumanize);
E int NDECL(dobreathe);
E int NDECL(dosteedbreathe);
E int NDECL(dospit);
E int NDECL(doremove);
E int NDECL(dospinweb);
E int NDECL(dosummon);
E int NDECL(dogaze);
E int NDECL(doeyestalk);
E int NDECL(dohide);
E int NDECL(dopoly);
E int NDECL(domindblast);
E int NDECL(dodryfountain);
E int NDECL(douseunicornhorn);
E int NDECL(doshriek);
E int NDECL(dolayegg);
E void NDECL(uunstick);
E void FDECL(skinback, (BOOLEAN_P));
E const char *FDECL(mbodypart, (struct monst *, int));
E const char *FDECL(body_part, (int));
E int NDECL(poly_gender);
E void FDECL(ugolemeffects, (int, double));
E boolean NDECL(ugenocided);
E const char *NDECL(udeadinside);
E int FDECL(get_ray_adtyp, (UCHAR_P));
E void NDECL(reset_polyself);

/* ### potion.c ### */

E void FDECL(set_itimeout, (long *, long));
E void FDECL(incr_itimeout, (long *, int));
E long FDECL(itimeout_incr, (long, int));
E void FDECL(make_confused, (long, BOOLEAN_P));
E void FDECL(make_stunned, (long, BOOLEAN_P));
E void FDECL(make_sick, (long, const char *, BOOLEAN_P, int));
E void FDECL(make_food_poisoned, (long, const char*, BOOLEAN_P, int));
E void FDECL(make_mummy_rotted, (long, const char*, BOOLEAN_P, int));
E void FDECL(make_slimed, (long, const char*, int, const char*, int));
E void FDECL(make_stoned, (long, const char *, int, const char *, int));
E void FDECL(make_vomiting, (long, BOOLEAN_P));
E void FDECL(make_blinded, (long, BOOLEAN_P));
E void NDECL(toggle_blindness);
E boolean FDECL(make_hallucinated, (long, BOOLEAN_P, long));
E void FDECL(make_deaf, (long, BOOLEAN_P));
E void NDECL(self_invis_message);
E int NDECL(dodrink);
E int FDECL(dopotion, (struct obj *));
E int FDECL(peffects, (struct obj *));
E void FDECL(healup, (int, int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(strange_feeling, (struct obj *, const char *, BOOLEAN_P));
E void FDECL(potionhit, (struct monst *, struct obj **, int));
E void FDECL(potionbreathe, (struct obj *, const char*));
E int NDECL(dodip);
E int FDECL(refill_obj_with_oil, (struct obj*, struct obj*));
E void FDECL(mongrantswish, (struct monst **));
E void FDECL(djinni_from_bottle, (struct obj *));
E struct monst *FDECL(split_mon, (struct monst *, struct monst *));
E const char *NDECL(bottlename);
E void NDECL(reset_potion);
E boolean FDECL(H2Opotion_dip, (struct obj*, struct obj*, BOOLEAN_P, const char*));

/* ### pray.c ### */

E boolean FDECL(critically_low_hp, (BOOLEAN_P));
E boolean NDECL(stuck_in_wall);
#ifdef USE_TRAMPOLI
E int NDECL(prayer_done);
#endif
E int NDECL(dosacrifice);
E boolean FDECL(can_pray, (BOOLEAN_P));
E int NDECL(dopray);
E const char *NDECL(u_gname);
E int NDECL(doturn);
E const char *NDECL(a_gname);
E const char *FDECL(a_gname_at, (XCHAR_P x, XCHAR_P y));
E const char *FDECL(align_gname, (ALIGNTYP_P));
E const char* FDECL(align_gdesc, (ALIGNTYP_P));
E const char* FDECL(align_ghisher, (ALIGNTYP_P));
E const char* NDECL(u_ghisher);
E const char *FDECL(halu_gname, (ALIGNTYP_P));
E const char *FDECL(align_gtitle, (ALIGNTYP_P));
E boolean FDECL(align_gfemale, (ALIGNTYP_P));
E void FDECL(altar_wrath, (int, int));
E int NDECL(absolution_spell);
E int NDECL(wiz_crown);
E int NDECL(get_artifact_replacement_item_otyp);
E void NDECL(removealtarsummons);

/* ### priest.c ### */

E int FDECL(move_special, (struct monst *, BOOLEAN_P, SCHAR_P, BOOLEAN_P,
                           BOOLEAN_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P));
E char FDECL(temple_occupied, (char *));
E boolean FDECL(inhistemple, (struct monst *));
E int FDECL(pri_move, (struct monst *));
E void FDECL(priestini, (d_level *, struct mkroom *, int, int, BOOLEAN_P, int));
E aligntyp FDECL(mon_aligntyp, (struct monst *));
E char *FDECL(priestname, (struct monst *, char *));
E boolean FDECL(p_coaligned, (struct monst *));
E struct monst *FDECL(findpriest, (CHAR_P));
E void FDECL(intemple, (int));
E void FDECL(forget_temple_entry, (struct monst *));
E void FDECL(priest_talk, (struct monst *));
E struct monst *FDECL(mk_roamer, (struct permonst *, ALIGNTYP_P, XCHAR_P,
                                  XCHAR_P, BOOLEAN_P));
E void FDECL(reset_hostility, (struct monst *));
E boolean FDECL(in_your_sanctuary, (struct monst *, XCHAR_P, XCHAR_P));
E void FDECL(ghod_hitsu, (struct monst *));
E void NDECL(angry_priest);
E void NDECL(clearpriests);
E void FDECL(restpriest, (struct monst *, BOOLEAN_P));
E void FDECL(newepri, (struct monst *));
E void FDECL(free_epri, (struct monst *));
E const char *FDECL(align_str, (ALIGNTYP_P));
E char *FDECL(piousness, (BOOLEAN_P, const char *));
E void FDECL(mstatusline, (struct monst *));
E void FDECL(print_mstatusline, (char*, struct monst*, int, BOOLEAN_P));
E void NDECL(ustatusline);

E char FDECL(smithy_occupied, (char*));
E struct monst* FDECL(findsmith, (CHAR_P));
E boolean FDECL(inhissmithy, (struct monst*));
E void FDECL(insmithy, (int));
E void FDECL(smithini, (d_level*, struct mkroom*, int, int, UCHAR_P, int));
E void FDECL(forget_smithy_entry, (struct monst*));
E void FDECL(newesmi, (struct monst*));
E void FDECL(free_esmi, (struct monst*));
E void NDECL(clearsmiths);
E void FDECL(restsmith, (struct monst*, BOOLEAN_P));
E int FDECL(smith_move, (struct monst*));

/* ### prop.c ### */
E void NDECL(props_init);
E const char* FDECL(get_property_name, (int));
E const char* FDECL(get_status_name, (struct monst*, int));

/* ### quest.c ### */

E void NDECL(onquest);
E void NDECL(nemdead);
E void FDECL(artitouch, (struct obj *));
E boolean NDECL(ok_to_quest);
E boolean FDECL(leader_speaks, (struct monst *));
E boolean FDECL(nemesis_speaks, (struct monst*));
E boolean FDECL(quest_chat, (struct monst *, BOOLEAN_P));
E boolean FDECL(quest_talk, (struct monst *, BOOLEAN_P));
E void FDECL(quest_stat_check, (struct monst *));
E void FDECL(finish_quest, (struct obj *));
E void FDECL(achievement_gained, (const char*));

/* ### questpgr.c ### */

E void NDECL(load_qtlist);
E void NDECL(unload_qtlist);
E short FDECL(quest_info, (int));
E const char *NDECL(ldrname);
E boolean FDECL(is_quest_artifact, (struct obj *));
E struct obj *FDECL(find_quest_artifact, (unsigned));
E void FDECL(com_pager, (struct monst*, int));
E void FDECL(com_pager_ex, (struct monst*, int, int, int, BOOLEAN_P));
E void FDECL(qt_pager, (struct monst*, int));
E void FDECL(qt_pager_ex, (struct monst*, int, int, int, BOOLEAN_P));
E struct permonst *NDECL(qt_montype);
E void NDECL(deliver_splev_message);
E void FDECL(write_quest_texts, (int));

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void FDECL(srandom, (unsigned));
E char *FDECL(initstate, (unsigned, char *, int));
E char *FDECL(setstate, (char *));
E long NDECL(random);
#endif /* RANDOM */

/* ### read.c ### */

E void FDECL(learnscroll, (struct obj *));
E char *FDECL(tshirt_text, (struct obj *, char *));
E int NDECL(doread);
E boolean FDECL(is_chargeable, (struct obj *));
E void FDECL(recharge, (struct obj *, int, BOOLEAN_P, const char*, BOOLEAN_P));
E void FDECL(forget_objects, (int));
E void FDECL(forget_levels, (int));
E void NDECL(forget_traps);
E void FDECL(forget_map, (int));
E int FDECL(seffects, (struct obj *, boolean*, struct monst*));
E void FDECL(drop_boulder_on_player,
             (BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(drop_boulder_on_monster, (int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(wand_explode, (struct obj *, int));
#ifdef USE_TRAMPOLI
E void FDECL(set_lit, (int, int, genericptr_t));
#endif
E void FDECL(litroom, (BOOLEAN_P, struct obj *));
E void FDECL(do_genocide, (int));
E void FDECL(punish, (struct obj *));
E void NDECL(unpunish);
E boolean FDECL(cant_revive, (int *, BOOLEAN_P, struct obj *));
E boolean NDECL(create_particular);
E int FDECL(maybe_tame, (struct monst*, struct obj*, struct monst*));
E int FDECL(maybe_controlled, (struct monst*, struct obj*, struct monst*));
E void FDECL(read_the_ruling_ring, (struct obj*));
E void FDECL(strip_charges, (struct obj*, BOOLEAN_P, BOOLEAN_P));
E void FDECL(p_glow1, (struct obj*, int, int, const char*, BOOLEAN_P));
E void FDECL(p_glow2, (struct obj*, const char*, int, int, const char*, BOOLEAN_P));
E void FDECL(dragon_scales_to_scale_mail, (struct obj*, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(get_valid_targeted_position, (int, int, int));
E boolean FDECL(get_valid_stinking_cloud_pos, (int, int));
E boolean FDECL(is_valid_stinking_cloud_pos, (int, int, BOOLEAN_P));
E void FDECL(display_stinking_cloud_positions, (int));
E int FDECL(bless_or_curse, (struct obj*, struct monst*, BOOLEAN_P));
E int FDECL(remove_curse, (struct obj*, struct monst*, BOOLEAN_P));
E void NDECL(reset_read);

/* ### rect.c ### */

E void NDECL(init_rect);
E NhRect *FDECL(get_rect, (NhRect *));
E NhRect *NDECL(rnd_rect);
E void FDECL(remove_rect, (NhRect *));
E void FDECL(add_rect, (NhRect *));
E void FDECL(split_rects, (NhRect *, NhRect *));

/* ## region.c ### */
E void NDECL(clear_regions);
E void NDECL(run_regions);
E boolean FDECL(in_out_region, (XCHAR_P, XCHAR_P));
E boolean FDECL(m_in_out_region, (struct monst *, XCHAR_P, XCHAR_P));
E void NDECL(update_player_regions);
E void FDECL(update_monster_region, (struct monst *));
E NhRegion *FDECL(visible_region_at, (XCHAR_P, XCHAR_P));
E void FDECL(show_region, (NhRegion *, XCHAR_P, XCHAR_P));
E void FDECL(save_regions, (int, int));
E void FDECL(rest_regions, (int, BOOLEAN_P));
E void FDECL(remove_expired_regions, (BOOLEAN_P));
E void FDECL(region_stats, (const char *, char *, long *, size_t *));
E NhRegion *FDECL(create_gas_cloud, (XCHAR_P, XCHAR_P, enum region_types, int, int, int, int));
E NhRegion* FDECL(create_simple_permanent_region, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, enum region_types));
E boolean NDECL(region_danger);
E void NDECL(region_safety);
E struct nhregion* FDECL(find_rid, (int));
E int FDECL(get_rid, (struct nhregion*));
E void FDECL(do_light_regions, (char**));

/* ### restore.c ### */

E void FDECL(inven_inuse, (BOOLEAN_P));
E int FDECL(dorecover_saved_game, (int));
E int FDECL(dorecover, (int));
E void FDECL(restcemetery, (int, struct cemetery **));
E void FDECL(trickery, (char *));
E void FDECL(getlev, (int, int, XCHAR_P, BOOLEAN_P));
E void FDECL(get_plname_from_file, (int, char *));
E void FDECL(get_save_game_stats_from_file, (int, struct save_game_stats*));
#ifdef SELECTSAVED
E int FDECL(restore_menu, (winid));
E int FDECL(select_saved_game, (winid, UCHAR_P, struct save_game_data*));
#endif
E void NDECL(minit);
E boolean FDECL(lookup_id_mapping, (unsigned, unsigned *));
E void FDECL(mread, (int, genericptr_t, size_t));
E int FDECL(validate, (int, const char *));
E void NDECL(reset_restpref);
E void FDECL(set_restpref, (const char *));
E void FDECL(set_savepref, (const char *));

/* ### rip.c ### */

E void FDECL(genl_outrip, (winid, int, time_t));

/* ### rm.c ### */
E void NDECL(init_rm);
E enum obj_material_types FDECL(get_location_type_material, (int, int));
E int FDECL(get_initial_location_subtype, (int));
E int FDECL(get_initial_location_vartype, (int, int));
E int FDECL(get_initial_tree_subtype, (SCHAR_P));
E const char* FDECL(get_door_name_at, (XCHAR_P, XCHAR_P));
E const char* FDECL(get_door_name_at_ptr, (struct rm*));
E const char* FDECL(get_short_door_name_at, (XCHAR_P, XCHAR_P));
E const char* FDECL(get_short_door_name_at_ptr, (struct rm*));
E boolean FDECL(m_can_destroy_door, (struct monst*, struct rm*, BOOLEAN_P));
E boolean FDECL(is_door_indestructible_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_indestructible_at_ptr, (struct rm*));
E boolean FDECL(is_door_nonpassable_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_nonpassable_at_ptr, (struct rm*));
E boolean FDECL(has_door_normal_lock_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(has_door_normal_lock_at_ptr, (struct rm*));
E boolean FDECL(has_box_normal_lock, (struct obj*));
E boolean FDECL(is_door_diggable_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_diggable_at_ptr, (struct rm*));
E boolean FDECL(is_door_kickable_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_kickable_at_ptr, (struct rm*));
E boolean FDECL(is_door_destroyed_by_booby_trap_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_destroyed_by_booby_trap_at_ptr, (struct rm*));
E boolean FDECL(is_door_destroyed_by_striking_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_destroyed_by_striking_at_ptr, (struct rm*));
E boolean FDECL(is_door_destroyed_by_monsters_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_destroyed_by_monsters_at_ptr, (struct rm*));
E boolean FDECL(door_blocks_vision_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(door_blocks_vision_at_ptr, (struct rm*));
E boolean FDECL(is_door_normally_without_lock_at, (XCHAR_P, XCHAR_P));
E boolean FDECL(is_door_normally_without_lock_at_ptr, (struct rm*));
E void FDECL(set_initial_location_floortype, (struct rm*, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(initialize_location, (struct rm*));
E void FDECL(set_initial_location_type_at, (XCHAR_P, XCHAR_P, int));
E void FDECL(set_initial_location_type_ptr, (struct rm*, int));

/* ### rnd.c ### */

#ifdef USE_ISAAC64
E void FDECL(init_isaac64, (unsigned long, int FDECL((*fn), (int))));
E long NDECL(nhrand);
#endif
E int FDECL(rn2, (int));
E int FDECL(rn2_on_display_rng, (int));
E int FDECL(rnl, (int));
E int FDECL(rnd, (int));
E int FDECL(d, (int, int));
E int FDECL(rne, (int));
E int FDECL(rnz, (int));
E int FDECL(rngh, (int, int));
E int FDECL(dd, (int, int, int));

/* ### role.c ### */

E boolean FDECL(validrole, (int));
E boolean FDECL(validrace, (int, int));
E boolean FDECL(validgend, (int, int, int));
E boolean FDECL(validalign, (int, int, int));
E int FDECL(randrole, (BOOLEAN_P));
E int FDECL(randrace, (int));
E int FDECL(randgend, (int, int));
E int FDECL(randalign, (int, int));
E int FDECL(str2role, (const char *));
E int FDECL(str2race, (const char *));
E int FDECL(str2gend, (const char *));
E int FDECL(str2align, (const char *));
E boolean FDECL(ok_role, (int, int, int, int));
E int FDECL(pick_role, (int, int, int, int));
E boolean FDECL(ok_race, (int, int, int, int));
E int FDECL(pick_race, (int, int, int, int));
E boolean FDECL(ok_gend, (int, int, int, int));
E int FDECL(pick_gend, (int, int, int, int));
E boolean FDECL(ok_align, (int, int, int, int));
E int FDECL(pick_align, (int, int, int, int));
E void NDECL(rigid_role_checks);
E boolean FDECL(setrolefilter, (const char *));
E boolean NDECL(gotrolefilter);
E void NDECL(clearrolefilter);
E char *FDECL(build_plselection_prompt, (char *, size_t, int, int, int, int, BOOLEAN_P));
E char *FDECL(root_plselection_prompt, (char *, size_t, int, int, int, int));
E void NDECL(plnamesuffix);
E void FDECL(role_selection_prolog, (int, winid));
E void FDECL(role_menu_extra, (int, winid, BOOLEAN_P));
E void NDECL(role_init);
E const char *FDECL(Hello, (struct monst *));
E const char *NDECL(Goodbye);
E int NDECL(u_to_glyph);
E int FDECL(player_to_glyph_index, (int, int, int, int, int));
E boolean FDECL(player_has_action_tile, (enum action_tile_types, int, int, int, int, int));
E unsigned long FDECL(get_player_action_flags, (enum action_tile_types, int, int, int, int, int));
E unsigned long FDECL(u_action_flags,(enum action_tile_types));
E unsigned long NDECL(u_item_use_flags);
#if 0
E boolean FDECL(player_has_attack_tile, (int, int, int, int, int));
E boolean FDECL(player_has_throw_tile, (int, int, int, int, int));
E boolean FDECL(player_has_fire_tile, (int, int, int, int, int));
E boolean FDECL(player_has_cast_tile, (int, int, int, int, int));
E boolean FDECL(player_has_special_attack_tile, (int, int, int, int, int));
E boolean FDECL(player_has_kick_tile, (int, int, int, int, int));
E boolean FDECL(player_has_item_use_tile, (int, int, int, int, int));
E boolean FDECL(player_has_door_use_tile, (int, int, int, int, int));
E boolean FDECL(player_has_death_tile, (int, int, int, int, int));
#endif

E int FDECL(glyph_to_player_mon, (int));
E int FDECL(attack_glyph_to_player_mon, (int));
E int FDECL(throw_glyph_to_player_mon, (int));
E int FDECL(fire_glyph_to_player_mon, (int));
E int FDECL(nodir_cast_glyph_to_player_mon, (int));
E int FDECL(dir_cast_glyph_to_player_mon, (int));
E int FDECL(special_attack_glyph_to_player_mon, (int));
E int FDECL(kick_glyph_to_player_mon, (int));
E int FDECL(item_use_glyph_to_player_mon, (int));
E int FDECL(door_use_glyph_to_player_mon, (int));
E int FDECL(death_glyph_to_player_mon, (int));
E int FDECL(passive_defense_glyph_to_player_mon, (int));
E int FDECL(special_attack2_glyph_to_player_mon, (int));
E int FDECL(special_attack3_glyph_to_player_mon, (int));
E void NDECL(save_initial_urolerace_values);
E void NDECL(reset_urolerace);

/* ### rumors.c ### */

E char *FDECL(getrumor, (int, char *, BOOLEAN_P));
E char *FDECL(get_rnd_text, (const char *, char *, int FDECL((*), (int))));
E void FDECL(outrumor, (struct monst*, struct obj*, int, int));
E void FDECL(outoracle, (struct monst*, struct obj*, BOOLEAN_P, int));
E void FDECL(save_oracles, (int, int));
E void NDECL(reset_oracles);
E void FDECL(restore_oracles, (int));
E int FDECL(doconsult, (struct monst *));
E int FDECL(do_oracle_identify, (struct monst*));
E int FDECL(do_oracle_enlightenment, (struct monst*));
E void NDECL(rumor_check);

/* ### save.c ### */

E int NDECL(dosave);
E int FDECL(dosave0, (BOOLEAN_P));
E int NDECL(check_existing_save_file);
E boolean FDECL(tricked_fileremoved, (int, char *));
#ifdef INSURANCE
E void NDECL(savestateinlock);
#endif
#ifdef MFLOPPY
E boolean FDECL(savelev, (int, XCHAR_P, int));
E boolean FDECL(swapin_file, (int));
E void NDECL(co_false);
#else
E void FDECL(savelev, (int, XCHAR_P, int));
#endif
E genericptr_t FDECL(mon_to_buffer, (struct monst *, int *));
E void FDECL(bufon, (int));
E void FDECL(bufoff, (int));
E void FDECL(bflush, (int));
E void FDECL(bwrite, (int, genericptr_t, size_t));
E void FDECL(bclose, (int));
E void FDECL(def_bclose, (int));
#if defined(ZEROCOMP)
E void FDECL(zerocomp_bclose, (int));
#endif
E void FDECL(savecemetery, (int, int, struct cemetery **));
E void FDECL(savefruitchn, (int, int));
E void NDECL(reset_fruitchn);
E void FDECL(store_plname_in_file, (int));
E void FDECL(store_save_game_stats_in_file, (int));
E void NDECL(free_dungeons);
E void NDECL(free_dynamic_data_A);
E void NDECL(free_dynamic_data_B);
E void NDECL(free_dynamic_data_C);
E void NDECL(freedynamicdata);
E void FDECL(store_savefileinfo, (int));

/* ### shk.c ### */

E void FDECL(setpaid, (struct monst *));
E long FDECL(money2mon, (struct monst *, long));
E void FDECL(money2u, (struct monst *, long));
E void FDECL(shkgone, (struct monst *));
E void FDECL(set_residency, (struct monst *, BOOLEAN_P));
E void FDECL(replshk, (struct monst *, struct monst *));
E void FDECL(restshk, (struct monst *, BOOLEAN_P));
E char FDECL(inside_shop, (XCHAR_P, XCHAR_P));
E void FDECL(u_left_shop, (char *, BOOLEAN_P));
E void FDECL(remote_burglary, (XCHAR_P, XCHAR_P));
E void FDECL(u_entered_shop, (char *));
E void FDECL(pick_pick, (struct obj *));
E boolean FDECL(same_price, (struct obj *, struct obj *));
E void NDECL(shopper_financial_report);
E int FDECL(inhishop, (struct monst *));
E struct monst *FDECL(shop_keeper, (CHAR_P));
E boolean FDECL(tended_shop, (struct mkroom *));
E boolean FDECL(is_unpaid, (struct obj *));
E void FDECL(delete_contents, (struct obj *));
E void FDECL(obfree, (struct obj *, struct obj *));
E void FDECL(home_shk, (struct monst *, BOOLEAN_P));
E void FDECL(make_happy_shk, (struct monst *, BOOLEAN_P));
E void FDECL(make_happy_shoppers, (BOOLEAN_P));
E void FDECL(hot_pursuit, (struct monst *));
E void FDECL(make_angry_shk, (struct monst *, XCHAR_P, XCHAR_P));
E struct monst* NDECL(can_pay_to_shkp);
E int NDECL(dopay);
E boolean FDECL(paybill, (int, BOOLEAN_P));
E void NDECL(finish_paybill);
E struct obj *FDECL(find_oid, (unsigned));
E long FDECL(contained_cost,
             (struct obj *, struct monst *, long, BOOLEAN_P, BOOLEAN_P));
E long FDECL(contained_gold, (struct obj *));
E void FDECL(picked_container, (struct obj *));
E void FDECL(gem_learned, (int));
E void FDECL(alter_cost, (struct obj *, long));
E long FDECL(unpaid_cost, (struct obj *, BOOLEAN_P));
E boolean FDECL(billable, (struct monst **, struct obj *, CHAR_P, BOOLEAN_P));
E void FDECL(addtobill, (struct obj *, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(splitbill, (struct obj *, struct obj *));
E void FDECL(subfrombill, (struct obj *, struct monst *));
E long FDECL(stolen_value,
             (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(sellobj_state, (int));
E void FDECL(sellobj, (struct obj *, XCHAR_P, XCHAR_P));
E int FDECL(doinvbill, (int));
E struct monst *FDECL(shkcatch, (struct obj *, XCHAR_P, XCHAR_P));
E void FDECL(add_damage, (XCHAR_P, XCHAR_P, long));
E int FDECL(repair_damage, (struct monst *, struct damage *, int *,
                            BOOLEAN_P));
E int FDECL(shk_move, (struct monst *));
E void FDECL(after_shk_move, (struct monst *));
E boolean FDECL(is_fshk, (struct monst *));
E void FDECL(shopdig, (int));
E void FDECL(pay_for_damage, (const char *, BOOLEAN_P));
E boolean FDECL(costly_spot, (XCHAR_P, XCHAR_P));
E struct obj *FDECL(shop_object, (XCHAR_P, XCHAR_P));
E void FDECL(price_quote, (struct obj *));
E void FDECL(shk_chat, (struct monst *));
E void FDECL(check_unpaid_usage, (struct obj *, BOOLEAN_P));
E void FDECL(check_unpaid, (struct obj *));
E void FDECL(costly_gold, (XCHAR_P, XCHAR_P, long));
E long FDECL(get_cost_of_shop_item, (struct obj *, int *));
E int FDECL(oid_price_adjustment, (struct obj *, unsigned));
E boolean FDECL(block_door, (XCHAR_P, XCHAR_P));
E boolean FDECL(block_entry, (XCHAR_P, XCHAR_P));
E char *FDECL(shk_your, (char *, struct obj *));
E char *FDECL(Shk_Your, (char *, struct obj *));
E long FDECL(get_cost, (struct obj*, struct monst*));
E int FDECL(shk_chastise_pet, (struct monst* mtmp, struct obj*, BOOLEAN_P));
E long FDECL(set_cost, (struct obj*, struct monst*));
E boolean FDECL(is_obj_on_shk_bill, (struct obj*, struct monst*));
E void FDECL(add_one_tobill, (struct obj*, BOOLEAN_P, struct monst*));
E void NDECL(reset_shk);

/* ### shknam.c ### */

E void FDECL(neweshk, (struct monst *));
E void FDECL(free_eshk, (struct monst *));
E void FDECL(stock_room, (int, struct mkroom *, BOOLEAN_P));
E int FDECL(get_gehennom_undead_spellcaster, (int));
E boolean FDECL(saleable, (struct monst *, struct obj *));
E int FDECL(get_shop_item, (int));
E char *FDECL(Shknam, (struct monst *));
E char *FDECL(shkname, (struct monst *));
E char* FDECL(true_shkname, (struct monst*));
E boolean FDECL(shkname_is_pname, (struct monst *));
E boolean FDECL(is_izchak, (struct monst *, BOOLEAN_P));
E const char* FDECL(shoptypename, (int));
E boolean FDECL(veggy_item, (struct obj* obj, int));
E void FDECL(izchak_talk, (struct monst*));
E long FDECL(get_object_base_value, (struct obj*));

/* ### sit.c ### */

E void NDECL(take_gold);
E int NDECL(dosit);
E void NDECL(rndcurse);
E void NDECL(attrcurse);
E void FDECL(m_attrcurse, (struct monst*));

/* ### sounds.c ### */

E void NDECL(dosounds);
E const char *FDECL(growl_sound, (struct monst *));
E void FDECL(growl, (struct monst *));
E void FDECL(yelp, (struct monst *));
E void FDECL(whimper, (struct monst *));
E void FDECL(beg, (struct monst *));
E int NDECL(doyell);
E int NDECL(dotalk);
E int NDECL(dotalksteed);
E int NDECL(dotalknearby);
#ifdef USER_SOUNDS
E int FDECL(add_sound_mapping, (const char *));
E void FDECL(play_sound_for_message, (const char *));
#endif
E int FDECL(count_sellable_items, (struct monst*));
E struct obj* FDECL(get_first_sellable_item, (struct monst*));
E boolean FDECL(m_sellable_item, (struct obj*, struct monst*));
E long FDECL(get_cost_of_monster_item, (struct obj*, struct monst*));
E long FDECL(m_contained_cost, (struct obj*, struct monst*));
E boolean FDECL(is_shop_item_type, (struct obj*, int));
E boolean FDECL(is_npc_item_identification_type, (struct obj*, int));
E int FDECL(release_item_from_hero_inventory, (struct obj*));
E int FDECL(ask_shk_reconciliation, (struct monst*));
E double FDECL(service_cost_charisma_adjustment, (int));
E void NDECL(genl_chat_message);
E void FDECL(popup_talk_lines, (struct monst*, const char**));
E void FDECL(popup_talk_line, (struct monst*, const char*));
E void FDECL(popup_talk_line_noquotes, (struct monst*, const char*));
E void FDECL(popup_talk_line_ex, (struct monst*, const char*, int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(popup_talk_line_with_know_mname, (struct monst*, const char*, BOOLEAN_P));
E int FDECL(service_identify, (struct monst*, long));
E void FDECL(You_ex1_popup, (const char*, const char*, int, int, int, unsigned long));
E int FDECL(dochatmon, (struct monst*));
E boolean FDECL(maybe_refillable_with_oil, (struct obj*));


/* ### soundset.c ### */

E void NDECL(update_game_music);
E void NDECL(play_level_ambient_sounds);
E void NDECL(play_environment_ambient_sounds);
E enum floor_surface_types FDECL(get_floor_surface_type, (int, int, BOOLEAN_P));
E void FDECL(play_movement_sound, (struct monst*, enum climbing_types));
E void FDECL(play_object_floor_sound, (struct obj*, enum object_sound_types, BOOLEAN_P));
E void FDECL(play_object_floor_sound_at_location, (struct obj*, enum object_sound_types, int, int, BOOLEAN_P));
E void FDECL(play_object_container_in_sound, (struct obj*, struct obj*));
E enum hit_surface_types FDECL(get_hit_surface_type, (enum hit_surface_source_types, ANY_P*));
E void FDECL(get_hit_location, (enum hit_surface_source_types, ANY_P*, xchar*, xchar*));
E void FDECL(play_simple_object_sound, (struct obj*, enum object_sound_types));
E void FDECL(play_simple_object_sound_at_location, (struct obj*, int, int, enum object_sound_types));
E void FDECL(set_simple_object_sound_id_and_volume, (enum object_soundset_types, enum object_sound_types, enum ghsound_types*, float*));
E void FDECL(play_simple_container_sound, (struct obj*, enum container_sound_types));
E void FDECL(play_simple_container_sound_at_location, (struct obj*, int, int, enum container_sound_types));
E void FDECL(set_simple_container_sound_id_and_volume, (enum object_soundset_types, enum container_sound_types, enum ghsound_types*, float*));
E void FDECL(play_object_hit_sound, (struct obj*, enum hit_surface_source_types, ANY_P*, double, enum hmon_atkmode_types));
E void FDECL(play_object_hit_sound_at_location, (struct obj*, enum hit_surface_source_types, ANY_P*, double, enum hmon_atkmode_types, XCHAR_P, XCHAR_P));
E void FDECL(play_simple_player_sound, (enum monster_sound_types));
E void FDECL(play_simple_player_sound_with_flags, (enum monster_sound_types, unsigned long));
E void FDECL(play_player_line_indexed_sound, (enum monster_sound_types, int, enum sound_play_groups play_group, enum immediate_sound_types, unsigned long));
E void FDECL(play_player_ouch_sound, (enum monster_ouch_sounds));
E void FDECL(play_player_flounder_sound, (enum monster_flounder_sounds));
E void FDECL(play_player_pissed_sound, (enum monster_pissed_sounds));
E void FDECL(play_simple_player_voice, (enum player_voice_types));
E void FDECL(set_simple_player_sound_id_and_volume, (enum player_soundset_types, enum monster_sound_types, enum ghsound_types*, float*));
E void FDECL(set_simple_player_voice_id_and_volume, (enum player_soundset_types, enum player_voice_types, enum ghsound_types*, float*));
E void FDECL(play_simple_monster_sound, (struct monst*, enum monster_sound_types));
E void FDECL(play_simple_monster_sound_with_flags, (struct monst*, enum monster_sound_types, unsigned long));
E void FDECL(set_simple_monster_sound_id_and_volume, (enum monster_soundset_types, enum monster_sound_types, enum ghsound_types*, float*));
E void FDECL(play_monster_simple_weapon_sound, (struct monst*, int, struct obj*, enum object_sound_types));
E void FDECL(play_monster_weapon_hit_sound, (struct monst*, enum hit_surface_source_types, ANY_P*, int, struct obj*, double, enum hmon_atkmode_types));
E void FDECL(play_monster_weapon_hit_sound_at_location, (struct monst*, enum hit_surface_source_types, ANY_P*, int, struct obj*, double, enum hmon_atkmode_types, XCHAR_P, XCHAR_P));
E void FDECL(play_monster_attack_floor_sound, (struct monst*, int, enum object_sound_types));
E void FDECL(set_simple_location_sound_id_and_volume, (enum location_soundset_types, enum location_sound_types, enum ghsound_types*, float*));
E void FDECL(play_simple_location_sound, (XCHAR_P, XCHAR_P, enum location_sound_types));
E void FDECL(play_ui_sound, (enum ui_sound_types));
E void FDECL(play_sfx_sound, (enum sfx_sound_types));
E void FDECL(play_sfx_sound_with_flags, (enum sfx_sound_types, unsigned long));
E void FDECL(play_sfx_sound_at_location, (enum sfx_sound_types, int, int));
E void FDECL(play_sfx_sound_at_location_with_minimum_volume, (enum sfx_sound_types, int, int, double));
E void FDECL(play_immediate_instrument_sound, (enum object_instrument_soundset_types, enum instrument_sound_types));
E void FDECL(play_immediate_instrument_sound_at_location, (enum object_instrument_soundset_types, enum instrument_sound_types, int, int));
E void FDECL(play_immediate_ray_sound_at_location, (int, enum ray_sound_types, int, int));
E void FDECL(start_ambient_ray_sound_at_location, (int, int, int));
E void FDECL(update_ambient_ray_sound_to_location, (int, int, int));
E void FDECL(stop_ambient_ray_sound, (int));
E void FDECL(start_occupation_ambient_sound, (enum object_soundset_types, enum object_occupation_types));
E void FDECL(stop_occupation_ambient_sound, (enum object_soundset_types, enum object_occupation_types));
E void FDECL(play_occupation_immediate_sound, (enum object_soundset_types, enum object_occupation_types, enum occupation_sound_types));
E void FDECL(play_occupation_immediate_sound_at_location, (enum object_soundset_types, enum object_occupation_types, enum occupation_sound_types, int, int));
E enum player_soundset_types NDECL(get_player_soundset);
E void NDECL(dosetsoundvolume);
E void FDECL(update_hearing_array, (int));
E void NDECL(update_ambient_sounds);
E void NDECL(update_hearing_array_and_ambient_sounds);
E void NDECL(clear_hearing_array_and_ambient_sounds);
E int NDECL(get_max_hearing_distance);
E void FDECL(update_hearing_array_and_ambient_sounds_if_point_within_hearing_range, (int, int));
E void FDECL(unblock_vision_and_hearing_at_point, (int, int));
E void FDECL(block_vision_and_hearing_at_point, (int, int));
E void FDECL(new_sound_source, (XCHAR_P, XCHAR_P, enum ghsound_types, double, enum soundsource_types, enum soundsource_ambient_subtypes, ANY_P*));
E void FDECL(del_sound_source, (enum soundsource_types, ANY_P*));
E void FDECL(save_sound_sources, (int, int, int));
E void NDECL(reset_sound_sources);
E void FDECL(restore_sound_sources, (int));
E void FDECL(sound_stats, (const char*, char*, long*, size_t*));
E void FDECL(relink_sound_sources, (BOOLEAN_P));
E void NDECL(sound_sources_sanity_check);
E void FDECL(obj_move_sound_source, (struct obj*, struct obj*));
E boolean NDECL(any_sound_source);
E void FDECL(snuff_sound_source, (int, int));
E void FDECL(obj_split_sound_source, (struct obj*, struct obj*));
E void FDECL(obj_merge_sound_sources, (struct obj*, struct obj*));
E void FDECL(obj_adjust_sound_volume, (struct obj*, double));
E void FDECL(begin_sound, (struct obj*, BOOLEAN_P));
E void FDECL(end_sound, (struct obj*, BOOLEAN_P));
E boolean FDECL(obj_has_sound_source, (struct obj*));
E enum ghsound_types FDECL(obj_ambient_sound, (struct obj*));
E double FDECL(obj_ambient_sound_volume, (struct obj*));
E enum ghsound_types FDECL(get_location_ambient_sound_type, (XCHAR_P, XCHAR_P, double*, enum soundsource_ambient_subtypes*));
E enum ghsound_types FDECL(get_dungeon_music, (int));
E enum ghsound_types FDECL(get_level_music, (struct d_level*));
E enum ghsound_types FDECL(get_room_music, (struct mkroom*));
E enum ghsound_types FDECL(get_level_ambient_sounds, (struct d_level*));
E enum ghsound_types FDECL(get_room_ambient_sounds, (struct mkroom*));
E enum ghsound_types NDECL(get_environment_ambient_sounds);
E enum ghsound_types FDECL(get_monster_ambient_sound_id, (enum monster_soundset_types));
E float FDECL(get_monster_ambient_sound_volume, (enum monster_soundset_types));
E void FDECL(get_god_indices, (int*, int*));
E void NDECL(play_intro_text);
E void NDECL(stop_all_immediate_sounds);
E void NDECL(stop_all_long_immediate_sounds);
E void FDECL(stop_all_dialogue_of_mon, (struct monst*));
E void FDECL(play_voice_shopkeeper_welcome, (struct monst*, int));
E void FDECL(play_voice_shopkeeper_simple_line, (struct monst*, enum shopkeeper_lines));
E void FDECL(play_voice_shopkeeper_leave_pick_outside, (struct monst*, const char*, int, BOOLEAN_P));
E void FDECL(play_voice_shopkeeper_cad_line, (struct monst*, enum shopkeeper_cad_lines, const char*));
E void FDECL(play_voice_shopkeeper_candelabrum_candles, (struct monst*, struct obj*));
E void FDECL(play_voice_shopkeeper_izchak_talks, (struct monst*, int));
E void FDECL(play_voice_shopkeeper_for_you, (struct monst*, int, int, long));
E void FDECL(play_voice_shopkeeper_pay_before_buying, (struct monst*, long, long));
E void FDECL(play_voice_shopkeeper_how_dare_you_damage, (struct monst*, UCHAR_P, const char*, BOOLEAN_P));
E void FDECL(play_voice_shopkeeper_costly_alteration, (struct monst*, struct obj*, enum cost_alteration_types));
E void FDECL(play_voice_monster_cuss, (struct monst*, int));
E void FDECL(play_voice_monster_cuss_with_god_name, (struct monst*, int));
E void FDECL(play_voice_monster_advice, (struct monst*, BOOLEAN_P));
E void FDECL(play_voice_wizard_of_yendor_cuss, (struct monst*, int, int));
E void FDECL(play_voice_wizard_of_yendor_simple_line, (struct monst*, enum wizard_of_yendor_simple_lines));
E void FDECL(play_monster_line_indexed_sound, (struct monst*, enum monster_sound_types, int, enum sound_play_groups, enum immediate_sound_types, double, unsigned long));
E void FDECL(play_monster_msg_indexed_sound, (struct monst*, enum monster_sound_types, int, enum sound_play_groups, enum immediate_sound_types, double, unsigned long));
E void FDECL(play_monster_simply_indexed_sound, (struct monst*, enum monster_sound_types, int, enum sound_play_groups, enum immediate_sound_types, double, const char*, unsigned long));
E void FDECL(play_monster_item_trading_line, (struct monst*, enum monster_item_trading_lines));
E void FDECL(play_monster_standard_dialogue_line, (struct monst*, enum monster_standard_dialogue_lines));
E void FDECL(play_monster_special_dialogue_line_with_flags, (struct monst*, int, unsigned long));
E void FDECL(play_monster_special_dialogue_line, (struct monst*, int));
E void FDECL(play_monster_standard_sound, (struct monst*, enum monster_standard_sounds));
E void FDECL(play_monster_special_sound_with_flags, (struct monst*, int, unsigned long));
E void FDECL(play_monster_special_sound, (struct monst*, int));
E void FDECL(play_monster_ouch_sound, (struct monst*, enum monster_ouch_sounds));
E void FDECL(play_monster_who_sound, (struct monst*, enum monster_who_sounds));
E void FDECL(play_monster_switchable_who_sound, (struct monst*, enum monster_who_sounds, enum monster_standard_dialogue_lines));
E void FDECL(play_monster_happy_sound, (struct monst*, enum monster_happy_sounds));
E void FDECL(play_monster_unhappy_sound, (struct monst*, enum monster_unhappy_sounds));
E void FDECL(play_monster_flounder_sound, (struct monst*, enum monster_flounder_sounds));
E void FDECL(play_monster_pissed_sound, (struct monst*, enum monster_pissed_sounds));
E void FDECL(play_monster_chat_sound, (struct monst*, int));
E enum monster_soundset_types FDECL(get_monster_soundset, (struct monst*));
E enum monster_soundset_types FDECL(get_monster_soundset_with_subtype, (struct monst*, int*));
E void FDECL(play_voice_com_pager, (struct monst*, int, BOOLEAN_P));
E void FDECL(play_voice_quest_pager, (struct monst*, int, BOOLEAN_P));
E void FDECL(play_voice_quest_leader_whoareyou, (struct monst*));
E void FDECL(play_voice_god_simple_line_at, (XCHAR_P, XCHAR_P, enum god_simple_lines));
E void FDECL(play_voice_god_simple_line_by_align, (ALIGNTYP_P, enum god_simple_lines));
E void FDECL(play_voice_oracle_major_consultation, (struct monst*, int));
E void FDECL(play_hermit_dialogue_line, (struct monst*, enum ghsound_types, UCHAR_P, int));


/* ### sys.c ### */

E void NDECL(sys_early_init);
E void NDECL(sysopt_release);
E void FDECL(sysopt_seduce_set, (int));
E void NDECL(reset_global_variables);
E void NDECL(reset_item_global_variables);

/* ### sys/msdos/sound.c ### */

#ifdef MSDOS
E int FDECL(assign_soundcard, (char *));
#endif

/* ### sp_lev.c ### */

E boolean FDECL(check_room, (xchar *, xchar *, xchar *, xchar *, BOOLEAN_P));
E boolean FDECL(create_room, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P,
                              XCHAR_P, XCHAR_P, XCHAR_P, int, int, int, int, SHORT_P));
E void FDECL(create_secret_door, (struct mkroom *, XCHAR_P));
E boolean
FDECL(dig_corridor, (coord *, coord *, BOOLEAN_P, SCHAR_P, SCHAR_P));
E void FDECL(fill_room, (struct mkroom *, BOOLEAN_P));
E boolean FDECL(load_special, (const char *));
E xchar FDECL(selection_getpoint, (int, int, struct opvar *));
E struct opvar *FDECL(selection_opvar, (char *));
E void FDECL(opvar_free_x, (struct opvar *));
E void FDECL(set_selection_floodfillchk, (int FDECL((*), (int,int))));
E void FDECL(selection_floodfill, (struct opvar *, int, int, BOOLEAN_P));
E void FDECL(create_carpet, (XCHAR_P, XCHAR_P, XCHAR_P, XCHAR_P, int));
E void NDECL(reset_splev);

/* ### spell.c ### */

E void FDECL(book_cursed, (struct obj *));
#ifdef USE_TRAMPOLI
E int NDECL(learn);
#endif
E int FDECL(study_book, (struct obj *));
E void FDECL(book_disappears, (struct obj *));
E void FDECL(book_substitution, (struct obj *, struct obj *));
E void NDECL(age_spells);
E int NDECL(docast);
E int NDECL(dospellmanage);
E int NDECL(dospellview);
E int FDECL(spell_skilltype, (int));
E int FDECL(spelleffects, (int, BOOLEAN_P, struct monst*));
E int NDECL(export_spells_for_wiki);
E int FDECL(spelldescription, (int));
E const char* FDECL(get_obj_saving_throw_description, (struct obj*));
E const char* FDECL(get_otyp_saving_throw_description, (int));
E int FDECL(setspellhotkey, (int));
E int FDECL(forgetspell, (int));
E int FDECL(spell_skill_base_success_bonus, (int));
E double FDECL(spell_skill_ulevel_success_bonus_per_level, (int));
E int FDECL(spell_skill_ulevel_success_bonus, (int));
E double FDECL(spell_skill_mana_cost_multiplier, (int));
E int NDECL(dospellmanagemenu);
E int FDECL(tport_spell, (int));
E void NDECL(losespells);
E int NDECL(dovspell);
E int NDECL(dosortspell);
E void FDECL(initialspell, (struct obj *));
E boolean FDECL(already_learnt_spell_type, (int));
E int NDECL(domix);
E int FDECL(getspellcooldown, (int));
E const char* FDECL(domatcompname, (struct materialcomponent*));
E int FDECL(subdirtype2explosiontype, (int));
E void FDECL(addspellintrinsictimeout, (int));
E double FDECL(get_spell_mana_cost, (int));
E double FDECL(get_spellbook_adjusted_mana_cost, (int));
E void FDECL(deduct_mana_cost, (double));
E uchar FDECL(is_obj_component_for, (int, struct obj*));
#if defined (DUMPLOG) || defined (DUMPHTML)
E void NDECL(dump_spells);
#endif
E void NDECL(sortspells);
E void FDECL(print_spell_level_text, (char*, int, BOOLEAN_P, UCHAR_P));
E void NDECL(reset_spells);
E long FDECL(get_object_spell_casting_penalty, (struct obj*));

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int NDECL(stealarm);
#endif
E long FDECL(somegold, (long));
E void FDECL(stealgold, (struct monst *));
E void FDECL(remove_worn_item, (struct obj *, BOOLEAN_P));
E int FDECL(steal, (struct monst *, char *));
E int FDECL(mpickobj, (struct monst *, struct obj *));
E void FDECL(stealamulet, (struct monst *));
E void FDECL(maybe_absorb_item, (struct monst *, struct obj *, int, int));
E void FDECL(mdrop_obj, (struct monst *, struct obj *, BOOLEAN_P));
E void FDECL(mdrop_special_objs, (struct monst *));
E void FDECL(release_monster_objects, (struct monst *, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(mdrop_droppable_objs, (struct monst*));
E struct obj *FDECL(findgold, (struct obj *));
E struct obj* FDECL(findobjecttype, (struct obj*, int));

/* ### steed.c ### */

E void NDECL(rider_cant_reach);
E boolean FDECL(can_saddle, (struct monst *));
E int FDECL(use_saddle, (struct obj *));
E void FDECL(put_saddle_on_mon, (struct obj *, struct monst *));
E boolean FDECL(can_ride, (struct monst *));
E int NDECL(doride);
E int NDECL(doridenearby);
E boolean FDECL(mount_steed, (struct monst *));
E void NDECL(exercise_steed);
E void NDECL(kick_steed);
E void FDECL(dismount_steed, (int));
E void FDECL(place_monster, (struct monst *, int, int));
E boolean FDECL(stucksteed, (BOOLEAN_P));

/* ### teleport.c ### */

E boolean FDECL(goodpos, (int, int, struct monst *, unsigned long));
E boolean FDECL(enexto, (coord *, XCHAR_P, XCHAR_P, struct permonst *));
E boolean FDECL(enexto_core, (coord *, XCHAR_P, XCHAR_P,
                              struct permonst *, unsigned long));
E void FDECL(teleds, (int, int, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(safe_teleds, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(teleds_with_effects, (int, int, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(safe_teleds_with_effects, (BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(teleport_pet, (struct monst *, BOOLEAN_P));
E void NDECL(tele);
E void NDECL(controlled_teleportation);
E void NDECL(wiztele);
E boolean FDECL(scrolltele, (struct obj *, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(modronportaltele, (struct trap*, struct monst*));
E int NDECL(dotelecmd);
E int FDECL(dotele, (BOOLEAN_P));
E void FDECL(level_tele, (int, int, d_level));
E void FDECL(domagicportal, (struct trap *));
E void FDECL(level_teleport_effect_out, (int, int));
E void FDECL(level_teleport_effect_in, (int, int));
E void FDECL(tele_trap, (struct trap *));
E void FDECL(level_tele_trap, (struct trap *, unsigned));
E void FDECL(rloc_to, (struct monst *, int, int));
E void FDECL(rloc_to_with_effects, (struct monst*, int, int));
E boolean FDECL(rloc, (struct monst *, BOOLEAN_P));
E boolean FDECL(rloc_with_effects, (struct monst*, BOOLEAN_P));
E boolean FDECL(rloc2, (struct monst*, BOOLEAN_P, BOOLEAN_P));
E boolean FDECL(tele_restrict, (struct monst *));
E void FDECL(mtele_trap, (struct monst *, struct trap *, int));
E int FDECL(mlevel_tele_trap, (struct monst *, struct trap *,
                               BOOLEAN_P, int));
E boolean FDECL(rloco, (struct obj *));
E int NDECL(random_teleport_level);
E boolean FDECL(u_teleport_mon, (struct monst *, BOOLEAN_P));
E void NDECL(reset_teleport);


/* ### tile.c ### */
#ifdef USE_TILES
E void FDECL(substitute_tiles, (d_level *));
#endif


/* ### tiledata.c ###*/
E void NDECL(init_tiledata);
E boolean FDECL(has_generic_player_action_tile, (enum action_tile_types action));
E int FDECL(process_tiledata, (int, const char*, int*, uchar*));
E boolean FDECL(is_dir_from_base_dir, (int, int, boolean*, boolean*));
E boolean FDECL(is_zap_char_from_base_zap_char, (int, int, boolean*, boolean*));
E int FDECL(get_enlargement_animation, (int));


/* ### timeout.c ### */

E void NDECL(burn_away_slime);
E void NDECL(nh_timeout);
E void FDECL(fall_asleep, (int, BOOLEAN_P));
E void FDECL(attach_egg_hatch_timeout, (struct obj *, long));
E void FDECL(attach_fig_transform_timeout, (struct obj *));
E void FDECL(kill_egg, (struct obj *));
E void FDECL(hatch_egg, (ANY_P *, long));
E void FDECL(learn_egg_type, (int));
E void FDECL(learn_corpse_type, (int));
E void FDECL(burn_object, (ANY_P *, long));
E void FDECL(begin_burn, (struct obj *, BOOLEAN_P));
E void FDECL(end_burn, (struct obj *, BOOLEAN_P));
E void FDECL(unsummon_item, (ANY_P*, long));
E void FDECL(begin_existence, (struct obj*));
E void FDECL(unsummon_monster, (ANY_P*, long));
E void FDECL(begin_summontimer, (struct monst*));
E void FDECL(begin_timestoptimer, (long));
E void FDECL(restart_time, (ANY_P*, long));
E void NDECL(do_storms);
E boolean FDECL(start_timer, (long, SHORT_P, SHORT_P, ANY_P *));
E long FDECL(stop_timer, (SHORT_P, ANY_P *));
E long FDECL(peek_timer, (SHORT_P, ANY_P *));
E void NDECL(run_timers);

E void FDECL(obj_move_timers, (struct obj *, struct obj *));
E void FDECL(obj_split_timers, (struct obj *, struct obj *));
E void FDECL(obj_stop_timers, (struct obj *));
E boolean FDECL(obj_has_timer, (struct obj *, SHORT_P));

E void FDECL(mon_move_timers, (struct monst*, struct monst*));
E void FDECL(mon_split_timers, (struct monst*, struct monst*));
E void FDECL(mon_stop_timers, (struct monst*));
E boolean FDECL(mon_has_timer, (struct monst*, SHORT_P));

E void FDECL(spot_stop_timers, (XCHAR_P, XCHAR_P, SHORT_P));
E long FDECL(spot_time_expires, (XCHAR_P, XCHAR_P, SHORT_P));
E long FDECL(spot_time_left, (XCHAR_P, XCHAR_P, SHORT_P));
E boolean FDECL(obj_is_local, (struct obj *));
E void FDECL(save_timers, (int, int, int));
E void FDECL(restore_timers, (int, int, BOOLEAN_P, long));
E void NDECL(reset_timers);
E void FDECL(timer_stats, (const char *, char *, long *, size_t *));
E void FDECL(relink_timers, (BOOLEAN_P));
E int NDECL(wiz_timeout_queue);
E void NDECL(timer_sanity_check);
E void FDECL(make_sound_object, (ANY_P*, long));
E void FDECL(property_expiry_message, (int, BOOLEAN_P));

/* ### topten.c ### */

E void FDECL(formatkiller, (char *, size_t, int, BOOLEAN_P));
E int FDECL(observable_depth, (d_level *));
E void FDECL(topten, (int, time_t));
E void FDECL(prscore, (int, char **));
E struct toptenentry *NDECL(get_rnd_toptenentry);
E struct obj *FDECL(tt_oname, (struct obj *));
E long NDECL(encodeconduct);
E long NDECL(encodeachieve);

/* ### track.c ### */

E void NDECL(initrack);
E void NDECL(settrack);
E coord *FDECL(gettrack, (int, int));
E void NDECL(reest_track);

/* ### trap.c ### */

E boolean FDECL(burnarmor, (struct monst *));
E int FDECL(erode_obj, (struct obj *, const char *, int, int));
E boolean FDECL(grease_protect, (struct obj *, const char *, struct monst *));
E struct trap *FDECL(maketrap, (int, int, int, int, unsigned long));
E void FDECL(fall_through, (BOOLEAN_P));
E struct monst *FDECL(animate_statue,
                      (struct obj *, XCHAR_P, XCHAR_P, int, int *));
E struct monst *FDECL(activate_statue_trap,
                      (struct trap *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(set_utrap, (unsigned long, UCHAR_P));
E void FDECL(reset_utrap, (BOOLEAN_P));
E void FDECL(dotrap, (struct trap *, UNSIGNED_SHORT_P));
E void FDECL(seetrap, (struct trap *));
E void FDECL(feeltrap, (struct trap *));
E int FDECL(mintrap, (struct monst *));
E void FDECL(instapetrify, (const char *));
E void FDECL(minstapetrify, (struct monst *, BOOLEAN_P));
E void FDECL(start_delayed_petrification, (struct monst*, BOOLEAN_P));
E void FDECL(start_delayed_sliming, (struct monst*, BOOLEAN_P));
E void FDECL(selftouch, (const char *));
E void FDECL(mselftouch, (struct monst *, const char *, BOOLEAN_P));
E void NDECL(float_up);
E void FDECL(fill_pit, (int, int));
E int FDECL(float_down, (long, long));
E void NDECL(climb_pit);
E boolean FDECL(fire_damage, (struct obj *, BOOLEAN_P, XCHAR_P, XCHAR_P));
E int FDECL(fire_damage_chain,
            (struct obj *, BOOLEAN_P, BOOLEAN_P, XCHAR_P, XCHAR_P));
E boolean FDECL(lava_damage, (struct obj *, XCHAR_P, XCHAR_P));
E void acid_damage(struct obj *);
E int FDECL(water_damage, (struct obj *, const char *, BOOLEAN_P));
E void FDECL(water_damage_chain, (struct obj *, BOOLEAN_P));
E boolean NDECL(drown);
E void FDECL(drain_en, (int));
E int NDECL(dountrap);
E void FDECL(cnv_trap_obj, (int, int, struct trap *, BOOLEAN_P));
E int FDECL(untrap, (BOOLEAN_P));
E int FDECL(check_all_box_traps, (BOOLEAN_P));
E int FDECL(check_box_trap, (struct obj*, BOOLEAN_P));
E boolean FDECL(openholdingtrap, (struct monst *, boolean *));
E boolean FDECL(closeholdingtrap, (struct monst *, boolean *));
E boolean FDECL(openfallingtrap, (struct monst *, BOOLEAN_P, boolean *));
E boolean FDECL(chest_trap, (struct obj *, int, BOOLEAN_P));
E void FDECL(deltrap, (struct trap *));
E boolean FDECL(delfloortrap, (struct trap *));
E struct trap *FDECL(t_at, (int, int));
E void FDECL(b_trapped, (const char *, int, XCHAR_P, XCHAR_P));
E boolean NDECL(unconscious);
E void FDECL(blow_up_landmine, (struct trap *));
E int FDECL(launch_obj, (SHORT_P, int, int, int, int, int));
E boolean NDECL(launch_in_progress);
E void NDECL(force_launch_placement);
E boolean FDECL(uteetering_at_seen_pit, (struct trap *));
E boolean NDECL(lava_effects);
E void NDECL(sink_into_lava);
E void NDECL(sokoban_guilt);
E int FDECL(untrap_probability, (int, int));
E const char* FDECL(get_trap_name, (int));
E const char* FDECL(get_trap_explanation, (struct trap*));
E int FDECL(get_shooting_trap_object, (struct trap*));
E void NDECL(reset_traps);

/* ### u_init.c ### */

E void NDECL(u_init);
E void NDECL(u_skills_init);
E const char* FDECL(get_role_achievement_description, (BOOLEAN_P));
E unsigned long FDECL(mon_known_spell_schools, (struct monst*));
E boolean FDECL(is_known_spell_school, (unsigned long, int));

/* ### uhitm.c ### */

E void FDECL(erode_armor, (struct monst *, int));
E boolean FDECL(attack_checks, (struct monst *, struct obj *));
E void FDECL(check_caitiff, (struct monst *));
E int FDECL(find_roll_to_hit, (struct monst *, UCHAR_P, struct obj *,
                               int *, int *));
E boolean FDECL(attack, (struct monst *));
E boolean FDECL(hmon, (struct monst *, struct obj *, int, int, boolean *));
E int FDECL(damageum, (struct monst *, struct attack *, struct obj*, int));
E void FDECL(missum, (struct monst *, struct attack *, BOOLEAN_P));
E int FDECL(passive, (struct monst *, struct obj *, BOOLEAN_P, int,
                      UCHAR_P, BOOLEAN_P));
E void FDECL(passive_obj, (struct monst *, struct obj *, struct attack *));
E void FDECL(stumble_onto_mimic, (struct monst *));
E int FDECL(flash_hits_mon, (struct monst *, struct obj *));
E void FDECL(light_hits_gremlin, (struct monst *, int));
E double FDECL(adjust_damage, (int, struct monst*, struct monst*, SHORT_P, unsigned long));
E int FDECL(calculate_damage_dealt_to_player, (double));
E int FDECL(deduct_player_hp, (double));
E int FDECL(deduct_monster_hp, (struct monst*, double));
E void FDECL(get_game_difficulty_multipliers, (double*, double*));
E void FDECL(get_game_difficulty_multipliers_by_level, (double*, double*, SCHAR_P));
E void FDECL(update_u_facing, (UCHAR_P));
E void FDECL(update_u_action, (enum action_tile_types));
E void FDECL(update_m_action, (struct monst*, enum action_tile_types));
E void FDECL(update_u_action_core, (enum action_tile_types, unsigned long, unsigned long));
E void FDECL(update_m_action_core, (struct monst*, enum action_tile_types, unsigned long, unsigned long));
E void FDECL(update_u_action_revert, (enum action_tile_types));
E void FDECL(update_m_action_revert, (struct monst*, enum action_tile_types));
E void FDECL(update_u_action_and_wait, (enum action_tile_types));
E void FDECL(update_m_action_and_wait, (struct monst*, enum action_tile_types));
E void FDECL(display_being_hit, (struct monst*, int, int, enum hit_tile_types, int, unsigned long));
E void FDECL(display_u_being_hit, (enum hit_tile_types, int, unsigned long));
E void FDECL(display_m_being_hit, (struct monst*, enum hit_tile_types, int, unsigned long, BOOLEAN_P));
E void NDECL(u_wait_until_action);
E void NDECL(m_wait_until_action);
E void NDECL(u_wait_until_end);
E void NDECL(m_wait_until_end);
E void FDECL(remove_monster_and_nearby_waitforu, (struct monst*));


/* ### unixmain.c ### */

#if defined(UNIX) && !defined(GNH_MOBILE)
#ifdef PORT_HELP
E void NDECL(port_help);
#endif
E void FDECL(sethanguphandler, (void (*)(int)));
E boolean NDECL(authorize_wizard_mode);
E boolean FDECL(check_user_string, (char *));
E char *NDECL(get_login_name);
#endif /* UNIX */

/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *, ...)) PRINTF_F(1, 2);
#endif /* UNIX || __BEOS__ */

/* ### unixunix.c ### */

#if defined(UNIX)
#if !defined(GNH_MOBILE)
E void NDECL(getlock);
E void FDECL(regularize, (char *));
#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void FDECL(msleep, (unsigned));
#endif
#ifdef SHELL
E int NDECL(dosh);
#endif /* SHELL */
#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int FDECL(child, (int));
#endif
#ifdef PANICTRACE
E boolean FDECL(file_exists, (const char *));
#endif
#else
E void FDECL(regularize, (char*));
#endif /* GNH_MOBILE */
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
#ifdef GNOME_GRAPHICS
E int FDECL(hide_privileges, (BOOLEAN_P));
#endif
#endif /* UNIX */

/* ### vault.c ### */

E void FDECL(newegd, (struct monst *));
E void FDECL(free_egd, (struct monst *));
E boolean FDECL(grddead, (struct monst *));
E struct monst *NDECL(findgd);
E void NDECL(vault_summon_gd);
E char FDECL(vault_occupied, (char *));
E void FDECL(uleftvault, (struct monst *));
E void NDECL(invault);
E int FDECL(gd_move, (struct monst *));
E void FDECL(paygd, (BOOLEAN_P));
E long NDECL(hidden_gold);
E boolean NDECL(gd_sound);
E void FDECL(vault_gd_watching, (unsigned int));
E long NDECL(carried_gem_value);
E long FDECL(contained_gem_value, (struct obj*));

/* ### version.c ### */

E char *FDECL(version_string, (char *));
E char *FDECL(getversionstring, (char *));
E int NDECL(doversion);
E int NDECL(doextversion);
#ifdef MICRO
E boolean FDECL(comp_times, (long));
#endif
E boolean FDECL(check_version, (struct version_info *, const char *, BOOLEAN_P));
E boolean FDECL(uptodate, (int, const char *));
E void FDECL(store_version, (int));
E unsigned long FDECL(get_feature_notice_ver, (char *));
E unsigned long NDECL(get_current_feature_ver);
E const char *FDECL(copyright_banner_line, (int));
E void FDECL(early_version_info, (BOOLEAN_P));

#ifdef RUNTIME_PORT_ID
E char *FDECL(get_port_id, (char *));
#endif
#ifdef RUNTIME_PASTEBUF_SUPPORT
E void FDECL(port_insert_pastebuf, (char *));
#endif

/* ### video.c ### */

#ifdef MSDOS
E int FDECL(assign_video, (char *));
#ifdef NO_TERMS
E void NDECL(gr_init);
E void NDECL(gr_finish);
#endif
E void FDECL(tileview, (BOOLEAN_P));
#endif
#ifdef VIDEOSHADES
E int FDECL(assign_videoshades, (char *));
E int FDECL(assign_videocolors, (char *));
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void NDECL(vis_tab_init);
#endif

/* ### vision.c ### */

E void NDECL(vision_init);
E int FDECL(does_block, (int, int, struct rm *));
E void NDECL(vision_reset);
E void FDECL(vision_recalc, (int));
E void FDECL(block_point, (int, int));
E void FDECL(unblock_point, (int, int));
E boolean FDECL(clear_path, (int, int, int, int));
E void FDECL(do_clear_area, (int, int, int,
                             void (*)(int, int, genericptr), genericptr_t));
E unsigned FDECL(howmonseen, (struct monst *));
E void NDECL(reset_vision);

#ifdef VMS

/* ### vmsfiles.c ### */

E int FDECL(vms_link, (const char *, const char *));
E int FDECL(vms_unlink, (const char *));
E int FDECL(vms_creat, (const char *, unsigned int));
E int FDECL(vms_open, (const char *, int, unsigned int));
E boolean FDECL(same_dir, (const char *, const char *));
E int FDECL(c__translate, (int));
E char *FDECL(vms_basename, (const char *));

/* ### vmsmail.c ### */

E unsigned long NDECL(init_broadcast_trapping);
E unsigned long NDECL(enable_broadcast_trapping);
E unsigned long NDECL(disable_broadcast_trapping);
#if 0
E struct mail_info *NDECL(parse_next_broadcast);
#endif /*0*/

/* ### vmsmain.c ### */

E int FDECL(main, (int, char **));
#ifdef CHDIR
E void FDECL(chdirx, (const char *, BOOLEAN_P));
#endif /* CHDIR */
E void FDECL(sethanguphandler, (void (*)(int)));
E boolean NDECL(authorize_wizard_mode);

/* ### vmsmisc.c ### */

E void NDECL(vms_abort) NORETURN;
E void FDECL(vms_exit, (int)) NORETURN;
#ifdef PANICTRACE
E void FDECL(vms_traceback, (int));
#endif

/* ### vmstty.c ### */

E int NDECL(vms_getchar);
E void NDECL(gettty);
E void FDECL(settty, (const char *));
E void FDECL(shuttty, (const char *));
E void NDECL(setftty);
E void NDECL(intron);
E void NDECL(introff);
E void VDECL(error, (const char *, ...)) PRINTF_F(1, 2);
#ifdef TIMED_DELAY
E void FDECL(msleep, (unsigned));
#endif

/* ### vmsunix.c ### */

E void NDECL(getlock);
E void FDECL(regularize, (char *));
E int NDECL(vms_getuid);
E boolean FDECL(file_is_stmlf, (int));
E int FDECL(vms_define, (const char *, const char *, int));
E int FDECL(vms_putenv, (const char *));
E char *NDECL(verify_termcap);
#if defined(CHDIR) || defined(SHELL) || defined(SECURE)
E void NDECL(privoff);
E void NDECL(privon);
#endif
#ifdef SHELL
E int NDECL(dosh);
#endif
#if defined(SHELL) || defined(MAIL)
E int FDECL(vms_doshell, (const char *, BOOLEAN_P));
#endif
#if defined(SUSPEND) && !defined(GNH_MOBILE)
E int NDECL(dosuspend);
#endif
#ifdef SELECTSAVED
E int FDECL(vms_get_saved_games, (const char *, char ***));
#endif

#endif /* VMS */

/* ### weapon.c ### */

E int FDECL(practice_needed_to_advance, (int, int));
E const char *FDECL(weapon_descr, (struct obj *));
E const char* FDECL(weapon_skill_name, (struct obj*));
E const char* FDECL(get_skill_name, (int));
E const char* FDECL(get_skill_plural_name, (int));
E int FDECL(m_weapon_range, (struct monst*, struct obj*, struct obj*));
E int FDECL(weapon_range, (struct obj*, struct obj*));
E int FDECL(basehitval, (struct obj*, struct monst*, struct monst*, int));
E int FDECL(weapon_to_hit_value, (struct obj *, struct monst *, struct monst*, int));
E int FDECL(weapon_dmg_value, (struct obj *, struct monst *, struct monst*, int));
E int FDECL(weapon_extra_dmg_value, (struct obj*, struct monst*, struct monst*, int));
E int FDECL(weapon_total_dmg_value, (struct obj*, struct monst*, struct monst*, int));
E int FDECL(get_critical_strike_percentage_chance, (struct obj*, struct monst*, struct monst*));
E boolean FDECL(eligible_for_extra_damage, (struct obj*, struct monst*, struct monst*));
E int FDECL(special_dmgval, (struct monst *, struct monst *, long, long *));
E void FDECL(silver_sears, (struct monst *, struct monst *, long));
E struct obj *FDECL(select_rwep, (struct monst *));
E boolean FDECL(monmightthrowwep, (struct obj *));
E struct obj *FDECL(select_hwep, (struct monst *, BOOLEAN_P, XCHAR_P, XCHAR_P));
E struct obj* FDECL(select_multiweapon_nth_hwep, (struct monst*, int));
E void FDECL(possibly_unwield, (struct monst *, BOOLEAN_P));
E void FDECL(mwepgone, (struct monst *));
E int FDECL(mon_wield_item, (struct monst *, BOOLEAN_P, XCHAR_P, XCHAR_P));
E int NDECL(u_strdex_to_hit_bonus);
E int NDECL(u_ranged_strdex_to_hit_bonus);
E int NDECL(u_str_dmg_bonus);
E int NDECL(u_thrown_str_dmg_bonus);
E int FDECL(m_strdex_to_hit_bonus, (struct monst*));
E int FDECL(m_ranged_strdex_to_hit_bonus, (struct monst*));
E int FDECL(m_str_dmg_bonus, (struct monst*));
E int FDECL(m_thrown_str_dmg_bonus, (struct monst*));
E int FDECL(strength_damage_bonus, (int));
E int FDECL(strength_tohit_bonus, (int));
E int FDECL(dexterity_ac_bonus, (int));
E int FDECL(dexterity_tohit_bonus, (int));
E int FDECL(dexterity_ranged_tohit_bonus, (int));
E double FDECL(constitution_hp_bonus, (int));
E void FDECL(wet_a_towel, (struct obj *, int, BOOLEAN_P));
E void FDECL(dry_a_towel, (struct obj *, int, BOOLEAN_P));
E const char* FDECL(skill_level_name_core, (int));
E char *FDECL(skill_level_name, (int, char *, UCHAR_P));
E const char *FDECL(skill_name, (int, BOOLEAN_P));
E boolean FDECL(can_advance, (int, BOOLEAN_P));
E void NDECL(update_can_advance_any_skill);
E int NDECL(doskill);
E int NDECL(doskill_core);
E int NDECL(enhance_weapon_skill);
E void FDECL(unrestrict_weapon_skill, (enum p_skills));
E void FDECL(add_weapon_skill_maximum_by_one, (enum p_skills));
E void FDECL(use_skill, (int, int));
E void FDECL(add_weapon_skill, (int));
E void FDECL(lose_weapon_skill, (int));
E enum p_skills FDECL(weapon_skill_type, (struct obj *));
E enum p_skills NDECL(uwep_skill_type);
E int FDECL(weapon_skill_hit_bonus, (struct obj *, int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, int, BOOLEAN_P));
E int FDECL(weapon_skill_dmg_bonus, (struct obj *, int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, int, BOOLEAN_P));
E int FDECL(shield_skill_ac_bonus, (int));
E int FDECL(shield_skill_mc_bonus, (int));
E int FDECL(dodge_skill_ac_bonus, (int));
E int FDECL(wand_skill_hit_bonus, (int));
E int FDECL(digging_skill_speed_bonus, (int));
E int FDECL(whip_skill_weapon_disarm_bonus, (int));
E int FDECL(whip_skill_weapon_disarm_bonus, (int));
E int FDECL(spear_skill_jousting_bonus, (int));
E int FDECL(riding_skill_jousting_bonus, (int));
E int FDECL(riding_skill_dmg_bonus, (int));
E int FDECL(riding_skill_hit_bonus, (int));
E int FDECL(riding_skill_saddling_bonus, (int));
E int FDECL(riding_skill_mount_bonus, (int));
E void FDECL(skill_init, (const struct def_skill*, const struct def_skill *));
E int FDECL(martial_arts_multishot_percentage_chance, (int));
E int FDECL(two_handed_weapon_multishot_percentage_chance, (int));
E int FDECL(get_skill_critical_strike_chance, (enum p_skills, BOOLEAN_P, BOOLEAN_P, int, BOOLEAN_P));
E double FDECL(get_wand_damage_multiplier, (int));
E int FDECL(get_exceptionality_multiplier, (UCHAR_P));
E void FDECL(print_weapon_style_string, (char*, BOOLEAN_P));
E int FDECL(limited_skill_level, (enum p_skills, BOOLEAN_P, BOOLEAN_P));
E int FDECL(adjusted_limited_skill_level, (enum p_skills, BOOLEAN_P, BOOLEAN_P));
E int FDECL(adjusted_skill_level, (enum p_skills));
E int FDECL(exceptionality_digging_speed_bonus, (struct obj*));
E int FDECL(exceptionality_weapon_disarm_bonus, (struct obj*));
E short FDECL(get_obj_wsdice, (struct obj*));
E short FDECL(get_obj_wsdam, (struct obj*));
E short FDECL(get_obj_wsdmgplus, (struct obj*));
E short FDECL(get_obj_wldice, (struct obj*));
E short FDECL(get_obj_wldam, (struct obj*));
E short FDECL(get_obj_wldmgplus, (struct obj*));
E void NDECL(dump_skills);


/* ### were.c ### */

E void FDECL(were_change, (struct monst *));
E int FDECL(counter_were, (int));
E int FDECL(were_beastie, (int));
E void FDECL(new_were, (struct monst *));
E int FDECL(were_summon, (struct permonst *, BOOLEAN_P, int *, char *));
E void NDECL(you_were);
E void FDECL(you_unwere, (BOOLEAN_P));
E void FDECL(set_ulycn, (int));

/* ### wield.c ### */

E void FDECL(setuwep, (struct obj *, long));
E void FDECL(setuwepcore, (struct obj*, long, BOOLEAN_P));
E void FDECL(setuwepquietly, (struct obj*, long));
E void FDECL(setuqwep, (struct obj *));
E void FDECL(setuswapwep, (struct obj *, long));
E void FDECL(setuqwepquietly, (struct obj*));
E void FDECL(setuswapwepquietly, (struct obj*, long));
E int NDECL(dowield);
E int NDECL(dounwield);
E int NDECL(doswaphandedness);
E int NDECL(doswapweapon_right_or_both);
E int NDECL(doswapweapon);
E int FDECL(dosingleswapweapon, (long, long));
E int NDECL(dowieldquiver);
E boolean FDECL(wield_tool, (struct obj *, const char *));
E void NDECL(drop_uswapwep);
E int NDECL(dotwoweapon);
E void NDECL(uwepgone);
E void NDECL(uwep2gone);
E void NDECL(uswapwepgone);
E void NDECL(uswapwep2gone);
E void NDECL(uqwepgone);
E void NDECL(untwoweapon);
E int FDECL(enchant_weapon, (struct obj *, struct obj*, int, BOOLEAN_P));
E int FDECL(welded, (struct obj *, struct monst*));
E void FDECL(weldmsg, (struct obj *));
E void FDECL(setmnotwielded, (struct monst *, struct obj *));
E boolean FDECL(mwelded, (struct obj *, struct monst*));
E void NDECL(update_unweapon);
E void FDECL(update_hand_unweapon, (int));

/* ### windows.c ### */

E void FDECL(choose_windows, (const char *));
#ifdef WINCHAIN
void FDECL(addto_windowchain, (const char *s));
void NDECL(commit_windowchain);
#endif
E boolean NDECL(genl_can_suspend_no);
E boolean NDECL(genl_can_suspend_yes);
E char FDECL(genl_message_menu, (CHAR_P, int, const char *));
E void FDECL(genl_preference_update, (const char *));
E char *FDECL(genl_getmsghistory_ex, (char**, char**, BOOLEAN_P));
E void FDECL(genl_putmsghistory_ex, (const char *, const char*, const char*, BOOLEAN_P));
#ifdef HANGUPHANDLING
E void NDECL(nhwindows_hangup);
#endif
E void FDECL(genl_status_init, (int));
E void NDECL(genl_status_finish);
E void FDECL(genl_status_enablefield,
             (int, const char *, const char *, BOOLEAN_P));
E void FDECL(genl_status_update, (int, genericptr_t, int, int, int, unsigned long *));

E void NDECL(genl_stretch_window);
E void FDECL(genl_set_animation_timer_interval, (unsigned int));
E int FDECL(genl_open_special_view, (struct special_view_info));
E void FDECL(genl_stop_all_sounds, (struct stop_all_info));
E void FDECL(genl_play_ghsound_occupation_ambient, (struct ghsound_occupation_ambient_info));
E void FDECL(genl_play_ghsound_effect_ambient, (struct ghsound_effect_ambient_info));
E void FDECL(genl_set_effect_ambient_volume, (struct effect_ambient_volume_info));
E void FDECL(genl_play_ghsound_level_ambient, (struct ghsound_level_ambient_info));
E void FDECL(genl_play_ghsound_environment_ambient, (struct ghsound_environment_ambient_info));

E void FDECL(genl_play_ghsound_music, (struct ghsound_music_info));
E void FDECL(genl_play_immediate_ghsound, (struct ghsound_immediate_info));
E void NDECL(genl_adjust_ghsound_general_volumes);
E void FDECL(genl_add_ambient_ghsound, (struct soundsource_t*));
E void FDECL(genl_delete_ambient_ghsound, (struct soundsource_t*));
E void FDECL(genl_set_ambient_ghsound_volume, (struct soundsource_t*));
E void NDECL(genl_clear_context_menu);
E void FDECL(genl_add_context_menu, (int, int, int, int, const char*, const char*, int, int));
E void FDECL(genl_update_status_button, (int, int, int, unsigned long));
E void FDECL(genl_toggle_animation_timer, (int, int, int, int, int, int, unsigned long));
E void FDECL(genl_display_floating_text, (int, int, const char*, int, int, int, unsigned long));
E void FDECL(genl_display_screen_text, (const char*, const char*, const char*, int, int, int, unsigned long));
E void FDECL(genl_display_popup_text, (const char*, const char*, int, int, int, int, unsigned long));
E void FDECL(genl_display_gui_effect, (int, int, int, unsigned long));
E void FDECL(genl_update_cursor, (int, int, int));
E int NDECL(genl_ui_has_input);
E void FDECL(genl_exit_hack, (int));

E void FDECL(dump_open_log, (time_t));
E void NDECL(dump_close_log);
E void FDECL(dump_forward_putstr, (winid, int, const char*, int));
#if defined (DUMPLOG) || defined (DUMPHTML)
E void FDECL(dump_redirect, (BOOLEAN_P));
E void NDECL(dump_start_screendump); /* defined in windows.c */
E void NDECL(dump_end_screendump);
E char* FDECL(dump_fmtstr, (const char*, char*));
E void FDECL(dump_putstr_no_utf8, (winid, int, const char*));
#ifdef DUMPLOG
E char* FDECL(print_dumplog_filename_to_buffer, (char*));
#endif
#ifdef DUMPHTML
E char* FDECL(print_dumphtml_filename_to_buffer, (char*));
E void FDECL(html_dump_glyph, (int, int, int, nhsym, int, unsigned long));
#endif
#endif

#if defined(ANDROID) && defined(DUMPLOG)
E void FDECL(and_get_dumplog_dir, (char*));
#endif

/* ### winnt.c ### */
#ifdef WIN32
E void NDECL(GnollHack_enter_winnt);
#endif

/* ### wizard.c ### */

E void NDECL(amulet);
E int FDECL(mon_has_amulet, (struct monst *));
E int FDECL(mon_has_special, (struct monst *));
E int FDECL(tactics, (struct monst *));
E boolean FDECL(has_aggravatables, (struct monst *));
E void NDECL(aggravate);
E void NDECL(clonewiz);
E int FDECL(pick_nasty, (int));
E int FDECL(summon_nasties, (struct monst *));
E int FDECL(summon_level_appropriate_monsters, (struct monst*));
E void NDECL(resurrect);
E void NDECL(intervene);
E void NDECL(wizdead);
E boolean FDECL(cuss, (struct monst *));

/* ### worm.c ### */

E int NDECL(get_wormno);
E void FDECL(initworm, (struct monst *, int));
E void FDECL(worm_move, (struct monst *));
E void FDECL(worm_nomove, (struct monst *));
E void FDECL(wormgone, (struct monst *));
E void FDECL(wormhitu, (struct monst *));
E void FDECL(cutworm, (struct monst *, XCHAR_P, XCHAR_P, BOOLEAN_P));
E void FDECL(see_wsegs, (struct monst *));
E void FDECL(detect_wsegs, (struct monst *, BOOLEAN_P));
E void FDECL(save_worm, (int, int));
E void NDECL(reset_worm);
E void FDECL(rest_worm, (int));
E void FDECL(place_wsegs, (struct monst *, struct monst *));
E void FDECL(sanity_check_worm, (struct monst *));
E void FDECL(remove_worm, (struct monst *));
E void FDECL(place_worm_tail_randomly, (struct monst *, XCHAR_P, XCHAR_P));
E int FDECL(size_wseg, (struct monst *));
E int FDECL(count_wsegs, (struct monst *));
E boolean FDECL(worm_known, (struct monst *));
E boolean FDECL(worm_cross, (int, int, int, int));
E int FDECL(wseg_at, (struct monst *, int, int));
E int FDECL(get_wseg_dir_at, (struct monst*, int, int));
E int FDECL(get_reverse_prev_wseg_dir_at, (struct monst*, int, int));
E boolean FDECL(is_wseg_head, (struct monst*, int, int));
E boolean FDECL(is_wseg_tailend, (struct monst*, int, int));


/* ### worn.c ### */

E void FDECL(setworn, (struct obj *, long));
E void FDECL(setnotworn, (struct obj *));
E void FDECL(setworncore, (struct obj*, long, BOOLEAN_P));
E void FDECL(setwornquietly, (struct obj*, long));
E void FDECL(setnotworncore, (struct obj*, BOOLEAN_P));
E void FDECL(setnotwornquietly, (struct obj*));
E struct obj *FDECL(wearmask_to_obj, (long));
E long FDECL(wearslot, (struct obj *));
//E void FDECL(mon_adjust_speed, (struct monst *, int, struct obj *));
E void FDECL(update_mon_extrinsics, (struct monst*, BOOLEAN_P));
E void FDECL(update_all_mon_statistics, (struct monst *, BOOLEAN_P));
E void FDECL(update_all_mon_statistics_core, (struct monst*, BOOLEAN_P));
E int FDECL(find_mac, (struct monst *));
E void FDECL(m_dowear, (struct monst *, BOOLEAN_P));
E struct obj *FDECL(which_armor, (struct monst *, long));
E void FDECL(mon_break_armor, (struct monst *, BOOLEAN_P));
E void FDECL(bypass_obj, (struct obj *));
E void NDECL(clear_bypasses);
E void FDECL(bypass_objlist, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(nxt_unbypassed_obj, (struct obj *));
E struct obj *FDECL(nxt_unbypassed_loot, (Loot *, struct obj *));
E int FDECL(racial_exception, (struct monst *, struct obj *));
E void FDECL(nonadditive_increase_mon_property, (struct monst*, int, int));
E boolean FDECL(nonadditive_increase_mon_property_verbosely, (struct monst*, int, int));
E void FDECL(increase_mon_property, (struct monst*, int, int));
E int FDECL(get_mon_property, (struct monst*, int));
E void FDECL(set_mon_property, (struct monst*, int, int));
E boolean FDECL(set_mon_property_verbosely, (struct monst*, int, int));
E boolean FDECL(verbose_wrapper, (enum verbose_function_types, struct monst*, int, int, BOOLEAN_P));
E boolean FDECL(increase_mon_property_verbosely, (struct monst*, int, int));
E boolean FDECL(increase_mon_property_b, (struct monst*, int, int, BOOLEAN_P));
E boolean FDECL(nonadditive_increase_mon_property_b, (struct monst*, int, int, BOOLEAN_P));
E boolean FDECL(set_mon_property_b, (struct monst*, int, int, BOOLEAN_P));
E int FDECL(count_unworn_items, (struct obj*));


/* ### write.c ### */

E int FDECL(dowrite, (struct obj *));
E int FDECL(ink_cost, (struct obj*));

/* ### zap.c ### */

E void FDECL(learnwand, (struct obj *));
E int FDECL(get_saving_throw_adjustment, (struct obj*, struct monst*, struct monst*));
E int FDECL(get_spell_skill_level_saving_throw_adjustment, (int));
E int FDECL(get_wand_skill_level_saving_throw_adjustment, (int));
E int FDECL(bhitm, (struct monst*, struct obj*, struct monst*));
E void FDECL(probe_monster, (struct monst *));
E int FDECL(probe_object, (struct obj*));
E void FDECL(display_monster_information, (struct monst*));
E void FDECL(print_monster_intrinsics, (winid win, struct monst*));
E void FDECL(print_monster_status, (winid win, struct monst*));
E void FDECL(print_monster_statistics, (winid win, struct monst*));
E void FDECL(display_monster_inventory, (struct monst*, BOOLEAN_P));
E boolean FDECL(get_obj_location, (struct obj *, xchar *, xchar *, int));
E boolean FDECL(get_mon_location, (struct monst *, xchar *, xchar *, int));
E boolean FDECL(get_region_location, (struct nhregion*, xchar*, xchar*, int));
E struct monst *FDECL(get_container_location, (struct obj * obj, int *, int *));
E struct monst *FDECL(montraits, (struct obj *, coord *, BOOLEAN_P, int, int, unsigned long));
E struct monst *FDECL(revive, (struct obj *, BOOLEAN_P, int, BOOLEAN_P));
E int FDECL(revive_from_inventory, (struct monst *));
E void FDECL(cancel_item, (struct obj *, BOOLEAN_P));
E boolean FDECL(drain_item, (struct obj *, BOOLEAN_P));
E struct obj *FDECL(poly_obj, (struct obj *, int));
E boolean FDECL(obj_resists, (struct obj *, int, int));
E boolean FDECL(obj_shudders, (struct obj *));
E void FDECL(do_osshock, (struct obj *));
E int FDECL(bhito, (struct obj *, struct obj *, struct monst*));
E int FDECL(bhitpile, (struct obj *, struct monst*, int (*)(OBJ_P, OBJ_P, MONST_P), int, int, SCHAR_P, UCHAR_P, BOOLEAN_P));
E int FDECL(zappable, (struct obj *));
E void FDECL(zapnodir, (struct obj *));
E int NDECL(dozap);
E double FDECL(zapyourself, (struct obj *, BOOLEAN_P));
E void FDECL(ubreatheu, (struct attack *));
E double FDECL(lightdamage, (struct obj *, BOOLEAN_P, int));
E boolean FDECL(flashburn, (long));
E boolean FDECL(cancel_monst, (struct monst *, struct obj *, BOOLEAN_P,
                               BOOLEAN_P, BOOLEAN_P, int));
E boolean FDECL(add_temporary_property, (struct monst*, struct obj*, BOOLEAN_P,
    BOOLEAN_P, BOOLEAN_P, int));
E void NDECL(zapsetup);
E void NDECL(zapwrapup);
E void FDECL(weffects, (struct obj *));
E const char *FDECL(exclam, (int force));
E void FDECL(hit, (const char *, struct monst *, const char *, int, const char*));
E void FDECL(hit_with_hit_tile, (const char*, struct monst*, const char*, int, const char*, enum hit_tile_types, BOOLEAN_P));
E void FDECL(miss, (const char *, struct monst *));
E struct monst *FDECL(bhit, (int, int, int, int, enum bhit_call_types,
                             int (*)(MONST_P, OBJ_P, MONST_P),
                             int (*)(OBJ_P, OBJ_P, MONST_P), struct obj **, struct monst*, UCHAR_P, BOOLEAN_P));
E struct monst *FDECL(boomhit, (struct obj *, int, int));
E double FDECL(zhitm, (struct monst *, int, struct obj*, struct monst*, int, int, int, struct obj **, uchar*));
E int FDECL(burn_floor_objects, (int, int, BOOLEAN_P, BOOLEAN_P));
E void FDECL(buzz, (int, struct obj*, struct monst*, int, int, int, XCHAR_P, XCHAR_P, int, int));
E void FDECL(dobuzz, (int, struct obj*, struct monst*, int, int, int, XCHAR_P, XCHAR_P, int, int, BOOLEAN_P));
E void FDECL(melt_ice, (XCHAR_P, XCHAR_P, const char *));
E void FDECL(start_melt_ice_timeout, (XCHAR_P, XCHAR_P, long));
E void FDECL(melt_ice_away, (ANY_P *, long));
E int FDECL(zap_over_floor, (XCHAR_P, XCHAR_P, int, boolean *, SHORT_P));
E void FDECL(fracture_rock, (struct obj *, BOOLEAN_P));
E boolean FDECL(pre_break_statue, (struct obj*));
E boolean FDECL(break_statue, (struct obj *));
E void FDECL(destroy_item, (int, int));
E int FDECL(destroy_mitem, (struct monst *, int, int));
E void FDECL(destroy_one_item, (struct obj*, int, int, BOOLEAN_P));
E boolean FDECL(is_obj_protected_by_property, (struct obj*, struct monst*, int));
E boolean FDECL(check_magic_resistance_and_inflict_damage, (struct monst *, struct obj *, struct monst*, BOOLEAN_P, int, int, int));
E boolean FDECL(inflict_spell_damage, (struct monst*, struct obj*, struct monst*, int, int, int));
E void FDECL(makewish, (BOOLEAN_P, BOOLEAN_P));
E void FDECL(summonitem, (struct obj*, int));
E void FDECL(summonblackblade, (struct obj*));
E void FDECL(summonmagearmor, (struct obj*));
E struct monst* FDECL(summoncreature, (int, int, const char*, unsigned long, unsigned long)); // int, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P, BOOLEAN_P));
E void FDECL(summondemon, (int));
E void FDECL(summondemogorgon, (int));
E void FDECL(summonbahamut, (int));
E void NDECL(armageddon);
E void FDECL(timestop, (int));
E boolean FDECL(check_rider_disintegration, (struct monst*, const char*));
E boolean FDECL(check_rider_death_absorption, (struct monst*, const char*));
E void FDECL(maybe_disintegrate_mon, (struct monst*, int, const char*));
E void FDECL(disintegrate_mon, (struct monst*, int, const char*));
E boolean FDECL(item_prevents_revival, (int));
E boolean FDECL(item_prevents_summoning, (int));
E int FDECL(get_displayed_object_type_from_subdir_type, (int));
E int FDECL(stone_to_flesh_obj, (struct obj*));
E int FDECL(mon_to_zombie, (int));
E int FDECL(mon_to_mummy, (int));
E int FDECL(get_spell_damage, (int, struct monst*, struct monst*));
E int FDECL(get_spell_skill_level, (int, struct monst*, struct monst*));
E int FDECL(get_obj_spell_duration, (struct obj*));
E int FDECL(get_otyp_spell_duration, (int));
E int FDECL(get_maximum_applicable_spell_damage_level, (int, struct monst*));
E boolean FDECL(is_buzztype_breath_weapon, (int));
E boolean FDECL(is_buzztype_eyestalk, (int));
E void NDECL(reset_zap);

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
