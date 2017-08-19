#include "draw.h"

void initScreens(void)
{
	u32 grayBGColor = ABGR8(0xFF, 0x88, 0x88, 0x88);
	
	pp2d_set_screen_color(GFX_TOP, grayBGColor);
	pp2d_set_screen_color(GFX_BOTTOM, grayBGColor);
}

void initTextures(void)
{
	pp2d_load_texture_png(TEXTURE_WHITE_TOKEN, "romfs:/white_token.png");
	pp2d_load_texture_png(TEXTURE_BLACK_TOKEN, "romfs:/black_token.png");
	
	pp2d_load_texture_png(TEXTURE_BOT_HEX, "romfs:/hex_bot.png");
	pp2d_load_texture_png(TEXTURE_MID_HEX, "romfs:/hex_mid.png");
	pp2d_load_texture_png(TEXTURE_TOP_HEX, "romfs:/hex_top.png");
	
	pp2d_load_texture_png(TEXTURE_HEX_POSSIBLE, "romfs:/hex_possible.png");
	pp2d_load_texture_png(TEXTURE_HEX_CURRENT, "romfs:/hex_current.png");
	pp2d_load_texture_png(TEXTURE_HEX_SELECTED, "romfs:/hex_selected.png");
	
	pp2d_load_texture_png(TEXTURE_GAME_BOARD, "romfs:/game_board.png");
}

void drawPossibleNeighbor(HexPiece * possibleHex, bool selected)
{
	pp2d_draw_texture(selected ? TEXTURE_HEX_SELECTED : TEXTURE_HEX_POSSIBLE, possibleHex->xPos, possibleHex->yPos);
}

void drawCurrent(HexPiece * selectedHex, HexPieceSides selectedSide)
{
	for (int i = 0; i < 6; i++) {
		if ((selectedHex->neighbors[i] != NULL || !(visibilityMode >= VISIBILITY_EXISTS)) && //if there is even a neighbor to be chosen
		    (selectedHex->neighbors[i]->texture == selectedHex->texture || !(visibilityMode >= VISIBILITY_LEVEL)) && //if it's on the same level as the current selected hex
		    (selectedHex->neighbors[i]->above == NULL || !(visibilityMode >= VISIBILITY_EMPTY)) //if there isn't a token already on it
		) {
			drawPossibleNeighbor(selectedHex->neighbors[i], selectedSide == i);
		}
	}
	pp2d_draw_texture(TEXTURE_HEX_CURRENT, selectedHex->xPos, selectedHex->yPos);
}

void drawToken(GameToken token)
{
	pp2d_draw_texture(token.color, token.under->xPos, token.under->yPos);
}

void drawHex(HexPiece hex)
{
	pp2d_draw_texture(hex.texture, hex.xPos, hex.yPos);
}

void drawGrid(HexPiece * grid, Team * teams, TeamsColor team, int selectedToken, HexPieceSides selectedSide, HexPiece * sourceHex, HexPiece * destinationHex)
{
	pp2d_draw_texture(TEXTURE_GAME_BOARD, 0, 0);
	
	//draw every single hex
	for (unsigned int offset = 0; offset < gridCompleteSize; offset++) {
		drawHex(grid[offset]);
	}
	
	//draw every token ON TOP of the hexes, and the selector and neighbors if needed
	for (int token = 0; token < homeRowSize; token++) {
		drawToken(teams[TEAM_WHITE].tokens[token]);
		if (selectedToken == token && team == TEAM_WHITE)
			drawCurrent(teams[TEAM_WHITE].tokens[token].under, selectedSide);
		
		drawToken(teams[TEAM_BLACK].tokens[token]);
		if (selectedToken == token && team == TEAM_BLACK)
			drawCurrent(teams[TEAM_BLACK].tokens[token].under, selectedSide);
	}
	
	if (sourceHex != NULL) {
		drawPossibleNeighbor(sourceHex, true);
	}
	if (destinationHex != NULL) {
		drawPossibleNeighbor(destinationHex, false);
	}
}
