#ifndef __COMMON_H
#define __COMMON_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include "Constants.h"

using namespace std;
using namespace std::tr1;

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef DOUBLE
#define DOUBLE double
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef BOOL
#define BOOL bool
#endif

#ifndef NULL
#define NULL ((VOID *)0)
#endif

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b))?((b)):((a)))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b))?((a)):((b)))
#endif

#ifndef ABS
#define ABS(a) ((a) > 0)?((a)):(-(a))
#endif

#define DELETEPTR(ptPtr) {if (NULL != (ptPtr)) {delete (ptPtr); (ptPtr)=NULL;}}
#define DELETEARR(ptPtr) {if (NULL != (ptPtr)) {delete[] (ptPtr); (ptPtr)=NULL;}}

#endif