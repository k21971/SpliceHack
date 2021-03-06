/* NetHack 3.7	fountain.c	$NHDT-Date: 1596498170 2020/08/03 23:42:50 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.69 $ */
/*      Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"
#include "artifact.h"
#include "artilist.h"

static void dowatersnakes(void);
static void dowaterdemon(void);
static void dowaternymph(void);
static void gush(int, int, genericptr_t);
static void dofindgem(void);
static void blowupfurnace(int, int);

/* used when trying to dip in or drink from fountain or sink or pool while
   levitating above it, or when trying to move downwards in that state */
void
floating_above(const char *what)
{
    const char *umsg = "are floating high above the %s.";

    if (u.utrap && (u.utraptype == TT_INFLOOR || u.utraptype == TT_LAVA)) {
        /* when stuck in floor (not possible at fountain or sink location,
           so must be attempting to move down), override the usual message */
        umsg = "are trapped in the %s.";
        what = surface(u.ux, u.uy); /* probably redundant */
    }
    You(umsg, what);
}

/* Fountain of snakes! */
static void
dowatersnakes(void)
{
    register int num = rn1(5, 2);
    struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
        if (!Blind)
            pline("An endless stream of %s pours forth!",
                  Hallucination ? makeplural(rndmonnam(NULL)) : "snakes");
        else
            You_hear("%s hissing!", something);
        while (num-- > 0)
            if ((mtmp = makemon(&mons[PM_WATER_MOCCASIN], u.ux, u.uy,
                                NO_MM_FLAGS)) != 0
                && t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water demon */
static void
dowaterdemon(void)
{
    struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_DEMON].mvflags & G_GONE)) {
        if ((mtmp = makemon(&mons[PM_WATER_DEMON], u.ux, u.uy,
                            NO_MM_FLAGS)) != 0) {
            if (!Blind)
                You("unleash %s!", a_monnam(mtmp));
            else
                You_feel("the presence of evil.");

            /* Give those on low levels a (slightly) better chance of survival
             */
            if (rnd(100) > (80 + level_difficulty())) {
                pline("Grateful for %s release, %s grants you a wish!",
                      mhis(mtmp), mhe(mtmp));
                /* give a wish and discard the monster (mtmp set to null) */
                mongrantswish(&mtmp);
            } else if (t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
        }
    } else
        pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water Nymph */
static void
dowaternymph(void)
{
    register struct monst *mtmp;

    if (!(g.mvitals[PM_WATER_NYMPH].mvflags & G_GONE)
        && (mtmp = makemon(&mons[PM_WATER_NYMPH], u.ux, u.uy,
                           NO_MM_FLAGS)) != 0) {
        if (!Blind)
            You("attract %s!", a_monnam(mtmp));
        else
            You_hear("a seductive voice.");
        mtmp->msleeping = 0;
        if (t_at(mtmp->mx, mtmp->my))
            (void) mintrap(mtmp);
    } else if (!Blind)
        pline("A large bubble rises to the surface and pops.");
    else
        You_hear("a loud pop.");
}

/* Gushing forth along LOS from (u.ux, u.uy) */
void
dogushforth(int drinking)
{
    int madepool = 0;

    do_clear_area(u.ux, u.uy, 7, gush, (genericptr_t) &madepool);
    if (!madepool) {
        if (drinking)
            Your("thirst is quenched.");
        else
            pline("Water sprays all over you.");
    }
}

static void
gush(int x, int y, genericptr_t poolcnt)
{
    register struct monst *mtmp;
    register struct trap *ttmp;

    if (((x + y) % 2) || (x == u.ux && y == u.uy)
        || (rn2(1 + distmin(u.ux, u.uy, x, y))) || (levl[x][y].typ != ROOM)
        || (sobj_at(BOULDER, x, y)) || nexttodoor(x, y))
        return;

    if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
        return;

    if (!((*(int *) poolcnt)++))
        pline("Water gushes forth from the overflowing fountain!");

    /* Put a pool at x, y */
    levl[x][y].typ = POOL, levl[x][y].flags = 0;
    /* No kelp! */
    del_engr_at(x, y);
    water_damage_chain(g.level.objects[x][y], TRUE);

    if ((mtmp = m_at(x, y)) != 0)
        (void) minliquid(mtmp);
    else
        newsym(x, y);
}

/* Find a gem in the sparkling waters. */
static void
dofindgem(void)
{
    if (!Blind)
        You("spot a gem in the sparkling waters!");
    else
        You_feel("a gem here!");
    (void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE - 1), u.ux, u.uy,
                     FALSE, FALSE);
    SET_FOUNTAIN_LOOTED(u.ux, u.uy);
    newsym(u.ux, u.uy);
    exercise(A_WIS, TRUE); /* a discovery! */
}

