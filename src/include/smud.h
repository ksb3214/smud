/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   : smud.h
** Purpose : Main include file for SMUD
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/
#ifndef _SMUD_H_
#define _SMUD_H_

#ifdef LINUX
#include <crypt.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/telnet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <mff.h>
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#include "events.h"

#define DEBUG_ON	0
#define VER "0.9"
#define GC guilds[count]
#define RC rooms[count]
#define UC users[count]
#define UU users[uid]
#define BP users[uid].bufpos
#define CB users[uid].buff
#define SU short uid
#define CM mobs[glob.curmob]
#define MC mobs[count]
#define MI mobs[mid]
#define CO objs[glob.curobj]
#define OC objs[count]

#define DEBUG(a) 	if (DEBUG_ON) { printf("DEBUG:%s\n",a); fflush(stdout);}

#define SALT "BLAH"
#define LINE "-------------------------------------------------------------------------\n"

/* Object flags */

#define oINVIS          1
#define oNODROP         2
#define oANTIGOOD       3
#define oANTINEUTRAL    4
#define oANTIEVIL       5
#define oANTIMAGE       6	// not used
#define oMAGESONLY      7	// not used
#define oNOSELL         8
#define oHOLD           9
#define oWIELD          10
#define oNOTAKE         11
#define oHEAD           12
#define oNECK           13
#define oARMS           14
#define oBODY           15
#define oCHEST          16
#define oLEGS           17
#define oFEET           18
#define oWRIST          19
#define oHAND           20
#define oSHIELD         21
#define oLIGHT          22
#define oATM		23
#define oBOMB		24
#define oPORTAL		25	// stargates, uses obj.dest

/* Object types */

#define LIGHT           1
#define SCROLL          2
#define WAND            3
#define STAFF           4
#define WEAPON          5
#define TREASURE        6
#define ARMOUR          7
#define POTION          8
#define OTHER           9
#define TRASH           10
#define CONTAINER       11
#define DRINKCON        12
#define KEY             13
#define FOOD            14
#define MONEY           15
#define BOAT            16
#define FOUNTAIN        17
#define DRINK           18
#define CASHMACHINE	19
#define TRANSPORT	20
#define POSTBOX		21

/* Room flags */
#define DARK            1
#define PEACE           2
#define SOUND           3
#define NOMAGIC         4
#define PRIVATE         5
#define GODROOM         6
#define MAGES           7
#define WARRIORS        8
#define THIEVES         9
#define ASSASSINS       10
#define TEMPLARS        11
#define OUTSIDE         12
#define NEEDBOAT        13
#define UWATER          14
#define SWIMABLE        15
#define FLY             16      
#define NORECALL        17
#define CONTLIGHT	18

/* Mob flags */

#define mSTATIONARY	1
#define mSCAVANGE	2
#define mAWARE		3
#define mSEEINVIS	4
#define mAGGRES		5
#define mWIMPY		6 // NOT USED - set the mob's wimpy (mobs[x].wimpy) to a % instead
#define mANTIEVIL	7
#define mANTIGOOD	8
#define mANTINEUT	9
#define mMEMORY		10
#define mZONEWAND	11
#define mHELPER		12
#define mNOBLIND	13
#define mNOSLEEP	14
#define mNOCHARM	15
#define mNOSUMMON	16
#define mINVISIBLE	17
#define mSANC		18
#define mINFRA		19
#define mSNEAK		20
#define mPOISON		21
#define mPLAGUE		22
#define mWATER		23
#define mWALKWATER	24
#define mFLY		25
#define mMAGE		26
#define mSHOPKEEPER	27
#define mMADDEN		28
#define mNOCOUNT	29
#define mPOLICE		30
#define mHEALER		31
#define mPOSTMASTER	32

