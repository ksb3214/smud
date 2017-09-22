/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :editor.c
** Purpose :Smuds very own line editor
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-03-02 0.00 ksb First Version
*/

#include "smud.h"

extern void editroomsave(SU);
extern void wuser(SU, char *);
extern short savenewsitem(SU);
extern void editsignsave(SU);
extern void editdebatesave(SU);
extern void editdebatequit(SU);
extern void wrapout(SU,char *,char, char);
extern void finishmail(SU);

void finish_edit(SU)
{
	switch (UU.eid) {
		case 1 : editroomsave(uid); break;
		case 2 : savenewsitem(uid); break;
		case 3 : editsignsave(uid); break;
		case 4 : editdebatesave(uid); break;
		case 5 : finishmail(uid); break;
		default : break;
	}

	UU.eid=0;
}

void quit_edit(SU)
{
        switch (UU.eid) {
                case 4 : editdebatequit(uid); break;
                default : break;
        }

        UU.eid=0;
}


short numlines(SU)
{
	short lc=0;
	short count;
	char *str;
	char *gotstr=NULL;

	str=UU.edit;
	gotstr=str;

	for (count=0;*str!='\0';count++) {
		if ((*str=='\n')&&(*(str+1)=='\n')) {
			lc++;
			str+=2;
			gotstr=str;
		}
		else str++;
	}

	if (str!=gotstr) lc++; // last line has no \n
	return lc;
}

void delcurrent(SU)
{
	char lstr[4096];
	char *str;
	short lc=1;
	short count;
	char *endline=NULL;

	str=UU.edit;
	lstr[0]='\0';

	if (UU.eline>1) {
		for (count=0;*str!='\0';count++) {
			lstr[count]=*str;
			lstr[count+1]='\0';
			if (count>0) {
				if ((*str=='\n')&&(*(str-1)=='\n')) lc++;
			}
			str++;
			if (lc==UU.eline) break;
		}
	}

	endline=strstr(str,"\n\n");
	if (!endline) {
		endline=strchr(str,'\0');
	}
	else
	{
		endline+=2;
	}
	sprintf(text,"%s%s",lstr,endline);
	strcpy(UU.edit,text);
	
	wuser(uid,"Deleted\n");

	if (UU.eline>numlines(uid)) {
		UU.eline--;
		if (UU.eline==0) {
			wuser(uid,"Buffer wiped.\n");
			UU.eline=999;
		}
	}
}

/* outputs line ln to the user of their current buffer */
void outline(SU,short ln)
{
	char *str;
/*	short done=0; - not used yet */
	char lstr[4096];
	char *lineend;
	short curline;

	str=UU.edit;

	if (ln>numlines(uid)) {
		wuser(uid,"[End of buffer]\n");
		return;
	}

	if (*str=='\0') {
		wuser(uid,"[Buffer empty]\n");
		return;
	}

	curline=1;

	while (*str!='\0')
	{
		if (curline==ln) {
			break;
		}
		if ((*str=='\n')&&(*(str+1)=='\n')) {
			curline++;
			str+=2;
		}
		else str++;
	}

	if (*str=='\0') {
		wuser(uid,"Editor error, notify an IMM.\n");
		UU.eid=0;
	}

	lineend=strstr(str,"\n\n");

	if (!lineend) {
		lineend=strchr(str,'\0');
	}

	strncpy(lstr,str,lineend-str);
	lstr[lineend-str]='\0';

	sprintf(text,"[%d]%c%s\n",ln,ln==UU.eline?'*':ln==numlines(uid)&&UU.eline==999?'+':' ',lstr);
	wrapout(uid,text,'L','N');
}

void editstats(SU)
{
	short cursize=0;

	cursize=strlen(UU.edit);
	sprintf(text,"[^BEDITOR^N] %d/%d bytes used. .? for help.\n",cursize,UU.maxedit);
	wuser(uid,text);
}

/* maxedit must be less than 4096 */
void start_edit(SU,short eid, short maxedit, char *cur)
{
	if (UU.eid!=0) {
		wuser(uid,"You already seem to be in the editor... neat trick.\n");
		return;
	}

	UU.eid=eid;

	if (cur) {
		strcpy(UU.edit,cur);
	}
	else
	{
		UU.edit[0]='\0';
	}

	UU.maxedit=maxedit;
	UU.eline=999; /* last line */

	editstats(uid);
}

