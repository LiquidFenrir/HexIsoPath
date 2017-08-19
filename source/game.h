#pragma once

#include "pp2d/pp2d.h"

#define homeRowSize 4 //each color (white and black) has a home row
#define gridOneSideLayers 3 //doesn't include the middle layer where both sides meet, but includes the home row
#define totalRowsCount ((gridOneSideLayers*2) +1)

extern unsigned int gridCompleteSize;

#define FIRST_WHITE_HOME_ROW_PIECE 0
#define LAST_WHITE_HOME_ROW_PIECE (homeRowSize-1)

#define FIRST_BLACK_HOME_ROW_PIECE (gridCompleteSize-homeRowSize)
#define LAST_BLACK_HOME_ROW_PIECE (gridCompleteSize-1)

#define OFFSET_FIRST_MIDDLE_ROW_PIECE 10
#define OFFSET_LAST_MIDDLE_ROW_PIECE 10

#define Y_OFFSET 32

typedef enum {
	TEXTURE_BOT_HEX = 1,
	TEXTURE_MID_HEX,
	TEXTURE_TOP_HEX,
	TEXTURE_WHITE_TOKEN,
	TEXTURE_BLACK_TOKEN,
	TEXTURE_HEX_SELECTOR,
	TEXTURE_HEX_POSSIBLE,
} TextureID;

enum HexPieceSides {
	HEX_SIDE_TOP_LEFT = 0,
	HEX_SIDE_TOP_RIGHT,
	HEX_SIDE_MID_RIGHT,
	HEX_SIDE_BOT_RIGHT,
	HEX_SIDE_BOT_LEFT,
	HEX_SIDE_MID_LEFT
};

enum TeamsColor {
	TEAM_WHITE = 0,
	TEAM_BLACK
};

typedef struct HexPiece HexPiece;
typedef struct GameToken GameToken;

struct HexPiece {
	TextureID texture; //also works as level
	HexPiece * neighbors[6];
	GameToken * above;
	int yPos, xPos;
};

struct GameToken {
	TextureID color; //also works as team
	HexPiece * under;
	bool surrounded;
	bool removed;
	bool finished;
};

typedef struct {
	GameToken tokens[homeRowSize];
} Team;

void initGridSize(void);
void initGame(HexPiece * grid, Team * teams);
