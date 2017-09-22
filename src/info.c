/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : info.c
** Purpose : any functions with provide informational commands
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern char *timeval(time_t val);
extern short defval(short, short);
extern short attval(short, short);
extern char *getguilddef(short);
extern short checklist(SU,char *,short);
extern void newnews(SU);
extern short getmflag(short, short);
extern char *cap1st(char *);
extern char *getdir(short);
extern short canexit(SU,short);
extern short getguild(short);
extern char *getguildname(short);
extern short getrflag(short, short);
extern short isdark(short);
extern short getroom(short);
extern char *getsex(SU,char *, char *, char *);
extern void wroom(short, char *, short);
extern short getheldobj(SU,char *);
extern short getobj(SU,char *);
extern short getmob(SU,char *);
extern void line(SU,char *);
extern void wall(char *, short);
extern short isoffline(char *);
extern short getuser(SU,char *);
extern void wuser(SU, char *);
extern char *getskillname(short);
extern double pervis(SU);
extern short countmail(SU);
extern short checkmail(SU);

extern void wrapout(SU,char *,char,char);

void league_kills(SU);
void league_areas(SU);
void league_mobs(SU);
void genlook(SU,short brief);

void exits(SU);


char *smon[] = {
	"mad gerbil",
	"dessicated coconut",
	"blue bat",
	"flickering candle",
	"two headed she-goat",
	"magical plant",
	"green moon",
	"exploding apple",
	"dead rat",
	"cleft tree",
	"lost sock",
	"jellied eel"
};

struct tab_st {
	unsigned long xp;
	char name[80];
	time_t laston;
};

void report(SU)
{
	char out[4096];

	sprintf(out,"%d/%dhp %d/%dman %d/%dmv",UU.hp,UU.maxhp,UU.man,UU.maxman,UU.mv,UU.maxmv);
	sprintf(text,"You report: %s\n",out);
	wuser(uid,text);
	sprintf(text,"%s reports: %s\n",UU.name,out);
	wroom(UU.room,text,uid);
}

// sets users lpos attribute
// outputs a message if this has changed
// loud should be 1 if you want messages to be displayed
void checkleague(SU, short loud)
{
	struct dirent **namelist;
	int n;
	char text[100];
	long count=0;
	struct tab_st tab[TABSIZE+1];
	long count2=0,usr=0;
	long chg=1;
	long enddisp=0;
	long leg,pos;

	if (UU.lvl<2) return;
	

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory read.\n");
		return;
	}

	/* now scroll through all sorted entries */

	while (n--)
	{
		if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
			usr=getuser(uid,namelist[n]->d_name);
			if (usr==-1) {
				// user offline
				usr=isoffline(namelist[n]->d_name);
				if (usr==-1) {
					printf("borked userfile (%s)\n",namelist[n]->d_name);
					return;
				}
			}
			if (users[usr].xp>=2000) {
				strcpy(tab[count].name,users[usr].name);
				tab[count].xp=users[usr].xp;
				tab[count].laston=users[usr].lastlogin;
				count++;
			}
		}
		free(namelist[n]);
	}
	free(namelist);

        while(chg) {
                chg=0;
                for (count2=0;count2<count-1;count2++)
                {
			if (tab[count2].xp<tab[count2+1].xp) 
                        {
				tab[TABSIZE].xp=tab[count2+1].xp;
				tab[TABSIZE].laston=tab[count2+1].laston;
				strcpy(tab[TABSIZE].name,tab[count2+1].name);

				tab[count2+1].xp=tab[count2].xp;
				tab[count2+1].laston=tab[count2].laston;
				strcpy(tab[count2+1].name,tab[count2].name);
				
				tab[count2].xp=tab[TABSIZE].xp;
				tab[count2].laston=tab[TABSIZE].laston;
				strcpy(tab[count2].name,tab[TABSIZE].name);
				
                                chg=1;
                        }
                }
        }

	enddisp=count;

	chg=0;
	for (count2=0;count2<enddisp;count2++) {
		if (strcasecmp(tab[count2].name,UU.name)==0) {
			leg=(int) (count2/10);
			leg++;
			pos=(count2+1)%10;
			if (pos==0) pos=10;
			if ((count2+1>UU.lpos)&&UU.lpos!=0) {
				chg=1;
				sprintf(text,"^rYou have fallen to position %d in league %d.^N\n", (int)pos, (int)leg);
				if (loud) wuser(uid,text);
				sprintf(text,"^r%s has fallen to position %d in league %d.^N\n", UU.name, (int)pos, (int)leg);
			}
			if ((count2+1<UU.lpos)||UU.lpos==0) {
				chg=1;
				sprintf(text,"^yYou have reached position %d in league %d.^N\n", (int)pos, (int)leg);
				if (loud) wuser(uid,text);
				sprintf(text,"^y%s has reached position %d in league %d.^N\n", UU.name, (int)pos, (int)leg);
			}
			if (chg) {
				if (loud) wall(text,uid);
				UU.lpos=count2+1;
			}
		}
	}
}

