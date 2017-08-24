#pragma once

#include "pp2d/pp2d/pp2d.h"

#define COLOR_RED (u32)RGBA8(0xFF, 0, 0, 0xFF)
#define COLOR_GREEN (u32)RGBA8(0, 0xFF, 0, 0xFF)
#define COLOR_BLUE (u32)RGBA8(0, 0, 0xFF, 0xFF)

#define COLOR_BLACK (u32)RGBA8(0, 0, 0, 0xFF)
#define COLOR_WHITE (u32)RGBA8(0xFF, 0xFF, 0xFF, 0xFF)

#define COLOR_HEX_CURRENT COLOR_RED
#define COLOR_HEX_POSSIBLE COLOR_BLUE
#define COLOR_HEX_SELECTED COLOR_GREEN | COLOR_RED //yellow

#define COLOR_HEX_BOT (u32)RGBA8(0, 0xAA, 0, 0xFF) //darker green
#define COLOR_HEX_MID (u32)RGBA8(0, 0xCC, 0, 0xFF) //dark green
#define COLOR_HEX_TOP (u32)RGBA8(0, 0xFF, 0, 0xFF) //green

#define COLOR_TOKEN_BLACK COLOR_BLACK
#define COLOR_TOKEN_WHITE COLOR_WHITE

#define COLOR_TOKEN_THREATENED COLOR_RED

u32 aboveHexColor(u32 currentColor);
u32 underHexColor(u32 currentColor);
