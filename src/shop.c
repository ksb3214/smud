/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :shop.c
** Purpose :Shop oriented commands
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 09-12-01 0.00 ksb First Version
*/

#include "smud.h"

extern short getnewheld(SU);
extern void wroom(short, char *, short);
extern short getoflag(short, short);
extern short getheldobj(SU,char *);
extern void wuser(SU, char *);
extern short getmflag(short, short);
extern void stolower(char *);
extern void read_obj(short);
long calc_price(short vnum);

/* calculates sale price
 */
long saleprice(short oid, short mid)
{
	double curprice;
	short numinstock=0;
	short count;

	curprice=(double)calc_price(objs[oid].vnum);

//	printf("curprice %f\n",curprice);
	for (count=0;count<MAXOBJS;count++) {
		if (OC.name[0]=='\0') continue;
		if (OC.vnum!=objs[oid].vnum) continue;
		if (OC.forsale==0) continue;
		if (OC.mob!=mobs[mid].vnum) continue;

		numinstock++;
	}

	for (count=0;count<numinstock;count++) {
		curprice*=0.50;
//		printf("curprice %f\n",curprice);
	}

	if (curprice<0.0) curprice=0.0;

	return (long)curprice;
}

/* loadstock will find a mob vnum and the stock associated with it
 * it will then simply load the object and assing it to the mob forsale
 */
// mobs array position (not vnum) passed as mid
void loadstock(short mid)
{
	short loop;
	smud_file *stockfile;
	char newstr[8192];
	char *strptr,*strptr2;
	short objno;

	DEBUG("loadstock")

//	printf("trying to load for %s (%d)\n",mobs[mid].name,mobs[mid].vnum);
	stockfile=read_file("../other/stock.mff");
	if (stockfile->vnumcount==0) 
	{ 
		// no extra stock
		freefile(stockfile);
		return;
	}

	for(loop=0; loop<stockfile->vnumcount; loop++)
	{
		if (stockfile->vnumlist[loop]==mobs[mid].vnum) {
			sprintf(newstr,getval(stockfile,stockfile->vnumlist[loop],"items"));
			strptr=newstr;
			while(strptr) {
				objno=atoi(strptr);
				if (objno==0) break;
				read_obj(objno);
				if (glob.curobj!=-1) {
					objs[glob.curobj].room=-1;
					objs[glob.curobj].user=-1;
					objs[glob.curobj].mob=mobs[mid].vnum;
					objs[glob.curobj].forsale=1;
				}
				strptr2=strptr;
				strptr=strchr(strptr2,'x');
				if (strptr) strptr++;
			}

		}
	}
	freefile(stockfile);
}

/* savestock will examine the stock of each shopkeeper and save any non
 * basic stock goods i.e. stuff which has been sold to them from
 * scavaging
 */
void savestock()
{
	short mid,count;
	short saved=0,this=0;
	char tmpstr[8192]; // shitloads of objects
	smud_file *stockfile;

	DEBUG("savestock")
	
	// first remove the stock file as if there is any stock to be had
	// it will recreate it
	
	unlink("../other/stock.mff");
	
	stockfile=new_file();
	for (mid=0;mid<MAXMOBS;mid++) {
		if (mobs[mid].vnum==-1) continue;
		if (!getmflag(mid,mSHOPKEEPER)) continue;

		// okay we have a shopkeeper, now what does he have

		this=0;
		tmpstr[0]='\0';
		for (count=0;count<MAXOBJS;count++) {
			if (OC.name[0]=='\0') continue;
			if (OC.forsale!=1) continue;
			if (OC.mob!=mobs[mid].vnum) continue;

			// Object is being sold by shopkeeper
			if (OC.originalstock) continue;

			// Object has been sold to the mob so save
			saved=1;  // as we have at least one item now
			this=1;
			if (tmpstr[0]!='\0') {
				strcat(tmpstr,"x");
			}
			sprintf(text,"%d",OC.vnum);
			strcat(tmpstr,text);
		}
		if (this) {
			add_entry(stockfile,mobs[mid].vnum,"items",tmpstr);
		}
	}

	if (saved) {
		write_file(stockfile,"../other/stock.mff");
	}
	freefile(stockfile);

}

