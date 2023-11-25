
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


// compile from "uFont/examples/" directory

#include "common.h"


#define FONT24		UFDIR"consola24.uf"
#define FONT38		UFDIR"76london38.uf"
#define FONT50		UFDIR"76london50.uf"
#define FONT80		UFDIR"76london80.uf"
#define FONT_DELAY	UFDIR"ravebold110.uf"


#define TFT_WIDTH	240
#define TFT_HEIGHT	240

#define TUNING_STANDARD		440.0f
#define NOTE_OCTAVES		6		// number of octaves covered

static _ufont_t font38;
static _ufont_t font50;
static _ufont_t font80;
static _ufont_t font24;
static _ufont_t fontDelay;

enum _notes {
	NOTE_C = 0,
	NOTE_C_SHARP,
	NOTE_D,
	NOTE_D_SHARP,
	NOTE_E,
	NOTE_F,
	NOTE_F_SHARP,
	NOTE_G,
	NOTE_G_SHARP,
	NOTE_A,
	NOTE_A_SHARP,
	NOTE_B
};

typedef struct {
	uint8_t note;
	char label[3];
	float freq[NOTE_OCTAVES];
}notetable_t;

static notetable_t notetable[12] = {
 {NOTE_C,       "C", {}},
 {NOTE_C_SHARP, "C#", {}},
 {NOTE_D,       "D", {}},
 {NOTE_D_SHARP, "D#", {}},
 {NOTE_E,       "E", {}},
 {NOTE_F,       "F", {}},
 {NOTE_F_SHARP, "F#", {}},
 {NOTE_G,       "G", {}},
 {NOTE_G_SHARP, "G#", {}},
 {NOTE_A,       "A", {}},
 {NOTE_A_SHARP, "A#", {}},
 {NOTE_B,       "B", {}},
};



static inline void clearFrame (void *buffer, const uint16_t colour)
{
	uint16_t *pixels = (uint16_t*)buffer;
	
	int tPixels = DWIDTH * DHEIGHT;
	while (tPixels--) pixels[tPixels] = colour;		
}

void printnotes ()
{
	for (int o = 0; o < NOTE_OCTAVES; o++){
		printf(" Octave %i\n", o);
		for (int n = 0; n < 12; n++){
			float freq = notetable[n].freq[o];
			char *label = notetable[n].label;
			printf("%s - %.4f\n", label, freq);
		}
		printf("\n");
	}
}

static const char *noteToLabel (const uint8_t note)
{
	return notetable[note].label;
}

static const float noteToFreq (const uint8_t note, const uint8_t octave)
{
	return notetable[note].freq[octave];
}

static float findNoteNearest (float freqIn, uint8_t *noteOut, uint8_t *octaveOut)
{
	float delta = 999999.0f;
	uint8_t note = -1;
	uint8_t octave = -1;
	
	for (int n = 0; n < 12; n++){
		for (int o = 0; o < NOTE_OCTAVES; o++){
			const float diff = fabsf(noteToFreq(n, o) - freqIn);
			if (diff < delta){
				delta = diff;
				note = n;
				octave = o;
			}
		}
	}
	
	if (note != -1){
		*noteOut = note;
		*octaveOut = octave;
	
		if (freqIn > noteToFreq(note, octave))
			return delta;
		else
			return -delta;
	}else{
		*noteOut = -1;
		return 0.0f;
	}
}

static void buildOctave (float pitchStandard, int octave)
{
	octave -= 4;
	if (octave < -4){
		return;
	}else if (octave < 0){
		for (int d = octave+1; d <= 0; d++)
			pitchStandard /= 2.0f;
	}else if (octave > 0){
		for (int d = 0; d < octave; d++)
			pitchStandard *= 2.0f;
	}

	int noteIdx = 11;
	
	// A# and B
	for (float n = 2; n >= 1; n -= 1.0f){
		float freq = pitchStandard * exp2f(n/12.0f);
		//printf("freq %.1f\n", freq);
		notetable[noteIdx--].freq[octave+4] = freq;
	}
		
	// C to A
	for (float n = 0; n <= 9; n += 1.0f){
		float freq = pitchStandard / exp2f(n/12.0f);
		//printf("freq %.1f\n", freq);
		notetable[noteIdx--].freq[octave+4] = freq;
	}
}

float findNoteNearestLower (uint8_t *note, uint8_t *octave)
{
	int8_t octaveLower = *octave;
	int8_t noteLower = *note - 1;
		
	if (noteLower < NOTE_C){
		noteLower = NOTE_B;
		if (--octaveLower < 0)
			noteLower = -1;
	}

	if (noteLower != -1){
		float freq = noteToFreq(noteLower, octaveLower);
		*note = noteLower;
		*octave = octaveLower;
		//printf("freq l %.1f, %i %i\n", freq, *note, *octave);
		return freq;
	}
	
	return 0.0f;
}

