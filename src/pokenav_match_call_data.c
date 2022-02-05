#include "global.h"
#include "battle_setup.h"
#include "data.h"
#include "event_data.h"
#include "string_util.h"
#include "battle.h"
#include "gym_leader_rematch.h"
#include "match_call.h"
#include "pokenav.h"
#include "strings.h"
#include "constants/region_map_sections.h"
#include "constants/trainers.h"


// NPC below means non-trainer character (no rematch or check page)
// Steven also uses this type but has a check page by using a MatchCallCheckPageOverride
enum
{
    MC_TYPE_NPC,
    MC_TYPE_TRAINER,
    MC_TYPE_LYRA,
    MC_TYPE_BIRCH,
    MC_TYPE_RIVAL,
    MC_TYPE_LEADER
};

// Static type declarations

typedef struct MatchCallTextDataStruct {
    const u8 *text;
    u16 flag;
    u16 flag2;
} match_call_text_data_t;

struct MatchCallStructCommon {
    u8 type;
    u8 mapSec;
    u16 flag;
};

struct MatchCallStructNPC {
    u8 type;
    u8 mapSec;
    u16 flag;
    const u8 *desc;
    const u8 *name;
    const match_call_text_data_t *textData;
};

// Shared by MC_TYPE_TRAINER and MC_TYPE_LEADER
struct MatchCallStructTrainer {
    u8 type;
    u8 mapSec;
    u16 flag;
    u16 rematchTableIdx;
    const u8 *desc;
    const u8 *name;
    const match_call_text_data_t *textData;
};

struct MatchCallLocationOverride {
    u16 flag;
    u8 mapSec;
};

struct MatchCallLyra {
    u8 type;
    u8 mapSec;
    u16 flag;
    u16 rematchTableIdx;
    const u8 *desc;
    const match_call_text_data_t *textData;
    const struct MatchCallLocationOverride *locationData;
};

struct MatchCallBirch {
    u8 type;
    u8 mapSec;
    u16 flag;
    const u8 *desc;
    const u8 *name;
};

struct MatchCallRival {
    u8 type;
    u8 playerGender;
    u16 flag;
    const u8 *desc;
    const u8 *name;
    const match_call_text_data_t *textData;
};

typedef union {
    const struct MatchCallStructCommon *common;
    const struct MatchCallStructNPC *npc;
    const struct MatchCallStructTrainer *trainer;
    const struct MatchCallLyra *lyra;
    const struct MatchCallBirch *birch;
    const struct MatchCallRival *rival;
    const struct MatchCallStructTrainer *leader;
} match_call_t;

struct MatchCallCheckPageOverride {
    u16 idx;
    u16 facilityClass;
    u32 flag;
    const u8 *flavorTexts[CHECK_PAGE_ENTRY_COUNT];
};

// Static RAM declarations

// Static ROM declarations

static bool32 MatchCall_GetEnabled_NPC(match_call_t);
static bool32 MatchCall_GetEnabled_Trainer(match_call_t);
static bool32 MatchCall_GetEnabled_Lyra(match_call_t);
static bool32 MatchCall_GetEnabled_Birch(match_call_t);
static bool32 MatchCall_GetEnabled_Rival(match_call_t);

static u8 MatchCall_GetMapSec_NPC(match_call_t);
static u8 MatchCall_GetMapSec_Trainer(match_call_t);
static u8 MatchCall_GetMapSec_Lyra(match_call_t);
static u8 MatchCall_GetMapSec_Birch(match_call_t);
static u8 MatchCall_GetMapSec_Rival(match_call_t);

static bool32 MatchCall_IsRematchable_NPC(match_call_t);
static bool32 MatchCall_IsRematchable_Trainer(match_call_t);
static bool32 MatchCall_IsRematchable_Lyra(match_call_t);
static bool32 MatchCall_IsRematchable_Birch(match_call_t);
static bool32 MatchCall_IsRematchable_Rival(match_call_t);

static bool32 MatchCall_HasCheckPage_NPC(match_call_t);
static bool32 MatchCall_HasCheckPage_Trainer(match_call_t);
static bool32 MatchCall_HasCheckPage_Lyra(match_call_t);
static bool32 MatchCall_HasCheckPage_Birch(match_call_t);
static bool32 MatchCall_HasCheckPage_Rival(match_call_t);

static u32 MatchCall_GetRematchTableIdx_NPC(match_call_t);
static u32 MatchCall_GetRematchTableIdx_Trainer(match_call_t);
static u32 MatchCall_GetRematchTableIdx_Lyra(match_call_t);
static u32 MatchCall_GetRematchTableIdx_Birch(match_call_t);
static u32 MatchCall_GetRematchTableIdx_Rival(match_call_t);

static void MatchCall_GetMessage_NPC(match_call_t, u8 *);
static void MatchCall_GetMessage_Trainer(match_call_t, u8 *);
static void MatchCall_GetMessage_Lyra(match_call_t, u8 *);
static void MatchCall_GetMessage_Birch(match_call_t, u8 *);
static void MatchCall_GetMessage_Rival(match_call_t, u8 *);

static void MatchCall_GetNameAndDesc_NPC(match_call_t, const u8 **, const u8 **);
static void MatchCall_GetNameAndDesc_Trainer(match_call_t, const u8 **, const u8 **);
static void MatchCall_GetNameAndDesc_Lyra(match_call_t, const u8 **, const u8 **);
static void MatchCall_GetNameAndDesc_Birch(match_call_t, const u8 **, const u8 **);
static void MatchCall_GetNameAndDesc_Rival(match_call_t, const u8 **, const u8 **);