void league(SU)
{
	struct dirent **namelist;
	int n;
	char text[100];
	long count=0;
	short table=0;
	struct tab_st tab[TABSIZE+1];
	long count2=0,usr=0;
	long chg=1;
	long disp=0;
	long pos;
	long enddisp=0;

	if(!strcasecmp(word[1],"areas") && (word_count>=1))
	{
		league_areas(uid);
		return;
	}

	if(!strcasecmp(word[1],"kills") && (word_count>=1))
	{
		league_kills(uid);
		return;
	}

	if(!strcasecmp(word[1],"mobs") && (word_count>=1))
	{
		league_mobs(uid);
		return;
	}

	if (word_count<1) {
		table=1;
	}
	else
	{
		table=atoi(word[1]);
		if (table<1) {
			wuser(uid,"No such table.\n");
			return;
		}
	}

	disp=(table-1)*10;

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory read.\n");
		return;
	}

	/* now scroll through all sorted entries */

	while (n--)
	{
		if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
			usr=getuser(uid,namelist[n]->d_name);
			if (usr==-1) {
				// user offline
				usr=isoffline(namelist[n]->d_name);
				if (usr==-1) {
					printf("borked userfile (%s)\n",namelist[n]->d_name);
					return;
				}
			}
			if (users[usr].xp>=2000) {
				strcpy(tab[count].name,users[usr].name);
				tab[count].xp=users[usr].xp;
				tab[count].laston=users[usr].lastlogin;
				count++;
			}
		}
		free(namelist[n]);
	}
	free(namelist);

        while(chg) {
                chg=0;
                for (count2=0;count2<count-1;count2++)
                {
			if (tab[count2].xp<tab[count2+1].xp) 
                        {
				tab[TABSIZE].xp=tab[count2+1].xp;
				tab[TABSIZE].laston=tab[count2+1].laston;
				strcpy(tab[TABSIZE].name,tab[count2+1].name);

				tab[count2+1].xp=tab[count2].xp;
				tab[count2+1].laston=tab[count2].laston;
				strcpy(tab[count2+1].name,tab[count2].name);
				
				tab[count2].xp=tab[TABSIZE].xp;
				tab[count2].laston=tab[TABSIZE].laston;
				strcpy(tab[count2].name,tab[TABSIZE].name);
				
                                chg=1;
                        }
                }
        }

	if (disp>count) {
		wuser(uid,"No such league... yet.\n");
		return;
	}

	enddisp=disp+10>count?count:disp+10;

	sprintf(text,"League division %d",table);
	line(uid,text);
	for (count2=disp;count2<enddisp;count2++) {
		pos=(count2+1)%10;
		if (pos==0) pos=10;
		sprintf(text,"%2d %-20s %d\n",(int)pos,tab[count2].name,(int)tab[count2].xp);
		wuser(uid,text);
	}
	line(uid,NULL);
}

void league_kills(SU)
{
        struct dirent **namelist;
        int n;
        char text[100];
        long count=0;
        short table=0;
        struct tab_st tab[TABSIZE+1];
        long count2=0,usr=0;
        long chg=1;
        long disp=0;
        long pos;
        long enddisp=0;
                                                
        if (word_count<2) {
                table=1;
        }
        else
        {
                table=atoi(word[2]);
                if (table<1) {
                        wuser(uid,"No such table.\n");
                        return;
                }
        }
                                        
        disp=(table-1)*10;

        n=scandir(USERDIR, &namelist, 0, alphasort);
        if (n < 0)       
        {
                wuser(uid, "Error on directory read.\n");
                return;
        }

        /* now scroll through all sorted entries */

        while (n--)
        {
                if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
                        usr=getuser(uid,namelist[n]->d_name);
                        if (usr==-1) {
                                // user offline
                                usr=isoffline(namelist[n]->d_name);
                                if (usr==-1) {
                                        printf("borked userfile (%s)\n",namelist[n]->d_name);
                                        return;
                                }
                        }
                        if (users[usr].kills>0) {
                                strcpy(tab[count].name,users[usr].name);
                                tab[count].xp=users[usr].kills;
                                tab[count].laston=users[usr].lastlogin;
                                count++;
                        }
                }
                free(namelist[n]);
        }
        free(namelist);

        while(chg) {
                chg=0;
                for (count2=0;count2<count-1;count2++)
                {
                        if (tab[count2].xp<tab[count2+1].xp)
                        {
                                tab[TABSIZE].xp=tab[count2+1].xp;
                                tab[TABSIZE].laston=tab[count2+1].laston;
                                strcpy(tab[TABSIZE].name,tab[count2+1].name);

                                tab[count2+1].xp=tab[count2].xp;
                                tab[count2+1].laston=tab[count2].laston;
                                strcpy(tab[count2+1].name,tab[count2].name);

                                tab[count2].xp=tab[TABSIZE].xp;
                                tab[count2].laston=tab[TABSIZE].laston;
                                strcpy(tab[count2].name,tab[TABSIZE].name);

                                chg=1;
                        }
                }
        }

        if (disp>count) {
                wuser(uid,"No such league... yet.\n");
                return;
        }

        enddisp=disp+10>count?count:disp+10;

        sprintf(text,"League division %d",table);
        line(uid,text);
        for (count2=disp;count2<enddisp;count2++) {
                pos=(count2+1)%10;
                if (pos==0) pos=10;
                sprintf(text,"%2d %-20s %d\n",(int)pos,tab[count2].name,(int)tab[count2].xp);
                wuser(uid,text);
        }
        line(uid,NULL);
}

void league_areas(SU)
{
        char text[100];
        long count=0;
        short table=0;
        struct tab_st tab[TABSIZE+1];
        long count2=0;
        long chg=1;
        long disp=0;
        long pos;
        long enddisp=0;
	short n;
         
        if (word_count<2) {         
                table=1;
        }
        else
        {
                table=atoi(word[2]);
                if (table<1) {    
                        wuser(uid,"No such table.\n");
                        return;
                }
        }

        disp=(table-1)*10;
	count=0;
	for(n=0; n<MAXAREAS; n++) 
	{
		if((areas[n].vnum>0) && (areas[n].vnum!=2000))
		{
			strcpy(tab[count].name,areas[n].name);
			tab[count].xp=areas[n].kills;
			count++;
		}
        }

        while(chg) {
                chg=0;
                for (count2=0;count2<count-1;count2++)
                {
                        if (tab[count2].xp<tab[count2+1].xp)
                        {
                                tab[TABSIZE].xp=tab[count2+1].xp;
                                strcpy(tab[TABSIZE].name,tab[count2+1].name);

                                tab[count2+1].xp=tab[count2].xp;
                                strcpy(tab[count2+1].name,tab[count2].name);

                                tab[count2].xp=tab[TABSIZE].xp;
                                strcpy(tab[count2].name,tab[TABSIZE].name);

                                chg=1;
                        }
                }
        }

        if (disp>count) {
                wuser(uid,"No such league... yet.\n");
                return;
        }

        enddisp=disp+10>count?count:disp+10;

        sprintf(text,"League division %d",table);
        line(uid,text);
        for (count2=disp;count2<enddisp;count2++) {
                pos=(count2+1)%10;
                if (pos==0) pos=10;
                sprintf(text,"%2d %-20s %d\n",(int)pos,tab[count2].name,(int)tab[count2].xp);
                wuser(uid,text);
        }
        line(uid,NULL);
}