void
dryup(xchar x, xchar y, boolean isyou)
{
    if (IS_FOUNTAIN(levl[x][y].typ)
        && (!rn2(3) || FOUNTAIN_IS_WARNED(x, y))) {
        if (isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x, y)) {
            struct monst *mtmp;

            SET_FOUNTAIN_WARNED(x, y);
            /* Warn about future fountain use. */
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                if (is_watch(mtmp->data) && couldsee(mtmp->mx, mtmp->my)
                    && mtmp->mpeaceful) {
                    if (!Deaf) {
                        pline("%s yells:", Amonnam(mtmp));
                        verbalize("Hey, stop using that fountain!");
                    } else {
                        pline("%s earnestly %s %s %s!",
                              Amonnam(mtmp),
                              nolimbs(mtmp->data) ? "shakes" : "waves",
                              mhis(mtmp),
                              nolimbs(mtmp->data)
                                      ? mbodypart(mtmp, HEAD)
                                      : makeplural(mbodypart(mtmp, ARM)));
                    }
                    break;
                }
            }
            /* You can see or hear this effect */
            if (!mtmp)
                pline_The("flow reduces to a trickle.");
            return;
        }
        if (isyou && wizard) {
            if (yn("Dry up fountain?") == 'n')
                return;
        }
        /* FIXME: sight-blocking clouds should use block_point() when
           being created and unblock_point() when going away, then this
           glyph hackery wouldn't be necessary */
        if (cansee(x, y)) {
            int glyph = glyph_at(x, y);

            if (!glyph_is_cmap(glyph) || glyph_to_cmap(glyph) != S_cloud)
                pline_The("fountain dries up!");
        }
        /* replace the fountain with ordinary floor */
        levl[x][y].typ = ROOM, levl[x][y].flags = 0;
        levl[x][y].blessedftn = 0;
        g.level.flags.nfountains--;
        /* The location is seen if the hero/monster is invisible
           or felt if the hero is blind. */
        newsym(x, y);
        if (isyou && in_town(x, y))
            (void) angry_guards(FALSE);
    }
}

void
dipfurnace(obj)
register struct obj *obj;
{
    burn_away_slime();
    switch(rnd(30)) {
        case 1:
        case 2:
        case 3:
            bless(obj);
            break;
        case 4:
            if (!Blind)
                pline("%s flickers with purple light.", Doname2(obj));
            else
                pline("%s shudders slightly in your grip.", Doname2(obj));
            obj->oerodeproof = 1;
            break;
        case 5:
            blowupfurnace(u.ux, u.uy);
            break;
        case 6: /* Strange feeling */
            pline("An unsettling tingling runs up your %s.", body_part(ARM));
            break;
        case 7: /* Strange feeling */
            You_feel("a sudden flare of heat.");
            break;
        default:
            pline("The lava in the furnace bubbles.");
            break;
    }
    lava_damage(obj, u.ux, u.uy);
    update_inventory();
}

