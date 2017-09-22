/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   :commun.c
** Purpose :All the communication functions
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include "smud.h"

extern void swall(char *, short);
extern short getroom(short);
extern short getrflag(short, short);
extern short getuser(SU,char *);
extern void wroom(short, char *, short);
extern void wgroup(SU, char *,short);
extern void wuser(SU, char *);
extern void unhide(SU);

void tell(SU);

void gsay(SU)
{

	DEBUG("gsay")
        if (!*comstr) {
                wuser(uid,"GroupSay what?\n");
                return;
        }

        sprintf(text,"^B[Group] %s says '%s'^N\n",UU.name,comstr);
        wgroup(uid,text,-1);
}

void gemote(SU)
{
	DEBUG("gemote")
	if (word_count<1) {
		wuser(uid,"GroupEmote what?\n");
		return;
	}

	sprintf(text,"^B[Group] %s%s%s^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
	wgroup(uid,text,-1);
}

void emote(SU)
{
	DEBUG("emote")
	if (word_count<1) {
		wuser(uid,"Emote what?\n");
		return;
	}
	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	unhide(uid);
	sprintf(text,"%s%s%s\n",UU.name,comstr[0]=='\''?"":" ",comstr);
	wroom(UU.room,text,-1);
	event(ePLAYEREMOTE,uid,-1,-1);
}

void ask(SU)
{
	short target;

	DEBUG("ask")

	if (word_count<1) {
		wuser(uid,"Ask what of whom?\n");
		return;
	}

/*	if (get_spirit(word[1])) {
		spirit_ask(uid);
		return;
	}
*/

	target=getuser(uid,word[1]);

	if (target==-1) {
		wuser(uid,"Who?\n");
		return;
	}

	if (word_count<2) {
		wuser(uid,"Ask them what exactly?\n");
		return;
	}

	if (comstr[strlen(comstr)-1]!='?') {
		strcat(comstr,"?");
	}

	tell(uid);
}

void tell(SU)
{
	short target;
	char exp[255];
	char rec[255];

	DEBUG("tell")

	if (word_count<1) {
		wuser(uid,"Tell what to whom?\n");
		return;
	}

	target=getuser(uid,word[1]);

	if (target==-1) {
		wuser(uid,"Who?\n");
		return;
	}

	if (word_count<2) {
		wuser(uid,"Tell them what exactly?\n");
		return;
	}

	switch(comstr[strlen(comstr)-1]) {
		case '?' : strcpy(exp,"You ask");
				   strcpy(rec,"asks you");
				   break;
		case '!' : strcpy(exp,"You exclaim");
				   strcpy(rec,"exclaims to you");
				   break;
		default  : strcpy(exp,"You tell");
				   strcpy(rec,"tells you");
				   break;
	}

	if (users[target].afk!=0) {
		wuser(uid,"The individual in question is currently AFK and may not respond.\n");
	}
	sprintf(text,"%s %s '%s'\n", exp, users[target].name,strchr(comstr,' ')+1);
	wuser(uid,text);
	sprintf(text,"^H%s %s '%s'^N\n",UU.name, rec, strchr(comstr,' ')+1);
	wuser(target,text);
}

void remote(SU)
{
	short target;

	DEBUG("pemote")

	if (word_count<1) {
		wuser(uid,"Remote what to whom?\n");
		return;
	}

	target=getuser(uid,word[1]);

	if (target==-1) {
		wuser(uid,"Who?\n");
		return;
	}

	if (word_count<2) {
		wuser(uid,"Remote them what exactly?\n");
		return;
	}

	if (users[target].afk!=0) {
		wuser(uid,"The individual in question is currently AFK and may not respond.\n");
	}
	sprintf(text,"You emote to %s: %s%s%s\n",users[target].name,UU.name,*(strchr(comstr,' ')+1)=='\''?"":" ",strchr(comstr,' ')+1);
	wuser(uid,text);
	sprintf(text,"^H> %s%s%s^N\n",UU.name, *(strchr(comstr,' ')+1)=='\''?"":" ", strchr(comstr,' ')+1);
	wuser(target,text);
}

void say(SU)
{

	DEBUG("say")
        if (!*comstr) {
                wuser(uid,"Say what?\n");
                return;
        }
	if (UU.sleep==2) {
		wuser(uid,"Try waking up first.\n");
		return;
	}

	unhide(uid);
        sprintf(text,"You say '%s'\n",comstr);
        wuser(uid,text);
        sprintf(text,"%s says '%s'\n",UU.name,comstr);
        wroom(UU.room,text,uid);
	event(ePLAYERSAY,uid,-1,-1);
}

void chat(SU)
{
	DEBUG("chat")

        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"Chat what?\n");
                return;
        }

        sprintf(text,"^gYou chat '%s'^N\n",comstr);
        wuser(uid,text);
        sprintf(text,"^G%s chats '%s'^N\n",UU.name,comstr);
        swall(text,uid);
}

void chem(SU)
{
	DEBUG("chem")
        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"Chat-emote what?\n");
                return;
        }

        sprintf(text,"^gTo chatters, '%s%s%s'^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        wuser(uid,text);
        sprintf(text,"^G.%s%s%s^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        swall(text,uid);
}

void question(SU)
{
	DEBUG("question")

        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"What is your question?\n");
                return;
        }

        sprintf(text,"^gYou question '%s'^N\n",comstr);
        wuser(uid,text);
        sprintf(text,"^G%s questions '%s'^N\n",UU.name,comstr);
        swall(text,uid);
}

void qemote(SU)
{
	DEBUG("qemote")
        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"qemote what?\n");
                return;
        }

        sprintf(text,"^gTo questions, '%s%s%s'^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        wuser(uid,text);
        sprintf(text,"^G.%s%s%s (question)^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        swall(text,uid);
}

void answer(SU)
{
	DEBUG("answer")

        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"What is your answer?\n");
                return;
        }

        sprintf(text,"^gYou answer '%s'^N\n",comstr);
        wuser(uid,text);
        sprintf(text,"^G%s answers '%s'^N\n",UU.name,comstr);
        swall(text,uid);
}

void aemote(SU)
{
	DEBUG("qemote")
        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"aemote what?\n");
                return;
        }

        sprintf(text,"^gTo answer, '%s%s%s'^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        wuser(uid,text);
        sprintf(text,"^G.%s%s%s (answer)^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        swall(text,uid);
}

void driv(SU)
{
	DEBUG("driv")

        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"Drivel what?\n");
                return;
        }

        sprintf(text,"^mYou drivel '%s'^N\n",comstr);
        wuser(uid,text);
        sprintf(text,"^M%s drivels '%s'^N\n",UU.name,comstr);
        swall(text,uid);
}

void drem(SU)
{
	DEBUG("drem")
        if (getrflag(getroom(UU.room),SOUND)) {
                wuser(uid,"The room you are in muffles the sound..\n");
                return;
        }

        if (*comstr=='\0') {
                wuser(uid,"Drivel-emote what?\n");
                return;
        }

        sprintf(text,"^mTo drivelers, '%s%s%s'^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        wuser(uid,text);
        sprintf(text,"^M.%s%s%s^N\n",UU.name,comstr[0]=='\''?"":" ",comstr);
        swall(text,uid);
}
