/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003,  Karl Bastiman, Janet Hyde
**
** Title   : events.h
** Purpose : defines for scripting events
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-08-02 0.00 ksb First Version
*/


#ifndef _SMUD_EVENTS_H
#define _SMUD_EVENTS_H

#define ePLAYERENT 	":playerent:"	// player enters a room
#define ePLAYERLEAVE	":playerleave:"	// player is about to leave the room
#define ePLAYERARRIVE	":playerarrive:" // player just appears
#define ePLAYERDIS	":playerdis:" 	// player just disappears
// #define ePLAYERPOSTLEAVE ":pleayerpostleave:"// player has left room
#define eNEWPLAYERLOGON ":newplayerlogon:" // new player first logs on
#define ePLAYERLOGON	":playerlogon:"	// player logs on
#define ePLAYERLOGOFF	":playerlogoff:"// player logs off
#define ePLAYERSAY	":playersay:"	// player 'says' to the room
#define ePLAYEREMOTE	":playeremote:"	// player 'emotes' to the room
#define ePLAYERDIE	":playerdie:"	// a player dies

#define eMOBENT		":mobent:"	// A mob enters a room
#define eMOBLEAVE	":mobleave:"	// A mob is about to leave
// #define eMOBPOSTLEAVE	":mobpostleave:"// the mob has left
#define eMOBDIE		":moddie:"	// the mob has died

#define eOBJUSE		":objuse:"	// object is used
#define eOBJDROP	":objdrop:"	// object is dropped
#define eOBJWIELD	":objwield:"	// object is wielded
#define eOBJHOLD	":objhold:"	// object is held
#define eOBJTAKEN	":objtaken:"	// object taken

#define eTICK		":tick:"	// each game tick
#define eGAMEHOUR	":gamehour:"	// each game hour (about 30 seconds)
#define eMINUTE		":minute:"	// real minute
#define eHOUR		":hour:"	// real hour

#define NOTHING		0
#define EQUAL		1
#define NOTEQUAL	2
#define LT		3
#define GT		4

extern short event(char *,short,short,short);
#endif
