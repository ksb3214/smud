/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :smud.c	
** Purpose :Main Smud file, everything started here ;)
**
** $id$
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 27-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void loadsigns();
extern short getdirnum(char *);
extern void awall(char *, short, short);
extern short getguild(short);
extern short getrflag(short, short);
extern void line(SU,char *);
extern void stolower(char *);
extern void police(short);
extern void enter(short);
extern void shortcuts(short);
extern void dofollow(SU, short, short);
extern void wroom(short, char *, short);
extern void qquit(SU);
extern void nofollow(SU);
extern void wrapout(short,char *,char,char);
extern void doprompt(SU,char *);
extern void wall(char *, short);
extern void justify(SU);
extern void killgroup(SU);
extern short psave(SU,short);
extern void unhide(SU);
extern void Sread(SU,short);
extern void Sstab(SU);
extern void Sburn(SU);
extern void Shide(SU);
extern void Scharge(SU);
extern void Sheadbutt(SU);
extern void Spickpocket(SU);
extern void Sthrow(SU);
extern void Spunch(SU);
extern void Sstrangle(SU);
extern void Skick(SU);
extern void Srescue(SU);
extern void build(SU);
extern void Sdisarm(SU);
extern void xpgain(SU);
extern void Sretreat(SU);
extern void Ssneak(SU);
extern void watch(SU);
extern void xcprompt(SU);
extern void xprompt(SU);
extern void nuke(SU);
extern void finance(SU);
extern void ginfo(SU);
extern void oload(SU);
extern void league(SU);
extern void criminal(SU);
extern void resetnews(SU);
extern void newsedit(SU);
extern void testedit(SU);
extern void giveprac(SU);
extern void editroom(SU);
extern void cunt(SU);
extern void boot(SU);
extern void summon(SU);
extern void practice(SU);
extern void skills(SU);
extern void sacrifice(SU);
extern void train(SU);
extern void orefresh(SU);
extern void mrefresh(SU);
extern void rrefresh(SU);
extern void fwho(SU);
extern void mud_reboot(SU);
extern void givecash(SU);
extern void rf(SU);
extern void tf(SU);
extern void mpurge(SU);
extern void join(SU);
extern void xguilds(SU);
extern void drink(SU);
extern void affects(SU);
extern void report(SU);
extern void immoff(SU);
extern void immon(SU);
extern void sell(SU);
extern void buy(SU);
extern void value(SU);
extern void list(SU);
extern void wimpy(SU);
extern void use(SU);
extern void visitable(SU);
extern void unvisitable(SU);
extern void dolist(SU,short);
extern void showlist(SU);
extern void visit(SU);
extern void leave(SU);
extern void home(SU);
extern void makehome(SU);
extern void info(SU);
extern void xareas(SU);
extern void gemote(SU);
extern void gsay(SU);
extern void group(SU);
extern void follow(SU);
extern void finger(SU);
extern void aemote(SU);
extern void qemote(SU);
extern void answer(SU);
extern void question(SU);
extern void drem(SU);
extern void driv(SU);
extern void news(SU);
//extern void title(SU);
//extern void page(SU);
extern void emote(SU);
extern void ask(SU);
extern void xweather(SU);
extern void balance(SU);
extern void arm(SU);
extern void deposit(SU);
extern void withdraw(SU);
extern void typo(SU);
extern void idea(SU);
extern void bug(SU);
extern void xgoto(SU);
extern void locate(SU);
extern short cathelp(short, char *);
extern void remote(SU);
extern void tell(SU);
//extern void set(SU);
extern void xset(SU);
extern void xshow(SU);
extern void xshutdown(SU);
extern void chem(SU);
extern void chat(SU);
extern void inventory(SU);
extern void score(SU);
extern void say(SU);
extern void look(SU);
extern void brieflook(SU);
extern void wuser(SU, char *);
extern void do_edit(SU);
extern short getinp(SU);
extern void advance(SU);
extern void editsign(SU);
extern void removesign(SU);

