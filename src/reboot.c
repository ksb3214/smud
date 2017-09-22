/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : Reboot.c
** Purpose : Smud seemless reboot
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 15-01-02 0.00 ksb First Version
*/

#include "smud.h"

extern void wall(char *, short);
extern short psave_all(short);
extern void offerCompress(SU);
extern short compressEnd(SU);
extern void save_debates();
extern void save_home_objs();
extern void wuser(SU, char *);

// checks to see if the header files have changed since smud was started up
// just checks smud.h atm, but ANY header which contains structures or any
// static datatype needs to be checked
short headerschanged()
{
	struct stat fs;

	stat(SMUDINCLUDE, &fs);

	if (fs.st_mtime>glob.startup) return 1;
	
	return 0;
}

void mud_reboot(SU)
{
	char *args[]={"./smud","R",NULL };
	short count;
	short force=0;
	FILE *fp;

	if (word_count>0) {
		if (word[1][1]=='f') force=1;
		wuser(uid,"^RForcing reboot, this could turn out bad!\n");
	}

	if ((headerschanged())&&(!force)) {
		wuser(uid,"^rHeaders have changed, reboot would be dangerous!\n");
		return;
	}

	wall("^rSMud is rebooting, this should be seamless, do not logoff.^N\n",-1);

	if (!psave_all(0)) {
		wall("^rSome userfiles did not save, aborting reboot.^N\n",-1);
		return;
	}
	save_debates();
	save_home_objs();

	if (!(fp=fopen("reboot/info.dat","w"))) {
		wall("^rFailed to open reboot file...bummer.^N\n",-1);
		return;
	}

	for (count=0;count<MAXUSERS;count++) {
		if (UC.mccp) {
			// stop compression
			compressEnd(count);
			// leave UU.mccp set for detection on reboot
		}
		fwrite((void *) &users[count], sizeof(struct chr),1,fp);
	}
	for (count=0;count<MAXMOBS;count++) {
		fwrite((void *) &mobs[count], sizeof(struct mob),1,fp);
	}
	for (count=0;count<MAXOBJS;count++) {
		fwrite((void *) &objs[count], sizeof(struct obj),1,fp);
	}
	for (count=0;count<MAXROOMS;count++) {
		fwrite((void *) &rooms[count], sizeof(struct rm),1,fp);
	}
	for (count=0;count<MAXGUILDS;count++) {
		fwrite((void *) &guilds[count], sizeof(struct gld),1,fp);
	}
	for (count=0;count<MAXAREAS;count++) {
		fwrite((void *) &areas[count], sizeof(struct areas_st),1,fp);
	}
	for (count=0;count<MAXTRANS;count++) {
		fwrite((void *) &trans[count], sizeof(struct transport_st),1,fp);
	}

	fwrite((void *) &glob, sizeof(struct globals),1,fp);

	fclose(fp);

	execvp("./smud",args);

	wall("^rAhhh, it failed... nevermind.^N\n",-1);
}

void rebootit()
{
/*	char *args[]={"smud","R",NULL }; - not here - in function above */
	short count;
	FILE *fp;

	if (!(fp=fopen("reboot/info.dat","r"))) {
		printf("Cannot open reboot file for reading.\n");
		exit(21);
	}

	for (count=0;count<MAXUSERS;count++) {
		fread((void *) &users[count], sizeof(struct chr),1,fp);
		if (UC.mccp) {
			// re offer compression
			offerCompress(count);
			// set UC.mccp to 0 because we need to trap the
			// return OPT before we compress
			UC.mccp=0;
		}
	}
	for (count=0;count<MAXMOBS;count++) {
		fread((void *) &mobs[count], sizeof(struct mob),1,fp);
	}
	for (count=0;count<MAXOBJS;count++) {
		fread((void *) &objs[count], sizeof(struct obj),1,fp);
	}
	for (count=0;count<MAXROOMS;count++) {
		fread((void *) &rooms[count], sizeof(struct rm),1,fp);
	}
	for (count=0;count<MAXGUILDS;count++) {
		fread((void *) &guilds[count], sizeof(struct gld),1,fp);
	}
	for (count=0;count<MAXAREAS;count++) {
		fread((void *) &areas[count], sizeof(struct areas_st),1,fp);
	}
	for (count=0;count<MAXTRANS;count++) {
		fread((void *) &trans[count], sizeof(struct transport_st),1,fp);
	}

	fread((void *) &glob, sizeof(struct globals),1,fp);

	fclose(fp);

	unlink("reboot/info.dat");

	wall("^gRebooted... now go kill stuff!^N\n",-1);

	glob.sreboots++;
	glob.lastreboot=time(0);
}

