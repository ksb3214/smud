/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : login.c
** Purpose : actions performed on login
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 30-06-02 0.00 ksb First Version
*/

#include "smud.h"

extern void race(SU);
//extern short is_new_message(SU);
extern void look(SU);
extern void brieflook(SU);
extern void wuser(SU, char *);
extern void joinguild(SU, short);
extern void wall(char *, short);
extern void echo_on(SU);
extern void get_host_name(SU);
extern void echo_off(SU);
extern void wuser(SU, char *);
extern short pload(SU);
extern void qquit(SU);
extern char *getguilddef(short gid);
extern long from_fund(long amount);
extern void wsock(int socket,char *str);
extern void offerCompress(SU);
extern short compressEnd(SU);
extern int got_new_debates(SU);
extern short getroom(short);

// login function at end of file.

/* finish login
 */
void end_login(SU)
{
	DEBUG("end_login")

	/* Defaults */
	/* Uncomment on first run or edit your pfile 
	**	UU.rank=GOD;
	*/
	UU.login=0;
	UU.lvl=1;
	UU.maxlvl=1;
	UU.hp=20; UU.man=100; UU.mv=100;
	UU.maxhp=20; UU.maxman=100; UU.maxmv=100;
	UU.prac=21;
	UU.xp=1000;
	UU.room=STARTROOM;
	UU.fight=-1;
	UU.cash=from_fund(50); // a little gold to start with
	UU.str=13; UU.maxstr=18;
	UU.dex=13; UU.maxdex=18;
	UU.con=13; UU.maxcon=18;
	UU.intl=13; UU.maxint=18;
	UU.wis=13; UU.maxwis=18;
	// do mccp here then we have some context for the client return
	// this is also done in olduserpass
	offerCompress(uid);
	wuser(uid,"\n");
	joinguild(uid,1);
	wuser(uid,"^Y   (..see help guilds..)^N\n\n");
	sprintf(text,"%s the level %d %s has signed on.\n",UU.name,UU.lvl,getguilddef(UU.guild));
	wall(text,uid);
	if(UU.brief)
	{
		brieflook(uid);
	} else {
		look(uid);
	}
	event(eNEWPLAYERLOGON,uid,-1,-1);
}

