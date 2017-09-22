/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :objs.c
** Purpose :Everything to do with objects
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short, long);
extern char *getsex(SU,char *, char *, char *);
extern short getuser(SU,char *);
extern unsigned long from_fund(unsigned long);
extern int psuedo_rand(int);
extern short getoflag(short, short);
extern void reset_obj(short);
extern void wuser(SU, char *);
extern void wroom(short, char *, short);
extern char *cap1st(char *);
extern void stolower(char *);
short getheldobj(SU,char *str);
void multiroom(short startvnum, short depth, void *(*task)(short vnum));
void wall(char *,short);
short getguild(short);
short next_victim(short);

short bombid; //used to identify the bomb to 'boom()'

void boom(short vnum)
{
	short count,dam,count2,fght;

	wroom(vnum,"An ^RExplosion^N rocks the room!\n",-1);

	for (count=0;count<MAXUSERS;count++)
	{
		if (UC.socket<0) continue;
		if (UC.room!=vnum) continue;

		dam=psuedo_rand(objs[bombid].yield);
		//if (!(UC.rank>NORM&&UC.immstatus)) {
			UC.hp-=dam;
		//}
		sprintf(text,"The explosion cost you %d health.\n",dam);
		wuser(count,text);
		if (UC.hp<1) {
			wuser(count,"The explosion ^RKILLED^N you!\n");
			sprintf(text,"^r%s has been killed in an explosion!^N\n",UC.name);
			wall(text,count);
			UC.hp=1;
			UC.fight=-1;
			UC.room=guilds[getguild(UC.guild)].recall_room;
			sprintf(text,"%s appears without warning.\n",UC.name);
			wroom(UC.room,text,count);
			/* find next victim */
			/* do it for all mobs which were attacking this Player*/
			fght=count;
			for (count2=0;count2<MAXMOBS;count2++) {
				if (mobs[count2].vnum==-1) continue;
				if (mobs[count2].fight!=fght) continue;

				mobs[count2].fight=next_victim(count2);
			}
		}
	}
}

/* any explosives which are armed and in rooms will reduce 'arm' by 1
 * if arm is then zero it will explode, fun eh.
 */
void dobombs()
{
	short count;

	for (count=0;count<MAXOBJS;count++) {
		if (OC.vnum==-1) continue;
		if (OC.name[0]=='\0') continue;
		if (OC.room==-1) continue;
		if (OC.arm==0) continue;
		if (!getoflag(count,oBOMB)) continue;

		OC.arm--;
		if (OC.arm<=0) {
			// booooooooooom!
			bombid=count;
			multiroom(OC.room,OC.coverage,(void *)&boom);
			reset_obj(count);
			continue;
		}
		
		wroom(OC.room,"^r~tick~^N\n",-1);
	}
}

void arm(SU)
{
	short hobj;

	if (word_count<1) {
		wuser(uid,"What would you like to arm?\n");
		return;
	}

	if (UU.sleep==2) {
		wuser(uid,"You thrash about in your sleep and arm nothing.\n");
		return;
	}

	hobj=getheldobj(uid, comstr);

	if (hobj==-1) {
		wuser(uid,"You must be holding some explosives to use this.\n");
		return;
	}

	if (!getoflag(UU.held[hobj],oBOMB)) {
		wuser(uid,"You must be holding some explosives to use this.\n");
		return;
	}

	if (objs[UU.held[hobj]].arm!=0) {
		sprintf(text,"%s is already primed.\n",objs[UU.held[hobj]].sdesc);
		wuser(uid,text);
		return;
	}

	objs[UU.held[hobj]].arm=objs[UU.held[hobj]].fuse;

	sprintf(text,"%s\n",objs[UU.held[hobj]].armstr);
	wuser(uid,text);
	sprintf(text,"%s arms %s.\n",UU.name,objs[UU.held[hobj]].sdesc);
	wroom(UU.room,text,uid);
}

/* str can just be a simple search string but may optionally
 * contain a number modifier to indicate which item
 *
 * i.e. "knife.2" will get the second knife
 */