/* Lists defines */
#define lNOISY		0
#define lIGNORE		1
#define lINFORM		2
#define lGRABME		3
#define lFRIEND		4
#define lBAR		5
#define lINVITE		6
#define lBEEP		7
#define lBLOCK		8
#define lFIND		9
#define lKEY		10

struct scr_st {
	char buf[MAXSCRIPT][SCRIPTLINE];
	short ln;	// number of lines
	char event[128];
	short mid;
	short oid;
	short uid;
	short me;
	short metype; // MOB/OBJ/ROOMS
};

struct transport_st {
	short vnum;		/* trans vnum */
	char name[128];
	char instr[128];
	char outstr[128];
	char warning[128];
	char move[128];
	char stop[128];
	short exit;		/* conforms to a room exit number */
	short room;		/* room vnum to act as carriage */
	short route[MAXROUTES]; /* vnums of rooms in route order */
	short wait,gap;		/* in game hours, how long to wait at a room,
				   how long between rooms */
	short status;		/* 0 - stopped, 1 - waiting, 2 - in transit */
	short waitfor;		/* gamehours left till next action */
	short nextstop;
};

struct skills_st {
	short vnum;		/* The unique kill identifier */
	char  name[30];		/* Skill name */
	short parent;		/* parent skill */
	char  type;		/* N,C,B,A default N */
	char  level;		/* 0,1,2 ... relates to x.x.x functions */
};

struct guild_skills_st {
	short vnum;		/* vnum of skill */
	short maxprac;		/* maximum % skill can get to, if -1 then its
				   dependant on the parent skill */
};

/* if the users prac is above the guilds allowed maxprac for that skill then
 * it is assumed the user has recieved a bonus
 */
struct user_skills_st {
	short vnum;		/* vnum of skill */
	short prac;		/* % practiced to */
};

struct lists_st {
	char owner[12]; 	/* whos list entry it is */
	char about[12];		/* who does it concern */
	char flags[32];		/* little over the top maybe */
};

struct kill_st {
	short mob;		/* if -1 then this element is unused */
	short num;
	time_t when;
	char user[12];
};

typedef struct userstat_s {
        short   max;
        short   min;
} userstat;

typedef struct hourly_stats_s {
        userstat        users[24];
	short	last_min;
} hourly_stats;

typedef struct mob_kills_s{
	short vnum;
	unsigned long kills;
	char name[80];
	char killer[50];
} mob_kills;

typedef struct debate_s {
	short vnum;
	char subject[80];
	char body[4096];
	short yes;
	short no;
	short abstain;
	char status;
} debate;

struct globals {
	int lsock;
	short curmob;
	short curobj;
	short ht,hr;
	short exitsmud;
	time_t normidle;
	time_t roomidle;
	short gamehour; /* 0 - 23 */
	short gameday;  /* 1 - 300 */ /* 10x30day months nice and easy */
	time_t startup;
	short sun;
	short weather;
	short cr_objs;  /* count the hours till reset objects */
	short r_objs;   /* how often objects are reset */
	time_t lastauto;
	time_t lastsave;
	time_t lastgod;
	short sreboots;
	time_t lastreboot;
	short intdays;
	short spec_port;
	struct skills_st skills[MAX_SKILLS];
	struct lists_st lists[MAX_LISTS]; 
	struct kill_st kills[MAX_KILLS]; /* penalise for repeated mob kills */
	hourly_stats stats;
	mob_kills mobkills[MAXMOBS];
	debate debates[MAXDEBATES];
};

struct areas_st {
	short vnum;
	char name[80];
	short lowlevel;
	short highlevel;
	char author[12];
	char fname[255];
	char mobfile[255];
	short reload;
	time_t last_reload;
	unsigned long kills;
};


//struct spl {
//        short prac; /* practiced, -1 = havent got, 0=got not prac */
//        short affected; /* -1 not affected, or n hours left */
//        short cost; /* In mana */
//        short special[30]; /* Anything, spell specific */
//        char score_str[70]; /* text before 'for n hours.' in score */
//};