static void MatchCall_BufferCallMessageText(const match_call_text_data_t *, u8 *);
static void MatchCall_BufferCallMessageTextByRematchTeam(const match_call_text_data_t *, u16, u8 *);
static void MatchCall_GetNameAndDescByRematchIdx(u32, const u8 **, const u8 **);

// .rodata

static const match_call_text_data_t sMrStoneTextScripts[] = {
    { MatchCall_Text_MrStone1,  0xFFFF,                              FLAG_ENABLE_MR_STONE_POKENAV },
    { MatchCall_Text_MrStone2,  FLAG_ENABLE_MR_STONE_POKENAV,        0xFFFF },
    { MatchCall_Text_MrStone3,  FLAG_DELIVERED_STEVEN_LETTER,        0xFFFF },
    { MatchCall_Text_MrStone4,  FLAG_RECEIVED_EXP_SHARE,             0xFFFF },
    { MatchCall_Text_MrStone5,  FLAG_RECEIVED_HM04,                  0xFFFF },
    { MatchCall_Text_MrStone6,  FLAG_DEFEATED_PETALBURG_GYM,         0xFFFF },
    { MatchCall_Text_MrStone7,  FLAG_RECEIVED_CASTFORM,              0xFFFF },
    { MatchCall_Text_MrStone8,  FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 0xFFFF },
    { MatchCall_Text_MrStone9,  FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 0xFFFF },
    { MatchCall_Text_MrStone10, FLAG_DEFEATED_SOOTOPOLIS_GYM,        0xFFFF },
    { MatchCall_Text_MrStone11, FLAG_SYS_GAME_CLEAR,                 0xFFFF },
    { NULL,                     0xFFFF,                              0xFFFF }
};

static const struct MatchCallStructNPC sMrStoneMatchCallHeader =
{
    .type = MC_TYPE_NPC,
    .mapSec = MAPSEC_RUSTBORO_CITY,
    .flag = 0xFFFF,
    .desc = gText_MrStoneMatchCallDesc,
    .name = gText_MrStoneMatchCallName,
    .textData = sMrStoneTextScripts
};

static const match_call_text_data_t sNormanTextScripts[] = {
    { MatchCall_Text_Norman1, FLAG_ENABLE_NORMAN_MATCH_CALL, 0xFFFF },
    { MatchCall_Text_Norman2, FLAG_DEFEATED_DEWFORD_GYM,     0xFFFF },
    { MatchCall_Text_Norman3, FLAG_DEFEATED_LAVARIDGE_GYM,   0xFFFF },
    { MatchCall_Text_Norman4, FLAG_DEFEATED_PETALBURG_GYM,   0xFFFF },
    { MatchCall_Text_Norman5, FLAG_RECEIVED_RED_OR_BLUE_ORB, 0xFFFF },
    { MatchCall_Text_Norman6, 0xFFFE,                        0xFFFF },
    { MatchCall_Text_Norman7, FLAG_SYS_GAME_CLEAR,           0xFFFF },
    { MatchCall_Text_Norman8, FLAG_SYS_GAME_CLEAR,           0xFFFF },
    { MatchCall_Text_Norman9, FLAG_SYS_GAME_CLEAR,           0xFFFF },
    { NULL,                   0xFFFF,                        0xFFFF }
};

static const struct MatchCallStructTrainer sNormanMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_PETALBURG_CITY,
    .flag = FLAG_ENABLE_NORMAN_MATCH_CALL,
    .rematchTableIdx = REMATCH_NORMAN,
    .desc = gText_NormanMatchCallDesc,
    .name = gText_NormanMatchCallName,
    .textData = sNormanTextScripts
};

static const struct MatchCallBirch sProfBirchMatchCallHeader =
{
    .type = MC_TYPE_BIRCH,
    .mapSec = 0,
    .flag = FLAG_ENABLE_PROF_BIRCH_MATCH_CALL,
    .desc = gText_ProfBirchMatchCallDesc,
    .name = gText_ProfBirchMatchCallName
};

static const match_call_text_data_t sMomTextScripts[] = {
    { MatchCall_Text_Mom1, 0xFFFF,                      0xFFFF },
    { MatchCall_Text_Mom2, FLAG_DEFEATED_PETALBURG_GYM, 0xFFFF },
    { MatchCall_Text_Mom3, FLAG_SYS_GAME_CLEAR,         0xFFFF },
    { NULL,                0xFFFF,                      0xFFFF }
};

static const struct MatchCallStructNPC sMomMatchCallHeader =
{
    .type = MC_TYPE_NPC,
    .mapSec = MAPSEC_LITTLEROOT_TOWN,
    .flag = FLAG_ENABLE_MOM_MATCH_CALL,
    .desc = gText_MomMatchCallDesc,
    .name = gText_MomMatchCallName,
    .textData = sMomTextScripts
};

static const match_call_text_data_t sStevenTextScripts[] = {
    { MatchCall_Text_Steven1, 0xFFFF,                              0xFFFF },
    { MatchCall_Text_Steven2, FLAG_RUSTURF_TUNNEL_OPENED,          0xFFFF },
    { MatchCall_Text_Steven3, FLAG_RECEIVED_RED_OR_BLUE_ORB,       0xFFFF },
    { MatchCall_Text_Steven4, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 0xFFFF },
    { MatchCall_Text_Steven5, FLAG_DEFEATED_MOSSDEEP_GYM,          0xFFFF },
    { MatchCall_Text_Steven6, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, 0xFFFF },
    { MatchCall_Text_Steven7, FLAG_SYS_GAME_CLEAR,                 0xFFFF },
    { NULL,                   0xFFFF,                              0xFFFF },
};

