/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :mobs.c
** Purpose :Holds all functions for processing mobs
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 27-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern short read_mob(short vnum);
extern unsigned long from_fund(unsigned long);
extern void reset_obj(short);
extern short wearit(short,short,short, short);
extern short read_obj(short);
extern void reset_mob(short);
extern void stolower(char *);
extern char *getdir(short);
extern short getroom(short);
extern void to_fund(unsigned long);
extern void wroom(short, char *, short);
extern void wuser(SU, char *);
extern char *cap1st(char *);
extern int psuedo_rand(int);
extern short getmflag(short, short);
extern void loadstock(short);



void healers()
{
/*	short count2,worn; - Not used yet */
	short count,mid;

	for (mid=0;mid<MAXMOBS;mid++) {
		if (MI.vnum==-1) continue;
		if (!getmflag(mid,mHEALER)) continue;
		
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket<0) continue;
			if (UC.room!=MI.room) continue;
			if (UC.hp>=UC.maxhp) continue;

			if (psuedo_rand(100)<50) {
				sprintf(text,"^y%s uses a hypospray on your neck and you feel better.^N\n",cap1st(MI.name));
				wuser(count,text);
				sprintf(text,"^y%s presses a hypospray against %s's neck.^N\n",cap1st(MI.name),UC.name);
				wroom(MI.room,text,count);
				UC.hp+=20;
				if (UC.hp>UC.maxhp) UC.hp=UC.maxhp;
			}
		} /* users loop */
	} /* mob loop */

}

void police(short rvnum)
{
	short count,count2,worn,mid;

	for (mid=0;mid<MAXMOBS;mid++) {
		if (MI.vnum==-1) continue;
		if (MI.room!=rvnum) continue;
		if (!getmflag(mid,mPOLICE)) continue;
		
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket<0) continue;
			if (UC.room!=MI.room) continue;
			if (UC.lvl<5) continue;
			if (UC.sleep==2) continue;
			if (UC.rank>NORM&&UC.immstatus==1) continue;
			if (time(0)-UC.lastfined<FINETIME) continue;

			worn=0;
			for (count2=0;count2<10;count2++) {
				if (UC.worn[count2]!=-1) {
					worn=1;
				}
			}

			if (!worn) {
				if (psuedo_rand(100)<50) {
					UC.naked_fine++;
					UC.lastfined=time(0);
					sprintf(text,"%s fines you %d gold coins for being naked in public.\n",MI.name,NAKEDFINE);
					wuser(count,text);
					sprintf(text,"%s gets fined for being naked.\n",UC.name);
					wroom(MI.room,text,count);
					if (UC.cash<1) {
						wuser(count,"You are skint, so you get let off, don't do it again!\n");
						continue;
					}
	
					if (UC.cash<NAKEDFINE) {
						to_fund(UC.cash);
						UC.cash=0;
					}
					else
					{
						to_fund(NAKEDFINE);
						UC.cash-=NAKEDFINE;
					}
				}
			} /* worn if */
		} /* users loop */
	} /* mob loop */

}

void movemob(short mid, short dir)
{
	short newr=0;

	DEBUG("movemob")
	if ((rooms[getroom(MI.room)].dir[dir]>0)) {
		if (!getmflag(mid,mZONEWAND)) {
			if (rooms[getroom(MI.room)].area!=rooms[getroom(rooms[getroom(MI.room)].dir[dir])].area)
			{
				return;
			}
		}

		event(eMOBLEAVE,-1,mid,-1);
		sprintf(text,"%s leaves %s.\n",cap1st(MI.sdesc),getdir(dir));
		wroom(MI.room,text,-1);
		newr=rooms[getroom(MI.room)].dir[dir];
		MI.room=newr;
		sprintf(text,"%s has arrived.\n",cap1st(MI.sdesc));
		wroom(MI.room,text,-1);
		event(eMOBENT,-1,mid,-1);
		police(MI.room);
	}
}

short getmob(SU,char *str)
{
		short count;
		char mobname[4096],mobdesc[4096];
		char cmp[4096];

		DEBUG("getmob")
		strcpy(cmp,str);
		stolower(cmp);
		for (count=0;count<MAXMOBS;count++)
		{
				if (MC.room==UU.room) {
						strcpy(mobname,MC.name);
						stolower(mobname);
						strcpy(mobdesc,MC.sdesc);
						stolower(mobdesc);
						if ((strstr(mobname,cmp))||(strstr(mobdesc,cmp))) {
										return count;
						}
				}
		}
	return -1;
}

short getmobindex(short vnum)
{
	short count;

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.vnum==vnum) return count;
	}
	return -1;
}

