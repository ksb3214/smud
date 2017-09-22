/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :socket.c
** Purpose :All the socket related gubbins
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

unsigned char inpchr;

extern void wsock(int,char *);
extern void activity(SU);
extern void quit1(SU);
extern short cat(SU, char *);
extern void reset_char(SU);
extern short compressStart(SU);
extern void wuser(SU, char *);

void terminate(char *str)
{
	int i;
	for (i=0;i<4096;i++)
	{
		if (*(str+i)<32)
		{
			*(str+i)='\0';
			return;
		}
	}

	str[i-1]='\0';
}


void qquit(SU)
{
	DEBUG("qquit")
	shutdown(UU.socket,2);
	close(UU.socket);
	UU.socket=-1;
	
}

void new_connect()
{
	struct sockaddr_in newsock;
	int len;
	int snew = 0;
/*	int dummy=1; - Not used yet */
/*	char *tmpstr; - Not used yet */
	short count,newb=0;
/*	short count2; - Not used yet */

	DEBUG("new_connect")

	len=sizeof(newsock);
	snew=accept(glob.lsock,(struct sockaddr *)&newsock,&len);

	/* figure out how many newbies we have logging in */
	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket==-1) continue;
		if (UC.login==0) continue;

		newb++;
	}

	if (newb>=MAXLOGINS) {
		wsock(snew,"\r\nThere are a fair few new players logging in at the moment, please try again\r\n");
		wsock(snew,"in a minute, sorry but this is just to avoid congestion.\r\n\r\n");
		close(snew);
		return;
	}

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket<0) {
			reset_char(count);
			UC.socket=snew;
			sprintf(text,"%c%c%c",IAC,DO,TELOPT_TTYPE);
			wsock(snew,text);
			sprintf(text,"%c%c%c",IAC,DO,TELOPT_NAWS);
			wsock(snew,text);
			// Just incase crashing occurs check a possible
			// NULL occurance here?
			strcpy(UC.curip,(char *)inet_ntoa(newsock.sin_addr));
			UC.login=1;
			cat(count,"motd.txt");
//			sprintf(text,"\r\nWelcome to Smud v%s (build %d)\r\n",VER,BUILD);
			sprintf(text,"\r\nWelcome to Smud v%s\r\n",VER);
			wsock(snew,text);
			wsock(snew,"\r\nPlease enter character name:\r\n");
			return;
		}
	}

	wsock(snew,"\r\nSorry maximum connections reached\r\n\r\n");
	close(snew);
}

