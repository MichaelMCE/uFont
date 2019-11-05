

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>


#include "libs/ufont.h"
#include "libs/fonts.h"


static inline const char *stristr (const char *string, const char *pattern)
{
	if (!string || !pattern)
		return NULL;
	
	char *pptr, *sptr;
	
	for (char *start = (char*)string; *start != 0; start++){
		for ( ; ((*start != 0) && (toupper(*start) != toupper(*pattern))); start++){
		}
		
		if (!*start) return NULL;

		pptr = (char*)pattern;
		sptr = (char*)start;

		while (toupper(*sptr) == toupper(*pptr)){
			sptr++;
			pptr++;
			if (!*pptr)
				return (start);
		}
	}
	return NULL;
}
/*
wchar_t *stristrw (const wchar_t *string, const wchar_t *pattern)
{
	if (!string || !pattern)
		return NULL;
	
	wchar_t *pptr, *sptr, *start;
	
	for (start = (wchar_t*)string; *start != 0; start++){
		for ( ; ((*start != 0) && (toupper(*start) != toupper(*pattern))); start++){
		}
		
		if (!*start) return NULL;

		pptr = (wchar_t*)pattern;
		sptr = (wchar_t*)start;

		while (toupper(*sptr) == toupper(*pptr)){
			sptr++;
			pptr++;
			if (!*pptr)
				return (start);
		}
	}
	return NULL;
}*/


const char *utilFontFind (const char *name)
{
	int i = 0;
	
	do{
		if (stristr(fonts[i], name))
			return fonts[i];
	}while(fonts[++i]);

	return NULL;
}