static const struct MatchCallStructNPC sStevenMatchCallHeader =
{
    .type = MC_TYPE_NPC,
    .mapSec = MAPSEC_NONE,
    .flag = FLAG_REGISTERED_STEVEN_POKENAV,
    .desc = gText_StevenMatchCallDesc,
    .name = gText_StevenMatchCallName,
    .textData = sStevenTextScripts
};

static const match_call_text_data_t sMayTextScripts[] = {
    { MatchCall_Text_May1,  0xFFFF,                              0xFFFF },
    { MatchCall_Text_May2,  FLAG_DEFEATED_DEWFORD_GYM,           0xFFFF },
    { MatchCall_Text_May3,  FLAG_DELIVERED_DEVON_GOODS,          0xFFFF },
    { MatchCall_Text_May4,  FLAG_HIDE_MAUVILLE_CITY_LYRA,       0xFFFF },
    { MatchCall_Text_May5,  FLAG_RECEIVED_HM04,                  0xFFFF },
    { MatchCall_Text_May6,  FLAG_DEFEATED_LAVARIDGE_GYM,         0xFFFF },
    { MatchCall_Text_May7,  FLAG_DEFEATED_PETALBURG_GYM,         0xFFFF },
    { MatchCall_Text_May8,  FLAG_RECEIVED_CASTFORM,              0xFFFF },
    { MatchCall_Text_May9,  FLAG_RECEIVED_RED_OR_BLUE_ORB,       0xFFFF },
    { MatchCall_Text_May10, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 0xFFFF },
    { MatchCall_Text_May11, FLAG_MET_TEAM_AQUA_HARBOR,           0xFFFF },
    { MatchCall_Text_May12, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 0xFFFF },
    { MatchCall_Text_May13, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, 0xFFFF },
    { MatchCall_Text_May14, FLAG_DEFEATED_SOOTOPOLIS_GYM,        0xFFFF },
    { MatchCall_Text_May15, FLAG_SYS_GAME_CLEAR,                 0xFFFF },
    { NULL,                 0xFFFF,                              0xFFFF }
};

static const struct MatchCallRival sMayMatchCallHeader =
{
    .type = MC_TYPE_RIVAL,
    .playerGender = MALE,
    .flag = FLAG_ENABLE_RIVAL_MATCH_CALL,
    .desc = gText_MayEthanMatchCallDesc,
    .name = gText_ExpandedPlaceholder_May,
    .textData = sMayTextScripts
};

static const match_call_text_data_t sEthanTextScripts[] = {
    { MatchCall_Text_Ethan1,  0xFFFF,                              0xFFFF },
    { MatchCall_Text_Ethan2,  FLAG_DEFEATED_DEWFORD_GYM,           0xFFFF },
    { MatchCall_Text_Ethan3,  FLAG_DELIVERED_DEVON_GOODS,          0xFFFF },
    { MatchCall_Text_Ethan4,  FLAG_HIDE_MAUVILLE_CITY_LYRA,       0xFFFF },
    { MatchCall_Text_Ethan5,  FLAG_RECEIVED_HM04,                  0xFFFF },
    { MatchCall_Text_Ethan6,  FLAG_DEFEATED_LAVARIDGE_GYM,         0xFFFF },
    { MatchCall_Text_Ethan7,  FLAG_DEFEATED_PETALBURG_GYM,         0xFFFF },
    { MatchCall_Text_Ethan8,  FLAG_RECEIVED_CASTFORM,              0xFFFF },
    { MatchCall_Text_Ethan9,  FLAG_RECEIVED_RED_OR_BLUE_ORB,       0xFFFF },
    { MatchCall_Text_Ethan10, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 0xFFFF },
    { MatchCall_Text_Ethan11, FLAG_MET_TEAM_AQUA_HARBOR,           0xFFFF },
    { MatchCall_Text_Ethan12, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 0xFFFF },
    { MatchCall_Text_Ethan13, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, 0xFFFF },
    { MatchCall_Text_Ethan14, FLAG_DEFEATED_SOOTOPOLIS_GYM,        0xFFFF },
    { MatchCall_Text_Ethan15, FLAG_SYS_GAME_CLEAR,                 0xFFFF },
    { NULL,                     0xFFFF,                              0xFFFF }
};

static const struct MatchCallRival sEthanMatchCallHeader =
{
    .type = MC_TYPE_RIVAL,
    .playerGender = FEMALE,
    .flag = FLAG_ENABLE_RIVAL_MATCH_CALL,
    .desc = gText_MayEthanMatchCallDesc,
    .name = gText_ExpandedPlaceholder_Ethan,
    .textData = sEthanTextScripts
};

static const match_call_text_data_t sLyraTextScripts[] = {
    { MatchCall_Text_Lyra1, 0xFFFF,                              0xFFFF },
    { MatchCall_Text_Lyra2, FLAG_RUSTURF_TUNNEL_OPENED,          0xFFFF },
    { MatchCall_Text_Lyra3, FLAG_DEFEATED_LAVARIDGE_GYM,         0xFFFF },
    { MatchCall_Text_Lyra4, FLAG_RECEIVED_CASTFORM,              0xFFFF },
    { MatchCall_Text_Lyra5, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 0xFFFF },
    { MatchCall_Text_Lyra6, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN, 0xFFFF },
    { MatchCall_Text_Lyra7, FLAG_DEFEATED_LYRA_VICTORY_ROAD,    0xFFFF },
    { NULL,                  0xFFFF,                              0xFFFF }
};

