/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :use.c
** Purpose :More or less just the use command
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern short getobj(SU,char *);
extern short getheldobj(SU,char *);
extern void reset_obj(short);
extern void look(SU);
extern void brieflook(SU);
extern void wroom(short, char *, short);
extern void wuser(SU, char *);
extern short getoflag(short, short);

/* redundant but lets keep it for a bit
 *
void douse(SU,short oid,short held,short heldno)
{
	short rely=0,groom=0,count=0,gotrely=0;

	DEBUG("douse")

	switch(objs[oid].type) {
		case TRANSPORT:
			if ((!held)&&(!getoflag(oid,oNOTAKE))) {
				wuser(uid,"Try taking it first.\n");
				return;
			}
			rely=atoi(objs[oid].other);
			groom=atoi(strchr(objs[oid].other,',')+1);
//			 test current room for 'rely' 
			for (count=0;count<MAXOBJS;count++) {
				if (OC.name[0]=='\0') continue;
				if (OC.room!=UU.room) continue;
				if (OC.vnum!=rely||rely==0) continue;
				gotrely=1;
				break;
			}
			if (rely==0) gotrely=1;
			if (!gotrely) {
				wuser(uid,"You cannot use that here.\n");
				return;
			}
			sprintf(text,"You make use of %s.\n",objs[oid].sdesc);
			wuser(uid,text);
			sprintf(text,"%s makes use of %s.\n",UU.name,objs[oid].sdesc);
			wroom(UU.room,text,uid);
			UU.room=groom;
			if(UU.brief)
			{
				brieflook(uid);
			} else {
				look(uid);
			}
			sprintf(text,"%s appears by way of %s.\n",UU.name,objs[oid].sdesc);
			wroom(UU.room,text,uid);
			break;
		case POTION:
			if (!held) {
				wuser(uid,"Try taking it first.\n");
				return;
			}

			wuser(uid,"You quaff the potion.\n");
			sprintf(text, "%s quaffs a potion.\n",UU.name);
			wroom(UU.room,text,uid);

			if (objs[oid].vnum==22) { // Blue potion
				wuser(uid,"You fade away.\n");
				sprintf(text,"%s fades to black.\n",UU.name);
				wroom(UU.room,text,uid);
				UU.invis=55;
				UU.held[heldno]=-1;
				reset_obj(oid);
			}
			break;
		default:
			wuser(uid,"You cannot use that here.\n");
			break;
	}
}
*/


void use(SU)
{
	short obj;

	DEBUG("use")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Use what?\n");
		return;
	}

	/* first of all lets find out what they are using */

	obj=getheldobj(uid,comstr);

	if (obj!=-1) {
	//	douse(uid,UU.held[obj],1,obj);
		event(eOBJUSE,uid,-1,UU.held[obj]);
		return;
	}

	obj=getobj(uid,comstr);

	if (obj!=-1) {
	//	douse(uid,obj,0,0);
		event(eOBJUSE,uid,-1,obj);
		return;
	}

	wuser(uid,"You cannot use what is not here.\n");
}

void drink(SU)
{
	DEBUG("drink")

	if (word_count<1) {
		wuser(uid,"Drink what?\n");
		return;
	}

	use(uid);
}

