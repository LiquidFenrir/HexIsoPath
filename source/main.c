#include "draw.h"

int main() {

	romfsInit();
	pp2d_init();
	
	initScreens();
	initTextures();
	
	u32 blackTextColor = RGBA8(0, 0, 0, 0xFF);
	u32 whiteTextColor = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	
	initGridSize();
	HexPiece grid[gridCompleteSize];
	memset(grid, 0, sizeof(HexPiece)*gridCompleteSize);
	Team teams[homeRowSize];
	memset(teams, 0, sizeof(Team)*homeRowSize);
	initGame(grid, teams);
		
	bool whitesTurn = true;
	int selectedToken = 0;
	
	while (aptMainLoop()) {
		hidScanInput();

		pp2d_begin_draw(GFX_TOP);
		
		if (whitesTurn)
			pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, whiteTextColor, "It's white's turn!");
		else
			pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, blackTextColor, "It's black's turn!");
		
		pp2d_draw_on(GFX_BOTTOM);
		
		drawGrid(grid, teams, whitesTurn, selectedToken);
		
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;
		else if (kDown & KEY_L) {
			selectedToken--;
			if (selectedToken < 0)
				selectedToken = homeRowSize-1;
		}
		else if (kDown & KEY_R) {
			selectedToken++;
			if (selectedToken >= homeRowSize)
				selectedToken = 0;
		}
		else if (kDown & KEY_A) {
			whitesTurn = !whitesTurn;
			selectedToken = 0;
		}
		
		pp2d_end_draw();
	}
	
	pp2d_end_draw();

	pp2d_exit();
	romfsExit();
	
	return 0;
}
