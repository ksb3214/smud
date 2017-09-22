/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :auto.c
** Purpose :what happens each tick
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern void adjxp(short,long);
extern void do_trans();
extern short getguild(short);
extern void wall(char *, short);
extern void prompt(SU);
extern char *cap1st(char *);
extern short getrflag(short, short);
extern void movemob(short, short);
extern unsigned long from_fund(unsigned long);

extern short getroom(short);
extern short getarea(short);
extern short getmflag(short, short);
extern void reset_mob(short);
extern short countgroup(SU);
extern void afk(SU);
extern void storekill(SU,short);
extern void dobombs();
extern short read_obj(short);
extern void savestock();
extern short memkills(short);
extern void flee(SU);
extern char *gettypes(short, short);
extern char *gettype(short, short);
extern short gethitindex(short, short);
extern short defval(short, short);
extern short attval(short, short);
extern void doskills(SU);
extern short getuskill(SU,short);
extern short next_victim(short);
extern void wroom(short, char *, short);
extern int psuedo_rand(int);
extern void qquit(SU);
extern void reset_obj(short);

extern void wuser(SU, char *);
extern void read_objs();
extern void mobflee(short);
extern void test_weather();
extern void agemobs();
extern void add_interest();
extern void average_bank();
extern void healers();
extern void age_objs();
extern void restore_mobs();
extern short psave_all(short);
extern void save_debates();
extern void save_home_objs();
extern short getnewheld(SU);
extern void sacrifice(SU);
extern void funkydeath(SU);

void minute()
{
	time_t t;
	struct tm *now;
	short ucnt=0;
	short count;

	t = time(NULL);
	now = gmtime(&t);
	for (count=0;count<MAXUSERS;count++) 
		if (UC.socket>-1) 
			ucnt++;

	if(ucnt>glob.stats.users[now->tm_hour].max)
		glob.stats.users[now->tm_hour].max = ucnt;

	if(ucnt<glob.stats.users[now->tm_hour].min)
		glob.stats.users[now->tm_hour].min = ucnt;

	event(eMINUTE,-1,-1,-1);
}

