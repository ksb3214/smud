#include <stdio.h>
#include <stdlib.h>

struct mobst {
	char desc[4096]; // generic description
	char flags[255]; // generic flags
	char worn[4096]; // all wearing
	short wield[51]; // each level wields?
	short slevel; // starting level
	short nlevel; // random levels beyond slevel
	short sroom;  // starting room
	short nroom;  // rooms beyond that
	char names[1000][128]; // all the names to process
};

struct mobst mobs;

int psuedo_rand(int max)
{
        return (1+(int) ((double)max*rand()/(RAND_MAX+1.0)));
}

void init()
{
	short count;

	srand(time(0));
	for (count=0;count<1000;count++) {
		mobs.names[count][0]='\0';
	}

	for (count=0;count<51;count++) {
		mobs.wield[count]=-1;
	}
}

void domobs(char *fname,short vnumstart)
{
	FILE *fp;
	short num=0;
	short names=0;
	short vnum=vnumstart;
	short count,count2,level;
	char text[4096];

	if (!(fp=fopen(fname,"r"))) {
		fprintf(stderr,"Bad config file name.\n");
		exit(-1);
	}

	while (fgets(text,255,fp)) {
		if (text[0]=='#') break;
		text[strlen(text)-1]='\0';

		if (strncmp(text,"desc",4)==0) {
			strcpy(mobs.desc,strchr(text,':')+1);
		}
		if (strncmp(text,"sroom",5)==0) {
			mobs.sroom=atoi(strchr(text,':')+1);
		}
		if (strncmp(text,"nroom",5)==0) {
			mobs.nroom=atoi(strchr(text,':')+1);
		}
		if (strncmp(text,"nlevel",6)==0) {
			mobs.nlevel=atoi(strchr(text,':')+1);
		}
		if (strncmp(text,"slevel",6)==0) {
			mobs.slevel=atoi(strchr(text,':')+1);
		}
		if (strncmp(text,"flags",5)==0) {
			strcpy(mobs.flags,strchr(text,':')+1);
		}
		if (strncmp(text,"worn",4)==0) {
			strcpy(mobs.worn,strchr(text,':')+1);
		}
		if (strncmp(text,"wield",5)==0) {
			num=atoi(text+5);
			mobs.wield[num]=atoi(strchr(text,':')+1);
		}
		if (strncmp(text,"name",4)==0) {
			strcpy(mobs.names[names],strchr(text,':')+1);
			names++;
		}
	}
	fclose(fp);

	// make sure each level has a weapon
	num=-1;
	for (count=0;count<51;count++) {
		if (mobs.wield[count]==-1) {
			mobs.wield[count]=num;
		}
		else
		{
			num=mobs.wield[count];
		}
	}

	/*
	// get rid of duplicate names
	for (count=0;count<1000;count++) {
		for (count2=0;count2<1000;count2++) {
			if (count==count2) continue;
			if (strcasecmp(mobs.names[count],mobs.names[count2])==0)
			{
				// duplicate
				mobs.names[count2][0]='\0';
			}
		}
	}
	*/

	printf("# Auto generated with makemob\n\n");
	for (count=0;count<1000;count++) {
		if (mobs.names[count][0]=='\0') continue;
		// got mob name
		printf("%d {\n",vnum);
		printf("  name %s\n",mobs.names[count]);
		printf("  shortdesc %s\n",mobs.names[count]);
		level=mobs.slevel+(psuedo_rand(mobs.nlevel)-1);
		printf("  objects %s",mobs.worn);
		if (mobs.wield[level]!=-1) {
			printf("\\n%d\n",mobs.wield[level]);
		}
		else
		{
			printf("\n");
		}
		level+=(int)(level/2);
		printf("  level %d\n",level);
		printf("  home %d\n",mobs.sroom+(psuedo_rand(mobs.nroom)-1));
		printf("  longdesc %s\n",mobs.desc);
		printf("  sex N\n");
		printf("  alignment 0\n");
		printf("}\n\n");
		vnum++;
	}	
}

int main(int argc, char *argv[])
{
	if (argc!=3) {
		printf("Usage: makemob <mobfile> <start vnum>\n");
		exit(-1);
	}

	init();
	domobs(argv[1],atoi(argv[2]));

	exit(0);
}
