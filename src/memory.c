/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   :
** Purpose :
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

#ifdef FREEBSD
long memory_usage()
{
	FILE *proc;
	int pid;
	char *work = malloc(128);
	int low,high;
	long total = 0;

	pid=getpid();
	sprintf(work,"/proc/%d/map",pid);
	proc = fopen(work,"r");
	if(!proc)
	{
		fprintf(stderr,"Error: cannot open %s\n",work);
		free(work);
		return 0;
	}

	while(fgets(work,120,proc))
	{
		if(sscanf(work,"0x%x 0x%x",&low,&high)==2)
		{
			total=total+(high-low);
		}
	}
	fclose(proc);
	free(work);
	return total;
}
#endif

#ifdef LINUX
long memory_usage()
{
        FILE *proc;
        int pid;
        char *work = malloc(128);
        unsigned int low,high;
        long total = 0;

        pid=getpid();
        sprintf(work,"/proc/%d/maps",pid);
        proc = fopen(work,"r");
        if(!proc)
        {
                fprintf(stderr,"Error: cannot open %s\n",work);
                free(work);
                return 0;
        }

        while(fgets(work,120,proc))
        {
                if(sscanf(work,"%x-%x",&low,&high)==2)
                {
                        total=total+(high-low);
                }
        }
        fclose(proc);
        free(work);
        return total;
}
#endif
