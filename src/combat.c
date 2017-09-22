/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :Combat.c
** Purpose :Combat associated commands and features
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 27-11-01 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short,long);
extern char *getmobsex(short,char *, char *, char *);
extern char *cap1st(char *);
extern void move(SU, short);
extern void wgroup(SU, char *,short);
extern short getuser(SU,char *);
extern char *getguilddef(short);
extern void look(SU);
extern void brieflook(SU);
extern short next_victim(short);
extern void wroom(short, char *, short);
extern char *getdir(short);
extern int psuedo_rand(int);
extern short getrflag(short, short);
extern void wuser(SU, char *);
extern void unhide(SU);
extern short getroom(short);
extern short getmob(SU,char *);

// this needs calling before you reset UU.fight
void funkydeath(SU)
{
	short chance=0;
	char plr[2048];
	char rm[2048];

	chance=psuedo_rand(10);

	switch(chance) {
		case 0 : sprintf(plr,"You maul the body and tear out the eyes with your fingernails.\n");
			sprintf(rm,"%s mauls the body and tears out the eyes with their fingernails.\n",UU.name);
			break;
		case 1 : sprintf(plr,"You decide a few more kicks to the head is in order.\n");
			sprintf(rm,"%s puts the boot in a few more times just to make sure.\n",UU.name);
			break;
		case 2 : sprintf(plr,"You stand over the corpse swearing at it for a full 5 minutes.\n");
			sprintf(rm,"%s practices bad language at the corpse.\n",UU.name);
			break;
		case 3 : sprintf(plr,"You feel randy and fuck the corpse in a deep wound.\n");
			sprintf(rm,"%s humps a deep wound in the corpse.\n",UU.name);
			break;
		case 4 : sprintf(plr,"You remove the corpse's rectum and sell it to a passing fast food merchant.\n");
			sprintf(rm,"%s removes the corpse's rectum and sells it to a passing fast food merchant.\n",UU.name);
			break;
		case 5 : sprintf(plr,"You smack the corpse in the head with a rock till it bursts.\n");
			sprintf(rm,"%s smacks the corpse in the head with a rock till it pops.\n",UU.name);
			break;
		case 6 : sprintf(plr,"You chop the body into tiny pieces then piss on it.\n");
			sprintf(rm,"%s chops the body into tiny pieces then pisses on it.\n",UU.name);
			break;
		case 7 : sprintf(plr,"You shit in the body's mouth, put a candle in and sing happy deathday.\n");
			sprintf(rm,"%s shits in the body's mouth, puts a candle in and starts to sing.\n",UU.name);
			break;
		case 8 : sprintf(plr,"You remove an arm from the corpse and slap it with it.\n");
			sprintf(rm,"%s slaps the corpse with its own arm.\n",UU.name);
			break;
		case 9 : sprintf(plr,"You remove all the skin from the corpse and wear it as a cape.\n");
			sprintf(rm,"%s removes all the skin from the corpse and wears it as a cape.\n",UU.name);
			break;
		case 10 : sprintf(plr,"You maul the body and tear out the eyes with your fingernails.\n");
			sprintf(rm,"%s mauls the body and tears out the eyes with their fingernails.\n",UU.name);
			break;
		default: sprintf(plr,"You maul the body and tear out the eyes with your fingernails.\n");
			sprintf(rm,"%s mauls the body and tears out the eyes with their fingernails.\n",UU.name);
			break;
	}
	sprintf(text,"^H%s",plr);
	wuser(uid,text);
	wroom(UU.room,rm,uid);
}

// This is also done in charge possibly combine the two
void killx(SU)
{
	short mob;
	short rc;

	DEBUG("killx")
	rc=getroom(UU.room);

	if (UU.sleep>0) {
		wuser(uid,"You must be awake and standing to fight.\n");
		return;
	}

	if (UU.immstatus) {
		wuser(uid,"Immortals cannot fight.\n");
		return;
	}

	if (UU.sleep>0) {
		wuser(uid,"You must be awake and standing to fight.\n");
		return;
	}

	if (getrflag(rc,PEACE)) {
		wuser(uid,"No fighting may occur here!\n");
		return;
	}

	
	if (UU.fight!=-1) {
		wuser(uid,"You are already fighting!\n");
		return;
	}

	if (word_count<1) {
		wuser(uid,"Kill who?\n");
		return;
	}

	mob=getmob(uid,comstr);
	if (mob==-1) {
		wuser(uid,"No one of that name here!\n");
		return;
	}
	if (mobs[mob].fight>-1) {
		wuser(uid,"That is already fighting someone!\n");
		return;
	}

	unhide(uid);

	if (mobs[mob].nokill[0]!='\0')
	{
		sprintf(text,"%s\n",mobs[mob].nokill);
		wuser(uid,text);
		return;
	}

	UU.fight=mob;

	mobs[mob].fight=uid;
	sprintf(text,"You prepare to ^RKILL^N %s.\n",mobs[mob].sdesc);
	wuser(uid,text);
	UU.invis=0;
}

