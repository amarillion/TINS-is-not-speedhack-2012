//Source file for logging functions.

#include <log.h>
#include <util.h>
#include <fonts.h>

//Log Stream Object
static FILE *logstr = NULL;

//Log Buffer for Printing.
#define LOG_RING 25
static char log_messages[LOG_RING][MAXSTR];
static int log_messages_index = 0;

//Output a newline
static int
log_newline ()
{
  int status = 0;

  if (logstr)
    {
      status += fprintf (logstr, "\n");
      fflush (logstr);
    }

  return status;
}

//printf like function to write to log
int
log_printf (char *format, ...)
{
  int status = 0;
#ifdef LOG
  time_t logtime;
  va_list ap;

  time (&logtime);

//Write timestamp
  if (logstr)
    status += fprintf (logstr, "[%s]: ", strip_crlfs (ctime (&logtime)));

//Write actual message
  if (logstr)
    {
      va_start (ap, format);
      status += vfprintf (logstr, format, ap);
      va_end (ap);
    }

//Add message to ring
  va_start (ap, format);
  vsprintf (log_messages[log_messages_index], format, ap);
  va_end (ap);
  log_messages_index++;
  if (log_messages_index >= LOG_RING)
    log_messages_index = 0;


//Write New Line
  if (logstr)
    {
      status += log_newline ();
      fflush (logstr);
    }
#endif //LOG

  return status;
}

//Closes logging system. 
void
quit_log ()
{
#ifdef LOG
  if (logstr)
    {
//Close Log Stream
      fflush (logstr);

      if (logstr != stdout && logstr != stderr)
	fclose (logstr);
      logstr = NULL;
    }
#endif //LOG
}

//Initializes logging system. 
int
init_log ()
{

#ifdef LOG
  quit_log ();
//Open Log Stream
  if (!strcmp ("stdout", LOGFILE))
    logstr = stdout;
  else if (!strcmp ("stderr", LOGFILE))
    logstr = stderr;
  else if (!strcmp ("null", LOGFILE))
    ;
  else
    logstr = fopen (LOGFILE, LOGMODE);

//Open Log Messages.
  {
    for (log_messages_index = LOG_RING; log_messages_index > 0;
	 log_messages_index--)
      {
	char *log_ptr = log_messages[log_messages_index - 1];
	*log_ptr = 0;
      }
  }

//Write Log Message
  log_newline ();
  log_printf ("-------------------------------------------------------");
  log_printf ("Log Initialized");
#endif //LOG

  return 0;
}

//Shows the log.
void
display_log ()
{
#ifdef LOG
  ALLEGRO_FONT *font = fm_get_font (FONT_TEXT);
  int index, font_height;
  float y;
  float x;
  if (!font)
    return;
  font_height = al_get_font_line_height (font);
  index = log_messages_index + 1;
  if (index >= LOG_RING)
    index = 0;
  y  = (float) font_height;
  x = 0.0f;
  do
    {
      char *message = log_messages[index];
      if (*message)
	{
	  al_draw_text (font, al_map_rgb (0, 0, 0), x+1, y+1,
			ALLEGRO_ALIGN_LEFT, message);
	  al_draw_text (font, al_map_rgb (0, 127, 255), x, y,
			ALLEGRO_ALIGN_LEFT, message);
	  y += (float) font_height;
	}
      ++index;
      if (index >= LOG_RING)
	index = 0;
    }
  while (index != log_messages_index);
#endif //LOG
}
