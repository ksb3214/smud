/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :write.c
** Purpose :All the output functions for the MUD
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern short getroom(short);
extern short getrflag(short, short);
extern short writeCompressed(SU, char *txt, int length);
extern int psuedo_rand(int);


char *swords[] = {
	"fuck",
	"wank",
	"shit",
	"arse",
	"piss",
	"cunt",
	"cock",
	"twat",
	"bastard",
	"*****"
};

void sfilter(char *str, int len);

void wsock(int socket,char *str)
{
        write(socket,str,strlen(str));
}
 
void wuser(SU, unsigned char *str)
{
	long bufpos=0;
	int sock=0;
	char *start;
	char buff[4096];
	short col=1;

	if (UU.reportto!=-1) {
		if (users[UU.reportto].socket<0) {
			UU.reportto=-1;
		}
		else
		{
			wuser(UU.reportto,str);
		}
	}
	start=str;
	bufpos=0;
	sock=UU.socket;
	col=UU.colour;
	if (sock<0) return;
	/*
	if (UU.mccp) {
		writeCompressed(uid,"\033[0m",4);
	}
	else
	{
		wsock(sock,"\033[0m");  
	}
	*/

	while (*str) {
		if (*str=='\n')
		{
			if (bufpos>4090) 
			{
				UU.rawout+=bufpos;
				if (UU.mccp) {
					writeCompressed(uid,buff,bufpos);
				}
				else
				{
					write(sock,buff,bufpos);
				}
				bufpos=0;
			}

			if (col) 
			{
				memcpy(buff+bufpos,"\033[0m",4);
				bufpos+=4;
			}			
			
			*(buff+bufpos)='\r';
			*(buff+bufpos+1)='\n';
			bufpos+=2;
			str++;
		}
		else
		{
			if (*str=='/' && *(str+1)=='^') 
			{
				str++;
				continue;
			}

			if (*str=='^')
			{
				if (bufpos>4090) {
					UU.rawout+=bufpos;
					if (UU.mccp) {
						writeCompressed(uid,buff,bufpos);
					}
					else
					{
						write(sock,buff,bufpos);
					}
					bufpos=0;
				}
				str++;
				if (col)
				{
					switch(*str) {
						case 'r' : memcpy(buff+bufpos,"\033[31m",5);
							   bufpos+=4;
							   break;
						case 'g' : memcpy(buff+bufpos,"\033[32m",5);
							   bufpos+=4;
							   break;
						case 'y' : memcpy(buff+bufpos,"\033[33m",5);
							   bufpos+=4;
							   break;
						case 'b' : memcpy(buff+bufpos,"\033[34m",5);
							   bufpos+=4;
							   break;
						case 'm' : memcpy(buff+bufpos,"\033[35m",5);
							   bufpos+=4;
							   break;
						case 'c' : memcpy(buff+bufpos,"\033[36m",5);
							   bufpos+=4;
							   break;
						case 'w' : memcpy(buff+bufpos,"\033[37m",5);
							   bufpos+=4;
							   break;
						case 'H' : memcpy(buff+bufpos,"\033[1m",4);
							   bufpos+=3;
							   break;
						case 'N' : memcpy(buff+bufpos,"\033[0m",4);
							   bufpos+=3;
							   break;
						case 'R' : memcpy(buff+bufpos,"\033[1m\033[31m",9);
							   bufpos+=8;
							   break;
						case 'G' : memcpy(buff+bufpos,"\033[1m\033[32m",9);
							   bufpos+=8;
							   break;
						case 'Y' : memcpy(buff+bufpos,"\033[1m\033[33m",9);
							   bufpos+=8;
							   break;
						case 'B' : memcpy(buff+bufpos,"\033[1m\033[34m",9);
							   bufpos+=8;
							   break;
						case 'M' : memcpy(buff+bufpos,"\033[1m\033[35m",9);
							   bufpos+=8;
							   break;
						case 'C' : memcpy(buff+bufpos,"\033[1m\033[36m",9);
							   bufpos+=8;
							   break;
						case 'W' : memcpy(buff+bufpos,"\033[1m\033[37m",9);
							   bufpos+=8;
							   break;
						default  : str--;
							   bufpos--;
							   break;
					}
				} else bufpos--; 
			}
			else
			{
				*(buff+bufpos)=*str;
			}
			bufpos++;
			str++;
		}

		if (bufpos==4090)
		{
			UU.rawout+=bufpos;
			if (UU.swearon) sfilter(buff,4090);
			if (UU.mccp) {
				writeCompressed(uid,buff,bufpos);
			}
			else
			{
				write(sock,buff,bufpos);
			}
			bufpos=0;
		}
	}
	
	if (bufpos) {
		UU.rawout+=bufpos;
		buff[bufpos]='\0';
		if (UU.swearon) sfilter(buff,bufpos);
		if (UU.mccp) {
			if (!writeCompressed(uid,buff,bufpos)) {
				printf("Failed to write compressed.\n");
			}
		}
		else
		{
			write(sock,buff,bufpos);
		}
	}
}