void
drinkfountain(void)
{
    /* What happens when you drink from a fountain? */
    register boolean mgkftn = (levl[u.ux][u.uy].blessedftn == 1);
    register int fate = rnd(30);

    if (Levitation) {
        floating_above("fountain");
        return;
    }

    if (mgkftn && u.uluck >= 0 && fate >= 10) {
        int i, ii, littleluck = (u.uluck < 4);

        pline("Wow!  This makes you feel great!");
        /* blessed restore ability */
        for (ii = 0; ii < A_MAX; ii++)
            if (ABASE(ii) < AMAX(ii)) {
                ABASE(ii) = AMAX(ii);
                g.context.botl = 1;
            }
        /* gain ability, blessed if "natural" luck is high */
        i = rn2(A_MAX); /* start at a random attribute */
        for (ii = 0; ii < A_MAX; ii++) {
            if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
                break;
            if (++i >= A_MAX)
                i = 0;
        }
        display_nhwindow(WIN_MESSAGE, FALSE);
        pline("A wisp of vapor escapes the fountain...");
        exercise(A_WIS, TRUE);
        levl[u.ux][u.uy].blessedftn = 0;
        return;
    }

    if (fate < 10) {
        pline_The("cool draught refreshes you.");
        u.uhunger += rnd(10); /* don't choke on water */
        newuhs(FALSE);
        if (mgkftn)
            return;
    } else {
        switch (fate) {
        case 19: /* Self-knowledge */
            You_feel("self-knowledgeable...");
            display_nhwindow(WIN_MESSAGE, FALSE);
            enlightenment(MAGICENLIGHTENMENT, ENL_GAMEINPROGRESS);
            exercise(A_WIS, TRUE);
            pline_The("feeling subsides.");
            break;
        case 20: /* Foul water */
            if (Is_oracle_level(&u.uz)) {
                pline("Oh wow!  Great stuff!");
                (void) make_hallucinated((HHallucination & TIMEOUT) +
                        rnd(100), FALSE, 0L);
            } else {
                pline_The("water is foul!  You gag and vomit.");
                morehungry(rn1(20, 11));
                vomit();
            }
            break;
        case 21: /* Poisonous */
            pline_The("water is contaminated!");
            if (how_resistant(POISON_RES) == 100) {
                pline("Perhaps it is runoff from the nearby %s farm.",
                      fruitname(FALSE));
                losehp(rnd(4), "unrefrigerated sip of juice", KILLED_BY_AN);
                break;
            }
            losestr(resist_reduce(rn1(4, 3), POISON_RES));
            losehp(resist_reduce(rnd(10), POISON_RES), "contaminated water", KILLED_BY);
            exercise(A_CON, FALSE);
            break;
        case 22: /* Fountain of snakes! */
            dowatersnakes();
            break;
        case 23: /* Water demon */
            dowaterdemon();
            break;
        case 24: { /* Maybe curse some items */
            register struct obj *obj;
            int buc_changed = 0;

            pline("This water's no good!");
            morehungry(rn1(20, 11));
            exercise(A_CON, FALSE);
            /* this is more severe than rndcurse() */
            for (obj = g.invent; obj; obj = obj->nobj)
                if (obj->oclass != COIN_CLASS && !obj->cursed && !rn2(5)) {
                    curse(obj);
                    ++buc_changed;
                }
            if (buc_changed)
                update_inventory();
            break;
        }
        case 25: /* See invisible */
            if (Blind) {
                if (Invisible) {
                    You("feel transparent.");
                } else {
                    You("feel very self-conscious.");
                    pline("Then it passes.");
                }
            } else {
                You_see("an image of someone stalking you.");
                pline("But it disappears.");
            }
            HSee_invisible |= FROMOUTSIDE;
            newsym(u.ux, u.uy);
            exercise(A_WIS, TRUE);
            break;
        case 26: /* See Monsters */
            (void) monster_detect((struct obj *) 0, 0);
            exercise(A_WIS, TRUE);
            break;
        case 27: /* Find a gem in the sparkling waters. */
            if (!FOUNTAIN_IS_LOOTED(u.ux, u.uy)) {
                dofindgem();
                break;
            }
            /*FALLTHRU*/
        case 28: /* Water Nymph */
            dowaternymph();
            break;
        case 29: /* Scare */
        {
            register struct monst *mtmp;

            pline("This %s gives you bad breath!",
                  hliquid("water"));
            for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
                if (DEADMONSTER(mtmp))
                    continue;
                monflee(mtmp, 0, FALSE, FALSE);
            }
            break;
        }
        case 30: /* Gushing forth in this room */
            dogushforth(TRUE);
            break;
        default:
            pline("This tepid %s is tasteless.",
                  hliquid("water"));
            break;
        }
    }
    dryup(u.ux, u.uy, TRUE);
}

/* Monty Python and the Holy Grail ;) */
static const char *const excalmsgs[] = {
    "had Excalibur thrown at them by some watery tart",
    "received Excalibur from a strange woman laying about in a pond",
    "signified by divine providence, was chosen to carry Excalibur",
    "has been given Excalibur, and now enjoys supreme executive power",
    "endured an absurd aquatic ceremony, and now wields Excalibur"
};

