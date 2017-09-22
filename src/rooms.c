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

extern short read_mobs(char *);
extern short getroom(short);
extern void wuser(short,char *);
extern short wroom(short,char *,short);
extern void adjxp(short,long);

void multiroom(short startvnum, short depth, void *(*task)(short vnum))
{
	short rm;
	short count;

	if (depth==0) return;

	rm=getroom(startvnum);

	if (rm==-1) return;

	// do the task here
	task(startvnum);

	// do for any exits
	for (count=0;count<10;count++) {
		if (rooms[rm].dir[count]==-1) continue;
		// recurse
		multiroom(rooms[rm].dir[count], depth-1, task);
	}
}

/* if the user hasnt been in the room before then add xpgain to their 
 * total xp and set a free element of UU.visited[] to the room vnum
 */
void xpgain(SU)
{
	short count;

	if (UU.room>=20000) return;

	for (count=0;count<MAXROOMS;count++) {
		if (UU.visited[count]==-1) continue;
		if (UU.visited[count]!=UU.room) continue;

		// already got xp for this room
		return;
	}

	adjxp(uid,rooms[getroom(UU.room)].xpgain);
//	wuser(uid,"Woo more xp..\n");

	for (count=0;count<MAXROOMS;count++) {
		if (UU.visited[count]==-1) {
			UU.visited[count]=UU.room;
			return;
		}
	}
}

short getfreeroom()
{
	short count=0;

	DEBUG("getfreeroom")

	for (count=0;count<MAXROOMS;count++) {
		if (RC.vnum==-1) return count;
	}

	return -1;
}

short read_rooms(char *fname, short area)
{
	short count=-1;
	char *door;
	smud_file *roomfile;
	char *tok;
	short loop;

	DEBUG("read_rooms")

	sprintf(text,"../rooms/%s",fname);
	printf("%s\n",text);
	roomfile=read_file(text);
	if (roomfile->vnumcount==0) 
	{ 
		freefile(roomfile);
		printf("Room file missing (%s)\n",text);
		exit(-1); 
	}

	for(loop=0; loop<roomfile->vnumcount; loop++)
	{
		printf(".");
		count=getfreeroom();
		if(count==-1)
		{
			printf("No rooms\n");
			return -1;
		}
		RC.vnum=roomfile->vnumlist[loop];
		RC.area=area;
		RC.guild=-1;
		sprintf(RC.name,getval(roomfile,roomfile->vnumlist[loop],"name"));
		sprintf(RC.desc,getval(roomfile,roomfile->vnumlist[loop],"desc"));
		sprintf(RC.flags,getval(roomfile,roomfile->vnumlist[loop],"flags"));
		sprintf(text,getval(roomfile,roomfile->vnumlist[loop],"direction"));
		//RC.xpgain=atol(getval(roomfile,roomfile->vnumlist[loop],"xpgain"));
		//if (RC.xpgain==0) RC.xpgain=1;
		RC.xpgain=1;

		
		tok=strtok(text,"\n");
		while(tok) 
		{
			 RC.dir[*tok-48]=atoi(tok+1); 
			 door=strchr(tok,'d');
			 if (door) {
				if (*(door+1)=='\0') {
					DEBUG("Door creation error - ignoring.")
				} else {
					RC.door[*tok-48][0]='Y';
					RC.door[*tok-48][1]=*(door+1);
					RC.door[*tok-48][2]=*(door+1);
					if (*(door+2)!='\0') {
						RC.keys[*tok-48]=atoi((char *)(door+2));
					}
				}
			}
			tok=strtok(NULL,"\n");
		}

	}
	freefile(roomfile);
	printf("\n");
	return 0;
}

short getroom(short vnum)
{
	short count=0;

	DEBUG("getroom")
	
	for (count=0;count<MAXROOMS;count++) {
		if (rooms[count].vnum==vnum) return count;
	}

	return -1;
}

short getarea(short vnum)
{
	short count=0;

	DEBUG("getarea")
	
	for (count=0;count<MAXAREAS;count++) {
		if (areas[count].vnum==vnum) return count;
	}

	return -1;
}

short read_areas()
{
	short count=-1;
	smud_file *areafile;

	DEBUG("read_areas")

	
	areafile = read_file("../other/areas.mff");
	if(areafile->vnumcount==0)
	{
		freefile(areafile);
		exit(-1);
	}

	printf("\n\n");
	for(count=0; count<areafile->vnumcount; count++)
	{
		areas[count].vnum=areafile->vnumlist[count];
		printf("Loading area %d...\n",areafile->vnumlist[count]);
		sprintf(areas[count].name,getval(areafile,areafile->vnumlist[count],"name"));
		printf("%s\n",getval(areafile,areafile->vnumlist[count],"name"));
		areas[count].lowlevel=atoi(getval(areafile,areafile->vnumlist[count],"min"));
		areas[count].highlevel=atoi(getval(areafile,areafile->vnumlist[count],"max"));
		sprintf(areas[count].fname,getval(areafile,areafile->vnumlist[count],"rooms"));
		sprintf(areas[count].author,getval(areafile,areafile->vnumlist[count],"creator"));
		sprintf(areas[count].mobfile,getval(areafile,areafile->vnumlist[count],"mobs"));
		areas[count].reload=atoi(getval(areafile,areafile->vnumlist[count],"reset"));
		if(read_rooms(areas[count].fname,areas[count].vnum)==-1) 
		{
			printf("Failed to load rooms (%s)",areas[count].name);
			freefile(areafile);
			exit(-1);
		}
		if (areas[count].mobfile[0]!='\0')
		{
			if (read_mobs(areas[count].mobfile)==0) 
			{
				printf("Area has mobs... loaded\n");
			}
		}
	}
	printf("\n");
	freefile(areafile);
	return 0;
}
