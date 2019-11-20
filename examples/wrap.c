
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



#include "common.h"

//#define fontFile	UFDIR"unicode36.uf"
//#define fontFile	UFDIR"10x20.uf"
#define fontFile	UFDIR"utopia_b24.uf"
//#define fontFile	UFDIR"lucida18.uf";
//#define fontFile	UFDIR"unifont32.uf"
//#define fontFile	UFDIR"freesans32.uf"


#if 1
static inline _ufont_surface_t *fontGetRenderSurface (_ufont_t *font)
{
	return font->render.surface;
}

static inline void breakStringFree (uint8_t **txt)
{
	if (!txt) return;
	
	for (int i = 0; txt[i]; i++)
		free(txt[i]);
	free(txt);
}

static inline int isWordBreak (const char ch)
{
	if (ch == ' ' || ch == ',' || ch == '.' || ch == '-' || ch == '|' || ch == '\\' || ch == '\"' || ch == ':' || ch == '/' || ch == '+' || ch == '(' || ch == ')'/* || ch == 12288*/)
		return 1;
	else
		return 0;
}
	
static inline uint8_t **breakString (const uint8_t *text)
{
	if (!text || !text[0]) return NULL;
		
	int strTotal = 8;
	int x1 = 0;
	int x2 = 0;
	int wIdx = 0;
	uint8_t **wrapped = calloc(1+strTotal, sizeof(char*));


	while (1){
		if (text[x2] && isWordBreak(text[x2])){
			int len = (x2 - x1) + 1;
	
			wrapped[wIdx] = calloc(len+1, sizeof(char));
			strncpy((char*)wrapped[wIdx], (char*)&text[x1], len);
			wrapped[wIdx][len] = 0;
			//printf("%i, %i\n", x2, wrapped[wIdx][len-1]);

			if (++wIdx >= strTotal-1){
				strTotal <<= 1;
				wrapped = realloc(wrapped, (1+strTotal) * sizeof(char*));
			}
			x1 = x2 + 0;
		}
		x2++;

		if (!text[x2]){
			int len = (x2 - x1) + 1;
			if (!len) break;
			wrapped[wIdx] = calloc(len+1, sizeof(char));
			strncpy((char*)wrapped[wIdx], (char*)&text[x1], len);
			wrapped[wIdx++][len] = 0;
			break;
		}
	}

	if (!wIdx){
		free(wrapped);
		return NULL;
	}else{
		wrapped[wIdx] = NULL;
		return wrapped;
	}
}

static inline void _fontPrint (_ufont_t *font, int *x, int *y, const uint8_t *str)
{

	int desX = *x;

	const uint16_t userFlags = fontGetRenderFlags(font);
	const uint16_t wrapFlags = BFONT_RENDER_BOUNDRECT|BFONT_RENDER_ADVANCE_X; //&~(BFONT_RENDER_ADVANCE_Y|BFONT_RENDER_WORDWRAP);
	fontSetRenderFlags(font, wrapFlags);

	int maxWidth = DWIDTH;
	//int maxHeight = DHEIGHT;
	
	_ufont_surface_t *surface = fontGetRenderSurface(font);
	if (!(font->render.flags&BFONT_RENDER_METRICS)){
		maxWidth = surface->width;
		//maxHeight = surface->height;
	}
	
	
	uint8_t **txt = breakString(str);
	if (txt){
		for (int i = 0; txt[i]; i++){
			int width, height;
			//printf("%i: '%s'\n", i, txt[i]);

			fontGetMetrics(font, txt[i], &width, &height);
			

			if ((*x+width-1) > maxWidth-1){
				*x = desX;
				*y += height;
			}
			if (!fontPrint(font, x, y, txt[i]))
				break;
		}
		breakStringFree(txt);
	}

	fontSetRenderFlags(font, userFlags);
}
#endif

int main (int argc, char **argv)
{

	if (!initDemoConfig("config.cfg"))
		return 0;


	uint8_t pixels[CALC_PITCH_1(DWIDTH) * DHEIGHT];
	memset(pixels, 0, sizeof(pixels));
	// this is where we'll render our text/pixels on to
	// supply a 1BPP buffer or pass NULL, one will then be created (alloc'd).
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0x777777), pixels);
	_ufont_t font;
	
	char *ufont = fontFile;
	if (argc == 2) ufont = argv[1];
	
	if (!fontOpen(&font, ufont)){
		printf("fontOpen() failed for '%s'\n", ufont);
		return 0;
	}
			
	printf("font '%s'\n", ufont);

	//fontSetGlyphPadding(&font, fontGetGlyphPadding(&font)+1);

	uint8_t *displayOutputBuffer = frame->pixels;
	fontSetDisplayBuffer(&font, displayOutputBuffer, DWIDTH, DHEIGHT);
	fontSetRenderSurface(&font, surface);
	fontSetRenderFlags(&font, BFONT_RENDER_NEWLINE|BFONT_RENDER_RETURN|BFONT_RENDER_NODEFAULT|BFONT_RENDER_ADVANCE_X|BFONT_RENDER_ADVANCE_Y /*|BFONT_RENDER_GLYPHRECT*/|BFONT_RENDER_BOUNDRECT);



	
	const uint8_t *str = (const uint8_t*)"This program is free software; you can redistribute it and/or "
	 "modify it under the terms of the GNU LIBRARY GENERAL PUBLIC LICENSE "
	 "as published by the Free Software Foundation; either version 2 "
	 "of the License, or (at your option) any later version.";


	int width, height;
	fontGetMetrics(&font, (uint8_t*)str, &width, &height);
	printf("metrics WxH: %i,%i\n", width, height);
	

	int y = 5;
	int x = 50;

	fontDrawLine(&font, x, 0, x, DHEIGHT-1, 1);

	fontPrint(&font, &x, &y, str);
	printf("print WxH: %i,%i\n", x, y-fontGetLineSpace(&font));

#if 1	
	int desX = 50;
	x = desX;
	y += 50;
	
	fontSetRenderFlags(&font,BFONT_RENDER_WORDWRAP|BFONT_RENDER_NEWLINE|BFONT_RENDER_NODEFAULT|BFONT_RENDER_ADVANCE_X|BFONT_RENDER_ADVANCE_Y /*|BFONT_RENDER_GLYPHRECT*//*|BFONT_RENDER_BOUNDRECT*/);
	_fontPrint(&font, &x, &y, str);
	printf("printWrap WxH: %i,%i\n", x, y);	
#endif
	fontApplySurface(&font, 0, 0);
	fontSurfaceFree(surface);
	fontClose(&font);

	lRefresh(frame);
	Sleep(200000);

	demoCleanup();
	return 1;
}