void agemobs()
{
	short count;

	DEBUG("agemobs")

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.fight!=-1) continue;
		MC.age++;

		if (MC.age>=MOB_ALIVE_DAYS) {
			sprintf(text,"%s dies from boredom.\n",cap1st(MC.sdesc));
			wroom(MC.room,text,-1);
			reset_mob(count);
		}
	}
}

/* male, female, neural */
char *getmobsex(short mid,char *g0, char *g1, char *g2)
{
	DEBUG("getmobsex")

	switch (mobs[mid].sex) {
		case 1 : return g0; break;
		case 2 : return g1; break;
		case 3 : return g2; break;
		default : return "Error!"; break;
	}
}

short next_victim(short mid)
{
	short count2=0;

	DEBUG("next_victim")

	for (count2=0;count2<MAXUSERS;count2++) {
		if (users[count2].socket<0) continue;
		if (users[count2].room!=mobs[mid].room) continue;
		if (users[count2].fight!=mid) continue;
		if (users[count2].sleep==2) continue;
		/* okay so the user is attacking the mob
		 * even if they arnt in a group
		 * so kill em anyway
		 */
		users[count2].sleep=0;
		users[count2].invis=0;
		return count2;
	}
	return -1;
}

short mobactive(short vnum)
{
	short count;

	DEBUG("mobactive")

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.vnum==vnum) return 1;
	}

	return 0;
}

short getfreemob()
{
	short count;

	DEBUG("getfreemob")

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) return count;
	}

	return -1;
}

void alloc_mob_kill(short vnum, char *name)
{
	short loop;
	for(loop=0; loop<MAXMOBS; loop++)
		if(glob.mobkills[loop].vnum==vnum)
			return;
	for(loop=0; glob.mobkills[loop].vnum!=-1; loop++);
	glob.mobkills[loop].vnum=vnum;
	strcpy(glob.mobkills[loop].name,name);
	glob.mobkills[loop].kills=0;
	return;
}

short read_mobs(char *fname)
{
	short lastroom=0;
	short rmess;
	short loop;
	short tmpobj,tmpcost;
	char *tok;
	smud_file *mobfile;
	DEBUG("read_mobs")

	sprintf(text,"../mobs/%s",fname);
	mobfile = read_file(text);
	if (mobfile->vnumcount==0)
	{
		freefile(mobfile);
		return -1;
	}

	for(loop=0; loop<mobfile->vnumcount; loop++)
	{
		if (!mobactive(mobfile->vnumlist[loop])) 
		{
			glob.curmob=getfreemob();
			if (glob.curmob==-1) 
			{
				freefile(mobfile);
				printf("no more mob space\n");
				exit(0);
			}
			reset_mob(glob.curmob);
			CM.vnum = mobfile->vnumlist[loop];
			sprintf(CM.name,getval(mobfile,mobfile->vnumlist[loop],"name"));
			sprintf(CM.sdesc,getval(mobfile,mobfile->vnumlist[loop],"shortdesc"));
			sprintf(CM.ldesc,getval(mobfile,mobfile->vnumlist[loop],"longdesc"));
			sprintf(CM.script,getval(mobfile,mobfile->vnumlist[loop],"script"));
			CM.lvl=atoi(getval(mobfile,mobfile->vnumlist[loop],"level"));
				CM.hp=(CM.lvl*25);
				CM.maxhp=CM.hp;
			sprintf(text,getval(mobfile,mobfile->vnumlist[loop],"sex"));
				CM.sex=text[0]=='M'?1:text[0]=='F'?2:3;
			CM.hitroll=atoi(getval(mobfile,mobfile->vnumlist[loop],"hitroll"));
			CM.ac=atoi(getval(mobfile,mobfile->vnumlist[loop],"ac"));
			CM.damroll=atoi(getval(mobfile,mobfile->vnumlist[loop],"damroll"));
			CM.align=atoi(getval(mobfile,mobfile->vnumlist[loop],"alignment"));
			sprintf(CM.flags,getval(mobfile,mobfile->vnumlist[loop],"flags"));
			sprintf(CM.nokill,getval(mobfile,mobfile->vnumlist[loop],"nokill"));
			CM.wimpy=atoi(getval(mobfile,mobfile->vnumlist[loop],"wimpy"));

			alloc_mob_kill(CM.vnum,CM.sdesc);

			tok=strtok(getval(mobfile,mobfile->vnumlist[loop],"objects"),"\n");
			while(tok)
			{
				tmpobj=atoi(tok);
				strcpy(text,tok);
				read_obj(tmpobj);
				if (wearit(glob.curmob,glob.curobj,-1,1)<1)
				{
					reset_obj(glob.curobj);
				} else {
					if (strchr(text,'d')||strchr(text,'D'))
					{
						objs[glob.curobj].deaddrop=1;
					}
				}
				tok=strtok(NULL,"\n");
			}

			loadstock(glob.curmob);

			tok=strtok(getval(mobfile,mobfile->vnumlist[loop],"attributes"),"\n");
			while(tok)
			{
				tmpobj=atoi(tok);
				tmpcost=atoi(strchr(tok,',')+1);
				read_obj(tmpobj);
				objs[glob.curobj].room=-1;
				objs[glob.curobj].user=-1;
				objs[glob.curobj].mob=mobfile->vnumlist[loop];
				objs[glob.curobj].forsale=1;
				objs[glob.curobj].originalstock=1;
				objs[glob.curobj].cost=tmpcost;
				if (reb!=1) CM.cash+=from_fund(200);
				tok=strtok(NULL,"\n");
			}

			CM.room=atoi(getval(mobfile,mobfile->vnumlist[loop],"home"));
			CM.sroom=CM.room;

			if (CM.room!=lastroom) 
			{
				rmess = psuedo_rand(4);
				switch (rmess) {
					case 1:
						wroom(CM.room,"Mobs seem to crawl from the woodwork.\n",-1);
						break;
					case 2:
						wroom(CM.room,"You feel annoyed as someone barges into you.\n",-1);
						break;
					case 3:
						wroom(CM.room,"You hear a slight noise as someone creeps in.\n",-1);
						break;
					case 4:
						wroom(CM.room,"You get a strange feeling of companionship.\n",-1);
						break;
				}
				lastroom=CM.room;
			}
		}
	}
	freefile(mobfile);
	return 0;
}
 
