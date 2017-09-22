/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :guilds.c
** Purpose :The smud guild system
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern short isoffline(char *);
extern short getuser(SU,char *);
extern void to_fund(unsigned long);
extern short getroom(short);
extern char *cap1st(char *);
void wuser(SU, char *);
extern void line(SU,char *);

char mstr[30];

void xguilds(SU)
{
	short count,count2,gc=0;
	char *ga=NULL;
	short jr=-1;

	DEBUG("guilds")

	line(uid,"Guilds available");
	sprintf(text, "^b%-35s %-5s %-7s %s^N\n","Name","Level","Joinfee", "Guildhouse location");
	wuser(uid,text);

	for (count=0;count<MAXGUILDS;count++) {
		if (GC.vnum==-1) continue;
		/*
		for (count2=0;count2<MAXROOMS;count2++) {
			if (rooms[count2].vnum==-1) continue;
			if (rooms[count2].joinroom!=GC.vnum) continue;
				jr=rooms[count2].area;
		}
		*/
		jr=GC.location;
		if (jr>-1) {
			for (count2=0;count2<MAXAREAS;count2++) {
				if (areas[count2].vnum==-1) continue;
				if (areas[count2].vnum!=jr) continue;
				ga=areas[count2].name;
			}
		}
		if (GC.minjoin<999) {
			sprintf(text, "%-35s %5d %7d %s\n",cap1st(GC.name),GC.minjoin,(int)GC.joinfee, ga?ga:"UnKnown");
		}
		else
		{
			sprintf(text, "%-35s Closed to new members.\n",cap1st(GC.name));
		}
		wuser(uid,text);
		gc++;
	}
	sprintf(text,"%d guilds",gc);
	line(uid,text);
}
short getfreeguild()
{
	short count=0;

	DEBUG("getfreeguild")

	for (count=0;count<MAXGUILDS;count++) {
		if (guilds[count].vnum==-1) return count;
	}

	return -1;
}

short read_guilds(char *fname)
{
	FILE *fp;
	short stat=0;
	short count=-1;
	short sc=0;
/*	char *door; - Not Used Yet */

	DEBUG("read_guilds")

	sprintf(text,"%s%s",OTHERDIR,fname);
	if (!(fp=fopen(text,"r"))) 
	{ 
		printf("Guild file missing (%s)\n",text);
		exit(-1); 
	}

	while (fgets(text,255,fp)) {
		if (text[0]=='#') { break; }
		if (text[0]=='%') { stat=stat==11?0:stat+1; continue;}
		text[strlen(text)-1]='\0';
		switch (stat) {
			case 0 : count=getfreeguild(); 
				 sc=0;
				 if (count==-1) {
					 return -1;
				 }
				 GC.vnum=atoi(text); 
				 break;
			case 1 : strcpy(GC.def,text); 
				 break;
			case 2 : strcpy(GC.name,text); 
				 printf("Loaded guild - %s (%d)\n",text,GC.vnum);
				 break;
			case 3 : GC.primary=text[0]=='P'?1:0; 
				 break;
			case 4 : GC.minjoin=atoi(text);
				 break;
			case 5 : GC.joinfee=atoi(text);
				 break;
		 	case 6 : rooms[getroom(atoi(text))].guild=GC.vnum;
				 if (strchr(text,'R')) {
					GC.recall_room=atoi(text);
				 	if (GC.pracroom==-1) GC.pracroom=atoi(text);
				 }
				 if (strchr(text,'T')) {
					GC.pracroom=atoi(text);
				 }
				 break;
			case 7 : rooms[getroom(atoi(text))].joinroom=GC.vnum;
				 break;
			case 8 : strcpy(GC.flags,text);
				 break;
			case 9 : break;
			case 10: GC.location=atoi(text);
				 break;
			case 11: /* skills */
				 if (text[0]=='\0') break;
				 if (!strchr(text,'x')) break;
				 GC.skills[sc].vnum=atoi(text);
				 GC.skills[sc].maxprac=atoi(strchr(text,'x')+1);
				 sc++;
				 break;
			default: exit(-2);
		}
	}
	fclose(fp);
	return 0;
}