const struct MatchCallLocationOverride sLyraLocationData[] = {
    { FLAG_HIDE_MAUVILLE_CITY_LYRA,          MAPSEC_VERDANTURF_TOWN },
    { FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT,    MAPSEC_NONE },
    { FLAG_HIDE_VICTORY_ROAD_ENTRANCE_LYRA,  MAPSEC_VICTORY_ROAD },
    { 0xFFFF,                                 MAPSEC_NONE }
};

static const struct MatchCallLyra sLyraMatchCallHeader =
{
    .type = MC_TYPE_LYRA,
    .mapSec = 0,
    .flag = FLAG_ENABLE_LYRA_MATCH_CALL,
    .rematchTableIdx = REMATCH_LYRA_VR,
    .desc = gText_LyraMatchCallDesc,
    .textData = sLyraTextScripts,
    .locationData = sLyraLocationData
};

static const match_call_text_data_t sScottTextScripts[] = {
    { MatchCall_Text_Scott1, 0xFFFF,                              0xFFFF },
    { MatchCall_Text_Scott2, FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY,  0xFFFF },
    { MatchCall_Text_Scott3, FLAG_RECEIVED_CASTFORM,              0xFFFF },
    { MatchCall_Text_Scott4, FLAG_RECEIVED_RED_OR_BLUE_ORB,       0xFFFF },
    { MatchCall_Text_Scott5, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 0xFFFF },
    { MatchCall_Text_Scott6, FLAG_DEFEATED_SOOTOPOLIS_GYM,        0xFFFF },
    { MatchCall_Text_Scott7, FLAG_SYS_GAME_CLEAR,                 0xFFFF },
    { NULL,                  0xFFFF,                              0xFFFF }
};


static const struct MatchCallStructNPC sScottMatchCallHeader =
{
    .type = 0,
    .mapSec = MAPSEC_NONE,
    .flag = FLAG_ENABLE_SCOTT_MATCH_CALL,
    .desc = gText_ScottMatchCallDesc,
    .name = gText_ScottMatchCallName,
    .textData = sScottTextScripts
};