void league_mobs(SU)
{
        char text[100];    
        long count=0;   
        short table=0;
        struct tab_st tab[TABSIZE+1];
        long count2=0;      
        long chg=1;
        long disp=0;
        long pos;
        long enddisp=0;
        short n;
                   
        if (word_count<2) {
                table=1;
        }
        else
        {
                table=atoi(word[2]);
                if (table<1) {
                        wuser(uid,"No such table.\n");   
                        return;
                }
        }

        disp=(table-1)*10;
        count=0;
        for(n=0; n<MAXMOBS; n++)
        {
                if((glob.mobkills[n].vnum>0) && (glob.mobkills[n].kills>0))
                {
                        strcpy(tab[count].name,glob.mobkills[n].name);
                        tab[count].xp=glob.mobkills[n].kills;
                        count++;
                }
        }

        while(chg) {
                chg=0;
                for (count2=0;count2<count-1;count2++)
                {
                        if (tab[count2].xp<tab[count2+1].xp)
                        {
                                tab[TABSIZE].xp=tab[count2+1].xp;
                                strcpy(tab[TABSIZE].name,tab[count2+1].name);

                                tab[count2+1].xp=tab[count2].xp;
                                strcpy(tab[count2+1].name,tab[count2].name);

                                tab[count2].xp=tab[TABSIZE].xp;
                                strcpy(tab[count2].name,tab[TABSIZE].name);

                                chg=1;
                        }
                }
        }

        if (disp>count) {
                wuser(uid,"No such league... yet.\n");
                return;
        }

        enddisp=disp+10>count?count:disp+10;

        sprintf(text,"League division %d",table);
        line(uid,text);
        for (count2=disp;count2<enddisp;count2++) {
                pos=(count2+1)%10;
                if (pos==0) pos=10;
                sprintf(text,"%2d %-50s %d\n",(int)pos,tab[count2].name,(int)tab[count2].xp);
                wuser(uid,text);
        }
        line(uid,NULL);
}

void examine(SU)
{
	short usr=0;
	short obj, heldobj;
	short count,mob=-1;
	double per;
	short cl=0;

	DEBUG("examine")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		usr=uid;
	}
	else
	{
		usr=getuser(uid,comstr);
		if (usr!=-1) {
			if (users[usr].room!=UU.room) usr=-1;
		}
		else
		{
			usr=getmob(uid,comstr);
			if (usr!=-1) {
				if (mobs[usr].room!=UU.room) usr=-1;
			}
			mob=usr;
		}
	}

	if (usr==-1) {
		/* not a user, maybe an object ? */
		obj=getobj(uid, comstr);
		if (obj==-1) {
			/* Maybe a held object? */
			heldobj=getheldobj(uid, comstr);
			if (heldobj==-1) {
				wuser(uid, "You do not see that here.\n");
				return;
			}
			/* okay it is */
			obj=UU.held[heldobj];
		}
		/* Examine object */
		sprintf(text,"%s\n",objs[obj].desc[0]=='\0'?"You see nothing special about it.":objs[obj].desc);
		wrapout(uid,text,'F','N');
		if (objs[obj].writing[0]!='\0') {
			sprintf(text,"It appears to have some %s text written on it.\n",getskillname(objs[obj].language));
			wrapout(uid,text,'F','N');
		}
		sprintf(text,"It is %d hours old.\n",objs[obj].age);
		wuser(uid,text);
		if(objs[obj].dropname[0]!='\0')
		{
			sprintf(text,"It was dropped by %s\n",objs[obj].dropname);
			wuser(uid,text);
		}
		if(objs[obj].mob_drop>-1)
		{
			sprintf(text,"It was dropped by mob number %d\n",objs[obj].mob_drop);
			wuser(uid,text);
		}

		sprintf(text,"%s examines %s.\n",UU.name,objs[obj].sdesc);
		wroom(UU.room,text,uid);
		return;
	}
	/* Examine a user (could be UU) */
	
	if (mob==-1) {
		sprintf(text,"%s looks at %s\n",UU.name,usr==uid?(char *)getsex(uid,"himself","herself","itself"):users[usr].name);
	}
	else
	{
		sprintf(text,"%s looks at %s\n",UU.name,cap1st(mobs[usr].name));
	}
	wroom(UU.room,text,uid);

	if (mob!=-1) {
		wuser(uid,"\n");
		wrapout(uid,mobs[usr].ldesc,'F','N');
		wuser(uid,"\n");
	}
	
	per=((double)(mob==-1?users[usr].hp:mobs[usr].hp)/(double)(mob==-1?users[usr].maxhp:mobs[usr].maxhp))*(double)100;
	sprintf(text,"%s is in perfect health.\n\n",mob==-1?users[usr].name:mobs[usr].name);
	if (per<90) {
		sprintf(text,"%s is looking a little off colour.\n\n",mob!=-1?mobs[usr].name:users[usr].name);
	}
	if (per<60) {
		sprintf(text,"%s is covered in cuts and bruises.\n\n",mob!=-1?mobs[usr].name:users[usr].name);
	}
	if (per<30) {
		sprintf(text,"%s is blooded and battered.\n\n",mob!=-1?mobs[usr].name:users[usr].name);
	}
	if (per<10) {
		sprintf(text,"%s is near death.\n\n",mob!=-1?mobs[usr].name:users[usr].name);
	}
	wuser(uid,text);
	for (count=0;count<13;count++) {
		if (mob==-1) {
			if (users[usr].worn[count]>-1) {
				if (cl==0) {
					sprintf(text,"%s is using:\n",mob!=-1?mobs[usr].name:users[usr].name);
					wuser(uid,text);
					cl=1;
				}
				sprintf(text,"%-22s ^m%s^N\n",wornstr[count],objs[users[usr].worn[count]].sdesc);
				wuser(uid,text);
			}
		}
		else
		{
			if (mobs[usr].worn[count]>-1) {
				if (cl==0) {
					sprintf(text,"%s is using:\n",mob!=-1?mobs[usr].name:users[usr].name);
					wuser(uid,text);
					cl=1;
				}
				sprintf(text,"%-22s ^m%s^N\n",wornstr[count],objs[mobs[usr].worn[count]].sdesc);
				wuser(uid,text);
			}
		}
	}
	if (cl==0) {
		sprintf(text,"%s is buck naked!\n",mob!=-1?mobs[usr].name:users[usr].name);
		wuser(uid,text);
	}
	wuser(uid,"\n");
}

