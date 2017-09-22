
/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :init.c
** Purpose :Initialisation routines
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern void to_fund(unsigned long);
extern void wroom(short, char *, short);
extern short getmobindex(short);


void reset_char(SU)
{
	short count=0;
/*	short count2=0; - Not used yet */
/*	FILE *fp; - Not used yet */


	DEBUG("reset_char")

	UU.wimpy=0;
	UU.hidden=0;
	UU.lastsaved=time(0);
	UU.logcount=0;
	UU.justify=1;
	UU.compout=0;
	UU.rawout=0;
	UU.out_compress=NULL;
	UU.out_compress_buf=NULL;
	UU.mccp=0; // not compressed
	strcpy(UU.prompt,"%hhp %mm %vmv>");
	strcpy(UU.cprompt,"%hhp %mm %vmv>");
	UU.reportto=-1;
	UU.kills=0;
	UU.queue[0]='\0';
	UU.lastfined=0;
	UU.naked_fine=0;
	UU.litter_fine=0;
	UU.eid=0;
	UU.edit[0]='\0';
	UU.swearon=0;
	UU.lastint=0;
	UU.avebank=0;
	UU.lastip[0]='\0';
	UU.curip[0]='\0';
	UU.lastlogin=0;
	UU.maxlvl=0;
	UU.canhold=20;
	UU.maxweight=150;
	UU.colour=0;
	UU.align=0;
	UU.maxcons=0;
	UU.prac=0;
	UU.invis=0;
	UU.guild=-1;
	UU.immstatus=0;
	UU.iacga=0;
	UU.visitable=1;
	UU.died=0;
	UU.lastdied=0;
	UU.laston=0;
	UU.homeroom=-1;
	UU.leaveroom=-1;
	UU.groupno=uid;
	strcpy(UU.oldinp,"look");
	UU.follow=-1;
	UU.firstlog=time(0);
	UU.alllog=0;
	UU.thislog=time(0);
	UU.lastnews=0;
	UU.name[0]='\0';
	UU.password[0]='\0';
	UU.socket=-1;
	UU.lvl=0;
	UU.lpos=0;
	UU.rank=NORM;
	UU.hp=0;
	UU.man=0;
	UU.mv=0;
	UU.maxhp=0;
	UU.maxman=0;
	UU.maxmv=0;
	UU.xp=0;
	UU.bufpos=0;
	UU.race=0;
	UU.room=0;
	UU.login=0;
	UU.sex=0;
	UU.flags[0]='\0';
	UU.str=0;
	UU.intl=0;
	UU.wis=0;
	UU.con=0;
	UU.dex=0;
	UU.maxstr=0;
	UU.maxint=0;
	UU.maxwis=0;
	UU.maxcon=0;
	UU.maxdex=0;
	UU.fight=-1;
	UU.idle=time(0);
	UU.idlewarn=0;
	UU.cash=0;
	UU.bank=0;
	UU.lastcom=time(0);
	UU.afk=0;
	UU.email[0]='\0';
	//UU.title[0]='\0';
	UU.sleep=0;
	for (count=0;count<13;count++) { UU.worn[count]=-1; }
	for (count=0;count<MAX_HELD;count++) { UU.held[count]=-1; }
	for (count=0;count<MAX_SKILLS;count++) {
		UU.skills[count].vnum=-1;
		UU.skills[count].prac=0;
	}
	for (count=0;count<MAXROOMS;count++) {
		UU.visited[count]=-1;
	}
	for (count=0;count<ACTIVEDEBATES;count++)
	{
		UU.voted[count]=-1;
	}
	UU.mydebate=-1;
	UU.terminal.type[0]='\0';	
	UU.terminal.height=24;
	UU.terminal.width=80;
	UU.defterm[0]='\0';
	UU.defprec=0;
	UU.autoloot=0;
	UU.autosac=0;
	UU.autoexit=0;
	UU.addr[0]='\0';
	UU.lastaddr[0]='\0';
	for (count=0;count<MAXCODES;count++) {
		UU.codes[count]=0;
	}
	for (count=0;abrchr[count]!='*';count++) {
		UU.abrchr[count]=abrchr[count];
		strcpy(UU.abrstr[count],abrstr[count]);
	}
	UU.abrchr[count]='*';
}

