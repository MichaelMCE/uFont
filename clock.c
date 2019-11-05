

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <wtypes.h>
#include <time.h>

#include <mylcd.h>
#include <demos.h>

#include "libs/ufont.h"
#include "libs/ufont_primitives.h"



//#define FONTFILE		"uf_digits/nanosecondthickbrk100.uf"		// 320x240
//#define FONTFILE		"uf/break84.uf"		// 320x240
#define FONTFILE		"uf_digits/ravebold180.uf"



static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}


static inline int getCharWidth (_ufont_t *font, uint8_t ch)
{
	int width = 0;
	uint16_t text[] = {ch, 0};
	fontGetMetricsList(font, text, &width, NULL);
	return width;
}


// find the smallest rect that will fit '88:88:88'
static inline void calcClockRect (_ufont_t *font, int *width, int *height)
{
	int maxW = 0;
	int maxH = 0;
	
	for (int i = '0'; i <= '9'; i++){
		uint8_t text[] = {i, i, ':', i, i, ':', i, i, 0};
		fontGetMetrics(font, text, width, height);
		//printf("getClockRect: %i %i\n", *width, *height);
		
		if (*width > maxW) maxW = *width;
		if (*height > maxH) maxH = *height;
	}

	*width = maxW;
	*height = maxH;
}

int main (int argc, char **argv)
{

	if (!initDemoConfig("K:\\code\\ufont\\config.cfg"))
		return 0;

	char *fontFile = FONTFILE;
	if (argc == 2) fontFile = argv[1];
	printf("File: '%s'\n", fontFile);

	//lSleep(5000);

	_ufont_t font;
	if (!fontOpen(&font, fontFile))
		return 0;
		
	// 16bpp (565) distination [frame] buffer
	// this is what we're sending to the display
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	memset(buffer, 0, sizeof(buffer));

	_ufont_surface_t *texture = fontCreateTexture("K:\\code\\ufont\\images/carbon2.png");
	fontSetDisplayBuffer(&font, buffer, DWIDTH, DHEIGHT);
	fontSetGlyphPadding(&font, 0);

	char timestr[12];
	int width = 0;
	int height = 0;
	calcClockRect(&font, &width, &height);
	//printf("::%i %i\n", width, height);

	_ufont_surface_t *surface = fontCreateSurface(width, height, COLOUR_24TO16(0x777777), NULL);
	fontSetRenderSurface(&font, surface);
	fontSetRenderFlags(&font, BFONT_RENDER_ADVANCE_X/*|BFONT_RENDER_BOUNDRECT| BFONT_RENDER_GLYPHRECT*//*|BFONT_RENDER_CONDENSED*/);

	while (1){
		clearFrame(buffer, COLOUR_CREAM);
		fontCleanSurface(&font, NULL);
		//fontDrawRectangle(&font, 0, 0, surface->width-1, surface->height-1, 1);
	
		time_t t = time(0);
		struct tm *tdate = localtime(&t);
		strftime(timestr, sizeof(timestr), "%H:%M:%S", tdate);
		fontGetMetrics(&font, (uint8_t*)timestr, &width, &height);
		
		int x = abs(surface->width - width)/2;
		int y = abs(surface->height - height)/2;
		//y = -font.header.fontDescent;
		fontPrint(&font, &x, &y, (uint8_t*)timestr);
		
		x = (DWIDTH - surface->width)/2;
		y = (DHEIGHT - surface->height)/2;
		fontApplySurfaceTexture(&font, texture, x, y);
		fontApplySurfaceOutline(&font, x, y);
		
		lUpdate(hw, buffer, sizeof(buffer));
		Sleep(500);
	}
	
	fontTextureFree(texture);
	fontSurfaceFree(surface);
	fontClose(&font);

	demoCleanup();

	return 1;
}

