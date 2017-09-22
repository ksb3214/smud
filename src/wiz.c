/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :wiz.c
** Purpose :Holds all the admin commands
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short, long);
extern void syslog(int, char *);
extern short psave_all(short);
extern char *getsex(SU,char *, char *, char *);
extern short getroom(short);
extern long totalpfiles(SU);
extern void line(SU,char *);
extern void look(SU);
extern void brieflook(SU);
extern short getguild(short);
extern void killgroup(SU);
extern void reset_mob(short);
extern short getmob(SU,char *);
extern unsigned long from_fund(unsigned long);
extern void read_objs();
extern void restore_mobs_now();
extern void wroom(short, char *, short);
extern void quit1(SU);
extern void wall(char *, short);
extern void awall(char *, short, short);
extern void qquit(SU);
extern void stolower(char *);
extern short isoffline(char *);
extern short getuser(SU,char *);
extern void wuser(SU, char *);
extern void start_edit(SU,short,short,char *);
extern short read_obj(short);
extern void reset_obj(short);
void createsign(short room,char *text,short lang);
void savesigns();
void oload(SU);
extern long memory_usage();
extern void save_debates();
extern void save_home_objs();
extern void read_areas();
extern void read_guilds(char *);

void oload(SU)
{
	short obno=0;
	short ret=0;
	
	if (word_count<1) {
		wuser(uid,"Specify an object vnum.\n");
		return;
	}
	
	obno=atoi(word[1]);

	ret=read_obj(obno);
	if (ret!=1) {
		sprintf(text,"No such object.\n");
		//reset_obj(glob.curobj);
	}
	else
	{
		CO.room=UU.room;
		sprintf(text,"You create %s [%d]\n",CO.sdesc,obno);
	}
	wuser(uid,text);
}

void editsignsave(SU)
{
	UU.edit[strlen(UU.edit)]='\0';
	createsign(UU.room,UU.edit,81);
	savesigns();
}

void editsign(SU)
{
	short count=0;

	for (count=0;count<MAXOBJS;count++) {
		if (objs[count].vnum==-1) continue;
		if (objs[count].vnum!=SIGN_OBJ) continue;
		if (objs[count].room!=UU.room) continue;

		wuser(uid,"There is already a sign here.\n");
		return;
	}
	start_edit(uid,3,798,NULL);
}

void removesign(SU) 
{
	short count;
	short done=0;

	for (count=0;count<MAXOBJS;count++) {
		if (objs[count].vnum==-1) continue;
		if (objs[count].vnum!=SIGN_OBJ) continue;
		if (objs[count].room!=UU.room) continue;

		reset_obj(count);
		wuser(uid,"Sign removed.\n");
		done=1;
	}

	if (!done) {
		wuser(uid,"No Sign to remove.\n");
	}
	else
	{
		savesigns();
	}
}

