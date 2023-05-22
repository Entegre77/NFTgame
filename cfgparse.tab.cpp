
/*  A Bison parser, made from cfgparse.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse cfgparse
#define yylex cfglex
#define yyerror cfgerror
#define yylval cfglval
#define yychar cfgchar
#define yydebug cfgdebug
#define yynerrs cfgnerrs
#define	INTEGER_NUM	258
#define	DOUBLE_NUM	259
#define	NAME	260
#define	SUN	261
#define	PLANET	262
#define	ASTEROID	263
#define	COMET	264
#define	SATELLITE	265
#define	ORBITER	266
#define	SPACESHIP	267
#define	RING	268
#define	ATMOSPHERE	269
#define	SPLASHBITMAP	270
#define	BORDER	271
#define	FILTER	272
#define	SHADING	273
#define	SIM_DATE	274
#define	PAUSE	275
#define	REALTIME	276
#define	PLANETS_LABELS	277
#define	STARS_LABELS	278
#define	INFO	279
#define	STAR_BRIGHT	280
#define	STAR_SIZE	281
#define	CAM_SPEED	282
#define	ATMOSPHERES	283
#define	CAM_MODE	284
#define	TARGET	285
#define	SOURCE	286
#define	SLICES	287
#define	STACKS	288
#define	JPEG_QUALITY	289
#define	MIPMAP	290
#define	MIPMAP_FILTER	291
#define	FONT	292
#define	LEVELOFDETAIL	293
#define	COLORDEPTH	294
#define	LOGO	295
#define	EOL	296
#define	MILKYWAY	297
#define	MESSIER	298
#define	DEMO	299
#define	DEMOSTEP	300
#define	TEXTURECOMPRESSION	301

#line 1 "cfgparse.y"

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

#line 32 "cfgparse.y"
typedef union {
    int ival;
    double dval;
    char name[100];
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		288
#define	YYFLAG		-32768
#define	YYNTBASE	50

#define YYTRANSLATE(x) ((unsigned)(x) <= 301 ? yytranslate[x] : 72)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    48,     2,    49,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     8,    12,    16,    20,    24,    29,    33,
    37,    41,    45,    49,    53,    57,    61,    65,    69,    73,
    77,    81,    85,    89,    93,    97,   101,   105,   109,   113,
   117,   121,   125,   129,   131,   133,   135,   143,   145,   147,
   149,   150,   181,   182,   184,   187,   188,   189,   219,   220,
   222,   225,   226,   248,   249,   274,   275,   303,   304,   316,
   317,   319,   322,   333,   334
};

static const short yyrhs[] = {    -1,
    50,    51,     0,    15,    47,     3,     0,    28,    47,     3,
     0,    16,    47,     3,     0,    17,    47,     5,     0,    18,
    47,     5,     0,    19,    47,     5,     5,     0,    20,    47,
     3,     0,    21,    47,     3,     0,    44,    47,     3,     0,
    45,    47,     3,     0,    22,    47,     3,     0,    23,    47,
     3,     0,    24,    47,     3,     0,    25,    47,     4,     0,
    26,    47,     4,     0,    27,    47,     4,     0,    29,    47,
     3,     0,    30,    47,     3,     0,    31,    47,     3,     0,
    32,    47,     3,     0,    33,    47,     3,     0,    34,    47,
     3,     0,    35,    47,     3,     0,    36,    47,     5,     0,
    37,    47,     3,     0,    38,    47,     3,     0,    46,    47,
     3,     0,    39,    47,     3,     0,    40,    47,     3,     0,
    42,    47,     3,     0,    43,    47,     3,     0,    52,     0,
    53,     0,    70,     0,     6,    48,     5,     4,     3,     4,
    49,     0,    54,     0,    63,     0,    65,     0,     0,     7,
     5,    48,     5,     3,     3,     3,     4,     4,     4,     4,
     3,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     3,    55,    67,    68,    56,    60,    49,     0,
     0,    57,     0,    56,    57,     0,     0,     0,    10,     5,
    48,     5,     3,     3,     3,     4,     4,     4,     4,     3,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     3,    58,    67,    60,    59,    49,     0,     0,    61,
     0,    60,    61,     0,     0,    11,     5,    48,     5,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,    62,    49,     0,     0,     8,     5,
    48,     5,     4,     3,     3,     3,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     3,    64,
    56,    49,     0,     0,     9,     5,    48,     5,     3,     3,
     3,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     3,     3,    66,
    49,     0,     0,    14,    48,     5,     3,     3,     3,     4,
     4,     4,     3,    49,     0,     0,    69,     0,    68,    69,
     0,    13,    48,     5,     3,     3,     3,     4,     4,     4,
    49,     0,     0,    12,     5,    48,     5,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
    71,    49,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    52,    52,    54,    60,    64,    68,    75,    82,    90,    94,
    98,   102,   106,   110,   114,   118,   122,   126,   130,   134,
   138,   142,   146,   150,   158,   162,   175,   178,   181,   184,
   203,   207,   211,   215,   215,   215,   217,   223,   223,   223,
   225,   255,   257,   258,   259,   261,   292,   294,   296,   297,
   298,   300,   328,   331,   365,   367,   396,   399,   400,   414,
   415,   415,   417,   434,   451
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","INTEGER_NUM",
"DOUBLE_NUM","NAME","SUN","PLANET","ASTEROID","COMET","SATELLITE","ORBITER",
"SPACESHIP","RING","ATMOSPHERE","SPLASHBITMAP","BORDER","FILTER","SHADING","SIM_DATE",
"PAUSE","REALTIME","PLANETS_LABELS","STARS_LABELS","INFO","STAR_BRIGHT","STAR_SIZE",
"CAM_SPEED","ATMOSPHERES","CAM_MODE","TARGET","SOURCE","SLICES","STACKS","JPEG_QUALITY",
"MIPMAP","MIPMAP_FILTER","FONT","LEVELOFDETAIL","COLORDEPTH","LOGO","EOL","MILKYWAY",
"MESSIER","DEMO","DEMOSTEP","TEXTURECOMPRESSION","'='","'{'","'}'","input","line",
"sun","body","planet","@1","satellites","satellite","@2","@3","orbiters","orbiter",
"@4","asteroid","@5","comet","@6","atmosphere","rings","ring","spaceship","@7", NULL
};
#endif

static const short yyr1[] = {     0,
    50,    50,    51,    51,    51,    51,    51,    51,    51,    51,
    51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
    51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
    51,    51,    51,    51,    51,    51,    52,    53,    53,    53,
    55,    54,    56,    56,    56,    58,    59,    57,    60,    60,
    60,    62,    61,    64,    63,    66,    65,    67,    67,    68,
    68,    68,    69,    71,    70
};

static const short yyr2[] = {     0,
     0,     2,     3,     3,     3,     3,     3,     4,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     1,     1,     1,     7,     1,     1,     1,
     0,    30,     0,     1,     2,     0,     0,    29,     0,     1,
     2,     0,    21,     0,    24,     0,    27,     0,    11,     0,
     1,     2,    10,     0,    19
};

static const short yydefact[] = {     1,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     2,    34,    35,
    38,    39,    40,    36,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     3,     5,     6,     7,     0,
     9,    10,    13,    14,    15,    16,    17,    18,     4,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    30,
    31,    32,    33,    11,    12,    29,     0,     0,     0,     0,
     0,     8,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    37,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    64,     0,     0,     0,
     0,     0,     0,     0,    65,     0,     0,     0,     0,    54,
     0,     0,    43,     0,     0,     0,     0,    44,     0,    41,
     0,    55,    45,     0,    58,     0,    56,     0,    60,     0,
     0,     0,     0,    43,    61,     0,    57,     0,     0,    49,
    62,     0,     0,     0,     0,     0,    50,     0,     0,     0,
     0,    42,    51,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    59,    63,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    46,    52,    58,
     0,    49,    53,    47,     0,    48,     0,     0
};

static const short yydefgoto[] = {     1,
    38,    39,    40,    41,   205,   197,   198,   280,   285,   226,
   227,   281,    42,   193,    43,   211,   209,   214,   215,    44,
   181
};

static const short yypact[] = {-32768,
     1,   -44,     7,    45,    46,    47,   -41,     6,     8,     9,
    10,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    49,    39,    40,    41,    42,    83,
    88,    87,    89,    90,    93,    94,    95,    96,    97,    98,
    99,   100,   102,   103,   104,   105,   106,   107,   108,   109,
   110,   111,   113,   114,   115,   116,   117,   118,   119,   120,
   121,   122,   123,   124,   125,-32768,-32768,-32768,-32768,   126,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   129,   130,   131,   133,
   134,-32768,   135,   137,   138,   139,   140,    44,   142,   143,
   144,   145,-32768,   146,   148,   149,   150,   151,   152,   153,
   154,   155,   156,   157,   158,   159,   160,   161,   162,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
   175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
   185,   186,   187,   188,   189,   190,-32768,   191,   192,   193,
    52,   194,   195,   196,-32768,   197,   199,   200,   201,-32768,
   202,   203,   127,   204,   206,   147,    -7,-32768,   207,-32768,
    65,-32768,-32768,   208,   112,   163,-32768,    76,   205,   209,
    85,   198,   210,    -8,-32768,   211,-32768,   212,   214,     4,
-32768,   213,   217,   218,   219,     0,-32768,   221,   220,   223,
   215,-32768,-32768,   224,   225,   227,   222,   228,   229,   230,
   231,   232,   233,   234,   235,   237,   238,   239,   240,   241,
   216,   226,   242,   243,-32768,-32768,   244,   245,   246,   247,
   248,   249,   250,   251,   252,   253,   255,   256,   257,   258,
   260,   262,   263,   264,   265,   267,   268,-32768,-32768,   112,
   236,   132,-32768,   132,   254,-32768,   271,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   -66,  -149,-32768,-32768,   -69,
  -226,-32768,-32768,-32768,-32768,-32768,   -63,-32768,    59,-32768,
-32768
};


#define	YYLAST		303


static const short yytable[] = {   233,
   287,   196,   196,    45,   213,    50,     2,     3,     4,     5,
   225,    46,     6,   196,   225,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,   202,    33,    34,    35,    36,    37,   203,   232,    47,
    48,    49,    51,    81,    52,    53,    54,   233,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
   203,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    86,    82,    83,    84,    85,
    87,    88,   133,    89,    90,    91,    92,    93,    94,    95,
   185,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   206,   108,   107,   109,   110,   111,   112,   113,
   114,   115,   116,   212,   117,   208,   118,   119,   120,   121,
   122,   123,   124,   217,   125,   126,   196,   127,   128,   129,
   130,   131,   225,   132,   134,   135,   136,   220,   137,   138,
   139,   201,   140,   141,   142,   143,   144,   145,   146,   147,
   148,   149,   150,   151,   152,   153,   154,   210,   155,   156,
   157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
   177,   178,   179,   180,   182,   183,   184,   186,   187,   188,
   189,   190,   218,   191,   192,   194,   195,   199,   200,   204,
   207,   216,   284,   222,   223,   228,   282,   213,   224,   229,
   230,     0,   235,   231,   234,   236,   241,   238,   239,   240,
     0,   242,   243,   244,   245,   246,   247,   248,   249,   250,
   251,     0,   252,   253,   254,   257,   258,   259,   260,   261,
   262,   263,   264,   265,   266,   267,   268,   219,   269,   270,
   271,   272,   237,   273,   255,   274,   275,   276,   277,   278,
   288,   279,   221,     0,   256,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   283,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   286
};

static const short yycheck[] = {   226,
     0,    10,    10,    48,    13,    47,     6,     7,     8,     9,
    11,     5,    12,    10,    11,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    49,    42,    43,    44,    45,    46,   197,    49,     5,
     5,     5,    47,     5,    47,    47,    47,   284,    47,    47,
    47,    47,    47,    47,    47,    47,    47,    47,    47,    47,
   220,    47,    47,    47,    47,    47,    47,    47,    47,    47,
    47,    47,    47,    47,    47,     3,    48,    48,    48,    48,
     3,     5,    49,     5,     5,     3,     3,     3,     3,     3,
    49,     4,     4,     4,     3,     3,     3,     3,     3,     3,
     3,     3,    48,     3,     5,     3,     3,     3,     3,     3,
     3,     3,     3,    48,     4,    14,     5,     5,     5,     5,
     5,     3,     3,    49,     4,     3,    10,     4,     4,     3,
     3,     3,    11,     4,     3,     3,     3,   214,     4,     4,
     3,     5,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     3,     5,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     3,     5,     4,     4,     4,     4,     4,     3,     3,
     3,     3,   282,     3,     3,     3,   280,    13,     5,     3,
     3,    -1,     3,     5,     4,     3,     5,     4,     4,     3,
    -1,     4,     4,     4,     4,     4,     4,     4,     4,     3,
     3,    -1,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,    48,     4,     4,
     4,     4,    48,     4,    49,     4,     4,     4,     4,     3,
     0,     4,   214,    -1,    49,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    49
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/cygnus/cygwin-b20/share/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/cygnus/cygwin-b20/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 56 "cfgparse.y"
{
    splashbitmap = yyvsp[0]. ival ;
;
    break;}
case 4:
#line 60 "cfgparse.y"
{
    atmospheres = yyvsp[0]. ival ;
;
    break;}
case 5:
#line 64 "cfgparse.y"
{
    border = yyvsp[0]. ival ;
;
    break;}
case 6:
#line 68 "cfgparse.y"
{
    if (!strcmp(yyvsp[0]. name , "GL_NEAREST"))
	filter = GL_NEAREST;
    if (!strcmp(yyvsp[0]. name , "GL_LINEAR"))
	filter = GL_LINEAR;
;
    break;}
case 7:
#line 75 "cfgparse.y"
{
    if (strcmp(yyvsp[0]. name , "GL_FLAT"))
	smodel = GL_SMOOTH;
    else
	smodel = GL_FLAT;
;
    break;}
case 8:
#line 82 "cfgparse.y"
{
    sscanf(yyvsp[-1]. name , "%d-%d-%d", &date.tm_mon, &date.tm_mday, &date.tm_year);
    sscanf(yyvsp[0]. name , "%d:%d:%d", &date.tm_hour, &date.tm_min, &date.tm_sec);
    date.tm_mon--;
    date.tm_year-=1900;
    days = mktime(&date) / 3600.0 / 24.0 - 10092.0;
;
    break;}
case 9:
#line 90 "cfgparse.y"
{
    paused = yyvsp[0]. ival ;
;
    break;}
case 10:
#line 94 "cfgparse.y"
{
    realtime = yyvsp[0]. ival ;
;
    break;}
case 11:
#line 98 "cfgparse.y"
{
    demomode = yyvsp[0]. ival ;
;
    break;}
case 12:
#line 102 "cfgparse.y"
{
    demostep = yyvsp[0]. ival ;
;
    break;}
case 13:
#line 106 "cfgparse.y"
{
    plabels = yyvsp[0]. ival ;
;
    break;}
case 14:
#line 110 "cfgparse.y"
{
    slabels = yyvsp[0]. ival ;
;
    break;}
case 15:
#line 114 "cfgparse.y"
{
    info = yyvsp[0]. ival ;
;
    break;}
case 16:
#line 118 "cfgparse.y"
{
    star_mag = yyvsp[0]. dval ;
;
    break;}
case 17:
#line 122 "cfgparse.y"
{
    star_size = yyvsp[0]. dval ;
;
    break;}
case 18:
#line 126 "cfgparse.y"
{
    speed = yyvsp[0]. dval ;
;
    break;}
case 19:
#line 130 "cfgparse.y"
{
    cmode = yyvsp[0]. ival ;
;
    break;}
case 20:
#line 134 "cfgparse.y"
{
    currtarget = yyvsp[0]. ival ;
;
    break;}
case 21:
#line 138 "cfgparse.y"
{
    currsource = yyvsp[0]. ival ;
;
    break;}
case 22:
#line 142 "cfgparse.y"
{
    slices = yyvsp[0]. ival ;
;
    break;}
case 23:
#line 146 "cfgparse.y"
{
    stacks = yyvsp[0]. ival ;
;
    break;}
case 24:
#line 150 "cfgparse.y"
{
    jpeg_quality = yyvsp[0]. ival ;
    if ((jpeg_quality < 0) || (jpeg_quality > 100)) {
	error("Invalid jpeg_quality value\n");
	shutdown(1);
    }
;
    break;}
case 25:
#line 158 "cfgparse.y"
{
    mipmap = yyvsp[0]. ival ;
;
    break;}
case 26:
#line 162 "cfgparse.y"
{
    if (!strcmp(yyvsp[0]. name , "GL_NEAREST_MIPMAP_NEAREST"))
	mipmap_filter = GL_NEAREST_MIPMAP_NEAREST;
    if (!strcmp(yyvsp[0]. name , "GL_LINEAR_MIPMAP_NEAREST"))
	mipmap_filter = GL_LINEAR_MIPMAP_NEAREST;
    if (!strcmp(yyvsp[0]. name , "GL_NEAREST_MIPMAP_LINEAR"))
	mipmap_filter = GL_NEAREST_MIPMAP_LINEAR;
    if (!strcmp(yyvsp[0]. name , "GL_NEAREST_MIPMAP_LINEAR"))
	mipmap_filter = GL_NEAREST_MIPMAP_LINEAR;
    if (!strcmp(yyvsp[0]. name , "GL_LINEAR_MIPMAP_LINEAR"))
	mipmap_filter = GL_LINEAR_MIPMAP_LINEAR;
;
    break;}
case 27:
#line 175 "cfgparse.y"
{
fonttype = yyvsp[0]. ival ;
    break;}
case 28:
#line 178 "cfgparse.y"
{
LOD = yyvsp[0]. ival ;
    break;}
case 29:
#line 181 "cfgparse.y"
{
tex_compr *= yyvsp[0]. ival ;
    break;}
case 30:
#line 184 "cfgparse.y"
{
    color_depth = yyvsp[0]. ival ;
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
;
    break;}
case 31:
#line 203 "cfgparse.y"
{
    logo = yyvsp[0]. ival ;
;
    break;}
case 32:
#line 207 "cfgparse.y"
{
    milkyway = yyvsp[0]. ival ;
;
    break;}
case 33:
#line 211 "cfgparse.y"
{
    messier = yyvsp[0]. ival ;
;
    break;}
case 37:
#line 217 "cfgparse.y"
{
    sprintf(planets[0].Name, "The Sun");
    planets[0].Mass = yyvsp[-1]. dval ;
    LOG("Initializing Sun", InitSun(yyvsp[-4]. name , yyvsp[-3]. dval , yyvsp[-2]. ival ))
;
    break;}
case 41:
#line 228 "cfgparse.y"
{
    strcpy(planets[body_index].Name, yyvsp[-22]. name );
    INITVECTOR(planets[body_index].Color, yyvsp[-19]. ival  / 255.0, yyvsp[-18]. ival  / 255.0,
	       yyvsp[-17]. ival  / 255.0);
    planets[body_index].Radius = yyvsp[-16]. dval ;
    planets[body_index].xflat = yyvsp[-15]. dval ;
    planets[body_index].yflat = yyvsp[-14]. dval ;
    planets[body_index].zflat = yyvsp[-13]. dval ;
    planets[body_index].Degrees = yyvsp[-11]. dval ;
    planets[body_index].Rotation = yyvsp[-10]. dval ;
    planets[body_index].Inclination = yyvsp[-9]. dval  * M_PI / 180.0;
    planets[body_index].AscendingNode = yyvsp[-8]. dval  * M_PI / 180.0;
    planets[body_index].Perihelion = yyvsp[-7]. dval  * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION(yyvsp[-6]. dval );
    planets[body_index].DailyMotion = yyvsp[-5]. dval  * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = yyvsp[-4]. dval ;
    planets[body_index].Eccentricity = yyvsp[-3]. dval ;
    planets[body_index].MeanLongitude = yyvsp[-2]. dval  * M_PI / 180.0;
    planets[body_index].Mass = yyvsp[-1]. dval ;
    planets[body_index].CustomFunction = yyvsp[0]. ival ;
    planets[body_index].Sat = 0;
    planets[body_index].TrailEnabled = 0;
    planets[body_index].Type = PLANET;
    LOG(yyvsp[-22]. name , SetUpBody(body_index, yyvsp[-20]. name , yyvsp[-12]. ival ));
    last_planet = body_index;
    body_index++;
;
    break;}
case 46:
#line 264 "cfgparse.y"
{
    currblock = SATELLITE;
    strcpy(planets[body_index].Name, yyvsp[-22]. name );
    INITVECTOR(planets[body_index].Color, yyvsp[-19]. ival  / 255.0, yyvsp[-18]. ival  / 255.0,
	       yyvsp[-17]. ival  / 255.0);
    planets[body_index].Radius = yyvsp[-16]. dval ;
    planets[body_index].xflat = yyvsp[-15]. dval ;
    planets[body_index].yflat = yyvsp[-14]. dval ;
    planets[body_index].zflat = yyvsp[-13]. dval ;
    planets[body_index].Degrees = yyvsp[-11]. dval ;
    planets[body_index].Rotation = yyvsp[-10]. dval ;
    planets[body_index].Inclination = yyvsp[-9]. dval  * M_PI / 180.0;
    planets[body_index].AscendingNode = yyvsp[-8]. dval  * M_PI / 180.0;
    planets[body_index].Perihelion = yyvsp[-7]. dval  * M_PI / 180.0;
    planets[body_index].MeanDistance = yyvsp[-6]. dval ;
    planets[body_index].DailyMotion = yyvsp[-5]. dval  * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = yyvsp[-4]. dval ;
    planets[body_index].Eccentricity = yyvsp[-3]. dval ;
    planets[body_index].MeanLongitude = yyvsp[-2]. dval  * M_PI / 180.0;
    planets[body_index].Mass = yyvsp[-1]. dval ;
    planets[body_index].Type = SATELLITE;
    planets[body_index].Sat = last_planet;
    planets[body_index].CustomFunction = yyvsp[0]. ival ;
    LOG(yyvsp[-22]. name , SetUpBody(body_index, yyvsp[-20]. name , yyvsp[-12]. ival ));
    last_satellite = body_index;
    body_index++;
;
    break;}
case 47:
#line 292 "cfgparse.y"
{
    currblock = PLANET;
;
    break;}
case 52:
#line 301 "cfgparse.y"
{
    strcpy(planets[body_index].Name, yyvsp[-17]. name );
    planets[body_index].Type = ORBITER;
    planets[body_index].Radius = yyvsp[-14]. dval ;
    INITVECTOR(planets[body_index].Color, 1.0, 1.0, 1.0);
    planets[body_index].Sat =
	(currblock == SATELLITE) ? last_satellite : last_planet;
    planets[body_index].up[0] = yyvsp[-13]. dval ;
    planets[body_index].up[1] = yyvsp[-12]. dval ;
    planets[body_index].up[2] = yyvsp[-11]. dval ;
    planets[body_index].dir[0] = yyvsp[-10]. dval ;
    planets[body_index].dir[1] = yyvsp[-9]. dval ;
    planets[body_index].dir[2] = yyvsp[-8]. dval ;
    planets[body_index].Rotation = 0.0;
    planets[body_index].Inclination = yyvsp[-7]. dval ;
    planets[body_index].AscendingNode = yyvsp[-6]. dval ;
    planets[body_index].Perihelion = yyvsp[-5]. dval ;
    planets[body_index].MeanDistance = yyvsp[-4]. dval ;
    planets[body_index].DailyMotion = yyvsp[-3]. dval ;
    planets[body_index].OrbitalPeriod = yyvsp[-2]. dval ;
    planets[body_index].Eccentricity = yyvsp[-1]. dval ;
    planets[body_index].MeanLongitude = yyvsp[0]. dval ;
    planets[body_index].CustomFunction = 0;
    LOG(yyvsp[-17]. name , SetUpOrbiter(body_index, yyvsp[-15]. name ));
    body_index++;
;
    break;}
case 54:
#line 334 "cfgparse.y"
{
    strcpy(planets[body_index].Name, yyvsp[-19]. name );
    planets[body_index].Radius = yyvsp[-16]. dval ;
    INITVECTOR(planets[body_index].Color, yyvsp[-15]. ival  / 255.0, yyvsp[-14]. ival  / 255.0,
	       yyvsp[-13]. ival  / 255.0);
    planets[body_index].xflat = yyvsp[-12]. dval ;
    planets[body_index].yflat = yyvsp[-11]. dval ;
    planets[body_index].zflat = yyvsp[-10]. dval ;
    planets[body_index].Rotation = yyvsp[-9]. dval ;
    planets[body_index].Inclination = yyvsp[-8]. dval  * M_PI / 180.0;
    planets[body_index].AscendingNode = yyvsp[-7]. dval  * M_PI / 180.0;
    planets[body_index].Perihelion = yyvsp[-6]. dval  * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION(yyvsp[-5]. dval );
    planets[body_index].DailyMotion = yyvsp[-4]. dval  * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = yyvsp[-3]. dval ;
    planets[body_index].Eccentricity = yyvsp[-2]. dval ;
    planets[body_index].MeanLongitude = yyvsp[-1]. dval  * M_PI / 180.0;
    planets[body_index].Sat = 0;
    planets[body_index].CustomFunction = yyvsp[0]. ival ;
    planets[body_index].Type = ASTEROID;
    planets[body_index].up[0] = 0.0;
    planets[body_index].up[1] = 1.0;
    planets[body_index].up[2] = 0.0;
    planets[body_index].dir[0] = 0.0;
    planets[body_index].dir[1] = 0.0;
    planets[body_index].dir[2] = -1.0;
    LOG(yyvsp[-19]. name , SetUpAsteroid(body_index, yyvsp[-17]. name ));
    last_planet = body_index;
    body_index++;
;
    break;}
case 56:
#line 370 "cfgparse.y"
{
    strcpy(planets[body_index].Name, yyvsp[-23]. name );
    INITVECTOR(planets[body_index].Color, yyvsp[-20]. ival  / 255.0, yyvsp[-19]. ival  / 255.0,
	       yyvsp[-18]. ival  / 255.0);
    planets[body_index].Radius = yyvsp[-17]. dval ;
    planets[body_index].xflat = yyvsp[-16]. dval ;
    planets[body_index].yflat = yyvsp[-15]. dval ;
    planets[body_index].zflat = yyvsp[-14]. dval ;
    planets[body_index].Degrees = yyvsp[-12]. dval ;
    planets[body_index].Rotation = yyvsp[-11]. dval ;
    planets[body_index].Inclination = yyvsp[-10]. dval  * M_PI / 180.0;
    planets[body_index].AscendingNode = yyvsp[-9]. dval  * M_PI / 180.0;
    planets[body_index].Perihelion = yyvsp[-8]. dval  * M_PI / 180.0;
    planets[body_index].MeanDistance = DISTCORRECTION(yyvsp[-7]. dval );
    planets[body_index].DailyMotion = yyvsp[-6]. dval  * M_PI / 180.0;
    planets[body_index].OrbitalPeriod = yyvsp[-5]. dval ;
    planets[body_index].Eccentricity = yyvsp[-4]. dval ;
    planets[body_index].MeanLongitude = yyvsp[-3]. dval  * M_PI / 180.0;
    planets[body_index].Sat = 0;
    planets[body_index].CustomFunction = yyvsp[0]. ival ;
    planets[body_index].Type = COMET;
    LOG(yyvsp[-23]. name , SetUpComet(body_index, yyvsp[-21]. name , yyvsp[-13]. dval , yyvsp[-2]. dval , yyvsp[-1]. ival ));
    last_planet = body_index;
    body_index++;
;
    break;}
case 59:
#line 401 "cfgparse.y"
{
    INITVECTOR(planets[body_index].Color, yyvsp[-7]. ival  / 255.0, yyvsp[-6]. ival  / 255.0,
	       yyvsp[-5]. ival  / 255.0);
    planets[body_index].Radius = yyvsp[-4]. dval ;
    planets[body_index].Rotation = yyvsp[-3]. dval ;
    planets[body_index].Sat = last_planet;
    planets[body_index].Type = ATMOSPHERE;
    LOG("Atmosphere", SetUpAtmosphere(body_index, yyvsp[-8]. name , yyvsp[-2]. dval , yyvsp[-1]. ival ));
    body_index++;
;
    break;}
case 63:
#line 418 "cfgparse.y"
{
    planets[body_index].Color[0] = yyvsp[-6]. ival  / 255.0;
    planets[body_index].Color[1] = yyvsp[-5]. ival  / 255.0;
    planets[body_index].Color[2] = yyvsp[-4]. ival  / 255.0;
    planets[body_index].Sat = last_planet;
    planets[body_index].Type = RING;
    planets[body_index].InnerRadius = yyvsp[-3]. dval ;
    planets[body_index].Radius = yyvsp[-2]. dval ;
    planets[body_index].initialized = 0;
    LOG("Ring", SetUpRings(body_index, yyvsp[-7]. name , yyvsp[-3]. dval , yyvsp[-2]. dval , yyvsp[-1]. dval ));
    body_index++;
;
    break;}
case 64:
#line 436 "cfgparse.y"
{
    strcpy(planets[body_index].Name, yyvsp[-15]. name );
    INITVECTOR(planets[body_index].Color, 1.0, 1.0, 1.0);
    planets[body_index].Type = SPACESHIP;
    planets[body_index].Radius = yyvsp[-12]. dval ;
    planets[body_index].Sat = 0;
    INITVECTOR(planets[body_index].pos,
	       yyvsp[-11]. dval  * AU, yyvsp[-10]. dval  * AU, yyvsp[-9]. dval  * AU);
    INITVECTOR(planets[body_index].up, yyvsp[-8]. dval , yyvsp[-7]. dval , yyvsp[-6]. dval );
    INITVECTOR(planets[body_index].dir, yyvsp[-5]. dval , yyvsp[-4]. dval , yyvsp[-3]. dval );
    INITVECTOR(planets[body_index].vel, yyvsp[-2]. dval , yyvsp[-1]. dval , yyvsp[0]. dval );
    LOG(yyvsp[-15]. name , SetUpSpaceShip(body_index, yyvsp[-13]. name ));
    body_index++;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/cygnus/cygwin-b20/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 454 "cfgparse.y"


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
