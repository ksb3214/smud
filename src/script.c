/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   :script.c
** Purpose :SMUD Scripts
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 08-08-02 0.00 ksb First Version
*/

#include "smud.h"

void stripspaces(char *str);
short parseline(char *str);
short s_def();
short s_cmp();
short s_say();
short s_jump(short);
void wroom(short,char *,short);
short getvariable(short);
short allocate_var(char *name, short type);
short getplayervar(short pn, short uid, char *var);
short getmobvar(short pn, short mid, char *var);
short setvar(char *vname, long ival, char *str);

struct scr_st inp;	// needs resetting before 'doscripts'

#define MAXVARS		100 // per script

// Variable/parameter types
#define STRING		1
#define INTEGER		2
#define FLOAT		3  // not used yet
#define MOB		4
#define OBJ		5
#define ROOM		6
#define PLAYER		7
#define AREA		8
#define GUILD		9
#define MAXTYPE		10 // always the last

struct param_st {
	short type; // STRING/INTEGER/FLOAT
	char str[4096];  // somewhat limited
	long ival;
	float fval;
	char vname[255]; // literal if not set
	// if vname is set then it will be stored after the instruction
	// has completed.
};

struct cmp_st {
	short equal;
	short notequal;
	short gt;
	short lt;
};

struct var_st {
	short type; // MOB/OBJ/ROOM/PLAYER/AREA/GUILD/STRING/INTEGER/FLOAT
	char str[4096];
	long ival;
	float fval;
	char vname[255];
};
	
// 20 maybe a little silly, but you never know ;)
// Just like word[] and like word_count it starts from 1 not 0
// params[0] is not used
struct param_st params[20];
short param_count;
char nextlabel[128];     // set this if you want execution to passover until label
short resetcount=-1;	// set this to change the current IC
char command[255];       // current command
struct cmp_st cmpflags;  // a really bad way to do it, but sodit
struct var_st vars[MAXVARS]; // max 100 vars per script should be enough

/* phew... ok... sequence of events..
 *
 * split the line into COMMAND Param0 Param1...ParamN
 * if the line is a label ignore it
 * if the line is an even ignore it
 * call the function for the command using a bunch of if's
 */
char *doscripts()
{
	short count;
	short found=0; // found event

	// quickly initialise all the variable slots
	for (count=0;count<MAXVARS;count++) {
		vars[count].type=-1;
	}

	if (allocate_var("player", PLAYER)<0) return NULL;
	if (setvar("player",inp.uid,NULL)<0) return NULL;
	if (allocate_var("mob", MOB)<0) return NULL;
	if (setvar("mob",inp.mid,NULL)<0) return NULL;
	if (allocate_var("object", OBJ)<0) return NULL;
	if (setvar("object",inp.oid,NULL)<0) return NULL;
	if (allocate_var("me", inp.metype)<0) return NULL;
	if (setvar("me",inp.me,NULL)<0) return NULL;

	nextlabel[0]='\0';
	for (count=0;count<inp.ln;count++) {
		stripspaces(inp.buf[count]);
//		printf("[%d] (%s)\n",count,inp.buf[count]);
		// get command
		if (strcmp(inp.event,inp.buf[count])==0) {
			found=1;
			continue;
		}

		if (!found) continue;
		if (inp.buf[count][0]==':') continue;

		// check for a label
		if (strncmp(inp.buf[count],"LBL:",4)==0) {
			if (strcmp(inp.buf[count]+4,nextlabel)==0) {
				//got needed label
				nextlabel[0]='\0';
				continue;
			}
		}

		// test for nextlabel, if it isnt clear we havent found it
		if (nextlabel[0]!='\0') continue;

		if (parseline(inp.buf[count])<0) {
			printf("Error around line %d.\n",count);
			return NULL;
		}

		// process commands, we'll use simple if's just to
		// make it really easy to add new stuff

		if (strcmp(command,"DEF")==0) 
			if (s_def()<0) return NULL;
		if (strcmp(command,"CMP")==0) 
			if (s_cmp()<0) return NULL;
		if (strcmp(command,"SAY")==0) 
			if (s_say()<0) return NULL;
		if (strcmp(command,"JE")==0) 
			if (s_jump(EQUAL)<0) return NULL;
		if (strcmp(command,"JNE")==0) 
			if (s_jump(NOTEQUAL)<0) return NULL;
		if (strcmp(command,"JGT")==0) 
			if (s_jump(GT)<0) return NULL;
		if (strcmp(command,"JLT")==0) 
			if (s_jump(LT)<0) return NULL;
		if (strcmp(command,"JMP")==0) 
			if (s_jump(NOTHING)<0) return NULL;

		if (strcmp(command,"EXIT")==0&&nextlabel[0]=='\0') return NULL;
		if (resetcount>-1) {
			count=resetcount;
			resetcount=-1;
		}
	}
	return NULL;
}

