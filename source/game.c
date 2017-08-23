#include "game.h"

int visibilityMode = VISIBILITY_EXISTS;
unsigned int gridCompleteSize = 0;

void initGridSize(void)
{
	for (unsigned int i = 0; i < gridOneSideLayers; i++) {
		gridCompleteSize += homeRowSize;
		gridCompleteSize += i;
	}
	gridCompleteSize *= 2; //because 2 sides
	gridCompleteSize += homeRowSize;
	gridCompleteSize += gridOneSideLayers;
}

void initGame(HexPiece * grid, Team * teams)
{
	unsigned int hexCountPerRow[totalRowsCount];
	
	hexCountPerRow[0] = homeRowSize;  //white home row
	hexCountPerRow[totalRowsCount-1] = homeRowSize; //black home row
	
	for (unsigned int i = 1; i < ((totalRowsCount-1)/2)+1; i++) {
		hexCountPerRow[i] = homeRowSize+i;
		hexCountPerRow[totalRowsCount-i-1] = homeRowSize+i;
	}
	
	int xPosFromHex[gridCompleteSize];

	unsigned int offset = 0;
	for (unsigned int row = 0; row < totalRowsCount; row++) {
		int xPos = (320 - homeRowSize - (hexCountPerRow[row]*32))/2; //the bottom screen is 320 pixels wide
		for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
			xPosFromHex[offset] = xPos;
			xPos += 32;
			offset++;
		}
	}
	
	teams[TEAM_WHITE].color = TEAM_WHITE;
	teams[TEAM_BLACK].color = TEAM_BLACK;
	
	offset = 0;
	// WHITE HOME ROW
	for (offset = 0; offset < homeRowSize; offset++) {
		GameToken * currentToken = &teams[TEAM_WHITE].tokens[offset];
		currentToken->team = &teams[TEAM_WHITE];
		
		currentToken->color = TEXTURE_WHITE_TOKEN;
		grid[offset].texture = TEXTURE_TOP_HEX;
		
		currentToken->under = &grid[offset];
		grid[offset].above = currentToken;
		
		grid[offset].yPos = Y_OFFSET;
		grid[offset].xPos = xPosFromHex[offset];
		
		grid[offset].offset = offset;
		
		if (offset == FIRST_WHITE_HOME_ROW_PIECE) {
			grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = &grid[LAST_WHITE_HOME_ROW_PIECE];
			grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
		}
		else if (offset == LAST_WHITE_HOME_ROW_PIECE) {
			grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = &grid[FIRST_WHITE_HOME_ROW_PIECE];
			grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
		}
		else {
			grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
			grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
		}
		
		//common to all
		grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = &grid[LAST_WHITE_HOME_ROW_PIECE+offset+1];
		grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = &grid[LAST_WHITE_HOME_ROW_PIECE+offset+2];
	}
	
	// ALL IN BETWEEN
	unsigned int row = 1;
	for (; row < (totalRowsCount-1)/2; row++) {
		for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
			grid[offset].texture = TEXTURE_MID_HEX;
			grid[offset].yPos = Y_OFFSET+ (24*row);
			grid[offset].xPos = xPosFromHex[offset];
			grid[offset].offset = offset;
			
			if (hex != hexCountPerRow[row]-1) {
				grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
				grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = &grid[offset-hexCountPerRow[row-1]];
			}
			if (hex != 0) {
				grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = &grid[offset-hexCountPerRow[row-1]-1];
				grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
			}
			
			grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = &grid[offset+hexCountPerRow[row]+1];
			grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = &grid[offset+hexCountPerRow[row]];
			
			offset++;
		}
	}
	
	//middle row, with the teleports
	for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
		grid[offset].texture = TEXTURE_MID_HEX;
		grid[offset].yPos = Y_OFFSET+ (24*row);
		grid[offset].xPos = xPosFromHex[offset];
		grid[offset].offset = offset;
		
		grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = &grid[offset-hexCountPerRow[row-1]-1];
		grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = &grid[offset-hexCountPerRow[row-1]];
		
		grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
		grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = &grid[offset+hexCountPerRow[row]];
		
		grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
		grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = &grid[offset+hexCountPerRow[row]-1];
		
		if (hex == hexCountPerRow[row]-1) {
			grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = NULL;
			grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset-hexCountPerRow[row]+1];
			grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = NULL;
		}
		if (hex == 0) {
			grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = NULL;
			grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset+hexCountPerRow[row]-1];
			grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = NULL;
		}
		
		offset++;
	}
	row++;
	
	for (; row < totalRowsCount-1; row++) {
		for (unsigned int hex = 0; hex < hexCountPerRow[row]; hex++) {
			grid[offset].texture = TEXTURE_MID_HEX;
			grid[offset].yPos = Y_OFFSET+ (24*row);
			grid[offset].xPos = xPosFromHex[offset];
			grid[offset].offset = offset;
			
			grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = &grid[offset-hexCountPerRow[row-1]];
			grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = &grid[offset-hexCountPerRow[row-1]+1];
			
			if (hex != hexCountPerRow[row]-1) {
				grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
				grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = &grid[offset+hexCountPerRow[row]];
			}
			if (hex != 0) {
				grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
				grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = &grid[offset+hexCountPerRow[row]-1];
			}
			
			offset++;
		}
	}
	
	// BLACK HOME ROW
	for (; offset < gridCompleteSize; offset++) {
		GameToken * currentToken = &teams[TEAM_BLACK].tokens[offset-FIRST_BLACK_HOME_ROW_PIECE];
		currentToken->team = &teams[TEAM_BLACK];
		
		currentToken->color = TEXTURE_BLACK_TOKEN;
		grid[offset].texture = TEXTURE_BOT_HEX;
		
		currentToken->under = &grid[offset];
		grid[offset].above = currentToken;
		
		grid[offset].yPos = Y_OFFSET+ (24*(totalRowsCount-1));
		grid[offset].xPos = xPosFromHex[offset];
		
		grid[offset].offset = offset;
		
		if (offset == FIRST_BLACK_HOME_ROW_PIECE) {
			grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
			grid[offset].neighbors[HEX_SIDE_BOT_LEFT] = &grid[LAST_BLACK_HOME_ROW_PIECE];;
		}
		else if (offset == LAST_BLACK_HOME_ROW_PIECE) {
			grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
			grid[offset].neighbors[HEX_SIDE_BOT_RIGHT] = &grid[FIRST_BLACK_HOME_ROW_PIECE];;
		}
		else {
			grid[offset].neighbors[HEX_SIDE_MID_LEFT] = &grid[offset-1];
			grid[offset].neighbors[HEX_SIDE_MID_RIGHT] = &grid[offset+1];
		}
		
		//common to all
		grid[offset].neighbors[HEX_SIDE_TOP_LEFT] = &grid[offset-homeRowSize-1];
		grid[offset].neighbors[HEX_SIDE_TOP_RIGHT] = &grid[offset-homeRowSize];
	}
}

