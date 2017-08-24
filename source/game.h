#pragma once

#include "pp2d/pp2d/pp2d.h"

// #define CONSOLEMODE 1 //un/comment at will to toggle debug mode

#ifdef CONSOLEMODE
	#define DEBUGPOS(...) \
		printf("%s (line %d)...\n", __func__, __LINE__); \
		DEBUG(__VA_ARGS__)
#else
	#define DEBUGPOS(...) \
		fprintf(stderr, "%s (line %d)...\n", __func__, __LINE__); \
		DEBUG(__VA_ARGS__)
#endif

#ifdef CONSOLEMODE
	#define DEBUG(...) printf(__VA_ARGS__)
#else
	#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#define homeRowSize 4 //each color (white and black) has a home row
#define gridOneSideLayers 3 //doesn't include the middle layer where both sides meet, but includes the home row
#define totalRowsCount ((gridOneSideLayers*2) +1)

extern unsigned int gridCompleteSize;
extern int visibilityMode;

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
	TEXTURE_THREATENED_TOKEN,
	TEXTURE_HEX_CURRENT,
	TEXTURE_HEX_POSSIBLE,
	TEXTURE_HEX_SELECTED,
	TEXTURE_TOP_BORDER,
	TEXTURE_BOTTOM_BORDER,
	TEXTURE_SIDE_BORDER,
} TextureID;

typedef enum {
	HEX_SIDE_TOP_LEFT = 0,
	HEX_SIDE_TOP_RIGHT,
	HEX_SIDE_MID_RIGHT,
	HEX_SIDE_BOT_RIGHT,
	HEX_SIDE_BOT_LEFT,
	HEX_SIDE_MID_LEFT
} HexPieceSides;

typedef enum {
	MODE_MOVE_HEX = 0,
	MODE_MOVE_TOKEN,
} MoveMode;

typedef enum {
	TEAM_BLACK = 0,
	TEAM_WHITE
} TeamsColor;

typedef enum {
	VISIBILITY_EXISTS = 0,
	VISIBILITY_EMPTY,
	VISIBILITY_LEVEL,
} Visibility;

typedef struct HexPiece HexPiece;
typedef struct GameToken GameToken;
typedef struct Team Team;

struct HexPiece {
	TextureID texture; //also works as level
	HexPiece * neighbors[6];
	GameToken * above;
	int yPos, xPos;
	unsigned int offset;
};

struct GameToken {
	TextureID color; //also works as team
	HexPiece * under;
	bool threatened;
	bool captured;
	Team * team;
};

struct Team {
	TeamsColor color;
	GameToken tokens[homeRowSize];
	bool winner;
};

void initGridSize(void);
void initGame(HexPiece * grid, Team * teams);
int moveToken(GameToken * token, HexPieceSides direction);
int moveHex(HexPiece * sourceHex, HexPiece * destinationHex, TeamsColor team);
void handleThreats(Team * teams);
