/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   : Mud mail system
** Purpose : Provide facilities for mailing messages and objects
**           to other players.
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include <smud.h>

extern void wuser(SU,char *);
extern short getuser(short,char *);
extern short isoffline(char *);
extern short getmflag(short, char);
extern short getheldobj(short,char *);
extern void start_edit(short,short,short,char *);
extern void stolower(char *);
extern void line(short, char *);
extern void wrapout(short, char *,char,char);
extern void read_obj(short);
extern void reset_obj(short);
extern void wroom(short,char *,short);


/*
	Comments and Ideas
	==================

Mailing letters just costs a standard fee (is this needed with the pager?)

Mailing objects incurs a fee depending on the weight of the object.

More valuable objects should have some form of insurance payment?

Have mail and items delivered to the players home room?

Object type 21 = post box

Mob flag "p" = postmaster

Letters can be posted into 21 or sent with "p"

Parcels can only be sent with "p"

*/

void mail_list(SU)
{
	short showall=0;
	char text[1024];
	char pad[1024];
	
	smud_file *mailfile;
	short count,count2;
	if((!strcasecmp(word[2],"all")) && (word_count==2))
		showall=1;
	if(showall)
	{
		line(uid,"Your Mail");
	} else {
		line(uid,"Your New Mail");
	}

	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	mailfile=read_file(text);
	for(count=0; count<mailfile->vnumcount; count++)
	{
		if((!strcmp(getval(mailfile,mailfile->vnumlist[count],"status"),"N")) || (showall==1))
		{
			sprintf(pad,"%s",getval(mailfile,mailfile->vnumlist[count],"subject"));
			pad[UU.terminal.width-59]='\0';
			while(strlen(pad)<UU.terminal.width-59)
			{
				count2=strlen(pad);
				pad[count2]=' ';
				pad[count2+1]='\0';
			}
			sprintf(text,"^G%2s^N %6d %-15s %s %-30s\n",
				getval(mailfile,mailfile->vnumlist[count],"status"),
				mailfile->vnumlist[count],
				getval(mailfile,mailfile->vnumlist[count],"from"),
				pad,
				getval(mailfile,mailfile->vnumlist[count],"sent"));
			wuser(uid,text);
		}
	}
	line(uid,NULL);
	freefile(mailfile);
}

void mail_undelete(SU)
{
        smud_file *mailfile;
        short count;
        char text[128];
        if(word_count<2)
        {
                wuser(uid,"Usage: mail delete [number]\n");
                return;
        }
        sprintf(text,"../mail/%s.mff",UU.name);
        stolower(text);
        mailfile=read_file(text);
        for(count=0; count<mailfile->vnumcount; count++)
        {
                if(atoi(word[2])==mailfile->vnumlist[count])
                {
                        add_entry(mailfile,mailfile->vnumlist[count],"status","");
                        sprintf(text,"../mail/%s.mff",UU.name);
                        stolower(text);
                        write_file(mailfile,text);
                        freefile(mailfile);
                        sprintf(text,"Message %d unmarked for deletion.\n",atoi(word[2]));
                        wuser(uid,text);
                        return;
                }
        }
        freefile(mailfile);
        wuser(uid,"No such mail message.\n");
}

void mail_delete(SU)
{
        smud_file *mailfile;
        short count;
        char text[128];
        if(word_count<2)
        {
                wuser(uid,"Usage: mail delete [number]\n");
                return;
        }
        sprintf(text,"../mail/%s.mff",UU.name);
        stolower(text);
        mailfile=read_file(text);
        for(count=0; count<mailfile->vnumcount; count++)
        {
                if(atoi(word[2])==mailfile->vnumlist[count])
                {
			add_entry(mailfile,mailfile->vnumlist[count],"status","D");
			sprintf(text,"../mail/%s.mff",UU.name);
			stolower(text);
			write_file(mailfile,text);
                        freefile(mailfile);
			sprintf(text,"Message %d marked for deletion.\n",atoi(word[2]));
			wuser(uid,text);
                        return;
                }
        }
        freefile(mailfile);
        wuser(uid,"No such mail message.\n");
}

void mail_purge(SU)
{
	smud_file *mailfile,*cleanfile;
	short count;
	short count2=1;
	char text[128];

	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	mailfile=read_file(text);
	cleanfile=new_file();
	
	for(count=0; count<mailfile->vnumcount; count++)
		if(strcmp("D",getval(mailfile,mailfile->vnumlist[count],"status"))!=0)
		{
			add_entry(cleanfile,count2,"from",getval(mailfile,mailfile->vnumlist[count],"from"));
			add_entry(cleanfile,count2,"sent",getval(mailfile,mailfile->vnumlist[count],"sent"));
			add_entry(cleanfile,count2,"status",getval(mailfile,mailfile->vnumlist[count],"status"));
			add_entry(cleanfile,count2,"body",getval(mailfile,mailfile->vnumlist[count],"body"));
			add_entry(cleanfile,count2,"subject",getval(mailfile,mailfile->vnumlist[count],"subject"));
			count2++;
		}
	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	write_file(cleanfile,text);
	freefile(mailfile);
	freefile(cleanfile);
	wuser(uid,"Deleted messages purged from mailbox.\n");
}