float findNoteNearestHigher (uint8_t *note, uint8_t *octave)
{
	int8_t octaveHigher = *octave;
	int8_t noteHigher = *note + 1;
	
	if (noteHigher > NOTE_B){
		noteHigher = NOTE_C;
		if (++octaveHigher >= NOTE_OCTAVES){
			octaveHigher = -1;
			noteHigher = -1;
		}
	}

	if (noteHigher != -1){
		float freq = noteToFreq(noteHigher, octaveHigher);
		*note = noteHigher;
		*octave = octaveHigher;
		//printf("freq h %.1f, %i %i\n", freq, *note, *octave);
		return freq;
	}

	return 0.0f;
}

static void buildTable (const float A4)
{
	for (int o = 0; o < NOTE_OCTAVES; o++){
		//printf("# %i\n", o);
		buildOctave(A4, o);
		//printf("\n");
	}
}

int drawTuner (_ufont_surface_t *surface, float notef, const float centScale)
{
	char notesText[16];
	int width;
	int height;
	
	
	// freq
	sprintf(notesText, "%.1f", notef);
	fontGetMetrics(&font38, (uint8_t*)notesText, &width, &height);
		
	int x = (abs(TFT_WIDTH - width)/2);
	int y = 15;
	fontPrint(&font38, &x, &y, (uint8_t*)notesText);
	
	if (notef < 10.0f || notef > 1500.0f)
		notef = 0.01;

	uint8_t noteNearest;
	uint8_t octaveNearest;
	float delta = findNoteNearest(notef, &noteNearest, &octaveNearest);
	float freqN = noteToFreq(noteNearest, octaveNearest);
	
	uint8_t noteOutL = noteNearest;
	uint8_t octaveOutL = octaveNearest;
	float freqL = findNoteNearestLower(&noteOutL, &octaveOutL);

	uint8_t noteOutH = noteNearest;
	uint8_t octaveOutH = octaveNearest;
	float freqH = findNoteNearestHigher(&noteOutH, &octaveOutH);
		
	float cent = 0.0f;
	if (delta > 0)
		cent = ((1.0f / (freqH - freqN)) * delta) * 100.0f;
	else if (delta < 0)
		cent = -((1.0f / (freqL - freqN)) * delta) * 100.0f;

	const int graphWidth = 230;
	float centX = (float)(graphWidth / centScale) * cent;
	centX += (graphWidth / 2.0f);
	centX += (TFT_WIDTH - graphWidth) / 2.0f;

	// note labels
	x = 4;
	y = 76;
	snprintf(notesText, sizeof(notesText), "%s", noteToLabel(noteOutL));
	fontPrint(&font50, &x, &y, (uint8_t*)notesText);

	snprintf(notesText, sizeof(notesText), "%s", noteToLabel(noteNearest));
	fontGetMetrics(&font80, (uint8_t*)notesText, &width, &height);
	x = ((TFT_WIDTH - width) / 2) - 2;
	y = 46;
	fontPrint(&font80, &x, &y, (uint8_t*)notesText);

	snprintf(notesText, sizeof(notesText), "%s", noteToLabel(noteOutH));
	fontGetMetrics(&font50, (uint8_t*)notesText, &width, &height);
	x = (TFT_WIDTH - width) - 4;
	y = 76;
	fontPrint(&font50, &x, &y, (uint8_t*)notesText);
	

	// horizontal mark
	x = (TFT_WIDTH - graphWidth) / 2;
	y = 132;
	surfaceDrawLine(surface, x, y, x+graphWidth, y, 1);
	
	// left mark
	surfaceDrawLine(surface, x, y-2, x, y+4, 1);
	surfaceDrawLine(surface, x+1, y-2, x+1, y+4, 1);

	// middle mark
	surfaceDrawLine(surface, x+(graphWidth/2)-1, y-2, x+(graphWidth/2)-1, y+4, 1);
	surfaceDrawLine(surface, x+(graphWidth/2)-0, y-2, x+(graphWidth/2)-0, y+4, 1);
	surfaceDrawLine(surface, x+(graphWidth/2)+1, y-2, x+(graphWidth/2)+1, y+4, 1);
		
	// right mark
	surfaceDrawLine(surface, x+graphWidth-1, y-2, x+graphWidth-1, y+4, 1);
	surfaceDrawLine(surface, x+graphWidth, y-2, x+graphWidth, y+4, 1);

	// cent labels
	snprintf(notesText, sizeof(notesText), "-%.0f", centScale/2.0f);
	x = 2;
	y = 132 + 6;
	fontPrint(&font24, &x, &y, (uint8_t*)notesText);

	snprintf(notesText, sizeof(notesText), "%.0f", centScale/2.0f);
	fontGetMetrics(&font24, (uint8_t*)notesText, &width, &height);
	x = (TFT_WIDTH - width) - 5;
	y = 132 + 6;
	fontPrint(&font24, &x, &y, (uint8_t*)notesText);

	// indicator
	x = 5;
	y = 132;
	if (fabsf(cent) > 4.0f)
		surfaceDrawTriangle(surface, x+(graphWidth/2)-13, TFT_HEIGHT-1, x+(graphWidth/2)+15, TFT_HEIGHT-1, centX, y+5, 1);
	else
		surfaceDrawTriangleFilled(surface, x+(graphWidth/2)-13, TFT_HEIGHT-1, x+(graphWidth/2)+15, TFT_HEIGHT-1, centX, y+5, 1);

	// cent
	int xOffset = x + (graphWidth * 0.16f);
	y = 166;
	snprintf(notesText, sizeof(notesText), "%.0f", fabsf(cent));
	fontPrint(&font38, &xOffset, &y, (uint8_t*)notesText);
	
	// octave
	xOffset = x + (graphWidth * 0.72f);
	y = 166;
	snprintf(notesText, sizeof(notesText), "%i", octaveNearest);
	fontPrint(&font38, &xOffset, &y, (uint8_t*)notesText);

	return 1;	
}

