/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :lists.c
** Purpose :Associations lists
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 01-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern short isoffline(char *);
extern short getuser(SU,char *);
extern void line(SU,char *);
extern char *getsex(SU,char *, char *, char *);
extern void wuser(SU, char *);
extern void outwho(SU,short);


void loadlist()
{
	FILE *fp;
	short cur=0;

        DEBUG("loadlist")

	sprintf(text,"%suserlists.def",USERDIR);
	if (!(fp=fopen(text,"r"))) {
		printf("Loading lists didnt work.\n");
		return;
	}

	while (fgets(text,255,fp)) {
		if (text[0]=='#') break;
		text[strlen(text)-1]='\0';
		strcpy(glob.lists[cur].owner,text);
		fgets(text,255,fp);
		text[strlen(text)-1]='\0';
		strcpy(glob.lists[cur].about,text);
		fgets(text,255,fp);
		text[strlen(text)-1]='\0';
		strcpy(glob.lists[cur].flags,text);
		cur++;
	}
	fclose(fp);
}
		
void savelist()
{
	FILE *fp;
	short count;

        DEBUG("savelist")

	sprintf(text,"%suserlists.def",USERDIR);
	if (!(fp=fopen(text,"w"))) {
		printf("Saving lists didnt work.\n");
		return;
	}

	for (count=0;count<MAX_LISTS;count++) {
		if (glob.lists[count].owner[0]=='\0') continue;
		fprintf(fp,"%s\n",glob.lists[count].owner);
		fprintf(fp,"%s\n",glob.lists[count].about);
		fprintf(fp,"%s\n",glob.lists[count].flags);
	}
	fprintf(fp,"#### end of file\n");
	fclose(fp);
}

/* if uid has flag set for about?
 */
