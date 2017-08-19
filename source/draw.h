#pragma once

#include "game.h"

void initScreens(void);
void initTextures(void);
void drawGrid(HexPiece * grid, Team * teams, bool whitesTurn, unsigned int selectedToken);