/* will find and allocate a variable slot, if there is no free slot it will
 * report an error and return -1
 */
short allocate_var(char *name, short type)
{
	short count;

	for (count=0;count<MAXVARS;count++) {
		if (vars[count].type!=-1) continue;
		/*
		if (strcmp(vars[count].vname,name)==0) {
			printf("Trying to reDEF a variable (%s)\n",name);
			return -1;
		}
		*/

		// found free var slot
		vars[count].type=type;
		strcpy(vars[count].vname,name);
		vars[count].str[0]='\0';
		vars[count].ival=0;
		vars[count].fval=0.0;
		return 0;
	}

	printf("No variable slots available, increase MAXVARS in script.c.\n");
	return -1;
}

short s_def()
{
	if (param_count!=2) {
		printf("DEF requires 2 parameters.\n");
		return -1;
	}

	if (params[1].type!=STRING) {
		printf("Variables are defined as STRINGS.\n");
		return -1;
	}

	if (params[2].type!=INTEGER) {
		printf("Variabe types must be integer.\n");
		return -1;
	}

	return allocate_var(params[1].str,params[2].ival);
}

short s_cmp()
{
	int scmp;
	cmpflags.equal=0;
	cmpflags.lt=0;
	cmpflags.gt=0;

	if (param_count!=2) {
		printf("CMP requires 2 parameters.\n");
		return -1;
	}

	if (params[1].type!=params[2].type) {
		printf("CMP parameters must be the same type.\n");
		return -1;
	}

	if (params[1].type==STRING) {
		scmp=strcmp(params[1].str,params[2].str);
		if (scmp==0) cmpflags.equal=1;
		if (scmp>0) cmpflags.gt=1;
		if (scmp<0) cmpflags.lt=1;
	}

	if (params[1].type==INTEGER) {
		if (params[1].ival==params[2].ival) cmpflags.equal=1;
		if (params[1].ival<params[2].ival) cmpflags.lt=1;
		if (params[1].ival>params[2].ival) cmpflags.gt=1;
	}
	
	return 0;
}

short s_say()
{
	short count;

	if (param_count<1) {
		printf("SAY needs a string.\n");
		return -1;
	}

	if (inp.metype!=MOB) {
		printf("Object needs to be a MOB.\n");
		return -1;
	}

	sprintf(text,"%s says '",mobs[inp.me].name);
	wroom(mobs[inp.me].room,text,-1);
	for (count=1;count<=param_count;count++) {
		if (params[count].type==STRING) {
			sprintf(text,"%s",params[count].str);
			wroom(mobs[inp.me].room,text,-1);
		}
		else
		{
			sprintf(text,"%ld",params[count].ival);
			wroom(mobs[inp.me].room,text,-1);
		}
	}	
	wroom(mobs[inp.me].room,"'\n",-1);
	return 0;
}