extern void autox();
extern void scan();
extern void init_socket();
extern void rebootit();
extern void loadlist();
extern short read_guilds(char *);
extern void read_skills();
extern void read_objs();
extern short read_areas();
extern void init_vars();
extern void init_sig();
extern void syslog(int, char *);
extern short numlines(SU);
extern void cdebate(SU);
extern void read_debates();
extern void read_trans();
extern void mail(SU);
extern void post(SU);
extern void mail_subject(SU);
extern void unwrap(SU);
extern void load_home_objs();
void xyzzy(SU);

char *abrchr=".,><':?*";

char *abrstr[]={
"chat ",
"chem ",
"tell ",
"remote ",
"say ",
"emote ",
"drivel "
};

struct transport_st trans[MAXTRANS];
struct areas_st areas[MAXAREAS];
struct chr users[MAXUSERS+1];
struct rm rooms[MAXROOMS];
struct mob mobs[MAXMOBS];
struct obj objs[MAXOBJS];
struct gld guilds[MAXGUILDS];
struct globals glob;

char inpstr[4096];
char text[4096];
char retval[4096];  /* to use in string returning functions */
char *comstr;
short word_count=0;
short reb=0;
char word[10][255];
char *oflags="*IDGNEAMSHWTdKFBCJOPQRL$bp";
char *rflags="*DCSNPGMWTAEOBUIFRL";
char *mflags="*SCBVAFEGNMZHDPROITJKLQUWXYsmnphl";

char *wornstr[] = {
		"<worn on head>",
		"<worn around neck>",
		"<worn on arms>",
		"<worn about body>",
		"<worn on chest>",
		"<worn on legs>",
		"<worn on feet>",
		"<worn around wrist>",
		"<worn around wrist>",
		"<worn on hands>",
		"<held>",
		"<wielded>",
		"<worn as shield>"
};                                                                              

char *listdesc[]={
	"Noisy",
	"Ignore",
	"Inform",
	"Grabme",
	"Friend",
	"Bar",
	"Invite",
	"Beep",
	"Block",
	"Find",
	"Key",
	"***"
};

char *listsdesc[]={
	"Nsy",
	"Ign",
	"Inf",
	"Grb",
	"Frd",
	"Bar",
	"Inv",
	"Bep",
	"Blk",
	"Fnd",
	"Key",
	"***"
};

void commands(SU);
void afk(SU);
void new_connect();
void xidle(SU);
void activity(SU);
short split_inpstr();
void login(SU);
void xsleep(SU);
void xrest(SU);
void xwake(SU);
void username(SU);
short getroom(short vnum);
void xtime(SU);
void confirmname(SU);
void xdrop(SU);
short canexit(SU,short dir);
short getobjn(short vnum);
void olduserpass(SU);
void prompt(SU);
void colour_status(SU);
void prompt_type(SU);
short cat(SU, char *filen);
short holding_light(SU);
void xsave(SU);
void examine(SU);
void hold(SU);
void noswear(SU);
void give(SU);
void wear(SU);
void newuserpass(SU);
char *getdir(short dn);
void xremove(SU);
void class(SU);
void quit1(SU);
void xclose(SU);
void xopen(SU);
void flee(SU);
short getobj(SU,char *str);
void move(SU, short dir, short noisy);
void who(SU);
short getexist(SU);
void killx(SU);
void exits(SU);
short getheldobj(SU,char *str);
void help(SU);
char *getsex(SU,char *g0, char *g1, char *g2);
void recall(SU);
void take(SU);
void sex(SU);
void consider(SU);

/* Wrapper functions */

void move_north(SU) { move(uid,0,1); }
void move_south(SU) { move(uid,2,1); }
void move_east(SU) { move(uid,1,1); }
void move_west(SU) { move(uid,3,1); }
void move_ne(SU) { move(uid,4,1); }
void move_se(SU) { move(uid,6,1); }
void move_sw(SU) { move(uid,7,1); }
void move_nw(SU) { move(uid,5,1); }
void move_up(SU) { move(uid,8,1); }
void move_down(SU) { move(uid,9,1); }

