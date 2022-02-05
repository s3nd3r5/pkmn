EverGrandeCity_BluesRoom_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, EverGrandeCity_BluesRoom_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, EverGrandeCity_BluesRoom_OnWarp
	map_script MAP_SCRIPT_ON_TRANSITION, EverGrandeCity_BluesRoom_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, EverGrandeCity_BluesRoom_OnFrame
	.byte 0

EverGrandeCity_BluesRoom_OnTransition:
	setflag FLAG_MET_SCOTT_IN_EVERGRANDE
	setflag FLAG_HIDE_EVER_GRANDE_POKEMON_CENTER_1F_SCOTT
	end

EverGrandeCity_BluesRoom_OnLoad:
	call_if_set FLAG_DEFEATED_ELITE_4_BLUE, EverGrandeCity_BluesRoom_EventScript_ResetAdvanceToNextRoom
	call_if_eq VAR_ELITE_4_STATE, 1, EverGrandeCity_BluesRoom_EventScript_CloseDoor
	end

EverGrandeCity_BluesRoom_EventScript_ResetAdvanceToNextRoom::
	call PokemonLeague_EliteFour_EventScript_ResetAdvanceToNextRoom
	return

EverGrandeCity_BluesRoom_EventScript_CloseDoor::
	call PokemonLeague_EliteFour_EventScript_CloseDoor
	return

EverGrandeCity_BluesRoom_OnWarp:
	map_script_2 VAR_TEMP_1, 0, EverGrandeCity_BluesRoom_EventScript_PlayerTurnNorth
	.2byte 0

EverGrandeCity_BluesRoom_EventScript_PlayerTurnNorth::
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	end

EverGrandeCity_BluesRoom_OnFrame:
	map_script_2 VAR_ELITE_4_STATE, 0, EverGrandeCity_BluesRoom_EventScript_WalkInCloseDoor
	.2byte 0

EverGrandeCity_BluesRoom_EventScript_WalkInCloseDoor::
	lockall
	call PokemonLeague_EliteFour_EventScript_WalkInCloseDoor
	setvar VAR_ELITE_4_STATE, 1
	releaseall
	end

EverGrandeCity_BluesRoom_EventScript_Blue::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_ELITE_4_BLUE, EverGrandeCity_BluesRoom_EventScript_PostBattleSpeech
	playbgm MUS_ENCOUNTER_ELITE_FOUR, FALSE
	msgbox EverGrandeCity_BluesRoom_Text_IntroSpeech, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_BLUE, EverGrandeCity_BluesRoom_Text_Defeat
	goto EverGrandeCity_BluesRoom_EventScript_Defeated
	end

EverGrandeCity_BluesRoom_EventScript_PostBattleSpeech::
	msgbox EverGrandeCity_BluesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_BluesRoom_EventScript_Defeated::
	setflag FLAG_DEFEATED_ELITE_4_BLUE
	call PokemonLeague_EliteFour_SetAdvanceToNextRoomMetatiles
	msgbox EverGrandeCity_BluesRoom_Text_PostBattleSpeech, MSGBOX_DEFAULT
	release
	end

EverGrandeCity_BluesRoom_Text_IntroSpeech:
	.string "Welcome, challenger!\n"
	.string "I'm BLUE of the ELITE FOUR.\p"
	.string "I like that look you're giving me.\n"
	.string "I guess you'll give me a good match.\l"
	.string "That's good! Looking real good!\p"
	.string "All right! You and me, let's enjoy\n"
	.string "a battle that can only be staged\l"
	.string "here in the POKéMON LEAGUE!$"

EverGrandeCity_BluesRoom_Text_Defeat:
	.string "Well, how do you like that? I lost!\n"
	.string "Eh, it was fun, so it doesn't matter.$"

EverGrandeCity_BluesRoom_Text_PostBattleSpeech:
	.string "Well, listen to what this loser has\n"
	.string "to say.\p"
	.string "You've got what it takes to go far.\n"
	.string "Now, go on to the next room and enjoy\l"
	.string "your next battle!$"

