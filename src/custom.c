/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :custom.c
** Purpose :Provides commands for user customisation
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 05-02-01 0.00 ksb First Version
*/

#include "smud.h"

extern void wroom(short, char *, short);
extern short getguild(short);
extern short getroom(short);
extern void wuser(SU, char *);
extern void line(short,char *);

void shortcuts(SU)
{
	short count;
	short abrno;
	short abrmax=0;

	DEBUG("shortcuts")

	if (word_count<1) {
		line(uid,"Shortcuts");
		for (count=0;UU.abrchr[count]!='*';count++) {
			sprintf(text,"[%02d] '%c' - %s\n",count,UU.abrchr[count],UU.abrstr[count]);
			wuser(uid,text);
		}
		line(uid,NULL);
		return;
	}

	if (word_count<2) {
		wuser(uid,"You need to specify a shortcut number and a command.\n");
		return;
	}

	for (count=0;UU.abrchr[count]!='*';count++) {
		abrmax++;
	}

	abrmax--;

	abrno=atoi(word[1]);

	if (abrno>abrmax||abrno<0) {
		sprintf(text,"Choose a shortcut between 0 and %d.\n",abrmax);
		wuser(uid,text);
		return;
	}

	sprintf(UU.abrstr[abrno],"%s ",word[2]);

	wuser(uid,"Shortcut changed.\n");
}

void xprompt(SU)
{
	DEBUG("xprompt")

	if (word_count<1) {
		wuser(uid,"You set your prompt to default.\n");
		strcpy(UU.prompt,"%hhp %mm %vmv>");
		return;
	}

	if (strlen(comstr)>127) {
		wuser(uid,"New prompt is too large!\n");
		return;
	}

	strcpy(UU.prompt,comstr);
	wuser(uid,"New prompt set.\n");
}

void xcprompt(SU)
{
	DEBUG("xcprompt")

	if (word_count<1) {
		wuser(uid,"You set your combat prompt to default.\n");
		strcpy(UU.cprompt,"%hhp %mm %vmv>");
		return;
	}

	if (strlen(comstr)>127) {
		wuser(uid,"New prompt is too large!\n");
		return;
	}

	strcpy(UU.cprompt,comstr);
	wuser(uid,"New combat prompt set.\n");
}

void wimpy(SU)
{
	short tmpwimp=0;

	DEBUG("wimpy")

	if (word_count<1) {
		if (UU.wimpy==0) {
			sprintf(text,"You currently have no wimp factor, good luck!\n");
		}
		else
		{
			sprintf(text,"You will autoflee when your hit points drop below %d.\n",UU.wimpy);
		}
		wuser(uid,text);
		return;
	}

	tmpwimp=atoi(word[1]);
	if (tmpwimp<0 || tmpwimp>UU.maxhp) {
		wuser(uid,"Don't be silly.\n");
		return;
	}

	if (tmpwimp>(int)(UU.maxhp/2)) {
		wuser(uid,"Your wimpy must be less than half your maximum hp.\n");
		return;
	}

	UU.wimpy=tmpwimp;

	if (UU.wimpy==0) {
		wuser(uid,"There is no escape now, pick your fights carefully.\n");
		return;
	}

	sprintf(text,"You set your wimpy to %d.\n",UU.wimpy);
	wuser(uid,text);
}

	
/*
void title(SU)
{
	DEBUG("title")
	if (word_count<1) {
		wuser(uid,"You reset your title.\n");
		UU.title[0]='\0';
		return;
	}

	if (strlen(comstr)>42) {
		wuser(uid,"Title is too long.\n");
		return;
	}

	UU.title[0]='\0';
	strcpy(UU.title,comstr);
	sprintf(text,"You set your title to:\n%s %s\n",UU.name,UU.title);
	wuser(uid,text);
}
*/

void train(SU)
{
	short rc=0;
	if (word_count<1) {
		wuser(uid,"Train what?\n");
		return;
	}

	rc=getroom(UU.room);

	if (UU.room!=guilds[getguild(UU.guild)].pracroom) {
		wuser(uid,"You need to be in your guild training room to do this.\n");
		return;
	}

	if (UU.prac<4) {
		wuser(uid,"You do not have enough practice points.\n");
		return;
	}
	
	if (strcasecmp(word[1],"str")==0) {
		if (UU.str>=UU.maxstr) {
			wuser(uid,"You have that trained to the max.\n");
			return;
		}
		UU.str++;
		wuser(uid,"^YYou advance your strength.^N\n");
		sprintf(text,"%s works out with his trainer.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.prac-=4;
		return;
	}

	if (strcasecmp(word[1],"dex")==0) {
		if (UU.dex>=UU.maxdex) {
			wuser(uid,"You have that trained to the max.\n");
			return;
		}
		UU.dex++;
		wuser(uid,"^YYou advance your dexterity.^N\n");
		sprintf(text,"%s works out with his trainer.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.prac-=4;
		return;
	}

	if (strcasecmp(word[1],"int")==0) {
		if (UU.intl>=UU.maxint) {
			wuser(uid,"You have that trained to the max.\n");
			return;
		}
		UU.intl++;
		wuser(uid,"^YYou advance your intelligence.^N\n");
		sprintf(text,"%s works out with his trainer.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.prac-=4;
		return;
	}

	if (strcasecmp(word[1],"con")==0) {
		if (UU.con>=UU.maxcon) {
			wuser(uid,"You have that trained to the max.\n");
			return;
		}
		UU.con++;
		wuser(uid,"^YYou advance your constitution.^N\n");
		sprintf(text,"%s works out with his trainer.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.prac-=4;
		return;
	}

	if (strcasecmp(word[1],"wis")==0) {
		if (UU.wis>=UU.maxwis) {
			wuser(uid,"You have that trained to the max.\n");
			return;
		}
		UU.wis++;
		wuser(uid,"^YYou advance your wisdom.^N\n");
		sprintf(text,"%s works out with his trainer.\n",UU.name);
		wroom(UU.room,text,uid);
		UU.prac-=4;
		return;
	}

	wuser(uid,"No such stat (str|wis|con|int|dex).\n");
}
