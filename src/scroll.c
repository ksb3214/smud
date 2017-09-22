/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   : scroll.c
** Purpose : this will handle ALL the magic on smud also with a
**           little help from doskills() for combat scrolls
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 16-07-02 0.00 ksb First Version
*/

#include "smud.h"

void simplehealth(SU,short obj,short target,short mob);
void simpleenergy(SU,short obj,short target,short mob);
short getuskill(short,short);
void wuser(short,char *);
void wroom(short,char *,short);
void wrapout(short,char *,char,char);
void reset_obj(short);
extern char *getsex(short,char *,char *,char *);

/* this is called when a scroll has been recited, it has already
 * been confirmed that the scroll can be read, this will determine
 * what sort of scroll it is and either chuck it in the combat
 * queue or call the appropriate function.
 *
 * it is upto the function to decide if the scroll disappears after
 * this or whether it just loses a charge or maybe something else.
 */
void scroll(SU, short obj, short target, short mob)
{
	if (getuskill(uid,objs[obj].discipline)<1) {
		wrapout(uid,"Whilst you can read it, you cannot control the magic contained within.\n",'J','N');
		return;
	}

	switch(objs[obj].vnum) {
		case 2000: // health
			   simplehealth(uid,obj,target,mob);
			   break;
		case 2001: // health
			   simpleenergy(uid,obj,target,mob);
			   break;
		default: break;
	}
}

void simplehealth(SU,short obj,short target,short mob)
{
	short act=0;
	short gain=0;

	if (target==-1) {
		wuser(uid,"Reciting this to the general world has no effect.\n");
		return;
	}

	if (mob) {
		wuser(uid,"You are trying to heal a mob, why?\n");
		return;
	}

	act=(int)((double)users[target].maxhp*0.2);
	gain=(int)((double)act*((double)getuskill(uid,objs[obj].discipline)/(double)100));
	if (gain<1) gain=1;

	sprintf(text,"You give %s a little more health [%d]\n",target==uid?"yourself":users[target].name,gain);
	wuser(uid,text);
	sprintf(text,"%s heals %s\n",UU.name,target==uid?getsex(uid,"himself","herself","itself"):users[target].name);
	wroom(UU.room,text,uid);

	users[target].hp+=gain;
	if (users[target].hp>users[target].maxhp) users[target].hp=users[target].maxhp;

	if (objs[obj].charges>0) objs[obj].charges--;
	if (objs[obj].charges==0)
	{
		sprintf(text,"%s loses the magic field which holds it together.\n",objs[obj].sdesc);
		wuser(uid,text);
		reset_obj(obj);
	}
}

void simpleenergy(SU,short obj,short target,short mob)
{
	short act=0;
	short gain=0;

	if (target==-1) {
		wuser(uid,"Reciting this to the general world has no effect.\n");
		return;
	}

	if (mob) {
		wuser(uid,"You are trying to energise a mob, why?\n");
		return;
	}

	act=(int)((double)users[target].maxhp*0.8);
	gain=(int)((double)act*((double)getuskill(uid,objs[obj].discipline)/(double)100));
	if (gain<1) gain=1;

	sprintf(text,"You give %s a little more energy [%d]\n",target==uid?"yourself":users[target].name,gain);
	wuser(uid,text);
	sprintf(text,"%s energises %s\n",UU.name,target==uid?getsex(uid,"himself","herself","itself"):users[target].name);
	wroom(UU.room,text,uid);

	users[target].mv+=gain;
	if (users[target].mv>users[target].maxmv) users[target].mv=users[target].maxmv;

	if (objs[obj].charges>0) objs[obj].charges--;
	if (objs[obj].charges==0)
	{
		sprintf(text,"%s loses the magic field which holds it together.\n",objs[obj].sdesc);
		wuser(uid,text);
		reset_obj(obj);
	}
}