// why have two functions? I hear you cry, well basically to keep the main
// command case in smud.c as simple as possible, and besides it's flibs's
// fault ;)
void look(SU)
{
	genlook(uid,0);
}

void brieflook(SU)
{
	genlook(uid,1);
}

void genlook(SU,short brief)
{
	short rc=0,cr=0;
	short count=0;
	short itsdark=0;
	short mobshere=0;
	short newmail=0;

	DEBUG("look")

	rc=getroom(UU.room);

	if (UU.sleep==2) {
		wuser(uid,"You can only see the inside of your eyelids.\n");
		return;
	}

	itsdark=isdark(UU.room);
	if (itsdark) {
		wuser(uid,"^rIt is pitch black.^N\n");
	}

	if (!itsdark || UU.immstatus==1)
	{
		sprintf(text,"^r%s^N",rooms[rc].name);
		wuser(uid,text);
		if (UU.rank>NORM && UU.immstatus==1) {
			sprintf(text," [^R%05d^N]\n",rooms[rc].vnum);
			wuser(uid,text);
		}
		else
		{
			wuser(uid,"\n");
		}
		if (rooms[rc].guild!=-1) {
			sprintf(text,"^gPart of %s. %s^N\n",getguildname(rooms[rc].guild),guilds[getguild(rooms[rc].guild)].pracroom==rooms[rc].vnum?"(training room)":"");
			wuser(uid,text);
		}
		sprintf(text,"^N^y[Exits:");
		for (count=0;count<10;count++) {
			if (rooms[rc].dir[count]>0) {
				if (!canexit(uid,count)) continue;
				cr++;
				strcat(text," ");
				strcat(text,(char *)getdir(count));
			}
		}
		if (!cr) { /* no exits */
			strcat(text," none ");
		}
		strcat(text,"]^N\n");
		wuser(uid,text);
		if (!brief) {
			sprintf(text,"^N%s\n",rooms[rc].desc);
			wrapout(uid,text,'J','N');
		}

		if(UU.autoexit) 
		{
			wuser(uid,"\n");
			exits(uid);
			wuser(uid,"\n");
		}

		/* show any carriages here */
		for (count=0;count<MAXTRANS;count++) {
			if (trans[count].vnum==-1) continue;
			if (rooms[getroom(trans[count].room)].dir[trans[count].exit]!=UU.room) continue;

			// carriage is here
			sprintf(text,"^m%s is waiting here^N (^gcarriage^N)\n",trans[count].name);
			wuser(uid,text);
		}

		/* Show any objects present */
		for (count=0;count<MAXOBJS;count++) {
			if (OC.name[0]=='\0') continue;
			if (OC.room==-1) continue;
			if (OC.room==UU.room) {
				sprintf(text,"^m%s has been left here.^N\n",cap1st(OC.sdesc));
				wuser(uid,text);
			}
		}

		/* Show doors */
		for (count=0;count<10;count++) {
			if (rooms[rc].door[count][0]=='Y') {
				sprintf(text,"The door which leads %s is %s.\n",getdir(count),rooms[rc].door[count][2]=='O'?"open":"closed");
				wuser(uid,text);
			}
		}
	}

	/* Show the mobs present */
	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.room==UU.room) {
			if (!itsdark || UU.immstatus==1) {
				sprintf(text,"^R%s is here. %s^N\n",cap1st(MC.sdesc),getmflag(count,mHEALER)?"^N(^ghealer^N)":getmflag(count,mSHOPKEEPER)?"^N(^gshopkeeper^N)":"");
				wuser(uid,text);
			}
			mobshere=1;
		}
	}

	if (itsdark&&mobshere) {
		wuser(uid,"You see glowing ^RRED^N eyes following you!\n");
	}

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (count==uid) continue;
		if (UC.hidden>0) continue;
		if (UC.room==UU.room) {
			sprintf(text,"^b%s the level %d %s is %s here.%s^N\n",UC.name,UC.lvl,getguilddef(UC.guild),UC.sleep==0?"standing":UC.sleep==1?"resting":"sleeping",UC.invis?" [invis]":"");
			wuser(uid,text);
		}
	}
	newnews(uid);
	if (UU.room==UU.homeroom) {
		newmail=checkmail(uid);
		if(newmail>0)
		{
			sprintf(text,"^WThere %s ^Y%d^W new letter%s waiting in your letterbox.\n",newmail==1?"is":"are",newmail,newmail==1?"":"s");
			wuser(uid,text);
		}
	}
}


