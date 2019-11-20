
//  Copyright (c) Michael McElligott
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU LIBRARY GENERAL PUBLIC LICENSE for details.




#include "uf.h"

#define DWIDTH		320
#define DHEIGHT		240







static const char *strtext = "Hello !dlrow ";




static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

void setup ()
{
	_ufont_t font;
	const char *fontPath = utilFontFind("london38");
	if (!fontOpen(&font, fontPath))
		return;
		
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	memset(buffer, 0, sizeof(buffer));
	
	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);

	//_ufont_palette_t *pal = fontPaletteLoad("palettes/pal1.png");

	// create a 1BPP work surface. this is the initial work surface
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0xFFBF33), NULL);
	
	// make this the active surface
	fontSetRenderSurface(&font, surface);
	
	
	int x = 10, y = 10;
	
	// draw text on to the active surface
	fontPrint(&font, &x, &y, strtext);

	
	x = DWIDTH/2;
	y = DHEIGHT/2;
	double incr = 360.0f / (float)(strlen(strtext));
	double length = 100.0f;
	int width;
	int height;
	char text[2] = {0, 0};

	for (float a = 90.0f; a < (3.0f*360.0f)+90.0f; a += 0.7f){
		int ct = 0;
		clearFrame(buffer, COLOUR_BLACK);
		fontCleanSurface(&font, NULL);

		for (float curang = -a; (curang < 360.0f - a) && strtext[ct]; curang += incr){
			float angle = curang * (2.0f * 3.14159265f) / 360.0f;
			float x1 = cosf(angle) * length + x;
			float y1 = sinf(angle) * length + y;

			text[0] = strtext[ct++];
			fontGetMetrics(&font, text, &width, &height);

			int _x = x1 - (width/2);
			int _y = y1 - (height/2);
			fontPrint(&font, &_x, &_y, text);
		}

		//fontApplySurfacePalette(&font, pal, 0, 0);
		fontApplySurface(&font, 0, 0);
		// send 'buffer' to display
		//sleep(15);
	}
	
	// render surface on to display buffer
	//fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	//fontPaletteFree(pal);
	
	fontClose(&font);
	
}

void loop ()
{

}