void restore_mobs()
{
	short count;

	DEBUG("restore_mobs")

	for (count=0;count<MAXAREAS;count++) {
		if (areas[count].vnum==-1) continue;
		if (areas[count].mobfile[0]=='\0') continue;
		if (areas[count].reload==0) continue;
		if (time(0)<areas[count].last_reload+areas[count].reload) continue;

		areas[count].last_reload=time(0);
		read_mobs(areas[count].mobfile);
	}
}

void restore_mobs_now()
{
	short count;

	DEBUG("restore_mobs")

	for (count=0;count<MAXAREAS;count++) {
		if (areas[count].vnum==-1) continue;
		if (areas[count].mobfile[0]=='\0') continue;
		if (areas[count].reload==0) continue;

		areas[count].last_reload=time(0);
		read_mobs(areas[count].mobfile);
	}
}

/* return how often a mob has been killed in smud memory */
short memkills(short mid)
{
	short count=0;

	for (count=0;count<MAX_KILLS;count++)
	{
		if (glob.kills[count].mob==-1) continue;

		// timeout?
		if (time(0)-glob.kills[count].when>MEMTIME) {
			glob.kills[count].mob=-1;
			continue;
		}

		if (glob.kills[count].mob==mid) return glob.kills[count].num;
	}
	return 0;
}

void storekill(SU,short mid)
{
	short got=-1;
	short count=0;
	time_t lookfor=0;
	
	if (rooms[getroom(UU.room)].area==1) {
		// dont count the newbie area
		return;
	}

	/* try and find the mob record */
	for (count=0;count<MAX_KILLS;count++)
	{
		if (glob.kills[count].mob==-1) continue;

		if (glob.kills[count].mob==mid) got=count;
	}

	if (got==-1) {
		/* not in memory, lets try to get an empty space */
		for (count=0;count<MAX_KILLS;count++)
		{
			if (glob.kills[count].mob==-1) {
				got=count;
				glob.kills[count].mob=mid;
				glob.kills[count].num=0;
				strcpy(glob.kills[count].user,UU.name);
			}
		}
	}

	if (got==-1) {
		/* no free space, okay, take over oldest record */
		for (count=0;count<MAX_KILLS;count++)
		{
			if (glob.kills[count].mob==-1) continue;

			if (time(0)-glob.kills[count].when>lookfor) {
				got=count;
				lookfor=time(0)-glob.kills[count].when;
			}
		}
		glob.kills[got].mob=mid;
		glob.kills[got].num=0;
		strcpy(glob.kills[got].user,UU.name);
	}

	if (got==-1) return;   /* oops */

	/* store the kill */
	glob.kills[got].num++;
	glob.kills[got].when=time(0);
	strcpy(glob.kills[got].user,UU.name);
}
