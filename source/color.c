#include "colors.h"

u32 aboveHexColor(u32 currentColor)
{
	switch (currentColor) {
		case COLOR_HEX_BOT:
			return COLOR_HEX_MID;
		case COLOR_HEX_MID:
			return COLOR_HEX_TOP;
	}
	
	return 0;
}

u32 underHexColor(u32 currentColor)
{
	switch (currentColor) {
		case COLOR_HEX_TOP:
			return COLOR_HEX_MID;
		case COLOR_HEX_MID:
			return COLOR_HEX_BOT;
	}
	
	return 0;
}
