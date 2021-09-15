-- NetHack 3.7	castle.des	$NHDT-Date: 1432512783 2015/05/25 00:13:03 $  $NHDT-Branch: master $:$NHDT-Revision: 1.11 $
--	Copyright (c) 1989 by Jean-Christophe Collet
-- NetHack may be freely redistributed.  See license for details.
--
--
-- This is the stronghold level :
-- there are several ways to enter it :
--	- opening the drawbridge (wand of opening, knock spell, playing
--	  the appropriate tune)
--
--	- enter via the back entry (this suppose a ring of levitation, boots
--	  of water walking, etc.)
--
-- Note : If you don't play the right tune, you get indications like in the
--	 MasterMind game...
--
-- To motivate the player : there are 4 storerooms (armors, weapons, food and
-- gems) and a wand of wishing in one of the 4 towers...

des.level_init({ style="mazegrid", bg ="-" });

des.level_flags("mazelevel", "noteleport", "noflipy")

des.map([[
}}}}}}}}}.............................................}}}}}}}}}
}BA---AB}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}BA---AB}
}|.....|-----------------------------------------------|.....|}
}|.....+...............................................+.....|}
}BA---A-------------------------+--------------K--------A---AB}
}}}}}}|........|..........+...........|.......S.S.......|}}}}}}
.....}|........|..........|...........|.......|.|.......|}.....
.....}|........--SS----SS--........W.W--------SSS--------}.....
.....}|........+..........+...........S.................+......
.....}|........--SS----SS--........W.W--------SSS--------}.....
.....}|........|..........|...........|.......|.|.......|}.....
}}}}}}|........|..........+...........|.......S.S.......|}}}}}}
}BA---A-------------------------+--------------K--------A---AB}
}|.....+...............................................+.....|}
}|.....|-----------------------------------------------|.....|}
}BA---AB}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}BA---AB}
}}}}}}}}}.............................................}}}}}}}}}
]]);

-- Random level layout modifications
local pillars     = { {05, 05}, {05, 11}, {57, 05}, {57, 11} };
local corners1    = { {02, 02}, {02, 14}, {60, 02}, {60, 14} };
local corners2    = { {02, 03}, {02, 13}, {60, 03}, {60, 13} };
local corners3    = { {06, 02}, {06, 14}, {56, 02}, {56, 14} };
local pillarpools = { {04, 06}, {04, 10}, {58, 06}, {58, 10} };
local turretbars1 = { {06, 06}, {06, 10} };
local thronetrees = { {36, 07}, {36, 09} };

-- "shooting gallery" bars for soldiers or dragons
-- 20% chance to add a barred corridor. it's pretty dangerous, so give player an altar as a reward
if percent(20) then
    des.replace_terrain({ region={15,07, 26,09}, fromterrain="S", toterrain="F", chance=100 });
    des.replace_terrain({ region={47,04, 47,12}, fromterrain="K", toterrain="-", chance=100 });
    des.door("locked",47,07);
    des.door("locked",47,09);
    des.altar({ x=36, y=08, align="random" });
    des.region({ region={27,05, 37,11},lit=1,type="temple", filled=2 });
else
    des.region({ region={27,05, 37,11},lit=1,type="throne", filled=2 });
    des.replace_terrain({ region={15,07, 26,09}, fromterrain="S", toterrain="-", chance=100 });
    des.feature({ type = "throne", x=36, y=08 });
    -- 32% chance (due to nested "if") for barred dragons
    if percent(40) then
        des.replace_terrain({ region={46,07, 48,09}, fromterrain="S", toterrain="F", chance=100 });
        des.replace_terrain({ region={47,04, 47,12}, fromterrain="K", toterrain="S", chance=100 });
    else
        des.replace_terrain({ region={47,04, 47,12}, fromterrain="K", toterrain="F", chance=100 });
end
end

-- Random registers initialisation
local object = { "[", ")", "*", "%" };
shuffle(object)

local place = selection.new();
place:set(04,02);
place:set(58,02);
place:set(04,14);
place:set(58,14);

local monster = { "L", "N", "E", "H", "M", "O", "R", "T", "X", "Z" }
shuffle(monster)

des.teleport_region({ region = {01,00,10,20}, region_islev=1, exclude={01,1,61,15}, dir="down" })
des.teleport_region({ region = {69,00,79,20}, region_islev=1, exclude={01,1,61,15}, dir="up" })
des.levregion({ region = {01,00,10,20}, region_islev=1, exclude={00,0,62,16}, type="stair-up" })

