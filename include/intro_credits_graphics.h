#ifndef GUARD_INTRO_CREDITS_GRAPHICS_H
#define GUARD_INTRO_CREDITS_GRAPHICS_H

// States for gIntroCredits_MovingSceneryState
enum {
    INTROCRED_SCENERY_NORMAL,
    INTROCRED_SCENERY_DESTROY,
    INTROCRED_SCENERY_FROZEN,
};

// Scenes for the Credits sequence
enum {
    SCENE_OCEAN_MORNING,
    SCENE_OCEAN_SUNSET,
    SCENE_FOREST_RIVAL_ARRIVE,
    SCENE_FOREST_CATCH_RIVAL,
    SCENE_CITY_NIGHT,
};

extern u16 gIntroCredits_MovingSceneryVBase;
extern s16 gIntroCredits_MovingSceneryVOffset;
extern s16 gIntroCredits_MovingSceneryState;

extern const struct CompressedSpriteSheet gSpriteSheet_IntroEthan[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroMay[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroBicycle[];
extern const struct CompressedSpriteSheet gSpriteSheet_IntroFlygon[];
extern const struct SpritePalette gSpritePalettes_IntroPlayerFlygon[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsEthan[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsMay[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsBicycle[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalEthan[];
extern const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[];
extern const struct SpritePalette gSpritePalettes_Credits[];

void LoadIntroPart2Graphics(u8 scene);
void SetIntroPart2BgCnt(u8 a);
void LoadCreditsSceneGraphics(u8);
void SetCreditsSceneBgCnt(u8);
u8 CreateBicycleBgAnimationTask(u8 a, u16 b, u16 c, u16 d);
void CycleSceneryPalette(u8);
u8 CreateIntroEthanSprite(s16 x, s16 y);
u8 CreateIntroMaySprite(s16 x, s16 y);
u8 CreateIntroFlygonSprite(s16 x, s16 y);

#endif // GUARD_INTRO_CREDITS_GRAPHICS_H
