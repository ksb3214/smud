/* SMUD - Slight MUD
** (c) 2000, 2001, 2002, 2003  Karl Bastiman, Janet Hyde
**
** Title   : settings.c
** Purpose : General user settings.
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 24-06-00 0.00 ksb First Version
*/

#include <smud.h>

extern void line(SU,char *);
extern void wuser(SU,char *);

char *settings[] = {
	"nothing good will ever come of this I can tell you.",
	"term","email","justify","defterm","defprec","autoexit","autoloot","autosac",
	"iacga","prompt","cprompt","noswear","colour","brief",
	"*"
};

enum sets {
	sNoThInG_gOoOoOoD,
	sTERM,sEMAIL,sJUSTIFY,sDEFTERM,sDEFPREC,sAUTOEXIT,sAUTOLOOT,sAUTOSAC,
	sIACGA,sPROMPT,sCPROMPT,sNOSWEAR,sCOLOUR,sBRIEF,
	sEND_SETTINGS
} setval;

void xset(SU)
{
	short count;
	
	if(word_count<2)
	{
		wuser(uid,"Usage: set [item] [value]\n");
		return;
	}

	setval=0;
	for(count=0; count<sEND_SETTINGS; count++)
		if(!strncasecmp(word[1],settings[count],strlen(word[1])))
			setval=count;
	switch(setval)
	{
		case sTERM	: 
			strcpy(UU.terminal.type,word[2]);
			sprintf(text,"Terminal type set to: %s\n",UU.terminal.type);
			wuser(uid,text);
			break;
		case sDEFTERM	: 
			strcpy(UU.defterm,word[2]);
			sprintf(text,"Default terminal type set to: %s\n",UU.defterm);
			wuser(uid,text);
			break;
		case sEMAIL	: 
			strcpy(UU.email,word[2]);
			sprintf(text,"Email addresss set to: %s\n",UU.email);
			wuser(uid,text);
			break;
		case sJUSTIFY	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.justify=1;
				wuser(uid,"Justification now on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.justify=0;
				wuser(uid,"Justification now off.\n");
				break;
			}
			wuser(uid,"Invalid value for justification.  Use 'on' or 'off'.\n");
			break;
		case sDEFPREC	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.defprec=1;
				wuser(uid,"Default terminal will now take precedence.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.defprec=0;
				wuser(uid,"Default terminal will only be used if system probe fails.\n");
				break;
			}
			wuser(uid,"Invalid value for default precedence.  Use 'on' or 'off'.\n");
			break;
		case sAUTOEXIT	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.autoexit=1;
				wuser(uid,"Autoexit on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.autoexit=0;
				wuser(uid,"Autoexit off.\n");
				break;
			}
			wuser(uid,"Invalid value for autoexit.  Use 'on' or 'off'.\n");
			break;
		case sAUTOLOOT	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.autoloot=1;
				wuser(uid,"Autoloot on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.autoloot=0;
				wuser(uid,"Autoloot off.\n");
				break;
			}
			wuser(uid,"Invalid value for autoloot.  Use 'on' or 'off'.\n");
			break;
		case sAUTOSAC	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.autosac=1;
				wuser(uid,"Autosac on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.autosac=0;
				wuser(uid,"Autosac off.\n");
				break;
			}
			wuser(uid,"Invalid value for autosac.  Use 'on' or 'off'.\n");
			break;
		case sIACGA	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.iacga=1;
				wuser(uid,"IACGA on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.iacga=0;
				wuser(uid,"IACGA off.\n");
				break;
			}
			wuser(uid,"Invalid value for iacga.  Use 'on' or 'off'.\n");
			break;
		case sNOSWEAR	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.swearon=1;
				wuser(uid,"Swear filter turned on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.swearon=0;
				wuser(uid,"Swear filter turned off.\n");
				break;
			}
			wuser(uid,"Invalid value for noswear.  Use 'on' or 'off'.\n");
			break;
		case sCOLOUR	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.colour=1;
				wuser(uid,"Colour turned on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.colour=0;
				wuser(uid,"Colour turned off.\n");
				break;
			}
			wuser(uid,"Invalid value for colour.  Use 'on' or 'off'.\n");
			break;
		case sBRIEF	: 
			if(!strcasecmp(word[2],"on"))
			{
				UU.brief=1;
				wuser(uid,"Brief output turned on.\n");
				break;
			}
			if(!strcasecmp(word[2],"off"))
			{
				UU.brief=0;
				wuser(uid,"Brief output turned off.\n");
				break;
			}
			wuser(uid,"Invalid value for brief.  Use 'on' or 'off'.\n");
			break;
		case sPROMPT	:
			if (strlen(comstr)>120) 
			{       
				wuser(uid,"New prompt is too large!\n");
				break;
			}
         
			strcpy(UU.prompt,comstr+strlen(word[1])+1);
			wuser(uid,"New prompt set.\n");
			break;
		case sCPROMPT	:
			if (strlen(comstr)>120) 
			{       
				wuser(uid,"New prompt is too large!\n");
				break;
			}
         
			strcpy(UU.cprompt,comstr+strlen(word[1])+1);
			wuser(uid,"New combat prompt set.\n");
			break;
		default:
			wuser(uid,"Unknown setting.\n");
			break;
	}
}

