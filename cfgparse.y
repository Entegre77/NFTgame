%{
#include <math.h>		/* For math functions, cos(), sin(), etc. */
#include <time.h>
#include "ou.h"


    void yyerror(char *s);
    void SetUpRings(int, char *, float, float, float),
	SetUpAtmosphere(int, char *, float, int),
        SetUpBody(int, char *, int),
	SetUpAsteroid(int, char *),
	SetUpOrbiter(int, char *),
	SetUpSpaceShip(int, char *),
	SetUpComet(int, char *, float, float, int),
	InitSun(char *, double, int);
    int yylex(void);
    int yylineno = 0;
    int body_index = 1, last_planet = 0, last_satellite = 0, currblock =
	PLANET;
    double v[3];
    extern planetdata planets[MAXBODIES];
    extern FILE *cfgin;
    extern GLenum smodel;
    extern int border, filter, paused, demomode, demostep, plabels, slabels, 
        realtime, cmode, currtarget, currsource, jpeg_quality, mipmap, info,
        mipmap_filter, fonttype, LOD, tex_compr, color_depth;
    extern GLenum cd_rgb, cd_lum, cd_rgba, cd_luma;
    extern float speed, star_mag;
    struct tm date;
%}

%union {
    int ival;
    double dval;
    char name[100];
}

%token < ival > INTEGER_NUM
%token < dval > DOUBLE_NUM
%token < name > NAME
%token < name > SUN PLANET ASTEROID COMET SATELLITE ORBITER SPACESHIP RING ATMOSPHERE
%token < name > SPLASHBITMAP BORDER FILTER SHADING SIM_DATE PAUSE REALTIME
%token < name > PLANETS_LABELS STARS_LABELS INFO STAR_BRIGHT STAR_SIZE CAM_SPEED
%token < name > ATMOSPHERES CAM_MODE TARGET SOURCE SLICES STACKS JPEG_QUALITY
%token < name > MIPMAP MIPMAP_FILTER FONT LEVELOFDETAIL COLORDEPTH LOGO EOL
%token < name > MILKYWAY MESSIER DEMO DEMOSTEP TEXTURECOMPRESSION

%expect 5

%%

input: |input line;

line:

SPLASHBITMAP '=' INTEGER_NUM {
    splashbitmap = $3;
}

|ATMOSPHERES '=' INTEGER_NUM {
    atmospheres = $3;
}

|BORDER '=' INTEGER_NUM {
    border = $3;
}

|FILTER '=' NAME {
    if (!strcmp($3, "GL_NEAREST"))
	filter = GL_NEAREST;
    if (!strcmp($3, "GL_LINEAR"))
	filter = GL_LINEAR;
}

|SHADING '=' NAME {
    if (strcmp($3, "GL_FLAT"))
	smodel = GL_SMOOTH;
    else
	smodel = GL_FLAT;
}

|SIM_DATE '=' NAME NAME {
    sscanf($3, "%d-%d-%d", &date.tm_mon, &date.tm_mday, &date.tm_year);
    sscanf($4, "%d:%d:%d", &date.tm_hour, &date.tm_min, &date.tm_sec);
    date.tm_mon--;
    date.tm_year-=1900;
    days = mktime(&date) / 3600.0 / 24.0 - 10092.0;
}

|PAUSE '=' INTEGER_NUM {
    paused = $3;
}

|REALTIME '=' INTEGER_NUM {
    realtime = $3;
}

|DEMO '=' INTEGER_NUM {
    demomode = $3;
}

|DEMOSTEP '=' INTEGER_NUM {
    demostep = $3;
}

|PLANETS_LABELS '=' INTEGER_NUM {
    plabels = $3;
}

|STARS_LABELS '=' INTEGER_NUM {
    slabels = $3;
}

|INFO '=' INTEGER_NUM {
    info = $3;
}

|STAR_BRIGHT '=' DOUBLE_NUM {
    star_mag = $3;
}

|STAR_SIZE '=' DOUBLE_NUM {
    star_size = $3;
}

|CAM_SPEED '=' DOUBLE_NUM {
    speed = $3;
}

|CAM_MODE '=' INTEGER_NUM {
    cmode = $3;
}

