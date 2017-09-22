/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : skills.c
** Purpose : All the skills related stuff
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 17-01-02 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short, long);
extern short getoflag(short, short);
extern void wuser(SU, char *);
extern short getguild(short);
extern void line(SU,char *);
extern short split_inpstr();
extern short getroom(short);
extern short getrflag(short, short);
extern short getmob(SU,char *);
extern int psuedo_rand(int);
extern void wroom(short, char *, short);
extern short getuser(SU,char *);
extern short getdirnum(char *);
extern char *getdir(short);
extern short next_victim(short);
extern void look(SU);
extern void brieflook(SU);
extern short getobj(SU, char *);
extern short getheldobj(SU, char *);
extern void scroll(short, short, short, short);
extern void wrapout(short,char *,char,char);
extern void wrapoutroom(short,char *,char,char,short);
extern char *getsex(short,char *,char *,char *);
extern void move(short, short, short);
extern void unhide(SU);

char lout[200][80];
char rout[200][80];
char ret[MAXQUEUE];
short scount=0;		/* used by outskills */
short halfway=0;	/* used by outskills */

void retreat(SU);

void read_skills()
{
	FILE *fp;
/*	short count; - not used yet */
	short cs=0;
	char *dt;
	short vnum=0,dot0=0,dot1=0;
	short parent;
	short level=0;

	sprintf(text,"%sskills.def",OTHERDIR);

	if (!(fp=fopen(text,"r"))) {
		printf("Skills file not found.\n");
		exit(20);
	}

	while (fgets(text,255,fp)) {
		if (text[0]=='#') break;
		text[strlen(text)-1]='\0';
		vnum=atoi(text);  /* read upto / */
		dt=strchr(text,'/');
		if (!dt) {
			printf("File format error 1 (skills)\n");
			exit(21);
		}
		dt++; /* get past / */
		parent=-1;
		if (*dt!='.') {
			/* level 0 */
			dot0=vnum;
			parent=-1;
			level=0;
		}
		if (*(dt+1)=='.') {
			/* level 2 */
			parent=dot1;
			dt+=2;
			level=2;
		}
		if (*dt=='.') {
			/* level 1 */
			parent=dot0;
			dot1=vnum;
			level=1;
			dt++;
		}

		if (strchr(dt,',')) {
			glob.skills[cs].type=*(strchr(dt,',')+1);
			*(strchr(dt,','))='\0';
		}
		else
		{
			glob.skills[cs].type='N';
		}
		glob.skills[cs].vnum=vnum;
		glob.skills[cs].parent=parent;
		glob.skills[cs].level=level;
		strcpy(glob.skills[cs].name,dt);
		cs++;
	}
	fclose(fp);

}

short countskills(short parent,short toponly)
{
	short count;
	short cs=0;

	for (count=0;count<MAX_SKILLS;count++) {
		if (glob.skills[count].vnum==-1) continue;
		if (glob.skills[count].parent!=parent) continue;

		cs++;
		if (!toponly) cs+=countskills(glob.skills[count].vnum,0);
	}

	return cs;
}

/* returns the amount that a skill has been practiced by a user
 */
short getuskill(SU,short vnum)
{
	short count;

	for (count=0;count<MAX_SKILLS;count++) {
		if (UU.skills[count].vnum==-1) continue;

		if (UU.skills[count].vnum!=vnum) continue;

		/* found the skill */

		return UU.skills[count].prac;
	}

	return 0;
}

/* sets the amount that a skill has been practiced by a user
 */
void setuskill(SU,short vnum,short amount)
{
	short count;
	short ele=-1;
	
	/* search for existing skill */
	for (count=0;count<MAX_SKILLS;count++) {
		if (UU.skills[count].vnum==-1) continue;
		if (UU.skills[count].vnum!=vnum) continue;
		/* found the skill */

		ele=count;
	}

	if (ele==-1) {
		/* search for new skill space */
		for (count=0;count<MAX_SKILLS;count++) {
			if (UU.skills[count].vnum!=-1) continue;
			/* found the free skill */

			ele=count;
		}
	}

	if (ele==-1) {
		wuser(uid,"Ran out of skill space...erk!, tell an IMM now!\n");
		return;
	}

	UU.skills[ele].vnum=vnum;
	UU.skills[ele].prac=amount;
}