void testedit(SU)
{
	start_edit(uid,1,2048,NULL);
}

/* okay so inpstr holds the current editor command, UU.edit should have the
 * current stream, and if they quit UU.eid should indicate what to do
 */
void do_edit(SU)
{
	char lstr[4096];
	char *str;
	short lc;
	short count;

	str=UU.edit;
	lstr[0]='\0';

	if (inpstr[0]=='.') {
		/* command */
		if (strcmp(inpstr,".view")==0) {
			if (UU.edit[0]=='\0') {
				wuser(uid,"[^BEDITOR^N] Buffer empty.\n");
				return;
			}
			for (count=1;count<=numlines(uid);count++) {
				outline(uid,count);
			}
			return;
		}

		if (strcmp(inpstr,".end")==0) {
			wuser(uid,"[^BEDITOR^N] ^GSaved^N.\n");
			finish_edit(uid);
			return;
		}

		if (strcmp(inpstr,".quit")==0) {
			wuser(uid,"[^BEDITOR^N] ^RAbandoned^N.\n");
			quit_edit(uid);
			return;
		}

		if (strcmp(inpstr,".stats")==0) {
			editstats(uid);
			return;
		}

		if (strcmp(inpstr,".-")==0) {
			if (UU.eline==999) {
				UU.eline=numlines(uid);
				if (UU.eline==0) {
					wuser(uid,"[^BEDITOR^N] Buffer currently empty.\n");
					UU.eline=999;
					return;
				}
			}
			else
			{
				UU.eline--;
			}
			if (UU.eline==0) {
				wuser(uid,"[^BEDITOR^N] Already at top of buffer.\n");
				UU.eline=1;
				return;
			}
			outline(uid,UU.eline);
			return;
		}

		if (strcmp(inpstr,".+")==0) {
			if (UU.eline==999) {
				wuser(uid,"[^BEDITOR^N] Already at end of buffer.\n");
				return;
			}
			UU.eline++;
			if (UU.eline>numlines(uid)) UU.eline=999;
			outline(uid,UU.eline);
			return;
		}

		if (strcmp(inpstr,".del")==0) {
			if (UU.eline==999) {
				wuser(uid,"[^BEDITOR^N] Can't delete past end of buffer.\n");
				return;
			}
			delcurrent(uid);
			return;
		}

		if (strcmp(inpstr,".wipe")==0) {
			wuser(uid,"[^BEDITOR^N] ^rBuffer wiped!^N\n");
			UU.edit[0]='\0';
			UU.eline=999;
			return;
		}

		if (strcmp(inpstr,".?")==0) {
			wuser(uid,"[^BEDITOR^N] Commands...\n");
			wuser(uid,"         .quit   - Quit and abandon changes.\n");
			wuser(uid,"         .end    - Quit and save changes.\n");
			wuser(uid,"         .stats  - Current buffer stats.\n");
			wuser(uid,"         .del    - Delete current line.\n");
			wuser(uid,"         .wipe   - Wipes whole buffer.\n");
			wuser(uid,"         .+      - Next line.\n");
			wuser(uid,"         .-      - Previous line.\n");
			wuser(uid,"         .view   - Output current buffer.\n");
			return;
		}
		wuser(uid,"[^BEDITOR^N] Unknown command!\n");
		return;
	}

	if (strlen(UU.edit)+strlen(inpstr)>=UU.maxedit) {
		wuser(uid,"[^BEDITOR^N] Maximum size reached!\n");
		return;
	}

	lc=1;

	if (UU.eline>1) {
		for (count=0;*str!='\0';count++) {
			lstr[count]=*str;
			lstr[count+1]='\0';
			if (count>0) { // lets not look before str starts
				if ((*str=='\n')&&(*(str-1)=='\n')) {
					lc++;
				}
			}
			str++;
			if (lc==UU.eline) break;
		}
	}

	sprintf(text,"%s%s%s%s%s",lstr,*lstr=='\0'||*str!='\0'?"":"\n\n",inpstr,*str=='\0'?"":"\n\n",str);
	strcpy(UU.edit,text);
	
	sprintf(text,"+ %s\n",inpstr);
	wuser(uid,text);
	if (UU.eline!=999) UU.eline++;
}