|TARGET '=' INTEGER_NUM {
    currtarget = $3;
}

|SOURCE '=' INTEGER_NUM {
    currsource = $3;
}

|SLICES '=' INTEGER_NUM {
    slices = $3;
}

|STACKS '=' INTEGER_NUM {
    stacks = $3;
}

|JPEG_QUALITY '=' INTEGER_NUM {
    jpeg_quality = $3;
    if ((jpeg_quality < 0) || (jpeg_quality > 100)) {
	error("Invalid jpeg_quality value\n");
	shutdown(1);
    }
}

|MIPMAP '=' INTEGER_NUM {
    mipmap = $3;
}

|MIPMAP_FILTER '=' NAME {
    if (!strcmp($3, "GL_NEAREST_MIPMAP_NEAREST"))
	mipmap_filter = GL_NEAREST_MIPMAP_NEAREST;
    if (!strcmp($3, "GL_LINEAR_MIPMAP_NEAREST"))
	mipmap_filter = GL_LINEAR_MIPMAP_NEAREST;
    if (!strcmp($3, "GL_NEAREST_MIPMAP_LINEAR"))
	mipmap_filter = GL_NEAREST_MIPMAP_LINEAR;
    if (!strcmp($3, "GL_NEAREST_MIPMAP_LINEAR"))
	mipmap_filter = GL_NEAREST_MIPMAP_LINEAR;
    if (!strcmp($3, "GL_LINEAR_MIPMAP_LINEAR"))
	mipmap_filter = GL_LINEAR_MIPMAP_LINEAR;
}

|FONT '=' INTEGER_NUM {
fonttype = $3}

|LEVELOFDETAIL '=' INTEGER_NUM {
LOD = $3}

|TEXTURECOMPRESSION '=' INTEGER_NUM {
tex_compr *= $3}

|COLORDEPTH '=' INTEGER_NUM {
    color_depth = $3;
    switch (color_depth) {
      default:
      case 16:
	cd_rgb = GL_RGB5;
	cd_lum = GL_LUMINANCE8;
	cd_rgba = GL_RGBA4;
	cd_luma = GL_LUMINANCE8_ALPHA8;
	break;
      case 32:
	cd_rgb = GL_RGB8;
	cd_lum = GL_LUMINANCE8;
	cd_rgba = GL_RGBA8;
	cd_luma = GL_LUMINANCE8_ALPHA8;
	break;
    }
}

|LOGO '=' INTEGER_NUM {
    logo = $3;
}

|MILKYWAY '=' INTEGER_NUM {
    milkyway = $3;
}

|MESSIER '=' INTEGER_NUM {
    messier = $3;
}

|sun | body | spaceship;

sun:SUN '{' NAME DOUBLE_NUM INTEGER_NUM DOUBLE_NUM '}' {
    sprintf(planets[0].Name, "The Sun");
    planets[0].Mass = $6;
    LOG("Initializing Sun", InitSun($3, $4, $5))
}

body:planet | asteroid | comet;

planet:PLANET NAME '{' NAME INTEGER_NUM INTEGER_NUM INTEGER_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM INTEGER_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM INTEGER_NUM {
    strcpy(planets[body_index].Name, $2);
    INITVECTOR(planets[body_index].Color, $5 / 255.0, $6 / 255.0,
	       $7 / 255.0);
    planets[body_index].Radius = $8;
    planets[body_index].xflat = $9;
    planets[body_index].yflat = $10;
    planets[body_index].zflat = $11;
    planets[body_index].Degrees = $13;
    planets[body_index].Rotation = $14;
    planets[body_index].Inclination = $15 * M_PI / 180.0;
    planets[body_index].AscendingNode = $16 * M_PI / 180.0;
    planets[body_index].Perihelion = $17 * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION($18);
    planets[body_index].DailyMotion = $19 * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = $20;
    planets[body_index].Eccentricity = $21;
    planets[body_index].MeanLongitude = $22 * M_PI / 180.0;
    planets[body_index].Mass = $23;
    planets[body_index].CustomFunction = $24;
    planets[body_index].Sat = 0;
    planets[body_index].TrailEnabled = 0;
    planets[body_index].Type = PLANET;
    LOG($2, SetUpBody(body_index, $4, $12));
    last_planet = body_index;
    body_index++;
}
atmosphere rings satellites orbiters '}';