void dolist_noisy(SU) { dolist(uid,lNOISY); }
void dolist_ignore(SU) { dolist(uid,lIGNORE); }
void dolist_inform(SU) { dolist(uid,lINFORM); }
void dolist_grabme(SU) { dolist(uid,lGRABME); }
void dolist_friend(SU) { dolist(uid,lFRIEND); }
void dolist_bar(SU) { dolist(uid,lBAR); }
void dolist_invite(SU) { dolist(uid,lINVITE); }
void dolist_beep(SU) { dolist(uid,lBEEP); }
void dolist_block(SU) { dolist(uid,lBLOCK); }
void dolist_find(SU) { dolist(uid,lFIND); }
void dolist_key(SU) { dolist(uid,lKEY); }

void Sqread(SU) { Sread(uid,0); }
void Slread(SU) { Sread(uid,1); }


#include "include/commands.h"

long cres=0;

/**************************************************************************/
/* either the port to start on or 'R' is passed as first variable
** on seamless reboot the port isnt needed as it's stored on glob.
*/
int main(int argc, char *argv[])
{
	short pt=0;
	DEBUG("+++++++++++++++++++++++++++++++++ started +++++++++++++++++++++++++++++++++++++++")

	srand(time(0));
	reb=0;
	if (argc>1) {
		if (argv[1][0]=='R') {
			reb=1;
		}
		else
		{
			// must be the port
			pt=atoi(argv[1]);
			printf("port %d specified.\n",pt);
			if (pt<1&&pt>10000) {
				pt=0;
			}
		}	
	}
	syslog(SYS,"Smud started");
	glob.startup=time(0);
	init_sig();
	init_vars();
	if (pt!=0) glob.spec_port=pt;
	read_skills();
	loadlist();
	loadsigns();
	if (reb) {
		reb=2;
		rebootit();
	}
	else
	{
		read_areas();
		load_home_objs();
		read_objs();
		read_guilds("guilds.def");
		read_debates();
		read_trans();
		init_socket();
	}
	while(!glob.exitsmud) {
		scan();
		autox();
	} 
	syslog(SYS,"Smud shutdown safely\n");
	shutdown(glob.lsock,2);
	close(glob.lsock);
	DEBUG("--------------------------------- dead -----------------------------------------")
	return 0;
}

void activity(SU)
{
	short len=0;
	int count=0,ret=0;
	char com[4096];
	FILE *ef;
	char tmpcom[4096];
	short foundc;

	DEBUG("activity")

	if (!getinp(uid)) ret=1; /* no actual usable input */
	if ((unsigned char)*inpstr==255) ret=2;
	if (*inpstr<=32) ret=3;
	if (*inpstr=='\n') ret=4;
	if (ret) {
//		sprintf(text,"debug (%d)\n",ret);
//		wuser(uid,text);
	//	printf("ret - (%d)(%c)\n",ret,inpchr);
		if (UU.login==0 && !BP && ret==4) prompt(uid);
		return;
	}

//	printf("str - (%s)(%d)\n",inpstr,inpstr[0]);
	while (inpstr[strlen(inpstr)-1]<32)
	{
		inpstr[strlen(inpstr)-1]='\0';
	}

	while (inpstr[strlen(inpstr)-1]==' ')
	{
		inpstr[strlen(inpstr)-1]='\0';
	}

	if (UU.eid>0) {
		do_edit(uid);
		return;
	}

	if (UU.login>0) {
		login(uid);
		return;
	}

	if(UU.maildest[0]!='\0')
	{
		mail_subject(uid);
		return;
	}

	UU.idle=time(0); /* reset idle time */
	UU.idlewarn=0;
	if (UU.afk!=0) {
		UU.afk=0;
		wuser(uid,"You are no longer AFK.\n");
	}
	
	if (inpstr[0]=='!') {
		strcpy(inpstr,UU.oldinp);
	}
	else
	{
		strcpy(UU.oldinp,inpstr);
	}

	for (count=0;UU.abrchr[count]!='*';count++) {
		if (inpstr[0]==UU.abrchr[count]) {
			strcpy(tmpcom,UU.abrstr[count]);
			strcat(tmpcom,inpstr+(*(inpstr+1)==' '?2:1));
			strcpy(inpstr,tmpcom);
		}
	}


	split_inpstr();

        comstr=strchr(inpstr,' ');
        if (!comstr)
        {
                comstr=strchr(inpstr,'\0');
                strncpy(com,inpstr,comstr-inpstr);
                com[(comstr-inpstr)]='\0';
        }
        else
        {
                strncpy(com,inpstr,comstr-inpstr);
                com[(comstr-inpstr)]='\0';
                comstr++;
        }

	/* Commands go here */
	foundc=-1;
	len=strlen(com);
	for (count=0;comlist[count].name[0]!='*';count++)
	{
		if (strncmp(com,comlist[count].name,len)==0)
		{
			if (comlist[count].priv<=(UU.immstatus?UU.rank:NORM)) {
				comlist[count].function(uid);
				foundc=count;
				break;
			}
		}
	}

/*	split_inpstr(); */

	if(foundc==-1) {
			wuser(uid,"Command unknown.\n");
			ef = fopen("../other/failed-commands.txt","a");
			fprintf(ef,"%s\n",inpstr);
			fclose(ef);
	}
	 //prompt(uid);
}

