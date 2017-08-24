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
	pp2d_load_texture_png(TEXTURE_THREATENED_TOKEN, "romfs:/token_threatened.png");
	
	pp2d_load_texture_png(TEXTURE_BOT_HEX, "romfs:/hex_bot.png");
	pp2d_load_texture_png(TEXTURE_MID_HEX, "romfs:/hex_mid.png");
	pp2d_load_texture_png(TEXTURE_TOP_HEX, "romfs:/hex_top.png");
	
	pp2d_load_texture_png(TEXTURE_HEX_POSSIBLE, "romfs:/hex_possible.png");
	pp2d_load_texture_png(TEXTURE_HEX_CURRENT, "romfs:/hex_current.png");
	pp2d_load_texture_png(TEXTURE_HEX_SELECTED, "romfs:/hex_selected.png");
	
	pp2d_load_texture_png(TEXTURE_TOP_BORDER, "romfs:/white_top_border.png");
	pp2d_load_texture_png(TEXTURE_BOTTOM_BORDER, "romfs:/black_bottom_border.png");
	pp2d_load_texture_png(TEXTURE_SIDE_BORDER, "romfs:/gray_side_border.png");
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
	if (token.threatened)
		pp2d_draw_texture(TEXTURE_THREATENED_TOKEN, token.under->xPos, token.under->yPos);
}

void drawHex(HexPiece hex)
{
	pp2d_draw_texture(hex.texture, hex.xPos, hex.yPos);
}

void drawGrid(HexPiece * grid, Team * teams, TeamsColor team, int selectedToken, HexPieceSides selectedSide, HexPiece * sourceHex, HexPiece * destinationHex)
{
	unsigned int hexCountPerRow[totalRowsCount];
	
	hexCountPerRow[0] = homeRowSize;  //white home row
	hexCountPerRow[totalRowsCount-1] = homeRowSize; //black home row
	
	for (unsigned int i = 1; i < ((totalRowsCount-1)/2)+1; i++) {
		hexCountPerRow[i] = homeRowSize+i;
		hexCountPerRow[totalRowsCount-i-1] = homeRowSize+i;
	}
	
	unsigned int offset = 0;
	// WHITE HOME ROW
	for (; offset < homeRowSize-1; offset++) {
		pp2d_draw_texture(TEXTURE_TOP_BORDER, grid[offset].xPos+16, grid[offset].yPos-8);
		drawHex(grid[offset]);
	}
	drawHex(grid[offset]);
	offset++;
	
	// ALL IN BETWEEN
	unsigned int row = 1;
	for (; row < (totalRowsCount-1)/2; row++) {
		pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset].xPos-16, grid[offset].yPos-24, NONE);
		for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
			drawHex(grid[offset]);
			offset++;
		}
		pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset-1].xPos+16, grid[offset-1].yPos-24, HORIZONTAL);
	}
	
	pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset].xPos-16, grid[offset].yPos-24, NONE);
	pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset].xPos-16, grid[offset].yPos+24, VERTICAL);
	//middle row, with the teleports
	for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
		drawHex(grid[offset]);
		offset++;
	}
	pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset-1].xPos+16, grid[offset-1].yPos-24, HORIZONTAL);
	pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset-1].xPos+16, grid[offset-1].yPos+24, BOTH);
	row++;
	
	for (; row < totalRowsCount-1; row++) {
		pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset].xPos-16, grid[offset].yPos+24, VERTICAL);
		for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
			drawHex(grid[offset]);
			offset++;
		}
		pp2d_draw_texture_flip(TEXTURE_SIDE_BORDER, grid[offset-1].xPos+16, grid[offset-1].yPos+24, BOTH);
	}
	
	// BLACK HOME ROW
	for (; offset < gridCompleteSize-1; offset++) {
		pp2d_draw_texture(TEXTURE_BOTTOM_BORDER, grid[offset].xPos+16, grid[offset].yPos+24);
		drawHex(grid[offset]);
	}
	drawHex(grid[offset]);
	
	//draw every token ON TOP of the hexes, and the selector and neighbors if needed
	for (int token = 0; token < homeRowSize; token++) {
		if (!teams[TEAM_WHITE].tokens[token].captured) {
			drawToken(teams[TEAM_WHITE].tokens[token]);
			if (selectedToken == token && team == TEAM_WHITE)
				drawCurrent(teams[TEAM_WHITE].tokens[token].under, selectedSide);
		}
		
		if (!teams[TEAM_BLACK].tokens[token].captured) {
			drawToken(teams[TEAM_BLACK].tokens[token]);
			if (selectedToken == token && team == TEAM_BLACK)
				drawCurrent(teams[TEAM_BLACK].tokens[token].under, selectedSide);
		}
	}
	
	if (sourceHex != NULL) {
		drawPossibleNeighbor(sourceHex, true);
	}
	if (destinationHex != NULL) {
		drawPossibleNeighbor(destinationHex, false);
	}
}
