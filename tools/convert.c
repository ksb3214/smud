#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mff.h>

int main(int argc,char **argv)
{
	FILE *def,*tag;
	char *temp = malloc(512);
	smud_file *myfile;
	char *buffer = malloc(1024);
	char *work = malloc(16384);
	short vnum;
	int i;

	if(argc<2)
	{
		printf(" Usage: %s <filename>\n",argv[0]);
		exit(0);
	}
	sprintf(temp,"%s.def",argv[1]);
	def = fopen(temp,"r");
	if(!def)
	{
		printf("Cannot open source file %s.\n",temp);
		exit(0);
	}

	sprintf(temp,"%s.tag",argv[1]);
	tag = fopen(temp,"r");
	if(!tag)
	{
		printf("Cannot open tag file %s.\n",temp);
		exit(0);
	}

	myfile=new_file();

	i=0;
	while(fgets(buffer,1024,def))
	{
		*(buffer+strlen(buffer)-1)=0;
		if(*buffer=='%')
		{
			if(i==0)
			{
				*(work+strlen(work)-1)=0;
				*(work+strlen(work)-1)=0;
				vnum=atoi(work);
			} else {
				if(fgets(temp,512,tag))
				{
					*(temp+strlen(temp)-1)=0;
					*(work+strlen(work)-1)=0;
					*(work+strlen(work)-1)=0;
					add_entry(myfile,vnum,temp,work);
				} else {
					rewind(tag);
					*(work+strlen(work)-1)=0;
					*(work+strlen(work)-1)=0;
					vnum=atoi(work);
					i=0;
				}
			}
			bzero(work,16384);
			i++;
		} else {
			work=strcat(work,buffer);
			work=strcat(work,"\\n");
		}
	}
	sprintf(temp,"%s.mff",argv[1]);
	write_file(myfile,temp);
	fclose(def);
	fclose(tag);
	free(temp);
	return 0;
}