void reset_trans()
{
	short count,count2;

	for (count=0;count<MAXTRANS;count++) {
		trans[count].vnum=-1;		
		trans[count].name[0]='\0';
		trans[count].instr[0]='\0';
		trans[count].outstr[0]='\0';
		trans[count].warning[0]='\0';
		trans[count].exit=8;		
		trans[count].room=-1;	
		trans[count].wait=5;
		trans[count].gap=2;
		trans[count].status=1;
		trans[count].waitfor=3;
		for (count2=0;count2<MAXROUTES;count2++) {
			trans[count].route[count2]=-1; 
		}
	}
}

void reset_guild(short gid)
{
	short count=0;

	DEBUG("reset_guild")

	guilds[gid].vnum=-1;
	guilds[gid].name[0]='\0';
	guilds[gid].primary=0;
	guilds[gid].def[0]='\0';
	guilds[gid].minjoin=0;
	guilds[gid].pracroom=-1;
	guilds[gid].joinfee=0;
	guilds[gid].flags[0]='\0';
	for (count=0;count<MAX_SKILLS;count++) {
		guilds[gid].skills[count].vnum=-1;
		guilds[gid].skills[count].maxprac=0;
	}
}

void reset_obj(short oid)
{
	short count,uid;
	DEBUG("reset_obj")

	if (objs[oid].user!=-1) {
		uid=objs[oid].user;
		if (UU.socket>-1) {
			for (count=0;count<MAX_HELD;count++) {
				if (UU.held[count]==oid) UU.held[count]=-1;
			}
			for (count=0;count<13;count++) {
				if (UU.worn[count]==oid) UU.worn[count]=-1;
			}
		}
	}

	if (objs[oid].mob!=-1) {
		uid=getmobindex(objs[oid].mob);
		if (uid!=-1) {
			for (count=0;count<13;count++) {
				if (mobs[uid].worn[count]==oid) mobs[uid].worn[count]=-1;
			}
		}
	}

	objs[oid].dest=-1;
	objs[oid].name[0]='\0';
	objs[oid].writing[0]='\0';
	objs[oid].language=0;
	objs[oid].vnum=0;
	objs[oid].forsale=0;
	objs[oid].room=-1;
	objs[oid].originalstock=0;
	objs[oid].mob=-1;
	objs[oid].user=-1;
	objs[oid].sdesc[0]='\0';
	objs[oid].desc[0]='\0';
	objs[oid].type=0;
	objs[oid].flags[0]='\0';
	objs[oid].weight=0;
	objs[oid].cost=0;
	objs[oid].other[0]='\0';
	objs[oid].age=0;
	objs[oid].lightage=0;
	objs[oid].charges=0;
	objs[oid].wep_type=0;
	objs[oid].portions=0;
	objs[oid].poisoned=0;
	objs[oid].hunger=0;
	objs[oid].thirst=0;
	objs[oid].deaddrop=0;
	objs[oid].mob_drop=-1;
	objs[oid].dropname[0]='\0';
	objs[oid].armstr[0]='\0';
	objs[oid].yield=0;
	objs[oid].arm=0;
	objs[oid].coverage=0;
	objs[oid].fuse=0;
	objs[oid].script[0]='\0';
}

void reset_mob(short mid)
{
	short count=0;
/*	short count2=0; - Not used yet */

	DEBUG("reset_mob")

//	printf("Mob resetting - (%s)\n",mobs[mid].name);

	if (mobs[mid].name[0]!='\0') {
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket<0) continue;
			if (UC.fight!=mid) continue;

			UC.fight=-1;
		}

		for (count=0;count<MAXOBJS;count++) {
			if (OC.vnum<1) continue;
			if (OC.mob==-1) continue;
			if (OC.mob!=mobs[mid].vnum) continue;

			if ((OC.deaddrop)&&(mobs[mid].hp<1)) {
				/* okay bung it in the mobs old room */
				OC.mob=-1;
				OC.user=-1;
				OC.room=mobs[mid].room;
				OC.dropname[0]='\0';
				OC.mob_drop=mobs[mid].vnum;
				sprintf(text,"%s drops %s.\n",mobs[mid].name,OC.sdesc);
				wroom(mobs[mid].room,text,-1);
				continue;
			}
			reset_obj(count);
		}
		if (mobs[mid].cash>0) to_fund(mobs[mid].cash);
	}

	mobs[mid].name[0]='\0';
	mobs[mid].vnum=-1;
	mobs[mid].room=-1;
	mobs[mid].sdesc[0]='\0';
	mobs[mid].ldesc[0]='\0';
	mobs[mid].cash=0;
	mobs[mid].fight=-1;
	mobs[mid].age=0;
	mobs[mid].wimpy=0;
	mobs[mid].nokill[0]='\0';
	mobs[mid].script[0]='\0';
	for (count=0;count<13;count++) {
		/* shouldnt need to do this as have reset objects as above 
		if (mobs[mid].worn[count]>-1) {
			reset_obj(mobs[mid].worn[count]);
		}
		*/
		mobs[mid].worn[count]=-1;
	}
}