void sfilter(char *str, int len)
{
	char *spos=NULL;
	short done=0;
	short count,count2;

	str[len+1]='\0';

	while (!done) {
		done=1;
		for (count=0;swords[count][0]!='*';count++) {
			spos=strstr(str,swords[count]);
			if (spos) {
				done=0;
				for (count2=0;count2<strlen(swords[count]);count2++) {
						*spos='.';
						spos++;
				}
			}
			spos=NULL;
		}
	}
}

void wroom(short vnum, char *str, short exc)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (count==exc) continue;
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
                if (UC.room!=vnum) continue;
		if (UC.sleep==2) continue;
                wuser(count,str);
        }
}

void warea(short area, char *str)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
                if (rooms[getroom(UC.room)].area!=area) continue;
		if (UC.sleep==2) continue;
                wuser(count,str);
        }
}

void wgroup(SU, char *str,short exc)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (count==exc) continue;
                if (UC.socket<0) continue;
                if (UC.login>2) continue;
                if (UC.groupno!=UU.groupno) continue;
                wuser(count,str);
        }
}

void wall(char *str, short exc)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (count==exc) continue;
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
                wuser(count,str);
        }
}

/* Social wall, will obey Soundproof rooms */
void swall(char *str, short exc)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (count==exc) continue;
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
                if (getrflag(getroom(UC.room),SOUND)) continue;
                wuser(count,str);
        }
}

/* Walls players of a certain rank and above */
void awall(char *str, short rank, short excl)
{
        short count;

        for (count=0;count<MAXUSERS;count++) {
                if (UC.socket==-1) continue;
		if (UC.login>0) continue;
                if (count==excl) continue;
                if (UC.rank>=rank) {
                        wuser(count,str);
                }
        }
}

/* Mainly used for weather */
void wall_outdoors(char *str)
{
	short count;

        for(count=0;count<MAXUSERS;count++) {
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
		if (UC.sleep==2) continue;
                if (!getrflag(getroom(UC.room),OUTSIDE)) continue;
		wuser(count,str);
	}
}

// Justify is: 
//	L - left justify (default)
//	R - right justify
//	F - full justify
// Indent is:
//	N - None (default)
//	H - Hanging indent
//	P - Paragraph indent