void
dipfountain(register struct obj *obj)
{
    if (Levitation) {
        floating_above("fountain");
        return;
    }

    /* Don't grant Excalibur when there's more than one object.  */
    /* (quantity could be > 1 if merged daggers got polymorphed) */
    if (obj->otyp == LONG_SWORD && obj->quan == 1L && u.ulevel >= 5 && !rn2(6)
        && !obj->oartifact
        && !exist_artifact(LONG_SWORD, artiname(ART_EXCALIBUR))) {
        if (u.ualign.type != A_LAWFUL) {
            /* Ha!  Trying to cheat her. */
            pline("A freezing mist rises from the %s and envelopes the sword.",
                  hliquid("water"));
            pline_The("fountain disappears!");
            curse(obj);
            if (obj->spe > -6 && !rn2(3))
                obj->spe--;
            obj->oerodeproof = FALSE;
            exercise(A_WIS, FALSE);
            livelog_printf(LL_ARTIFACT, "was denied Excalibur! The Lady of the Lake has deemed %s unworthy", uhim());
        } else {
            /* The lady of the lake acts! - Eric Backus */
            /* Be *REAL* nice */
            pline(
              "From the murky depths, a hand reaches up to bless the sword.");
            pline("As the hand retreats, the fountain disappears!");
            obj = oname(obj, artiname(ART_EXCALIBUR));
            discover_artifact(ART_EXCALIBUR);
            bless(obj);
            obj->oeroded = obj->oeroded2 = 0;
            obj->oerodeproof = TRUE;
            exercise(A_WIS, TRUE);
            livelog_printf(LL_ARTIFACT, "%s", excalmsgs[rn2(SIZE(excalmsgs))]);
        }
        update_inventory();
        levl[u.ux][u.uy].typ = ROOM, levl[u.ux][u.uy].flags = 0;
        newsym(u.ux, u.uy);
        g.level.flags.nfountains--;
        if (in_town(u.ux, u.uy))
            (void) angry_guards(FALSE);
        return;
    } else {
        int er = water_damage(obj, NULL, TRUE);

        if (obj->otyp == POT_ACID
            && er != ER_DESTROYED) { /* Acid and water don't mix */
            useup(obj);
            return;
        } else if (er != ER_NOTHING && !rn2(2)) { /* no further effect */
            return;
        }
    }

    switch (rnd(30)) {
    case 16: /* Curse the item */
        if (obj->oclass != COIN_CLASS && !obj->cursed) {
            curse(obj);
        }
        break;
    case 17:
    case 18:
    case 19:
    case 20: /* Uncurse the item */
        if (obj->cursed) {
            if (!Blind)
                pline_The("%s glows for a moment.", hliquid("water"));
            uncurse(obj);
        } else {
            pline("A feeling of loss comes over you.");
        }
        break;
    case 21: /* Water Demon */
        dowaterdemon();
        break;
    case 22: /* Water Nymph */
        dowaternymph();
        break;
    case 23: /* an Endless Stream of Snakes */
        dowatersnakes();
        break;
    case 24: /* Find a gem */
        if (!FOUNTAIN_IS_LOOTED(u.ux, u.uy)) {
            dofindgem();
            break;
        }
        /*FALLTHRU*/
    case 25: /* Water gushes forth */
        dogushforth(FALSE);
        break;
    case 26: /* Strange feeling */
        pline("A strange tingling runs up your %s.", body_part(ARM));
        break;
    case 27: /* Strange feeling */
        You_feel("a sudden chill.");
        break;
    case 28: /* Strange feeling */
        pline("An urge to take a bath overwhelms you.");
        {
            long money = money_cnt(g.invent);
            struct obj *otmp;
            if (money > 10) {
                /* Amount to lose.  Might get rounded up as fountains don't
                 * pay change... */
                money = somegold(money) / 10;
                for (otmp = g.invent; otmp && money > 0; otmp = otmp->nobj)
                    if (otmp->oclass == COIN_CLASS) {
                        int denomination = objects[otmp->otyp].oc_cost;
                        long coin_loss =
                            (money + denomination - 1) / denomination;
                        coin_loss = min(coin_loss, otmp->quan);
                        otmp->quan -= coin_loss;
                        money -= coin_loss * denomination;
                        if (!otmp->quan)
                            delobj(otmp);
                    }
                You("lost some of your gold in the fountain!");
                CLEAR_FOUNTAIN_LOOTED(u.ux, u.uy);
                exercise(A_WIS, FALSE);
            }
        }
        break;
    case 29: /* You see coins */
        /* We make fountains have more coins the closer you are to the
         * surface.  After all, there will have been more people going
         * by.  Just like a shopping mall!  Chris Woodbury  */

        if (FOUNTAIN_IS_LOOTED(u.ux, u.uy))
            break;
        SET_FOUNTAIN_LOOTED(u.ux, u.uy);
        (void) mkgold((long) (rnd((dunlevs_in_dungeon(&u.uz) - dunlev(&u.uz)
                                   + 1) * 2) + 5),
                      u.ux, u.uy);
        if (!Blind)
            pline("Far below you, you see coins glistening in the %s.",
                  hliquid("water"));
        exercise(A_WIS, TRUE);
        newsym(u.ux, u.uy);
        break;
    }
    update_inventory();
    dryup(u.ux, u.uy, TRUE);
}