satellites:
        | satellite
        | satellites satellite

satellite: SATELLITE NAME '{' NAME INTEGER_NUM INTEGER_NUM INTEGER_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
                INTEGER_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
                DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
                INTEGER_NUM {
    currblock = SATELLITE;
    strcpy(planets[body_index].Name, $2);
    INITVECTOR(planets[body_index].Color, $5 / 255.0, $6 / 255.0,
	       $7 / 255.0);
    planets[body_index].Radius = $8;
    planets[body_index].xflat = $9;
    planets[body_index].yflat = $10;
    planets[body_index].zflat = $11;
    planets[body_index].Degrees = $13;
    planets[body_index].Rotation = $14;
    planets[body_index].Inclination = $15 * M_PI / 180.0;
    planets[body_index].AscendingNode = $16 * M_PI / 180.0;
    planets[body_index].Perihelion = $17 * M_PI / 180.0;
    planets[body_index].MeanDistance = $18;
    planets[body_index].DailyMotion = $19 * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = $20;
    planets[body_index].Eccentricity = $21;
    planets[body_index].MeanLongitude = $22 * M_PI / 180.0;
    planets[body_index].Mass = $23;
    planets[body_index].Type = SATELLITE;
    planets[body_index].Sat = last_planet;
    planets[body_index].CustomFunction = $24;
    LOG($2, SetUpBody(body_index, $4, $12));
    last_satellite = body_index;
    body_index++;
}

atmosphere orbiters {
    currblock = PLANET;
} '}';

orbiters:
        |orbiter
        | orbiters orbiter

orbiter:ORBITER NAME '{' NAME DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
                              DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM  {
    strcpy(planets[body_index].Name, $2);
    planets[body_index].Type = ORBITER;
    planets[body_index].Radius = $5;
    INITVECTOR(planets[body_index].Color, 1.0, 1.0, 1.0);
    planets[body_index].Sat =
	(currblock == SATELLITE) ? last_satellite : last_planet;
    planets[body_index].up[0] = $6;
    planets[body_index].up[1] = $7;
    planets[body_index].up[2] = $8;
    planets[body_index].dir[0] = $9;
    planets[body_index].dir[1] = $10;
    planets[body_index].dir[2] = $11;
    planets[body_index].Rotation = 0.0;
    planets[body_index].Inclination = $12;
    planets[body_index].AscendingNode = $13;
    planets[body_index].Perihelion = $14;
    planets[body_index].MeanDistance = $15;
    planets[body_index].DailyMotion = $16;
    planets[body_index].OrbitalPeriod = $17;
    planets[body_index].Eccentricity = $18;
    planets[body_index].MeanLongitude = $19;
    planets[body_index].CustomFunction = 0;
    LOG($2, SetUpOrbiter(body_index, $4));
    body_index++;
}

'}';


asteroid:ASTEROID NAME '{' NAME DOUBLE_NUM INTEGER_NUM INTEGER_NUM
    INTEGER_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM INTEGER_NUM {
    strcpy(planets[body_index].Name, $2);
    planets[body_index].Radius = $5;
    INITVECTOR(planets[body_index].Color, $6 / 255.0, $7 / 255.0,
	       $8 / 255.0);
    planets[body_index].xflat = $9;
    planets[body_index].yflat = $10;
    planets[body_index].zflat = $11;
    planets[body_index].Rotation = $12;
    planets[body_index].Inclination = $13 * M_PI / 180.0;
    planets[body_index].AscendingNode = $14 * M_PI / 180.0;
    planets[body_index].Perihelion = $15 * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION($16);
    planets[body_index].DailyMotion = $17 * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = $18;
    planets[body_index].Eccentricity = $19;
    planets[body_index].MeanLongitude = $20 * M_PI / 180.0;
    planets[body_index].Sat = 0;
    planets[body_index].CustomFunction = $21;
    planets[body_index].Type = ASTEROID;
    planets[body_index].up[0] = 0.0;
    planets[body_index].up[1] = 1.0;
    planets[body_index].up[2] = 0.0;
    planets[body_index].dir[0] = 0.0;
    planets[body_index].dir[1] = 0.0;
    planets[body_index].dir[2] = -1.0;
    LOG($2, SetUpAsteroid(body_index, $4));
    last_planet = body_index;
    body_index++;
}

