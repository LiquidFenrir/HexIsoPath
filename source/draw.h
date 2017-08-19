#pragma once

#include "game.h"

void initScreens(void);
void initTextures(void);
void drawGrid(HexPiece * grid, Team * teams, TeamsColor team, int selectedToken, HexPieceSides selectedSide, HexPiece * sourceHex, HexPiece * destinationHex);
