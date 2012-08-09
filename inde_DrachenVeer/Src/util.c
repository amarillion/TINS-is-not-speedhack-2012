//Source file for miscellaneous functions and utilities.

#include <util.h>
#include <ctype.h>

//Compares two Strings for Utmost n Characters.
int _strincmp(char *s1, char *s2, int n){
	/* case insensitive comparison */
	int d;
	while (--n >= 0) {
#ifdef ASCII_CTYPE
	  if (!isascii(*s1) || !isascii(*s2))
	    d = *s1 - *s2;
	  else
#endif
	    d = (tolower((unsigned char)*s1) - tolower((unsigned char)*s2));
	  if ( d != 0 || *s1 == '\0' || *s2 == '\0' )
	    return d;
	  ++s1;
	  ++s2;
	}
	return(0);
}

//Truncates a string off trailing newlines and carriage returns
char *
strip_crlfs (char *str)
{
  char *tmp = str;
  if (str)
    while (*str)
      {
	if (*str == '\r' || *str == '\n')
	  *str = 0;
	else
	  str++;
      }
  return tmp;
}

//Skips data from current line onto next line
void *
skip_line (FILE * flevel)
{
  int c = EOF;
  char data[MAXSTR], *retval;

  *data = 0;
//Ignore a Line
  retval = fgets (data, MAXSTR, flevel);

//Clean Up
  c = fgetc (flevel);
  while (c != EOF && !isprint (c))
    c = fgetc (flevel);
  if (c != EOF && isprint (c))
    ungetc (c, flevel);

  return NULL;
}

//File Exists.
int
file_exists (char *file)
{
  FILE *rfile = NULL;
  rfile = fopen (file, "rb");
  if (rfile)
    {
      fclose (rfile);
      rfile = NULL;
      return 1;
    }
  return 0;
}

//Get File Length.
static long
get_file_length (FILE * file)
{
  long fcur, flength;
  fcur = ftell (file);
  fseek (file, 0, SEEK_END);
  flength = ftell (file);
  fseek (file, fcur, SEEK_SET);
  return flength;
}

//Get File Contents.
char *
get_file_contents (char *file, long *length)
{
  FILE *rfile = NULL;
  long flength = 0;
  char *array = NULL;
  if (file)
    rfile = fopen (file, "rb");
  if (rfile)
    {
      flength = get_file_length (rfile);
      array = malloc (sizeof (char) * (flength + 1));
      if (array)
	{
	  fread (array, sizeof (char), flength, rfile);
	  array[flength] = 0;
	}
      fclose (rfile);
      rfile = NULL;
    }
  if (length)
    *length = flength;
  return array;
}

//Loads a ALLEGRO_BITMAP
ALLEGRO_BITMAP *
load_bitmap_ex (char *file, ALLEGRO_COLOR * pal)
{
  ALLEGRO_BITMAP *lbmp = NULL;
  lbmp = al_load_bitmap (file);
  if (lbmp)
    {
// Set 24-32-bit magenta to transparent
      al_convert_mask_to_alpha (lbmp, al_map_rgb (255, 0, 255));
// Set 16bit magenta to transparent
      al_convert_mask_to_alpha (lbmp, al_map_rgb (248, 0, 248));
// Set indexed magenta to transparent
      al_convert_mask_to_alpha (lbmp, al_map_rgb (240, 0, 240));
    }
  return lbmp;
}