static const match_call_text_data_t sBrockTextScripts[] = {
    { MatchCall_Text_Brock1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Brock2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Brock3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Brock4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                    0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sBrockMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_RUSTBORO_CITY,
    .flag = FLAG_ENABLE_BROCK_MATCH_CALL,
    .rematchTableIdx = REMATCH_BROCK,
    .desc = gText_BrockMatchCallDesc,
    .name = NULL,
    .textData = sBrockTextScripts
};

static const match_call_text_data_t sMistyTextScripts[] = {
    { MatchCall_Text_Misty1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Misty2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Misty3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Misty4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                   0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sMistyMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_DEWFORD_TOWN,
    .flag = FLAG_ENABLE_MISTY_MATCH_CALL,
    .rematchTableIdx = REMATCH_MISTY,
    .desc = gText_MistyMatchCallDesc,
    .name = NULL,
    .textData = sMistyTextScripts
};

static const match_call_text_data_t sSurgeTextScripts[] = {
    { MatchCall_Text_Surge1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Surge2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Surge3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Surge4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                    0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sSurgeMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_MAUVILLE_CITY,
    .flag = FLAG_ENABLE_SURGE_MATCH_CALL,
    .rematchTableIdx = REMATCH_SURGE,
    .desc = gText_SurgeMatchCallDesc,
    .name = NULL,
    .textData = sSurgeTextScripts
};

static const match_call_text_data_t sErikaTextScripts[] = {
    { MatchCall_Text_Erika1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Erika2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Erika3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Erika4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                     0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sErikaMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_LAVARIDGE_TOWN,
    .flag = FLAG_ENABLE_ERIKA_MATCH_CALL,
    .rematchTableIdx = REMATCH_ERIKA,
    .desc = gText_ErikaMatchCallDesc,
    .name = NULL,
    .textData = sErikaTextScripts
};

static const match_call_text_data_t sSabrinaTextScripts[] = {
    { MatchCall_Text_Sabrina1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Sabrina2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Sabrina3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Sabrina4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                   0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sSabrinaMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_FORTREE_CITY,
    .flag = FLAG_ENABLE_SABRINA_MATCH_CALL,
    .rematchTableIdx = REMATCH_SABRINA,
    .desc = gText_SabrinaMatchCallDesc,
    .name = NULL,
    .textData = sSabrinaTextScripts
};

static const match_call_text_data_t sJanineTextScripts[] = {
    { MatchCall_Text_Janine1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Janine2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Janine3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Janine4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                     0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sJanineMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_MOSSDEEP_CITY,
    .flag = FLAG_ENABLE_JANINE_MATCH_CALL,
    .rematchTableIdx = REMATCH_JANINE,
    .desc = gText_JanineMatchCallDesc,
    .name = NULL,
    .textData = sJanineTextScripts
};

static const match_call_text_data_t sBlaineTextScripts[] = {
    { MatchCall_Text_Blaine1, 0xFFFE,              0xFFFF },
    { MatchCall_Text_Blaine2, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Blaine3, 0xFFFF,              0xFFFF },
    { MatchCall_Text_Blaine4, FLAG_SYS_GAME_CLEAR, 0xFFFF },
    { NULL,                 0xFFFF,              0xFFFF }
};

static const struct MatchCallStructTrainer sBlaineMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_SOOTOPOLIS_CITY,
    .flag = FLAG_ENABLE_BLAINE_MATCH_CALL,
    .rematchTableIdx = REMATCH_BLAINE,
    .desc = gText_BlaineMatchCallDesc,
    .name = NULL,
    .textData = sBlaineTextScripts
};

static const match_call_text_data_t sBlueTextScripts[] = {
    { MatchCall_Text_Blue, 0xFFFF, 0xFFFF },
    { NULL,                  0xFFFF, 0xFFFF }
};

static const struct MatchCallStructTrainer sBlueMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_EVER_GRANDE_CITY,
    .flag = FLAG_REMATCH_BLUE,
    .rematchTableIdx = REMATCH_BLUE,
    .desc = gText_EliteFourMatchCallDesc,
    .name = NULL,
    .textData = sBlueTextScripts
};

static const match_call_text_data_t sBrunoTextScripts[] = {
    { MatchCall_Text_Bruno, 0xFFFF, 0xFFFF },
    { NULL,                  0xFFFF, 0xFFFF }
};

static const struct MatchCallStructTrainer sBrunoMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_EVER_GRANDE_CITY,
    .flag = FLAG_REMATCH_BRUNO,
    .rematchTableIdx = REMATCH_BRUNO,
    .desc = gText_EliteFourMatchCallDesc,
    .name = NULL,
    .textData = sBrunoTextScripts
};

static const match_call_text_data_t sLoreleiTextScripts[] = {
    { MatchCall_Text_Lorelei, 0xFFFF, 0xFFFF },
    { NULL,                  0xFFFF, 0xFFFF }
};

static const struct MatchCallStructTrainer sLoreleiMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_EVER_GRANDE_CITY,
    .flag = FLAG_REMATCH_LORELEI,
    .rematchTableIdx = REMATCH_LORELEI,
    .desc = gText_EliteFourMatchCallDesc,
    .name = NULL,
    .textData = sLoreleiTextScripts
};

static const match_call_text_data_t sClaireTextScripts[] = {
    { MatchCall_Text_Claire, 0xFFFF, 0xFFFF },
    { NULL,                 0xFFFF, 0xFFFF }
};

static const struct MatchCallStructTrainer sClaireMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_EVER_GRANDE_CITY,
    .flag = FLAG_REMATCH_CLAIRE,
    .rematchTableIdx = REMATCH_CLAIRE,
    .desc = gText_EliteFourMatchCallDesc,
    .name = NULL,
    .textData = sClaireTextScripts
};

static const match_call_text_data_t sLanceTextScripts[] = {
    { MatchCall_Text_Lance, 0xFFFF, 0xFFFF },
    { NULL,                   0xFFFF, 0xFFFF }
};

static const struct MatchCallStructTrainer sLanceMatchCallHeader =
{
    .type = MC_TYPE_LEADER,
    .mapSec = MAPSEC_EVER_GRANDE_CITY,
    .flag = FLAG_REMATCH_LANCE,
    .rematchTableIdx = REMATCH_LANCE,
    .desc = gText_ChampionMatchCallDesc,
    .name = NULL,
    .textData = sLanceTextScripts
};

static const match_call_t sMatchCallHeaders[] = {
    [MC_HEADER_MR_STONE]   = {.npc    = &sMrStoneMatchCallHeader},
    [MC_HEADER_PROF_BIRCH] = {.birch  = &sProfBirchMatchCallHeader},
    [MC_HEADER_ETHAN]      = {.rival  = &sEthanMatchCallHeader},
    [MC_HEADER_MAY]        = {.rival  = &sMayMatchCallHeader},
    [MC_HEADER_LYRA]      = {.lyra  = &sLyraMatchCallHeader},
    [MC_HEADER_NORMAN]     = {.leader = &sNormanMatchCallHeader},
    [MC_HEADER_MOM]        = {.npc    = &sMomMatchCallHeader},
    [MC_HEADER_STEVEN]     = {.npc    = &sStevenMatchCallHeader},
    [MC_HEADER_SCOTT]      = {.npc    = &sScottMatchCallHeader},
    [MC_HEADER_BROCK]    = {.leader = &sBrockMatchCallHeader},
    [MC_HEADER_MISTY]     = {.leader = &sMistyMatchCallHeader},
    [MC_HEADER_SURGE]    = {.leader = &sSurgeMatchCallHeader},
    [MC_HEADER_ERIKA]   = {.leader = &sErikaMatchCallHeader},
    [MC_HEADER_SABRINA]     = {.leader = &sSabrinaMatchCallHeader},
    [MC_HEADER_JANINE]  = {.leader = &sJanineMatchCallHeader},
    [MC_HEADER_BLAINE]       = {.leader = &sBlaineMatchCallHeader},
    [MC_HEADER_BLUE]     = {.leader = &sBlueMatchCallHeader},
    [MC_HEADER_BRUNO]     = {.leader = &sBrunoMatchCallHeader},
    [MC_HEADER_LORELEI]     = {.leader = &sLoreleiMatchCallHeader},
    [MC_HEADER_CLAIRE]      = {.leader = &sClaireMatchCallHeader},
    [MC_HEADER_LANCE]    = {.leader = &sLanceMatchCallHeader}
};

static bool32 (*const sMatchCallGetEnabledFuncs[])(match_call_t) = {
    MatchCall_GetEnabled_NPC,
    MatchCall_GetEnabled_Trainer,
    MatchCall_GetEnabled_Lyra,
    MatchCall_GetEnabled_Rival,
    MatchCall_GetEnabled_Birch
};

static u8 (*const sMatchCallGetMapSecFuncs[])(match_call_t) = {
    MatchCall_GetMapSec_NPC,
    MatchCall_GetMapSec_Trainer,
    MatchCall_GetMapSec_Lyra,
    MatchCall_GetMapSec_Rival,
    MatchCall_GetMapSec_Birch
};

static bool32 (*const sMatchCall_IsRematchableFunctions[])(match_call_t) = {
    MatchCall_IsRematchable_NPC,
    MatchCall_IsRematchable_Trainer,
    MatchCall_IsRematchable_Lyra,
    MatchCall_IsRematchable_Rival,
    MatchCall_IsRematchable_Birch
};

static bool32 (*const sMatchCall_HasCheckPageFunctions[])(match_call_t) = {
    MatchCall_HasCheckPage_NPC,
    MatchCall_HasCheckPage_Trainer,
    MatchCall_HasCheckPage_Lyra,
    MatchCall_HasCheckPage_Rival,
    MatchCall_HasCheckPage_Birch
};

static u32 (*const sMatchCall_GetRematchTableIdxFunctions[])(match_call_t) = {
    MatchCall_GetRematchTableIdx_NPC,
    MatchCall_GetRematchTableIdx_Trainer,
    MatchCall_GetRematchTableIdx_Lyra,
    MatchCall_GetRematchTableIdx_Rival,
    MatchCall_GetRematchTableIdx_Birch
};

static void (*const sMatchCall_GetMessageFunctions[])(match_call_t, u8 *) = {
    MatchCall_GetMessage_NPC,
    MatchCall_GetMessage_Trainer,
    MatchCall_GetMessage_Lyra,
    MatchCall_GetMessage_Rival,
    MatchCall_GetMessage_Birch
};

static void (*const sMatchCall_GetNameAndDescFunctions[])(match_call_t, const u8 **, const u8 **) = {
    MatchCall_GetNameAndDesc_NPC,
    MatchCall_GetNameAndDesc_Trainer,
    MatchCall_GetNameAndDesc_Lyra,
    MatchCall_GetNameAndDesc_Rival,
    MatchCall_GetNameAndDesc_Birch
};

static const struct MatchCallCheckPageOverride sCheckPageOverrides[] = {
    {
        .idx = MC_HEADER_STEVEN,
        .facilityClass = FACILITY_CLASS_STEVEN,
        .flag = 0xFFFF,
        .flavorTexts = {
            [CHECK_PAGE_STRATEGY] = gText_MatchCallSteven_Strategy,
            [CHECK_PAGE_POKEMON]  = gText_MatchCallSteven_Pokemon,
            [CHECK_PAGE_INTRO_1]  = gText_MatchCallSteven_Intro1_BeforeMeteorFallsBattle,
            [CHECK_PAGE_INTRO_2]  = gText_MatchCallSteven_Intro2_BeforeMeteorFallsBattle
        }
    },
    {
        .idx = MC_HEADER_STEVEN,
        .facilityClass = FACILITY_CLASS_STEVEN,
        .flag = FLAG_DEFEATED_MOSSDEEP_GYM,
        .flavorTexts = {
            [CHECK_PAGE_STRATEGY] = gText_MatchCallSteven_Strategy,
            [CHECK_PAGE_POKEMON]  = gText_MatchCallSteven_Pokemon,
            [CHECK_PAGE_INTRO_1]  = gText_MatchCallSteven_Intro1_AfterMeteorFallsBattle,
            [CHECK_PAGE_INTRO_2]  = gText_MatchCallSteven_Intro2_AfterMeteorFallsBattle
        }
    },
    {
        .idx = MC_HEADER_ETHAN,
        .facilityClass = FACILITY_CLASS_ETHAN,
        .flag = 0xFFFF,
        .flavorTexts = MCFLAVOR(Ethan)
    },
    {
        .idx = MC_HEADER_MAY,
        .facilityClass = FACILITY_CLASS_MAY,
        .flag = 0xFFFF,
        .flavorTexts = MCFLAVOR(May)
    }
};

// .text

static u32 MatchCallGetFunctionIndex(match_call_t matchCall)
{
    switch (matchCall.common->type)
    {
        default:
        case MC_TYPE_NPC:
            return 0;
        case MC_TYPE_TRAINER:
        case MC_TYPE_LEADER:
            return 1;
        case MC_TYPE_LYRA:
            return 2;
        case MC_TYPE_RIVAL:
            return 3;
        case MC_TYPE_BIRCH:
            return 4;
    }
}

u32 GetTrainerIdxByRematchIdx(u32 rematchIdx)
{
    return gRematchTable[rematchIdx].trainerIds[0];
}

s32 GetRematchIdxByTrainerIdx(s32 trainerIdx)
{
    s32 rematchIdx;

    for (rematchIdx = 0; rematchIdx < REMATCH_TABLE_ENTRIES; rematchIdx++)
    {
        if (gRematchTable[rematchIdx].trainerIds[0] == trainerIdx)
            return rematchIdx;
    }
    return -1;
}

bool32 MatchCall_GetEnabled(u32 idx)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return FALSE;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    return sMatchCallGetEnabledFuncs[i](matchCall);
}

