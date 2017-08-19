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
	pp2d_load_texture_png(TEXTURE_HEX_SELECTOR, "romfs:/hex_selector.png");
}

void drawPossibleNeighbor(HexPiece * possibleHex)
{
	pp2d_draw_texture(TEXTURE_HEX_POSSIBLE, possibleHex->xPos, possibleHex->yPos);
}

void drawSelector(HexPiece * selectedHex)
{
	for (int i = 0; i < 6; i++) {
		if (selectedHex->neighbors[i] != NULL && //if there is even a neighbor to be chosen
		    selectedHex->neighbors[i]->texture == selectedHex->texture && //if it's on the same level as the current selected hex
		    selectedHex->neighbors[i]->above == NULL //if there isn't a token already on it
		) {
			drawPossibleNeighbor(selectedHex->neighbors[i]);
		}
	}
	pp2d_draw_texture(TEXTURE_HEX_SELECTOR, selectedHex->xPos, selectedHex->yPos);
}

void drawToken(GameToken token)
{
	pp2d_draw_texture(token.color, token.under->xPos, token.under->yPos);
}

void drawHex(HexPiece hex)
{
	pp2d_draw_texture(hex.texture, hex.xPos, hex.yPos);
}

void drawGrid(HexPiece * grid, Team * teams, bool whitesTurn, unsigned int selectedToken)
{
	//draw every single hex
	for (unsigned int offset = 0; offset < gridCompleteSize; offset++) {
		drawHex(grid[offset]);
	}
	
	//draw every token ON TOP of the hexes, and the selector and neighbors if needed
	for (unsigned int token = 0; token < homeRowSize; token++) {
		drawToken(teams[TEAM_WHITE].tokens[token]);
		if (selectedToken == token && whitesTurn)
			drawSelector(teams[TEAM_WHITE].tokens[token].under);
		
		drawToken(teams[TEAM_BLACK].tokens[token]);
		if (selectedToken == token && !whitesTurn)
			drawSelector(teams[TEAM_BLACK].tokens[token].under);
	}
}
