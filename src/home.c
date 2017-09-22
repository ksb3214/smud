/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :home.c
** Purpose :Home room commands
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 22-11-01 0.00 ksb First Version
*/

#include "smud.h"

extern short getguild(short);
extern short checklist(SU,char *,short);
extern short isoffline(char *);
extern void look(SU);
extern void brieflook(SU);
extern void wroom(short, char *, short);
extern void start_edit(SU, short, short, char *);
extern short getroom(short);
extern short getfreeroom();
extern short getuser(SU,char *);
extern void wuser(SU, char *);
extern short checkmail(SU);
extern void read_obj(short);

void visitable(SU)
{
	DEBUG("visitable")
	
	if (UU.homeroom==-1) {
		wuser(uid,"You do not have a homeroom yet.\n");
		return;
	}

	UU.visitable=1;
	wuser(uid,"You set your homeroom to be visitable.\n");
}

void unvisitable(SU)
{
	DEBUG("unvisitable")
	
	if (UU.homeroom==-1) {
		wuser(uid,"You do not have a homeroom yet.\n");
		return;
	}

	UU.visitable=0;
	wuser(uid,"You set your homeroom to be unvisitable.\n");
}

short savearea(short vnum)
{
	short aid=-1;
	short saved=0;
	short count;
	char exits[10*20];
	char exits2[10*20];
	int count8;
	smud_file *roomfile;
	char text[256];

	DEBUG("savearea")
	
	for (count=0;count<MAXAREAS;count++)
	{
		if (areas[count].vnum==vnum) {
			aid=count;
		}
	}

	if (aid==-1) {
		return -1;
	}
	
	/* okay save all the rooms associated with that area to the file
	 * specified in areas[x].fname, in no particular order
	 */

	roomfile=new_file();
	
	for (count=0;count<MAXROOMS;count++)
	{
		if (RC.area==vnum) {
			bzero(exits,10*10);
			bzero(exits2,10*10);

			for(count8=0; count8<10; count8++)
			{
				if(RC.dir[count8]>0)
				{
					if(RC.door[count8][0]=='Y')
					{
						if(RC.keys[count8]>0)
						{
							sprintf(exits2,"%s%d%05dd%c%05d\n",exits,count8,RC.dir[count8],RC.door[count8][1],RC.keys[count8]);
						} else {
							sprintf(exits2,"%s%d%05dd%c\n",exits,count8,RC.dir[count8],RC.door[count8][1]);
						}
					} else {
						sprintf(exits2,"%s%d%05d\n",exits,count8,RC.dir[count8]);
					}
					sprintf(exits,"%s",exits2);
				}
			}
			if(exits[strlen(exits)-1]!='\n')
				exits[strlen(exits)-1]='\n';
					
			add_entry(roomfile,RC.vnum,"name",RC.name);
			add_entry(roomfile,RC.vnum,"desc",RC.desc);
			add_entry(roomfile,RC.vnum,"flags",RC.flags);
			add_entry(roomfile,RC.vnum,"direction",exits);
			saved=1;
		}
	}
	sprintf(text,"../rooms/%s",areas[aid].fname);
	write_file(roomfile,text);
	freefile(roomfile);
	return 0;
}

void makehome(uid)
{
	short count;
	short rc;
	short target;
	short vnum=20000;

	DEBUG("makehome")
	
	if (word_count<1) {
		wuser(uid,"Create a homeroom for who?\n");
		return;
	}

        target=getuser(uid,word[1]);

        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	if (users[target].homeroom!=-1) {
		wuser(uid,"They already have a homeroom\n");
		return;
	}

	rc=getfreeroom();

	if (rc==-1) {
		wuser(uid,"No rooms avaliable!\n");
		return;
	}

	for (count=0;count<MAXROOMS;count++) {
		if (RC.vnum==-1) continue;
		if (RC.vnum>25000) continue;
		if (RC.vnum>=vnum) vnum=RC.vnum+1;
	}

	/* should have highest user vnum */

	users[target].homeroom=vnum;
	rooms[rc].vnum=vnum;
	rooms[rc].area=20000;
	sprintf(text,"%s's room",users[target].name);
	strcpy(rooms[rc].name,text);
	sprintf(text,"This is a small room, almost cell like in its construction you feel somehow as if the creators intended you to remain confined when you are too scared to face the creatures of Smud.");
	strcpy(rooms[rc].desc,text);
	strcpy(rooms[rc].flags,"\n"); /* needs some flags */

	sprintf(text,"Homeroom %d created for %s.\n",vnum,users[target].name);
	wuser(uid,text);
	wuser(target,"You have just been assigned a homeroom. (see ^yhelp home^N)\n");
	if (savearea(20000)<0) {
		wuser(uid,"Failed to save homerooms.\n");
	}
}

void editroomsave(SU)
{
	short rc=0;
	if (UU.edit[0]=='\0') {
		wuser(uid,"Can't really allow an empty room description.\n");
		return;
	}

	rc=getroom(UU.room);

	strcpy(rooms[rc].desc,UU.edit);

	// the last char will be a \n
	rooms[rc].desc[strlen(rooms[rc].desc)-1]='\0';

	if (savearea(rooms[rc].area)<0) {
		wuser(uid,"Failed to save homerooms.\n");
	}
	else
	{
		wuser(uid,"Room description changed.\n");
	}
}

void editroom(SU)
{
	short rc=0;

	if (UU.homeroom==-1) {
		wuser(uid,"You don't have a homeroom to edit.\n");
		return;
	}

	if (UU.room!=UU.homeroom) {
		if(UU.immstatus==0) {
			wuser(uid,"You can only edit your own homeroom.\n");
			return;
		}
	}

	rc=getroom(UU.room);

	start_edit(uid,1,600,rooms[rc].desc);
}

