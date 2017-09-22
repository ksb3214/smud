/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :news.c
** Purpose :The simple news system
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 22-11-01 0.00 ksb First Version
*/
// this all needs redesigning
//
#include "smud.h"

extern void line(SU,char *);
extern short cat(SU, char *);
extern void start_edit(SU, short, short, char *);
extern void wuser(SU, char *);


short savenewsitem(SU)
{
	FILE *fp;
	short count;

	if (UU.edit[0]=='\0') {
		wuser(uid,"No point in a blank news item.\n");
		return 0;
	}

	for (count=1;count<999;count++) {
		sprintf(text,"%s%d",NEWSDIR,count);
		if (!(fp=fopen(text,"r"))) {
			/* found it */
			break;
		}
		fclose(fp);
	}

	if (count==999) {
		wuser(uid,"News pool full, delete some files.\n");
		return 0;
	}

	sprintf(text,"%s%d",NEWSDIR,count);
	if (!(fp=fopen(text,"w"))) {
		wuser(uid,"Failed to write new news.\n");
		return 0;
	}

	fprintf(fp,"%s",UU.edit);
	fclose(fp);

	sprintf(text,"Written news item %d.\n",count);
	wuser(uid,text);
	return 0;
}

void newsedit(SU)
{
	wuser(uid,"Creating new news item.\n");
	start_edit(uid,2,4000,NULL);
}

void resetnews(SU)
{
	FILE *fp;
	short count;

	for (count=1;count<999;count++) {
		sprintf(text,"%s%d",NEWSDIR,count);
		if (!(fp=fopen(text,"r"))) {
			/* found it */
			break;
		}
		fclose(fp);
		unlink(text);
	}

	sprintf(text,"Reset news pool.\n");
	wuser(uid,text);
}
	
short newsexist(short num)
{
	FILE *fp;

	DEBUG("newsexist")
	
	sprintf(text,"%s%d",NEWSDIR,num);
	if (!(fp=fopen(text,"r"))) {
		return 0;
	}
	else
	{	
		fclose(fp);
		return 1;
	}
}

void checkreset(SU)
{
	DEBUG("checkreset")
	
	if (!newsexist(UU.lastnews)) {
		UU.lastnews=0; /* news dir has been reset */
	}
}

void incnews(SU)
{
	DEBUG("incnews")
	
	checkreset(uid);
	UU.lastnews++;
}

void newnews(SU)
{
/*	FILE *fp; - Not used yet */

	DEBUG("newnews")
	
	sprintf(text,"%s%d",NEWSDIR,UU.lastnews);
	checkreset(uid);
	if (!newsexist(UU.lastnews+1)) {
		return;
	}

	wuser(uid,"There are ^YNEW^N announcements.  You can read them with \"news\".\n");
}

void news(SU)
{
	short rn=0;
	DEBUG("news")

	if (word_count<1) {
		incnews(uid);
		rn=UU.lastnews;
	}else
	{
		rn=atoi(word[1]);
		if (rn<1) {
			wuser(uid,"Invalid news number.\n");
			return;
		}
	}

	if (!newsexist(rn)) {
		wuser(uid,word_count>0?"There is no such news item.\n":"No news is good news.\n");
		if (word_count==0) UU.lastnews--;
		return;
	}
	sprintf(text,"News item %d",rn);
	line(uid,text);
	sprintf(text,"%s%d",NEWSDIR,rn);
	if (!cat(uid,text)) {
		wuser(uid,"The news system fizzes and pops... try again later.\n");
		return;
	}
	line(uid,NULL);
}