short getobj(SU,char *str)
{
	short count;
	short found=0;
	short mod=-1;
	char needle[4096];
	char *ptr;
	char haystack[4096];

	DEBUG("getobj")

	ptr=strchr(str,'.');
	if (ptr) {
		mod=atoi(ptr+1);
		*ptr='\0';

		if (mod<1||mod>999) {
			return -1;
		}
	}

	for (count=0;count<MAXOBJS;count++)
	{
		if (OC.room==-1) continue;
		if (OC.room==UU.room) {
			sprintf(needle,"%s",str);
			sprintf(haystack,"%s",OC.name);
			stolower(needle);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				found++;
				if (found<mod) continue;
				return count;
			}
			sprintf(haystack,"%s",OC.sdesc);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				found++;
				if (found<mod) continue;
				return count;
			}
		}
	}
	return -1;
}

/* returns position in held array for user */
short getheldobj(SU,char *str)
{
	short found=0;
	short mod=-1;
	char *ptr;
	short count;
	char needle[4096];
	char haystack[4096];

	DEBUG("getheldobj")

	ptr=strchr(str,'.');
	if (ptr) {
		mod=atoi(ptr+1);
		*ptr='\0';

		if (mod<1||mod>999) {
			return -1;
		}
	}

	for (count=0;count<MAX_HELD;count++)
	{
		if (UU.held[count]>-1) {
			sprintf(needle,"%s",str);
			sprintf(haystack,"%s",objs[UU.held[count]].name);
			stolower(needle);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				found++;
				if (found<mod) continue;
				return count;
			}
			sprintf(haystack,"%s",objs[UU.held[count]].sdesc);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				found++;
				if (found<mod) continue;
				return count;
			}
		}
	}
	return -1;
}

/* returns position in worn array for user */
short getwornobj(SU,char *str)
{
	short count;
	char needle[4096];
	char haystack[4096];

	DEBUG("getheldobj")

	for (count=0;count<13;count++)
	{
		if (UU.worn[count]>-1) {
			sprintf(needle,"%s",str);
			sprintf(haystack,"%s",objs[UU.worn[count]].name);
			stolower(needle);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				return count;
			}
			sprintf(haystack,"%s",objs[UU.worn[count]].sdesc);
			stolower(haystack);
			if (strstr(haystack,needle)) {
				return count;
			}
		}
	}
	return -1;
}

short getobjn(short vnum)
{
	short count=0;

	DEBUG("getobjn")
	
	for (count=0;count<MAXOBJS;count++) {
		if (objs[count].vnum==vnum) return count;
	}

	return -1;
}

/* life span in game hours (must be less than 20000 */
short getlifespan(short oid)
{
	/* 2 game hours = 1 minute
	 * 120  = 1 hour
	 * 1200 = 10 hours
	 */
	switch(objs[oid].type) {
		case 1 : return 120; break;
		case 2 : return 120; break;
		case 3 : return 1300; break;
		case 4 : return 1100; break;
		case 5 : return 1250; break;
		case 6 : return 2400; break;
		case 7 : return 1340; break;
		case 8 : return 120; break;
		case 9 : return 1200; break;
		case 10: return 2; break;
		case 11: return 1150; break;
		case 12: return 500; break;
		case 13: return 1200; break;
		case 14: return 20; break; /* corpse, body parts */
		case 15: return 120; break;
		case 16: return 900; break;
		case 17: return 2200; break;
		case 18: return 30; break;
		case 19: return 3120; break;
		case 20: return 1200; break;
		default : return 120; break;
	}
}

/* called every game hour
 */
