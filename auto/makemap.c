#include <stdio.h>

struct mapst {
	short id;    // 0 - no tag, 1 - room, 2 - link
	short vnum;  // auto incremented
	char mapno; // room label number
};

struct mapst map[1000][1000];

char names[255][255];

void init()
{
	short xx,yy;

	for (xx=0;xx<1000;xx++) {
		for (yy=0;yy<1000;yy++) {
			map[xx][yy].id=0;
		}
	}

	for (xx=0;xx<255;xx++) {
		names[xx][0]='\0';
	}
}

// ALL type 2 +
// north/south type 3 |
// east/west type 4 -
// northeast/southwest type 5 /
// northwest/southeast type 6 \
// 
short hasjunction(short xx, short yy)
{
	short dir;

	for (dir=0;dir<8;dir++) {
		switch (dir) {
			case 0: // north
				if (map[xx][yy-1].id==3||map[xx][yy-1].id==2) {
					if (map[xx][yy-2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 1: // east
				if (map[xx+1][yy].id==4||map[xx][yy-1].id==2) {
					if (map[xx+2][yy].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 2: // south
				if (map[xx][yy+1].id==3||map[xx][yy+1].id==2) {
					if (map[xx][yy+2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 3: // west
				if (map[xx-1][yy].id==4||map[xx-1][yy].id==2) {
					if (map[xx-2][yy].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 4: // northeast
				if (map[xx+1][yy-1].id==5||map[xx+1][yy-1].id==2) {
					if (map[xx+2][yy-2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 5: // northwest
				if (map[xx-1][yy-1].id==6||map[xx-1][yy-1].id==2) {
					if (map[xx-2][yy-2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 6: // southeast
				if (map[xx+1][yy+1].id==6||map[xx+1][yy+1].id==2) {
					if (map[xx+2][yy+2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			case 7: // southwest
				if (map[xx-1][yy+1].id==5||map[xx-1][yy+1].id==2) {
					if (map[xx-2][yy+2].mapno!=map[xx][yy].mapno) return 1;
				}
				break;
			default: //silly
				break;
		}
	}

	return 0;
}

/* Does a description, this is likely to be fairly cheesy but noone reads this shit anyway
 *
 * You are on 'street' which runs 'direction-direction' from here.
 * 'street' also runs direction [and direction] from this point.
 * To the 'direction' you can see another junction.
 *
 * and thats it. for now.
 *
 * this func just printf's output as makemap is designed to be piped.
 */
// ALL type 2 +
// north/south type 3 |
// east/west type 4 -
// northeast/southwest type 5 /
// northwest/southeast type 6 \
// 
void dodesc(short xx, short yy)
{
	char dirstr[255];
	short dir,count;
	short dxx,dyy;
	char *tmpptr;
	
	printf("  desc ");
	dirstr[0]='\0';
	for (dir=0;dir<8;dir++) {
		switch (dir) {
			case 0: // north
				if (map[xx][yy-1].id==3||map[xx][yy-1].id==2) {
					if (map[xx][yy-2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"north-");
					}
				}
				break;
			case 1: // east
				if (map[xx+1][yy].id==4||map[xx+1][yy].id==2) {
					if (map[xx+2][yy].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"east-");
					}
				}
				break;
			case 2: // south
				if (map[xx][yy+1].id==3||map[xx][yy+1].id==2) {
					if (map[xx][yy+2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"south-");
					}
				}
				break;
			case 3: // west
				if (map[xx-1][yy].id==4||map[xx-1][yy].id==2) {
					if (map[xx-2][yy].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"west-");
					}
				}
				break;
			case 4: // northeast
				if (map[xx+1][yy-1].id==5||map[xx+1][yy-1].id==2) {
					if (map[xx+2][yy-2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"northeast-");
					}
				}
				break;
			case 5: // northwest
				if (map[xx-1][yy-1].id==6||map[xx-1][yy-1].id==2) {
					if (map[xx-2][yy-2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"northwest-");
					}
				}
				break;
			case 6: // southeast
				if (map[xx+1][yy+1].id==6||map[xx+1][yy+1].id==2) {
					if (map[xx+2][yy+2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"southeast-");
					}
				}
				break;
			case 7: // southwest
				if (map[xx-1][yy+1].id==5||map[xx-1][yy+1].id==2) {
					if (map[xx-2][yy+2].mapno==map[xx][yy].mapno) {
						strcat(dirstr,"southwest-");
					}
				}
				break;
			default: //silly
				break;
		}
	}

	printf("You are on %s",names[map[xx][yy].mapno]);
	if (dirstr[0]=='\0') {
		printf(".\\n");
	}
	else
	{
		dirstr[strlen(dirstr)-1]='\0';
		printf(" running %s from here.\\n",dirstr);
	}
		
	/* check for a different street running from here, the easiest way has to be
	 * to just check for all known streets in turn and spitting out a message for
	 * any that match
	 */

	for (count=0;count<255;count++) {
		if (names[count][0]=='\0') continue; // skip non existant
		if (count==map[xx][yy].mapno) continue; // lets not do this one

		dirstr[0]='\0';
		for (dir=0;dir<8;dir++) {
			switch (dir) {
				case 0: // north
					if (map[xx][yy-1].id==3||map[xx][yy-1].id==2) {
						if (map[xx][yy-2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"north");
						}
					}
					break;
				case 1: // east
					if (map[xx+1][yy].id==4||map[xx+1][yy].id==2) {
						if (map[xx+2][yy].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"east");
						}
					}
					break;
				case 2: // south
					if (map[xx][yy+1].id==3||map[xx][yy+1].id==2) {
						if (map[xx][yy+2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"south");
						}
					}
					break;
				case 3: // west
					if (map[xx-1][yy].id==4||map[xx-1][yy].id==2) {
						if (map[xx-2][yy].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"west");
						}
					}
					break;
				case 4: // northeast
					if (map[xx+1][yy-1].id==5||map[xx+1][yy-1].id==2) {
						if (map[xx+2][yy-2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"northeast");
						}
					}
					break;
				case 5: // northwest
					if (map[xx-1][yy-1].id==6||map[xx-1][yy-1].id==2) {
						if (map[xx-2][yy-2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"northwest");
						}
					}
					break;
				case 6: // southeast
					if (map[xx+1][yy+1].id==6||map[xx+1][yy+1].id==2) {
						if (map[xx+2][yy+2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"southeast");
						}
					}
					break;
				case 7: // southwest
					if (map[xx-1][yy+1].id==5||map[xx-1][yy+1].id==2) {
						if (map[xx-2][yy+2].mapno==count) {
							if (dirstr[0]!='\0') strcat(dirstr," and ");
							strcat(dirstr,"southwest");
						}
					}
					break;
				default: //silly
					break;
			}
		}
		if (dirstr[0]!='\0') {
			printf("%s is to the %s.\\n",names[count],dirstr);
		}
	}

	// search for close junctions
	dirstr[0]='\0';
	for (dir=0;dir<8;dir++) {
		dxx=xx;
		dyy=yy;
		switch (dir) {
			case 0: // north
				for (count=0;count<2;count++) {
					if (map[dxx][dyy-1].id==3||map[dxx][dyy-1].id==2) {
						if (hasjunction(dxx,dyy-2)) {
							strcat(dirstr,"north,");
							break;
						}
					}
					dyy-=2;
				}
				break;
			case 1: // east
				for (count=0;count<2;count++) {
					if (map[dxx+1][dyy].id==4||map[dxx+1][dyy].id==2) {
						if (hasjunction(dxx+2,dyy)) {
							strcat(dirstr,"east,");
							break;
						}
					}
					dxx+=2;
				}
				break;
			case 2: // south
				for (count=0;count<2;count++) {
					if (map[dxx][dyy+1].id==3||map[dxx][dyy+1].id==2) {
						if (hasjunction(dxx,dyy+2)) {
							strcat(dirstr,"south,");
							break;
						}
					}
					dyy+=2;
				}
				break;
			case 3: // west
				for (count=0;count<2;count++) {
					if (map[dxx-1][dyy].id==4||map[dxx-1][dyy].id==2) {
						if (hasjunction(dxx-2,dyy)) {
							strcat(dirstr,"west,");
							break;
						}
					}
					dxx-=2;
				}
				break;
			case 4: // northeast
				for (count=0;count<2;count++) {
					if (map[dxx+1][dyy-1].id==5||map[dxx+1][dyy-1].id==2) {
						if (hasjunction(dxx+2,dyy-2)) {
							strcat(dirstr,"northeast,");
							break;
						}
					}
					dxx+=2;
					dyy-=2;
				}
				break;
			case 5: // northwest
				for (count=0;count<2;count++) {
					if (map[dxx-1][dyy-1].id==6||map[dxx-1][dyy-1].id==2) {
						if (hasjunction(dxx-2,dyy-2)) {
							strcat(dirstr,"northwest,");
							break;
						}
					}
					dxx-=2;
					dyy-=2;
				}
				break;
			case 6: // southeast
				for (count=0;count<2;count++) {
					if (map[dxx+1][dyy+1].id==6||map[dxx+1][dyy+1].id==2) {
						if (hasjunction(dxx+2,dyy+2)) {
							strcat(dirstr,"southeast,");
							break;
						}
					}
					dxx+=2;
					dyy+=2;
				}
				break;
			case 7: // southwest
				for (count=0;count<2;count++) {
					if (map[dxx-1][dyy+1].id==5||map[dxx-1][dyy+1].id==2) {
						if (hasjunction(dxx-2,dyy+2)) {
							strcat(dirstr,"southwest,");
							break;
						}
					}
					dxx-=2;
					dyy+=2;
				}
				break;
			default: //silly
				break;
		}
	}

	if (dirstr[0]!='\0') {
		dirstr[strlen(dirstr)-1]='\0';
		tmpptr=(char *)strrchr(dirstr,',');
		if (tmpptr) {
			*tmpptr='\0';
			tmpptr++;
		}
		printf("To the %s%s%s you can make out other junctions.\\n",dirstr,tmpptr?" and ":"",tmpptr?tmpptr:"");
	}

	printf("\n");
}

void domap(char *fname, short vnum)
{
	FILE *fp;
	char text[4096];
	short procname=0;
	short xx=0,yy=0;
	short vn=vnum;
	short done=0,dir=0;
	short ele=0;
	short rc=0;

	if (!(fp=fopen(fname,"r"))) {
		printf("No such map file.\n");
		exit(-1);
	}

// north/south type 3 |
// east/west type 4 -
// northeast/southwest type 5 /
// northwest/southeast type 6 \
// 
	while(fgets(text,255,fp)) {
		text[strlen(text)-1]='\0';
		if (text[0]=='%') { procname=1; continue; }
		if (text[0]=='#') break;

		if (!procname) {
			// process map
			for (xx=0;xx<strlen(text);xx++) {
				switch (text[xx]) {
					case '.' : map[xx][yy].id=0; break;
					case '+' : map[xx][yy].id=2; break;
					case 'X' : map[xx][yy].id=2; break;
					case '/' : map[xx][yy].id=5; break;
					case '\\' : map[xx][yy].id=6; break;
					case '-' : map[xx][yy].id=4; break;
					case '|' : map[xx][yy].id=3; break;
					default  : // presume a room
						   // if (text[xx]<'0'||text[xx]>'9') break;
						   map[xx][yy].id=1;
						   map[xx][yy].vnum=vn;
						   vn++;
						   map[xx][yy].mapno=text[xx];
						   //printf("%d\n",map[xx][yy].mapno);
						   break;
				}
			}
			yy++;
		}
		else
		{
			strcpy(names[text[0]],text+2); // yukky
		}
	}
	fclose(fp);
	for (xx=0;xx<100;xx++) {
		for (yy=0;yy<1000;yy++) {
			if (map[xx][yy].id!=1) continue;
			if (names[map[xx][yy].mapno][0]!='=') continue;

			map[xx][yy].vnum=atoi(names[map[xx][yy].mapno]+1);
		}
	}

	for (xx=0;xx<1000;xx++) {
		for (yy=0;yy<1000;yy++) {
			if (map[xx][yy].id!=1) continue;
			if (names[map[xx][yy].mapno][0]=='=') continue;

			// we have a room
			rc++;
			printf("%d {\n",map[xx][yy].vnum);
			printf("  name %s\n",names[map[xx][yy].mapno]);
			dodesc(xx,yy);
			printf("  flags O\n");
			printf("  direction ");
			for (dir=0;dir<8;dir++) {
				switch (dir) {
					case 0: // north
						if (map[xx][yy-1].id==3||map[xx][yy-1].id==2) {
							printf("%d%d\\n",dir,map[xx][yy-2].vnum);
						}
						break;
					case 1: // east
						if (map[xx+1][yy].id==4||map[xx+1][yy].id==2) {
							printf("%d%d\\n",dir,map[xx+2][yy].vnum);
						}
						break;
					case 2: // south
						if (map[xx][yy+1].id==3||map[xx][yy+1].id==2) {
							printf("%d%d\\n",dir,map[xx][yy+2].vnum);
						}
						break;
					case 3: // west
						if (map[xx-1][yy].id==4||map[xx-1][yy].id==2) {
							printf("%d%d\\n",dir,map[xx-2][yy].vnum);
						}
						break;
					case 4: // northeast
						if (map[xx+1][yy-1].id==5||map[xx+1][yy-1].id==2) {
							printf("%d%d\\n",dir,map[xx+2][yy-2].vnum);
						}
						break;
					case 5: // northwest
						if (map[xx-1][yy-1].id==6||map[xx-1][yy-1].id==2) {
							printf("%d%d\\n",dir,map[xx-2][yy-2].vnum);
						}
						break;
					case 6: // southeast
						if (map[xx+1][yy+1].id==6||map[xx+1][yy+1].id==2) {
							printf("%d%d\\n",dir,map[xx+2][yy+2].vnum);
						}
						break;
					case 7: // southwest
						if (map[xx-1][yy+1].id==5||map[xx-1][yy+1].id==2) {
							printf("%d%d\\n",dir,map[xx-2][yy+2].vnum);
						}
						break;
					default: //silly
						break;
				}
			}
			printf("\n}\n\n");
		}
	}
	printf("# %d rooms generated\n",rc);
}

int main(int argc, char *argv[])
{
	if (argc!=3) {
		printf("Usage: makemap <mapfile> <start vnum>\n");
		exit(-1);
	}

	init();
	domap(argv[1],atoi(argv[2]));

	exit(0);
}