void affects(SU)
{
	short got=0;

	DEBUG("affects")
	
	line(uid,"You are affected by");

	if (UU.invis) {
		sprintf(text,"  Invisibility for the next %d hours.\n",UU.invis);
		wuser(uid,text);
		got=1;
	}
	if (!got) {
		wuser(uid,"  Nothing.\n");
	}
	line(uid,NULL);
}
	
void outwho(SU,short friend)
{
	short count=0;
	char afkmsg[30];
	short uo=0;

	DEBUG("who")

	if (friend) {
		line(uid,"Friends online");
	}
	else
	{
		line(uid,"Users online");
	}
	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.login>0) continue;
		if ((!checklist(uid,UC.name,lFRIEND))&&(friend)) continue;
		if (UC.afk!=0) {
			if (UC.afk!=1) {
				sprintf(afkmsg,"[%d afk]",(int)((UC.afk-time(0))/60));
			}
			else
			{
				sprintf(afkmsg,"[afk]");
			}
		}
		sprintf(text,"[%s%s%s%s] %s the level %d %s\n",
			UC.rank>NORM+1?(UC.immstatus?"^YI^N":"^yi^N"):".",
			UC.mccp?"^GM^N":".",
			UC.afk?"^MA^N":".",
			UC.eid?"^BE^N":UC.fight!=-1?"^bF^N":".",
			UC.name,
			UC.lvl,
			(char *)getguilddef(UC.guild));
		/*
		sprintf(text,"[%s%s%s%s] %4d %-10s - %s %s\n",
			UC.rank>NORM+1?(UC.immstatus?"^YI^N":"^yi^N"):".",
			UC.mccp?"^GM^N":".",
			UC.afk?"^MA^N":".",
			UC.eid?"^BE^N":".",
			UC.lvl,
			(UC.lvl<50)?(char *)getguilddef(UC.guild):"Hero",
			UC.name,UC.afk!=0?afkmsg:"");
			*/
		wuser(uid,text);
		uo++;
	}
	if (friend) {
		if (uo) {
			sprintf(text,"%d player%s",uo,uo==1?"":"s");
		}
		else
		{
			sprintf(text,"No friends on at the moment");
		}
	}
	else
	{
		if (uo<2) {
			sprintf(text,"Just you here");
		}
		else
		{
			sprintf(text,"%d players",uo);
		}
	}
	line(uid,text);
}

void who(SU)
{
	outwho(uid,0);
}

void checkinvtext(SU,short nextlen)
{
	DEBUG("checkinvtext")
	
	if (strlen(text)+nextlen>(UU.terminal.width-5)) {
		strcat(text,"\n");
		wuser(uid,text);
		sprintf(text,"          ");
	}
}

void inventory(SU)
{
	short count;
	short shop[1024];
	short quan[1024];
	short num=0;
	short got=0;
	short count2=0;
	short comma=0;
	char tmp[128];

	DEBUG("inventory")

	line(uid,"Inventory");
	for (count=0;count<MAX_HELD;count++) {
		if (UU.held[count]==-1) continue;
		got=0;
		for (count2=0;count2<num;count2++) {
			if (shop[count2]==objs[UU.held[count]].vnum) {
				quan[count2]++;
				got=1;
			}
		}
		if (got) continue;
		shop[num]=objs[UU.held[count]].vnum;
		quan[num]=1;
		num++;
	}

	/* Held/weild */
	sprintf(text,"^rHolding :^N ");
	if (UU.worn[10]!=-1) {
		checkinvtext(uid,strlen(objs[UU.worn[10]].sdesc));
		strcat(text,objs[UU.worn[10]].sdesc);
		comma=1;
	}
	if (UU.worn[11]!=-1) {
		if (comma) strcat(text,", ");
		checkinvtext(uid,strlen(objs[UU.worn[11]].sdesc));
		strcat(text,objs[UU.worn[11]].sdesc);
		strcat(text," (wielded)");
		comma=1;
	}
	if (UU.worn[12]!=-1) {
		if (comma) strcat(text,", ");
		checkinvtext(uid,strlen(objs[UU.worn[12]].sdesc));
		strcat(text,objs[UU.worn[12]].sdesc);
		strcat(text," (used as shield)");
		comma=1;
	}
	if (comma) {
		strcat(text,".\n");
	}
	else
	{
		strcat(text,"Nothing.\n");
	}
	wuser(uid,text);


	/* Wearing */
	comma=0;
	sprintf(text,"^YWearing :^N ");
	for (count=0;count<10;count++) {
		if (UU.worn[count]!=-1) {
			if (comma) {
				strcat(text,", ");
			}
			checkinvtext(uid,strlen(objs[UU.worn[count]].sdesc));
			strcat(text,objs[UU.worn[count]].sdesc);
			comma=1;
		}
	}
	if (comma) {
		strcat(text,".\n");
	}
	else
	{
		strcat(text,"Nothing.\n");
	}
	wuser(uid,text);

	/* Carrying */
	comma=0;
	sprintf(text,"^GCarrying:^N ");
	for (count=0;count<num;count++) {
		for (count2=0;count2<MAXOBJS;count2++) {
			if (objs[count2].vnum==0) continue;
			if (objs[count2].user!=uid) continue;
			if (objs[count2].vnum==shop[count]) {
				if (comma) {
					strcat(text,", ");
				}
				checkinvtext(uid,strlen(objs[count2].sdesc));
				strcat(text,objs[count2].sdesc);
				if (quan[count]>1) {
					sprintf(tmp," (%d)",quan[count]);
					if (quan[count]>5) {
						sprintf(tmp," (many)");
					}
					strcat(text,tmp);
				}
				shop[count]=-1;
				comma=1;
			}
		}
	}
	if (comma) {
		strcat(text,".\n");
	}
	else
	{
		strcat(text,"Nothing.\n");
	}
	wuser(uid,text);
	if (UU.cash) {
		sprintf(text,"Your purse contains %d gold coin%s.\n",(int)UU.cash,UU.cash==1?"":"s");
	}
	else
	{
		sprintf(text,"Your purse is empty.\n");
	}
	wuser(uid,text);
	line(uid,NULL);
}

