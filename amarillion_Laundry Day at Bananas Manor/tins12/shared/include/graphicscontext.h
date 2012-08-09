#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

struct BITMAP;

struct GraphicsContext
{
	BITMAP *buffer;
	int xofst;
	int yofst;
};

#endif
