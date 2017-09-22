/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003,  Karl Bastiman, Janet Hyde
**
** Title   : debate.c
** Purpose : smud debate syste,
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/
#include <smud.h>

extern void wuser(SU, char *);
extern void start_edit(SU, short, short, char *);
extern void line(SU,char *);
extern void wrapout(SU,char *,char,char);

int got_new_debates(SU)
{
	short count,count2;
	short found=0;
	if(UU.lvl<2) return 0;
	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].status=='A')
		{
			found=0;
			for(count2=0; count2<ACTIVEDEBATES; count2++)
			{
				if(glob.debates[count].vnum==UU.voted[count2])
					found=1;
			}
			if(found==0)
			{
				return 1;
			}
		}
	}
	return 0;
}

void read_debates()
{
	smud_file *debatefile;
	short count;
	char text[2];
	
	debatefile = read_file("../other/debates.mff");
	if(debatefile->vnumcount==0)
	{
		fprintf(stderr,"Error: cannot open debates.\n");
		freefile(debatefile);
		return;
	}
	for(count=0; count<debatefile->vnumcount; count++)
	{
		glob.debates[count].vnum=debatefile->vnumlist[count];
		sprintf(glob.debates[count].subject,"%s",getval(debatefile,debatefile->vnumlist[count],"subject"));
		sprintf(glob.debates[count].body,"%s",getval(debatefile,debatefile->vnumlist[count],"body"));
		sprintf(text,"%s",getval(debatefile,debatefile->vnumlist[count],"status"));
		glob.debates[count].status=text[0];
		glob.debates[count].yes=atoi(getval(debatefile,debatefile->vnumlist[count],"yes"));
		glob.debates[count].no=atoi(getval(debatefile,debatefile->vnumlist[count],"abstain"));
		glob.debates[count].abstain=atoi(getval(debatefile,debatefile->vnumlist[count],"abstain"));
	}
	freefile(debatefile);
}

void editdebatequit(SU)
{
	short count;
	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].vnum==UU.mydebate)
		{
			glob.debates[count].vnum=-1;
			glob.debates[count].status='\0';
		}
	}
	UU.mydebate=-1;
	wuser(uid,"Debate creation aborted.\n");
	return;
}

void editdebatesave(SU)
{
	short count;
	if(UU.edit[0]=='\0')
	{
		wuser(uid,"Sorry, you can't have an empty debate.  Aborted.\n");
		editdebatequit(uid);
		return;
	}
	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].vnum==UU.mydebate)
		{
			glob.debates[count].status='A';
			strcpy(glob.debates[count].body,UU.edit);
			glob.debates[count].body[strlen(glob.debates[count].body)-1]='\0';
		}
	}
	UU.mydebate=-1;
	wuser(uid,"Debate created and marked active.\n");
	return;
}

void debate_add(SU)
{
	short topvnum=0;
	short count;
	short slot=-1;
	short active=0;
	char text[1024];

	if(word_count<2)
	{
		wuser(uid,"Usage: debate add [subject]\n");	
		return;
	}
	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].vnum>topvnum)
			topvnum=glob.debates[count].vnum;
		if((glob.debates[count].vnum==-1) && (slot==-1))
			slot=count;
		if(glob.debates[count].status=='A')
			active++;
	}

	if(active>=ACTIVEDEBATES)
	{
		wuser(uid,"Error: Too many active debates open.  Close a debate first.\n");
		return;
	}

	if(slot==-1)
	{
		wuser(uid,"Error: no spare debate slots.\n");
		return;
	}
	topvnum++;

	sprintf(text,"Vnum would be: %d\n",topvnum);
	wuser(uid,text);
	UU.mydebate=topvnum;
	glob.debates[slot].vnum=topvnum;
	glob.debates[slot].status='N';
	bzero(glob.debates[slot].subject,80);
	strncpy(glob.debates[slot].subject,strchr(comstr,' ')+1,79);
	sprintf(text,"Subject: %s\n",glob.debates[slot].subject);
	bzero(glob.debates[slot].body,4096);
	start_edit(uid,4,4095,glob.debates[slot].body);
	wuser(uid,text);
}

void debate_list(SU)
{
	short showall=0;
	short count;
	char text[80];
	short count2=0;

	if(word_count==2)
	{
		if(!strcmp(word[2],"all"))
		showall=1;
	}

	if(showall==1)
	{
		line(uid,"All Debates");
	} else {
		line(uid,"Active Debates");
	}

	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].status=='A')
		{
			sprintf(text,"  %6d %s\n",glob.debates[count].vnum,glob.debates[count].subject);
			wuser(uid,text);
			count2++;
		}
		if((glob.debates[count].status=='C') && (showall==1))
		{
			sprintf(text,"C %6d %s\n",glob.debates[count].vnum,glob.debates[count].subject);
			wuser(uid,text);
			count2++;
		}
	}
	sprintf(text,"%d debates listed",count2);
	if(count2==0)
		sprintf(text,"No debates to list");
	if(count2==1)
		sprintf(text,"Only one debate listed");
	line(uid,text);
	return;
}