void age_objs()
{
	short count,count2,worn;

	for (count=0;count<MAXOBJS;count++) {
		if (OC.vnum==0) continue;
		if (OC.vnum==SIGN_OBJ) continue;
		if (OC.forsale) continue;
		if (OC.mob!=-1) continue;  /* dont age mobs stuff till they
					      drop it
					      */
		/* temporary, dont age weapons and armour */
		if ((OC.type==5)||(OC.type==7)) continue;
		if(OC.room>=20000) continue;
		/* if user is at home dont age shit*/
		if (OC.user>-1) {
			if (users[OC.user].room>=20000) continue;
			worn=0;
			for (count2=0;count2<13;count2++) {
				if (users[OC.user].worn[count2]==count) {
					worn=1;
				}
			}
			if (!worn) continue;
		}

		OC.age++;
		if (OC.age<getlifespan(count)) continue;
		if (OC.room!=-1) {
			/* object is in a room */
			sprintf(text,"%s crumbles to bits.\n",cap1st(OC.sdesc));
			wroom(OC.room,text,-1);
		}
		if (OC.user!=-1) {
			sprintf(text,"%s falls to bits around you.\n",cap1st(OC.sdesc));
			wuser(OC.user,text);
		}
		reset_obj(count);
	}
}

void sacrifice(SU)
{
	short count=0,got=0;
	short award=0;

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Sacrifice what?\n");
		return;
	}

	if (strcasecmp(word[1],"all")==0) {
		for (count=0;count<MAXOBJS;count++) {
			if (OC.vnum==0) continue;
			if (OC.room!=UU.room) continue;
			if ((getoflag(count,oNOTAKE))&&(OC.type!=10)) continue;

			sprintf(text,"You sacrifice %s.\n",OC.sdesc);
			wuser(uid,text);
			sprintf(text,"%s sacrifices %s.\n",UU.name,OC.sdesc);
			wroom(UU.room,text,uid);
			got++;
			if (OC.type==10) got++; /* small bonus for trash */
			reset_obj(count);
		}
	}
	else
	{
		count=getobj(uid,word[1]);

		if (count==-1) {
			wuser(uid,"That doesn't appear to be here.\n");
			return;
		}

		if (OC.room!=UU.room) return;
		if ((getoflag(count,oNOTAKE))&&(OC.type!=10)) {
			wuser(uid,"Don't be daft.\n");
			return;
		}

		sprintf(text,"You sacrifice %s.\n",OC.sdesc);
		wuser(uid,text);
		sprintf(text,"%s sacrifices %s.\n",UU.name,OC.sdesc);
		wroom(UU.room,text,uid);
		got=1;
		if (OC.type==10) got++; /* small bonus for trash */
		reset_obj(count);
	}
	if (got) {
		award=(got*psuedo_rand(10))+2;
		UU.cash+=from_fund(award);
		sprintf(text,"^gThe GODs award you %d gold coins",award);
		wuser(uid,text);
		if (psuedo_rand(500)<25) {
			award=psuedo_rand(10);
			sprintf(text," and %d xp.\n",award);
			wuser(uid,text);
			adjxp(uid,award);
		}
		else wuser(uid,".\n");
	}
	else
	{
		wuser(uid,"Nothing here to sacrifice.\n");
	}
}

short getfreeobj()
{
	short count;

	DEBUG("getfreeobj")
	
	for (count=0;count<MAXOBJS;count++) {
		if (OC.name[0]=='\0') { 
			reset_obj(count);
			return count;
		}
	}

	return -1;
}

short dup_obj()
{
	short count;

	DEBUG("dup_obj")

	for (count=0;count<MAXOBJS;count++) {
		if (OC.name[0]=='\0') continue;
		if (CO.vnum!=OC.vnum) continue;
		if (CO.room!=OC.room) continue;
		if (count==glob.curobj) continue;
		return 1;
	}

	return 0;
}