/* Splits inpstr into word[x], ignores first word (i.e. command) */
short split_inpstr()
{
	char *tmpstr;
	short pos=-1;

	DEBUG("split_inpstr")

	tmpstr=strchr(inpstr,' ');

	word_count=0;

	if (!tmpstr) return 0;

	
	while ((*tmpstr!='\0')&&(word_count<9)) {
		if (*tmpstr!=' ')
		{
			if (*(tmpstr-1)==' ') word_count++;
		  	pos++;
			word[word_count][pos]=*tmpstr;
			word[word_count][pos+1]='\0'; 
		}
		else
		{
			pos=-1;
		}
		tmpstr++;
	}

	return word_count;
}

void quit1(SU)
{
	DEBUG("quit1")
	if (UU.login==0) {
		UU.alllog+=time(0)-UU.thislog;
		if (UU.lvl>=2) {
			wuser(uid,"^ySaving...^N\n");
			if (!psave(uid,1)) {
				wuser(uid,"Could not save character (internal error)\n");
			}
		}
		else
		{
			wuser(uid,"Your player is not saved as you didn't reach level 2.\n");
		}
		killgroup(uid);
		wuser(uid,"Goodbye, see you soon!\n");
		sprintf(text,"%s has signed off.\n",UU.name);
		wall(text,uid);
		nofollow(uid);
		event(ePLAYERLOGOFF,uid,-1,-1);
	}
	qquit(uid);
}

/* Set noisy if you want the room to be informed and triggers to activate
 */
void move(SU, short dir, short noisy)
{
	short newr=0;
	short oldr=0;

	DEBUG("move")

	if (UU.fight!=-1) {
		wuser(uid,"You cannot move in mid-fight.\n");
		return;
	}

	if (UU.sleep==1) {
		wuser(uid,"Stop resting first.\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (UU.mv<2) {
		wuser(uid,"You need to stop and rest first.\n");
		return;
	}
	if (noisy) {
		unhide(uid);
	}
	else
	{
		UU.hidden=0;
	}
		

	if ((rooms[getroom(UU.room)].dir[dir]>0)&&(canexit(uid,dir))) {
		if (noisy) {
			event(ePLAYERLEAVE,uid,-1,-1);
			sprintf(text,"%s leaves %s.\n",UU.name,getdir(dir));
			wroom(UU.room,text,uid);
		}
		newr=rooms[getroom(UU.room)].dir[dir];
		oldr=UU.room;
		UU.room=newr;
		if (noisy) {
			sprintf(text,"%s has arrived.\n",UU.name);
			wroom(UU.room,text,uid);
		}
		if (UU.rank==NORM) UU.mv-=2;
		if(UU.brief)
		{
			brieflook(uid);
		} else {
			look(uid);
		}
		if (noisy) {
			event(ePLAYERENT,uid,-1,-1);
			dofollow(uid, oldr, dir);
			police(UU.room);
		}
		xpgain(uid);
	}
	else
	{
		wuser(uid,"Alas, you cannot go that way.\n");
	}
}

short getuser(SU,char *str)
{
	short count;
	char tmpstr[4096];
	char tmpuname[4096];

	DEBUG("getuser")
	strcpy(tmpstr,str);
	stolower(tmpstr);

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket<0) continue;
		strcpy(tmpuname,UC.name);
		stolower(tmpuname);
		if (strncmp(tmpstr,tmpuname,strlen(str))==0) {
			return count;
		}
	}
	return -1;
}

