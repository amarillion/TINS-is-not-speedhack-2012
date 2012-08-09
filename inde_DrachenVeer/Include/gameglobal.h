#ifndef _GAMEGLOBAL_H_
#define _GAMEGLOBAL_H_

//Header file which gives a game global variable list one can use to compile with. 

#include <header.h>
#include <level.h>
#include <global.h>

extern int nlevels;
extern int curlevel;
extern char **levelnames;
extern level **levels;
extern int score;

#endif //_GAMEGLOBAL_H_