static bool32 MatchCall_GetEnabled_NPC(match_call_t matchCall)
{
    if (matchCall.npc->flag == 0xFFFF)
        return TRUE;
    return FlagGet(matchCall.npc->flag);
}

static bool32 MatchCall_GetEnabled_Trainer(match_call_t matchCall)
{
    if (matchCall.trainer->flag == 0xFFFF)
        return TRUE;
    return FlagGet(matchCall.trainer->flag);
}

static bool32 MatchCall_GetEnabled_Lyra(match_call_t matchCall)
{
    if (matchCall.lyra->flag == 0xFFFF)
        return TRUE;
    return FlagGet(matchCall.lyra->flag);
}

static bool32 MatchCall_GetEnabled_Rival(match_call_t matchCall)
{
    if (matchCall.rival->playerGender != gSaveBlock2Ptr->playerGender)
        return FALSE;
    if (matchCall.rival->flag == 0xFFFF)
        return TRUE;
    return FlagGet(matchCall.rival->flag);
}

static bool32 MatchCall_GetEnabled_Birch(match_call_t matchCall)
{
    return FlagGet(matchCall.birch->flag);
}

u8 MatchCall_GetMapSec(u32 idx)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return 0;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    return sMatchCallGetMapSecFuncs[i](matchCall);
}