short getshopobj(short mid,char *str)
{
	short count;
	char needle[4096];
	char haystack[4096];

	DEBUG("getshopobj")

	for (count=0;count<MAXOBJS;count++) {
		if (OC.name[0]=='\0') continue;
		if (OC.forsale!=1) continue;
		if (OC.mob!=mobs[mid].vnum) continue;
		

		sprintf(needle,"%s",str);
		sprintf(haystack,"%s",OC.name);
		stolower(needle);
		stolower(haystack);
		if (strstr(haystack,needle)) {
			return count;
		}
		sprintf(haystack,"%s",OC.sdesc);
		stolower(haystack);
		if (strstr(haystack,needle)) {
			return count;
		}
	}
	return -1;
}

short getshop(SU)
{
	short count;

	DEBUG("getshop")

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.room!=UU.room) continue;
		if (!getmflag(count,mSHOPKEEPER)) continue;

		return count;
	}

	return -1;
}

short isopen(SU)
{
	return 1;
/*
	if (glob.gamehour<6||glob.gamehour>19) {
		wuser(uid,"This shop is closed at this hour.\n");
		return 0;
	}
*/
	return 1;
}

void list(SU)
{
	short mid;
	short found=0;
	short count=0;
	short shop[1024];
	short quan[1024];
	short num=0;
	short got=0;
	short count2=0;

	DEBUG("list")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	mid=getshop(uid);

	if (mid==-1) {
		wuser(uid,"There is no shopkeeper present.\n");
		return;
	}

	if (!isopen(uid)) return;

	sprintf(text,"%s is currently selling...\n",mobs[mid].name);
	wuser(uid,text);
	for (count=0;count<MAXOBJS;count++) {
		if (OC.name[0]=='\0') continue;
		if (OC.forsale==0) continue;
		if (OC.mob!=mobs[mid].vnum) continue;

		/* okay the object is forsale
		 */
		got=0;
		for (count2=0;count2<num;count2++) {
			if (shop[count2]==OC.vnum) {
				quan[count2]++;
				got=1;
			}
		}
		if (got) continue;
		shop[num]=OC.vnum;
		quan[num]=1;
		num++;
	}
	for (count=0;count<num;count++) {
		for (count2=0;count2<MAXOBJS;count2++) {
			if (objs[count2].vnum==0) continue;
			if (objs[count2].forsale!=1) continue;
			if (objs[count2].mob!=mobs[mid].vnum) continue;
			if (objs[count2].vnum==shop[count]) {
				sprintf(text,"   %s for %ld gold coins.",objs[count2].sdesc,calc_price(objs[count2].vnum));
				wuser(uid,text);
				if (quan[count]>1) {
					sprintf(text," (%d left)\n",quan[count]);
				}
				else
				{
					sprintf(text,"\n");
				}
				wuser(uid,text);
				found++;
				shop[count]=-1;
			}
		}
	}

	if (!found) {
		wuser(uid,"   Nothing at all.\n");
	}
}