void score(SU)
{
	short rat=0;
	char tmp[1024];
	DEBUG("score")
		sprintf(text,"You are %s, the level %d %s",UU.name,UU.lvl,getguilddef(UU.guild));
		line(uid,text);
		sprintf(text,"str: %02d(%02d), ",UU.str,UU.maxstr);
		wuser(uid,text);
		sprintf(text,"con: %02d(%02d), ",UU.con,UU.maxcon);
		wuser(uid,text);
		sprintf(text,"dex: %02d(%02d), ",UU.dex,UU.maxdex);
		wuser(uid,text);
		sprintf(text,"int: %02d(%02d), ",UU.intl,UU.maxint);
		wuser(uid,text);
		sprintf(text,"wis: %02d(%02d)\n",UU.wis,UU.maxwis);
		wuser(uid,text);
		sprintf(text,"Hit %d(%d), Mana %d(%d), Move %d(%d)\n",UU.hp,UU.maxhp,UU.man,UU.maxman,UU.mv,UU.maxmv);
		wuser(uid,text);
		sprintf(text,"Your experience is %d, making you level %d.\n",(int)UU.xp,UU.lvl);
		wuser(uid,text);
		rat=attval(uid,0)+defval(uid,0);
		sprintf(tmp,"more or less naked");
		if (rat>5) sprintf(tmp,"soft and cuddly");
		if (rat>20) sprintf(tmp,"as hard as brie");
		if (rat>35) sprintf(tmp,"slightly fearsome");
		if (rat>50) sprintf(tmp,"a little scary");
		if (rat>65) sprintf(tmp,"an apprentice maniac");
		if (rat>80) sprintf(tmp,"a cool customer");
		if (rat>99) sprintf(tmp,"a tough cat");
		if (rat>120) sprintf(tmp,"a real bruiser");
		if (rat>135) sprintf(tmp,"hard as nails");
		if (rat>150) sprintf(tmp,"a force to be reckoned with");
		if (rat>165) sprintf(tmp,"virtually indestructable");
		if (rat>180) sprintf(tmp,"bordering on god like");
		if (rat>200) sprintf(tmp,"a bringer of BAD things");
		sprintf(text,"Your current rating is %d making you %s.\n",rat,tmp);
		wuser(uid,text);
		sprintf(tmp,"Devil like");
		if (UU.align>-300) sprintf(tmp,"somewhat of a demon");
		if (UU.align>-100) sprintf(tmp,"pretty bad");
		if (UU.align>-10) sprintf(tmp,"fairly neutral");
		if (UU.align>200) sprintf(tmp,"quite good");
		if (UU.align>600) sprintf(tmp,"saintly");
		if (UU.align>800) sprintf(tmp,"almost GOD like");

		sprintf(text,"Your current alignment is %d, making you %s.\n",UU.align,tmp);
		wuser(uid,text);
		sprintf(text,"You are a member of %s.\n",getguildname(UU.guild));
		wuser(uid,text);
		sprintf(text,"You have %d practice point%s left.\n",UU.prac,UU.prac==1?"":"s");
		wuser(uid,text);
		sprintf(text,"Your wimpy is set to %d hitpoint%s.\n",UU.wimpy,UU.wimpy==1?"":"s");
		wuser(uid,text);
/*		sprintf(text,"Your current weapon %s (%c).\n",gettypes(uid,0),objs[UU.worn[11]].type);
		wuser(uid,text);
		*/
		if (UU.homeroom!=-1) {
			sprintf(text,"Your homeroom is currently %s by others.\n",UU.visitable?"visitable":"unvisitable");
		}
		else
		{
			sprintf(text,"You do not yet have a homeroom.\n");
		}
		wuser(uid,text);

		line(uid,NULL);
}

struct sarea_st {
	short idx;
	short low;
	short high;
	short croom;
};

void xareas(SU)
{
	short count;
	short count2,croom,carea=0;
	char levels[20];
	short low,high,val;
	struct sarea_st sareas[MAXAREAS+1];
	short sort=0;
	short chg=1;
	short rat=0;
	char hunt[80];

	rat=attval(uid,0)+defval(uid,0);
	line(uid,"Areas");

	for (count=0;count<MAXAREAS;count++) {
		if (areas[count].vnum==-1) continue;
		croom=0;
		for (count2=0;count2<MAXROOMS;count2++) {
			if (rooms[count2].vnum==-1) continue;
			if (rooms[count2].area!=areas[count].vnum) continue;
			croom++;
		}
		low=999;
		high=0;
		for (count2=0;count2<MAXMOBS;count2++) {
			if (mobs[count2].vnum==-1) continue;
			if (rooms[getroom(mobs[count2].room)].area!=areas[count].vnum) continue;
			if (getmflag(count2,mSHOPKEEPER)) continue;
			if (getmflag(count2,mNOCOUNT)) continue;

			val=defval(count2,1)+attval(count2,1);
			if (val>high) high=val;
			if (val<low) low=val;
		}
		sareas[sort].idx=count;
		sareas[sort].low=low;
		sareas[sort].high=high;
		sareas[sort].croom=croom;
		sort++;
	}

	while(chg) {
		chg=0;
		for (count=0;count<sort-1;count++) {
			if (sareas[count].high>sareas[count+1].high) {
				sareas[MAXAREAS].idx=sareas[count].idx;
				sareas[MAXAREAS].low=sareas[count].low;
				sareas[MAXAREAS].high=sareas[count].high;
				sareas[MAXAREAS].croom=sareas[count].croom;

				sareas[count].idx=sareas[count+1].idx;
				sareas[count].low=sareas[count+1].low;
				sareas[count].high=sareas[count+1].high;
				sareas[count].croom=sareas[count+1].croom;

				sareas[count+1].idx=sareas[MAXAREAS].idx;
				sareas[count+1].low=sareas[MAXAREAS].low;
				sareas[count+1].high=sareas[MAXAREAS].high;
				sareas[count+1].croom=sareas[MAXAREAS].croom;
				chg=1;
			}
		}
	}

	for (count=0;count<sort;count++) {
		sprintf(levels,"%-3d %3d",sareas[count].low,sareas[count].high);
		if (sareas[count].low==999 && sareas[count].high==0) {
			sprintf(levels,"  All  ");
		}
		sprintf(hunt,"%s",rat<sareas[count].high&&rat>sareas[count].low?"^Ghunt here?^N":"");
		sprintf(text,"[%s] %-30s %-10s (%02d rooms) %s\n",levels,areas[sareas[count].idx].name,areas[sareas[count].idx].author,sareas[count].croom,hunt);
		wuser(uid,text);
		carea++;
	}
	sprintf(text,"%d areas",carea);
	line(uid,text);
}

