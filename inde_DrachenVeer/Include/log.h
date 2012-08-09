#ifndef _LOG_H_
#define _LOG_H_

//Header file containing useful logging function declarations.

#include <header.h>

//Log filename
#define LOGFILE "gamelog.txt"
#define LOGMODE "a+t"

//Logging functions

//printf like function to write to log
int log_printf (char *format, ...);

//Closes logging system. 
void quit_log ();

//Initializes logging system. 
int init_log ();

//Shows the log.
void display_log();

#endif //_LOG_H_