satellites '}';

comet:COMET NAME '{' NAME INTEGER_NUM INTEGER_NUM INTEGER_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM INTEGER_NUM INTEGER_NUM {
    strcpy(planets[body_index].Name, $2);
    INITVECTOR(planets[body_index].Color, $5 / 255.0, $6 / 255.0,
	       $7 / 255.0);
    planets[body_index].Radius = $8;
    planets[body_index].xflat = $9;
    planets[body_index].yflat = $10;
    planets[body_index].zflat = $11;
    planets[body_index].Degrees = $13;
    planets[body_index].Rotation = $14;
    planets[body_index].Inclination = $15 * M_PI / 180.0;
    planets[body_index].AscendingNode = $16 * M_PI / 180.0;
    planets[body_index].Perihelion = $17 * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION($18);
    planets[body_index].DailyMotion = $19 * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = $20;
    planets[body_index].Eccentricity = $21;
    planets[body_index].MeanLongitude = $22 * M_PI / 180.0;
    planets[body_index].Sat = 0;
    planets[body_index].CustomFunction = $25;
    planets[body_index].Type = COMET;
    LOG($2, SetUpComet(body_index, $4, $12, $23, $24));
    last_planet = body_index;
    body_index++;
}

'}';


atmosphere:
|ATMOSPHERE '{' NAME INTEGER_NUM INTEGER_NUM INTEGER_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM INTEGER_NUM '}' {
    INITVECTOR(planets[body_index].Color, $4 / 255.0, $5 / 255.0,
	       $6 / 255.0);
    planets[body_index].Radius = $7;
    planets[body_index].Rotation = $8;
    planets[body_index].Sat = last_planet;
    planets[body_index].Type = ATMOSPHERE;
    LOG("Atmosphere", SetUpAtmosphere(body_index, $3, $9, $10));
    body_index++;
}

;

rings:
|ring | rings ring;

ring:RING '{' NAME INTEGER_NUM INTEGER_NUM INTEGER_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM '}' {
    planets[body_index].Color[0] = $4 / 255.0;
    planets[body_index].Color[1] = $5 / 255.0;
    planets[body_index].Color[2] = $6 / 255.0;
    planets[body_index].Sat = last_planet;
    planets[body_index].Type = RING;
    planets[body_index].InnerRadius = $7;
    planets[body_index].Radius = $8;
    planets[body_index].initialized = 0;
    LOG("Ring", SetUpRings(body_index, $3, $7, $8, $9));
    body_index++;
}

;


spaceship:SPACESHIP NAME '{' NAME DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM
    DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM DOUBLE_NUM {
    strcpy(planets[body_index].Name, $2);
    INITVECTOR(planets[body_index].Color, 1.0, 1.0, 1.0);
    planets[body_index].Type = SPACESHIP;
    planets[body_index].Radius = $5;
    planets[body_index].Sat = 0;
    INITVECTOR(planets[body_index].pos,
	       $6 * AU, $7 * AU, $8 * AU);
    INITVECTOR(planets[body_index].up, $9, $10, $11);
    INITVECTOR(planets[body_index].dir, $12, $13, $14);
    INITVECTOR(planets[body_index].vel, $15, $16, $17);
    LOG($2, SetUpSpaceShip(body_index, $4));
    body_index++;
}

'}';


%%

void yyerror(char *s)
{
    fprintf(stderr, "%s in line %d of configuration file\n", s, yylineno);
    shutdown(1);
}

/* Read configuration file */
void ReadConfigFile(void)
{
    char s[200];

    sprintf(s, "ou.conf");
    cfgin = fopen(s, "rt");
    if (cfgin == NULL) {
    	sprintf(s, "%s/ou.conf", confpath);
    	cfgin = fopen(s, "rt");
	if (cfgin == NULL) {
	    error("Couldn't find configuration file");
	    shutdown(1);
	}
    }
    yyparse();
    fclose(cfgin);
    NUMBODIES = body_index;
}