short s_jump(short flag)
{
	short jt=0; // if 1 then do the jump

	if (param_count!=1) {
		printf("JMP requires a label to jump to.\n");
		return -1;
	}	

	switch (flag) {
		case NOTHING: jt=1; break;
		case EQUAL: if (cmpflags.equal==1) jt=1; break;
		case NOTEQUAL: if (cmpflags.equal==0) jt=1; break;
		case GT: if (cmpflags.gt==1) jt=1; break;
		case LT: if (cmpflags.lt==1) jt=1; break;
		default: break;
	}

	if (jt) {
		sprintf(nextlabel,"\"%s\"",params[1].str);
		resetcount=0;
	}	

	return 0;
}

// command is upto the first colon, parameters follow and are sperated by
// commas
short parseline(char *str)
{
	char tmpstr[4096]; // lots of silly sized variables
	char tmpstr2[4096]; // lots of silly sized variables
	char *pos=NULL;
	short pn=0;

	// get command
	strcpy(tmpstr,str);
	pos=strchr(tmpstr,':');

	if (!pos) {
		printf("Malformed command\n");
		return -1;
	}

	*pos='\0';

	strcpy(command,tmpstr);

	pos++;

	strcpy(tmpstr,pos);

	while (*pos!='\0') {
		// check for a string
		if (tmpstr[0]=='"') {
			pos=strchr(tmpstr+1,'"');
			if (!pos) {
				printf("No closing quote on string.\n");
				return -1;
			}
			pn++;
			*pos='\0';
			pos++;
			if (*pos==',') pos++;
			strcpy(params[pn].str,tmpstr+1);
			params[pn].type=STRING;
			params[pn].vname[0]='\0';
			strcpy(tmpstr2,pos);
			strcpy(tmpstr,tmpstr2);
			pos=tmpstr;
			continue;
		}

		// check to see if its a variable name
		if (tmpstr[0]=='$') {
			pos=strchr(tmpstr,',');
			if (!pos) {
				pos=strchr(tmpstr,'\0');
			}
			else
			{
				*pos='\0';
				pos++;
			}
			pn++;
			// we dont need the $
			strcpy(params[pn].vname,tmpstr+1);
			if (getvariable(pn)<0) return -1;
			strcpy(tmpstr2,pos);
			strcpy(tmpstr,tmpstr2);
			pos=tmpstr;
			continue;
		}

		// so its either an int or a float, for now we're only gonna
		// support ints

		// for now assume an int
		pos=strchr(tmpstr,',');
		if (!pos) {
			pos=strchr(tmpstr,'\0');
		}
		*pos='\0';
		pn++;
		params[pn].ival=atol(tmpstr);
		params[pn].type=INTEGER;
		params[pn].vname[0]='\0';

		strcpy(tmpstr2,pos);
		strcpy(tmpstr,tmpstr2);
		pos=tmpstr;
	}

	param_count=pn;
	return 0;
}

// this will strip all spaces which arnt inverted comma's enclosed
// this isnt a problem from a command pov because all commands are
// followed by colons
void stripspaces(char *str)
{
	char tmp[4096];
	short count=0;
	short count2=0;
	short quotes=0; // are we in quoted text?

	for (count=0;str[count]!='\0';count++) {
		if (str[count]=='"') quotes=quotes==0?1:0;
		if (str[count]==' '&&quotes==0) {
			continue;
		}

		tmp[count2]=str[count];
		count2++;
		tmp[count2]='\0';
	}

	strcpy(str,tmp);
}

// Reads the contents of fname into inp.buf
void read_script(char *fname)
{
	FILE *fp;
	short ln=0;

	sprintf(text,"%s%s",SCRIPTDIR,fname);
	if (!(fp=fopen(text,"r"))) {
		printf("No such script!\n");
		return;
	}

	while(fgets(text,255,fp)) {
		if (text[0]=='\n') continue;
		if (text[0]=='#') continue;
		text[strlen(text)-1]='\0';
		strcpy(inp.buf[ln],text);
		ln++;
	}
	inp.ln=ln;

	fclose(fp);
}

