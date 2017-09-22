/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : sign.c
** Purpose : all the non-IMM dynamic sign code
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 10-07-02 0.00 ksb First Version
*/

#include "smud.h"

extern void read_obj(short);
extern void wroom(short,char *,short);

void savesigns()
{
	short aid=-1;
	short count;
	smud_file *signfile;

	DEBUG("savesigns")
	
	// first remove the signs file as if there are any signs to be had
	// it will recreate it
	
	unlink("../other/signs.mff");

	for (count=0;count<MAXOBJS;count++)
	{
		if (objs[count].vnum==SIGN_OBJ) {
			aid=count;
		}
	}

	if (aid==-1) {
		return;
	}
	
	signfile=new_file();
	for (count=0;count<MAXOBJS;count++)
	{
		if (OC.vnum==SIGN_OBJ) {
			add_entry(signfile,OC.room,"writing",OC.writing);
			sprintf(text,"%d",OC.language);
			add_entry(signfile,OC.room,"language",text);
		}
	}
	sprintf(text,"../other/signs.mff");
	write_file(signfile,text);
	freefile(signfile);
}

void createsign(short room,char *text,short lang)
{
	short count;
	// dont create one if one is already in the room
	for (count=0;count<MAXOBJS;count++) {
		if (objs[count].vnum==-1) continue;
		if (objs[count].vnum!=SIGN_OBJ) continue;
		if (objs[count].room!=room) continue;
		return;
	}

	read_obj(SIGN_OBJ);
	if (glob.curobj!=-1) {
		objs[glob.curobj].room=room;
		objs[glob.curobj].user=-1;
		objs[glob.curobj].mob=-1;
		sprintf(objs[glob.curobj].writing,text);
		objs[glob.curobj].language=lang;
	}
	wroom(room,"A new sign has been created here.\n",-1);
}

/* should be called once
 */
void loadsigns()
{
	smud_file *signfile;
	short loop;
	char writing[4096];
	short lang=0;

	DEBUG("loadsigns")

//	printf("Reading signs...");
	signfile=read_file("../other/signs.mff");
	if (signfile->vnumcount==0) 
	{ 
		printf("none found.\n");
		freefile(signfile);
		return;
	}

	for(loop=0; loop<signfile->vnumcount; loop++)
	{
//		printf(".");
		sprintf(writing,getval(signfile,signfile->vnumlist[loop],"writing"));
		lang=atoi(getval(signfile,signfile->vnumlist[loop],"language"));
		createsign(signfile->vnumlist[loop],writing,lang);
	}
	freefile(signfile);
//	printf("\n");
}
