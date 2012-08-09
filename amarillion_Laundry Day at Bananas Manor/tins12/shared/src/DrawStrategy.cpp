/*
 * DrawStrategy.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "DrawStrategy.h"
#include <alfont.h>
#include "color.h"
#include <allegro.h>
#include "component.h"
#include <tegel.h>

Vis::Vis() {
	// TODO Auto-generated constructor stub

}

Vis::~Vis() {
}

DefaultStrategy::DefaultStrategy(std::string &s)
{
	this->s = s;
}

void DefaultStrategy::draw(const GraphicsContext &gc)
{
	alfont_textout_centre_aa (gc.buffer, parent->getFont(), s.c_str(), 0, 0, WHITE);
}

void ClearScreen::draw(const GraphicsContext &gc)
{
	clear_to_color(gc.buffer, color);
}

void Pattern::draw(const GraphicsContext &gc)
{
	int xofst = 0;
	int yofst = 0;
	if (parent)
	{
		IMotionPtr motion;
		motion = parent->getMotion();
		int counter = parent->getCounter();
		xofst = motion ? motion->getdx(counter) : 0;
		yofst = motion ? motion->getdy(counter) : 0;
	}


	int TILEW = texture->w;
	int TILEH = texture->h;

	for (int x = (xofst % TILEW) - TILEW; x < gc.buffer->w + TILEW; x += TILEW)
	{
		for (int y = (yofst % TILEH) - TILEH; y < gc.buffer->h + TILEH; y += TILEH)
		{
			draw_sprite (gc.buffer, texture, x, y);
		}
	}
}

// fill up target bmp with the same map over and over
void teg_draw_tiled_rle (BITMAP *bmp, const TEG_MAP* map, int layer, int xview, int yview, int frame)
{
	int x, y;
	int tilex, tiley;
	int tilew, tileh;
	tileh = map->tilelist->tileh;
	tilew = map->tilelist->tilew;
	tiley = (yview / tileh) % map->h;
	while (tiley < 0) tiley += map->h;
	for (y = -(yview % tileh); y < bmp->h; y += tileh)
	{
		tilex = (xview / tilew) % map->w;
		while (tilex < 0) tilex += map->w;
		for (x = - (xview % tilew) ; x < bmp->w; x += tilew)
		{
			RLE_SPRITE *s;
			int i;
			i = teg_mapget(map, layer, tilex, tiley);
			if (i >= 0 && i < map->tilelist->tilenum)
			{
				i += frame * map->tilelist->tilenum;
				s = map->tilelist->tiles[i].rle;
				if (s != NULL)
					draw_rle_sprite (bmp, s, x, y);
			}
			tilex++;
			if (tilex >= map->w) tilex = 0;
		}
		tiley++;
		if (tiley >= map->h) tiley = 0;
	}
}

void TileMap::draw(const GraphicsContext &gc)
{
	int xofst = 0;
	int yofst = 0;
	int counter = 0;
	if (parent)
	{
		IMotionPtr motion;
		motion = parent->getMotion();
		counter = parent->getCounter();
		xofst = motion ? motion->getdx(counter) : 0;
		yofst = motion ? motion->getdy(counter) : 0;

		ViewPort *viewPort = parent->getViewPort();
		if (viewPort)
		{
			xofst += viewPort->getx();
			yofst += viewPort->gety();
		}
	}

	int frame = (counter / 20) % tilemap->tilelist->animsteps;
	teg_draw_tiled_rle(gc.buffer, tilemap, 0, xofst, yofst, frame);
}