void bug(SU)
{
	DEBUG("bug")
	if (UU.email[0]=='\0') {
		wuser(uid,"You must set an email address to use this command.\n");
		return;
	}
	if (word_count<1) {
		wuser(uid,"Log what as a bug?\n");
		return;
	}
	sprintf(text,"echo \"FROM:%s\nSUBJECT:Bug Report\n\nBUG LOG\n\nLogged by:%s\n\nText:\n%s\n\" | /usr/sbin/sendmail %s",UU.email,UU.name,comstr,EMAILBUG);
	system(text);
	wuser(uid,"Bug logged, thankyou!\n");
}

void typo(SU)
{
	DEBUG("typo")
	if (UU.email[0]=='\0') {
		wuser(uid,"You must set an email address to use this command.\n");
		return;
	}
	if (word_count<1) {
		wuser(uid,"Log what as a typo?\n");
		return;
	}
	sprintf(text,"echo \"FROM:%s\nSUBJECT:Typo Report\n\nTYPO LOG\n\nLogged by:%s\n\nText:\n%s\n\" | /usr/sbin/sendmail %s",UU.email,UU.name,comstr,EMAILTYPO);
	system(text);
	wuser(uid,"Typo logged, thankyou!\n");
}

void idea(SU)
{
	DEBUG("idea")
	if (UU.email[0]=='\0') {
		wuser(uid,"You must set an email address to use this command.\n");
		return;
	}
	if (word_count<1) {
		wuser(uid,"Log what as an idea?\n");
		return;
	}
	sprintf(text,"echo \"FROM:%s\nSUBJECT:Idea Report\n\nIDEA LOG\n\nLogged by:%s\n\nText:\n%s\n\" | /usr/sbin/sendmail %s",UU.email,UU.name,comstr,EMAILIDEA);
	system(text);
	wuser(uid,"Idea logged, thankyou!\n");
}

void finger(SU)
{
        short target;
	char *tname;
	char text2[128];
	struct tm *ttm;
	time_t tt;
	double per;
	char *term=NULL;

        DEBUG("finger")

	tname=word[1];

        if (word_count<1) {
		tname=UU.name;
	}

        target=getuser(uid,tname);

	if (target==-1) {
		target=isoffline(tname);
	}

        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	/* Finger info, only works for online players atm
	 */
	sprintf(text2,"Finger info for %s",users[target].name);
	line(uid,text2);
	sprintf(text,"Status            : level %d %s. %s\n",users[target].lvl,getguilddef(users[target].guild), (UU.rank>NORM&&UU.immstatus)?((users[target].homeroom==-1)?"(no homeroom)":"(has homeroom)"):"");
	wuser(uid,text);
	ttm=(struct tm *)gmtime((time_t *)&users[target].firstlog);
	sprintf(text,"First logged on   : %02d/%02d/%02d, %02d:%02d\n",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year,ttm->tm_hour,ttm->tm_min);
	wuser(uid,text);
	if (target<MAXUSERS) {
		ttm=(struct tm *)gmtime((time_t *)&users[target].thislog);
		sprintf(text,"Has been on since : %02d/%02d/%02d, %02d:%02d\n",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year,ttm->tm_hour,ttm->tm_min);
		wuser(uid,text);
	}
	else
	{
		ttm=(struct tm *)gmtime((time_t *)&users[target].laston);
		sprintf(text,"Last logged on    : %02d/%02d/%02d, %02d:%02d\n",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year,ttm->tm_hour,ttm->tm_min);
		wuser(uid,text);
	}
	tt=users[target].alllog+(time(0)-users[target].thislog);
//	sprintf(text,"Total logon time  : %ddays %dhrs %dmins %dsecs\n",(int)tt/86400,((int)tt%86400)/3600,((int)tt%3600)/60,((int)tt%60));
	sprintf(text,"Total logon time  : %s\n",timeval(tt));
	wuser(uid,text);
	sprintf(text,"Has been killed   : %d time%s\n",(int)users[target].died,users[target].died==1?"":"s");
	wuser(uid,text);
	sprintf(text,"Has killed        : %ld time%s\n",users[target].kills,users[target].kills==1?"":"s");
	wuser(uid,text);
	if (users[target].lastdied>0) {
		ttm=(struct tm *)gmtime((time_t *)&users[target].lastdied);
		sprintf(text,"Last Died         : %02d/%02d/%02d, %02d:%02d\n",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year,ttm->tm_hour,ttm->tm_min);
		wuser(uid,text);
	}
	sprintf(text,"Explored          : %-6.2f%% of available world\n",pervis(target));
	wuser(uid,text);
	sprintf(text,"MCCP version      : %s",users[target].mccp==0?"Not used":users[target].mccp==1?"One":"Two");
	wuser(uid,text);
	if (users[target].mccp) {
		per=((double)users[target].compout/(double)users[target].rawout)*100;
		sprintf(text,", %ldb compressed to %ldb (%5.2f%%)\n",users[target].rawout,users[target].compout,100-per);
		wuser(uid,text);
	}
	else wuser(uid,"\n");

	sprintf(text,"Mail messages     : %d (%d new)\n",countmail(target),checkmail(target));
	wuser(uid,text);

	if (UU.rank>=IMM) {
		line(uid,"Immortal information");
		sprintf(text,"Email Address     : %s\n",users[target].email[0]=='\0'?"unset":users[target].email);
		wuser(uid,text);
		if (target<MAXUSERS) {
			sprintf(text,"Logged in from    : %s (%s)\n",users[target].addr,users[target].curip);
		}
		else
		{
			sprintf(text,"Last logged IP    : %s\n",users[target].lastip);
		}
		wuser(uid,text);

		if (users[target].terminal.type[0]!='\0') {
			term=users[target].terminal.type;
		}
		if (users[target].defterm[0]!='\0') {
			if (users[target].defprec||users[target].terminal.type[0]=='\0') {
				term=users[target].defterm;
			}
		}
		if (!term) term="unknown";

		sprintf(text,"Terminal          : %s (%dx%d)\n",term,users[target].terminal.width,users[target].terminal.height);
		wuser(uid,text);
	}

	line(uid,NULL);
}

