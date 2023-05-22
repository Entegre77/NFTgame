%{
#include <math.h>
#include "ou.h"

int reserverd_word(char *s);
extern int yylineno;
#define YY_NO_UNPUT
%}

%option noyywrap

DIGIT   [0-9]
TEXT    [a-z_\-:/0-9.]*

%%

border                  return BORDER;
atmospheres             return ATMOSPHERES;
splash                  return SPLASHBITMAP;
filter                  return FILTER;
shading                 return SHADING;
date                    return SIM_DATE;
pause                   return PAUSE;
realtime                return REALTIME;
demo                    return DEMO;
demostep                return DEMOSTEP;
planets_labels          return PLANETS_LABELS;
stars_labels            return STARS_LABELS;
info                    return INFO;
star_brightness_factor  return STAR_BRIGHT;
star_size               return STAR_SIZE;
camera_speed            return CAM_SPEED;
camera_mode             return CAM_MODE;
target                  return TARGET;
source                  return SOURCE;
slices                  return SLICES;
stacks                  return STACKS;
jpeg_quality            return JPEG_QUALITY;
mipmap                  return MIPMAP;
mipmap_filter           return MIPMAP_FILTER;
sun                     return SUN;
planet                  return PLANET;
satellite               return SATELLITE;
orbiter                 return ORBITER;
spaceship               return SPACESHIP;
asteroid                return ASTEROID;
comet                   return COMET;
atmosphere              return ATMOSPHERE;
ring                    return RING;
font                    return FONT;
LOD                     return LEVELOFDETAIL;
texture_compression     return TEXTURECOMPRESSION;
color_depth             return COLORDEPTH;
logo                    return LOGO;
milky_way               return MILKYWAY;
messier                 return MESSIER;

{DIGIT}+ { cfglval.ival = atoi(yytext); return INTEGER_NUM; }

{DIGIT}+"."{DIGIT}*"E"*{DIGIT}* { cfglval.dval = atof(yytext); return DOUBLE_NUM; }

{TEXT} { strcpy(cfglval.name, yytext); return NAME; }


"\#"(.)*"\n" yylineno++;
"/*" {
    register int c;

    for (;;) {
	while ((c = yyinput()) != '*' && c != EOF);	/* eat up text of comment */

	if (c == '*') {
	    while ((c = yyinput()) == '*');
	    if (c == '/')
		break;		/* found the end */
	}

	if (c == EOF) {
	    error("EOF in comment");
	    break;
	}
    }
}

[\t]+
\n      { yylineno++; } 
"{"     return ('{');
"}"     return ('}');
"="     return ('=');
";"     return (';');
.

%%
