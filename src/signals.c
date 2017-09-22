/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :signal.c
** Purpose :Signal handling
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 23-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern short psave_all(short);
extern void syslog(int, char *);
extern void save_debates();
extern void save_home_objs();
extern void wall(char *, short);

short emergency=0;

#ifdef LINUX

char *signal_names[] = {
	"nothing",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGBUS",
	"SIGFPE",
	"SIGKILL",
	"SIGUSR1",
	"SIGSEGV",
	"SIGUSR2",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGSTKFLT",
	"SIGCHLD",
	"SIGCONT",
	"SIGSTOP",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGURG",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGIO",
	"SIGPWR",
	"SIGSYS"};
#else
char *signal_names[] = {
	"nothing",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGIOT",
	"SIGEMT",
	"SIGFPE",
	"SIGKILL",
	"SIGBUS",
	"SIGSEGV",
	"SIGSYS",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGURG",
	"SIGSTOP",
	"SIGTSTP",
	"SIGCONT",
	"SIGCHLD",
	"SIGTTIN",
	"SIGTTOU",
	"SIGIO",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGVTALRM",
	"SIGPROF",
	"SIGWINCH",
	"SIGINFO",
	"SIGUSR1",
	"SIGUSR2"};
#endif

void do_nothing( int dummy )
{
	printf("%s   \r",signal_names[dummy]);
}

void save_and_quit(int dummy)
{
	short die;
	FILE *fp;
        die=1;
	if(emergency==1)
	{
		printf("***EMERGENCY***\nReceived shutdown signal while shutting down!!!\nKilling now!\n");
		exit(0);
	}
	emergency=1;

        /* save all players online */
        if (!psave_all(1)) {  
                syslog(SYS,"Failed saving players on exit\n");
        }
        save_debates();     
        save_home_objs();    

        if (die) /* shutdown and kill autorun */
        {
                if (!(fp=fopen(".killscript","w"))) {
                        syslog(SYS,"Cannot kill 'autorun'\n");
                }
                else
                {
                        fprintf(fp,"blah");
                        fclose(fp);
                }
        }

        sprintf(text,"Smud is ^rshutting down^N, %s, sorry!\n",die?"and may be down for a while":"but will be backup in about 60 seconds");
        wall(text,-1);
        glob.exitsmud=1;
	emergency=0;
}

void handle(int sig)
{
//	printf("Caught signal %s (%d)\n",signal_names[sig],sig);
	switch(sig)
	{
		case SIGKILL:
		case SIGTERM:
		case SIGSEGV:
		case SIGINT:
		case SIGBUS: save_and_quit(sig);
			     break;
		case SIGCHLD:
			wait();
			break;
		default:
				do_nothing(sig);
				break;
	}
	fflush(stdout);
}

void init_sig()
{
	signal(SIGHUP,&handle);
	signal(SIGINT,&handle);
	signal(SIGQUIT,&handle);
	signal(SIGILL,&handle);
	signal(SIGTRAP,&handle);
	signal(SIGABRT,&handle);
	signal(SIGIOT,&handle);
	signal(SIGBUS,&handle);
	signal(SIGFPE,&handle);
	signal(SIGKILL,&handle);
	signal(SIGUSR1,&handle);
	signal(SIGSEGV,&handle);
	signal(SIGUSR2,&handle);
	signal(SIGPIPE,&handle);
	signal(SIGALRM,&handle);
	signal(SIGTERM,&handle);
#ifdef LINUX
	signal(SIGSTKFLT,&handle);
#endif
	signal(SIGCHLD,&handle);
	signal(SIGCONT,&handle);
	signal(SIGSTOP,&handle);
	signal(SIGTSTP,&handle);
	signal(SIGTTIN,&handle);
	signal(SIGTTOU,&handle);
	signal(SIGURG,&handle);
	signal(SIGXCPU,&handle);
	signal(SIGXFSZ,&handle);
	signal(SIGVTALRM,&handle);
	signal(SIGPROF,&handle);
	signal(SIGWINCH,&handle);
	signal(SIGIO,&handle);
#ifdef LINUX
	signal(SIGPWR,&handle);
#endif
	signal(SIGSYS,&handle);
}