void criminal(SU)
{
	line(uid,"Criminal record");
	if (UU.lastfined==0) {
		wuser(uid,"You have a clean record, well done.\n");
		line(uid,NULL);
		return;
	}
	sprintf(text,"You have been fined for public nakedness %d time%s.\n",(int)UU.naked_fine,UU.naked_fine==1?"":"s");
	wuser(uid,text);
	sprintf(text,"You have been fined for littering %d time%s.\n",(int)UU.litter_fine,UU.litter_fine==1?"":"s");
	wuser(uid,text);

	line(uid,NULL);
}

void colour(SU)
{
	if (UU.colour) {
		UU.colour=0;
		wuser(uid,"You turn colour OFF.\n");
	}
	else
	{
		UU.colour=1;
		wuser(uid,"You turn colour ^YON.^N\n");
	}
}

void xtime(SU)
{
	short hour,d;
	unsigned long timenow;
	struct tm *ttm;

	DEBUG("xtime")
	if (glob.gamehour>=12) {
		/* PM */
		hour=glob.gamehour==12?12:glob.gamehour-11;
	}
	else
	{
		/* AM */
		hour=glob.gamehour==0?12:glob.gamehour;
	}

	d=(glob.gameday%30)+1;
	sprintf(text,"It is %d o'clock %s, on the %d%s day in the month of the %s.\n",hour,(glob.gamehour>12||glob.gamehour==0)?"pm":"am",d,
			(d==1||d==21)?"st":(d==2||d==22)?"nd":(d==3||d==23)?"rd":"th",smon[(int)(glob.gameday/30)]);
	wuser(uid,text);


	ttm=(struct tm *)gmtime((time_t *)&glob.startup);
	sprintf(text,"Smud was started at %02d:%02d on %02d/%02d/%02d and has been up %s.\n",ttm->tm_hour,ttm->tm_min,ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900,timeval(time(0)-glob.startup));
	wuser(uid,text);
	ttm=(struct tm *)gmtime((time_t *)&glob.lastreboot);
	sprintf(text,"Soft rebooted %d times. (last %02d/%02d/%02d, %02d:%02d)\n",glob.sreboots,ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900,ttm->tm_hour,ttm->tm_min);
	wuser(uid,text);
	timenow=time(0);
	ttm=(struct tm *)gmtime((time_t *)&timenow);
	sprintf(text,"The system time is %02d:%02d on %02d/%02d/%02d.\n",ttm->tm_hour,ttm->tm_min,ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900);
	wuser(uid,text);
}

void xidle(SU)
{
	short count;
	time_t idlec;
	
	DEBUG("xidle")
	line(uid,"Idle times");
	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.login!=0) continue;

		idlec=time(0)-UC.idle;
		sprintf(text,"%-20s %s\n",UC.name,idlec<30?"No time at all":timeval(idlec));
		wuser(uid,text);
	}
	line(uid,NULL);
}

void exits(SU)
{
	short count;
	short rc;
	short cr=0;
	char tmp[128];

	DEBUG("exits")
	rc=getroom(UU.room);

	if (UU.sleep==2) {
		wuser(uid,"You see all sorts of exits in your dreams.\n");
		return;
	}

	wuser(uid,"Obvious Exits:\n");
	for (count=0;count<10;count++) {
		if ((rooms[rc].dir[count]>0)&&(canexit(uid,count))) {
			if (UU.rank>NORM && UU.immstatus==1) {
				sprintf(tmp," [^R%05d^N]",rooms[rc].dir[count]);
			}
			else
			{
				sprintf(tmp," ");
			}
			sprintf(text,"^y%-9s^N - ^r%s^N%s\n",getdir(count),rooms[getroom(rooms[rc].dir[count])].name,tmp);
			wuser(uid,text);
			cr++;
		}
	}
	if (!cr) { /* no exits */
		wuser(uid,"     None!\n");
	}
}

void build(SU)
{
	FILE *fp;
	char builddate[256],build[256],gccver[256];

	if (!(fp=fopen(".verinfo","r"))) {
		wuser(uid,"Version info is broken, please inform an IMM\n");
		return;
	}

	// we assume .verinfo has the right amount of lines
	fgets(build,255,fp);
	builddate[255]='\0';
	fgets(builddate,255,fp);
	build[255]='\0';
	fgets(gccver,255,fp);
	gccver[255]='\0';
	fclose(fp);

	line(uid,"Build Info");
	sprintf(text,"Build Date: %s",builddate);
	wuser(uid,text);
	sprintf(text,"Build No. : %s",build);
	wuser(uid,text);
	sprintf(text,"GCC Ver   : %s",gccver);
	wuser(uid,text);
	line(uid,NULL);
}