short read_obj_file(smud_file *file,short vnum)
{
	short found=0;
	short count=0;
	DEBUG("read_obj_file")

	if(file->vnumcount==0)
	{
		return 0;
	}
	
	for (count=0;count<file->vnumcount;count++) {
		if (file->vnumlist[count]==vnum) {
			found=1;
		}
	}

	if (found==0) return 0;

	DEBUG("read_obj:opened file")

	glob.curobj=getfreeobj();
	if (glob.curobj<0) {
		printf("No free object space\n");
		DEBUG("no new object number")
	}
	DEBUG("read_obj:got object")


	CO.vnum=vnum;
	sprintf(CO.name,getval(file,vnum,"name"));
	sprintf(CO.sdesc,getval(file,vnum,"shortdesc"));
	sprintf(CO.desc,getval(file,vnum,"longdesc"));
	CO.type=atoi(getval(file,vnum,"type"));
	sprintf(CO.flags,getval(file,vnum,"flags"));
	CO.weight=atoi(getval(file,vnum,"weight"));
	CO.cost=atoi(getval(file,vnum,"cost"));
	CO.language=atoi(getval(file,vnum,"language"));
	CO.discipline=atoi(getval(file,vnum,"discipline"));
	sprintf(CO.writing,getval(file,vnum,"writing"));
	sprintf(CO.other,getval(file,vnum,"info"));
	sprintf(CO.armstr,getval(file,vnum,"armstr"));
	sprintf(CO.script,getval(file,vnum,"script"));
	CO.fuse=atoi(getval(file,vnum,"fuse"));
	CO.yield=atoi(getval(file,vnum,"yield"));
	CO.coverage=atoi(getval(file,vnum,"coverage"));

	if (CO.type==2) {
		CO.charges=atoi(CO.other);
	}
	if(atoi(getval(file,vnum,"home"))>0)
	{
		CO.room=atoi(getval(file,vnum,"home"));
	}
	DEBUG("read_obj:end")
/*	reset_obj(glob.curobj); */
	return 1;
}

short read_obj(short vnum)
{
	FILE *fp;
	char text[1024];
	smud_file *objfile;

	sprintf(text,"grep -l '^%d {' ../objects/*.mff",vnum);

	DEBUG("read_obj")

	fp = popen(text,"r");
	if(!fp)
	{
		return -1;
	}
	if(fgets(text,255,fp))
	{
		*(text+strlen(text)-1)=0;
		objfile = read_file(text);
		if (read_obj_file(objfile,vnum)) {
			pclose(fp); 
			freefile(objfile);
			return 1; 
		}
		freefile(objfile);
	}
	pclose(fp);
	return 0;
}
	
void read_objs()
{
	FILE *fp;
	smud_file *objfile;
	char *text = malloc(256);
	short loop;

	DEBUG("read_objs")

	fp = popen("ls ../objects/*.mff","r");
	if(!fp)
	{
		printf("Cannot get list of object files!\n");
		exit(10);
	}

	while (fgets(text,255,fp)) 
	{
		*(text+strlen(text)-1)=0;
		objfile = read_file(text);
		for(loop=0; loop<objfile->vnumcount; loop++)
		{
			if(atoi(getval(objfile,objfile->vnumlist[loop],"home"))>0)
			{
				read_obj_file(objfile,objfile->vnumlist[loop]);
				if (dup_obj()) {
					reset_obj(glob.curobj);
				}
			}
		}
		freefile(objfile);
	}	
	pclose(fp);
	free(text);
}

short getnewheld(SU)
{
	short count;
	short got=0;

	DEBUG("getnewheld")

	/* check for item quantity limit */
	for (count=0;count<MAX_HELD;count++) {
		if (UU.held[count]!=-1) got++;
	}
	for (count=0;count<13;count++) {
		if (UU.worn[count]!=-1) got++;
	}

	if (got>=UU.canhold) return -1;

	got=0;
	/* check for weight issues */
	for (count=0;count<MAX_HELD;count++) {
		if (UU.held[count]!=-1) got+=objs[UU.held[count]].weight;
	}
	for (count=0;count<13;count++) {
		if (UU.worn[count]!=-1) got+=objs[UU.worn[count]].weight;
	}

	if (got>=UU.maxweight) return -1;

	/* find a free placement */
	for (count=0;count<MAX_HELD;count++) {
		if (UU.held[count]==-1) return count;
	}

	return -1;
}