typedef struct terminal_info_s {
	char type[100]; /* a total guess, used to be 20 but OSX terminal broke that */
	short width;
	short height;
} terminal_info;

struct chr {
        char name[15];
        char password[15];
        int socket;
        short lvl;
	short maxlvl;		/* indicates the highest level of the char */
	short rank;
        short hp,man,mv,maxhp,maxman,maxmv;
        long xp;
        long bufpos;
        char buff[4096];
        short race;
        short room;
        short login;
        short sex;
	short align;
	unsigned long idle;
	short idlewarn;
        char flags[40];
	// attributes
	// changing to:
	// Speed
	// Strength
	// Agility
	// Intelligence
	// Memory
	// Resilience
        short str,intl,wis,con,dex;
        short maxstr,maxint,maxwis,maxcon,maxdex;
        short fight;  /* will be a mob vnum */
        short worn[13];
        short held[MAX_HELD];
	short canhold;   /* maximum objects player can hold */
	short maxweight; /* maximum weight player can carry */
        //struct spl spells[MAX_SPELLS];
	short groupno; /* number(uid) of the principle grouper */
	unsigned long cash;
	unsigned long bank;
	unsigned long avebank;
	unsigned long lastint;
	unsigned long lastcom;
	unsigned long afk;  /* when they expect to be back from being afk */
	char email[255];
	//char title[128];
	short sleep;  /* 0 - awake, 1 - resting, 2 - sleeping */
	short lastnews; /* last read news item */
	time_t firstlog,thislog,alllog;
	short follow;
	char oldinp[4096];
	short homeroom;
	short leaveroom;
	unsigned long died;
	time_t lastdied;
	time_t laston;
	time_t lastlogin;
	char lastip[20];
	char curip[20];
	short visitable;
	short wimpy;
	short iacga;
	short immstatus;
	short guild;
	short invis;  /* 1=yes, 0=no */
	short prac;  // how many practices left
	short maxcons; // peak con value in a fight, keep it fair
	short colour; // set if the user wants colour
	short swearon; // swear filer?
	char prompt[128];
	char cprompt[128];
	unsigned long kills;
	short reportto;
	long lpos;

	// editor stuff
	char edit[EDITSIZE]; // editor buffer
	short eid; // what are they editing 0 default
	short eline; // current line to insert *before*
	short maxedit; // Maximum edit stream size
	// Fine stuff
	time_t lastfined;
	unsigned long naked_fine;
	unsigned long litter_fine;
	//
	// skills command queue
	char queue[MAXQUEUE];
	//
	struct user_skills_st skills[MAX_SKILLS];
	// and this is the one which will make the whole lot HUGE
	short visited[MAXROOMS];
	// MCCP support will be set to:
	// 0 - none, 1 - version1, 2 - version 2
	short mccp;
	unsigned char *out_compress_buf;
	z_stream *out_compress;
	unsigned long rawout,compout;
	short voted[ACTIVEDEBATES];
	short mydebate;
	terminal_info terminal;
	short justify;
	short defprec;		
	char defterm[20];	
	char maildest[15];
	char mailsubject[256];

	short autoexit;		
	short autoloot;		
	short autosac;		
	short brief;
	unsigned long logcount;
	char addr[128];
	char lastaddr[128];
	time_t lastsaved;
	char abrchr[20];
	char abrstr[20][128];
	short hidden;
	char codes[MAXCODES];  // stargate stuff
};

/* skills command queue

   The queue is a simple string, the process is simple... the skill command
   add its skill id and associated data onto the end of the string, autox
   will then use whatever is at the start of the string to determine the
   command used... an empty string equals no queued up skill commands.
*/

/* worn numbers
**

0  - head
1  - neck
2  - arms
3  - body
4  - chest
5  - legs
6  - feet
7  - left wrist
8  - right wrist
9  - hands
10 - held
11 - wield
12 - shield

**
*/