void username(SU)
{
	short ucount=0,count=0,found=0,ret=0;
	struct tm *ttm;

	DEBUG("username")
	get_host_name(uid);

	if ((strlen(inpstr)<3)||(strlen(inpstr)>10)) {
		wuser(uid,"Incorrect length, must be between 3 and 10 chars\n");
		wuser(uid,"Try another name:\n");
		return;
	}

	while (inpstr[strlen(inpstr)-1]=='\n' || inpstr[strlen(inpstr)-1]=='\r')
	{
		inpstr[strlen(inpstr)-1]='\0';
	}

	/* okay, the following need to be dealt with...
	 * quit
	 * who
	 * and flibs's lovely 'info_stats' system......
	 */
	if (strcasecmp(inpstr,"stats_info")==0) {
		/* output loads of absolutely not false shite */
		// okay below are ALL of the options available and the code,
		// if it currently exists to do it, however we will keep the
		// stuff which isnt applicable or impressive commented for now
		sprintf(text,"url: http://www.smud.co.uk\n");
		wuser(uid,text);
		sprintf(text,"contact: smud@amber.org.uk\n");
		wuser(uid,text);
		sprintf(text,"connect: telnet://smud.co.uk:4000\n");
		wuser(uid,text);
		//sprintf(text,"altconnect: %s\n");
		//wuser(uid,text);
		ttm=(struct tm *)gmtime((time_t *)&glob.startup);
		sprintf(text,"bootdate: %d%02d%02d %02d%02d%02d\n",ttm->tm_year+1900,ttm->tm_mon+1,ttm->tm_mday,ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
		wuser(uid,text);
		sprintf(text,"uptime: %ld\n",time(0)-glob.startup);
		wuser(uid,text);
		//sprintf(text,"totalnewbielogins: %s\n");
		//wuser(uid,text);
		//sprintf(text,"totallogins: %s\n");
		//wuser(uid,text);
		sprintf(text,"newbies: yes\n");
		wuser(uid,text);
		sprintf(text,"residents: yes\n");
		wuser(uid,text);
		ucount=0;
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket==-1) continue;
			if (UC.login>0) continue;
			ucount++;
		}
		sprintf(text,"users: %d\n",ucount);
		wuser(uid,text);
		sprintf(text,"maxusers: %d\n",MAXUSERS);
		wuser(uid,text);
		//sprintf(text,"maximumusers: %s\n");
		//wuser(uid,text);
		//sprintf(text,"maximumusers_date: %s\n");
		//wuser(uid,text);
		//sprintf(text,"maximumeverusers: %s\n");
		//wuser(uid,text);
		//sprintf(text,"maximumeverusers_date: %s\n");
		//wuser(uid,text);
		sprintf(text,"description: %s\n",INFOSTR);
		wuser(uid,text);
		//sprintf(text,"intercom: %s\n");
		//wuser(uid,text);
		qquit(uid);
	}
	if (strcasecmp(inpstr,"quit")==0) {
		wuser(uid,"Bye bye then...\n");
		qquit(uid);
	}

	if (strcasecmp(inpstr,"who")==0) {
		sprintf(text,"\nPlayers online:\n");
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket<0) continue;
			if (UC.login!=0) continue;
			ret++;
			if (found) {
				strcat(text,", ");
				if (ret==4) {
					ret=0;
					strcat(text,"\n");
				}
			}
			strcat(text,UC.name);
			found++;
		}
		strcat(text,".\n");
		if (!found) {
			wuser(uid,"No players online at the moment.\n");
		}else
		{
			wuser(uid,text);
			sprintf(text,"%d players online at the moment.\n",found);
			wuser(uid,text);
		}
		wuser(uid,"\nPlease enter a name:\n");
		return;
	}
	strcpy(UU.name,inpstr);
	UU.name[0]=toupper(UU.name[0]);

	if (pload(uid)) {
		wuser(uid,"Welcome back...\nNow enter your password:\n");
		echo_off(uid);
		UU.login=2;
		return;
	}
	
	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (count==uid) continue;
		if (strcmp(UU.name,UC.name)==0)
		{
			strcpy(UU.password,UC.password);
			wuser(uid,"Welcome back...\nNow enter your password:\n");
			echo_off(uid);
			UU.login=2;
			return;
		}
	}

	sprintf(text,"Did I get that right '%s'? (yes/no) \n",UU.name);
	wuser(uid,text);
	UU.login=7;
}

void confirmname(SU)
{
	DEBUG("confirmname")
	if (inpstr[0]=='y'||inpstr[0]=='Y') {
		wuser(uid,"Welcome to Smud...\nPlease choose a password:\n");
		echo_off(uid);
		UU.login=3;
	}
	else
	{
		wuser(uid,"Okay, lets try another name:\n");
		UU.login=1;
	}
}