/* returns the amount that a skill has been practiced by a user
 */
short getskillmax(SU,short vnum)
{
	short count;
	short gid;

	gid=getguild(UU.guild);

	for (count=0;count<MAX_SKILLS;count++) {
		if (guilds[gid].skills[count].vnum==-1) continue;

		if (guilds[gid].skills[count].vnum!=vnum) continue;

		/* found the skill */

		return guilds[gid].skills[count].maxprac;
	}

	return 0;
}

char *getskillname(short vnum)
{
	short count;

	for (count=0;count<MAX_SKILLS;count++) {
		if (glob.skills[count].vnum==-1) continue;
		if (glob.skills[count].vnum!=vnum) continue;

		sprintf(retval,"%s",glob.skills[count].name);
		return retval;
	}

	sprintf(retval,"unknown");
	return retval;
}

void outskills(SU,short parent,short level,short toponly)
{
	short count;
	short dot;
	char *out;
	short max,amount;

	for (count=0;count<MAX_SKILLS;count++) {
		if (glob.skills[count].vnum==-1) continue;
		if (glob.skills[count].parent!=parent) continue;

		if (scount<halfway) {
			out=lout[scount];
		}
		else
		{
			out=rout[scount-halfway];
		}

		strcpy(out,"");
		for (dot=0;dot<level;dot++) {
			strcat(out,"-");
		}
		strcat(out,glob.skills[count].name);
		if (glob.skills[count].type=='C') strcat(out,"*");
		for (dot=strlen(out);dot<25;dot++) {
			out[dot]='.';
			out[dot+1]='\0';
		}
		if (glob.skills[count].type!='N') {
			amount=getuskill(uid,glob.skills[count].vnum);
			max=getskillmax(uid,glob.skills[count].vnum);
			sprintf(text," %2d/%2d%%",amount,max);
			strcat(out,text);
		}
				
		scount++;
		if (!toponly) outskills(uid,glob.skills[count].vnum,level+1,0);
	}
}

void out2user(SU)
{
	short count;

	for (count=0;count<200;count++) {
		if (lout[count][0]=='\0') break;

		sprintf(text,"%-38s %s\n",lout[count],rout[count]);
		wuser(uid,text);
	}
}

void resetout()
{
	short count;

	for (count=0;count<200;count++) {
		lout[count][0]='\0';
		rout[count][0]='\0';
	}
	scount=0;
	halfway=0;
}

void skills(SU)
{
	short count;
	short parent=-1;


	if (word_count<1) {
		count=countskills(-1,1);
		resetout();
		halfway=(int)(count/2)+1;
		outskills(uid,-1,0,1);
		line(uid,"skills");
	}
	else
	{
		for (count=0;count<MAX_SKILLS;count++) {
			if (glob.skills[count].vnum==-1) continue;
			if (glob.skills[count].parent!=-1) continue;

			if (strncasecmp(word[1],glob.skills[count].name,strlen(word[1]))==0) {
				parent=glob.skills[count].vnum;
				sprintf(text,"%s skills",glob.skills[count].name);
				line(uid,text);
				break;
			}
		}

		if (parent==-1) {
			wuser(uid,"Skill not known.\n");
			return;
		}

		count=countskills(parent,0);
		resetout();
		halfway=(int)(count/2)+1;
		outskills(uid,parent,0,0);
	}

	out2user(uid);
	sprintf(text,"%d available",count);
	line(uid,text);
}

/* evaluates abbreviations */
void transskill()
{
	short count=0,count2=0;
	short parent=-1;

	sprintf(retval,"%s","");

	for (count=0;count<word_count;count++) {
		for (count2=0;count2<MAX_SKILLS;count2++) {
			if (glob.skills[count2].vnum==-1) continue;
			if (glob.skills[count2].level!=count) continue;
			if (glob.skills[count2].parent!=parent) continue;
			if (strncasecmp(word[count+1],glob.skills[count2].name,strlen(word[count+1]))==0) 
			{
				strcat(retval,glob.skills[count2].name);
				parent=glob.skills[count2].vnum;
				if (count!=word_count-1) {
					strcat(retval,".");
				}
				break;
			}
		}
	}
}

