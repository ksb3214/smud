/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :pfiles.c
** Purpose :Loads and saves (deletes etc) player files
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 27-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void reset_char(SU);
extern void wuser(SU, char *);
extern void reset_obj(short);
extern void syslog(int, char *);
extern short read_obj(short);
extern void stolower(char *);

/* Attempts to load a pfile, will return 0 if not successfull, otherwise will
** return 1.
*/ 
short pload(SU)
{
	FILE *fp;
	char *dp;
	short stat;
	char filename[255];
	short value=0;
	short count;
	short numworn;
	short age=0;
	short skillno=0;
	short abrno=0;

	DEBUG("pload")
	filename[0]='\0';
	strcpy(filename,UU.name);

	stolower(filename);

	sprintf(text,"%s%s",USERDIR,filename);

	if (!(fp=fopen(text,"r"))) {
		/* no such file */
		return 0;
	}

	/* file open, okay lets read */
	stat=0;
	while (stat==0) {
		fgets(text,255,fp);
		if (text[0]=='#') break;
		text[strlen(text)-1]='\0';
		dp=text+4; /* all values are nnn:dp */
		value=atoi(text); /* will return all upto the first : */
		switch(value) {
			case 100: strcpy(UU.name,dp); break;
			case 101: strcpy(UU.password,dp); break;
			case 102: UU.lvl=atoi(dp); break;
			case 103: UU.rank=atoi(dp); break;
			case 104: UU.hp=atoi(dp); break;
			case 105: UU.man=atoi(dp); break;
			case 106: UU.mv=atoi(dp); break;
			case 107: UU.maxhp=atoi(dp); break;
			case 108: UU.maxman=atoi(dp); break;
			case 109: UU.maxmv=atoi(dp); break;
			case 110: UU.xp=atoi(dp); break;
			case 111: break; /* used to be class */
			case 112: UU.race=atoi(dp); break;
			case 113: UU.room=atoi(dp); break;
			case 114: UU.sex=atoi(dp); break;
			case 115: strcpy(UU.flags,dp); break;
			case 116: UU.str=atoi(dp); break;
			case 117: UU.intl=atoi(dp); break;
			case 118: UU.wis=atoi(dp); break;
			case 119: UU.con=atoi(dp); break;
			case 120: UU.dex=atoi(dp); break;
			case 121: UU.maxstr=atoi(dp); break;
			case 122: UU.maxint=atoi(dp); break;
			case 123: UU.maxwis=atoi(dp); break;
			case 124: UU.maxcon=atoi(dp); break;
			case 125: UU.maxdex=atoi(dp); break;
			case 126: UU.bank=atol(dp); break;
			case 127: UU.cash=atol(dp); break;
			case 128: strcpy(UU.email,dp); break;
			//case 129: strcpy(UU.title,dp); break;
			case 130: UU.sleep=atol(dp); break;
			case 131: UU.lastnews=atol(dp); break;
			case 132: UU.firstlog=atol(dp); break;
			case 133: UU.alllog=atol(dp); break;
			case 134: UU.homeroom=atol(dp); break;
			case 135: UU.leaveroom=atol(dp); break;
			/* num 136 is for inventory (non worn/held) */
			case 136: if (uid==MAXUSERS) break;
			          for (count=0;count<MAX_HELD;count++) {
					  if (UU.held[count]==-1) break;
				  }
				  if (count==MAX_HELD) break;
				  age=0;
				  if (strchr(dp,'a')) {
					  age=atoi(strchr(dp,'a')+1);
				  }
				  read_obj(atol(dp));
				  objs[glob.curobj].room=-1;
				  objs[glob.curobj].mob=-1;
				  objs[glob.curobj].user=uid;
				  objs[glob.curobj].age=age;
				  UU.held[count]=glob.curobj;
				  break;
			/* num 137 is for worn/held */
			case 137: if (uid==MAXUSERS) break;
				  numworn=atol(dp);
				  age=0;
				  if (strchr(dp,'a')) {
					  age=atoi(strchr(dp,'a')+1);
				  }
				  read_obj(atol(strchr(dp,'!')+1));
				  objs[glob.curobj].room=-1;
				  objs[glob.curobj].mob=-1;
				  objs[glob.curobj].user=uid;
				  objs[glob.curobj].age=age;
				  UU.worn[numworn]=glob.curobj;
				  break;
			case 138: UU.died=atol(dp); break;
			case 139: UU.lastdied=atol(dp); break;
			case 140: UU.laston=atol(dp); break;
			case 141: UU.wimpy=atol(dp); break;
			case 142: UU.visitable=atol(dp); break;
			case 143: UU.iacga=atol(dp); break;
			case 144: UU.invis=atol(dp); break;
			case 145: UU.guild=atol(dp); break;
			case 146: UU.prac=atol(dp); break;
			case 147: UU.align=atol(dp); break;
			case 148: UU.colour=atol(dp); break;
			case 149: UU.canhold=atol(dp); break;
			case 150: UU.maxweight=atol(dp); break;
			case 151: UU.maxlvl=atoi(dp); break;
			case 152: UU.skills[skillno].vnum=atoi(dp);
				  UU.skills[skillno].prac=atoi(strchr(dp,'x')+1);
				  skillno++;
				  break;
			case 153: UU.lastlogin=atol(dp); break;
			case 154: strcpy(UU.lastip,dp); break;
			case 155: UU.lastint=atol(dp); break;
			case 156: UU.swearon=atol(dp); break;
			case 157: UU.lastfined=atol(dp); break;
			case 158: UU.naked_fine=atol(dp); break;
			case 159: strcpy(UU.prompt,dp); break;
			case 160: strcpy(UU.cprompt,dp); break;
			case 161: UU.kills=atol(dp); break;
			case 162: UU.litter_fine=atol(dp); break;
			case 163: UU.lpos=atol(dp); break;
				  // visited saves - 164
			case 164: for (count=0;count<MAXROOMS;count++) {
					  if (UU.visited[count]==-1) {
						  UU.visited[count]=atol(dp);
						  break;
					  }
				  }
				  break;
			case 165: for (count=0;count<ACTIVEDEBATES;count++) {
					  if (UU.voted[count]==-1) {
						  UU.voted[count]=atol(dp);
						  break;
					  }
				  }
				  break;
			case 166: UU.justify=atol(dp); break;
			case 167: strcpy(UU.defterm,dp); break;
			case 168: UU.defprec=atol(dp); break;
			case 169: UU.autoexit=atol(dp); break;		
			case 170: UU.autoloot=atol(dp); break;		
			case 171: UU.autosac=atol(dp); break;		
			case 172: UU.brief=atol(dp); break;		
			case 173: UU.logcount=atol(dp); break;		
			case 174: strcpy(UU.lastaddr,dp); break;
			case 175: UU.abrchr[abrno]=dp[0];
				  strcpy(UU.abrstr[abrno],dp+1);
				  abrno++;
				  UU.abrchr[abrno]='*';
				  break;
			case 176: UU.hidden=atol(dp); break;		
			case 177: UU.codes[atol(dp)]=1; break;
			default: syslog(SYS,"Bad pfile option\n"); break;
		}
	}

	fclose(fp);

	return 1;
}