void
breakfurnace(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("furnace breaks apart and spills its contents!");
    levl[x][y].typ = LAVAPOOL, levl[x][y].flags = 0;
    newsym(x, y);
    g.level.flags.nfurnaces--;
}

void
blowupfurnace(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("furnace rumbles, then explodes into smithereens!");
    levl[x][y].typ = ROOM, levl[x][y].flags = 0;
    newsym(x, y);
    g.level.flags.nfurnaces--;
    explode(u.ux, u.uy, 11, rnd(30), TOOL_CLASS, EXPL_FIERY);
}

void
drinkfurnace(void)
{
    if (Levitation) {
        floating_above("furnace");
        return;
    }
    if (!likes_lava(g.youmonst.data)) {
        pline("Glug glug glug...");
        /* ugly hack, look away */
        u.uhp = 0;
        losehp(1, "trying to drink lava", KILLED_BY);
        return;
    }
    burn_away_slime();
    switch(rn2(20)) {
    case 0:
        pline("You chug some lava. Yum!");
        u.uhunger += rnd(50);
        break;
    case 1:
        breakfurnace(u.ux, u.uy);
        break;
    case 2:
    case 3:
        pline_The("%s moves as though of its own will!", hliquid("lava"));
        if ((g.mvitals[PM_LAVA_DEMON].mvflags & G_GONE)
            || !makemon(&mons[PM_LAVA_DEMON], u.ux, u.uy, NO_MM_FLAGS))
            pline("But it settles down.");
        break;
    default:
        pline("You take a sip of the lava.");
        u.uhunger += rnd(5);
    }
}

