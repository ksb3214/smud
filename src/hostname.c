/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003,  Karl Bastiman, Janet Hyde
**
** Title   : Hostname.c
** Purpose : does concurrent hostname lookups using threads
**
** Date     Ver  Who Why
** -------------------------------------------------------------
*/

#include <smud.h>

void name_get_func(char *id)
{
	unsigned long addr;
	struct hostent *hp;
	short uid;

	uid=atoi(id);

//	printf("Getting host name for %s...\n",UU.curip);
	if((addr=inet_addr(UU.curip))==-1)
	{
		sprintf(UU.addr,"unknown address");
		return;
	}
	
	hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
	if(hp == (struct hostent *)0)
	{
		sprintf(UU.addr,"unknown address");
		return;
	}
	sprintf(UU.addr,"%s",hp->h_name);
	return;
}

void get_host_name(SU)
{
	pthread_t child;
	char text[128];
	sprintf(text,"%d",uid);
/*	name_get_func(text); */
	pthread_create(&child,NULL,(void *)&name_get_func, text);
	__pthread_detach(child); 
}