struct gld {
	short vnum;
	short recall_room; /* members recall to here */
	char name[50];
	short primary;  /* 1 if primary group, 0 if secondary */
	char def[20];   /* What is seen in who */
	short minjoin;   /* Joining level */
	short location; /* which area is the guild in */
	short pracroom; /* Guild room to prac/train, default to recall */
	unsigned long joinfee;
	char flags[50];
//	short spbonus[MAX_SPELLS];
	struct guild_skills_st skills[MAX_SKILLS];
};

struct rm {
	char name[50];
	short vnum;
	short area;	/* vnum of area */
	short guild;	/* vnum if guild room */
	short joinroom; /* vnum if user can use the join command here */
	short xpgain;	/* xp gain gotten just by entering the room */
	char desc[791];
	short dir[10]; /* n,e,s,w,ne,nw,se,sw,u,d */
	char door[10][4]; /* door?,initial state,current state */
	short keys[10];   /* keys to open doors (vnum of objects) */
	char flags[40];
};

struct mob {
	char name[50];
	short vnum;
	short room;
	short sroom; /* starting room vnum */
	char sdesc[80];
	char ldesc[360];
	char flags[40];
	short lvl;
	short sex;
	short hp,maxhp;
	short ac,damroll,hitroll,align;
	short fight; /* will be a character number */
	short worn[13]; /* rnums of objects */
/*	short held[MAX_HELD];  rnums of objects */
//	struct spl spells[MAX_SPELLS];
	unsigned long cash;
	short age;  /* in game days */
	char nokill[128];
	short wimpy; 	// >>Percentage<< of maxhp to flee at
	char script[128]; // filename of script
};

struct obj {
	char name[50];
	short vnum;
	short room; /* Only one of         */
	short mob;  /* Of these can be set */
	short user; /* At any one time     */   
	short forsale; /* Should only be 1 if user or mob is set*/
	short originalstock; /* set to 1 if loaded with shopkeeper */
	char sdesc[80];
	char desc[320];
	char writing[800]; /* is anything written on it? */
	short language;    /* skill num needed to read it */
	short type;
	char flags[40];
	short weight;
	long cost;
	char other[80];
	short age;
	short lightage;
	short charges; /* if scroll, wand or staff */
	short wep_type;
	short portions; /* if drinks con */
	short poisoned; /* if food */
	short hunger; /* hours of hunger sated, if food */
	short thirst; /* thirst sated, if drink */
	short deaddrop; /* if assigned to a mob, then drop when dead? */
	short discipline; /* if a scroll */
	char dropname[15];
	short mob_drop;
	// explosives
	short fuse;	// how long till BANG
	short arm;	// countdown to BANG
	short yield;	// how big BANG
	short coverage; // how far BANG
	char armstr[255];	// what to say to player upon arming BANG
	char script[128];	// filename of script
	short dest;		// room vnum if oPORTAL -1 if off
};

struct command_st {
	char *name;
	short priv;
	void (* function)(SU);
};

typedef struct command_st sys_command;

extern struct transport_st trans[MAXTRANS];
extern struct areas_st areas[MAXAREAS];
extern struct chr users[MAXUSERS+1];
extern struct rm rooms[MAXROOMS];
extern struct mob mobs[MAXMOBS];
extern struct obj objs[MAXOBJS];
extern struct gld guilds[MAXGUILDS];
extern char inpstr[4096];
extern unsigned char inpchr;
extern char text[4096];
extern char retval[4096];
extern char *comstr;
extern short word_count;
extern short reb;
extern char word[10][255];
extern char *oflags;
extern char *rflags;
extern char *mflags;
extern char *wornstr[];
extern char *listdesc[];
extern char *listsdesc[];
extern struct globals glob;
extern struct scr_st inp;
extern char *abrstr[];
extern char *abrchr;

#endif