// called anywhere in the code using a MACRO for ev (eventtype)
// all or none of the additional parameters can be set, if they are unset
// then -1 is used.
short event(char *ev,short uid, short mid, short oid)
{
	short count;
	/* first search mobs, then objects, then rooms */
	for (count=0;count<MAXMOBS;count++) {
		if (MC.vnum==-1) continue;
		if (MC.script[0]=='\0') continue;

		// get the script and process it
		read_script(MC.script); // sets inp.buf
		strcpy(inp.event,ev);
		inp.uid=uid;
		inp.mid=mid;
		inp.oid=oid;
		inp.me=count;
		inp.metype=MOB;

		doscripts();
	}

	for (count=0;count<MAXOBJS;count++) {
		if (OC.vnum==-1) continue;
		if (OC.name[0]=='\0') continue;
		if (OC.script[0]=='\0') continue;

		// get the script and process it
		read_script(OC.script); // sets inp.buf
		strcpy(inp.event,ev);
		inp.uid=uid;
		inp.mid=mid;
		inp.oid=oid;
		inp.me=count;
		inp.metype=OBJ;

		doscripts();
	}

	return 0;
}


// if a parameter is a variable this will fill the values
// I hope ;)
short getvariable(short pn)
{
	char var[255];
	char ext[255]; // anything after a period
	char *pt;
	short count=0;

	ext[0]='\0';
	strcpy(var,params[pn].vname);
	pt=strchr(var,'.');

	if (pt) {
		*pt='\0';
		pt++;
		strcpy(ext,pt);
	}

	// reset some bits
	params[pn].type=-1;
	params[pn].ival=-1;
	params[pn].str[0]='\0';

	for (count=0;count<MAXVARS;count++) {
		if (vars[count].type<0) continue;
		if (strcmp(vars[count].vname,var)==0) {
			params[pn].type=vars[count].type;
			if (vars[count].type==STRING) {
				strcpy(params[pn].str,vars[count].str);
			}
			else
			{
				params[pn].ival=vars[count].ival;
			}
			break;
		}
	}

	if (params[pn].type==-1) {
		printf("No such variable defined.\n");
		return -1;
	}

	if (ext[0]=='\0') {
		// return what we have
		return 0;
	}

	// okay we need to return the relevant index
	switch (params[pn].type) {
		case PLAYER: return getplayervar(pn, params[pn].ival, ext);
		case MOB: return getmobvar(pn, params[pn].ival, ext);
		// need more
		default: printf("No such index.\n"); return -1;
	}

	printf("Wierd error in variable get.\n");
	return -1;
}

/* this will set an already defined variable, if it has a ext part then it
 * will attempt to set the variable in the structure
 */
short setvar(char *vname, long ival, char *str)
{
	char var[255];
	char ext[255]; // anything after a period
	char *pt;
	short count=0;

	ext[0]='\0';
	strcpy(var,vname);
	pt=strchr(var,'.');

	if (pt) {
		*pt='\0';
		pt++;
		strcpy(ext,pt);
	}

	for (count=0;count<MAXVARS;count++) {
		if (vars[count].type<0) continue;
		if (strcmp(vars[count].vname,var)==0) {
			if (ext[0]=='\0') {
				if (vars[count].type==STRING) {
					strcpy(vars[count].str,str);
				}
				else
				{
					vars[count].ival=ival;
				}
			}
			else
			{
				printf("Not supported yet.\n");
			}
			return 0;
		}
	}

	printf("No such variable.\n");
	return -1;
}