void give(SU)
{
	short hobj,tobj,usr;
	char *trg=NULL;

	DEBUG("give")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}


	if (word_count<2) {
		wuser(uid,"Give [item|x coins] player.\n");
		return;
	}

	if (strcasecmp(word[2],"coins")==0) {
		// gold
		if (word_count<3) {
			wuser(uid,"Give x coins player.\n");
			return;
		}
		wuser(uid,"Not implemented yet.\n");
		return;
	}

	hobj=getheldobj(uid,word[1]);

	if (hobj>-1) {
		// held object to give to user
		if (getoflag(UU.held[hobj],oNODROP)) {
			wuser(uid,"You cannot drop that!\n");
			return;
		}
		trg=word[2];
		if (strcasecmp(word[2],"to")==0) {
			if (word_count<3) {
				wuser(uid,"Give it to who?\n");
				return;
			}
			trg=word[3];
		}
		usr=getuser(uid,trg);
		if (usr==-1) {
			wuser(uid,"who?\n");
			return;
		}
		// Got user, got object

		tobj=getnewheld(usr);

		if (tobj==-1) {
			sprintf(text,"%s cannot hold that.\n",getsex(usr,"he","she","it"));
			wuser(uid,text);
			return;
		}

		sprintf(text,"%s gives %s to you.\n",UU.name,objs[UU.held[hobj]].sdesc);
		wuser(usr,text);
		sprintf(text,"You give %s %s.\n",users[usr].name,objs[UU.held[hobj]].sdesc);
		wuser(uid,text);
		objs[UU.held[hobj]].user=usr;
		users[usr].held[tobj]=UU.held[hobj];
		UU.held[hobj]=-1;
		return;
	}
	else
	{
		wuser(uid,"Give what to who?\n");
		return;
	}
}

void dotake(SU, short hobj, short obj)
{
	short count;

	DEBUG("dotake")

	count=hobj;

	objs[obj].room=-1;
	objs[obj].mob=-1;
	objs[obj].user=uid;

	UU.held[count]=obj;

	sprintf(text,"You get %s.\n",objs[obj].sdesc);
	wuser(uid, text);

	sprintf(text,"%s gets %s.\n",UU.name,objs[obj].sdesc);
	wroom(UU.room,text,uid);
}

void take(SU)
{
	short count;
	short obj,hobj,got=0;

	DEBUG("take")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Take what?\n");
		return;
	}

	if (strcasecmp(comstr,"all")==0) {
		for (count=0;count<MAXOBJS;count++) {
			if (OC.vnum==-1) continue;
			if (OC.room!=UU.room) continue;
			if (getoflag(count,oNOTAKE)) continue;
			hobj=getnewheld(uid);
			if (hobj==-1) {
				wuser(uid,"You couldn't possibly carry anything else.\n");
				return;
			}
			got=1;
			dotake(uid,hobj,count);
		}
		if (!got) {
			wuser(uid,"There is nothing which can be taken here.\n");
		}
		return;
	}

	obj=getobj(uid, comstr);

	if (obj==-1) {
		wuser(uid,"You cannot take what isn't here.\n");
		return;
	}

	if (getoflag(obj,oNOTAKE)) {
		wuser(uid,"You cannot take that!\n");
		return;
	}

	count=getnewheld(uid);

	if (count==-1) {
		wuser(uid,"You couldn't possibly carry anything else.\n");
		return;
	}

	dotake(uid,count,obj);
}

void dodrop(SU, short hobj)
{
	short obj=hobj;

	DEBUG("dodrop")

	objs[UU.held[obj]].room=UU.room;
	objs[UU.held[obj]].mob=-1;
	objs[UU.held[obj]].user=-1;


	sprintf(text,"You drop %s.\n",objs[UU.held[obj]].sdesc);
	wuser(uid, text);

	sprintf(text,"%s drops %s.\n",UU.name,objs[UU.held[obj]].sdesc);
	wroom(UU.room,text,uid);

	UU.held[obj]=-1;
}

void xdrop(SU)
{
	short count; 
	short obj;
	short done=0;

	DEBUG("xdrop")
	if (word_count<1) {
		wuser(uid,"Drop what?\n");
		return;
	}

	if (strcasecmp(comstr,"all")==0) {
		for (count=0;count<MAX_HELD;count++) {
			if (UU.held[count]==-1) continue;
			if (getoflag(UU.held[count],oNODROP)) {
				sprintf(text,"You cannot drop %s.\n",objs[UU.held[count]].sdesc);
				wuser(uid,text);
				continue;
			}
			dodrop(uid,count);
			done=1;
		}
		if (!done) {
			wuser(uid,"You are not carrying anything which can be dropped.\n");
		}
		return;
	}

	obj=getheldobj(uid, comstr); /* relates to users.held array not 
					obj array */
	if (obj==-1) {
		wuser(uid,"You cannot drop what you are not holding.\n");
		return;
	}

	if (getoflag(UU.held[obj],oNODROP)) {
		wuser(uid,"You cannot drop that!\n");
		return;
	}

	dodrop(uid,obj);
}

