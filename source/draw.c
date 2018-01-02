#include "draw.h"

void initScreens(void)
{
    u32 grayBGColor = ABGR8(0xFF, 0x88, 0x88, 0x88);

    pp2d_set_screen_color(GFX_TOP, grayBGColor);
    pp2d_set_screen_color(GFX_BOTTOM, grayBGColor);
}

void initTextures(void)
{
    pp2d_load_texture_png(TEXTURE_SIDE_BORDER, "romfs:/side_border.png");

    pp2d_load_texture_png(TEXTURE_HEX, "romfs:/hex.png");
    pp2d_load_texture_png(TEXTURE_HEX_OUTER, "romfs:/hex_outer.png");

    pp2d_load_texture_png(TEXTURE_TOKEN, "romfs:/token.png");
    pp2d_load_texture_png(TEXTURE_TOKEN_OUTER, "romfs:/token_outer.png");
}

void drawPossibleNeighbor(HexPiece * possibleHex, bool selected)
{
    pp2d_draw_texture_blend(TEXTURE_HEX_OUTER, possibleHex->xPos, possibleHex->yPos, selected ? COLOR_HEX_SELECTED : COLOR_HEX_POSSIBLE);
}

void drawCurrent(HexPiece * selectedHex, HexPieceSides selectedSide)
{
    for (int i = 0; i < 6; i++) {
        if ((selectedHex->neighbors[i] != NULL || !(visibilityMode >= VISIBILITY_EXISTS)) && //if there is even a neighbor to be chosen
            (selectedHex->neighbors[i]->color == selectedHex->color || !(visibilityMode >= VISIBILITY_LEVEL)) && //if it's on the same level as the current selected hex
            (selectedHex->neighbors[i]->above == NULL || !(visibilityMode >= VISIBILITY_EMPTY)) //if there isn't a token already on it
        ) {
            drawPossibleNeighbor(selectedHex->neighbors[i], selectedSide == i);
        }
    }
    pp2d_draw_texture_blend(TEXTURE_HEX_OUTER, selectedHex->xPos, selectedHex->yPos, COLOR_HEX_CURRENT);
}

void drawToken(GameToken token)
{
    pp2d_draw_texture_blend(TEXTURE_TOKEN, token.under->xPos, token.under->yPos, token.color);
    if (token.threatened)
        pp2d_draw_texture_blend(TEXTURE_TOKEN_OUTER, token.under->xPos, token.under->yPos, COLOR_TOKEN_THREATENED);
}

void drawHex(HexPiece hex)
{
    pp2d_draw_texture_blend(TEXTURE_HEX, hex.xPos, hex.yPos, hex.color);
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
        pp2d_texture_select_part(TEXTURE_HEX, grid[offset].xPos+16, grid[offset].yPos-8, 0, 16, 32, 16);
        pp2d_texture_blend(COLOR_WHITE);
        pp2d_texture_draw();
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
        pp2d_texture_select_part(TEXTURE_HEX, grid[offset].xPos+16, grid[offset].yPos+24, 0, 0, 32, 16);
        pp2d_texture_blend(COLOR_BLACK);
        pp2d_texture_draw();
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