// var will actually be the bit after the period
short getplayervar(short pn, short uid, char *var)
{
	/* Group all integers first
	 */
	params[pn].type=INTEGER;

	if (strcmp(var,"room")==0) {
		params[pn].ival=users[uid].room;
		return 0;
	}

	if (strcmp(var,"lvl")==0) {
		params[pn].ival=users[uid].lvl;
		return 0;
	}

	if (strcmp(var,"maxlvl")==0) {
		params[pn].ival=users[uid].maxlvl;
		return 0;
	}

	if (strcmp(var,"rank")==0) {
		params[pn].ival=users[uid].rank;
		return 0;
	}

	if (strcmp(var,"hp")==0) {
		params[pn].ival=users[uid].hp;
		return 0;
	}

	if (strcmp(var,"man")==0) {
		params[pn].ival=users[uid].man;
		return 0;
	}

	if (strcmp(var,"mv")==0) {
		params[pn].ival=users[uid].mv;
		return 0;
	}

	if (strcmp(var,"maxhp")==0) {
		params[pn].ival=users[uid].maxhp;
		return 0;
	}

	if (strcmp(var,"maxman")==0) {
		params[pn].ival=users[uid].maxman;
		return 0;
	}

	if (strcmp(var,"maxmv")==0) {
		params[pn].ival=users[uid].maxmv;
		return 0;
	}

	if (strcmp(var,"xp")==0) {
		params[pn].ival=users[uid].xp;
		return 0;
	}

	if (strcmp(var,"sex")==0) {
		params[pn].ival=users[uid].sex;
		return 0;
	}

	if (strcmp(var,"align")==0) {
		params[pn].ival=users[uid].align;
		return 0;
	}

	if (strcmp(var,"idle")==0) {
		params[pn].ival=users[uid].idle;
		return 0;
	}

	if (strcmp(var,"room")==0) {
		params[pn].ival=users[uid].room;
		return 0;
	}

	/* now the strings
	 */
	params[pn].type=STRING;

	if (strcmp(var,"name")==0) {
		strcpy(params[pn].str,users[uid].name);
		return 0;
	}

	return -1;
}

// var will actually be the bit after the period
short getmobvar(short pn, short mid, char *var)
{
	/* Group all integers first
	 */
	params[pn].type=INTEGER;

	if (strcmp(var,"room")==0) {
		params[pn].ival=mobs[mid].room;
		return 0;
	}

/*
	if (strcmp(var,"lvl")==0) {
		params[pn].ival=users[uid].lvl;
		return 0;
	}

	if (strcmp(var,"maxlvl")==0) {
		params[pn].ival=users[uid].maxlvl;
		return 0;
	}

	if (strcmp(var,"rank")==0) {
		params[pn].ival=users[uid].rank;
		return 0;
	}

	if (strcmp(var,"hp")==0) {
		params[pn].ival=users[uid].hp;
		return 0;
	}

	if (strcmp(var,"man")==0) {
		params[pn].ival=users[uid].man;
		return 0;
	}

	if (strcmp(var,"mv")==0) {
		params[pn].ival=users[uid].mv;
		return 0;
	}

	if (strcmp(var,"maxhp")==0) {
		params[pn].ival=users[uid].maxhp;
		return 0;
	}

	if (strcmp(var,"maxman")==0) {
		params[pn].ival=users[uid].maxman;
		return 0;
	}

	if (strcmp(var,"maxmv")==0) {
		params[pn].ival=users[uid].maxmv;
		return 0;
	}

	if (strcmp(var,"xp")==0) {
		params[pn].ival=users[uid].xp;
		return 0;
	}

	if (strcmp(var,"sex")==0) {
		params[pn].ival=users[uid].sex;
		return 0;
	}

	if (strcmp(var,"align")==0) {
		params[pn].ival=users[uid].align;
		return 0;
	}

	if (strcmp(var,"idle")==0) {
		params[pn].ival=users[uid].idle;
		return 0;
	}

	if (strcmp(var,"room")==0) {
		params[pn].ival=users[uid].room;
		return 0;
	}
*/
	/* now the strings
	 */
	 /*
	params[pn].type=STRING;

	if (strcmp(var,"name")==0) {
		strcpy(params[pn].str,users[uid].name);
		return 0;
	}
	*/

	return -1;
}