void help(SU)
{
	char helpstr[4096];
	FILE *fp;
	struct dirent **namelist;
	int n;
	short exact=0;
	short countout=0;
	short part=0;
	char act[255],partstr[255];

	helpstr[0]='\0';

	DEBUG("help")
	line(uid,"Smud help system");
	if (word_count<1) {
		sprintf(comstr,"help");
	}

	strcpy(act,comstr);
	n=scandir("../help/", &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory help.\n");
	}
	else
	{
		while (n--)
		{
			if (strncmp(comstr,namelist[n]->d_name,strlen(comstr))==0) {
				part++;
				if (countout!=0) {
					strcat(helpstr,", ");
				}
				else
				{
					strcat(helpstr,"  ");
				}
				sprintf(partstr,"../help/%s",namelist[n]->d_name);
				sprintf(text,"help ^y%s^N",namelist[n]->d_name);
				strcat(helpstr,text);
				if (countout==2) {
					strcat(helpstr,"\n");
					countout=0;
				}
				else countout++;
				if (strcasecmp(comstr,namelist[n]->d_name)==0) {
					// exact match
					exact=1;
					sprintf(act,"../help/%s",comstr);
				}
			}
			free(namelist[n]);
		}
		free(namelist);
	}

	if ((UU.rank>NORM)&&(UU.immstatus)) {
		n=scandir("../help/imm/", &namelist, 0, alphasort);
		if (n < 0)
		{
			wuser(uid, "Error on directory help.\n");
		}
		else
		{
			while (n--)
			{
				if (strncmp(comstr,namelist[n]->d_name,strlen(comstr))==0) {
					part++;
					if (countout!=0) {
						strcat(helpstr,", ");
					}
					else
					{
						strcat(helpstr,"  ");
					}
					sprintf(partstr,"../help/imm/%s",namelist[n]->d_name);
					sprintf(text,"help ^g%s^N",namelist[n]->d_name);
					strcat(helpstr,text);
					if (countout==2) {
						strcat(helpstr,"\n");
						countout=0;
					}
					else countout++;
					if (strcasecmp(comstr,namelist[n]->d_name)==0) {
						// exact match
						exact=1;
						sprintf(act,"../help/imm/%s",comstr);
					}
				}
				free(namelist[n]);
			}
			free(namelist);
		}
	}

	if ((!exact)&&(part==1)) {
		strcpy(act,partstr);
		exact=1;
	}

	if (exact) {
		if (!cathelp(uid,act)) {
			printf("help screwed %s\n",act);
		}
	}
	else
	{
		if (part) {
			wuser(uid,"Try one of the following:\n");
			wuser(uid,helpstr);
			if (countout!=0) wuser(uid,"\n");
		}
		else
		{
			wuser(uid,"Help does not exist on that topic.\n");
			if (!(fp=fopen("../other/failed-help.def","a"))) {
				// bah
				return;
			}
			fprintf(fp,"%s\n",comstr);
			fclose(fp);
		}
	}
	line(uid,NULL);
}