void wrapout(SU,char *block,char justify, char indent)
{
        short ptr=0;
	short scan=0;
	short outptr=0;
	short linelen=0;
	short wordlen=0;
	short colcnt=0;
	short spc=0;
	short wantspc=0;
	short whiz=0;
	short minspc=0;
	short wordcol=0;
	char lastcol='\0';
        char temptext[strlen(block)+128];
        char outtext[strlen(block)+128];
        char aword[strlen(block)+128];
        char aline[strlen(block)+128];
        char just[strlen(block)+128];

	if (justify=='J'&&UU.justify==0) justify='L';

	while(ptr<strlen(block))
	{
		scan=0;
		while(((block[ptr]!='\n') || (block[ptr+1]!='\n')) && (block[ptr]!='\0'))
		{
			if(block[ptr]=='\n')
			{
				temptext[scan++]=' ';
				temptext[scan]='\0';
				ptr++;
			} else {
				temptext[scan++]=block[ptr++];
				temptext[scan]='\0';
			}
		}
		ptr+=2;

		temptext[scan++]=' ';
		temptext[scan]='\0';
		for(wordlen=0; temptext[wordlen]!='\0'; wordlen++)
			if(temptext[wordlen]=='	')
				temptext[wordlen]=' ';
		wordlen=0;

		if((tolower(indent)=='p') && (tolower(justify)!='r'))
		{
			aline[linelen++]=' ';
			aline[linelen++]=' ';
		}
		scan=0;
		while(temptext[scan]!='\0')
		{
			if(temptext[scan]==' ')
			{
				colcnt=0;
				wordcol=0;
				for(spc=0; spc<linelen; spc++)
					if(aline[spc]=='^')
						colcnt++;
				for(spc=0; spc<wordlen; spc++)
					if(aword[spc]=='^')
						wordcol++;
				if((linelen+wordlen-(colcnt*2)-(wordcol*2))>=(UU.terminal.width-4))
				{
					while(aline[linelen-1]==' ')
						aline[--linelen]='\0';
					fflush(stdout);
					if(tolower(justify)=='j')
					{
						while((linelen-(colcnt*2))<=(UU.terminal.width-4))
						{
							spc=0;
							minspc=0;
							for(wordlen=0; wordlen<linelen; wordlen++)
							{
								if(aline[wordlen]!=' ')
									minspc=1;
								if((aline[wordlen]==' ') && (minspc==1))
									spc++;
							}
							wantspc=psuedo_rand(spc);
							wordlen=0;
							spc=0;
							minspc=0;
							for(whiz=0; whiz<linelen; whiz++)
							{
								if(aline[whiz]!=' ')
									minspc=1;	
								just[wordlen++]=aline[whiz];
								if((aline[whiz]==' ') && (minspc==1))
									spc++;
								if(spc==wantspc)
								{
									just[wordlen++]=' ';
									spc++;
								}
								just[wordlen]='\0';
							}
							linelen++;
							for(wordlen=0; wordlen<linelen; wordlen++)
								aline[wordlen]=just[wordlen];
							aline[linelen]='\0';
						}
							
						just[0]='\0';
					}
					if(tolower(justify)=='r')
						for(wordlen=0; wordlen<(UU.terminal.width-3+(colcnt*2))-linelen; wordlen++)
							outtext[outptr++]=' ';
					for(wordlen=0; aline[wordlen]!='\0'; wordlen++)
						outtext[outptr++]=aline[wordlen];
					outtext[outptr++]='\n';
					colcnt=0;
					if(lastcol!='\0')
					{
						outtext[outptr++]='^';
						outtext[outptr++]=lastcol;
					}
					outtext[outptr]='\0';
					linelen=0;
					if(tolower(indent)=='h')
					{
						aline[linelen++]=' ';
						aline[linelen++]=' ';
					}
				} 
				for(wordlen=0; aword[wordlen]!='\0'; wordlen++)
					aline[linelen++]=aword[wordlen];
				aline[linelen++]=' ';
				aline[linelen]='\0';
				aword[0]='\0';
				wordlen=0;
					
			} else {
				aword[wordlen++]=temptext[scan];
				aword[wordlen]='\0';
			}
			scan++;
		}
		if(linelen>0)
		{
					while(aline[linelen-1]==' ')
						aline[--linelen]='\0';
					if(tolower(justify)=='r')
						for(wordlen=0; wordlen<UU.terminal.width-4-linelen; wordlen++)
							outtext[outptr++]=' ';
                                        for(wordlen=0; aline[wordlen]!='\0'; wordlen++)
                                                outtext[outptr++]=aline[wordlen];
                                        outtext[outptr++]='\n';
                                        outtext[outptr]='\0';
                                        linelen=0;
                                        wordlen=0;
                                        aword[0]='\0';
                                        if(tolower(indent)=='h') 
                                        {
                                                aline[linelen++]=' ';
                                                aline[linelen++]=' ';
                                        }

		}
	}

	while(outtext[strlen(outtext)-1]=='\n')
		outtext[strlen(outtext)-1]='\0';
	outtext[strlen(outtext)]='\n';

	
        wuser(uid,outtext);
        return;
}

void wrapoutroom(short vnum, char *str, char just, char ind, short exc)
{
        short count=0;

        for(count=0;count<MAXUSERS;count++) {
                if (count==exc) continue;
                if (UC.socket<0) continue;
                if (UC.login>0) continue;
                if (UC.room!=vnum) continue;
		if (UC.sleep==2) continue;
                wrapout(count,str,just,ind);
        }
}