-- Doors
des.door("closed",07,03)
des.door("closed",55,03)
des.door("locked",32,04)
des.door("locked",26,05)
des.door("locked",46,05)
des.door("locked",48,05)
des.door("closed",15,08)
des.door("closed",26,08)
des.door("locked",38,08)
des.door("locked",56,08)
des.door("locked",26,11)
des.door("locked",46,11)
des.door("locked",48,11)
des.door("locked",32,12)
des.door("closed",07,13)
des.door("closed",55,13)
-- The drawbridge
des.drawbridge({ dir="east", state="closed", x=05,y=08})
-- Storeroom number 1
des.object(object[1],39,05)
des.object(object[1],40,05)
des.object(object[1],41,05)
des.object(object[1],42,05)
des.object(object[1],43,05)
des.object(object[1],44,05)
des.object(object[1],45,05)
des.object(object[1],39,06)
des.object(object[1],40,06)
des.object(object[1],41,06)
des.object(object[1],42,06)
des.object(object[1],43,06)
des.object(object[1],44,06)
des.object(object[1],45,06)
-- Storeroom number 2
des.object(object[2],49,05)
des.object(object[2],50,05)
des.object(object[2],51,05)
des.object(object[2],52,05)
des.object(object[2],53,05)
des.object(object[2],54,05)
des.object(object[2],55,05)
des.object(object[2],49,06)
des.object(object[2],50,06)
des.object(object[2],51,06)
des.object(object[2],52,06)
des.object(object[2],53,06)
des.object(object[2],54,06)
des.object(object[2],55,06)
-- Storeroom number 3
des.object(object[3],39,10)
des.object(object[3],40,10)
des.object(object[3],41,10)
des.object(object[3],42,10)
des.object(object[3],43,10)
des.object(object[3],44,10)
des.object(object[3],45,10)
des.object(object[3],39,11)
des.object(object[3],40,11)
des.object(object[3],41,11)
des.object(object[3],42,11)
des.object(object[3],43,11)
des.object(object[3],44,11)
des.object(object[3],45,11)
-- Storeroom number 4
des.object(object[4],49,10)
des.object(object[4],50,10)
des.object(object[4],51,10)
des.object(object[4],52,10)
des.object(object[4],53,10)
des.object(object[4],54,10)
des.object(object[4],55,10)
des.object(object[4],49,11)
des.object(object[4],50,11)
des.object(object[4],51,11)
des.object(object[4],52,11)
des.object(object[4],53,11)
des.object(object[4],54,11)
des.object(object[4],55,11)
-- THE WAND OF WISHING in 1 of the 4 towers
local px, py = place:rndcoord(1);
des.object({ id = "chest", trapped = 0, locked = 1, x = px, y = py,
             contents = function()
                des.object( {id="wishing", buc = "uncursed", spe = 2 });
             end
});
-- Prevent monsters from eating it.  (@'s never eat objects)
des.engraving({ x = px, y = py, type="burn", text="Elbereth" })
des.object({ id = "scare monster", x = px, y = py, buc="cursed" })
-- The treasure of the lord
des.object("chest",37,08)
-- Traps
des.trap("trap door",40,08)
des.trap("trap door",44,08)
des.trap("trap door",48,08)
des.trap("trap door",52,08)
des.trap("trap door",55,08)
-- Soldiers guarding the entry hall
des.monster("soldier",08,06)
des.monster("soldier",09,05)
des.monster("soldier",11,05)
des.monster("soldier",12,06)
des.monster("soldier",08,10)
des.monster("soldier",09,11)
des.monster("soldier",11,11)
des.monster("soldier",12,10)
des.monster("lieutenant",09,08)
-- Soldiers guarding the towers
des.monster("soldier",03,02)
des.monster("soldier",05,02)
des.monster("soldier",57,02)
des.monster("soldier",59,02)
des.monster("soldier",03,14)
des.monster("soldier",05,14)
des.monster("soldier",57,14)
des.monster("soldier",59,14)
-- The four dragons that are guarding the storerooms
des.monster("D",47,05)
des.monster("D",47,06)
des.monster("D",47,10)
des.monster("D",47,11)
-- Sea monsters in the moat
des.monster("giant eel",05,07)
des.monster("giant eel",05,09)
des.monster("giant eel",57,07)
des.monster("giant eel",57,09)
des.monster("shark",05,00)
des.monster("shark",05,16)
des.monster("shark",57,00)
des.monster("shark",57,16)
-- The throne room and the court monsters
des.monster(monster[10],27,05)
des.monster(monster[1],30,05)
des.monster(monster[2],33,05)
des.monster(monster[3],36,05)
des.monster(monster[4],28,06)
des.monster(monster[5],31,06)
des.monster(monster[6],34,06)
des.monster(monster[7],37,06)
des.monster(monster[8],27,07)
des.monster(monster[9],30,07)
des.monster(monster[10],33,07)
des.monster(monster[1],36,07)
des.monster(monster[2],28,08)
des.monster(monster[3],31,08)
des.monster(monster[4],34,08)
des.monster(monster[5],27,09)
des.monster(monster[6],30,09)
des.monster(monster[7],33,09)
des.monster(monster[8],36,09)
des.monster(monster[9],28,10)
des.monster(monster[10],31,10)
des.monster(monster[1],34,10)
des.monster(monster[2],37,10)
des.monster(monster[3],27,11)
des.monster(monster[4],30,11)
des.monster(monster[5],33,11)
des.monster(monster[6],36,11)
-- MazeWalks
des.mazewalk(00,10,"west")
des.mazewalk(62,06,"east")
-- Non diggable walls
des.non_diggable(selection.area(00,00,62,16))
-- Subrooms:
--   Entire castle area
des.region(selection.area(00,00,62,16),"unlit")
--   Moat
-- 50% chance to change shape of moat.
if percent(50) then
    for pos=1,4 do
        des.terrain(pillars[pos], "T")
    end
    for pos=1,4 do
        des.terrain(pillarpools[pos], "}")
    end
end
--   Courtyards
des.region(selection.area(00,05,05,11),"lit")
des.region(selection.area(57,05,62,11),"lit")
-- "turrets"; 10% chance to add bars for soldiers to shoot through.
if percent(10) then
    for pos=1,2 do
        des.terrain(turretbars1[pos], "F")
    end
end

--   Throne room / temple
-- 40% chance to put trees next to the throne/altar
if percent(40) then
    for pos=1,2 do
        des.terrain(thronetrees[pos], "T");
    end
end
-- 20% chance to put fountains next to the throne/altar
if percent(20) then
    des.replace_terrain({ region={35,07, 37,09}, fromterrain="W", toterrain="{", chance=90 })
    des.replace_terrain({ region={35,07, 37,09}, fromterrain="W", toterrain=".", chance=100 })
else
    des.replace_terrain({ region={35,07, 37,09}, fromterrain="W", toterrain=".", chance=100 })
end

--   Antechamber
des.region(selection.area(07,05,14,11),"lit");
-- 60% chance to replace the single fountain with either
-- 4 fountains or a historic statue of some type of local monster.
if percent(40) then
    des.feature("fountain", 10,08)
elseif percent(50) then
    des.replace_terrain({ region={07,05,14,11}, fromterrain=".", toterrain="g" })
    des.feature("fountain", 09,08)
    des.feature("fountain", 11,08)
    des.feature("fountain", 10,07)
    des.feature("fountain", 10,09)
else
    des.replace_terrain({ region={07,05,14,11}, fromterrain=".", toterrain="g" })
    des.object({id="statue", x=10, y=08, montype=monster[10], historic=true})
end
--20% chance of several statues in the antechamber, a preview of what's coming
if percent (20) then
    des.object({id="statue", x=09, y=07, montype=monster[1], historic=false})
    des.object({id="statue", x=09, y=09, montype=monster[2], historic=false})
    des.object({id="statue", x=11, y=07, montype=monster[3], historic=false})
    des.object({id="statue", x=11, y=09, montype=monster[4], historic=false})
end
-- 20% chance to turn the antechamber into a garden.
-- the garden could be even prettier if we cherrypick grass terrain from xnethack
if percent(25) then
    local garden = selection.circle(10,08, 2);
    local tree = selection.percentage(garden, 70);
    des.replace_terrain({ selection=tree, fromterrain=".", toterrain="T" });
    des.terrain(08,08, ".")
--        the following code should do roughly the same thing as the above code
--        however, there is a bug, for which copperwater has submitted a PR in vanilla nethack
--        des.replace_terrain({ region={08,06, 13,10}, fromterrain=".", toterrain="T", chance=20 });
end

--   Storerooms
des.region(selection.area(39,05,45,06),"lit")
des.region(selection.area(39,10,45,11),"lit")
des.region(selection.area(49,05,55,06),"lit")
des.region(selection.area(49,10,55,11),"lit")

--   Corners
des.region(selection.area(02,02,06,03),"lit")
des.region(selection.area(56,02,60,03),"lit")
des.region(selection.area(02,13,06,14),"lit")
des.region(selection.area(56,13,60,14),"lit")
-- 40% total chance to alter the corners of towers.
if percent(25) then
    des.replace_terrain({ selection=map, fromterrain="B", toterrain="}"});
    des.replace_terrain({ selection=map, fromterrain="A", toterrain="|"});
    for pos=1,4 do
        des.terrain(corners1[pos], "-")
        des.terrain(corners2[pos], "-")
        des.terrain(corners3[pos], "-")
    end
else
    des.replace_terrain({ selection=map, fromterrain="B", toterrain="-"});
    des.replace_terrain({ selection=map, fromterrain="A", toterrain="-"});
end
if percent(20) then -- this overwrites the above 5% of the overall time
    for pos=1,4 do
        des.terrain(corners1[pos], " ")
        des.terrain(corners2[pos], " ")
    end
end

--   Barracks
des.region({ region={16,05, 25,06},lit=1,type="barracks", filled=1 })
des.region({ region={16,10, 25,11},lit=1,type="barracks", filled=1 })

--   Hallways
des.region(selection.area(08,03,54,03),"unlit")
des.region(selection.area(08,13,54,13),"unlit")
des.region(selection.area(16,08,25,08),"unlit")
des.region(selection.area(39,08,55,08),"unlit")

--   Storeroom alcoves
des.region(selection.area(47,05,47,06),"unlit")
des.region(selection.area(47,10,47,11),"unlit")
