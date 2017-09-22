/* SMUD - Slight MUD
** (c) 2000-2003, Karl Bastiman, Janet Hyde
**
** Title   :atm.c
** Purpose :Teller machines and general banking
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 05-07-00 0.00 ksb First Version
*/

#include "smud.h"

extern short getoflag(short, short);
extern void line(SU,char *);
extern short isoffline(char *);
extern short getuser(SU,char *);
extern void wuser(SU, char *);


char mstr[30];

unsigned long from_fund(unsigned long amount);

double calcint()
{
	return INTEREST;
}

unsigned long getfund()
{
	FILE *fp;
	unsigned long fund=0;

	sprintf(text,"%s",FUNDFILE);
	if (!(fp=fopen(text,"r"))) {
		printf("no fund.def file\n");
		return 0;
	}

	fgets(text,255,fp);

	fund=atol(text);

	fclose(fp);

	return fund;
}

void putfund(unsigned long fund)
{
	FILE *fp;

	sprintf(text,"%s",FUNDFILE);
	if (!(fp=fopen(text,"w"))) {
		printf("no fund.def file\n");
		return;
	}

	fprintf(fp,"%ld\n#end of file\n",fund);

	fclose(fp);
}

unsigned long usercash(SU)
{
	struct dirent **namelist;
	int n;
	long usr=0;
	unsigned long count=0;

	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory read.\n");
		return 0;
	}

	/* now scroll through all sorted entries */

	while (n--)
	{
		if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
			usr=getuser(uid,namelist[n]->d_name);
			if (usr==-1) {
				// user offline
				usr=isoffline(namelist[n]->d_name);
				if (usr==-1) {
					printf("borked userfile (%s)\n",namelist[n]->d_name);
					return 0;
				}
			}
			/* user stuff goes here */
			count+=users[usr].cash;
			count+=users[usr].bank;
		}
		free(namelist[n]);
	}
	free(namelist);

	return count;
}

char *richest(SU)
{
	struct dirent **namelist;
	int n;
	long usr=0;
	unsigned long mcash=0;
	unsigned long count=0;

	strcpy(mstr,"no-one yet!");
	n=scandir(USERDIR, &namelist, 0, alphasort);
	if (n < 0)
	{
		wuser(uid, "Error on directory read.\n");
		return (char *)0;
	}

	/* now scroll through all sorted entries */

	while (n--)
	{
		if ((strcmp(namelist[n]->d_name,"userlists.def")!=0)&&(!(strchr(namelist[n]->d_name,'.')))) {
			usr=getuser(uid,namelist[n]->d_name);
			if (usr==-1) {
				// user offline
				usr=isoffline(namelist[n]->d_name);
				if (usr==-1) {
					printf("borked userfile (%s)\n",namelist[n]->d_name);
					return (char *)0;
				}
			}
			/* user stuff goes here */
			count=users[usr].cash+users[usr].bank;
			if (count>mcash) {
				strcpy(mstr,users[usr].name);
				mcash=count;
			}
		}
		free(namelist[n]);
	}
	free(namelist);

	return mstr;
}

unsigned long mobcash()
{
	short count;
	unsigned long cash=0;

	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		cash+=MC.cash;
	}

	return cash;
}

void finance(SU)
{
	line(uid,"Smud financial statement");
	sprintf(text,"Total users funds : %ld\n",usercash(uid));
	wuser(uid,text);
	sprintf(text,"Total mob funds   : %ld\n",mobcash());
	wuser(uid,text);
	sprintf(text,"Treasury          : %ld\n",getfund());
	wuser(uid,text);
	sprintf(text,"Richest Player    : %s\n",richest(uid));
	wuser(uid,text);
	sprintf(text,"Interest Rate     : %-5.2f%%\n",calcint()*100);
	wuser(uid,text);
	line(uid,NULL);
}

/* accumulates saved money per game hour */
void average_bank()
{
	short count;

	DEBUG("average_bank")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;

		UC.avebank+=UC.bank;
	}
}

/* add interest at the start of each new day */
void add_interest()
{
	short count;
	short interest=0;

	DEBUG("add_interest")

	for (count=0;count<MAXUSERS;count++) {
		if (UC.socket<0) continue;
		if (UC.bank<100) continue;

		interest=(int)((UC.avebank/INTDAYS)*calcint());

		UC.avebank=0;

		if (interest<1) interest=1;

		UC.bank+=from_fund(interest);

		UC.lastint=interest;
//		sprintf(text,"You have gained ^Y%d gold coins^N in interest this week.\n",interest);
//		if (time(0)-UC.idle<AUTOSAVE) wuser(count,text);
	}
}

/* Pass a room vnum */
short is_atm(short vnum)
{
	short count;

	DEBUG("is_atm")

	for (count=0;count<MAXOBJS;count++) {
		if (OC.vnum==-1) continue;
		if (OC.room!=vnum) continue;
		if (getoflag(count,oATM)) return 1;
	}

	return 0;
}

void withdraw(SU)
{
	unsigned long amount=0;

	DEBUG("withdraw")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (!is_atm(UU.room)) {
		wuser(uid,"I guess you better find an ATM.\n");
		return;
	}
		
	if (word_count<1) {
		wuser(uid,"Specify how much you wish to withdraw.\n");
		return;
	}

	amount=atoi(comstr);
	if(!strcasecmp(comstr,"all"))
		amount=UU.bank;

	if (amount==0) {
		wuser(uid,"Just grab the air if you want nothing.\n");
		return;
	}

	if (amount<0) {
		wuser(uid,"Try using the 'deposit' command.\n");
		return;
	}

	if (amount>UU.bank) {
		wuser(uid,"Your account isn't that flush.\n");
		return;
	}

	UU.bank-=amount;
	UU.cash+=amount;

	sprintf(text,"You have withdrawn %d coins from your personal account.\n",(int)amount);
	wuser(uid,text);
}

void deposit(SU)
{
	unsigned long amount=0;

	DEBUG("deposit")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (!is_atm(UU.room)) {
		wuser(uid,"I guess you better find an ATM.\n");
		return;
	}
		
	if (word_count<1) {
		wuser(uid,"Specify how much you wish to deposit.\n");
		return;
	}

	amount=atoi(comstr);
	if(!strcasecmp(comstr,"all"))
		amount=UU.cash;

	if (amount==0) {
		wuser(uid,"A nice gesture but it's still nothing.\n");
		return;
	}

	if (amount<0) {
		wuser(uid,"Try using the 'withdraw' command.\n");
		return;
	}

	if (amount>UU.cash) {
		wuser(uid,"You don't have that kind of cash on you.\n");
		return;
	}

	UU.bank+=amount;
	UU.cash-=amount;

	sprintf(text,"You have deposited %d coins into your personal account.\n",(int)amount);
	wuser(uid,text);
}

void balance(SU)
{
	DEBUG("balance")

	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	if (!is_atm(UU.room)) {
		wuser(uid,"I guess you better find an ATM.\n");
		return;
	}

	sprintf(text,"You currently have a balance of: %d gold coins.\n",(int)UU.bank);
	wuser(uid,text);
	if (UU.lastint>0) {
		sprintf(text,"Your last interest payment was %d gold coins.\n",(int)UU.lastint);
	}
	else
	{
		sprintf(text,"The bank hasn't paid you any interest recently.\n");
	}
	wuser(uid,text);
}

void adjust_fund(unsigned long by)
{
	putfund(getfund()+by);
}

void to_fund(unsigned long amount)
{
	adjust_fund(amount);
}

unsigned long from_fund(unsigned long amount)
{
	adjust_fund(amount-(amount*2));
	return amount;
}