void mail_read(SU)
{
	smud_file *mailfile;
	short count;
	short id=-1;
	char text[1024];
	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	mailfile=read_file(text);
	if(word_count<2)
	{
		for(count=0; count<mailfile->vnumcount; count++)
		{
			if((!strcmp(getval(mailfile,mailfile->vnumlist[count],"status"),"N")) && (id==-1))
			{	
				id=mailfile->vnumlist[count];
			}
		}
	} else {
		id=atoi(word[2]);
	}
	if(id==-1)
	{
		wuser(uid,"No new mail.  Try 'mail read [number]'\n");
		return;
	}
	for(count=0; count<mailfile->vnumcount; count++)
	{
		if(id==mailfile->vnumlist[count])
		{
			line(uid,NULL);
			sprintf(text," ^GFrom: ^N%s\n ^GSent: ^N%s\n ^GSubject: ^N%s\n",
				getval(mailfile,mailfile->vnumlist[count],"from"),
				getval(mailfile,mailfile->vnumlist[count],"sent"),
				getval(mailfile,mailfile->vnumlist[count],"subject"));
			wuser(uid,text);
			line(uid,NULL);
			wrapout(uid,getval(mailfile,mailfile->vnumlist[count],"body"),'J','P');
			line(uid,NULL);
			if(!strcasecmp(getval(mailfile,mailfile->vnumlist[count],"status"),"N"))
			{
				add_entry(mailfile,mailfile->vnumlist[count],"status","");
				sprintf(text,"../mail/%s.mff",UU.name);
				stolower(text);
				write_file(mailfile,text);
			}
			freefile(mailfile);
			return;
		}
	}
	freefile(mailfile);
	wuser(uid,"No such mail message.\n");
}
	
void finishmail(SU)
{
	smud_file *mailfile;
	char text[1024];
	short count;
	short spare=0;
	time_t t;
	struct tm *now;
	short target;
	
	wuser(uid,"Mail finished\n");
	sprintf(text,"../mail/%s.mff",UU.maildest);
	mailfile=read_file(text);
	for(count=0; count<mailfile->vnumcount; count++)
		if(mailfile->vnumlist[count]>spare)
			spare=mailfile->vnumlist[count];

	spare++;
	t = time(NULL);
	now = gmtime(&t);
	add_entry(mailfile,spare,"from",UU.name);
	strftime(text,128,"%a %B %d, %I:%M%p",now);
	add_entry(mailfile,spare,"sent",text);
	add_entry(mailfile,spare,"subject",UU.mailsubject);
	add_entry(mailfile,spare,"status","N");
	add_entry(mailfile,spare,"body",UU.edit);
	sprintf(text,"../mail/%s.mff",UU.maildest);
	write_file(mailfile,text);
	freefile(mailfile);
        target=getuser(uid,UU.maildest);
        
        if (target==-1) {
                target=isoffline(UU.maildest);
        }
	if(target>-1&&target!=MAXUSERS)
	{
		//wroom(users[target].homeroom,"You hear the letterbox clatter as some post arrives.\n",uid);
		if (users[target].room!=users[target].homeroom) {
			wuser(target,"^gYou recieve word of ^Ynew mail^N^g waiting at home.\n");
		}
		else
		{
			wuser(target,"^gSome ^Ynew mail^N^g is delivered.\n");
		}
	}
	bzero(UU.maildest,15);
	bzero(UU.mailsubject,255);
}

// command format "post <item> to <player>"
// where item is either "letter" or a string that can be identified as an
// object the player is carying.