void flee(SU)
{
	short chance=0;
	short fd=-1,sr=0;
	short count=0;
	short count2=0;

	DEBUG("flee")
	chance=psuedo_rand(100);

	if (UU.fight==-1) {
		wuser(uid,"You have nothing to flee from.\n");
		return;
	}

	if (chance>75) {
		wuser(uid,"You failed to flee!\n");
	} else
	{
		while (fd==-1) {
			count++;
			if (count>20) { /* infinite loop protection */
				wuser(uid,"You failed to flee!\n");
				return;
			}
			sr=psuedo_rand(10)-1;
			if (rooms[getroom(UU.room)].dir[sr]>0)
				fd=sr;
		}
		sprintf(text,"%s has fled %s.\n",UU.name,getdir(fd));
		wroom(UU.room,text,uid);
		UU.room=rooms[getroom(UU.room)].dir[fd];
		UU.fight=-1;
		adjxp(uid,-25);
		for (count2=0;count2<MAXMOBS;count2++) {
			if (mobs[count2].vnum==-1) continue;
			if (mobs[count2].fight!=uid) continue;

			mobs[count2].fight=next_victim(count2);
		}
		if(UU.brief)
		{
			brieflook(uid);
		} else {
			look(uid);
		}
		wuser(uid,"You flee! (you lose 25xp)\n");
	}
}

void mobflee(short mid)
{
        short chance=0;
        short fd=-1,sr=0;
        short count=0;
        short count2=0;

        DEBUG("flee")
        chance=psuedo_rand(100);

        if (chance<=75) 
        {
                while (fd==-1) {
                        count++;
                        if (count>20) { /* infinite loop protection */
                                return;
                        }
                        sr=psuedo_rand(10)-1;
                        if (rooms[getroom(mobs[mid].room)].dir[sr]>0)
                                fd=sr;
                }
                sprintf(text,"%s has fled %s.\n",mobs[mid].name,getdir(fd));
                wroom(mobs[mid].room,text,-1);
                mobs[mid].room=rooms[getroom(mobs[mid].room)].dir[fd];
		for(count2=0; count2<MAXUSERS; count2++)
		{
			if(users[count2].fight==mid)
				users[count2].fight=-1;
		}
		mobs[mid].fight=-1;
        }
}

char *gettype(short id, short mob)
{
	char test;
	if (mob) {
		if (mobs[id].vnum==-1) return "hit";
		if (mobs[id].worn[11]==-1) return "hit";
		test=objs[mobs[id].worn[11]].other[0];
	}
	else
	{
		if (users[id].socket==-1) return "hit";
		if (users[id].worn[11]==-1) return "hit";
		test=objs[users[id].worn[11]].other[0];
	}

	switch (test) {
		case 'a' : return "hit";
		case 'b' : return "sting";
		case 'c' : return "whip";
		case 'd' : return "slash";
		case 'e' : return "bite";
		case 'f' : return "bludgeon";
		case 'g' : return "crush";
		case 'h' : return "pound";
		case 'i' : return "claw";
		case 'j' : return "maul";
		case 'k' : return "thrash";
		case 'l' : return "pierce";
		case 'm' : return "blast";
		case 'n' : return "punch";
		case 'o' : return "stab";
		default  : return "hit";
	}
}