void recall(SU)
{
	short rc;

	DEBUG("recall")
	rc=getroom(UU.room);

	if (UU.fight!=-1) {
		wuser(uid,"You are fighting, flee first.\n");
		return;
	}

	if (UU.room>=20000) {
		wuser(uid,"You cannot recall from homerooms.\n");
		return;
	}

	if (UU.sleep>0) {
		wuser(uid,"You must be awake and standing to recall.\n");
		return;
	}

	if (getrflag(rc,NORECALL)) {
		wuser(uid,"God has forsaken you.\n");
		return;
	}

	if (UU.room==guilds[getguild(UU.guild)].recall_room) {
		wuser(uid,"You are already at recall.\n");
		return;
	}

	sprintf(text,"%s prays for transportation.\n",UU.name);
	wroom(UU.room,text,uid);
	UU.room=guilds[getguild(UU.guild)].recall_room;
	sprintf(text,"%s appears without warning.\n",UU.name);
	wroom(UU.room,text,uid);
	if(UU.brief)
	{
		brieflook(uid);
	} else {
		look(uid);
	}
	event(ePLAYERARRIVE,uid,-1,-1);
}

void afk(SU)
{
	unsigned long afktime=0;

	DEBUG("afk")
	if (UU.fight!=-1) {
		// return with no error
		return;
	}

	if (word_count<1) {
		UU.afk=1;
	}
	else
	{
		afktime=atol(comstr);
		if (afktime<1) {
			wuser(uid,"Don't be silly.\n");
			return;
		}
		if (afktime>MAX_AFK) {
			wuser(uid,"You cannot be afk for that long.\n");
			return;
		}
		UU.afk=(time(0)+(afktime*60));
	}

	wuser(uid,"You are now AFK.\n");
}

void xsleep(SU)
{
	DEBUG("sleep")
	if (UU.sleep==2) {
		wuser(uid,"You are already asleep.\n");
		return;
	}

	if (UU.fight>-1) {
		wuser(uid,"Now is not the time for sleep.\n");
		return;
	}

	UU.sleep=2;
	wuser(uid,"You sleep.\n");
	sprintf(text,"%s sleeps.\n",UU.name);
	wroom(UU.room,text,uid);
}