void advance(SU)
{
	short target;

	if (word_count<1) {
		wuser(uid,"Usage: advance <playername>\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
	        wuser(uid,"Who?\n");
       		return;
        }

	adjxp(target,1000);
	sprintf(text,"Advancing %s by 1 level.\n",users[target].name);
	wuser(uid,text);
	sprintf(text,"%s has advanced you a level.\n",UU.name);
	wuser(target,text);
	sprintf(text,"[^Bstaff^N] %s advances %s.\n",UU.name,users[target].name);
	awall(text,NORM+1,uid);
}

void nuke(SU)
{
	short target;
	char tstr[30];

	if (word_count<2) {
		wuser(uid,"This command will permanetely ^Rdelete^N a player!\n");
		wuser(uid,"Usage: nuke <playername> Y\n");
		return;
	}

	if (word[2][0]!='Y') {
		wuser(uid,"You must specify a capital Y after the name to delete.\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
		target=isoffline(word[1]);
		if (target==-1) {
	                wuser(uid,"Who?\n");
       		        return;
		}
        }

	strcpy(tstr,users[target].name);
	stolower(tstr);

	if (target<MAXUSERS) {
		wuser(uid,"Kicking them off first!\n");
		wuser(target,"You are being logged off and your player file is being removed!\n");
		qquit(target);
	}

	wuser(uid,"Removing pfile!\n");
	sprintf(text,"%s%s",USERDIR,tstr);
	unlink(text);
	sprintf(text,"[^Bstaff^N] %s ^Rremoved^N from smud database.\n",tstr);
	awall(text,NORM+1,-1);
}

void watch(SU)
{
	short target;
/*	char tstr[30]; - Not used yet */

	if (word_count<1) {
		wuser(uid,"Watch who?\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
	        wuser(uid,"Who?\n");
       		return;
        }

	if(target==uid)
	{
		wuser(uid,"That's you, you narcisistic pervert!\n");
		return;
	}

	if (users[target].reportto==-1) {
		sprintf(text,"^rwarning: ^N%s is now seeing all that you see!\n",UU.name);
		wuser(target,text);
		sprintf(text,"You are now watching %s, DO NOT abuse this!\n",users[target].name);
		wuser(uid,text);
		users[target].reportto=uid;
	}
	else
	{
		sprintf(text,"^gwarning: ^N%s has now stopped snooping on you!\n",UU.name);
		wuser(target,text);
		sprintf(text,"You stop watching %s.\n",users[target].name);
		wuser(uid,text);
		users[target].reportto=-1;
	}

}

void boot(SU)
{
	short target;

	if (word_count<1) {
		wuser(uid,"Boot who?\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	wuser(target,"\nWell done, you've managed to annoy an IMM!\n\n");
	sprintf(text,"%s has been removed from the game for being annoying.\n",users[target].name);
	wall(text,-1);
	quit1(target);
}
	
void cunt(SU)
{
	short target;

	if (word_count<1) {
		wuser(uid,"Cunt Boot who?\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	wuser(target,"\nFuckoff, it has been decided you are a CUNT!\n\n");
	sprintf(text,"%s has been removed from the game for being an arsewipe\n",users[target].name);
	wall(text,-1);
	qquit(target);
}
	
void summon(SU)
{
	short target;

	if (word_count<1) {
		wuser(uid,"WizSummon who?\n");
		return;
	}

	target=getuser(uid,word[1]);
	
        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }
	
	sprintf(text,"%s has been summoned by %s.\n",users[target].name,UU.name);
	wroom(users[target].room,text,target);
	sprintf(text,"You have been summoned by %s.\n",UU.name);
	wuser(target,text);
	users[target].fight=-1;
	users[target].room=UU.room;

	sprintf(text,"%s arrives at the command of %s.\n",users[target].name,UU.name);
	wroom(UU.room,text,target);
}

void mrefresh(SU)
{
	wuser(uid,"^gRefreshing mobs.^N\n");
	restore_mobs_now();
}

void orefresh(SU)
{
	wuser(uid,"^gRefreshing objects.^N\n");
	glob.cr_objs=0;
	read_objs();
}

void givecash(SU)
{
	short target,amount;


	DEBUG("givecash")

	if (word_count<2) {
		wuser(uid,"Specify, who to and how much.\n");
		return;
	}

	target=getuser(uid,word[1]);

	if (target==-1) {
		wuser(uid,"No one of that name online.\n");
		return;
	}

	amount=atoi(word[2]);

	if ((amount<1) || (amount>5000)) {
		wuser(uid,"The amount must be between 1 and 5000 gold coins.\n");
		return;
	}

	users[target].cash+=from_fund(amount);

	sprintf(text,"%s gives you %d gold coins.\n",UU.name,amount);
	wuser(target,text);
	sprintf(text,"You give %s %d gold coins.\n",users[target].name,amount);
	wuser(uid,text);
}

void giveprac(SU)
{
	short target,amount;


	DEBUG("giveprac")

	if (word_count<2) {
		wuser(uid,"Specify, who to and how many.\n");
		return;
	}

	target=getuser(uid,word[1]);

	if (target==-1) {
		wuser(uid,"No one of that name online.\n");
		return;
	}

	amount=atoi(word[2]);

	if ((amount<1) || (amount>50)) {
		wuser(uid,"The amount must be between 1 and 50.\n");
		return;
	}

	users[target].prac+=amount;

	sprintf(text,"%s gives you %d extra practice points.\n",UU.name,amount);
	wuser(target,text);
	sprintf(text,"You give %s %d practice points.\n",users[target].name,amount);
	wuser(uid,text);
}

void mpurge(SU)
{
	short mob;
/*	short rc; - Not used yet */

	DEBUG("mpurge")

	mob=getmob(uid,comstr);
	if (mob==-1) {
		wuser(uid,"Nothing of that name here!\n");
		return;
	}
	if (mobs[mob].fight>-1) {
		wuser(uid,"That is currently fighting someone!\n");
		return;
	}

	sprintf(text,"You mob purge %s.\n",mobs[mob].sdesc);
	wuser(uid,text);
	reset_mob(mob);
}

void immon(SU)
{
	DEBUG("immon")

	if (UU.rank==NORM) {
		wuser(uid,"You are not an Immortal.\n");
		return;
	}

	if (UU.immstatus==1) {
		wuser(uid,"You are already in IMM mode.\n");
		return;
	}

	if (UU.fight!=-1) {
		wuser(uid,"Noway buster, finish fighting first.\n");
		return;
	}

	UU.immstatus=1;
	if (UU.groupno!=uid) {
		killgroup(uid);
		wuser(uid,"You can no longer group (you will however continue to follow).\n");
		UU.groupno=uid;
	}
	
	wuser(uid,"You are now in IMM mode.\n");
}

void immoff(SU)
{
	DEBUG("immoff")

	if (UU.immstatus==0) {
		wuser(uid,"You are already in PLAYER mode.\n");
		return;
	}

	UU.immstatus=0;
	if (UU.rank<GOD) {
		if (UU.room>=20000) {
			UU.leaveroom=guilds[getguild(UU.guild)].recall_room;
		}
		else
		{
			UU.room=guilds[getguild(UU.guild)].recall_room;
			UU.leaveroom=-1;
			sprintf(text,"%s arrives, blood thursty and ready for action.\n",UU.name);
			wroom(UU.room,text,uid);
		}
	}
	if(UU.brief)
	{
		brieflook(uid);
	} else {
		look(uid);
	}
	wuser(uid,"You are now in PLAYER mode and have recalled.\n");
}


void info(SU)
{
	struct tm *now;
	time_t t;
	short count;
	short uc=0,ur=0,um=0,ua=0,ui=0,ut=0;
	short ug=0,ul=0,ucl=0;
	short sale=0;
	short loop,loop2;
	char tit,tat;
	float scale;
	char oldtat='N';
	DEBUG("info")

	if (word_count>0) {
		if(!strcasecmp(word[1],"memory"))
		{
			sprintf(text,"Current memory usage: %u bytes\n",(unsigned int)memory_usage());
			wuser(uid,text);
			return;
		}

        if(!strcasecmp(word[1],"users"))
        {
		t = time(NULL);
		now = gmtime(&t);
                scale = 70.0/MAXUSERS;
                for(loop=0; loop<24; loop++)
                {
			sprintf(text,"%2d: |",loop+1);
                        for(loop2=1; loop2<70; loop2++)
                        {
                                tit='\0';
				tat='N';
                                if((loop2<glob.stats.users[(loop+1)%24].min*scale) && (tit=='\0') && (glob.stats.users[(loop+1)%24].min!=MAXUSERS))
				{
                                        tit='#';
					tat='R';
				}
                                if((loop2<glob.stats.users[(loop+1)%24].max*scale) && (tit=='\0') && (glob.stats.users[(loop+1)%24].max!=0))
				{
                                        tit='@';
					tat='G';
				}
                                if(tit=='\0')
                                        tit='.';
				if (oldtat!=tat) {
					sprintf(retval,"^%c",tat);
					strcat(text,retval);
					oldtat=tat;
				}
				sprintf(retval,"%c",tit);
				strcat(text,retval);
			}
			wuser(uid,text);
			wuser(uid,"\n");
                }
		wuser(uid,"\n");
		return;
        }
	}

	
	line(uid,"Game information");

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (UC.login>0) ucl++;
		uc++;
	}
	for (count=0;count<MAXMOBS;count++) {
		if (mobs[count].vnum==-1) continue;
		um++;
	}
	for (count=0;count<MAXROOMS;count++) {
		if (rooms[count].vnum==-1) continue;
		ur++;
	}
	for (count=0;count<MAXAREAS;count++) {
		if (areas[count].vnum==-1) continue;
		ua++;
	}
	for (count=0;count<MAXTRANS;count++) {
		if (trans[count].vnum==-1) continue;
		ut++;
	}
	for (count=0;count<MAXOBJS;count++) {
		if (objs[count].vnum==0) continue;
		ui++;
		if (objs[count].forsale) sale++;
	}
	for (count=0;count<MAXGUILDS;count++) {
		if (guilds[count].vnum==-1) continue;
		ug++;
	}
	for (count=0;count<MAX_LISTS;count++) {
		if (glob.lists[count].owner[0]=='\0') continue;
		ul++;
	}
	sprintf(text,"Users            %5d/%5d (%d not logged in yet) (%ld total pfiles)\n",uc,MAXUSERS,ucl,totalpfiles(uid));
	wuser(uid,text);
	sprintf(text,"Mobs             %5d/%5d\n",um,MAXMOBS);
	wuser(uid,text);
	sprintf(text,"Objects          %5d/%5d (%d for sale)\n",ui,MAXOBJS,sale);
	wuser(uid,text);
	sprintf(text,"Areas            %5d/%5d\n",ua,MAXAREAS);
	wuser(uid,text);
	sprintf(text,"Rooms            %5d/%5d\n",ur,MAXROOMS);
	wuser(uid,text);
	sprintf(text,"Guilds           %5d/%5d\n",ug,MAXGUILDS);
	wuser(uid,text);
	sprintf(text,"Lists            %5d/%5d\n",ul,MAX_LISTS);
	wuser(uid,text);
	sprintf(text,"Carriages        %5d/%5d\n",ut,MAXTRANS);
	wuser(uid,text);
	line(uid,NULL);
}

void xgoto(SU)
{
	short realno=0;
	short moved=0;

	DEBUG("xgoto")

	if (word_count<1) {
		wuser(uid,"You need to specify a character name or room vnum.\n");
		return;
	}

	/* try and see if it is a valid room vnum first */

	realno=getroom(atoi(comstr));

	if (realno>-1) {
		sprintf(text,"%s waves %s hands, opens a portal, then steps through it.\n",UU.name,getsex(uid,"his","hers","its"));
		wroom(UU.room,text,uid);
		UU.room=atoi(comstr);
		sprintf(text,"A portal opens and %s steps out.\n",UU.name);
		wroom(UU.room,text,uid);
		if(UU.brief)
		{
			brieflook(uid);
		} else {
			look(uid);
		}
		moved=1;
	}

	realno=getuser(uid,comstr);

	if (realno>-1) {
		sprintf(text,"%s waves %s hands, opens a portal, then steps through it.\n",UU.name,getsex(uid,"his","hers","its"));
		wroom(UU.room,text,uid);
		UU.room=users[realno].room;
		sprintf(text,"A portal opens and %s steps out.\n",UU.name);
		wroom(UU.room,text,uid);
		if(UU.brief)
		{
			brieflook(uid);
		} else {
			look(uid);
		}
		moved=1;
	}

	if (!moved) {
		wuser(uid,"That doesn't seem to be a room vnum nor a player name.\n");
	}
}

void locate(SU)
{
	short count,got=0,gotany=0;
	char tmpstr[255];

	DEBUG("locate")

	if (word_count<1) {
		wuser(uid, "You need to specify a search string.\n");
		return;
	}

	for (count=0;count<MAXROOMS;count++) {
		if (RC.vnum==-1) continue;
		strcpy(tmpstr,RC.name);
		stolower(tmpstr);
		if (strstr(tmpstr,comstr)) {
			gotany=1;
			sprintf(text,"[room](%05d) - %s\n",RC.vnum,RC.name);
			wuser(uid,text);
		}
	}

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		got=0;
		strcpy(tmpstr,MC.name);
		stolower(tmpstr);
		if (strstr(tmpstr,comstr)) got=1;
		strcpy(tmpstr,MC.sdesc);
		stolower(tmpstr);
		if (strstr(tmpstr,comstr)) got=1;
		if (got) {
			gotany=1;
			sprintf(text,"[mob ](%05d) - %s (%05d)\n",MC.room,MC.sdesc,MC.vnum);
			wuser(uid,text);
		}
	}

	if (!gotany) {
		wuser(uid,"Nothing found.\n");
	}
}

void xshutdown(SU)
{
	short die=0;
	FILE *fp;

	DEBUG("xshutdown")

	if (word_count<1) {
		wuser(uid,"Please specify ^Breboot^N or ^Rdie^N.\n");
		return;
	}

	if (strcmp(word[1],"die")==0) die=1;

	/* save all players online */
	if (!psave_all(1)) {
		syslog(SYS,"Failed saving players on exit\n");
	}
	save_debates();
	save_home_objs();

	if (die) /* shutdown and kill autorun */
	{
		if (!(fp=fopen(".killscript","w"))) {
			syslog(SYS,"Cannot kill 'autorun'\n");
		}
		else
		{
			fprintf(fp,"blah");
			fclose(fp);
		}
	}

	sprintf(text,"Smud is ^rshutting down^N, %s, sorry!\n",die?"and may be down for a while":"but will be backup in about 60 seconds");
	wall(text,-1);
	glob.exitsmud=1;
}

/* generic value command for admins */
void set(SU)
{
	short value;
/*	long lvalue; - Not used yet */

	DEBUG("set")

	if (word_count<1) { /* give a list of setable values */
		wuser(uid,"Set allows you to alter the following..\n");
		if (UU.rank>=GUIDE) {
		}
		if (UU.rank>=CARETAKER) {
		}
		if (UU.rank>=OVERSEER) {
		}
		if (UU.rank>=IMM) {
			sprintf(text,"%-20s (%ds) - Idle out time whilst active.\n","normidle",(int)glob.normidle);
			wuser(uid,text);
			sprintf(text,"%-20s (%ds) - Idle out time when in home room.\n","roomidle",(int)glob.roomidle);
			wuser(uid,text);
			sprintf(text,"%-20s (%dhr) - Object recreate interval (game hours)\n","objr",glob.r_objs);
			wuser(uid,text);
		}
		if (UU.rank>=GOD) {
		}
		return;
	}

	if (UU.rank<IMM) return; /* need to be IMM to do these */

	if (strcmp(word[1],"objr")==0)
	{
		if (word_count<2) {
			wuser(uid,"A value in game hours is needed.\n");
			return;
		}

		value=atoi(word[2]);
		if ((value<1)||(value>92)) {
			wuser(uid,"Out of range (1<->92)\n");
			return;
		}

		glob.r_objs=value;
		glob.cr_objs=0;
		sprintf(text,"Object reset set to %d game hours.\n",glob.r_objs);
		wuser(uid,text);
		sprintf(text,"%s has set 'objr' to %d\n",UU.name,glob.r_objs);
		awall(text,IMM,uid);
	}

	if (strcmp(word[1],"normidle")==0)
	{
		if (word_count<2) {
			wuser(uid,"normidle requires a value in seconds.\n");
			return;
		}

		value=atoi(word[2]);
		if ((value<180)||(value>14400)) {
			wuser(uid,"Out of range (180<->14400)\n");
			return;
		}

		glob.normidle=value;
		sprintf(text,"Normidle set to %d seconds (%d minutes)\n",(int)glob.normidle,(int)glob.normidle/60);
		wuser(uid,text);
		sprintf(text,"%s has set 'normidle' to %d\n",UU.name,(int)glob.normidle);
		awall(text,IMM,uid);
	}

	if (strcmp(word[1],"roomidle")==0)
	{
		if (word_count<2) {
			wuser(uid,"Roomidle requires a value in seconds.\n");
			return;
		}

		value=atoi(word[2]);
		if ((value<180)||(value>14400)) {
			wuser(uid,"Out of range (180<->14400)\n");
			return;
		}

		glob.roomidle=value;
		sprintf(text,"Roomidle set to %d seconds (%d minutes)\n",(int)glob.roomidle,(int)glob.roomidle/60);
		wuser(uid,text);
		sprintf(text,"%s has set 'roomidle' to %d\n",UU.name,(int)glob.roomidle);
		awall(text,IMM,uid);
	}
}

void rrefresh(SU)       
{
        short count,count2;
        
        printf("--> STARTING ROOM REFRESH\n");
         
        for(count=0; count<MAXAREAS; count++)
        {
                areas[count].vnum=-1;
                sprintf(areas[count].name,"%s","");
                sprintf(areas[count].fname,"%s","");
                sprintf(areas[count].author,"%s","");
                sprintf(areas[count].mobfile,"%s","");
                areas[count].lowlevel=0;
                areas[count].highlevel=0;
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


	for(count=0; count<MAXGUILDS; count++)
	{
		GC.vnum=-1;
	}
	read_areas();
	read_guilds("guilds.def");

        wuser(uid,"Room refresh finished.\n");
        return;
}