void autox()
{
	short count,dam,hit,xpa,sr;
/*	short hpg; - Not used yet */
	short count2=0,fght;
	short pcona, pcond, mcona, mcond;
	short gmem,loop;
	short numdead=0;
	short mobcon=0;
	short attack,prob,dodge=0,parry=0,block=0;
	char bonstr[40];
	short bonus, xdamage;
	short hand=0; // handicap
	short rc=0;
	time_t t;
	struct tm *now;
	short fid=-1;
	short carryob,corpse=-1;

	DEBUG("autox")

	word_count=0;  // a bit of a kludge, but makes sure afk() works
	t = time(NULL);
	now = gmtime(&t);
	if(glob.stats.last_min!=now->tm_min)
	{
		glob.stats.last_min = now->tm_min;
		minute();
	}

	if (time(0)-glob.lastauto<2) return;

	event(eTICK,-1,-1,-1);

	dobombs();

	/* AUTOSAVE */
	if ((time(0)-glob.lastsave>AUTOSAVE)) {
		psave_all(0);
		glob.lastsave=time(0);
	}

	glob.lastauto=time(0);
	glob.ht++;
	glob.hr=0;
	if (glob.ht==15) { glob.ht=0;glob.hr=1; }

	/* HOURLY TICK */
	if (glob.hr==1) {
		save_debates();
		save_home_objs();
		savestock();
		restore_mobs();
		age_objs();
		healers();
		do_trans();
		glob.gamehour++;
		if (glob.gamehour==24) { /* new day */
			glob.gamehour=0;
			glob.gameday=glob.gameday==300?0:glob.gameday+1;
			average_bank();
			glob.intdays++;
			if (glob.intdays==INTDAYS) {
				glob.intdays=0;
				add_interest();
			}
			agemobs();
		}
		test_weather();
		glob.cr_objs++;
		if (glob.cr_objs==glob.r_objs) {
			glob.cr_objs=0;
			read_objs();
		}
	}

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket==-1) continue;
		if (UC.login!=0) { 
			if (time(0)-UC.thislog>LOGINIDLE) {
				wuser(count,"Sorry you idled out of login.\n");
				qquit(count);
				continue;
			}
			// put some new user timeout code here
		}

		/* HOURLY TICK */
		if (glob.hr) {
			if (UC.room<20000) {
				if (UC.sleep==0) {
					UC.hp+=(int)(UC.maxhp*.001)+1;
					UC.man+=(int)(UC.maxhp*.001)+1;
					UC.mv+=(int)(UC.maxhp*.001)+1;
				}
				if (UC.sleep==1) {
					UC.hp+=(int)(UC.maxhp*.01)+1;
					UC.man+=(int)(UC.maxhp*.01)+1;
					UC.mv+=(int)(UC.maxhp*.01)+1;
				}
				if (UC.sleep==2) {
					UC.hp+=(int)(UC.maxhp*.2)+psuedo_rand(UC.str)+1;
					UC.man+=(int)(UC.maxhp*.2)+psuedo_rand(UC.intl)+1;
					UC.mv+=(int)(UC.maxhp*.2)+psuedo_rand(UC.dex)+1;
				}
				UC.hp=UC.hp>UC.maxhp?UC.maxhp:UC.hp;
				UC.man=UC.man>UC.maxman?UC.maxman:UC.man;
				UC.mv=UC.mv>UC.maxmv?UC.maxmv:UC.mv;
				if (UC.invis) {
					UC.invis--;
					if (!UC.invis) {
						wuser(count,"You are now visible.\n");
						sprintf(text,"%s becomes visible.\n",UC.name);
						wroom(UC.room,text,count);
					}
				}
			}
			rc=getroom(UC.room);
			if (getrflag(rc,OUTSIDE)) {
				if ((UC.worn[6]==-1)&&(UC.sleep!=2)) {
					// nothing on feet
					if (glob.weather>=wRAIN) {
						// it's raining
						if (psuedo_rand(200)<20) {
							wuser(count,"^c~squelch~^N\n");
						}
					}
				}
			}

		}

		/* IDLEOUT */
		if (UC.afk==0) {
			if (time(0)-UC.idle>AUTOAFK) {
				afk(count);
			}	
			// idle removed for now
//			if (time(0)-UC.idle>glob.normidle) { /* need to add for homerooms */
//				wuser(count,"Sorry you have been idle for too long.\n");
//				psave(count);
//				qquit(count);
//				continue;
//			}
//			if ((time(0)-UC.idle>glob.normidle-120)&&(UC.idlewarn==0)) { /* two minute warning */
//				wuser(count,"You will idle-out in 2 minutes.\n");
//				UC.idlewarn=1;
//			}	
		}
		// temp, whilst no idle-out
		if (UC.afk<0) UC.afk=0;
		/* see if the users group has started fighting */
		for (count2=0;count2<MAXUSERS;count2++) {
			if (users[count2].socket<0) continue;
			if (users[count2].room!=UC.room) continue;
			if (count2==count) continue;
			if (users[count2].groupno!=UC.groupno) continue;
			if (users[count2].fight==-1) continue;
			if (UC.fight!=-1) continue;
			if (UC.sleep==2) continue;
			UC.fight=users[count2].fight;
			UC.invis=0;
			UC.sleep=0;
		}

		/* see if a mob is attacking us */
		for (count2=0;count2<MAXMOBS;count2++) {
			if (mobs[count2].vnum==-1) continue;
			if (mobs[count2].fight!=count) continue;
			if (UC.fight!=-1) continue;
			UC.fight=count2;
			UC.sleep=0;
			UC.invis=0;
		}

		/* FIGHT */
		if (UC.fight==-1) {
			if (UC.queue[0]!='\0') {
				wuser(count,"^rCombat queue cleared!^N\n");
				UC.queue[0]='\0';
			}
			UC.maxcons=0;
			continue;
		}
		UC.idle=time(0);

		if (mobs[UC.fight].room!=UC.room) {
			wuser(count,"Somehow you escaped that one!\n");
			UC.fight=-1;
			for (count2=0;count2<MAXMOBS;count2++) {
				if (mobs[count2].vnum==-1) continue;
				if (mobs[count2].fight!=count) continue;

				mobs[count2].fight=next_victim(count2);
			}
			return;
		}

	//	godint(count,0);

		for (attack=0;attack<3;attack++) {
			if (UC.fight==-1) break; /* get out!! */
			if (attack==1) {
				prob=psuedo_rand(100);
				if (prob>getuskill(count,24)) continue;
			}
			if (attack==2) {
				prob=psuedo_rand(100);
				if (prob>getuskill(count,11)) continue;
				if (UC.worn[10]==-1) continue;
			}
			// do skills first
			if (attack==0) doskills(count);
			// incase doskills ended the fight
			if (UC.fight==-1) break;

			pcona=attval(count,0);
			mcond=defval(UC.fight,1);
			pcond=defval(count,0);
	
			if (pcona+pcond>UC.maxcons) UC.maxcons=pcona+pcond;
		
			pcona+=psuedo_rand(10);
			mcond+=psuedo_rand(10);
			// handicap
			hand=0;
			if (UC.lvl<5) hand=4;
			dam=0;
			hit=0;
			if (pcona+hand>=mcond) {
				dam=psuedo_rand(gethitindex(count,0))+1;
				dam*=(pcona+(hand==0?0:1)-mcond);
				if (attack==2) dam=1;
				if (dam<0) dam=0;
				mobs[UC.fight].hp-=dam;
				hit=1;
			}
			if (hit) {
				/* work out any bonuses from skills */
				prob=0;
				bonus=0;

				if (UC.worn[11]==-1) {
					strcpy(bonstr,"^Bbarehand^N ");
					prob=getuskill(count,23);
				}
				else
				{
					switch (objs[UC.worn[11]].other[0]) {
						case 'b' :
						case 'l' :
						case 'o' :
						case 'd' : /* sharp */
							   strcpy(bonstr,"^Bsharp^N ");
							   prob=getuskill(count,20);
							   break;
						case 'f' :
						case 'h' :
						case 'n' : /* blunt */
							   strcpy(bonstr,"^Bblunt^N ");
							   prob=getuskill(count,21);
							   break;
						case 'g' :
						case 'j' : /* heavy */
							   strcpy(bonstr,"^Bheavy^N ");
							   prob=getuskill(count,22);
							   break;
						default : break;
					}
				}

				if (prob>0) {
					if (psuedo_rand(CHANCE)<prob) {
						bonus=1;
						mobs[UC.fight].hp-=BONUSDAM;
						dam+=BONUSDAM;
					}
				}

				/* everyone can get xdamage so do this
				 * seperatly
				 */

				xdamage=0;
				prob=getuskill(count,25);
				if (psuedo_rand(300)<prob) {
					mobs[UC.fight].hp-=BONUSDAM;
					xdamage=1;
					dam+=BONUSDAM;
				}

				sprintf(text,"You ^g%s^N %s.[%d] %s%s\n",gettype(count,0),mobs[UC.fight].name,dam,bonus?bonstr:"",xdamage?"^Bxdam^N":"");
				wuser(count,text);
				sprintf(text,"%s ^g%s^N %s.[%d] %s%s\n",UC.name,gettypes(count,0),mobs[UC.fight].name,dam,bonus?bonstr:"",xdamage?"^Bxdam^N":"");
				wroom(UC.room,text,count);
				prompt(count);
			}
			else
			{
				sprintf(text,"You miss %s.\n",mobs[UC.fight].name);
				wuser(count,text);
				sprintf(text,"%s misses %s.\n",UC.name,mobs[UC.fight].name);
				wroom(UC.room,text,count);
			}

			if (UC.hp<=UC.wimpy) {
				flee(count);
				if (UC.fight==-1) return;
			}
			if ((mobs[UC.fight].wimpy>0) && (mobs[UC.fight].hp>0))
			{
				if((short)(mobs[UC.fight].maxhp*((float)mobs[UC.fight].wimpy/(float)100))>mobs[UC.fight].hp)
				{
					mobflee(UC.fight);
					if(UC.fight==-1) return;
				}
			}
			if (mobs[UC.fight].hp<=0) {
				corpse=-1;
				numdead=memkills(mobs[UC.fight].vnum);
				sprintf(text,"%s is DEAD!\n",mobs[UC.fight].name);
				wroom(UC.room,text,-1);
				areas[getarea(rooms[getroom(UC.room)].area)].kills++;
				for(loop=0; loop<MAXMOBS; loop++)
					if(glob.mobkills[loop].vnum==mobs[UC.fight].vnum)
					{
						glob.mobkills[loop].kills++;
						strcpy(glob.mobkills[loop].killer,UC.name);
					}

				/* Make the corpse */
				read_obj(CORPSE_OBJ);
				if (glob.curobj!=-1) {
					objs[glob.curobj].room=mobs[UC.fight].room;
					objs[glob.curobj].user=-1;
					objs[glob.curobj].mob=-1;
					sprintf(objs[glob.curobj].sdesc,"the corpse of %s",mobs[UC.fight].name);
					corpse=glob.curobj;
				}
				/* corpse done */
				storekill(count,mobs[UC.fight].vnum);
				fght=UC.fight; /* as UC.fight will get reset in the
					 loop
					 */
				funkydeath(count);
				for (count2=0;count2<MAXUSERS;count2++) {
					if (users[count2].socket<0) continue;
					if (users[count2].room!=UC.room) continue;
					if (users[count2].groupno!=UC.groupno) continue;
					if (users[count2].fight==-1) continue;
					if (users[count2].sleep==2) continue;
	
					xpa=0;
					mobcon=attval(fght,1)+defval(fght,1);
					if (mobcon>users[count2].maxcons+5) mobcon=users[count2].maxcons+5;
					if (users[count2].maxcons!=0) {
						xpa=((((mobcon)-(users[count2].maxcons))+5)*45)+psuedo_rand(100);
						users[count2].maxcons=0;
					}
					if (users[count2].lvl<5) xpa*=2;
	
					gmem=countgroup(count2);

					if (xpa>0) xpa=(int)(xpa/gmem);
					if ((xpa>0)&&(numdead>1)) xpa=(int)(xpa/numdead);
					if (xpa<0) xpa=0;
					if (xpa>0&&count2==count) UC.kills++;
					sprintf(text,"You receive %d experience points.\n",xpa);
					wuser(count2,text);
					adjxp(count2,xpa);
					// change alignment
					if (mobs[fght].align<users[count2].align) {
						users[count2].align-=xpa;
						if (users[count2].align<-1000) users[count2].align=-1000;
					}
					else
					{
						users[count2].align+=xpa;
						if (users[count2].align>1000) users[count2].align=1000;
					}

					users[count2].fight=-1;
				}
				fid=mobs[fght].vnum;
				mobs[fght].fight=-1;
				event(eMOBDIE,count,fght,-1);
				reset_mob(fght);
				if(UC.autoloot)
				{
					for(count2=0; count2<MAXOBJS; count2++)
					{
						if((objs[count2].mob_drop==fid) && (objs[count2].room==UC.room))
						{
							carryob=getnewheld(count);

							if (carryob==-1) {
								wuser(count,"You couldn't possibly carry anything else.\n");
							} else {

								objs[count2].room=-1;
								objs[count2].mob=-1;
								objs[count2].user=count;

								UC.held[carryob]=count2;

								sprintf(text,"You get a %s.\n",objs[count2].name);
								wuser(count, text);

								sprintf(text,"%s gets a %s.\n",UC.name,objs[count2].name);
								wroom(UC.room,text,count);
							}

						}
					}
				}
				if(UC.autosac)
				{
					short got,award;
					if(corpse!=-1)
					{
						sprintf(text,"You sacrifice %s.\n",objs[corpse].sdesc);
						wuser(count,text);
						sprintf(text,"%s sacrifices %s.\n",UC.name,objs[corpse].sdesc);
						wroom(UC.room,text,count);
						got=1;
						if (objs[corpse].type==10) got++; /* small bonus for trash */
						reset_obj(corpse);
						if (got) {
							award=(got*psuedo_rand(10))+2;
							UC.cash+=from_fund(award);
							sprintf(text,"^gThe GODs award you %d gold coins",award);
							wuser(count,text);
							if (psuedo_rand(500)<25) {
								award=psuedo_rand(10);
								adjxp(count,award);
								sprintf(text," and %d xp.\n",award);
								wuser(count,text);
							}
							else wuser(count,".\n");
						}
						corpse=-1;
					}
				}
					
			}	
		} /*end of multi attack */
			
	}
	for (count=0;count<MAXMOBS;count++) {
		/* AUTO MOVEMENT */
		if (MC.vnum==-1) continue;

		/* HOURLY TICK */
		if (glob.hr) {
			if (MC.fight!=-1) {
				MC.hp+=1;
			}
			else
			{
				MC.hp+=(int)(MC.maxhp*0.2);
			}
			if (MC.hp>MC.maxhp) MC.hp=MC.maxhp;
		}

		if ((!getmflag(count,mSTATIONARY))&&(MC.fight==-1)) {
			if (psuedo_rand(100)<MOBMOVE) {
				/* the git is on the move */
				sr=psuedo_rand(10)-1;
				if (rooms[getroom(MC.room)].dir[sr]>0) {
					movemob(count, sr);
				}
			}
		}

		/* AGGIE LOGIC */
		if ((MC.fight==-1)&&(getmflag(count,mAGGRES))&&(!getrflag(getroom(MC.room),PEACE))) {
			for (count2=0;count2<MAXUSERS;count2++) {
				if (users[count2].socket<0) continue;
				if (users[count2].rank==GOD) continue;
				if (users[count2].room!=MC.room) continue;
				if (users[count2].afk!=0) continue;
				if ((users[count2].lvl>MC.lvl)&&(!getmflag(count,mMADDEN))) continue;
				if (users[count2].sleep==2) continue;
				if ((users[count2].invis)&&(!getmflag(count,mSEEINVIS))) continue;
				if (users[count2].hidden>0) continue;
				MC.fight=count2;
				if (users[count2].fight==-1) users[count2].fight=count;
				users[count2].invis=0;
				users[count2].sleep=0;
				sprintf(text,"%s attacks you for no apparent reason.\n",cap1st(MC.name));
				wuser(MC.fight,text);
				sprintf(text,"%s attacks %s.\n",cap1st(MC.name),users[count2].name);
				wroom(MC.room,text,count2);
				break;
			}
		}
		/* MOB FIGHT */
		if (users[MC.fight].socket==-1) 
		{
			/* user logged off mid fight */
			MC.fight=next_victim(count2);
		}

		if (users[MC.fight].room!=MC.room)
		{
			if (users[MC.fight].socket!=-1) {
				if (users[MC.fight].fight==MC.vnum) users[MC.fight].fight=-1;
			}
			MC.fight=-1;
		}

		if (MC.fight==-1) continue;

		//godint(count,1);

		for (attack=0;attack<2;attack++) {
			if (MC.fight==-1) break;
			if (attack==1) {
				prob=psuedo_rand(100)-20;
				if (prob>MC.lvl) continue;
			}
			mcona=attval(count,1);
			pcond=defval(MC.fight,0);
			mcona+=psuedo_rand(10);
			pcond+=psuedo_rand(10);
			dam=0;
			hit=0;
			if (mcona>pcond) {
				dam=psuedo_rand(gethitindex(count,1));
				dam*=(mcona-pcond);
				if (dam<0) dam=0;
				users[MC.fight].hp-=dam;
				hit=1;
			}
			/* Dodge */
			if (hit) {
				prob=psuedo_rand(CHANCE)+((mcona-pcond)*2);
				prob=prob<0?0:prob;
				if (prob<getuskill(MC.fight,4)) {
					dodge=1;
					hit=0;
					users[MC.fight].hp+=dam;
				}
				else
				{
					dodge=0;
				}
			}
			/* Parry */
			if ((hit)&&(users[MC.fight].worn[11]!=-1)) {
				prob=psuedo_rand(CHANCE)+((mcona-pcond)*2);
				prob=prob<0?0:prob;
				if (prob<getuskill(MC.fight,5)) {
					parry=1;
					hit=0;
					users[MC.fight].hp+=dam;
				}
				else
				{
					parry=0;
				}
			}
			/* Block */
			if ((hit)&&(users[MC.fight].worn[10]!=-1)) {
				prob=psuedo_rand(CHANCE)+((mcona-pcond)*2);
				prob=prob<0?0:prob;
				if (prob<getuskill(MC.fight,10)) {
					block=1;
					hit=0;
					users[MC.fight].hp+=dam;
				}
				else
				{
					block=0;
				}
			}



			if (dodge) {
				sprintf(text,"You ^Bdodge^N a hit from %s.\n",MC.name);
				wuser(MC.fight,text);
				sprintf(text,"%s ^Bdodges^N a hit from %s.\n",users[MC.fight].name,MC.name);
				wroom(MC.room,text,MC.fight);
			}
			else if (parry) {
				sprintf(text,"You ^Bparry^N a hit from %s.\n",MC.name);
				wuser(MC.fight,text);
				sprintf(text,"%s ^Bparries^N a hit from %s.\n",users[MC.fight].name,MC.name);
				wroom(MC.room,text,MC.fight);
			}
			else if (block) {
				sprintf(text,"You ^Bblock^N a hit from %s.\n",MC.name);
				wuser(MC.fight,text);
				sprintf(text,"%s ^Bblocks^N a hit from %s.\n",users[MC.fight].name,MC.name);
				wroom(MC.room,text,MC.fight);
			}
			else if (hit) {
				sprintf(text,"%s ^r%s^N you.[%d (%d)]\n",MC.name,gettypes(count,1),dam,users[MC.fight].hp);
				wuser(MC.fight,text);
				sprintf(text,"%s ^r%s^N %s.\n",MC.name,gettypes(count,1),users[MC.fight].name);
				wroom(MC.room,text,MC.fight);
				prompt(MC.fight);
			}
			else
			{
				sprintf(text,"%s misses you.\n",MC.name);
				wuser(MC.fight,text);
				sprintf(text,"%s misses %s.\n",MC.name,users[MC.fight].name);
				wroom(MC.room,text,MC.fight);
			}

			if (users[MC.fight].hp<=0) {
				users[MC.fight].died++;
				users[MC.fight].lastdied=time(0);
				wuser(MC.fight,"You are DEAD!\n");
				sprintf(text,"%s has been killed by %s.\n",users[MC.fight].name,MC.name);
				wall(text,MC.fight);
				if (users[MC.fight].lvl<51) {
					xpa=(int)((users[MC.fight].xp-(users[MC.fight].lvl*1000))/2);
				}
				else
				{
					xpa=(int)((users[MC.fight].xp-50000)*HERODEATH);
					xpa+=500;
				}

				sprintf(text,"You have lost %dxp.\n",xpa);
				wuser(MC.fight,text);
				adjxp(MC.fight,-xpa);
				users[MC.fight].hp=1;
				users[MC.fight].fight=-1;
				users[MC.fight].room=guilds[getguild(users[MC.fight].guild)].recall_room;
				sprintf(text,"%s appears without warning.\n",users[MC.fight].name);
				wroom(users[MC.fight].room,text,MC.fight);
				/* find next victim */
				/* do it for all mobs which were attacking this Player*/
				fght=MC.fight;
				for (count2=0;count2<MAXMOBS;count2++) {
					if (mobs[count2].vnum==-1) continue;
					if (mobs[count2].fight!=fght) continue;
	
					mobs[count2].fight=next_victim(count2);
				}
				event(ePLAYERDIE,fght,count,-1);
			}	
		}
	}
}
