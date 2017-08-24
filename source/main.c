#include "draw.h"
#include "uds.h"

int main() {
	udsInit(0x3000, NULL);
	romfsInit();
	
	#ifdef CONSOLEMODE
		gfxInitDefault();
		consoleInit(GFX_TOP, NULL);
	#else
		consoleDebugInit(debugDevice_SVC);
		
		pp2d_init();
		initScreens();
		initTextures();
	#endif
	
	u32 blackTextColor = RGBA8(0, 0, 0, 0xFF);
	u32 whiteTextColor = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
	u32 redTextColor = RGBA8(0xBB, 0, 0, 0xFF);
	
	initGridSize();
	HexPiece grid[gridCompleteSize];
	memset(grid, 0, sizeof(HexPiece)*gridCompleteSize);
	Team teams[homeRowSize];
	memset(teams, 0, sizeof(Team)*homeRowSize);
	initGame(grid, teams);
	
	bool server = false;
	bool networkPlay = false;
	TeamsColor networkTeam = TEAM_WHITE;
	
	bool whitesTurn = true;
	bool gameRunning = true;
	int selectedToken = -1;
	int selectedSide = HEX_SIDE_TOP_LEFT;
	
	MoveMode mode = MODE_MOVE_HEX;
	
	bool movedHex = false;
	bool movedToken = false;
	
	int offset = homeRowSize;
	HexPiece * sourceHex = &grid[offset];
	bool selectedSource = false;
	HexPiece * destinationHex = NULL;
	int ret = 0;
	
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		
		#ifndef CONSOLEMODE
		pp2d_begin_draw(GFX_TOP);
		
		pp2d_draw_text_center(GFX_TOP, 128, 0.6f, 0.6f, redTextColor, "Press START to quit!");
		
		if (teams[TEAM_WHITE].winner) {
			pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, whiteTextColor, "White team won!");
			gameRunning = false;
		}
		else if (teams[TEAM_BLACK].winner) {
			pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, blackTextColor, "Black team won!");
			gameRunning = false;
		}
		else {
			if (whitesTurn)
				pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, whiteTextColor, "It's white's turn!");
			else
				pp2d_draw_text_center(GFX_TOP, 100, 1.0f, 1.0f, blackTextColor, "It's black's turn!");
		}
		
		if (networkPlay) {
			if (server)
				pp2d_draw_text(8, SCREEN_HEIGHT-24, 0.6f, 0.6f, whiteTextColor, "Server");
			else
				pp2d_draw_text(8, SCREEN_HEIGHT-24, 0.6f, 0.6f, blackTextColor, "Client");
			
			//if it's your turn to play, send the keys you press
			if (networkTeam == (whitesTurn ? TEAM_WHITE : TEAM_BLACK)) {
				u32 sendKeys = kDown & ~KEY_START;
				sendData(&sendKeys, sizeof(u32));
			}
			//if it's not your turn, receive the keys being pressed by the other
			else {
				u32 receivedKeys = 0;
				size_t receivedSize = 0;
				receiveData(&receivedKeys, sizeof(u32), &receivedSize);
				kDown = receivedKeys | (kDown & KEY_START);
			}
			//only exception is START key to allow quitting at any time
		}
		
		pp2d_draw_on(GFX_BOTTOM);
		
		drawGrid(grid, teams, whitesTurn ? TEAM_WHITE : TEAM_BLACK, selectedToken, (HexPieceSides)selectedSide, sourceHex, destinationHex);
		#endif
		
		if (kDown & KEY_START)
			break;
		else if (!gameRunning); //don't allow inputs other than quitting once a team has won
		else if (kDown & KEY_L && mode == MODE_MOVE_TOKEN) {
			selectedToken--;
			if (selectedToken < 0)
				selectedToken = homeRowSize-1;
		}
		else if (kDown & KEY_R  && mode == MODE_MOVE_TOKEN) {
			selectedToken++;
			if (selectedToken >= homeRowSize)
				selectedToken = 0;
		}
		else if (kDown & (KEY_A | KEY_Y)) {
			if (kDown & KEY_A) {
				if (mode == MODE_MOVE_TOKEN) {
					ret = moveToken(&teams[whitesTurn].tokens[selectedToken], (HexPieceSides)selectedSide);
					if (ret == 0) {
						if (movedHex) {
							movedToken = false;
							movedHex = false;
							whitesTurn = !whitesTurn;
							handleThreats(teams);
							mode = MODE_MOVE_HEX;
							selectedToken = 0;
						}
						else {
							movedToken = true;
						}
					}
				}
				else if (mode == MODE_MOVE_HEX) {
					if (selectedSource) {
						ret = moveHex(sourceHex, destinationHex, whitesTurn ? TEAM_WHITE : TEAM_BLACK);
						if (ret == 0) {
							if (movedToken) {
								movedToken = false;
								movedHex = false;
								whitesTurn = !whitesTurn;
								handleThreats(teams);
								mode = MODE_MOVE_HEX;
								selectedToken = 0;
							}
							else {
								movedHex = true;
							}
							selectedSource = false;
						}
					}
					else {
						TeamsColor team = whitesTurn ? TEAM_WHITE : TEAM_BLACK;
						
						//dont even allow them to select their home row hexes to save time
						if (team == TEAM_BLACK && sourceHex->offset >= FIRST_BLACK_HOME_ROW_PIECE)
							continue;
						
						if (team == TEAM_WHITE && sourceHex->offset <= LAST_WHITE_HOME_ROW_PIECE)
							continue;
						
						selectedSource = true;
						destinationHex = &grid[offset];
					}
				}
			}
			if (ret == 0) {
				switch (mode) {
					case MODE_MOVE_TOKEN:
						if (!movedHex) {
							mode = MODE_MOVE_HEX;
							offset = homeRowSize;
							sourceHex = &grid[offset];
							destinationHex = NULL;
							selectedToken = -1;
							selectedSide = HEX_SIDE_TOP_LEFT;
						}
						break;
					case MODE_MOVE_HEX:
						if (!movedToken && !selectedSource) {
							mode = MODE_MOVE_TOKEN;
							offset = homeRowSize;
							sourceHex = NULL;
							destinationHex = NULL;
							selectedToken = 0;
							selectedSide = HEX_SIDE_TOP_LEFT;
						}
						break;
				}
			}
			ret = 0;
		}
		else if (kDown & KEY_B) {
			if (mode == MODE_MOVE_HEX && selectedSource) {
				offset = homeRowSize;
				sourceHex = &grid[offset];
				destinationHex = NULL;
				selectedSource = false;
			}
		}
		else if (kDown & KEY_X) {
			switch (visibilityMode) {
				case VISIBILITY_EXISTS:
					visibilityMode = VISIBILITY_LEVEL;
					break;
				case VISIBILITY_LEVEL:
					visibilityMode = VISIBILITY_EMPTY;
					break;
				case VISIBILITY_EMPTY:
					visibilityMode = VISIBILITY_EXISTS;
					break;
			}
		}
		else if (kDown & KEY_LEFT) {
			if (mode == MODE_MOVE_TOKEN) {
				selectedSide--;
				if (selectedSide < HEX_SIDE_TOP_LEFT)
					selectedSide = HEX_SIDE_MID_LEFT;
			}
			else if (mode == MODE_MOVE_HEX) {
				offset--;
				if (offset < FIRST_WHITE_HOME_ROW_PIECE)
					offset = LAST_BLACK_HOME_ROW_PIECE;
				if (selectedSource) {
					destinationHex = &grid[offset];
				}
				else {
					sourceHex = &grid[offset];
				}
			}
		}
		else if (kDown & KEY_RIGHT) {
			if (mode == MODE_MOVE_TOKEN) {
				selectedSide++;
				if (selectedSide > HEX_SIDE_MID_LEFT)
					selectedSide = HEX_SIDE_TOP_LEFT;
			}
			else if (mode == MODE_MOVE_HEX) {
				offset++;
				if (offset > (int)LAST_BLACK_HOME_ROW_PIECE)
					offset = FIRST_WHITE_HOME_ROW_PIECE;
				
				if (selectedSource) {
					destinationHex = &grid[offset];
				}
				else {
					sourceHex = &grid[offset];
				}
			}
		}
		else if (kDown & KEY_UP && !networkPlay) {
			//the server is the white team
			server = true;
			ret = initComm(server);
			DEBUG("initComm server: %.8x\n", ret);
			if (R_SUCCEEDED(ret)) {
				networkPlay = true;
				networkTeam = TEAM_WHITE;
			}
		}
		else if (kDown & KEY_DOWN && !networkPlay) {
			//the client is the black team
			server = false;
			ret = initComm(server);
			DEBUG("initComm client: %.8x\n", ret);
			if (R_SUCCEEDED(ret)) {
				networkPlay = true;
				networkTeam = TEAM_BLACK;
			}
		}
		
		#ifdef CONSOLEMODE
			gfxFlushBuffers();
			gfxSwapBuffers();
			gspWaitForVBlank();
		#else
			pp2d_end_draw();
		#endif
	}

	if (networkPlay)
		exitComm(server);
	
	#ifdef CONSOLEMODE
		gfxExit();
	#else
		pp2d_end_draw();
		pp2d_exit();
	#endif
	
	romfsExit();
	udsExit();
	
	return 0;
}