void home(SU)
{
//	short newmail;
	DEBUG("home")
	
	if (UU.homeroom==-1) {
		wuser(uid,"You do not have a homeroom.\n");
		return;
	}

	if (UU.room==UU.homeroom) {
		wuser(uid,"You are already at home.\n");
		return;
	}

	if (UU.fight!=-1) {
		wuser(uid,"You are fighting, use flee to run.\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if(UU.room<20000) UU.leaveroom=UU.room;
	wuser(uid,"You leave for home.\n");
	sprintf(text,"%s leaves for home.\n",UU.name);
	wroom(UU.room,text,uid);
	UU.room=UU.homeroom;
	sprintf(text,"%s arrives.\n",UU.name);
	wroom(UU.room,text,uid);
	if(UU.brief)
	{	
		brieflook(uid);
	} else {
		look(uid);
	}
	/*
	newmail=checkmail(uid);
	if(newmail>0)
	{
		if(newmail==1)
		{
			wuser(uid,"^WThere is ^Y1^W new letter waiting in your letterbox.\n");
		} else {
			sprintf(text,"^WThere are ^Y%d^W new letters waiting in your letterbox.\n",newmail);
			wuser(uid,text);
		}
	}
	*/
}

void visit(SU)
{
	short target;

	DEBUG("visit")

	if (word_count<1) {
		wuser(uid,"Visit who?\n");
		return;
	}

        target=getuser(uid,word[1]);

	if (target==-1) {
		target=isoffline(word[1]);
	}

        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	if (users[target].homeroom==-1) {
		wuser(uid,"They do not have a homeroom\n");
		return;
	}

	if (UU.fight!=-1) {
		wuser(uid,"You are fighting, use flee to run.\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (!users[target].visitable) {
		if(!checklist(target,UU.name,lINVITE)) {
			wuser(uid,"Their room is not visitable.\n");
			return;
		}
	}

	if (target==MAXUSERS) {
		if (!checklist(target,UU.name,lKEY)) {
			wuser(uid,"They are not online right now.\n");
			return;
		}
	}

	if (UU.room<20000) {
		UU.leaveroom=UU.room;
	}

	wuser(uid,"You go visiting.\n");
	sprintf(text,"%s leaves for %s's home.\n",UU.name,users[target].name);
	wroom(UU.room,text,uid);
	UU.room=users[target].homeroom;
	sprintf(text,"%s arrives.\n",UU.name);
	wroom(UU.room,text,uid);
	if(UU.brief)
	{
		brieflook(uid);
	} else {
		look(uid);
	}
}

void leave(SU)
{
	DEBUG("leave");

	if (UU.room<20000) {
		wuser(uid,"You are not in a home room.\n");
		return;
	}

	wuser(uid,"You leave to face more nasty things.\n");
	sprintf(text,"%s leaves to kill things.\n",UU.name);
	wroom(UU.room,text,uid);
	UU.room=UU.leaveroom;
	if ((UU.room==-1)||(getroom(UU.room)==-1)) {
		wuser(uid,"Hmmm... I cant remember where you were, okay better dump you at recall.\n");
		UU.room=guilds[getguild(UU.guild)].recall_room;
	}
	UU.leaveroom=-1;
	sprintf(text,"%s arrives.\n",UU.name);
	wroom(UU.room,text,uid);
	if(UU.brief)
	{
		brieflook(uid);
	} else {
		look(uid);
	}
}

void save_home_objs()
{
	smud_file *objfile;
	short count,count2=0;

	objfile=new_file();
	for(count=0; count<MAXOBJS; count++)
	{
		if((objs[count].room>=20000) && (objs[count].vnum!=SIGN_OBJ))
		{
			count2++;
			sprintf(text,"%d",objs[count].vnum);
			add_entry(objfile,count2,"vnum",text);
			sprintf(text,"%d",objs[count].room);
			add_entry(objfile,count2,"room",text);
			sprintf(text,"%d",objs[count].age);
			add_entry(objfile,count2,"age",text);
			sprintf(text,"%d",objs[count].charges);
			add_entry(objfile,count2,"charges",text);
			sprintf(text,"%d",objs[count].lightage);
			add_entry(objfile,count2,"lightage",text);
			add_entry(objfile,count2,"writing",objs[count].writing);
			sprintf(text,"%d",objs[count].language);
			add_entry(objfile,count2,"language",text);
			add_entry(objfile,count2,"other",objs[count].other);
		}
	}
	write_file(objfile,"../other/home-room-objs.mff");
	freefile(objfile);
}

void load_home_objs()
{
	smud_file *objfile;
	short count;

	objfile=read_file("../other/home-room-objs.mff");
	for(count=0; count<objfile->vnumcount; count++)
	{
		if(atoi(getval(objfile,objfile->vnumlist[count],"vnum"))!=SIGN_OBJ)
		{
			read_obj(atoi(getval(objfile,objfile->vnumlist[count],"vnum")));
			CO.room=atoi(getval(objfile,objfile->vnumlist[count],"room"));
			CO.age=atoi(getval(objfile,objfile->vnumlist[count],"age"));
			CO.lightage=atoi(getval(objfile,objfile->vnumlist[count],"lightage"));
			CO.charges=atoi(getval(objfile,objfile->vnumlist[count],"charges"));
			CO.language=atoi(getval(objfile,objfile->vnumlist[count],"language"));
			strcpy(CO.writing,getval(objfile,objfile->vnumlist[count],"writing"));
			strcpy(CO.other,getval(objfile,objfile->vnumlist[count],"other"));
		}
	}
	freefile(objfile);
}
