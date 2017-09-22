/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :config.h
** Purpose :All the configurable #defines
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 03-07-00 0.00 ksb First Version
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define GAME_PORT	4000	/* the default port for the game to run on */

#define MAXUSERS 	50	/* Maximum users on at any one time */
#define MAXMOBS 	500	/* Maximum active mobs */
#define MAXAREAS	20	/* Maximum areas */
#define MAXROOMS 	3000	/* Maximum rooms for whole world */
#define MAXOBJS 	2000	/* Maximum objects, held of otherwise */
#define MAXGUILDS	10	/* Maximum guilds */
#define MAXTRANS	10	/* Maximum carriages */
#define MAXROUTES	50	/* Maximum stops per carriage */
#define MAXCODES	128	/* Stargate codes */
#define MAX_HELD        100	/* items each char or mob can hold (max) */
#define MAX_LISTS	500	/* Maximum user associations in the system */
#define MAX_KILLS	25	/* Smud memory for mob kills */
#define MAX_SKILLS	100	/* must be more than ../other/skills */
#define EDITSIZE	4096	/* maximum edit size */
#define TABSIZE		4000	/* must be more than pfiles saved */
#define MAXQUEUE	4096	/* queue for skills commands */
#define MAXDEBATES	100	/* maximum number of debates */
#define ACTIVEDEBATES	10	/* maximum number of active debates */
#define MAXSCRIPT	500	/* maximum script lines */
#define SCRIPTLINE	255	/* maximum single line */

/* MCCP */
#define COMPRESS1	85	/* IAC for MCCP version 1 */
#define COMPRESS2	86	/* IAC for MCCP version 2 */
#define COMPRESS_BUF_SIZE	8192
#define TRUE		1
#define FALSE		0
#define COMP_LEVEL	9	/* Compression level 0-none to 9-max */

/* Fines! */
#define FINETIME	120	/* seconds grace between possible fines */
#define NAKEDFINE	100	/* wear something */

#define STARTROOM	1900	/* newbie start room */
#define SIGN_OBJ	25	/* object vnum used for stationary signs */
#define CORPSE_OBJ	208	/* object vnum to use as corpse */
#define CHANCE		150	/* % chance skills will work */
#define MAGICCHANCE	100	/* % chance skills will work */
#define LETTER_COST 	2	/* cost to send a letter */
#define WEIGHT_MULT 	5	/* amount weight of object is multiplied 
				   by for cost of sending */
#define AUTOAFK		1200	/* seconds idle before autoafk */
#define PARCEL_VNUM 	209 	/* the vnum of the parcel object */
#define SAVE_LIMIT	300	/* minimum seconds between manual saves */
 

/* Directories */
#define USERDIR 	"../users/" /* Pfile storage/retrieval */
#define LOGDIR          "../log/"	 /* system logs */
#define NEWSDIR		"../news/" /* news files */
#define OTHERDIR	"../other/"	/* anything else */
#define FUNDFILE	"../other/fund.def"
#define SCRIPTDIR	"../scripts/"	/* Smud Scripts */
#define SMUDINCLUDE	"/home/smud/smud/src/include/smud.h"

#define EMAILBUG	"smud@amber.org.uk"
#define EMAILIDEA	"smud@amber.org.uk"
#define EMAILTYPO	"smud@amber.org.uk"

#define MOB_ALIVE_DAYS	5
#define MOBMOVE		3 /* percentage movement */
#define MEMTIME		1800 /* how long to remember kills for */

#define HERODEATH	0.05 /* percnetage lost when a hero dies */
#define BONUSDAM	2	/* damage awarded from bonus roll */
#define INTDAYS		6	/* 6 = 7 days */
#define INTEREST	0.001   /* percent per game week */
#define AUTOSAVE	600 /* 10 minutes */
#define MAX_AFK		400 /* in minutes */
#define MAXLOGINS	20  /* Maximum simultanious new logins */
#define LOGINIDLE	180 /* maximum time allowed to login */

/* log.c defines
*/

#define SYS             1
#define COMMAND         2
#define USER            3
#define ADMIN           4

/* Ranks
*/

#define NORM            0
#define GUIDE           10
#define CARETAKER       20
#define OVERSEER        30
#define IMM             40
#define GOD             50

/* Fighting
*/
#define HPMOD		10 /* Percentage increase per hour asleep (+con) */
#define MANMOD		10
#define MOVMOD		10
#define HPMODA		2  /* % increase whilst awake */
#define MANMODA		2  /* % increase whilst awake */
#define MOVMODA		2  /* % increase whilst awake */

/* idle
*/

#define NORMIDLE        3600     /* 15 minutes */
#define ROOMIDLE        3600     /* 1 hour */

/* Weather 
*/
#define CHANGEPER	10 /* percentage chance of change in weather hourly */

#define SUN_RISE	0
#define SUN_LIGHT	1
#define TWILIGHT	2
#define	NIGHT		3

#define wSUN_RISE	"The sun rises in the east.\n"
#define wSTART_DAY	"A new day begins.\n"
#define wSUN_DOWN	"The sun sets over to the west.\n"
#define wSTART_NIGHT	"The night closes in.\n"

#define SUNSUP		"You find it light enough to see.\n"
#define NIGHTNOCLOUD	"It's dark, but the moon is bright.\n"
#define NIGHTCLOUD	"It's too dark to see anything.\n"
#define INSIDE		"You are currently indoors.\n"

#define wCLEAR		0
#define wCLOUDY		1
#define wRAIN		2
#define wLIGHTNING	3

#define CLEARTOCLOUD	"^cThe sky starts to get cloudy.^N\n"
#define CLOUDTORAIN	"^cIt starts to rain.^N\n"
#define RAINTOLIGHT	"^cLightning starts to show in the sky.^N\n"
#define LIGHTTORAIN	"^cThe lightning stops.^N\n"
#define RAINTOCLOUD	"^cThe rain stops.^N\n"
#define CLOUDTOCLEAR	"^cThe clouds disappear.^N\n"

#define ITSCLEAR	"The elements appear to be on your side, at the moment.\n"
#define ITSCLOUDY	"There are clouds in the sky.\n"
#define ITSRAINING	"The heavens empty on you.\n"
#define ITSLIGHTN	"You see sharp flashes of light in the sky.\n"

// logfile defines, these will be used as the first parameter to the syslog
// command throughout the program, change with care, you can use duplicates
// if you really want to.
#define lLOGIN		"login"
#define lLOGOUT		"logout"
#define lNEW		"newplayers"
#define lREMOVE		"remove"
#define lSIGNAL		"signals"
#define lSETTINGS	"settings"

#define INFOSTR		"Smud has been developed from ground up to provide a challenging and exciting environment for you mudders"
#endif