short getguild(short vnum)
{
	short count=0;

	DEBUG("getguild")
	
	for (count=0;count<MAXGUILDS;count++) {
		if (guilds[count].vnum==vnum) return count;
	}

	return -1;
}

char *getguilddef(short vnum)
{

	DEBUG("getguilddef")
	
	return guilds[getguild(vnum)].def;
}

char *getguildname(short vnum)
{
	DEBUG("getguildname")
	
	return guilds[getguild(vnum)].name;
}

short guildfromname(char *str)
{
	short count;

	for (count=0;count<MAXGUILDS;count++) {
		if (GC.vnum==-1) continue;
		if (strcasecmp(str,GC.def)==0)
		{
			return count;
		}
	}

	return -1;

}

void joinguild(SU, short vnum)
{
	short gid;

	DEBUG("joinguild")
	
	gid=getguild(vnum);

	if (gid==-1) return;

	/* at this point we assume the user has not left the default guild,
	 * is of the correct joining level, and has enough money
	 */

	UU.guild=vnum;
	sprintf(text, "^yYou join %s.^N\n",getguildname(vnum));
	wuser(uid,text);
}

void join(SU)
{
	short gid;
	DEBUG("join")
	
	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (rooms[getroom(UU.room)].joinroom==-1) {
		wuser(uid,"This is not a guild application room.\n");
		return;
	}

	if (UU.guild>1) {
		wuser(uid,"You have already joined a guild. (see help guilds)\n");
		return;
	}

	gid=getguild(rooms[getroom(UU.room)].joinroom);
	if (gid==-1) {
		wuser(uid,"The guild is not accepting applicants at the moment.\n");
		return;
	}

	if (guilds[gid].minjoin==999) {
		wuser(uid,"This guild is closed to new member applications at the moment.\n");
		return;
	}

	if (UU.lvl<guilds[gid].minjoin) {
		wuser(uid,"You are too in-experienced to join this guild.\n");
		return;
	}

	if (UU.cash<guilds[gid].joinfee) {
		wuser(uid,"You are too poor to join this guild.\n");
		return;
	}

	if (guilds[gid].vnum==1&&UU.guild==1) {
		wuser(uid,"You are a member of the Adventurers Guild by default, see help guilds.\n");
		return;
	}

	UU.cash-=guilds[gid].joinfee;
	to_fund(guilds[gid].joinfee);
	joinguild(uid,guilds[gid].vnum);
}

long members(SU,short vnum)
{
	struct dirent **namelist;
	int n;
	long usr=0;
	long count=0;

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		printf( "Error on directory read.\n");
		return 0L;
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
					return 0L;
				}
			}
			/* user stuff goes here */
			if (users[usr].guild==vnum) count++;
		}
		free(namelist[n]);
	}
	free(namelist);

	return count;
}

char *master(SU,short vnum)
{
	struct dirent **namelist;
	int n;
	long usr=0;
/*	long count=0; - Not used yet */
	unsigned long mxp=0;

	strcpy(mstr,"Vacant");

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		printf( "Error on directory read.\n");
		return (char *)0;
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
					return (char *)0;
				}
			}
			/* user stuff goes here */
			if (users[usr].guild==vnum) 
			{
				if (users[usr].xp>mxp) {
					mxp=users[usr].xp;
					strcpy(mstr,users[usr].name);
				}
			}
		}
		free(namelist[n]);
	}
	free(namelist);

	return mstr;
}

void ginfo(SU)
{
	short gid;

	if (word_count<1) {
		wuser(uid,"Info on which guild?\n");
		return;
	}

	gid=guildfromname(word[1]);

	if (gid==-1) {
		wuser(uid,"No such guild.\n");
		return;
	}

	sprintf(text,"Guild info for... %s",cap1st(guilds[gid].name));
	line(uid,text);

	sprintf(text,"Guild master    : %s\n",master(uid,guilds[gid].vnum));
	wuser(uid,text);
	sprintf(text,"Members         : %ld\n",members(uid,guilds[gid].vnum));
	wuser(uid,text);
	line(uid,NULL);
}