void xshow(SU)
{
	char text[256];
	line(uid,"Your Settings");
	sprintf(text,         "Detected Terminal (^cterm^N)          : %s\n",UU.terminal.type);
	wuser(uid,text);
	sprintf(text,         "Chosen Terminal (^cdefterm^N)         : %s\n",UU.defterm);
	wuser(uid,text);
	wuser(uid,UU.defprec? "Chosen takes precedence (^cdefprec^N) : ^gon^N\n":
			      "Chosen takes precedence (^cdefprec^N) : ^roff^N\n");
	wuser(uid,UU.justify? "Justify output (^cjustify^N)          : ^gon^N\n":
			      "Justify output (^cjustify^N)          : ^roff^N\n");
	wuser(uid,UU.autoexit?"Show verbose exits (^cautoexit^N)     : ^gon^N\n":
			      "Show verbose exits (^cautoexit^N)     : ^roff^N\n");
	wuser(uid,UU.autoloot?"Loot corpses (^cautoloot^N)           : ^gon^N\n":
			      "Loot corpses (^cautoloot^N)           : ^roff^N\n");
	wuser(uid,UU.autosac? "Sacrifice corpses (^cautosac^N)       : ^gon^N\n":
			      "Sacrifice corpses (^cautosac^N)       : ^roff^N\n");
	wuser(uid,UU.iacga?   "Send IACGA for prompts (^ciacga^N)    : ^gon^N\n":
			      "Send IACGA for prompts (^ciacga^N)    : ^roff^N\n");
	wuser(uid,UU.swearon? "Swearing filters (^cnoswear^N)        : ^gon^N\n":
			      "Swearing filters (^cnoswear^N)        : ^roff^N\n");
	wuser(uid,UU.colour?  "Colour output (^ccolour^N)            : ^gon^N\n":
			      "Colour output (^ccolour^N)            : ^roff^N\n");
	wuser(uid,UU.brief?   "Brief output (^cbrief^N)              : ^gon^N\n":
			      "Brief output (^cbrief^N)              : ^roff^N\n");
	sprintf(text,         "Your active email address (^cemail^N) : %s\n",UU.email);
	wuser(uid,text);
	sprintf(text,         "Normal prompt (^cprompt^N)            : %s\n",UU.prompt);
	wuser(uid,text);
	sprintf(text,         "Combat prompt (^ccprompt^N)           : %s\n",UU.cprompt);
	wuser(uid,text);
	line(uid,NULL);
	return;
}