char *gettypes(short id, short mob)
{
	char test;
	if (mob) {
		if (mobs[id].vnum==-1) return "hits";
		if (mobs[id].worn[11]==-1) return "hits";
		test=objs[mobs[id].worn[11]].other[0];
	}
	else
	{
		if (users[id].socket==-1) return "hits";
		if (users[id].worn[11]==-1) return "hits";
		test=objs[users[id].worn[11]].other[0];
	}

	switch (test) {
		case 'a' : return "hits";
		case 'b' : return "stings";
		case 'c' : return "whips";
		case 'd' : return "slashes";
		case 'e' : return "bites";
		case 'f' : return "bludgeons";
		case 'g' : return "crushes";
		case 'h' : return "pounds";
		case 'i' : return "claws";
		case 'j' : return "mauls";
		case 'k' : return "thrashes";
		case 'l' : return "pierces";
		case 'm' : return "blasts";
		case 'n' : return "punchs";
		case 'o' : return "stabs";
		default  : return "hits";
	}
}

short gethitindex(short index, short mob)
{
	short val=1;

	DEBUG("gethitindex")

	if (mob) {
		if (mobs[index].worn[11]!=-1) {
			val=atoi(objs[mobs[index].worn[11]].other+1);
		}
	}
	else
	{
		if (users[index].worn[11]!=-1) {
			val=atoi(objs[users[index].worn[11]].other+1);
		}
	}

	if (val>200) val=200;
	if (val<0) val=0;

	return val;
}

short defval(short index, short mob)
{
	short val;
	short count;

	DEBUG("defval")

	/* okay 1 point per level */
	val=mob?mobs[index].lvl:users[index].lvl;
	if (!mob) val=(val>50)?50:val;

	for (count=0;count<10;count++)
	{
		if((mob?atoi(objs[mobs[index].worn[count]].other):
		       atoi(objs[users[index].worn[count]].other))==-1) 
		       continue;
		val+=mob?atoi(objs[mobs[index].worn[count]].other):
			 atoi(objs[users[index].worn[count]].other);
	}

	return val;
}

short attval(short index, short mob)
{
	short val;
/*	short count; - Not used yet */

	DEBUG("attval")

	/* okay 1 point per level */
	val=mob?mobs[index].lvl:users[index].lvl;
	if (!mob) val=(val>50)?50:val;

	val+=gethitindex(index,mob);

	return val;
}

void group(SU)
{
	short count;
	short target;

	DEBUG("group")

	if (word_count<1) {
		sprintf(text,"%s's group:\n",users[UU.groupno].name);
		wuser(uid,text);
		/* show stats */
		for (count=0;count<MAXUSERS;count++) {
			if (UC.socket<0) continue;
			if (UC.groupno!=UU.groupno) continue;
			sprintf(text,"%s, level %d %s, with a rating of %d and %d of %d health.\n",UC.name,UC.lvl,getguilddef(UC.guild),attval(count,0)+defval(count,0),UC.hp,UC.maxhp);
//			sprintf(text,"[%4d %-10s]%10s %4d/%4d hp %4d/%4d man %4d/%4d mov %6dxp\n",UC.lvl,getguilddef(UC.guild),UC.name,UC.hp,UC.maxhp,UC.man,UC.maxman,UC.mv,UC.maxmv,UC.xp);
			wuser(uid,text);
		}
		return;
	}
	
        target=getuser(uid,word[1]);

	if (UU.immstatus) {
		wuser(uid,"Immortals cannot group.\n");
		return;
	}

        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	if (target==uid) {
		wuser(uid,"Chasing your own tail?\n");
		return;
	}
	
	if (users[target].room!=UU.room) {
		wuser(uid,"They are not here.\n");
		return;
	}

	if (users[target].follow!=uid) {
		wuser(uid,"They are not following you.\n");
		return;
	}
	if (users[target].immstatus) {
		wuser(uid,"You cannot group with an immortal.\n");
		sprintf(text,"%s tried to group you, but you are in IMM mode.\n",UU.name);
		wuser(target,text);
		return;
	}
	if (UU.fight!=-1) {
		wuser(uid,"You cannot group whilst fighting.\n");
		return;
	}

	if (UU.lvl>users[target].lvl) {
		if (UU.lvl-users[target].lvl>8) {
			wuser(uid,"You can only group with players who are within 8 levels of you.\n");
			return;
		}
	}
	else
	{
		if (users[target].lvl-UU.lvl>8) {
			wuser(uid,"You can only group with players who are within 8 levels of you.\n");
			return;
		}
	}

	unhide(uid);

	users[target].groupno=uid;
	sprintf(text,"You join %s's group.\n",UU.name);
	wuser(target,text);
	sprintf(text,"%s joins the group.\n",users[target].name);
	wgroup(uid,text,target);
}

