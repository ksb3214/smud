/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : transport.c
** Purpose : The smud transport system
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 12-07-02 0.00 ksb First Version
*/

#include "smud.h"

void wroom(short,char *,short);
short getroom(short);
void wuser(short,char *);
void look(SU);
void brieflook(SU);

short getfreetrans()
{
	short count;

	for (count=0;count<MAXTRANS;count++) {
		if (trans[count].vnum==-1) return count;
	}

	return -1;
}

void read_trans()
{
	short count=-1;
	smud_file *tranfile;
	short loop;
	char tmpstr[4096];
	char *ptr,*ptr2;
	short rn;

	DEBUG("read_rooms")

	tranfile=read_file("../other/transport.mff");
	if (tranfile->vnumcount==0) 
	{ 
		freefile(tranfile);
		printf("No carriages.\n");
		return;
	}

	printf("Reading carriages.\n");

	for(loop=0; loop<tranfile->vnumcount; loop++)
	{
		count=getfreetrans();
		if(count==-1)
		{
			freefile(tranfile);
			printf("No carriage space left.\n");
			return;
		}
		trans[count].vnum=tranfile->vnumlist[loop];
		sprintf(trans[count].name,getval(tranfile,tranfile->vnumlist[loop],"name"));
		sprintf(trans[count].instr,getval(tranfile,tranfile->vnumlist[loop],"instr"));
		sprintf(trans[count].outstr,getval(tranfile,tranfile->vnumlist[loop],"outstr"));
		sprintf(trans[count].warning,getval(tranfile,tranfile->vnumlist[loop],"warning"));
		sprintf(trans[count].move,getval(tranfile,tranfile->vnumlist[loop],"move"));
		sprintf(trans[count].stop,getval(tranfile,tranfile->vnumlist[loop],"stop"));
		trans[count].exit=atol(getval(tranfile,tranfile->vnumlist[loop],"exit"));
		trans[count].room=atol(getval(tranfile,tranfile->vnumlist[loop],"room"));
		trans[count].wait=atol(getval(tranfile,tranfile->vnumlist[loop],"wait"));
		trans[count].gap=atol(getval(tranfile,tranfile->vnumlist[loop],"gap"));
		trans[count].status=1;
		trans[count].waitfor=trans[count].wait;
		sprintf(tmpstr,getval(tranfile,tranfile->vnumlist[loop],"route"));
		// we rely on there being at least one room in the route
		// omg this for sucks, but Im hot and cant be arsed fighting with strtok
		ptr=tmpstr;
		for (rn=0;rn<MAXROUTES;rn++) {
			printf("route %d\n",rn);
			trans[count].route[rn]=atol(ptr);
			ptr2=strchr(ptr,'x');
			if (!ptr2) break;  // no more routes
			ptr=ptr2+1;
		}
		// initialise rooms exit
		rooms[getroom(trans[count].room)].dir[trans[count].exit]=trans[count].route[0];
		trans[count].nextstop=1;

	}
	freefile(tranfile);
}

// called every game hour
void do_trans()
{
	short count;

	for (count=0;count<MAXTRANS;count++) {
		if (trans[count].vnum==-1) continue;

		trans[count].waitfor--;

		if (trans[count].waitfor==0) {
			if (trans[count].status==0) {
				trans[count].waitfor=trans[count].wait;
				continue;
			}
			if (trans[count].status==1) {
				// move off
				sprintf(text,"%s\n",trans[count].instr);
				wroom(rooms[getroom(trans[count].room)].dir[trans[count].exit],text,-1);
				sprintf(text,"%s\n",trans[count].move);
				wroom(trans[count].room,text,-1);
				trans[count].status=2;
				trans[count].waitfor=trans[count].gap;
				rooms[getroom(trans[count].room)].dir[trans[count].exit]=-1;
				continue;
			}
			if (trans[count].status==2) {
				// stop
				rooms[getroom(trans[count].room)].dir[trans[count].exit]=trans[count].route[trans[count].nextstop];
				sprintf(text,"%s\n",trans[count].outstr);
				wroom(rooms[getroom(trans[count].room)].dir[trans[count].exit],text,-1);
				sprintf(text,"%s\n",trans[count].stop);
				wroom(trans[count].room,text,-1);
				trans[count].status=1;
				trans[count].waitfor=trans[count].wait;
				trans[count].nextstop++;
				if (trans[count].route[trans[count].nextstop]==-1) trans[count].nextstop=0;
				continue;
			}
			continue;
		}
		if (trans[count].waitfor==1) {
			if (trans[count].status==1) {
				sprintf(text,"%s\n",trans[count].warning);
				wroom(rooms[getroom(trans[count].room)].dir[trans[count].exit],text,-1);
				sprintf(text,"%s\n",trans[count].warning);
				wroom(trans[count].room,text,-1);
			}
		}
	}
}

// used to board a waiting carriage
void enter(SU)
{
	short count;

	if (word_count<1) {
		wuser(uid,"Enter what?\n");
		return;
	}


	/* show any carriages here */
	for (count=0;count<MAXTRANS;count++) {
		if (trans[count].vnum==-1) continue;
		if (rooms[getroom(trans[count].room)].dir[trans[count].exit]!=UU.room) continue;

		// carriage is here
		if (strstr(trans[count].name,word[1])) {
			if (UU.sleep) {
				wuser(uid,"Try being awake and standing first.\n");
				return;
			}

			if (UU.fight!=-1) {
				wuser(uid,"You are mid-fight, stay and stop being such a chicken!\n");
				return;
			}

			sprintf(text,"%s boards %s\n",UU.name,trans[count].name);
			wroom(UU.room,text,uid);
			UU.room=trans[count].room;
			sprintf(text,"%s arrives\n",UU.name);
			wroom(UU.room,text,uid);
			if(UU.brief)
			{
				brieflook(uid);
			} else {
				look(uid);
			}
			sprintf(text,"You board %s\n",trans[count].name);
			wuser(uid,text);
			return;
		}
	}

	wuser(uid,"Nothing of that name to enter here.\n");
}

