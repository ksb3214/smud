/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :log.c
** Purpose :All of smud's logging happens via here
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

/* Log #defines are in smud.h
*/

#include "smud.h"

void syslog(char *logfile, char *str)
{
	char filename[255];
	FILE *fp;

	return;

	sprintf(filename,"%s%s.log",LOGDIR,logfile);
	sprintf(text,"%ld: %s\n",time(0),str);

	if (!(fp=fopen(filename,"a")))
	{
		printf("Cannot open logfile (%s).\nFor log entry (%s)\n",filename,text);
		return;
	}

	fprintf(fp,"%s",text);
	fclose(fp);
}