void xrest(SU)
{
	DEBUG("rest")
	if (UU.sleep==1) {
		wuser(uid,"You are already resting.\n");
		return;
	}

	if (UU.fight>-1) {
		wuser(uid,"Now is not the time for resting.\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"You wake up and rest.\n");
		sprintf(text,"%s wakes up and rests.\n",UU.name);
	}
	else
	{	
		wuser(uid,"You rest for a while.\n");
		sprintf(text,"%s rests.\n",UU.name);
	}
	UU.sleep=1;
	wroom(UU.room,text,uid);
}

/* used for wake and stand */
void xwake(SU)
{
	short usr=0;

	DEBUG("wake")

	if (word_count==1) {
		if (UU.sleep==2) {
			wuser(uid,"Try waking up first.\n");
			return;
		}
		usr=getuser(uid,comstr);
		if (usr<0) {
			wuser(uid,"Wake who?\n");
			return;
		}
		if (users[usr].room!=UU.room) {
			wuser(uid,"Wake who?\n");
			return;
		}
		if (users[usr].sleep!=2) {
			wuser(uid,"They are not asleep.\n");
			return;
		}
		sprintf(text,"%s awakes %s.\n",UU.name,users[usr].name);
		wroom(UU.room,text,uid);
		sprintf(text,"You awake %s.\n",users[usr].name);
		wuser(uid,text);
		users[usr].sleep=0;
		sprintf(text,"%s wakes you.\n",UU.name);
		wuser(usr,text);
		return;
	}

	if (UU.sleep==0) {
		wuser(uid,"You are already awake and standing.\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"You wake up and stand up.\n");
		sprintf(text,"%s wakes up and stands.\n",UU.name);
	}
	else
	{	
		wuser(uid,"You stand up.\n");
		sprintf(text,"%s stands.\n",UU.name);
	}
	UU.sleep=0;
	wroom(UU.room,text,uid);
}

/* Will hopefully sort and display commands */
/* 30/06/2002 - Updated to segregate normals commands and skills */
void commands(SU)
{
        short sortit[2000]; /* ewwwwwww */
        short j=0,i=0;
        short count=0;
        short chg=1;
        short tmpint=0;
	char col[10];
	char fname[128];
	short nohelp=0;
	short skills=0;
	char text[32686];
	FILE *fp;

	DEBUG("commands")

	line(uid,skills?"Smud Commands - Skills":"Smud Commands");
        while (comlist[j].name[0]!='*') {
                if (UU.immstatus?UU.rank:NORM>=comlist[j].priv)
                {
			if (comlist[j].name[0]=='@') {
				if (skills) {
                        		sortit[i]=j;
                        		i++;
				}
			}
			else
			{
				if (!skills) {
                        		sortit[i]=j;
                        		i++;
				}
			}
                }
                j++;
        }
        while(chg) {
                chg=0;
                for (count=0;count<i-1;count++)
                {
                        if (strcmp(comlist[sortit[count]].name,comlist[sortit[count+1]].name)>0)
                        {
                                tmpint=sortit[count];
                                sortit[count]=sortit[count+1];
                                sortit[count+1]=tmpint;
                                chg=1;
                        }
                }
        }

        text[0]='\0';
        for (count=0;count<i;count++)
        {
		if (comlist[sortit[count]].priv>NORM) {
			strcpy(col,"^g");
		} else col[0]='\0';
		strcat(text,col);
		nohelp=0;
		if ((UU.rank>NORM)&&(UU.immstatus)) {
			sprintf(fname,"../help/%s",comlist[sortit[count]].name);
			if (!(fp=fopen(fname,"r"))) {
				nohelp=1;
			}
			else
			{
				nohelp=0;
				fclose(fp);
			}
			sprintf(fname,"../help/imm/%s",comlist[sortit[count]].name);
			if (nohelp) {
				if (!(fp=fopen(fname,"r"))) {
					nohelp=1;
				}
				else
				{
					nohelp=0;
					fclose(fp);
				}
			}
			if (nohelp) {
				strcat(text,"^Y");
			}
		}
                strcat(text,comlist[sortit[count]].name);
		if (comlist[sortit[count]].priv>NORM) strcat(text,"^N");
		if (nohelp) strcat(text,"^N");
		strcat(text,", ");
        }
        *(strrchr(text,','))='.';
	wrapout(uid,text,'J','N');
	sprintf(text,"%d commands",i);
	line(uid,text);
}

char *swapstr(SU,char *str)
{
	char *tmp;
	char tstr[255];

	tmp=str;
	strcpy(retval,"");
	for (tmp=str;*tmp!='\0';tmp++)
	{
		if (*tmp=='%') {
			tmp++;
			if (*tmp=='\0') break;
			/* figure out the key or just put % */

			switch(*tmp) {
				case 'h' : sprintf(tstr,"%d",UU.hp); break;
				case 'H' : sprintf(tstr,"%d",UU.maxhp); break;
				case 'm' : sprintf(tstr,"%d",UU.man); break;
				case 'M' : sprintf(tstr,"%d",UU.maxman); break;
				case 'v' : sprintf(tstr,"%d",UU.mv); break;
				case 'V' : sprintf(tstr,"%d",UU.maxmv); break;
				case 'x' : sprintf(tstr,"%d",(int)UU.xp); break;
				case 'g' : sprintf(tstr,"%d",(int)UU.cash); break;
				case 'a' : sprintf(tstr,"%d",UU.align); break;
				case 'r' : sprintf(tstr,"%s",rooms[getroom(UU.room)].name); break;
				case 'T' : if (UU.fight!=-1) {
					   	sprintf(tstr,"%s",mobs[UU.fight].name); break;
					   }
					   else
					   {
						   sprintf(tstr,"%s","");
					   }
					   break;
				case 't' : if (UU.fight!=-1) {
					   	sprintf(tstr,"%d",mobs[UU.fight].hp);
					   }
					   else
					   {
						   sprintf(tstr,"%s","");
					   }
					   break;
				default: sprintf(tstr,"%s","");
			}
		}
		else
		{
			sprintf(tstr,"%c",*tmp);
		}
		strcat(retval,tstr);
	}

	return retval;
}

void prompt(SU)
{
	DEBUG("prompt")

	sprintf(text,"%s",swapstr(uid,UU.fight!=-1?UU.cprompt:UU.prompt));
	if(UU.maildest[0]!='\0')
	{	
		sprintf(text,"Subject: ");
	}
	if(UU.eid>0)
	{	
		if(UU.eline<999)
		{
			sprintf(text,"ED %d/%d> ",UU.eline,numlines(uid));
		} else {
			sprintf(text,"ED +/%d> ",numlines(uid));
		}
	}
	doprompt(uid,text);
}

void xopen(SU)
{
	short dir;
	short count;
	short gotkey=0;

	DEBUG("xopen")

	if (word_count<1) {
		wuser(uid,"Open what?\n");
		return;
	}

	dir=getdirnum(word[1]);

	if (dir==-1) {
		wuser(uid,"I don't see what you want to open.\n");
		return;
	}

	if (rooms[getroom(UU.room)].door[dir][0]!='Y') {
		wuser(uid,"There is nothing to open in that direction.\n");
		return;
	}

	if (rooms[getroom(UU.room)].door[dir][2]=='O') {
		wuser(uid,"That door is already wide open.\n");
		return;
	}

	if (rooms[getroom(UU.room)].door[dir][2]=='L') {
		/* need a key */
		for (count=0;count<MAX_HELD;count++) {
			if (UU.held[count]==-1) continue;
			if (objs[UU.held[count]].vnum!=rooms[getroom(UU.room)].keys[dir]) continue;
			gotkey=1;
		}
		if (gotkey==0) {
			wuser(uid,"You do not have a key for that door.\n");
			return;
		}
	}

	/* okay we have a closed door, maybe locked, lets open it */
	rooms[getroom(UU.room)].door[dir][2]='O';

	sprintf(text,"You %sopen the door.\n",gotkey?"unlock and ":"");
	wuser(uid,text);

	sprintf(text,"%s %sopens the %s door.\n",UU.name,gotkey?"unlocks and ":"",getdir(dir));
	wroom(UU.room,text,uid);
}

void xclose(SU)
{
	short dir;
/*	short count; - Not used yet */
/*	short gotkey=0; - Not used yet */

	DEBUG("xclose")

	if (word_count<1) {
		wuser(uid,"Close what?\n");
		return;
	}

	dir=getdirnum(word[1]);

	if (dir==-1) {
		wuser(uid,"I don't see what you want to close.\n");
		return;
	}

	if (rooms[getroom(UU.room)].door[dir][0]!='Y') {
		wuser(uid,"There is nothing to close in that direction.\n");
		return;
	}

	if (rooms[getroom(UU.room)].door[dir][2]!='O') {
		wuser(uid,"That door is already closed.\n");
		return;
	}

	rooms[getroom(UU.room)].door[dir][2]='C';

	wuser(uid,"You close the door.\n");

	sprintf(text,"%s closes the %s door.\n",UU.name,getdir(dir));
	wroom(UU.room,text,uid);
}

void xsave(SU)
{
	if (UU.lvl<2) {
		wuser(uid,"Your player is not saved until you reach level 2.\n");
		return;
	}
	if (time(0)-UU.lastsaved<SAVE_LIMIT) {
		wuser(uid,"^yYou saved only a moment ago.^N\n");
		return;
	}
	UU.lastsaved=time(0);
	wuser(uid,"^ySaving...^N\n");
	if (!psave(uid,0)) {
		wuser(uid,"^rSave failed, speak to an IMM.^N\n");
	}
}

void xyzzy(SU)
{
	wuser(uid,"Nothing happens.\n");
}
