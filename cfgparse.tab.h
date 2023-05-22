typedef union {
    int ival;
    double dval;
    char name[100];
} YYSTYPE;
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


extern YYSTYPE cfglval;