/* skill text is passed in the format of..
 *
 * skill.subskill.subskill
 *
 * abbreviations can be used, if the first match will be returned or -1.
 */
short getskillstr(char *str)
{
	short vnum=-1;
	short parent=-1;
	short count=0;
	short count2=0;
	short level=0;

	sprintf(inpstr,"skill %s",str);

	while (strchr(inpstr,'.')) {
		*(strchr(inpstr,'.'))=' ';
	}

	split_inpstr();

//	printf("1-(%s), 2-(%s), 3-(%s), wc-(%d)\n",word[1],word[2],word[3],word_count);
	for (count=0;count<word_count;count++) {
		vnum=-1;
		for (count2=0;count2<MAX_SKILLS;count2++) {
			if (glob.skills[count2].vnum==-1) continue;
			if (glob.skills[count2].parent!=parent) continue;
			if (glob.skills[count2].level!=level) continue;
			if (strncasecmp(word[count+1],glob.skills[count2].name,strlen(word[count+1]))==0) 
			{
				if ((glob.skills[count2].type=='N')&&(count==word_count-1)) 
				{
					parent=-1;
					vnum=-1;
				}
				else
				{
					parent=glob.skills[count2].vnum;
					vnum=glob.skills[count2].vnum;
				}
			}
		}
		if (vnum==-1) break;
		level++;
	}

	return vnum;
}

void practice(SU)
{
	short vnum,max,current,incr;
	char tmp[255];

	if (word_count<1) {
		sprintf(text,"You have %d usable practice point%s.\n",UU.prac,UU.prac==1?"":"s");
		wuser(uid,text);
		return;
	}

	if (UU.room!=guilds[getguild(UU.guild)].pracroom) {
		wuser(uid,"You need to be in your guild training room to do this.\n");
		return;
	}

	if (UU.prac<1) {
		wuser(uid,"You have no practice points left, go fight some more.\n");
		return;
	}

	strcpy(tmp,word[1]);
	vnum=getskillstr(word[1]);

	if (vnum==-1) {
		wuser(uid,"No such skill.\n");
		return;
	}

	max=getskillmax(uid,vnum);
	current=getuskill(uid,vnum);

	if (current>=max) {
		wuser(uid,"You have maxed out that skill.\n");
		return;
	}

	incr=(int)(max*0.05);
	incr+=1;
	current+=incr;

	if (current>max) current=max;

	setuskill(uid,vnum,current);
	UU.prac-=1;

	transskill();
	sprintf(text,"You advance %s.\n",retval);
	wuser(uid,text);
}
 
/* actual skills....
 *
 * This is all pretty simple really, this section is for command based skills
 * which will either be instant, such as charge or be queued so that the
 * player can only perform one per turn (like kick/stab/etc).
 *
 * The former is treated just a a normal command with a test for the
 * appropriate skill at the start.
 *
 * The latter has two parts, the initial queuer, which should determine if
 * the player has practiced that skill and if the situation is appropriate
 * to *try* and use the skill, it should then add the skill number to the 
 * queue using addqueue, along with any parameters, you need to take 
 * particular care to ensure that when the skill is dealt with in the
 * second part; doskills; that you 'popqueue' any parameters.
 *
 * At the end of combat the players queue is removed.
 */

char *popqueue(SU)
{
	char *temp;
	char tempq[MAXQUEUE];

	bzero(ret,MAXQUEUE);

	if (UU.queue[0]=='\0') {
		return NULL;
	}

	temp=strchr(UU.queue,' ');

	if (temp==NULL) {
		temp="";
	}

	strncpy(ret,UU.queue,temp-UU.queue);

	if (temp[0]!='\0') temp++;
	strcpy(tempq,temp);
	strcpy(UU.queue,tempq);

	return ret;
}

short addqueue(SU,char *str)
{
	if (strlen(UU.queue)+strlen(str)>MAXQUEUE) {
		return 0;
	}

	strcat(UU.queue,str);
	return 1;
}