/* This saves a player and all his objects, if res is set then the player and
 * objects will be reset
 */
short psave(SU,short res)
{
	FILE *fp;
	char filename[255];
	short count;

	DEBUG("psave")

	filename[0]='\0';
	strcpy(filename,UU.name);

	stolower(filename);

	sprintf(text,"%s%s",USERDIR,filename);

	if (!(fp=fopen(text,"w"))) {
		/* bummer */
		//syslog(SYS,"Cannot open pfile for writing\n");
		printf("Error - (%s)\n",strerror(errno));
		printf("Failed to open pfile.\n");
		return 0;
	}

	/* file created, lets write */
	fprintf(fp,"100:%s\n",UU.name);
	fprintf(fp,"101:%s\n",UU.password);
	fprintf(fp,"102:%d\n",UU.lvl);
	fprintf(fp,"103:%d\n",UU.rank);
	fprintf(fp,"104:%d\n",UU.hp);
	fprintf(fp,"105:%d\n",UU.man);
	fprintf(fp,"106:%d\n",UU.mv);
	fprintf(fp,"107:%d\n",UU.maxhp);
	fprintf(fp,"108:%d\n",UU.maxman);
	fprintf(fp,"109:%d\n",UU.maxmv);
	fprintf(fp,"110:%d\n",(int)UU.xp);
	fprintf(fp,"112:%d\n",UU.race);
	fprintf(fp,"113:%d\n",UU.room);
	fprintf(fp,"114:%d\n",UU.sex);
	fprintf(fp,"115:%s\n",UU.flags);
	fprintf(fp,"116:%d\n",UU.str);
	fprintf(fp,"117:%d\n",UU.intl);
	fprintf(fp,"118:%d\n",UU.wis);
	fprintf(fp,"119:%d\n",UU.con);
	fprintf(fp,"120:%d\n",UU.dex);
	fprintf(fp,"121:%d\n",UU.maxstr);
	fprintf(fp,"122:%d\n",UU.maxint);
	fprintf(fp,"123:%d\n",UU.maxwis);
	fprintf(fp,"124:%d\n",UU.maxcon);
	fprintf(fp,"125:%d\n",UU.maxdex);
	fprintf(fp,"126:%ld\n",UU.bank);
	fprintf(fp,"127:%ld\n",UU.cash);
	fprintf(fp,"128:%s\n",UU.email);
	//fprintf(fp,"129:%s\n",UU.title);
	fprintf(fp,"130:%d\n",UU.sleep);
	fprintf(fp,"131:%d\n",UU.lastnews);
	fprintf(fp,"132:%ld\n",UU.firstlog);
	fprintf(fp,"133:%ld\n",UU.alllog);
	fprintf(fp,"134:%d\n",UU.homeroom);
	fprintf(fp,"135:%d\n",UU.leaveroom);
	/* 136 is objects */
	for (count=0;count<MAX_HELD;count++) {
		if (UU.held[count]!=-1) {
			fprintf(fp,"136:%da%d\n",objs[UU.held[count]].vnum,objs[UU.held[count]].age);
			if (res) reset_obj(UU.held[count]);
		}
	}
	/* 137, worn/held */
	for (count=0;count<13;count++) {
		if (UU.worn[count]!=-1) {
			fprintf(fp,"137:%d!%da%d\n",count,objs[UU.worn[count]].vnum,objs[UU.worn[count]].age);
			if (res) reset_obj(UU.worn[count]);
		}
	}
	fprintf(fp,"138:%ld\n",UU.died);
	fprintf(fp,"139:%ld\n",UU.lastdied);
	fprintf(fp,"140:%ld\n",time(0));
	fprintf(fp,"141:%d\n",UU.wimpy);
	fprintf(fp,"142:%d\n",UU.visitable);
	fprintf(fp,"143:%d\n",UU.iacga);
	fprintf(fp,"144:%d\n",UU.invis);
	fprintf(fp,"145:%d\n",UU.guild);
	fprintf(fp,"146:%d\n",UU.prac);
	fprintf(fp,"147:%d\n",UU.align);
	fprintf(fp,"148:%d\n",UU.colour);
	fprintf(fp,"149:%d\n",UU.canhold);
	fprintf(fp,"150:%d\n",UU.maxweight);
	fprintf(fp,"151:%d\n",UU.maxlvl);
	for (count=0;count<MAX_SKILLS;count++) {
		if (UU.skills[count].vnum==-1) continue;
		fprintf(fp,"152:%dx%d\n",UU.skills[count].vnum,UU.skills[count].prac);
	}
	fprintf(fp,"153:%ld\n",time(0));
	fprintf(fp,"154:%s\n",UU.curip);
	fprintf(fp,"155:%ld\n",UU.lastint);
	fprintf(fp,"156:%d\n",UU.swearon);
	fprintf(fp,"157:%ld\n",UU.lastfined);
	fprintf(fp,"158:%ld\n",UU.naked_fine);
	fprintf(fp,"159:%s\n",UU.prompt);
	fprintf(fp,"160:%s\n",UU.cprompt);
	fprintf(fp,"161:%ld\n",UU.kills);
	fprintf(fp,"162:%ld\n",UU.litter_fine);
	fprintf(fp,"163:%ld\n",UU.lpos);
	// visited
	for (count=0;count<MAXROOMS;count++) {
		if (UU.visited[count]==-1) continue;
		fprintf(fp,"164:%d\n",UU.visited[count]);
	}
	// votes
	for (count=0;count<ACTIVEDEBATES;count++) {
		if (UU.voted[count]==-1) continue;
		fprintf(fp,"165:%d\n",UU.voted[count]);
	}
	fprintf(fp,"166:%d\n",UU.justify);
	fprintf(fp,"167:%s\n",UU.defterm);
	fprintf(fp,"168:%d\n",UU.defprec);
	fprintf(fp,"169:%d\n",UU.autoexit);
	fprintf(fp,"170:%d\n",UU.autoloot);
	fprintf(fp,"171:%d\n",UU.autosac);
	fprintf(fp,"172:%d\n",UU.brief);
	fprintf(fp,"173:%ld\n",UU.logcount);
	fprintf(fp,"174:%s\n",UU.addr);
	for (count=0;UU.abrchr[count]!='*';count++) {
		fprintf(fp,"175:%c%s\n",UU.abrchr[count],UU.abrstr[count]);
	}
	fprintf(fp,"176:%d\n",UU.hidden);
	for (count=0;count<MAXCODES;count++) {
		if (UU.codes[count]==1) {
			fprintf(fp,"177:%d\n",count);
		}
	}
	/* must have this last */
	fprintf(fp,"# Last line\n");
	fclose(fp);
	// if (time(0)-UU.idle<AUTOSAVE) wuser(uid,"^ySaving...^N\n");
	return 1;
}

/* saves all online players, if res is set then players MUST be reset afterwards
 * or the game must close down... i.e. autosave should not set res
 */
short psave_all(short res)
{
	short count;
	short noway=1;

	DEBUG("psave_all")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.login!=0) continue;
		if (UC.lvl<2) continue;
//		if (res) {
//			UC.alllog+=time(0)-UC.thislog;
//			UC.thislog=time(0);
//		}
		if (!psave(count,res)) {
			wuser(count,"^rYour user info could not be saved, sorry!^N");
			noway=0;
		}
	}

	return noway;
}

short isoffline(char *str)
{
	DEBUG("isoffline")

	/* this uses MAXUSERS+1 for its temp char */
	reset_char(MAXUSERS);
	strcpy(users[MAXUSERS].name,str);
	return (pload(MAXUSERS)?MAXUSERS:-1);
}

long totalpfiles(SU)
{
	struct dirent **namelist;
	int n;
/*	char text[100]; - Not used yet */
	long count=0;

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory read.\n");
		return 0;
	}

	/* now scroll through all sorted entries */

	while (n--)
	{
		if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
			count++;
		}
		free(namelist[n]);
	}
	free(namelist);

	return count;
}

