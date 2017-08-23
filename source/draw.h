#pragma once

#include "game.h"

void initScreens(void);
void initTextures(void);
void drawGrid(HexPiece * grid, Team * teams, TeamsColor currentTeam, int selectedToken, HexPieceSides selectedSide, HexPiece * sourceHex, HexPiece * destinationHex);