void debate_show(SU)
{
	short deb=-1;
	char text[80];
	short count;

	if(word_count<2)
	{
		wuser(uid,"Usage: debate show [number]\n");
		return;
	}

	for(count=0; count<MAXDEBATES; count++)
		if(glob.debates[count].vnum==atoi(word[2]))
			deb=count;

	if(deb==-1)
	{
		wuser(uid,"No such debate.\n");
		return;
	}
	line(uid,glob.debates[deb].subject);
	wrapout(uid,glob.debates[deb].body,'J','P');
//	wuser(uid,"\n");
	if(glob.debates[deb].status=='C')
		line(uid,"*** THIS DEBATE IS CLOSED ***");
	if((glob.debates[deb].status!='C') && (UU.immstatus==1))
		line(uid,"Current vote information");
	if((glob.debates[deb].status=='C') || (UU.immstatus==1))
	{
		sprintf(text,"       Yes: %6d     No: %6d     Abstain: %6d\n",
			glob.debates[deb].yes,
			glob.debates[deb].no,
			glob.debates[deb].abstain);
		wuser(uid,text);
	}
	line(uid,NULL);
	return;
}

void debate_close(uid)
{
	short count;
	short deb = -1;

	if(word_count<2)
	{
		wuser(uid,"Usage: debate close [number]\n");
		return;
	}
	for(count=0; count<MAXDEBATES; count++)
		if(glob.debates[count].vnum==atoi(word[2]))
			deb=count;
	if(deb==-1)
	{
		wuser(uid,"No such debate.\n");
		return;
	}
	if(glob.debates[deb].status!='A')
	{
		wuser(uid,"That debate isn't active.\n");
		return;
	}
	glob.debates[deb].status='C';
	wuser(uid,"Debate closed.  Users can now see the results.\n");
	return;
}

void debate_vote(SU)
{
	short count,count2;
	short deb=-1;

	if(UU.lvl<2)
	{
		wuser(uid,"Sorry, you are not yet old enough to vote.\n");
		return;
	}
	if(word_count<3)
	{
		wuser(uid,"Usage: debate vote [number] [yes/no/abstain]\n");
		return;
	}

	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].status=='C')
		{
// Clear out any old votes for this debate
			for(count2=0; count2<ACTIVEDEBATES; count2++)
			{
				if(users[uid].voted[count2]==glob.debates[count].vnum)
				{	
					users[uid].voted[count2]=-1;
				}
			}
		}
		if(glob.debates[count].vnum==atoi(word[2]))
			deb=count;
	}
	if(deb==-1)
	{
		wuser(uid,"No such debate.\n");
		return;
	}
	if(glob.debates[deb].status=='C')
	{
		wuser(uid,"That debate is closed!\n");
		return;
	}
	for(count=0; count<ACTIVEDEBATES; count++)
	{
		if(users[uid].voted[count]==glob.debates[deb].vnum)
		{
			wuser(uid,"You have already voiced your opinion on that subject.\n");
			return;
		}
	}
	count2=-1;
	for(count=0; count<ACTIVEDEBATES; count++)
	{
		if((users[uid].voted[count]==-1) && (count2==-1))
			count2=count;
	}
	if(count2==-1)	// This should never happen...
	{
		wuser(uid,"That'd odd... I can't seem to find a free slot for your vote.\n");
		wuser(uid,"There seem to be more active votes that there is room for.\n");
		wuser(uid,"Please report this to an IMM immediately.\n");
		return;
	}
	users[uid].voted[count2]=glob.debates[deb].vnum;
	if(!strncasecmp("yes",word[3],strlen(word[3]))) glob.debates[deb].yes++;
	if(!strncasecmp("no",word[3],strlen(word[3]))) glob.debates[deb].no++;
	if(!strncasecmp("abstain",word[3],strlen(word[3]))) glob.debates[deb].abstain++;
	wuser(uid,"Thank you - your opinion is most highly valued.\n");
	return;
}

void save_debates()
{
	smud_file *debatefile;
	short count;
	char text[10];
	debatefile = new_file();
	for(count=0; count<MAXDEBATES; count++)
	{
		if(glob.debates[count].vnum>-1)
		{
			add_entry(debatefile,glob.debates[count].vnum,"subject",glob.debates[count].subject);
			add_entry(debatefile,glob.debates[count].vnum,"body",glob.debates[count].body);
			sprintf(text,"%d",glob.debates[count].yes);
			add_entry(debatefile,glob.debates[count].vnum,"yes",text);
			sprintf(text,"%d",glob.debates[count].no);
			add_entry(debatefile,glob.debates[count].vnum,"no",text);
			sprintf(text,"%d",glob.debates[count].abstain);
			add_entry(debatefile,glob.debates[count].vnum,"abstain",text);
			sprintf(text,"%c",glob.debates[count].status);
			add_entry(debatefile,glob.debates[count].vnum,"status",text);
		}
	}
	write_file(debatefile,"../other/debates.mff");
	freefile(debatefile);
	return;
}

void cdebate(SU)
{
	if(word_count>=1)
	{
		if(!strcasecmp(word[1],"add") && users[uid].immstatus==1)
		{
			debate_add(uid);
			return;
		}
		if(!strcasecmp(word[1],"vote"))
		{
			debate_vote(uid);
			return;
		}
		if(!strcasecmp(word[1],"close") && users[uid].immstatus==1)
		{
			debate_close(uid);
			return;
		}
		if(!strcasecmp(word[1],"list"))
		{
			debate_list(uid);
			return;
		}
		if(!strcasecmp(word[1],"show"))
		{
			debate_show(uid);
			return;
		}
	}
	if(users[uid].immstatus==1)
	{
		wuser(uid,"Usage: debate [add/vote/close/list/show]\n");
	} else {
		wuser(uid,"Usage: debate [vote/list/show]\n");
	}
	return;
}