void olduserpass(SU)
{
	short recon=-1;
	short count;
	struct tm *ttm;

	DEBUG("olduserpass")

	while (inpstr[strlen(inpstr)-1]=='\n' || inpstr[strlen(inpstr)-1]=='\r')
	{
		inpstr[strlen(inpstr)-1]='\0';
	}
	if (strcmp(crypt(inpstr,UU.password),UU.password)==0) {
		UU.login=0;
		wuser(uid,"Password verified!\n\n");
		echo_on(uid);
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket==-1) continue;
			if (count==uid) continue;
			if (strcmp(UU.name,UC.name)==0) recon=count;
		}
		if (recon!=-1)
		{
			sprintf(text,"%s reconnects.\n",UU.name);
			wall(text,uid);
			wuser(uid,"You reconnect.\n");
			compressEnd(recon);
			users[recon].mccp=0;
			close(users[recon].socket);
			users[recon].socket=UU.socket;
			offerCompress(recon);
			UU.socket=-1;
		}
		else
		{
			if (UU.guild==-1) {
				joinguild(uid,1);
			}
			if (UU.maxlvl==0) {
				UU.maxlvl=UU.lvl;
			}
			// do mccp here then we have some context for the client return
			// this is also done in end_login
			offerCompress(uid);
			sprintf(text,"%s the level %d %s has signed on.\n",UU.name,UU.lvl,getguilddef(UU.guild));
			wall(text,uid);
			if (UU.lastlogin!=0) {
				ttm=(struct tm *)gmtime((time_t *)&UU.lastlogin);
				sprintf(text,"^gYou last logged in on %02d/%02d/%02d, %02d:%02d from %s^N\n\n",ttm->tm_mday,ttm->tm_mon+1,1900+ttm->tm_year,ttm->tm_hour,ttm->tm_min,UU.lastaddr[0]!='\0'?UU.lastaddr:UU.lastip);
				wuser(uid,text);
			}

			if (getroom(UU.room)==-1) { 
				UU.room=1;
				wuser(uid,"You have been placed in the starting room.\nThis is most likely due to an IMM removing the room you last occupied.\n");
			}
			look(uid);
			/* old pager code, see backups pre 18/07/2002
			msc=is_new_message(uid);
			if (msc) {
				sprintf(text,"You have ^G%d^N new message%s.\n",msc,msc==1?"":"s");
				wuser(uid,text);
			}
			*/
			if(got_new_debates(uid)==1)
				wuser(uid,"There are ^RNEW^N debates for you to participate in. (see help debate)\n");

			sprintf(text,"^bYour next stats advance will be at level %d.^N\n",UU.maxlvl+1);
			wuser(uid,text);
			event(ePLAYERLOGON,uid,-1,-1);
		}
	}
	else
	{
		echo_on(uid);
		wuser(uid,"Password incorrect!\n\n");
		qquit(uid);
	}
}

void prompt_type(SU)
{
	if (strcasecmp(inpstr,"yes")==0) {
		UU.iacga=1;
	}

	UU.login=9;
	wuser(uid,"\nDoes your client and terminal support colour? [YES/NO]\n");
	wuser(uid,"(if you do not know, type NO and read 'help colour' once logged in)\n");
}

void colour_status(SU)
{
	if (strcasecmp(inpstr,"yes")==0) {
		UU.colour=1;
	}

	UU.login=6;
	wuser(uid,"\nWhat sex are you?\n");
	wuser(uid,"[(1)Male, (2)Female, (3)N/A\n");
}

void newuserpass(SU)
{
	DEBUG("newuserpass")
	echo_on(uid);
	while (inpstr[strlen(inpstr)-1]=='\n' || inpstr[strlen(inpstr)-1]=='\r')
	{
		inpstr[strlen(inpstr)-1]='\0';
	}
	if (strlen(inpstr)<3) {
		wuser(uid,"Password too short...\n");
		wuser(uid,"Please try again:\n");
		echo_off(uid);
		return;
	}

	strcpy(UU.password,crypt(inpstr,SALT));
	UU.login=8;
	wuser(uid,"\nIf you are using a client like TinyFugue which can display the prompt\nin a seperate area of the screen then please type YES here, otherwise, or\nif you are not sure, type NO and read 'help iacga' when logged in.\n");
	wuser(uid,"Please type YES or NO:\n");
}

void sex(SU)
{
	short ch=0;

	DEBUG("sex")
	ch=atoi(inpstr);

	if ((ch<1)||(ch>3)) {
		wuser(uid,"Not a valid sex, try again...\n");
		return;
	}

	UU.sex=ch;
	end_login(uid);
}

void login(SU)
{
	DEBUG("login")

	switch(UU.login) {
		case 1 : username(uid);		break;
		case 2 : olduserpass(uid);	break;
		case 3 : newuserpass(uid);	break;
		case 6 : sex(uid);		break;
		case 7 : confirmname(uid);	break;
		case 8 : prompt_type(uid);	break;
		case 9 : colour_status(uid);	break;
		default : break;
	}
}

