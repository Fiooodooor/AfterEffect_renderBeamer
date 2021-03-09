//#include <stdio.h>
#include <stdlib.h>
#include "/Users/ccc/Desktop/fontforge-master/builds/inc/fontforge-config.h"
#include <fontforge/fontforge.h>
#include "gf_font_generate.h"
#define ALLOWED_CHARACTERSA "_-0123456789ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz"
static EncMap *createMapCopy(EncMap *map);
static void leaveAllowedOnly(char *sourceStr);
extern int			doinitFontForgeMain(void);
extern SplineFont  *LoadSplineFont(const char *, enum openflags);
extern int			GenerateScript(SplineFont *, char *, const char *, int, int, char *, struct sflist *, EncMap *, NameList *, int);
extern void			SFFlatten(SplineFont **cidmaster);
extern char**		GetFontNames(const char*);

#include <dlfcn.h>


EncMap *createMapCopy(EncMap *map)
{
	EncMap *new;
	int n;

	if ((new = chunkalloc(sizeof(EncMap))) != NULL)
	{
		*new = *map;
		if ((new->map = malloc(map->enccount * sizeof(int32))) != NULL)
		{
			if ((new->backmap = malloc(map->backmax * sizeof(int32))) != NULL)
			{
				memcpy(new->map, map->map, map->enccount * sizeof(int32));
				memcpy(new->backmap, map->backmap, map->backmax * sizeof(int32));
				/* NOTE: This new->enc 'also' points to same map->enc. */
				if (map->remap == NULL)
					return(new);
				for (n = 0; map->remap[n].infont != -1; ++n);
				if ((new->remap = malloc(n * sizeof(struct remap))) != NULL)
				{
					memcpy(new->remap, map->remap, n * sizeof(struct remap));
					return(new);
				}
				free(new->backmap);
			}
			free(new->map);
		}
		free(new);
	}
	return(NULL);
}

int convert_font(const char* src_file, const char* output_path, const char* output_file, int font_nr, char* destFilePath)
{
    destFilePath[0]='\0';
    doinitFontForgeMain();
    SplineFont* font = (SplineFont*)LoadSplineFont(src_file, 1);
       
    if (font != NULL)
    {
        if(font->subfontcnt > 0)
        {
            EncMap *tmpMapCpy = createMapCopy(font->map);
            if(!tmpMapCpy)
                return -13;
            SFFlatten(&font);
            if(font->map == NULL)
                font->map = tmpMapCpy;
            else
                free(tmpMapCpy);
        }
        unsigned long len = 0;
        if(font->fontname && (len = strlen(font->fontname)) > 5) {
            char* tempFontName = malloc(sizeof(char)*(len+5));
            char fontExtension[5];
            strcpy(fontExtension, &output_file[strlen(output_file) - 4]);
            if(tempFontName!=NULL) {
                strcpy(tempFontName, font->fontname);
                leaveAllowedOnly(tempFontName);
                snprintf(destFilePath, LIB_MAXPATH_SIZE-1, "%s/%d_%s%s", output_path, font_nr, tempFontName, fontExtension);
                free(tempFontName);
            }
            else {
                snprintf(destFilePath, LIB_MAXPATH_SIZE-1, "%s/%d_%s%s", output_path, font_nr, font->fontname, fontExtension);
            }
        }
        else {
            snprintf(destFilePath, LIB_MAXPATH_SIZE-1, "%s/%d_%s", output_path, font_nr, output_file);
        }

        if(GenerateScript(font, destFilePath, NULL, 0, -1, NULL, NULL, font->map, NULL, 1)) {
            return 1;
        }
        else {
            return -16;
        }
    }
    return -14;
}
void leaveAllowedOnly(char *sourceStr)
{
    unsigned long long charNrPos = 0;
    char *analizedString = sourceStr;
    char *mask = ALLOWED_CHARACTERSA;

    while (analizedString && *analizedString)
    {
        charNrPos = strspn(analizedString, mask);
        analizedString += charNrPos;
        if (analizedString[0] == '\0')
            break;
        else
            analizedString[0] = '_';
    }
}