void init_vars()
{
	short count;
	short count2;

	DEBUG("init_vars")

	glob.intdays=0;
	glob.lsock=0;
	glob.lastreboot=time(0);
	glob.sreboots=0;
	glob.lastsave=time(0);
	glob.lastgod=time(0);
        glob.curmob=-1;
        glob.curobj=-1;
        glob.ht=0;glob.hr=0;
        glob.exitsmud=0;
        glob.normidle=NORMIDLE;
        glob.roomidle=ROOMIDLE;
        glob.gamehour=0; /* 0 - 23 */
        glob.gameday=1;  /* 1 - 300 */ /* 10x30day months nice and easy */
	glob.spec_port=0; /* port specified on command line */
        glob.startup=time(0);
        glob.sun=NIGHT;
        glob.weather=wCLEAR;
	glob.r_objs=10;
	glob.cr_objs=0;
	glob.lastauto=time(0);

	reset_trans();

	for (count=0;count<MAX_KILLS;count++)
	{
		glob.kills[count].mob=-1;
		glob.kills[count].user[0]='\0';
		glob.kills[count].when=0;
		glob.kills[count].num=0;
	}

	for (count=0;count<MAX_LISTS;count++)
	{
		glob.lists[count].owner[0]='\0';
		glob.lists[count].about[0]='\0';
		strcpy(glob.lists[count].flags,"0000000000000000000000000000");
	}

	for (count=0;count<MAX_SKILLS;count++)
	{
		glob.skills[count].vnum=-1;
		glob.skills[count].parent=-1;
		glob.skills[count].name[0]='\0';
	}

	for (count=0;count<MAXUSERS;count++)
	{
		reset_char(count);
	}
	
	for (count=0;count<MAXOBJS;count++)
	{
		reset_obj(count);
	}

	for (count=0;count<MAXMOBS;count++)
	{
		mobs[count].name[0]='\0';
		reset_mob(count);
	}

	for (count=0;count<MAXGUILDS;count++)
	{
		reset_guild(count);
	}

	for (count=0;count<MAXAREAS;count++)
	{
		areas[count].vnum=-1;
		areas[count].kills=0;
	}
	for (count=0;count<MAXMOBS;count++)
	{
		glob.mobkills[count].kills=0;
		glob.mobkills[count].vnum=-1;
	}

	for (count=0;count<MAXROOMS;count++)
	{
		RC.vnum=-1;
		RC.area=-1;
		RC.guild=-1;
		RC.joinroom=-1;
		for (count2=0;count2<10;count2++)
		{
			RC.door[count2][0]='N';
			RC.keys[count2]=-1;
			RC.dir[count2]=-1;
		}
	}
	for (count=0;count<24;count++) {
		glob.stats.users[count].max=0;
		glob.stats.users[count].min=MAXUSERS;
	}
	for (count=0;count<MAXDEBATES;count++)
	{
		glob.debates[count].vnum=-1;
		glob.debates[count].yes=0;
		glob.debates[count].no=0;
		glob.debates[count].abstain=0;
		glob.debates[count].status='\0';
	}
}

void init_socket()
{
	struct sockaddr_in bind_addr;
	int on=1;
	int size=0;

	DEBUG("init_socket")

	size=sizeof(struct sockaddr_in);
	bind_addr.sin_family=AF_INET;
	bind_addr.sin_addr.s_addr=INADDR_ANY;

	if ((glob.lsock=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		exit(-1);
	}

	setsockopt(glob.lsock,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));

	bind_addr.sin_port=htons(glob.spec_port!=0?glob.spec_port:GAME_PORT);
	if (bind(glob.lsock,(struct sockaddr *)&bind_addr,size)==-1)
	{
		exit(-1);
	}

	if (listen(glob.lsock,10)==-1)
	{
		exit(-1);
	}

	fcntl(glob.lsock,F_SETFL,O_NDELAY);
}