/* **************** combat ************ */
void Sretreat(SU)
{
	if (getuskill(uid,3)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"Retreat from what exactly?\n");
		return;
	}

	sprintf(text,"3 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

/* You can only disarm whoever you are attacking
 * this will cause the mob's weapon to drop to the
 * floor but will also cause their rating to drop
 * and the resultant xp.
 */
void Sdisarm(SU)
{
	if (getuskill(uid,6)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"Disarm who exactly?\n");
		return;
	}

	sprintf(text,"6 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Sburn(SU)
{
	if (getuskill(uid,8)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to burn?\n");
		return;
	}

	if (UU.worn[10]==-1) {
		wuser(uid,"You need a burning torch to burn someone.\n");
		return;
	}

	if (!getoflag(UU.worn[10],oLIGHT)) {
		wuser(uid,"You need a burning torch to burn someone.\n");
		return;
	}

	sprintf(text,"8 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Sstab(SU)
{
	if (getuskill(uid,9)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to stab?\n");
		return;
	}

	if (UU.worn[10]==-1) {
		wuser(uid,"You need to hold a dagger to stab someone.\n");
		return;
	}

	if (objs[UU.worn[1]].type!=5) {
		wuser(uid,"You need to hold a dagger to stab someone.\n");
		return;
	}

	sprintf(text,"9 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

/* A player can rescue any other player so long as they are not being attacked
 * currently by any mob.
 *
 * If the rescue is successfull then the players attention turns to the first
 * mob found attacking the object player and thats mobs attention turns to
 * the rescuer.
 */
void Srescue(SU)
{
	if (getuskill(uid,13)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"rescue who from what exactly?\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Rescue who?\n");
		return;
	}

	sprintf(text,"13 %s ",word[1]);
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Skick(SU)
{
	if (getuskill(uid,15)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to kick?\n");
		return;
	}

	sprintf(text,"15 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Sstrangle(SU)
{
	if (getuskill(uid,16)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to strangle?\n");
		return;
	}

	sprintf(text,"16 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Spunch(SU)
{
	if (getuskill(uid,17)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to punch?\n");
		return;
	}

	sprintf(text,"17 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Sheadbutt(SU)
{
	if (getuskill(uid,18)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	if (UU.fight==-1) {
		wuser(uid,"You're not fighting anyone to headbutt?\n");
		return;
	}

	sprintf(text,"18 ");
	if (!addqueue(uid,text)) {
		wuser(uid,"I doubt you can remember to do even one more thing.\n");
		return;
	}
}

void Sfire(SU)
{
}

// Same as KILL
// Does not use the skill queue
void Scharge(SU)
{
	short mob=0;
	short rc=0;
	short sub=0;

	DEBUG("Scharge")

	if (getuskill(uid,26)<1) {
		wuser(uid,"Like you'd know how.\n");
		return;
	}

	rc=getroom(UU.room);

	if (UU.immstatus) {
		wuser(uid,"Immortals cannot fight.\n");
		return;
	}

	if (UU.sleep>0) {
		wuser(uid,"You must be awake and standing to fight.\n");
		return;
	}

	if (getrflag(rc,PEACE)) {
		wuser(uid,"No fighting may occur here!\n");
		return;
	}

	
	if (UU.fight!=-1) {
		wuser(uid,"You are already fighting!\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Charge who?\n");
		return;
	}

	mob=getmob(uid,comstr);
	if (mob==-1) {
		wuser(uid,"No one of that name here!\n");
		return;
	}
	if (mobs[mob].fight>-1) {
		wuser(uid,"That is already fighting someone!\n");
		return;
	}

	UU.fight=mob;
	mobs[mob].fight=uid;
	UU.invis=0;

	if (psuedo_rand(CHANCE)<getuskill(uid,26)) {
		sub=(int)(mobs[mob].hp*0.10);
		sprintf(text,"You ^bCHARGE^N %s! [%d]\n",mobs[mob].name,sub);
		wuser(uid,text);
		sprintf(text,"%s ^bCHARGES^N %s! [%d]\n",UU.name,mobs[mob].name,sub);
		wroom(UU.room,text,uid);
		mobs[mob].hp-=sub;
	}
	else
	{
		wuser(uid,"You fail to charge.\n");
	}
}

/* ************************************* */

/* the actual skills command called from auto.c
 */

void doskills(SU)
{
	short skillno=0;
	short target=0;
	short count=0;
	char p1[MAXQUEUE];

	if (UU.queue[0]=='\0') {
		// nothing to do
		return;
	}

	skillno=atoi(popqueue(uid));
	//sprintf(text,"Skill = %d\n",skillno);
	//wuser(uid,text);

	/* it depends on the skill as to what else needs popping off
	 * the queue
	 */
	switch (skillno) {
		case 3: // retreat, pretty much the same as flee, but with no xp loss
			retreat(uid);
			break;
		case 6: // disarm
			if (mobs[UU.fight].worn[11]==-1) {
				sprintf(text,"%s is not armed.\n",mobs[UU.fight].name);
				wuser(uid,text);
				return;
			}

			if (psuedo_rand(CHANCE)>getuskill(uid,6)) {
				sprintf(text,"You fail to disarm %s.\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"%s fails to disarm %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				return;
			}
			// disarm succeeded
			objs[mobs[UU.fight].worn[11]].user=-1;
			objs[mobs[UU.fight].worn[11]].mob=-1;
			objs[mobs[UU.fight].worn[11]].room=UU.room;
			mobs[UU.fight].worn[11]=-1;
			sprintf(text,"^bYou disarm %s.^N\n",mobs[UU.fight].name);
			wuser(uid,text);
			sprintf(text,"^b%s disarms %s.^N\n",UU.name,mobs[UU.fight].name);
			wroom(UU.room,text,uid);
			break;
		case 8: // burn
			if (psuedo_rand(CHANCE)<getuskill(uid,8)) {
				sprintf(text,"^bYou set %s alight!^N\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"^b%s burns %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=10;
			}
			else
			{
				wuser(uid,"You fail to burn anything.\n");
			}
			break;
		case 9: // stab
			if (psuedo_rand(CHANCE)<getuskill(uid,9)) {
				sprintf(text,"^bYou stab %s in the face!^N\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"^b%s stabs %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=10;
			}
			else
			{
				wuser(uid,"You fail to stab anything.\n");
			}
			break;
		case 13: // rescue
			strcpy(p1,popqueue(uid));
			target=getuser(uid,p1);
			if (target==-1) {
				wuser(uid,"That person isn't here to rescue.\n");
				return;
			}
			if (users[target].room!=UU.room) {
				wuser(uid,"That person isn't here to rescue.\n");
				return;
			}
			for (count=0;count<MAXMOBS;count++) {
				if (MC.vnum==-1) continue;
				if (MC.fight!=uid) continue;

				wuser(uid,"You're too busy to rescue anyone.\n");
				return;
			}
			for (count=0;count<MAXMOBS;count++) {
				if (MC.vnum==-1) continue;
				if (MC.fight!=target) continue;

				// mob found, try to rescue
				if (psuedo_rand(CHANCE)<getuskill(uid,13)) {
					MC.fight=uid;
					UU.fight=count;
					sprintf(text,"^bYou rescue %s^N\n",users[target].name);
					wuser(uid,text);
					sprintf(text,"^b%s rescues %s^N\n",UU.name,users[target].name);
					wroom(UU.room,text,uid);
				}
				else
				{
					sprintf(text,"You fail to rescue %s\n",users[target].name);
					wuser(uid,text);
					sprintf(text,"%s fails to rescue %s\n",UU.name,users[target].name);
					wroom(UU.room,text,uid);
				}
				return;
			}
			// if we get here then no mob is attacking target
			sprintf(text,"%s is not under attack.\n",users[target].name);
			wuser(uid,text);
			break;
		case 15: // kick
			if (psuedo_rand(CHANCE)<getuskill(uid,15)) {
				int part;
				char partstr[30];
				part = psuedo_rand(4);
				if(part==1) { sprintf(partstr,"%s","groin"); }
				if(part==2) { sprintf(partstr,"%s","stomach"); }
				if(part==3) { sprintf(partstr,"%s","knee"); }
				if(part==4) { sprintf(partstr,"%s","head"); }
				sprintf(text,"^bYou kick %s squarely in the %s!^N\n",mobs[UU.fight].name,partstr);
				wuser(uid,text);
				sprintf(text,"^b%s kicks %s in the %s.\n",UU.name,mobs[UU.fight].name,partstr);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=5;
			}
			else
			{
				wuser(uid,"You fail to kick anything.\n");
			}
			break;
		case 16: // strangle
			if (psuedo_rand(CHANCE)<getuskill(uid,16)) {
				sprintf(text,"^bYou close your hands around the neck of %s!^N\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"^b%s strangles %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=5;
			}
			else
			{
				wuser(uid,"You fail to strangle anything.\n");
			}
			break;
		case 17: // punch
			if (psuedo_rand(CHANCE)<getuskill(uid,17)) {
				sprintf(text,"^bYou punch %s in the teeth!^N\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"^b%s punches %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=5;
			}
			else
			{
				wuser(uid,"You fail to punch anything.\n");
			}
			break;
		case 18: // headbutt
			if (psuedo_rand(CHANCE)<getuskill(uid,18)) {
				sprintf(text,"^bYou lay the nut on %s!^N\n",mobs[UU.fight].name);
				wuser(uid,text);
				sprintf(text,"^b%s headbutts %s.\n",UU.name,mobs[UU.fight].name);
				wroom(UU.room,text,uid);
				mobs[UU.fight].hp-=5;
			}
			else
			{
				wuser(uid,"You fail to headbutt anything.\n");
			}
			break;
		case 19: // fire
			break;
		case 26: // charge -- NOT USED HERE
			break;
		// Covert..............................
		case 29: // sneak
			break;
		case 30: // hide
			break;
		case 31: // pickpocket
			break;
		case 32: // steal
			break;
		case 33: // pass
			break;
		case 34: // lockpick
			break;
		case 38: // case
			break;
		case 43: // trap
			break;
		// Magic...............................
		// Faith...............................
		case 64: // Manufacture
			break;
		// Other...............................
		case 77: // throw
			break;
		case 79: // map
			break;
		case 80: // read
			break;
		// Default WILL clear the queue
		default: wuser(uid,"Combat queue error, please report to an IMM.. clearing.\n");
			 UU.queue[0]='\0';
			 break;
	}
}

// does the same thing as 'flee' without xp loss
void retreat(SU)
{
	short chance=0;
	short fd=-1,sr=0;
	short count=0;
	short count2=0;

	DEBUG("retreat")
	chance=psuedo_rand(CHANCE);

	if (UU.fight==-1) {
		wuser(uid,"You have nothing to flee from.\n");
		return;
	}

	if (chance>getuskill(uid,3)) {
		wuser(uid,"You failed to retreat!\n");
	} else
	{
		while (fd==-1) {
			count++;
			if (count>20) { /* infinite loop protection */
				wuser(uid,"You failed to retreat!\n");
				return;
			}
			sr=psuedo_rand(10)-1;
			if (rooms[getroom(UU.room)].dir[sr]>0)
				fd=sr;
		}
		sprintf(text,"%s ^bretreats^N %s.\n",UU.name,getdir(fd));
		wroom(UU.room,text,uid);
		UU.room=rooms[getroom(UU.room)].dir[fd];
		UU.fight=-1;
		for (count2=0;count2<MAXMOBS;count2++) {
			if (mobs[count2].vnum==-1) continue;
			if (mobs[count2].fight!=uid) continue;

			mobs[count2].fight=next_victim(count2);
		}
		if(UU.brief)
		{
			brieflook(uid);
		} else {
			look(uid);
		}
		wuser(uid,"You ^bretreat^N\n");
	}
}

// if recite is set then it gets read to a player or the room
void Sread(SU, short recite)
{
	short obj;
	short lang=0;
	char outtxt[4096];
	short count;
	short heldobj;
	short target=-1;

	/* read is no longer a skill
	if (getuskill(uid,80)<1) {
		wuser(uid,"If only you knew how to read.\n");
		return;
	}
	*/

	if (word_count<1) {
		if (recite) {
			wuser(uid,"Recite what?\n");
		}
		else
		{
			wuser(uid,"Read what?\n");
		}
		return;
	}

	obj=getobj(uid, word[1]);
	if (obj==-1) {
		/* Maybe a held object? */
		heldobj=getheldobj(uid, word[1]);
		if (heldobj==-1) {
			wuser(uid, "You do not see that here.\n");
			return;
		}
		/* okay it is */
		obj=UU.held[heldobj];
	}
	/* Examine object */
	if (objs[obj].writing[0]=='\0') {
		wuser(uid,"It doesn't appear to have any writing on it.\n");
		return;
	}
	/* output writing */
	lang=objs[obj].language;

	if ((getuskill(uid,lang)<1)) {
		if (!(UU.rank>NORM&&UU.immstatus)) {
			wuser(uid,"The strange marks make no sense to you at all.\n");
			return;
		}
	}

	if (recite) {
		if (word_count>=2) {
			target=getuser(uid,word[word_count]);
			if (target==-1) {
				wuser(uid,"Nobody here of that name.\n");
				return;
			}
			if (users[target].room!=UU.room) {
				wuser(uid,"Nobody here of that name.\n");
				return;
			}
		}
	}

	srand(1234);
	outtxt[0]='\0';
	for (count=0;count<strlen(objs[obj].writing);count++) {
		if ((psuedo_rand(95)<=getuskill(uid,lang))||(objs[obj].writing[count]=='\n')||((UU.rank>NORM)&&(UU.immstatus))) {
			outtxt[count]=objs[obj].writing[count];
			outtxt[count+1]='\0';
		}
		else
		{
			outtxt[count]='.';
			outtxt[count+1]='\0';
		}
	}
	srand(time(0));

	/* reciting scrolls...
	 *
	 * If the user can first read the scroll, then they can recite it
	 * anyone, even non magic folk can try to recite spells.
	 *
	 * This will detect that it is a scroll, figure out what it is aimed
	 * at then have a go by calling scroll(uid, objnum, target, mob?)
	 *
	 * if it is a combat scroll then it will be entered into the combat
	 * queue under what ever skill the discipline is with the object vnum
	 * as the queue parameter, this should prevent scrolls being
	 * inadvertantly used after combat, which would suck, big time
	 */
	if (recite) {
		if (objs[obj].type==SCROLL) {
			if (target>-1) {
				sprintf(text,"You concentrate on %s\n",target==uid?"yourself":users[target].name);
				wuser(uid,text);
				sprintf(text,"%s concentrates on %s\n",UU.name,target==uid?getsex(uid,"himself","herself","itself"):users[target].name);
				wroom(UU.room,text,uid);
			}
			sprintf(text,"You recite %s\n",objs[obj].sdesc);
			wuser(uid,text);
			sprintf(text,"%s recites %s\n",UU.name,objs[obj].sdesc);
			wroom(UU.room,text,uid);

			// dont support casting on mobs right now
			scroll(uid,obj,target,0);
		}
		else
		{
			if (target>-1) {
				sprintf(text,"You recite %s to %s\n",objs[obj].sdesc,uid==target?"yourself":users[target].name);
				wuser(uid,text);
				if (target!=uid) {
					sprintf(text,"%s recites to you:\n",UU.name);
					wuser(target,text);
				}
				wrapout(target,outtxt,'J','N');
			}
			else
			{
				sprintf(text,"%s recites %s:\n",UU.name,objs[obj].sdesc);
				wroom(UU.room,text,uid);
				wuser(uid,"You recite to others:\n");
				wrapoutroom(UU.room,outtxt,'J','N',-1);
//				wroom(UU.room,"\n",-1);
			}
		}
		return;
	}
	// read, dont recite
	wuser(uid,"It reads:\n");
	wrapout(uid,outtxt,'J','N');
	sprintf(text,"%s reads %s\n",UU.name,objs[obj].sdesc);
	wroom(UU.room,text,uid);
}

/* throw has a few uses, 1 is to lob bombs n stuff which can be done
 * at any time, the other is for ranged combat, which is a combat
 * queue issue, this function deals with the former use
 */
void Sthrow(SU)
{
	short objno,hobj,rm,dir;
	short newroom;

	if (getuskill(uid,77)<1) {
		wuser(uid,"You have no idea how to throw anything.\n");
		return;
	}

	if (word_count<2) {
		wuser(uid,"Throw what where?\n");
		return;
	}
	
	if (UU.sleep==2) {
		wuser(uid,"You are far too asleep to throw at the moment.\n");
		return;
	}

	hobj=getheldobj(uid,word[1]);

	if (hobj==-1) {
		wuser(uid,"You can only throw things which you are carrying.\n");
		return;
	}

	dir=getdirnum(word[2]);

	if (dir==-1) {
		wuser(uid,"Throw it where?\n");
		return;
	}

	rm=getroom(UU.room);

	if (rooms[rm].dir[dir]==-1) {
		wuser(uid,"Throw it where?\n");
		return;
	}

	if (getoflag(UU.held[hobj],oNODROP)) {
		wuser(uid,"That just refuses to be thrown anywhere.\n");
		return;
	}

	if (psuedo_rand(CHANCE)>getuskill(uid,77)) {
		sprintf(text,"You totally fumble %s and it drops at your feet.\n",objs[UU.held[hobj]].sdesc);
		wuser(uid,text);
		newroom=UU.room;
	}
	else
	{
		sprintf(text,"You lob %s %s.\n",objs[UU.held[hobj]].sdesc,getdir(dir));
		wuser(uid,text);
		sprintf(text,"%s throws %s %s.\n",UU.name,objs[UU.held[hobj]].sdesc,getdir(dir));
		wroom(UU.room,text,uid);
		newroom=rooms[rm].dir[dir];
	}

	objno=UU.held[hobj];
	UU.held[hobj]=-1;

	objs[objno].user=-1;
	objs[objno].mob=-1;
	objs[objno].room=newroom;

	/* call some object related action func */
	// actually maybe not ;)
}

// allows a user to move from room to room without any notifications to either
// rooms
void Ssneak(SU)
{
	short dir;

	if (getuskill(uid,29)<1) {
		wuser(uid,"You wish you knew how.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Sneak where?\n");
		return;
	}
	
	dir=getdirnum(word[2]);

	if (dir==-1) {
		wuser(uid,"That isn't a direction, let alone one you can sneak to!\n");
		return;
	}

	if (psuedo_rand(CHANCE)>getuskill(uid,29)) {
		wuser(uid,"You fail to sneak there.\n");
		move(uid,dir,1);
	}
	else
	{
		wuser(uid,"You lie low and prepare to sneak off...\n");
		move(uid, dir, 0);
	}
}

void Shide(SU)
{
	if (getuskill(uid,30)<1) {
		wuser(uid,"You wish you knew how.\n");
		return;
	}

	if (UU.hidden>0) {
		wuser(uid,"You are already hidden away.\n");
		return;
	}

	if (psuedo_rand(CHANCE)>getuskill(uid,30)) {
		wuser(uid,"You fail to hide and make a lot of noise.\n");
		sprintf(text,"%s falls over trying to hide.\n",UU.name);
		wroom(UU.room,text,uid);
	}
	else
	{
		wuser(uid,"You hide away out of sight...\n");
		UU.hidden=1;
	}

}

void Spickpocket(SU)
{
	short mob;
	long amount;

	if (getuskill(uid,31)<1) {
		wuser(uid,"You wish you knew how.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Take how much from whom?\n");
		return;
	}

	if (word_count<2) {
		wuser(uid,"Who from?\n");
		return;
	}

	amount=atol(word[1]);

	if (amount<1) {
		wuser(uid,"Somewhat strange amount to try and steal.\n");
		return;
	}

	if (amount>100) {
		wuser(uid,"You really won't get away with trying for that much.\n");
		return;
	}

	mob=getmob(uid,word[2]);
	if (mob==-1) {
		wuser(uid,"No one of that name here!\n");
		return;
	}
	if (mobs[mob].fight>-1) {
		wuser(uid,"That is fighting someone!\n");
		return;
	}
	if (mobs[mob].room!=UU.room) {
		wuser(uid,"No one of that name here!\n");
		return;
	}

	if (mobs[mob].cash<amount) {
		amount=mobs[mob].cash;
	}

	if (psuedo_rand(CHANCE)>getuskill(uid,31)) {
		unhide(uid);
		wuser(uid,"You are not as light fingered as you think.\n");
		sprintf(text,"%s grabs your arm... 'Oi!'.\n",mobs[mob].name);
		wuser(uid,text);
		mobs[mob].fight=uid;  // rumble
	}
	else
	{
		wuser(uid,"You lift some dosh\n");
		UU.cash+=amount;
		mobs[mob].cash-=amount;
	}
}
