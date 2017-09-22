
/*
 * Generic file load/save routines
 */

#ifndef _MFF_FILE_
#define _MFF_FILE_

#define SECTION_ITEMS 30
#define SECTION_MAX 1000

typedef struct file_entry_s {
	short vnum;
	char *tag;
	char *data;
} file_entry;

typedef struct smud_file_s {
	short fileptr;
	short lines;
	file_entry data[SECTION_ITEMS*SECTION_MAX];
	short vnumlist[SECTION_MAX];
	short vnumcount;
} smud_file;

/*
 * smud_file *readfile("filename.mff");
 *
 * Load a .mff file into memory and returns a pointer to the structure in
 * memory.
 */
extern smud_file *read_file(char *name);

/*
 * char *getval(smudfile,vnum,"tagname");
 *
 * Returns a string containing the data for "tagname" from section vnum.
 */
extern char *getval(smud_file *, short, char *);

/*
 * smud_file *new_file();
 *
 * Allocate memory for a new file ready for populating with new data prior
 * to writing it to disk.
 */
extern smud_file *new_file();

/*
 * add_entry(smudfile,vnum,"tagname","data");
 *
 * Add data for "tagname" in section vnum to file smudfile.  Smudfile should
 * be a fresh file created with new_file(). Existing data for that vnum/tag
 * combination will be overwritten.
 */
extern void add_entry(smud_file *, short, char *, char *);

/*
 * write_file(smudfile,"filename.mff");
 *
 * Write the data in the structure pointed to by smudfile to the disk file
 * named "filename.mff".
 */
extern void write_file(smud_file *, char *);

/*
 * freefile(smudfile);
 *
 * Free all memory allocated to the structure pointed to by smudfile.  All
 * data for that file will be cleared from memory.
 */
extern void freefile(smud_file *);

/*
 * Useful variables in the smud_file structure:
 * 
 * smudfile->vnumlist[]
 *   An array of short's containing all vnum codes present in the file.
 *
 * smudfile->vnumcount
 *   A count of the number of vnum codes in the vnumlist[] array.
 */

#endif