short get_iac(SU)
{
	unsigned char iac_command[50];
	unsigned char in[2];
	short count,count2,width,height;

//	printf("IAC ");
	if(!read(UU.socket,in,1))
	{
//		printf("e1\n");
		return 0;
	}
//	printf(".");
	iac_command[0]=in[0];
	if(iac_command[0]==255)
	{
//		printf("iac\n");
		return 255;
	}
	if(iac_command[0]==DONT)
	{
		if(!read(UU.socket,in,1))
		{
//			printf("e2\n");
			return 0;
		}
		iac_command[1]=in[0];
//		printf("DONT %d\n",iac_command[1]);
		switch(iac_command[1])
		{
			default:
				sprintf(text,"%c%c%c",IAC,WONT,iac_command[1]);
				wuser(uid,text);
		}
	}
	if(iac_command[0]==DO)
	{
		if(!read(UU.socket,in,1))
		{
//			printf("e3\n");
			return 0;
		}
		iac_command[1]=in[0];
//		printf("DO %d\n",iac_command[1]);
		switch(iac_command[1])
		{
			case TELOPT_ECHO :
				break;
                        case COMPRESS2 :
                                // reset counters for accurate stats
                                UU.rawout=0;
                                UU.compout=0;
                                UU.mccp=2; // client supports mccp v.2
                                if (!compressStart(uid)) {
                                        UU.mccp=0;
                                }
                                break;
                        case COMPRESS1 :
                                // reset counters for accurate stats
                                UU.rawout=0;
                                UU.compout=0;
                                UU.mccp=1; // client supports mccp v.1
                                if (!compressStart(uid)) {
                                        UU.mccp=0;
                                }
                                break;
			default:
				sprintf(text,"%c%c%c",IAC,WONT,iac_command[1]);
				wuser(uid,text);
		}
	}
	if(iac_command[0]==WILL)
	{
		if(!read(UU.socket,in,1))
		{
//			printf("e4\n");
			return 0;
		}
		iac_command[1]=in[0];
//		printf("WILL %d\n",iac_command[1]);
		switch(iac_command[1])
		{
			case TELOPT_TTYPE :
				sprintf(text,"%c%c%c%c%c%c",IAC,SB,TELOPT_TTYPE,1,IAC,SE);
				wuser(uid,text);
				break;
			case TELOPT_NAWS :
				break;
			default:
				break;
//				sprintf(text,"%c%c%c",IAC,DONT,iac_command[1]);
//				wuser(uid,text);
		}
	}
	if(iac_command[0]==SB)
	{
//Sub negotiation
		count=2;
		while(iac_command[count-1]!=SE)
		{
			if(!read(UU.socket,in,1))
			{
//				printf("e5\n");
				return 0;
			}
			iac_command[count++]=in[0];
		}
//		printf("sub negotiaition: %d\n",iac_command[2]);
		switch (iac_command[2])
		{
			case TELOPT_TTYPE :
				count2=0;
				for(count=4; iac_command[count]!=IAC; count++)
				{
					text[count2++]=iac_command[count];
				}
				text[count2]='\0';
//				printf("Terminal type: %s\n",text);
				strcpy(UU.terminal.type,text);
				break;
			case TELOPT_NAWS :
				width=(iac_command[3]*256)+iac_command[4];
				height=(iac_command[5]*256)+iac_command[6];
//				printf("New terminal size: %d x %d\n",width,height);
				UU.terminal.width=width;
				UU.terminal.height=height;
				break;
			default:
				break;
//				printf("Not handled: %d\n",iac_command[2]);
		}
	}	
		
	return 0;
}

short getinp(SU)
{
	long len=0;
	int i=0;
	unsigned char it[2];

	DEBUG("getinp")

	memset(inpstr,0,4095);
	len=read(UU.socket,it,1);

	inpstr[0]=it[0];
	inpstr[1]=it[1];

	// IAC stuff
	// This should all be packaged in a nice switch really
	if (it[0]==IAC) {
		get_iac(uid);
		return 0;
	}

	if (len<1) 
	{
		quit1(uid);
		return 0;
	}

	if ((inpstr[len-1]<=32)&&(!BP)) return 1;

	// We can assume split input here 
	for (i=0;i<len;i++) {
		if (inpstr[i]==8 || inpstr[i]==127) 
		{
			if (BP)
			{	
				BP--;
				continue;
			}
		}
	
		CB[BP]=inpstr[i];

		// this ends a line of input on any old garbage
		if (inpstr[i]<32 || BP+2==4096)
		{
			terminate(CB);
			strcpy(inpstr,CB);
			BP=0;
			CB[0]='\0';
			return 1;
		}

		BP++;
	}

	return 0;
}

void scan()
{
	fd_set fset;
	struct timeval tv;
	short count=0;

	DEBUG("scan")

	FD_ZERO(&fset);
	FD_SET(glob.lsock,&fset);

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket>-1) FD_SET(UC.socket,&fset);
	}

	tv.tv_sec=2;
	tv.tv_usec=0;

	if (select(FD_SETSIZE,&fset,0,0,&tv) == -1) return;

	if (FD_ISSET(glob.lsock,&fset)) new_connect();

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket>-1)
		{
			if (FD_ISSET(UC.socket,&fset))
			{
				activity(count);
			}
		}
	}

}