static const short fusions[][3] = {
    /* weapons */
    { BASEBALL_BAT,  CLUB, RUBBER_HOSE },
    { BATTLE_AXE,  AXE, AXE },
    { HEAVY_IRON_BALL,  IRON_CHAIN, BOULDER }, /* arguably non-weapon, but */
    { LONG_SWORD,  SHORT_SWORD, SHORT_SWORD },
    { SPIKED_CHAIN,  IRON_CHAIN, DAGGER },
    { SPIKED_CHAIN,  IRON_CHAIN, KNIFE },
    { TRIPLE_FLAIL,  FLAIL, FLAIL },
    { TSURUGI, TWO_HANDED_SWORD, KATANA },
    /* armor */
    { HELM_OF_TELEPATHY,  AMULET_OF_ESP, HELMET },
    { SHIELD_OF_REFLECTION,  AMULET_OF_REFLECTION, LARGE_SHIELD },
    { TINFOIL_HAT,  DENTED_POT, DUNCE_CAP },
    { LEVITATION_BOOTS, HIGH_BOOTS, RIN_LEVITATION },
    { LEVITATION_BOOTS, LOW_BOOTS, RIN_LEVITATION },
    { SHIELD_OF_REFLECTION, SMALL_SHIELD, AMULET_OF_REFLECTION },
    { CLOAK_OF_PROTECTION, PLAIN_CLOAK, AMULET_OF_GUARDING },
    /* rings */
    { RIN_CONFLICT,  RIN_HUNGER, RIN_AGGRAVATE_MONSTER },
    { RIN_POLYMORPH_CONTROL,  RIN_POLYMORPH, RIN_TELEPORT_CONTROL },
    { RIN_TELEPORT_CONTROL,  RIN_TELEPORTATION, RIN_POLYMORPH_CONTROL },
    /* tools */
    { LENSES,  CRYSTAL_BALL, TOWEL },
    { LOCK_PICK,  SKELETON_KEY, SKELETON_KEY },
    { SKELETON_KEY,  LOCK_PICK, LOCK_PICK },
    /* magical tools */
    { CRYSTAL_BALL,  LOADSTONE, SCR_IDENTIFY },
    { CRYSTAL_BALL,  LUCKSTONE, SCR_IDENTIFY },
    { HORN_OF_PLENTY,  FOOD_RATION, TOOLED_HORN },
    { MAGIC_FLUTE,  FLUTE, SCR_TAMING },
    { OILSKIN_SACK,  CAN_OF_GREASE, BAG_OF_HOLDING },
    { OILSKIN_SACK,  CAN_OF_GREASE, BAG_OF_RATS },
    { FIRE_HORN, TOOLED_HORN, WAN_FIRE },
    { FROST_HORN, TOOLED_HORN, WAN_COLD },
    { OILSKIN_SACK, SACK, CAN_OF_GREASE },
    { DRUM_OF_EARTHQUAKE, LEATHER_DRUM, SCR_EARTH },
    { MAGIC_HARP, HARP, SCR_TAMING },
    { DWARVISH_MATTOCK, PICK_AXE, AXE },
    /* wands */
    { WAN_CREATE_MONSTER,  WAN_POLYMORPH, WAN_NOTHING },
    { WAN_ENLIGHTENMENT,  WAN_OPENING, WAN_LIGHT },
    { WAN_NOTHING,  WAN_OPENING, WAN_LOCKING },
    /* food/gems/junk */
    { K_RATION,  C_RATION, FOOD_RATION },
    { FRUITCAKE, SLICE_OF_CAKE, SLIME_MOLD },
    { PUMPKIN_PIE, CREAM_PIE, PUMPKIN },
    { APPLE_PIE, CREAM_PIE, APPLE },
    /* amulets */
    { AMULET_OF_REINCARNATION, AMULET_OF_LIFE_SAVING, AMULET_OF_CHANGE },
    { AMULET_OF_LIFE_SAVING, AMULET_OF_REINCARNATION, RIN_POLYMORPH_CONTROL },
    { 0, 0, 0, }
    /* scale mail + dragon scales = dsm */
};

static const short artifusions[][3] = {
    { ART_FROSTBURN,  ART_FROST_BRAND,ART_FIRE_BRAND },
    { ART_MORTALITY_DIAL,  ART_TROLLSBANE,ART_WEREBANE },
    { ART_SQUALL,  ART_SUNSPOT,ART_SONICBOOM },
};