void post(SU)
{
	short count;
	short cansend=0;
	short obj;
	short target;
	float cost;
	char destination[20];
	
	if(UU.fight>-1) {
		wuser(uid,"You cannot enter the editor whilst fighting!\n");
		return;
	}
	if(word_count<3)
	{
		wuser(uid,"Usage: post [letter/<item>] to [player]\n");
		return;
	}

	for(count=1; count<word_count; count++)
	{
		if(!strcasecmp("to",word[count]))
		{
			strcpy(destination,word[count+1]);
		}
		fflush(stdout);
	}
	
	if(destination[0]=='\0')
	{
		wuser(uid,"Usage: post [letter/<item>] to [player]\n");
		return;
	}

        target=getuser(uid,destination);

        if (target==-1) {
                target=isoffline(destination);
        }

	if(target==-1)
	{
		wuser(uid,"You sending post to Santa again or something?\n");
		return;
	}

	if(users[target].homeroom==-1)
	{
		sprintf(text,"Sending mail addressed to %s at no fixed abode? I think not.\n",users[target].name);
		wuser(uid,text);
		return;
	}

	if(!strcasecmp(word[1],"letter")) //sending a simple letter
	{
		cansend=0;
		for(count=0; count<MAXOBJS; count++)
			if((objs[count].room==UU.room) && (objs[count].type==POSTBOX))
				cansend=1;
		if(cansend==0)
			for(count=0; count<MAXMOBS; count++)
				if(getmflag(count,mPOSTMASTER))
					if(mobs[count].room==UU.room)
						cansend=1;

		if (UU.room==UU.homeroom) cansend=1;

		if(cansend==0)
		{
			wuser(uid,"I should find somewhere to post your letter first if I were you.\n");
			return;
		}

		if(UU.cash<LETTER_COST)
		{
			wuser(uid,"You cannot afford to send any letters at the moment.\n");
			return;
		}

		strcpy(UU.maildest,users[target].name);
		stolower(UU.maildest);
	} else {	//sending a parcel
		for(count=0; count<MAXMOBS; count++)
			if(getmflag(count,mPOSTMASTER))
				if(mobs[count].room==UU.room)
					cansend=1;
		if(cansend==0)
                {
                        wuser(uid,"I should find somewhere to post your parcel first if I were you.\n");
                        return;
                }
		text[0]='\0';
		count=1;
		while((strcasecmp(word[count],"to")!=0) && (count<word_count))
		{
			strcat(text,word[count]);
			strcat(text," ");
			count++;
		}
		if(count==word_count)
		{
			wuser(uid,"Usage: post [letter/<item>] to [player]\n");
			return;
		}
		while(text[strlen(text)-1]==' ')
			text[strlen(text)-1]='\0';
		obj=getheldobj(uid,text);
		if(obj==-1)
		{
			wuser(uid,"You don't have one of those to send.\n");
			return;
		}
		obj=UU.held[obj];

		cost=objs[obj].weight * WEIGHT_MULT;

		if(cost>UU.cash)
		{
			wuser(uid,"You can't afford to send that anywhere!\n");
			return;
		}
		UU.cash-=cost;

		read_obj(PARCEL_VNUM);
		CO.room=users[target].homeroom;
		sprintf(CO.other,"%d",objs[obj].vnum);
		CO.age=objs[obj].age;
		CO.charges=objs[obj].charges;
		CO.lightage=objs[obj].lightage;
		sprintf(CO.writing,"With love from %s.",UU.name);
		CO.language=81;
		reset_obj(obj);
		wuser(uid,"Item sent.\n");
		wroom(CO.room,"The postman drops of a parcel.\n",uid);
		return;
	}
}

void unwrap(SU)
{
	short count;
	char text[128];

	if(UU.room!=UU.homeroom)
	{
		wuser(uid,"You should go home to unwrap your parcels.\n");
		return;
	}

	for(count=0; count<MAXOBJS; count++)
		if((objs[count].vnum==PARCEL_VNUM) && (objs[count].room==UU.room))
		{
			read_obj(atoi(objs[count].other));
			CO.room=UU.room;
			CO.age=objs[count].age;
			CO.charges=objs[count].charges;
			CO.lightage=objs[count].lightage;

			sprintf(text,"You unwrap the parcel and find %s\n",CO.sdesc);
			wuser(uid,text);
			reset_obj(count);
			return;
		}
	wuser(uid,"There is nothing here to unwrap!\n");
}

void mail_subject(SU)
{
	bzero(UU.mailsubject,255);
	strncpy(UU.mailsubject,inpstr,254);
	start_edit(uid,5,1023,"");
	return;
}

short checkmail(SU)
{
	smud_file *mailfile;
	char text[128];
	short count;
	short new=0;

	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	mailfile=read_file(text);
	for(count=0; count<mailfile->vnumcount; count++)
		if(!strcmp(getval(mailfile,mailfile->vnumlist[count],"status"),"N"))
			new++;
	freefile(mailfile);
	return new;
}

short countmail(SU)
{
	smud_file *mailfile;
	char text[128];
	short new=0;

	sprintf(text,"../mail/%s.mff",UU.name);
	stolower(text);
	mailfile=read_file(text);
	new=mailfile->vnumcount;
	freefile(mailfile);
	return new;
}

void mail(SU)
{
	if(word_count<1)
	{
		wuser(uid,"See the mail help page for information on how to use mail.\n");	
		return;
	}
	if(UU.room!=UU.homeroom)
	{
		wuser(uid,"How are you supposed to do that when your mail is back at home?\n");
		return;
	}
	if(!strcasecmp(word[1],"list")) { mail_list(uid); return; }
	if(!strcasecmp(word[1],"read")) { mail_read(uid); return; }
	if(!strcasecmp(word[1],"delete")) { mail_delete(uid); return; }
	if(!strcasecmp(word[1],"undelete")) { mail_undelete(uid); return; }
	if(!strcasecmp(word[1],"purge")) { mail_purge(uid); return; }
	wuser(uid,"See the mail help page for information on how to use mail.\n");	
	return;
}	
