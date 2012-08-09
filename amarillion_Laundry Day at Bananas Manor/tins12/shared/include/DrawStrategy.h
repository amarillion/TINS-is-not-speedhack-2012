/*
 * Vis.h
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#ifndef DRAWSTRATEGY_H_
#define DRAWSTRATEGY_H_

#include <boost/shared_ptr.hpp>
#include "graphicscontext.h"

struct BITMAP;
struct TEG_MAP;

class IComponent;

class Vis {
protected:
	IComponent* parent;
public:
	Vis();
	virtual ~Vis();
	virtual void draw(const GraphicsContext &gc) = 0;
	void setParent(IComponent *parent) { this->parent = parent; }
};

class DefaultStrategy : public Vis
{
public:
	DefaultStrategy(std::string &s);
private:
	std::string s;
	virtual void draw(const GraphicsContext &gc);
};

class ClearScreen : public Vis
{
public:
	ClearScreen(int color) { this->color = color; }
	virtual void draw(const GraphicsContext &gc);
private:
	int color;
};

class Pattern : public Vis
{
public:
	Pattern(BITMAP *bmp) { this->texture = bmp; }
	virtual void draw(const GraphicsContext &gc);
private:
	BITMAP *texture;
};

class TileMap : public Vis
{
public:
	TileMap(TEG_MAP *tilemap) { this->tilemap = tilemap; }
	virtual void draw(const GraphicsContext &gc);
private:
	TEG_MAP *tilemap;
};

typedef boost::shared_ptr<Vis> VisPtr;

#endif /* DRAWSTRATEGY_H_ */
