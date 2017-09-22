/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :weather.c
** Purpose :Controls weather changes and reporting of changes
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void wall_outdoors(char *);
extern int psuedo_rand(int);
extern void wuser(SU, char *);
extern short getroom(short);
extern short getrflag(short, short);


/* Uses global variable - 'sun' and 'weather'
**
** sun is..
**
** SUN_RISE, SUN_LIGHT, TWILIGHT, NIGHT
**
** weather is..
**
** wCLEAR, wCLOUDY, wRAIN, wLIGHTNING 
**
*/

/* The weather command */
void xweather(SU)
{
	DEBUG("xweather")
	if (!getrflag(getroom(UU.room),OUTSIDE)) {
		wuser(uid,INSIDE);
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	switch(glob.sun) {
		case SUN_RISE:
		case SUN_LIGHT:
		case TWILIGHT:
			wuser(uid,SUNSUP); break;
		case NIGHT:
			if (glob.weather!=0) {
				wuser(uid,NIGHTCLOUD);
			}
			else
			{
				wuser(uid,NIGHTNOCLOUD);
			}
			break;
		default: break;
	}

	switch(glob.weather) {
		case wCLEAR     : wuser(uid,ITSCLEAR); break;
		case wCLOUDY    : wuser(uid,ITSCLOUDY); break;
		case wRAIN      : wuser(uid,ITSRAINING); break;
		case wLIGHTNING : wuser(uid,ITSLIGHTN); break;
		default: break;
	}
}

/* test for a change in weather/sun (called every game hour) */
void test_weather()
{
	short chance=0;
	short changeto=0;
	short dir=0;
	short test=0;

	DEBUG("test_weather")

	chance=psuedo_rand(100);

	if (chance<CHANGEPER) { /* the weather changes */
		dir=psuedo_rand(100)>50?1:-1;
		changeto=glob.weather+dir;
		if (changeto>wLIGHTNING) changeto=wLIGHTNING;
		if (changeto<wCLEAR) changeto=wCLEAR;
		if (changeto!=glob.weather) {
			test=glob.weather*10+changeto;
			switch(test) {
				case 1:  wall_outdoors(CLEARTOCLOUD); break;
				case 12: wall_outdoors(CLOUDTORAIN); break;
				case 23: wall_outdoors(RAINTOLIGHT); break;
				case 32: wall_outdoors(LIGHTTORAIN); break;
				case 21: wall_outdoors(RAINTOCLOUD); break;
				case 10: wall_outdoors(CLOUDTOCLEAR); break;
				default: wall_outdoors("Weather is screwed\n");
			}
			glob.weather=changeto;
		}
	}


	switch(glob.gamehour) {
		case 5: glob.sun=SUN_RISE;
			wall_outdoors(wSUN_RISE);
			break;
		case 6: glob.sun=SUN_LIGHT;
			wall_outdoors(wSTART_DAY);
			break;
		case 21: glob.sun=TWILIGHT;
			wall_outdoors(wSUN_DOWN);
			break;
		case 22: glob.sun=NIGHT;
			wall_outdoors(wSTART_NIGHT);
			break;
		default: break;
	}
}

