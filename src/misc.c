/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :misc.c
** Purpose :All the odds and ends of functions
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 27-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short,long);
extern void checkleague(SU,short);
extern void wall(char *, short);
extern void wuser(SU, char *);
extern void wroom(SU, char *,short);
extern char *getguilddef(short);
extern short getuser(SU,char *);
extern short getroom(short);
extern void wrapout(short, char *,char,char);
void checkxp(short);

struct ds_st {
	char str[128];
	short dir;
};

struct ds_st dirstr[] = {
	{"north",0}, {"n",0},
	{"east",1}, {"e",1},
	{"south",2}, {"s",2},
	{"west",3}, {"w",3},
	{"northeast",4}, {"ne",4},
	{"northwest",5}, {"nw",5},
	{"southeast",6}, {"se",6},
	{"southwest",7}, {"sw",7},
	{"up",8}, {"u",8},
	{"down",9}, {"d",9},
	{"****",-1}
};

/* this should be used if you want the room to see the player unhide
i.e. dont use with 'move'
*/
void unhide(SU)
{
	if (UU.hidden>0) {
		wuser(uid,"You show yourself.\n");
		sprintf(text,"%s moves out from the shadows.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.hidden=0;
	}
}

void adjxp(SU, long adj)
{
	checkleague(uid,0);
	UU.xp+=adj;
	checkxp(uid);
	checkleague(uid,1);
}

/* percentage of mud visited
 */
double pervis(SU)
{
	short count;
	short numrooms=0;
	short numvis=0;
	double per;

	for (count=0;count<MAXROOMS;count++) {
		if (RC.vnum==-1) continue;
		if (RC.vnum>=20000) continue;
		numrooms++;
	}
	for (count=0;count<MAXROOMS;count++) {
		if (UU.visited[count]==-1) continue;
		if (getroom(UU.visited[count])==-1) continue;
		numvis++;
	}

//	printf("numroom %d, numvis %d\n",numrooms,numvis);
	per=(double)numvis/(double)numrooms*(double)100;
//	printf("Percentage %f\n",per);
	return per;
}

char *intostr(char chr, short num)
{
	short count=0;

	retval[0]='\0';

	if (num>4000||num<1) {
		return retval;
	}

	for (count=0;count<num;count++)
	{
		retval[count]=chr;
		retval[count+1]='\0';
	}

	return retval;
}

/* conerts a number to a string
 *
 * i.e. 435 = "Four hundred and thirty five"
 */
char *numstr(short num)
{
	return NULL;
}

char *timeval(time_t num)
{
	short weeks=0,days=0,hrs=0,mins=0,secs=0;
	short start=0;
	char tmp[255];

	weeks=(int)(num/604800);

	// if they have been doing whatever for more than a week just show
	// weeks and days, this means modifying days to only show days further
	// than the nearest week
	if (weeks) {
		days=(int)((num%604800)/86400);
	}
	else
	{
		days=(int)(num/86400);
		hrs=(int)((num%86400)/3600);
		mins=(int)((num%3600)/60);
		secs=(int)(num%60);
	}

	retval[0]='\0';
	tmp[0]='\0';
	if (weeks) {
		// just concat weeks and days
		sprintf(retval,"%dweek%s %dday%s",weeks,weeks==1?"":"s",days,days==1?"":"s");
	}
	else
	{
		if (days) {
			sprintf(tmp,"%dday%s",days,days==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (hrs||start) {
			sprintf(tmp,"%s%dhr%s",start?" ":"",hrs,hrs==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (mins||start) {
			sprintf(tmp,"%s%dmin%s",start?" ":"",mins,mins==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}

		if (secs||start) {
			sprintf(tmp,"%s%dsec%s",start?" ":"",secs,secs==1?"":"s");
			strcat(retval,tmp);
			start=1;
		}
	}
		
	return retval;
}

short canexit(SU,short dir)
{
/*	short count; - Not used yet */
	short rc;

	DEBUG("canexit")

	rc=getroom(UU.room);

	if (rooms[rc].door[dir][0]!='Y') return 1;
	if (rooms[rc].door[dir][2]=='O') return 1;

	return 0;
}

short getuser_room(SU,char *str)
{
	short tmp;

	DEBUG("getuser_room")
	tmp=getuser(uid,str);
	
	return tmp==-1?-1:users[tmp].room!=UU.room?-1:tmp;
}

char *getrank(SU)
{
	DEBUG("getrank")
	switch(UU.immstatus?UU.rank:NORM) {
		case NORM      : return (char *)getguilddef(UU.guild);
		case GUIDE     : return "GUIDE";
		case CARETAKER : return "CARETAKER";
		case OVERSEER  : return "OVERSEER";
		case IMM       : return "IMMORTAL";
		case GOD       : return "GOD";
		default   : return "ERROR";
	}
}

// seperate cat for help because help files do not follow the 
// two \n's for a paragraph that everything has to
short cathelp(SU, char *filen)
{
        FILE *fp;
	char outhelp[8192]; /* loads of space */
	short count=0;
	char nextc;
	short storeit=2;
	short ret;

	DEBUG("cat")

	if (!(fp=fopen(filen,"r"))) {
		sprintf(text,"%s.htm",filen);
		if (!(fp=fopen(text,"r"))) {
			return 0;
		}
	}

	ret=0;
	outhelp[0]='\0';

        while ((nextc=(char)fgetc(fp))!=EOF) {
		if (nextc=='<') storeit=0;
		if (nextc=='>') storeit=1;
		if (nextc=='\n') {
			if (count==0) continue;
			ret++;
		}
		else
		{
			ret=0;
		}
		if ((storeit==2)&&(ret<3)) {
			if (nextc=='\n') {
				outhelp[count]='\n';
				count++;
			}
			outhelp[count]=nextc;
			count++;
		}
		if (storeit==1) storeit=2;
        }
	outhelp[count]='\0';

        fclose(fp);
	wrapout(uid,outhelp,'J','N');
	//wuser(uid,outhelp);

	return 1;
}

/* will now handle html tags, KSB 03/07/00 */
/* wont allow more than one \n in a row */
short cat(SU, char *filen)
{
        FILE *fp;
	char outhelp[8192]; /* loads of space */
	short count=0;
	char nextc;
	short storeit=2;
	short ret;

	DEBUG("cat")

	if (!(fp=fopen(filen,"r"))) {
		sprintf(text,"%s.htm",filen);
		if (!(fp=fopen(text,"r"))) {
			return 0;
		}
	}

	ret=0;

        while ((nextc=(char)fgetc(fp))!=EOF) {
		if (nextc=='<') storeit=0;
		if (nextc=='>') storeit=1;
		if (nextc=='\n') {
			ret++;
		}
		else
		{
			ret=0;
		}
		if ((storeit==2)&&(ret<3)) {
			outhelp[count]=nextc;
			count++;
		}
		if (storeit==1) storeit=2;
        }
	outhelp[count]='\0';

	if (UU.login==0) {
		wrapout(uid,outhelp,'J','N');
	}
	else
	{
		wuser(uid,outhelp);
	}

        fclose(fp);
	return 1;
}

short getdirnum(char *str)
{
	short count=0;

	DEBUG("getdirnum")
	
	while (dirstr[count].str[0]!='*') {
		if (strcasecmp(str,dirstr[count].str)==0) {
			return dirstr[count].dir;
		}
		count++;
	}

	return -1;
}

char *getdir(short dn)
{
	DEBUG("getdir")
	switch(dn) {
		case 0 : return "north";
		case 1 : return "east";
		case 2 : return "south";
		case 3 : return "west";
		case 4 : return "northeast";
		case 5 : return "northwest";
		case 6 : return "southeast";
		case 7 : return "southwest";
		case 8 : return "up";
		case 9 : return "down";
		default : return "error!";
	}
}

// 1=male, 2=female, 3=it
char *getsex(SU,char *g0, char *g1, char *g2)
{
	DEBUG("getsex")
	switch (UU.sex) {
		case 1 : return g0; break;
		case 2 : return g1; break;
		case 3 : return g2; break;
		default : return "Error!"; break;
	}
}

int psuedo_rand(int max)
{
        return (1+(int) ((double)max*rand()/(RAND_MAX+1.0)));
}

char *cap1st(char *str)
{
	strcpy(retval,str);

	retval[0]=toupper(retval[0]);

	return retval;
}

void line(SU,char *str)
{
	char mid[128]; /* Not used yet: ,last[128]; */
	char tmpstr[4096];

        DEBUG("line")

	mid[0]='\0';

	if (str) {
		sprintf(mid,"^G[^Y%s^G]^R",str);
	}

	sprintf(tmpstr,"^R..%s",mid);
	sprintf(text,"%s%s^N\n",tmpstr,intostr('.',((UU.terminal.width-strlen(tmpstr))-1)+(*mid=='\0'?0:8)));
	wuser(uid,text);
}

void checkxp(SU)
{
	short hpg=0;
	short mang=0;
	short mvg=0;
	short pg=0;

	DEBUG("checkxp")

	/* Hit point gain .. uses constitution
	 * mana gain .. uses intelligence
	 * movement gain .. uses dextrity
	 * practice gain .. wisdom
	 */ 
	if (UU.xp<1000) {
		UU.xp=1000;
		wuser(uid,"The spirits restore you to ^Ylevel 1^N.\n");
	}

	if ((int)(UU.xp/1000)>UU.lvl) {
		if ((int)(UU.xp/1000)>UU.maxlvl) {
			hpg=(psuedo_rand(UU.con)+7)*(UU.xp/1000-UU.maxlvl);
			mang=(psuedo_rand(UU.intl)+7)*(UU.xp/1000-UU.maxlvl);
			mvg=(psuedo_rand(UU.dex)+7)*(UU.xp/1000-UU.maxlvl);
			pg=(psuedo_rand((int)(UU.wis/2))+4)*(UU.xp/1000-UU.maxlvl);
			if (UU.lvl<10) {
				UU.canhold+=2;
				UU.maxweight+=10;
			}

			if (UU.lvl>=50) { 
				pg=1*(UU.xp/1000-UU.maxlvl);
				hpg=2*(UU.xp/1000-UU.maxlvl);
				mang=2*(UU.xp/1000-UU.maxlvl);
				mvg=2*(UU.xp/1000-UU.maxlvl);
			}

			UU.maxhp+=hpg;
			UU.maxman+=mang;
			UU.maxmv+=mvg;

			sprintf(text,"Your gain is %d hp, %d man, %d mov.\n",hpg,mang,mvg);
			wuser(uid,text);
			sprintf(text,"You also gain %d extra practice points.\n",pg);
			wuser(uid,text);

			UU.prac+=pg;
		}

		UU.lvl=(int)(UU.xp/1000);
		sprintf(text,"^yYou have reached level %d!^N\n",UU.lvl);
		wuser(uid,text);
		if (UU.lvl==2) {
			wuser(uid,"^yYour player will now save.^N\n");
		}
		sprintf(text,"^y%s has reached level %d.^N\n",UU.name,UU.lvl);
		wall(text,uid);
		if (UU.lvl>UU.maxlvl) UU.maxlvl=UU.lvl;
	}                                                                    
	if ((int)(UU.xp/1000)<UU.lvl) {
		UU.lvl=(int)(UU.xp/1000);
		sprintf(text,"^rYou have dropped to level %d.^N\n",UU.lvl);
		wuser(uid,text);
		sprintf(text,"^r%s has dropped to level %d.^N\n",UU.name,UU.lvl);
		wall(text,uid);
	}                                                                    
}

void stolower(char *str)
{
	char *p;

	DEBUG("stolower")

	for (p=str;*p!='\0';p++)
	{
		*p=tolower(*p);
	}
}

/*** Tell telnet not to echo characters - for password entry ***/
void echo_off(SU)
{
char seq[4];

sprintf(seq,"%c%c%c",IAC,WILL,TELOPT_ECHO);
wuser(uid,seq);
}


/*** Tell telnet to echo characters ***/
void echo_on(SU)
{
char seq[4];

sprintf(seq,"%c%c%c",IAC,WONT,TELOPT_ECHO);
wuser(uid,seq);
}

void doprompt(SU,char *str)
{
	unsigned char tmpstr[1000];
	unsigned char tmpstr2[1000];
	if (UU.iacga) {
		sprintf(tmpstr,"%s%c%c",str,IAC,GA);
	}
	else
	{
		sprintf(tmpstr,"%s\r\n",str);
	}
	if(UU.immstatus==1)
	{
		sprintf(tmpstr2,"^R[IMM]^N %s",tmpstr);
		sprintf(tmpstr,"%s",tmpstr2);
	}
	wuser(uid,tmpstr);
}

short getmflag(short mo, short flag)
{
        DEBUG("getmflag")

		if (strchr(mobs[mo].flags,mflags[flag])) return 1;
		return 0;
}

short getrflag(short rn, short flag)
{

        DEBUG("getrflag")

	if (strchr(rooms[rn].flags,rflags[flag])) return 1;
	return 0;
}

short getoflag(short ob, short flag)
{			
        DEBUG("getoflag")

		if (strchr(objs[ob].flags,oflags[flag])) return 1;
		return 0;
}

short holding_light(SU)
{
/*	short count; - Not used yet */

	DEBUG("holding_light")

	/* see if they have a torch */

	if (getoflag(UU.worn[10],oLIGHT)) return 1;

	return 0;
}

/* reports if a given room is in darkness
 */
short isdark(short vnum)
{
	short rc=0;
	short itsdark=0;
	short count=0;

	rc=getroom(vnum);

	if ((getrflag(rc,DARK))||(getrflag(rc,OUTSIDE)&&(glob.sun==NIGHT)&&(glob.weather!=wCLEAR))) itsdark=1;

	if (getrflag(rc,CONTLIGHT)) itsdark=0;

	for (count=0;count<MAXOBJS;count++)
	{
		if (OC.room!=vnum) continue;
		if (getoflag(count,oLIGHT)) itsdark=0;
	}

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket<0) continue;
		if (UC.room!=vnum) continue;
		if (holding_light(count)) itsdark=0;
	}

	return itsdark;
}