int
doforging(void)
{
    struct obj* obj1;
    struct obj* obj2;
    struct obj* output;

    int objtype = 0;
    int arti_id = 0;
    const struct artifact *a;

    int i;

    if (!IS_FURNACE(levl[u.ux][u.uy].typ)) {
        You("need a forge in order to forge items.");
        return 0;
    }

    obj1 = getobj("use as a base", any_obj_ok, GETOBJ_PROMPT);
    if (!obj1) {
        You("need a base object to forge with.");
        return 0;
    }

    obj2 = getobj("combine with the base item", any_obj_ok, GETOBJ_PROMPT);
    if (!obj2) {
        You("need more than one object.");
        return 0;
    }

    if (obj1 == obj2) {
        You_cant("combine an item with itself!");
        return 0;
    }

    if (is_worn(obj1) || is_worn(obj2)) {
        You("must remove the items you wish to forge.");
        return 0;
    }

    /* No, you cannot imbue weapons with cockatrice or Rider essence. Sorry.
       Literally anything else is fair game though! */
    feel_cockatrice(obj1, TRUE);
    feel_cockatrice(obj2, TRUE);

    if (rider_corpse_revival(obj1, FALSE))
        return 1;
    if (rider_corpse_revival(obj2, FALSE))
        return 1;

    /* Artifacts can never be applied to a non-arficat base. */
    if (obj2->oartifact && !obj1->oartifact) {
        pline_The("artifact seems to resist the attempt!");
        return 1;
    }

    /* Artifact fusions. */
    if (obj1->oartifact && obj2->oartifact) {
        for (i = 0; fusions[i][0] > 0; i++) {
            if ((obj1->oartifact == artifusions[i][1] && obj2->oartifact == artifusions[i][2]) ||
                (obj2->oartifact == artifusions[i][1] && obj1->oartifact == artifusions[i][2])) {
                arti_id = artifusions[i][0];
                a = &artilist[arti_id];
                break;
            }
        }
        if (arti_id) {
            if (arti_id == ART_FROSTBURN) objtype = obj1->otyp;
            else objtype = (int)a->otyp;
            output = mksobj(objtype, TRUE, FALSE);
            output = oname(output, a->name);
            output->oartifact = arti_id;
            g.artiexist[arti_id] = TRUE;

            You("create %s!", an(xname(output)));
            livelog_printf(LL_DIVINEGIFT, "used a forge to create %s", an(xname(obj1)));

            goto fixup_object;
        } else {
            pline_The("artifacts refuse to be combined!");
            return 1;
        }
    }
    /* Imbuing items */
    else if ((obj2->otyp == CORPSE || obj2->otyp == TIN || obj2->otyp == FIGURINE || obj2->otyp == MASK) 
        && obj2->corpsenm != NON_PM) {

        You("imbue %s with the %s essence.", yobjnam(obj1, (char *) 0), mons[obj2->corpsenm].pmnames[NEUTRAL]);
        obj1->corpsenm = obj2->corpsenm;
        useup(obj2);
        update_inventory();

        goto furnace_dead;
    }
    /* Mundane item fusions */
    else {
        for (i = 0; fusions[i][0] > 0; i++) {
            if ((obj1->otyp == fusions[i][1] && obj2->otyp == fusions[i][2]) ||
                (obj2->otyp == fusions[i][1] && obj1->otyp == fusions[i][2])) {
                objtype = fusions[i][0];
                break;
            }
        }
        if (objtype){
            output = mksobj(objtype, TRUE, FALSE);

            /* Take on the secondary object's material. */
            if (valid_obj_material(output, obj2->material)) output->material = obj2->material;
            else if (valid_obj_material(output, obj1->material)) output->material = obj1->material;

            You("combine the items in the furnace.");

            goto fixup_object;
        } else {
            You("fail to combine those two items.");
            return 1;
        }
    }

fixup_object:
    /* Copy enchantment over when applicable.
     * IMPORTANT: Please make sure that any combinations of things that use ->spe are either matching 
     * in object class with the output or just in general won't result in something stupid, like a magic
     * marker ending up with a +3 charges from a magic flute or a crystal ball with 45 charges from a magic 
     * marker. This should be done by ensuring that fusions[] doesn't have stuff like that, and if it does,
     * it's special-cased. As-is, will occasionally result in stuff like a wooden flute with 5 charges,
     * but that should be OK.
    */

    if (output->oclass == obj2->oclass) {
        output->spe = obj2->spe;

        if (output->oclass == obj1->oclass)
            output->spe = max(output->spe, obj1->spe);

    } else if (output->oclass == obj1->oclass){
        output->spe = obj1->spe;
    }

    /* Transfer curses and blessings */
    output->cursed = obj2->cursed;
    output->blessed = obj2->blessed;

    /* Keep corpsenm if set */
    if (obj2->corpsenm != -1) output->corpsenm = obj2->corpsenm;
    else if (obj1->corpsenm != -1) output->corpsenm = obj1->corpsenm;

    /* Toss out old objs, add new one */
    useup(obj1);
    useup(obj2);
    output = addinv(output);
    update_inventory();
    return 1; // forging artifacts or items never poofs the furnace

furnace_dead:
    /* Destroy the furnace. */
    if (!rn2(2)) {
        pline_The("lava in the furnace cools.");
        levl[u.ux][u.uy].typ = ROOM;
        newsym(u.ux, u.uy);
        g.level.flags.nfurnaces--;
    } else {
        pline_The("lava in the furnace bubbles ominously.");
    }
    return 1;
}