void hold(SU)
{
	short obj;
	short oldworn=-1;

	DEBUG("hold")
	if (word_count<1) {
		wuser(uid,"Hold what?\n");
		return;
	}

	obj=getheldobj(uid, comstr);

	if (obj==-1) {
		wuser(uid, "You cannot hold what you are not carrying.\n");
		return;
	}

	if(!getoflag(UU.held[obj],oHOLD)) {
		sprintf(text,"You cannot hold a %s.\n",objs[UU.held[obj]].sdesc);
		wuser(uid,text);
		return;
	}

	if (UU.worn[10]!=-1) {
		sprintf(text,"You stop holding %s.\n",objs[UU.worn[10]].sdesc);
		wuser(uid,text);
		sprintf(text,"%s stops holding %s.\n",UU.name,objs[UU.worn[10]].sdesc);
		wroom(UU.room,text,uid);
		oldworn=UU.worn[10];
	}

	sprintf(text,"You hold %s.\n",objs[UU.held[obj]].sdesc);
	wuser(uid,text);
	sprintf(text,"%s holds %s.\n",UU.name,objs[UU.held[obj]].sdesc);
	wroom(UU.room,text,uid);

	UU.worn[10]=UU.held[obj];
	UU.held[obj]=oldworn;
}

/* does the process of transferring an object from worn to held, does
 * no error checking, so make sure the parameters passed are correct
 */
void removeit(SU,short wobj,short hobj) 
{
	DEBUG("removeit")

	UU.held[hobj]=UU.worn[wobj];
	UU.worn[wobj]=-1;
	sprintf(text,"You stop using %s.\n",objs[UU.held[hobj]].sdesc);
	wuser(uid,text);
	sprintf(text,"%s stops using %s.\n",UU.name,objs[UU.held[hobj]].sdesc);
	wroom(UU.room,text,uid);
}
	
void xremove(SU)
{
	short obj;
/*	short oid=-1; - Not used yet */
/*	short wid=-1; - Not used yet */
/*	char pname[128]; - Not used yet */
	short count;
	short gotsome=0;

	DEBUG("remove")
	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Remove what?\n");
		return;
	}

	if (strcasecmp(word[1],"all")==0) {
		for (count=0;count<13;count++) {
			if (UU.worn[count]!=-1) {
				obj=getnewheld(uid);
				if (obj==-1) {
					wuser(uid,"You couldn't possibly hold anything else.\n");
					break;
				}
				removeit(uid,count,obj);
				gotsome=1;
			}
		}
		if (!gotsome) {
			wuser(uid,"You arn't using anything that can be removed.\n");
		}
		return;
	}

	count=getwornobj(uid, comstr);

	if (count==-1) {
		wuser(uid, "You cannot remove what you are not wearing.\n");
		return;
	}

	obj=getnewheld(uid);

	if (obj==-1) {
		wuser(uid,"You couldn't possibly hold anything else.\n");
		return;
	}

	removeit(uid,count,obj);
}