void sell(SU)
{
	short mid;
/*	short found=0; - Not used yet */
	short obj;
	long value;

	DEBUG("sell")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	mid=getshop(uid);

	if (mid==-1) {
		wuser(uid,"There is no shopkeeper present.\n");
		return;
	}

	if (!isopen(uid)) return;

	if (word_count<1) {
		wuser(uid,"Sell what?\n");
		return;
	}

	obj=getheldobj(uid,comstr);

	if (obj==-1) {
		wuser(uid,"You cannot sell what you do not have.\n");
		return;
	}

	if (getoflag(UU.held[obj],oNODROP)) {
		wuser(uid,"You can't drop that, so you can't sell it either.\n");
		return;
	}

	value=saleprice(UU.held[obj],mid);
	if (value<0) value=0;

	if (value==0) {
		sprintf(text,"%s says 'Just bin it, it isnt worth anything to me.'\n",mobs[mid].name);
		wuser(uid,text);
		return;
	}
	
	if (mobs[mid].cash<value) {
		sprintf(text,"%s hasn't got the money for that.\n",mobs[mid].name);
		wuser(uid,text);
		return;
	}

	sprintf(text,"You sell %s to %s for %d gold coins.\n",objs[UU.held[obj]].sdesc,mobs[mid].name,(int)value);
	wuser(uid,text);
	sprintf(text,"%s sells %s to %s.\n",UU.name,objs[UU.held[obj]].sdesc,mobs[mid].name);
	wroom(UU.room,text,uid);

	objs[UU.held[obj]].forsale=1;
	objs[UU.held[obj]].user=-1;
	objs[UU.held[obj]].room=-1;
	objs[UU.held[obj]].mob=mobs[mid].vnum;
	UU.held[obj]=-1;
	mobs[mid].cash-=value;
	UU.cash+=value;
	
}

void value(SU)
{
	short mid;
/*	short found=0; - Not used yet */
	short obj;
	long value;

	DEBUG("value")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	mid=getshop(uid);

	if (mid==-1) {
		wuser(uid,"There is no shopkeeper present.\n");
		return;
	}

	if (!isopen(uid)) return;

	if (word_count<1) {
		wuser(uid,"Value what?\n");
		return;
	}

	obj=getheldobj(uid,comstr);

	if (obj==-1) {
		wuser(uid,"You cannot sell what you do not have.\n");
		return;
	}

	if (getoflag(UU.held[obj],oNODROP)) {
		wuser(uid,"You can't drop that, so you can't sell it either.\n");
		return;
	}

	value=saleprice(UU.held[obj],mid);
	if (value<0) value=0;
	
	if (value==0) {
		sprintf(text,"%s says 'it isnt worth anything to me.'\n",mobs[mid].name);
		wuser(uid,text);
		return;
	}
	
	if (mobs[mid].cash<value) {
		sprintf(text,"%s hasn't got the money for that.\n",mobs[mid].name);
		wuser(uid,text);
		return;
	}

	sprintf(text,"%s values it at %d gold coins.\n",mobs[mid].name,(int)value);
	wuser(uid,text);
}

void buy(SU)
{
	short mid;
/*	short found=0; - Not used yet */
	short obj;
	long value;
	short count=0;

	DEBUG("buy")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	mid=getshop(uid);

	if (mid==-1) {
		wuser(uid,"There is no shopkeeper present.\n");
		return;
	}

	if (!isopen(uid)) return;

	if (word_count<1) {
		wuser(uid,"buy what?\n");
		return;
	}

	/* this returns the array index to object */
	obj=getshopobj(mid,comstr);

	if (obj==-1) {
		wuser(uid,"None of that in stock at the moment.\n");
		return;
	}

	value=calc_price(objs[obj].vnum);

	if (UU.cash<value) {
		wuser(uid,"You do not have enough funds.\n");
		return;
	}

	count=getnewheld(uid);

	if (count==-1) {
		wuser(uid,"You cannot possibly carry anything else.\n");
		return;
	}

	UU.held[count]=obj;

	objs[obj].forsale=0;
	objs[obj].mob=-1;
	objs[obj].room=-1;
	objs[obj].user=uid;

	sprintf(text,"You buy %s for %d gold coins.\n",objs[obj].sdesc,(int)value);
	wuser(uid,text);

	sprintf(text,"%s buys something.\n",UU.name);
	wroom(UU.room,text,uid);

	UU.cash-=value;
	mobs[mid].cash+=value;
}