static u8 MatchCall_GetMapSec_NPC(match_call_t matchCall)
{
    return matchCall.npc->mapSec;
}

static u8 MatchCall_GetMapSec_Trainer(match_call_t matchCall)
{
    return matchCall.trainer->mapSec;
}

static u8 MatchCall_GetMapSec_Lyra(match_call_t matchCall)
{
    s32 i;

    for (i = 0; matchCall.lyra->locationData[i].flag != 0xFFFF; i++)
    {
        if (!FlagGet(matchCall.lyra->locationData[i].flag))
            break;
    }
    return matchCall.lyra->locationData[i].mapSec;
}

static u8 MatchCall_GetMapSec_Rival(match_call_t matchCall)
{
    return MAPSEC_NONE;
}

static u8 MatchCall_GetMapSec_Birch(match_call_t matchCall)
{
    return MAPSEC_NONE;
}

bool32 MatchCall_IsRematchable(u32 idx)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return 0;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    return sMatchCall_IsRematchableFunctions[i](matchCall);
}

static bool32 MatchCall_IsRematchable_NPC(match_call_t matchCall)
{
    return FALSE;
}

static bool32 MatchCall_IsRematchable_Trainer(match_call_t matchCall)
{
    if (matchCall.trainer->rematchTableIdx >= REMATCH_ELITE_FOUR_ENTRIES)
        return FALSE;
    return gSaveBlock1Ptr->trainerRematches[matchCall.trainer->rematchTableIdx] ? TRUE : FALSE;
}

static bool32 MatchCall_IsRematchable_Lyra(match_call_t matchCall)
{
    return gSaveBlock1Ptr->trainerRematches[matchCall.lyra->rematchTableIdx] ? TRUE : FALSE;
}

static bool32 MatchCall_IsRematchable_Rival(match_call_t matchCall)
{
    return FALSE;
}

static bool32 MatchCall_IsRematchable_Birch(match_call_t matchCall)
{
    return FALSE;
}

bool32 MatchCall_HasCheckPage(u32 idx)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return FALSE;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    if (sMatchCall_HasCheckPageFunctions[i](matchCall))
        return TRUE;
    for (i = 0; i < ARRAY_COUNT(sCheckPageOverrides); i++)
    {
        if (sCheckPageOverrides[i].idx == idx)
            return TRUE;
    }
    return FALSE;
}

static bool32 MatchCall_HasCheckPage_NPC(match_call_t matchCall)
{
    return FALSE;
}

static bool32 MatchCall_HasCheckPage_Trainer(match_call_t matchCall)
{
    return TRUE;
}

static bool32 MatchCall_HasCheckPage_Lyra(match_call_t matchCall)
{
    return TRUE;
}

static bool32 MatchCall_HasCheckPage_Rival(match_call_t matchCall)
{
    return FALSE;
}

static bool32 MatchCall_HasCheckPage_Birch(match_call_t matchCall)
{
    return FALSE;
}

u32 MatchCall_GetRematchTableIdx(u32 idx)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return REMATCH_TABLE_ENTRIES;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    return sMatchCall_GetRematchTableIdxFunctions[i](matchCall);
}

static u32 MatchCall_GetRematchTableIdx_NPC(match_call_t matchCall)
{
    return REMATCH_TABLE_ENTRIES;
}

static u32 MatchCall_GetRematchTableIdx_Trainer(match_call_t matchCall)
{
    return matchCall.trainer->rematchTableIdx;
}

static u32 MatchCall_GetRematchTableIdx_Lyra(match_call_t matchCall)
{
    return matchCall.lyra->rematchTableIdx;
}

static u32 MatchCall_GetRematchTableIdx_Rival(match_call_t matchCall)
{
    return REMATCH_TABLE_ENTRIES;
}

static u32 MatchCall_GetRematchTableIdx_Birch(match_call_t matchCall)
{
    return REMATCH_TABLE_ENTRIES;
}

void MatchCall_GetMessage(u32 idx, u8 *dest)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    sMatchCall_GetMessageFunctions[i](matchCall, dest);
}

static void MatchCall_GetMessage_NPC(match_call_t matchCall, u8 *dest)
{
    MatchCall_BufferCallMessageText(matchCall.npc->textData, dest);
}

// This is the one functional difference between MC_TYPE_TRAINER and MC_TYPE_LEADER
static void MatchCall_GetMessage_Trainer(match_call_t matchCall, u8 *dest)
{
    if (matchCall.common->type != MC_TYPE_LEADER)
        MatchCall_BufferCallMessageText(matchCall.trainer->textData, dest);
    else
        MatchCall_BufferCallMessageTextByRematchTeam(matchCall.leader->textData, matchCall.leader->rematchTableIdx, dest);
}