void follow(SU)
{
	short target=0;

	DEBUG("follow")

	if (word_count<1) {
		if (UU.follow==-1) {
			wuser(uid,"Follow who?\n");
			return;
		}
		wuser(uid,"You stop following.\n");
		sprintf(text,"%s stops following %s.\n",UU.name,users[UU.follow].name);
		wroom(UU.room,text,uid);
		UU.follow=-1;
		UU.groupno=uid;
		return;
	}

        target=getuser(uid,word[1]);

	if (target==uid) {
		if (UU.follow==-1) {
			wuser(uid,"Chasing your own tail?\n");
			return;
		}
		wuser(uid,"You stop following.\n");
		if (users[UU.follow].socket>-1 && users[UU.follow].login==0) {
			sprintf(text,"%s stops following %s.\n",UU.name,users[UU.follow].name);
			wroom(UU.room,text,uid);
		}
		UU.follow=-1;
		UU.groupno=uid;
		return;
	}

        if (target==-1) {
                wuser(uid,"Who?\n");
                return;
        }

	if (users[target].room!=UU.room) {
		wuser(uid,"They are not here.\n");
		return;
	}

	if (UU.follow==target) {
		wuser(uid,"You are already following them.\n");
		return;
	}

	if (UU.follow!=-1) {
		if (users[UU.follow].socket>-1 && users[UU.follow].login==0) {
			sprintf(text,"%s stops following %s.\n",UU.name,users[UU.follow].name);
			wroom(UU.room,text,uid);
		}
		UU.follow=-1;
		UU.groupno=uid;
	}

	unhide(uid);
	UU.follow=target;

	sprintf(text,"You are now following %s.\n",users[target].name);
	wuser(uid,text);
	if (UU.rank>NORM&&UU.immstatus)
	{
		wuser(uid,"Just to note, you are still in IMM mode.\n");
	}
	sprintf(text,"%s follows %s.\n",UU.name,users[target].name);
	wroom(UU.room,text,uid);
}

/* This specifies the uid to be followed and the room to look for players
 * in, the original player should have moved rooms already so this should
 * avoid recursive follows
 */
void dofollow(SU, short oldroom, short dir)
{
	short count;

	DEBUG("dofollow")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (UC.room!=oldroom) continue;
		if (UC.follow!=uid) continue;
		if (UC.fight!=-1) continue;
		if (UC.sleep!=0) continue;
		if (count==uid) continue;
		move(count, dir);
	}
}

void nofollow(SU)
{
	short count;

	DEBUG("nofollow")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (UC.follow==uid) {
			UC.follow=-1;
		 	sprintf(text,"You stop following %s.\n",UU.name);
			wuser(count,text);
		}
	}
}

void killgroup(SU)
{
	short count;

	DEBUG("killgroup")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (count==uid) continue;
		if (UC.groupno!=UU.groupno) continue;

		UC.groupno=count;

		sprintf(text,"%s disbands the group.\n",UU.name);
		wuser(count,text);
	}
}

/* sees if god/devil will interviene in fight
 */
void godint(short usr, short mob)
{
	short palign=0,falign=0;
	short con=0;

	palign=mob?mobs[usr].align:users[usr].align;
	falign=mob?users[mobs[usr].fight].align:mobs[users[usr].fight].align;

	palign+=1000;
	falign+=1000;
	
	if (palign>falign) con=palign-falign;
	if (palign<=falign) con=falign-palign;

	if (con>250) {
		if (psuedo_rand(1000)>995) {
			if ((time(0)-glob.lastgod)<600) return;
			glob.lastgod=time(0);
			if (mob) {
				sprintf(text,"^BSome deity takes pity on %s.^N\n",mobs[usr].name);
				wroom(mobs[usr].room,text,-1);
				mobs[usr].hp+=100;
				if (mobs[usr].hp>mobs[usr].maxhp) mobs[usr].hp=mobs[usr].maxhp;
			}
			else
			{
				wuser(usr,"^BSome deity takes pity on you.^N\n");
				sprintf(text,"^BSome deity takes pity on %s.^N\n",users[usr].name);
				wroom(users[usr].room,text,usr);
				users[usr].hp+=100;
				if (users[usr].hp>users[usr].maxhp) users[usr].hp=users[usr].maxhp;
			}
		}
	}
}

short countgroup(SU)
{
	short count;
	short gmem=0;

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.groupno==UU.groupno) gmem++;
	}

	return gmem;
}