short wearit(short index,short obj,short heldobj, short mob)
{
/*	short oid=-1; - Not used yet */
	short wid=-1;
	char pname[128];
	char mname[128];

	DEBUG("wearit")

	if(getoflag(obj,oHEAD)) {
		wid=0;
		strcpy(pname,"their head");
		strcpy(mname,"your head");
	}
	if(getoflag(obj,oNECK)) {
		wid=1;
		strcpy(pname,"their neck");
		strcpy(mname,"your neck");
	}

	if(getoflag(obj,oARMS)) {
		wid=2;
		strcpy(pname,"their arms");
		strcpy(mname,"your arms");
	}

	if(getoflag(obj,oBODY)) {
		wid=3;
		strcpy(pname,"their body");
		strcpy(mname,"your body");
	}

	if(getoflag(obj,oCHEST)) {
		wid=4;
		strcpy(pname,"their chest");
		strcpy(mname,"your chest");
	}

	if(getoflag(obj,oLEGS)) {
		wid=5;
		strcpy(pname,"their legs");
		strcpy(mname,"your legs");
	}

	if(getoflag(obj,oFEET)) {
		wid=6;
		strcpy(pname,"their feet");
		strcpy(mname,"your feet");
	}

	if(getoflag(obj,oWRIST)) {
		wid=7;
		strcpy(pname,"their wrist");
		strcpy(mname,"your wrist");
	}

	if(getoflag(obj,oHAND)) {
		wid=9;
		strcpy(pname,"their hands");
		strcpy(mname,"your hands");
	}

	if(getoflag(obj,oSHIELD)) {
		wid=12;
		strcpy(pname,"as a shield");
		strcpy(mname,"as a shield");
	}

	if (objs[obj].type==WEAPON) {
		wid=11;
		strcpy(pname,"as a weapon");
		strcpy(mname,"as a weapon");
	}

	if (wid==-1) return -1;

	if (!mob) {
		if (users[index].worn[wid]!=-1) {
			sprintf(text,"You already have something on %s.\n",mname);
			wuser(index,text);
			return 0;
		}
		objs[obj].mob=-1;
		objs[obj].room=-1;
		objs[obj].user=index;
		users[index].worn[wid]=obj;
		users[index].held[heldobj]=-1;
		sprintf(text,"You put %s on %s.\n",objs[obj].sdesc,mname);
		wuser(index,text);
		sprintf(text,"%s puts %s on %s.\n",users[index].name,objs[obj].sdesc,pname);
		wroom(users[index].room,text,index);
	}
	else
	{
		if (mobs[index].worn[wid]!=-1) return 0;
		mobs[index].worn[wid]=obj;
		objs[obj].mob=mobs[index].vnum;
		objs[obj].user=-1;
		objs[obj].room=-1;
	}
	return 1;
}

void wear(SU)
{
	short obj;
	short wid=-1;
	short count=0;

	DEBUG("wear")
	
	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Wear what?\n");
		return;
	}

	if (strcasecmp(word[1],"all")==0) {
		/* wear all holding */
		if (UU.fight!=-1) {
			/* cannot wear all whilst fighting */
			wuser(uid,"You dont really have enough time to wear it all at once.\n");
			return;
		}

		for (count=0;count<MAX_HELD;count++) {
			if (UU.held[count]!=-1) {
				wid=wearit(uid,UU.held[count],count,0);
			}
		}

		return;
	}

	obj=getheldobj(uid, comstr);

	if (obj==-1) {
		wuser(uid, "You cannot wear what you are not carrying.\n");
		return;
	}

	wid=wearit(uid,UU.held[obj],obj,0);

	if (wid==-1) {
		wuser(uid,"You cannot wear that.\n");
		return;
	}

}

short count_total_objs(short vnum)
{
	short count;
	short tot=0;
        for (count=0;count<MAXOBJS;count++) {
                if (objs[count].vnum==vnum) tot++;
        }
	return tot;
}

short count_sale_objs(short vnum)
{
	short count;
	short tot=0;
        for (count=0;count<MAXOBJS;count++) {
		if(objs[count].forsale==1)
			if (objs[count].vnum==vnum) tot++;
        }
	return tot;
}

long calc_price(short vnum)
{
	float num,sale;
	float saleperc;
	short obj;
	float div;
	float cost;

	num = count_total_objs(vnum);
	if(num<=0)
	{
		return 0;
	}
	sale = count_sale_objs(vnum);
	obj = getobjn(vnum);
	cost = objs[obj].cost;
	div = ((cost/100)*(num/10));
	if(div>(cost/2))
		div=cost/2;
	cost = cost - div;
	
	saleperc=(sale/num);
	if(saleperc>0.75)
		saleperc=0.75;
	return (long)(cost*(1-saleperc));
}