static void MatchCall_GetMessage_Lyra(match_call_t matchCall, u8 *dest)
{
    MatchCall_BufferCallMessageText(matchCall.lyra->textData, dest);
}

static void MatchCall_GetMessage_Rival(match_call_t matchCall, u8 *dest)
{
    MatchCall_BufferCallMessageText(matchCall.rival->textData, dest);
}

static void MatchCall_GetMessage_Birch(match_call_t matchCall, u8 *dest)
{
    BufferPokedexRatingForMatchCall(dest);
}

static void MatchCall_BufferCallMessageText(const match_call_text_data_t *textData, u8 *dest)
{
    u32 i;
    for (i = 0; textData[i].text != NULL; i++)
        ;
    if (i)
        i--;
    while (i)
    {
        if (textData[i].flag != 0xFFFF && FlagGet(textData[i].flag) == TRUE)
            break;
        i--;
    }
    if (textData[i].flag2 != 0xFFFF)
        FlagSet(textData[i].flag2);
    StringExpandPlaceholders(dest, textData[i].text);
}

static void MatchCall_BufferCallMessageTextByRematchTeam(const match_call_text_data_t *textData, u16 idx, u8 *dest)
{
    u32 i;
    for (i = 0; textData[i].text != NULL; i++)
    {
        if (textData[i].flag == 0xFFFE)
            break;
        if (textData[i].flag != 0xFFFF && !FlagGet(textData[i].flag))
            break;
    }
    if (textData[i].flag != 0xFFFE)
    {
        if (i)
            i--;
        if (textData[i].flag2 != 0xFFFF)
            FlagSet(textData[i].flag2);
        StringExpandPlaceholders(dest, textData[i].text);
    }
    else
    {
        if (FlagGet(FLAG_SYS_GAME_CLEAR))
        {
            do
            {
                if (gSaveBlock1Ptr->trainerRematches[idx])
                    i += 2;
                else if (CountBattledRematchTeams(idx) >= 2)
                    i += 3;
                else
                    i++;
            } while (0);
        }

        StringExpandPlaceholders(dest, textData[i].text);
    }
}

void MatchCall_GetNameAndDesc(u32 idx, const u8 **desc, const u8 **name)
{
    match_call_t matchCall;
    u32 i;

    if (idx >= ARRAY_COUNT(sMatchCallHeaders))
        return;
    matchCall = sMatchCallHeaders[idx];
    i = MatchCallGetFunctionIndex(matchCall);
    sMatchCall_GetNameAndDescFunctions[i](matchCall, desc, name);
}

static void MatchCall_GetNameAndDesc_NPC(match_call_t matchCall, const u8 **desc, const u8 **name)
{
    *desc = matchCall.npc->desc;
    *name = matchCall.npc->name;
}

static void MatchCall_GetNameAndDesc_Trainer(match_call_t matchCall, const u8 **desc, const u8 **name)
{
    match_call_t _matchCall = matchCall;
    if (_matchCall.trainer->name == NULL)
        MatchCall_GetNameAndDescByRematchIdx(_matchCall.trainer->rematchTableIdx, desc, name);
    else
        *name = _matchCall.trainer->name;
    *desc = _matchCall.trainer->desc;
}

static void MatchCall_GetNameAndDesc_Lyra(match_call_t matchCall, const u8 **desc, const u8 **name)
{
    MatchCall_GetNameAndDescByRematchIdx(matchCall.lyra->rematchTableIdx, desc, name);
    *desc = matchCall.lyra->desc;
}

static void MatchCall_GetNameAndDesc_Rival(match_call_t matchCall, const u8 **desc, const u8 **name)
{
    *desc = matchCall.rival->desc;
    *name = matchCall.rival->name;
}

static void MatchCall_GetNameAndDesc_Birch(match_call_t matchCall, const u8 **desc, const u8 **name)
{
    *desc = matchCall.birch->desc;
    *name = matchCall.birch->name;
}

static void MatchCall_GetNameAndDescByRematchIdx(u32 idx, const u8 **desc, const u8 **name)
{
    const struct Trainer *trainer = gTrainers + GetTrainerIdxByRematchIdx(idx);
    *desc = gTrainerClassNames[trainer->trainerClass];
    *name = trainer->trainerName;
}

const u8 *MatchCall_GetOverrideFlavorText(u32 idx, u32 offset)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sCheckPageOverrides); i++)
    {
        if (sCheckPageOverrides[i].idx == idx)
        {
            for (; i + 1 < ARRAY_COUNT(sCheckPageOverrides) &&
                sCheckPageOverrides[i + 1].idx == idx &&
                FlagGet(sCheckPageOverrides[i + 1].flag); i++);
            return sCheckPageOverrides[i].flavorTexts[offset];
        }
    }
    return NULL;
}

int MatchCall_GetOverrideFacilityClass(u32 idx)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sCheckPageOverrides); i++)
    {
        if (sCheckPageOverrides[i].idx == idx)
            return sCheckPageOverrides[i].facilityClass;
    }
    return -1;
}

bool32 MatchCall_HasRematchId(u32 idx)
{
    int i;

    for (i = 0; i < (int)ARRAY_COUNT(sMatchCallHeaders); i++)
    {
        u32 id = MatchCall_GetRematchTableIdx(i);
        if (id != REMATCH_TABLE_ENTRIES && id == idx)
            return TRUE;
    }
    return FALSE;
}

void SetMatchCallRegisteredFlag(void)
{
    int r0 = GetRematchIdxByTrainerIdx(gSpecialVar_0x8004);
    if (r0 >= 0)
        FlagSet(FLAG_MATCH_CALL_REGISTERED + r0);
}