void conout(SU, short mob)
{
		long acon;
		long dcon;
		long gcon;
		short matt,mdef,patt,pdef;
		char hstr[128];

	DEBUG("conout")

		matt=attval(mob,1);
		mdef=defval(mob,1);
		patt=attval(uid,0);
		pdef=defval(uid,0);
		
		gcon=(matt+mdef)-(patt+pdef);
		acon=matt-patt;
		dcon=mdef-pdef;
		
		/* default */
		sprintf(text,"^gYou must be bored or something to attack %s^N",mobs[mob].name);
		if (acon>=-5) {
			sprintf(text,"^gYou'll have little problem with %s^N",mobs[mob].name);
		}
		if (acon>=-3) {
			sprintf(text,"^b%s looks fairly soft^N",cap1st(mobs[mob].name));
		}
		if (acon>=-1) {
			sprintf(text,"^bYou are a good match against %s^N",mobs[mob].name);
		}
		if (acon>=2) {
			sprintf(text,"^bYou feel somewhat threatened by %s^N",mobs[mob].name);
		}
		if (acon>=4) {
			sprintf(text,"^rYou cower in fear from %s^N",mobs[mob].name);
		}
		if (acon>=6) {
			sprintf(text,"^rYou are insane to consider %s^N",mobs[mob].name);
		}
		wuser(uid,text);

		/* default */
		sprintf(text," and ^gyou notice %s pathetic armour.^N\n",getmobsex(mob,"his","her","its"));
		if (dcon>=-5) {
			sprintf(text," and ^g%s defenses are minimal.^N\n",getmobsex(mob,"his","her","its"));
		}
		if (dcon>=-3) {
			sprintf(text,", ^b%s is slightly armoured.^N\n",getmobsex(mob,"he","she","it"));
		}
		if (dcon>=-1) {
			sprintf(text,", ^byou are as well armoured as %s is.^N\n",getmobsex(mob,"he","she","it"));
		}
		if (dcon>=2) {
			sprintf(text,", ^byour equipment is less than perfect.^N\n");
		}
		if (dcon>=4) {
			sprintf(text,", ^ryou feel naked compared to %s.^N\n",getmobsex(mob,"him","her","it"));
		}
		if (dcon>=6) {
			sprintf(text," and ^r%s is superbly armoured.^N\n",getmobsex(mob,"he","she","it"));
		}
		wuser(uid,text);
		/*
		sprintf(text,"     combined con rating of %d.\n",matt+mdef);
		wuser(uid,text);
		*/

		if (UU.lvl<5) {
			sprintf(hstr,"Rating:%d",matt+mdef);
		}
		else
		{
			hstr[0]='\0';
		}

		sprintf(text,"^yIt is doubtfull you'll get anything by killing %s. %s^N\n",getmobsex(mob,"him","her","it"),hstr);
		if (gcon>=-4) {
			sprintf(text,"^gYou may gain a little bit by killing %s. %s^N\n",getmobsex(mob,"him","her","it"),hstr);
		}
		if (gcon>=-2) {
			sprintf(text,"^bYou should gain well by killing %s. %s^N\n",getmobsex(mob,"him","her","it"),hstr);
		}
		if (gcon>=2) {
			sprintf(text,"^rYou should gain a lot if you manage to kill %s. %s^N\n",getmobsex(mob,"him","her","it"),hstr);
		}
		wuser(uid,text);
}

void consider(SU)
{
		short mob;
		short count;
/*		long acon; - Not used yet */
/*		long dcon; - Not used yet */
		short got=0;
/*		short matt,mdef,patt,pdef; - Not used yet */

		DEBUG("consider")
		if (word_count<1) {
			wuser(uid,"Consider what?\n");
			return;
		}

		if (UU.sleep==2) {
			wuser(uid,"Wake up first.\n");
			return;
		}

		if (strcmp(word[1],"all")!=0) {
			mob=getmob(uid,comstr);
			if (mob<0) {
				wuser(uid,"Nothing of that sort here.\n");
				return;
			}
			conout(uid,mob);
		}
		else
		{
			for (count=0;count<MAXMOBS;count++)
			{
				if (MC.vnum==-1) continue;
				if (MC.room!=UU.room) continue;
				got=1;
				conout(uid,count);
			}
			if (!got) {
				wuser(uid,"There is nothing to consider here.\n");
			}
		}
}