void
breaksink(int x, int y)
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("pipes break!  Water spurts out!");
    g.level.flags.nsinks--;
    levl[x][y].typ = FOUNTAIN, levl[x][y].looted = 0;
    levl[x][y].blessedftn = 0;
    SET_FOUNTAIN_LOOTED(x, y);
    g.level.flags.nfountains++;
    newsym(x, y);
}

void
drinksink(void)
{
    struct obj *otmp;
    struct monst *mtmp;

    if (Levitation) {
        floating_above("sink");
        return;
    }
    switch (rn2(20)) {
    case 0:
        You("take a sip of very cold %s.", hliquid("water"));
        break;
    case 1:
        You("take a sip of very warm %s.", hliquid("water"));
        break;
    case 2:
        You("take a sip of scalding hot %s.", hliquid("water"));
        if (how_resistant(FIRE_RES) == 100) {
            pline("It seems quite tasty.");
            monstseesu(M_SEEN_FIRE);
        } else
            losehp(resist_reduce(rnd(6), FIRE_RES), "sipping boiling water", KILLED_BY);
        /* boiling water burns considered fire damage */
        break;
    case 3:
        if (g.mvitals[PM_SEWER_RAT].mvflags & G_GONE)
            pline_The("sink seems quite dirty.");
        else {
            mtmp = makemon(&mons[PM_SEWER_RAT], u.ux, u.uy, NO_MM_FLAGS);
            if (mtmp)
                pline("Eek!  There's %s in the sink!",
                      (Blind || !canspotmon(mtmp)) ? "something squirmy"
                                                   : a_monnam(mtmp));
        }
        break;
    case 4:
        do {
            otmp = mkobj(POTION_CLASS, FALSE);
            if (otmp->otyp == POT_WATER) {
                obfree(otmp, (struct obj *) 0);
                otmp = (struct obj *) 0;
            }
        } while (!otmp);
        otmp->cursed = otmp->blessed = 0;
        pline("Some %s liquid flows from the faucet.",
              Blind ? "odd" : hcolor(OBJ_DESCR(objects[otmp->otyp])));
        otmp->dknown = !(Blind || Hallucination);
        otmp->quan++;       /* Avoid panic upon useup() */
        otmp->fromsink = 1; /* kludge for docall() */
        (void) dopotion(otmp);
        obfree(otmp, (struct obj *) 0);
        break;
    case 5:
        if (!(levl[u.ux][u.uy].looted & S_LRING)) {
            You("find a ring in the sink!");
            (void) mkobj_at(RING_CLASS, u.ux, u.uy, TRUE);
            levl[u.ux][u.uy].looted |= S_LRING;
            exercise(A_WIS, TRUE);
            newsym(u.ux, u.uy);
        } else
            pline("Some dirty %s backs up in the drain.", hliquid("water"));
        break;
    case 6:
        breaksink(u.ux, u.uy);
        break;
    case 7:
        pline_The("%s moves as though of its own will!", hliquid("water"));
        if ((g.mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE)
            || !makemon(&mons[PM_WATER_ELEMENTAL], u.ux, u.uy, NO_MM_FLAGS))
            pline("But it quiets down.");
        break;
    case 8:
        pline("Yuk, this %s tastes awful.", hliquid("water"));
        more_experienced(1, 0);
        newexplevel();
        break;
    case 9:
        pline("Gaggg... this tastes like sewage!  You vomit.");
        morehungry(rn1(30 - ACURR(A_CON), 11));
        vomit();
        break;
    case 10:
        pline("This %s contains toxic wastes!", hliquid("water"));
        if (!Unchanging) {
            You("undergo a freakish metamorphosis!");
            polyself(0);
        }
        break;
    /* more odd messages --JJB */
    case 11:
        You_hear("clanking from the pipes...");
        break;
    case 12:
        You_hear("snatches of song from among the sewers...");
        break;
    case 19:
        if (Hallucination) {
            pline("From the murky drain, a hand reaches up... --oops--");
            break;
        }
        /*FALLTHRU*/
    default:
        You("take a sip of %s %s.",
            rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot",
            hliquid("water"));
    }
}

/*fountain.c*/
