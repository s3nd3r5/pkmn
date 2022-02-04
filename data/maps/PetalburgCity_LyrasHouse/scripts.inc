.set LOCALID_LYRAS_DAD, 1

PetalburgCity_LyrasHouse_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PetalburgCity_LyrasHouse_OnFrame
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PetalburgCity_LyrasHouse_OnWarp
	.byte 0

PetalburgCity_LyrasHouse_OnWarp:
	map_script_2 VAR_PETALBURG_CITY_STATE, 4, PetalburgCity_LyrasHouse_EventScript_PlayerLyrasDadFaceEachOther
	.2byte 0

PetalburgCity_LyrasHouse_EventScript_PlayerLyrasDadFaceEachOther::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_EAST
	turnobject LOCALID_LYRAS_DAD, DIR_WEST
	end

PetalburgCity_LyrasHouse_OnFrame:
	map_script_2 VAR_PETALBURG_CITY_STATE, 4, PetalburgCity_LyrasHouse_EventScript_GiveHM03Surf
	.2byte 0

PetalburgCity_LyrasHouse_EventScript_GiveHM03Surf::
	lockall
	msgbox PetalburgCity_LyrasHouse_Text_PleaseExcuseUs, MSGBOX_DEFAULT
	giveitem ITEM_HM03
	setflag FLAG_RECEIVED_HM03
	msgbox PetalburgCity_LyrasHouse_Text_SurfGoAllSortsOfPlaces, MSGBOX_DEFAULT
	setvar VAR_PETALBURG_CITY_STATE, 5
	releaseall
	end

PetalburgCity_LyrasHouse_EventScript_LyrasDad::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_LYRA_VICTORY_ROAD, PetalburgCity_LyrasHouse_EventScript_DefeatedLyraInVictoryRoad
	goto_if_set FLAG_RECEIVED_HM03, PetalburgCity_LyrasHouse_EventScript_ReceievedHM03Surf
	goto_if_set FLAG_THANKED_FOR_PLAYING_WITH_LYRA, PetalburgCity_LyrasHouse_EventScript_PlayedWithLyra
	msgbox PetalburgCity_LyrasHouse_Text_ThanksForPlayingWithLyra, MSGBOX_DEFAULT
	setflag FLAG_THANKED_FOR_PLAYING_WITH_LYRA
	release
	end

PetalburgCity_LyrasHouse_EventScript_ReceievedHM03Surf::
	msgbox PetalburgCity_LyrasHouse_Text_LyraIsComingHomeSoon, MSGBOX_DEFAULT
	release
	end

PetalburgCity_LyrasHouse_EventScript_DefeatedLyraInVictoryRoad::
	msgbox PetalburgCity_LyrasHouse_Text_YouMetLyraInEverGrandeCity, MSGBOX_DEFAULT
	release
	end

PetalburgCity_LyrasHouse_EventScript_PlayedWithLyra::
	msgbox PetalburgCity_LyrasHouse_Text_WonderHowLyraIsDoing, MSGBOX_DEFAULT
	release
	end

PetalburgCity_LyrasHouse_EventScript_LyrasMom::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_HM03, PetalburgCity_LyrasHouse_EventScript_ReceivedHM03Surf
	msgbox PetalburgCity_LyrasHouse_Text_LyraWasReallyHappy, MSGBOX_DEFAULT
	release
	end

PetalburgCity_LyrasHouse_EventScript_ReceivedHM03Surf::
	msgbox PetalburgCity_LyrasHouse_Text_LyraLeftWithoutTelling, MSGBOX_DEFAULT
	release
	end

PetalburgCity_LyrasHouse_Text_ThanksForPlayingWithLyra:
	.string "You're…\n"
	.string "Ah, you must be {PLAYER}{KUN}, right?\p"
	.string "Thank you for playing with LYRA a\n"
	.string "little while ago.\p"
	.string "He's been frail and sickly ever\n"
	.string "since he was a baby.\p"
	.string "We've sent him to stay with my relatives\n"
	.string "in VERDANTURF TOWN for a while.\p"
	.string "The air is a lot cleaner there\n"
	.string "than it is here.\p"
	.string "What's that? Where's LYRA?\n"
	.string "He's already left, our LYRA.\p"
	.string "I wonder where he could have\n"
	.string "gotten by now?$"

PetalburgCity_LyrasHouse_Text_WonderHowLyraIsDoing:
	.string "I wonder how our LYRA is doing?$"

PetalburgCity_LyrasHouse_Text_PleaseExcuseUs:
	.string "{PLAYER}{KUN}! Please excuse us for\n"
	.string "dragging you here this way.\p"
	.string "But our LYRA's become very healthy\n"
	.string "since he went to VERDANTURF TOWN.\p"
	.string "We owe it all to you!\p"
	.string "When LYRA left town, you helped\n"
	.string "him catch a POKéMON, right?\p"
	.string "I think that made LYRA really\n"
	.string "happy.\p"
	.string "Actually, not just LYRA.\n"
	.string "It made me, his father, happy too.\p"
	.string "Happy that he's gained such a great\n"
	.string "friend as you.\p"
	.string "This isn't a bribe or anything, but\n"
	.string "I'd really like you to have this.$"

PetalburgCity_LyrasHouse_Text_SurfGoAllSortsOfPlaces:
	.string "If your POKéMON can SURF, you'll be\n"
	.string "able to go to all sorts of places.$"

PetalburgCity_LyrasHouse_Text_LyraIsComingHomeSoon:
	.string "LYRA's coming home soon.\n"
	.string "I'm looking forward to that.$"

PetalburgCity_LyrasHouse_Text_YouMetLyraInEverGrandeCity:
	.string "Oh? You met LYRA in\n"
	.string "EVER GRANDE CITY?\p"
	.string "Oh, {PLAYER}{KUN}, don't be silly.\p"
	.string "He may have gotten healthy, but he\n"
	.string "can't go somewhere far away like\l"
	.string "that all by himself.$"

PetalburgCity_LyrasHouse_Text_LyraWasReallyHappy:
	.string "LYRA was really happy when he told\n"
	.string "us that he caught a POKéMON.\p"
	.string "It's been ages since I've seen him\n"
	.string "smile like that.$"

PetalburgCity_LyrasHouse_Text_LyraLeftWithoutTelling:
	.string "I want you to keep this a secret\n"
	.string "from my husband…\p"
	.string "But our LYRA left VERDANTURF TOWN\n"
	.string "without telling anyone.\p"
	.string "You know, LYRA is frail, but\n"
	.string "he's surprisingly strong-willed.\p"
	.string "I'm sure that he'll come back safe\n"
	.string "and sound one day!$"

