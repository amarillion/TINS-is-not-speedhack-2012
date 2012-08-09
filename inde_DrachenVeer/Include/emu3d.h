#ifndef _EMU3D_H_
#define _EMU3D_H_

//Header file containing emulated 3D function declarations.

#include <header.h>

// ANAGLYPHIC: Red/Cyan
// TRIOCHROME: Green/Magenta
// FOV: Field of View.
#define EMU3D_NULL        -1
#define EMU3D_FOV          0 
#define EMU3D_TRIOCHROME   1 
#define EMU3D_ANAGLYPHIC   2
#define EMU3D_SILHOUTTE    3
#define EMU3D_TYPES        4

//3D Mode set
void emu3d_render_mode (int mode);

//3D Start
void emu3d_start (int layers);

//3D Layer Add
void emu3d_layer_done ();

//3D Layer End
void emu3d_end ();

//Uninitialize 3D Renderer
void quit_emu3d_renderer ();

//Initialize 3D Renderer
int init_emu3d_renderer ();

#endif //_EMU3D_H_