short checklist(SU,char *about,short flag)
{
	short count;

        DEBUG("checklist")

	for (count=0;count<MAX_LISTS;count++) {
		if (glob.lists[count].owner[0]=='\0') continue;
		if (strcasecmp(UU.name, glob.lists[count].owner)!=0) continue;
		if (strcasecmp(about, glob.lists[count].about)!=0) continue;
		if (glob.lists[count].flags[flag]=='1') {
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

void fwho(SU)
{
/*	short count; - Not used yet */

	outwho(uid,1);
	/*
	line(uid,"Friends currently online");
	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.login!=0) continue;
		if (checklist(uid, UC.name, lFRIEND)) {
			sprintf(text,"%-12s",UC.name);
			wuser(uid,text);
		}
	}
	line(uid,NULL);
	*/
}

void tf(SU)
{
	short count;
	short target=-1;
	char buf[4096];


        DEBUG("tf")

	if (word_count<1) {
		wuser(uid,"Tell your friends what?\n");
		return;
	}

	if (isdigit(word[1][0])) {
		/* assume trying to talk to another users friends list */
		target=atoi(word[1]);
		if (target<1 || target>=MAXUSERS) {
			target=-1;
		}
		else
		{
			target--;
			if (target==uid) {
				target=-1;
				if (word_count<2) {
					wuser(uid,"Tell your them what?\n");
					return;
				}
				strcpy(buf,comstr);
				strcpy(comstr,strchr(buf,' ')+1);
			}
			else
			{
				if (!checklist(target,UU.name,lFRIEND)) {
					wuser(uid,"You are not on their friends list.\n");
					return;
				}
			}
			if (word_count<2) {
				wuser(uid,"Tell your them what?\n");
				return;
			}
			
		}
	}

	sprintf(text,"^rYou tell %s%s friends '%s'^N\n",target==-1?"Your":users[target].name,target==-1?"":"'s",target==-1?comstr:strchr(comstr,' ')+1);
	wuser(uid,text);
	for (count=0;count<MAXUSERS;count++) {
		if (count==uid) continue;
		if (target>-1) {
			if (checklist(target, UC.name, lFRIEND)||target==count) {
				sprintf(text,"^r(%d) %s tells %s%s friends '%s'^N\n",target+1,UU.name,count==target?"Your":users[target].name,count==target?"":"'s",target==-1?comstr:strchr(comstr,' ')+1);
				wuser(count,text);
			}
		}
		else
		{
			if (checklist(uid, UC.name, lFRIEND)) {
				sprintf(text,"^r(%d) %s tells %s friends '%s'^N\n",uid+1,UU.name,getsex(uid,"his","her","its"),comstr);
				wuser(count,text);
			}
		}
	}
}

void rf(SU)
{
	short count;
	short target=-1;
	char buf[4096];


        DEBUG("tf")

	if (word_count<1) {
		wuser(uid,"Remote your friends what?\n");
		return;
	}

	if (isdigit(word[1][0])) {
		/* assume trying to talk to another users friends list */
		target=atoi(word[1]);
		if (target<1 || target>=MAXUSERS) {
			target=-1;
		}
		else
		{
			target--;
			if (target==uid) {
				target=-1;
				if (word_count<2) {
					wuser(uid,"Remote them what?\n");
					return;
				}
				strcpy(buf,comstr);
				strcpy(comstr,strchr(buf,' ')+1);
			}
			else
			{
				if (!checklist(target,UU.name,lFRIEND)) {
					wuser(uid,"You are not on their friends list.\n");
					return;
				}
			}
			if (word_count<2) {
				wuser(uid,"Remote them what?\n");
				return;
			}
		}
	}

	sprintf(text,"^r[%s%s friends] %s %s^N\n",target==-1?"Your":users[target].name,target==-1?"":"'s",UU.name,target==-1?comstr:strchr(comstr,' ')+1);
	wuser(uid,text);
	for (count=0;count<MAXUSERS;count++) {
		if (count==uid) continue;
		if (target>-1) {
			if (checklist(target, UC.name, lFRIEND)||target==count) {
				sprintf(text,"^r(%d) [%s%s friends] %s %s^N\n",target+1,count==target?"Your":users[target].name,count==target?"":"'s",UU.name,target==-1?comstr:strchr(comstr,' ')+1);
				wuser(count,text);
			}
		}
		else
		{
			if (checklist(uid, UC.name, lFRIEND)) {
				sprintf(text,"^r(%d) [%s's friends] %s %s^N\n",uid+1,UU.name,UU.name,comstr);
				wuser(count,text);
			}
		}
	}
}

/*
void rf(SU)
{
	short count;

        DEBUG("rf")

	if (word_count<1) {
		wuser(uid,"remote your friends what?\n");
		return;
	}
	sprintf(text,"^r[Friends] %s %s^N\n",UU.name,comstr);
	wuser(uid,text);
	for (count=0;count<MAXUSERS;count++) {
		if (checklist(uid, UC.name, lFRIEND)) {
			sprintf(text,"^r(%d) [Friends] %s %s^N\n",uid,UU.name,comstr);
			wuser(count,text);
		}
	}
}
*/

void showlist(SU)
{
	short count,count2,lc=0;
	char key[128];

        DEBUG("showlist")

	sprintf(key,"%s","");

	count=0;
	while (listsdesc[count][0]!='*') {
		strcat(key,listsdesc[count]);
		strcat(key," ");
		count++;
	}
	line(uid,"Your list");
	sprintf(text,"%-13s - %s\n","  -- name --  ",key);
	wuser(uid,text);

	for (count2=0;count2<MAX_LISTS;count2++) {
		if (strcasecmp(UU.name,glob.lists[count2].owner)!=0) continue;
		lc++;
		sprintf(key,"%s","");
		count=0;
		while (listsdesc[count][0]!='*') {
			if (glob.lists[count2].flags[count]!='0') {
				strcat(key,"YES");
			}
			else
			{
				strcat(key,"   ");
			}
			strcat(key," ");
			count++;
		}
		sprintf(text,"%-13s  - %s\n",glob.lists[count2].about,key);
		wuser(uid,text);
	}
	sprintf(text,"%d entries",lc);
	line(uid,text);
}

void dolist(SU,short wh)
{
	short target,killit;
	short count,got=-1;

	DEBUG("dolist")

	if (word_count<1) {
		wuser(uid,"Set flag for who?\n");
		return;
	}

	target=getuser(uid,word[1]);

	if (target==-1) {
		target=isoffline(word[1]);
	}

	if (target==-1) {
		wuser(uid,"Who?\n");
		return;
	}

	if(target==uid) {
		wuser(uid,"Awwww maybe someone else will pay you attention.\n");
		return;
	}

	/* look for existing record */
	for (count=0;count<MAX_LISTS;count++) {
		if (strcasecmp(UU.name,glob.lists[count].owner)!=0) continue;
		if (strcasecmp(users[target].name,glob.lists[count].about)!=0) continue;
		got=count;
		break;
	}
	
	if (got==-1) {
		for (count=0;count<MAX_LISTS;count++) {
			if (glob.lists[count].owner[0]!='\0') continue;
			got=count;
			break;
		}
	}

	if (got==-1) {
		wuser(uid,"There is no more space in the MUD for list entries, please whinge at an IMM.\n");
		return;
	}

	/* firstly lets copy the info into got, who cares if it's already
	 * there
	 */

	strcpy(glob.lists[got].owner,UU.name);
	strcpy(glob.lists[got].about,users[target].name);

	if(glob.lists[got].flags[wh]=='0') {
		glob.lists[got].flags[wh]='1';
		sprintf(text,"You set the %s flag for %s.\n",listdesc[wh],users[target].name);
		wuser(uid,text);
		sprintf(text,"%s sets %s ^Y%s^N flag for you.\n",UU.name,getsex(uid,"his","her","its"),listdesc[wh]);
		wuser(target,text);
	}
	else
	{
		glob.lists[got].flags[wh]='0';
		sprintf(text,"You unset the %s flag for %s.\n",listdesc[wh],users[target].name);
		wuser(uid,text);
		sprintf(text,"%s unsets %s ^Y%s^N flag for you.\n",UU.name,getsex(uid,"his","her","its"),listdesc[wh]);
		wuser(target,text);
	}

	/* check to see if entry is still needed ? */

	killit=1;
	for (count=0;count<20;count++) {
		if (glob.lists[got].flags[count]!='0') killit=0;
	}

	if (killit) {
		sprintf(text,"You remove the list entry for %s.\n",users[target].name);
		wuser(uid,text);
		glob.lists[got].owner[0]='\0';
		glob.lists[got].about[0]='\0';
	}
	savelist();
}

