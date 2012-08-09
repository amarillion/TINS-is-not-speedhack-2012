#ifndef _UTIL_H_
#define _UTIL_H_

//Header file for miscellaneous functions and utilities.

#include <header.h>

//Length of an Array.
#define array_length(arr) (sizeof((arr))/sizeof(*(arr)))

//Compares two Strings for Utmost n Characters.
int _strincmp(char *s1, char *s2, int len);

//Truncates a string off trailing newlines and carriage returns
char *strip_crlfs (char *str);

//Skips data from current line onto next line
void *skip_line (FILE * flevel);

//File Exists.
int file_exists (char *file);

//Get File Contents.
char *get_file_contents (char *file, long *length);

//Loads a BITMAP
ALLEGRO_BITMAP *load_bitmap_ex (char *file, ALLEGRO_COLOR * pal);

#endif //_UTIL_H_