int moveToken(GameToken * token, HexPieceSides direction)
{
	if (token->captured)
		return -1;
	
	HexPiece * nextHex = token->under->neighbors[direction];
	
	//can't move if there's nothing there
	if (nextHex == NULL)
		return -1;
	
	//can't move there if there's already a token there
	if (nextHex->above != NULL)
		return -1;
	
	//can't move there if it's not at the right height
	if (nextHex->texture != (token->team->color == TEAM_WHITE ? TEXTURE_TOP_HEX : TEXTURE_BOT_HEX))
		return -1;
	
	//the tokens can't return to their home row but they can move inside it if they haven't left it
	if (token->team->color == TEAM_WHITE && token->under->offset > LAST_WHITE_HOME_ROW_PIECE && nextHex->offset <= LAST_WHITE_HOME_ROW_PIECE)
		return -1;
	
	if (token->team->color == TEAM_BLACK && token->under->offset < FIRST_BLACK_HOME_ROW_PIECE && nextHex->offset >= FIRST_BLACK_HOME_ROW_PIECE)
		return -1;
	
	token->under->above = NULL; //remove the token from the previous hex
	token->under = nextHex; //put it on the new one
	token->under->above = token; //tell the new one the token is above it
	
	//check for victory (reminder: winning is getting a token to the home row of the other team)
	if (token->team->color == TEAM_WHITE && token->under->offset >= FIRST_BLACK_HOME_ROW_PIECE)
		token->team->winner = true;
	
	if (token->team->color == TEAM_BLACK && token->under->offset <= LAST_WHITE_HOME_ROW_PIECE)
		token->team->winner = true;
	
	return 0;
}

int moveHex(HexPiece * sourceHex, HexPiece * destinationHex, TeamsColor team)
{
	//can't if they're the same
	if (destinationHex == sourceHex)
		return -1;
	
	//can't do anything if there is a token on them
	if (destinationHex->above != NULL || sourceHex->above != NULL)
		return -1;
	
	//can't if the source is at the bottom or the destination is at the top
	if (destinationHex->texture == TEXTURE_TOP_HEX || sourceHex->texture == TEXTURE_BOT_HEX)
		return -1;
	
	//each team is not allowed to modified its own home row
	if (team == TEAM_WHITE && (destinationHex->offset <= LAST_WHITE_HOME_ROW_PIECE || sourceHex->offset <= LAST_WHITE_HOME_ROW_PIECE))
		return -1;
	
	if (team == TEAM_BLACK && (destinationHex->offset >= FIRST_BLACK_HOME_ROW_PIECE || sourceHex->offset >= FIRST_BLACK_HOME_ROW_PIECE))
		return -1;
	
	destinationHex->texture++;
	sourceHex->texture--;
	
	return 0;
}

void handleThreats(Team * teams)
{
	for (int team = TEAM_WHITE; team >= TEAM_BLACK; team--) {
		Team * currentTeam = &teams[team];
		for (int token = 0; token < homeRowSize; token++) {
			GameToken * currentToken = &currentTeam->tokens[token];
			
			if (currentToken->captured)
				continue;
			
			HexPiece * hexUnderToken = currentToken->under;
			int neighborsCount = 0;
			for (int side = HEX_SIDE_TOP_LEFT; side <= HEX_SIDE_MID_LEFT; side++) {
				HexPiece * neighborHex = hexUnderToken->neighbors[side];
				if (neighborHex != NULL) { //if there is even a neighbor to check
					GameToken * neighborToken = neighborHex->above;
					if (neighborToken != NULL) { //if the neighbor hex has a token on top
						if (neighborToken->color != currentToken->color) { //if the token is from the other team
							neighborsCount++;
						}
					}
				}
			}
			
			if (neighborsCount >= 2) {
				if (currentToken->threatened) {
					currentToken->captured = true;
					hexUnderToken->above = NULL;
					currentToken->under = NULL;
				}
				else {
					currentToken->threatened = true;
				}
			}
			else {
				//clear the threatened flag if it moved into a safe spot
				currentToken->threatened = false;
			}
		}
	}
}