int drawDelay (_ufont_surface_t *surface, const int delayPeriod)
{
	
	int width = 0;
	int height = 0;
	char notesText[16];


	sprintf(notesText, "Delay");
	fontGetMetrics(&font24, (uint8_t*)notesText, &width, &height);
	int x = (abs(TFT_WIDTH - width) / 2) + 1;
	int y = 16;
	fontPrint(&font24, &x, &y, (uint8_t*)notesText);

	sprintf(notesText, "%i", delayPeriod);
	fontGetMetrics(&fontDelay, (uint8_t*)notesText, &width, &height);
		
	x = (abs(TFT_WIDTH - width)/2) - 2;
	y = (abs(TFT_HEIGHT - height)/2) - 4;
	fontPrint(&fontDelay, &x, &y, (uint8_t*)notesText);

	return 1;
}

int main (int argc, char **argv)
{
	if (!initDemoConfig("config.cfg"))
		return 0;


	buildTable(TUNING_STANDARD);

	const float notef = 41.0032f;
	const float centScale = 50.0f;
	const int delayPeriod = 525;


	if (!fontOpen(&font24, FONT24))
		return 0;	

	if (!fontOpen(&font38, FONT38))
		return 0;

	if (!fontOpen(&font50, FONT50))
		return 0;

	if (!fontOpen(&font80, FONT80))
		return 0;

	if (!fontOpen(&fontDelay, FONT_DELAY))
		return 0;

	// 16bpp (565) distination [frame] buffer
	// this is what we're sending to the display
	uint8_t buffer[CALC_PITCH_16(DWIDTH)*DHEIGHT];
	memset(buffer, 0, sizeof(buffer));

	fontSetDisplayBuffer(&font38, buffer, DWIDTH, DHEIGHT);
	fontSetDisplayBuffer(&font50, buffer, DWIDTH, DHEIGHT);
	fontSetDisplayBuffer(&font80, buffer, DWIDTH, DHEIGHT);
	fontSetDisplayBuffer(&font24, buffer, DWIDTH, DHEIGHT);
	fontSetDisplayBuffer(&fontDelay, buffer, DWIDTH, DHEIGHT);

	// create a 1BPP work surface. this is the initial work surface
	_ufont_surface_t *surface = fontCreateSurface(DWIDTH, DHEIGHT, COLOUR_24TO16(0xFFBF33), NULL);
	
	// make this the active surface
	fontSetRenderSurface(&font38, surface);
	fontSetRenderSurface(&font50, surface);
	fontSetRenderSurface(&font80, surface);
	fontSetRenderSurface(&font24, surface);
	fontSetRenderSurface(&fontDelay, surface);	

	//surfaceDrawRectangle(surface, 0, 0, 239, 239, 1);
	surfaceDrawCircle(surface, 120, 120, 120, 1);

	if (1)
		drawTuner(surface, notef, centScale);
	else
		drawDelay(surface, delayPeriod);


	fontApplySurface(&fontDelay, 7, 7);
	lUpdate(hw, buffer, sizeof(buffer));

	fontSurfaceFree(surface);
	fontClose(&font24);
	fontClose(&font38);
	fontClose(&font50);
	fontClose(&font80);
	fontClose(&fontDelay);
	

	Sleep(30000);
	demoCleanup();

	return 1;
}


