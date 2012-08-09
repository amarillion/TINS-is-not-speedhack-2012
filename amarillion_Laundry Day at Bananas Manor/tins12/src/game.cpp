#include <assert.h>
#include "engine.h"
#include "color.h"
#include <math.h>
#include <list>
#include <algorithm>
#include "game.h"
#include "engine.h"
#include "color.h"
#include "anim.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "player.h"
#include "main.h"
#include "ComponentBuilder.h"
#include "util.h"

using namespace std;
using namespace boost;

class MyObjectRemover
{
public:
	bool operator()(Sprite *s)
	{
		if (!s->isAlive())
		{
			delete s;
			return true;
		}
		else
			return false;
	}
};

int Game::update ()
{
	IContainer::update();
	updateObjects();

	if (player != NULL)
	{
		int lookat = player->getx() + ((player->getdir() == 1) ? 300 : -300);
		int x = lookat - viewPort->getx();
		int newx = viewPort->getx();
		int newy = viewPort->gety();

		int delta = x - SCREEN_W / 2;

		if (abs(delta) > SCREEN_W)
		{
			newx = x - SCREEN_W / 2;
		}
		else if (delta < -250)
		{
			newx -= 8;
		}
		else if (delta > 250)
		{
			newx += 8;
		}

		if (y < SCREEN_H/4)
		{
			newy = player->gety() - SCREEN_H / 4;
		}
		else if (y > ((SCREEN_H / 4) * 3))
		{
			newy = player->gety() - (SCREEN_H / 4) * 3;
		}

		viewPort->setx (bound (0, newx, teg_pixelw(map) - SCREEN_W));
		viewPort->sety (bound (0, newy, teg_pixelh(map) - SCREEN_H));
	}

	if (bEsc.justPressed())
	{
		engine->setEvent(Engine::E_PAUSE);
	}
	return 0;
}

void Game::updateObjects()
{
	//update
	list<Sprite*>::iterator i;
	for (i = sprites.begin(); i != sprites.end(); i++)
	{
		if ((*i)->isAlive()) (*i)->update();
	}

	// detect collisions
	for (i = sprites.begin(); i != sprites.end(); i++)
	{
		if (!(*i)->isAlive()) continue;
		SpriteEx * sei = dynamic_cast<SpriteEx*>(*i);
		if (sei == NULL) continue; // not a SpriteEx (must be explosion)

		list<Sprite*>::iterator j;
		for (j = i; j != sprites.end(); j++)
		{
			if (*i == *j) continue;
			if (!(*j)->isAlive()) continue;
			SpriteEx * sej = dynamic_cast<SpriteEx*>(*j);
			if (sej == NULL) continue; // not a SpriteEx (must be explosion)

			if ((*i)->hasOverlap(*j))
			{
				// determine relative position of collision
				double vx = sei->getx() - sej->getx();
				double vy = sei->gety() - sej->gety();

				int dir = (fabs(vx) > fabs(vy)) ? SpriteEx::DIR_HORIZONTAL : SpriteEx::DIR_VERTICAL;
				sei->onCol(sej->getSpriteType(), sej, dir);
				sej->onCol(sei->getSpriteType(), sei, dir);
			}

		}
	}

	// remove all that are not alive!
	sprites.remove_if (MyObjectRemover());
}

void Game::done()
{
	killAll();
	engine->setEvent(Engine::E_FINISHED);
}

Game::Game (Engine *engine) : aViewPort(), bViewPort (&aViewPort, 2), cViewPort (&aViewPort, 4), sprites(), bEsc(KEY_ESC), bCheat(KEY_F9)
{
	this->engine = engine;
	visible = false;
	awake = false;
}

void Game::draw (const GraphicsContext &gc)
{
	IContainer::draw(gc);
	alfont_textprintf_aa_ex (gc.buffer, gamefont, 0, 0, GREEN, 0, "LIFE: %02i", lives);

	list<Sprite*>::iterator i;
	for (i = sprites.begin(); i != sprites.end(); i++)
	{
		if ((*i)->isVisible()) (*i)->draw(gc);
	}

#ifdef DEBUG
	if (engine->isDebug())
	{
		textprintf_ex (gc.buffer, font, 0, 16, GREEN, -1, "%i sprites", (int)sprites.size());
	}
#endif
}

map<string, Anim*> Game::anims;

void Game::init()
{
	gamefont = engine->getResources()->getAlfont("megaman_2", 24);
	smallfont = engine->getResources()->getAlfont("megaman_2", 16);
	anims = engine->getResources()->getAnims();
}

void Game::initGame()
{
	lives = START_LIVES;
	engine->getLevelState().clear();
}

void Game::initLevel()
{
	killAll();
	TEG_MAP *bg;
	TEG_MAP *bg2;

	switch (engine->getLevelState().getSelectedLevel())
	{
	case 0:
		map = engine->getResources()->getMap("map1");
		bg2 = engine->getResources()->getMap("bg2");
		bg = engine->getResources()->getMap("bg1");
		break;
	case 1:
		map = engine->getResources()->getMap("map2");
		bg2 = engine->getResources()->getMap("bg3");
		bg = engine->getResources()->getMap("bg1");
		break;
	case 2:
		map = engine->getResources()->getMap("map3");
		bg2 = engine->getResources()->getMap("bg4");
		bg = engine->getResources()->getMap("bg1");
		break;
	case 3:
		map = engine->getResources()->getMap("map4");
		bg2 = engine->getResources()->getMap("bg2");
		bg = engine->getResources()->getMap("bg1");
		break;
	}

	assert (bg);
	viewPort = &aViewPort;

	buildComponent().clearScreen(makecol (170, 170, 255));

	// set up game background
	VisPtr tile2 = VisPtr(new TileMap(bg2));
	buildComponent().vis(tile2).get()->setViewport(&cViewPort);

	VisPtr tile = VisPtr(new TileMap(bg));
	buildComponent().vis(tile).get()->setViewport(&bViewPort);

	tile = VisPtr(new TileMap(map));
	buildComponent().vis(tile).get()->setViewport(viewPort);

	player = new Player(this, 128, 64);
	add (player);

	for (int mx = 0; mx < map->w; ++mx)
	{
		for (int my = 0; my < map->h; ++my)
		{
			int tile = teg_mapget(map, 2, mx, my);
			int flags = map->tilelist->tiles[tile].flags;

			int xx = mx * map->tilelist->tilew;
			int yy = my * map->tilelist->tileh - 64;
			Enemy *e = NULL;
			switch (flags)
			{
			case 5: e = new Enemy(this, xx, yy, 0);
				add (e); break;
			case 6: e = new Enemy(this, xx, yy, 1);
				add (e); break;
			case 7: e = new Enemy(this, xx, yy, 2);
				add (e); break;
			case 8: e = new Enemy(this, xx, yy, 3);
				add (e); break;
			default:
				/* ignore */
				break;
			}
		}
	}

	smoke = shared_ptr<Smoke>(new Smoke());
	IContainer::add (dynamic_pointer_cast<IComponent>(smoke));

}


void Game::killAll()
{
	IContainer::killAll();
	list<Sprite*>::iterator i;
	for (i = sprites.begin(); i != sprites.end(); ++i)
	{
		delete (*i);
		(*i) = NULL;
	}
	sprites.clear();
	smoke.reset();
}

void Game::add(Sprite *o)
{
	sprites.push_back (o);
}

void Game::handleMessage(int event)
{
	IContainer::handleMessage(event);
	switch (event)
	{
	case MSG_PLAYER_DIED:
		lives--;
		if (lives == 0)
		{
			engine->setEvent(Engine::E_GAME_OVER);
		}
		else
		{
			engine->setEvent(Engine::E_LEVEL_INTRO);
		}
		break;
	case MSG_PLAYER_WINLEVEL:
		engine->setEvent(Engine::E_LEVEL_CLEAR);
		break;
	}
}

Sprite::Sprite (Game *game, int amx, int amy) : anim(NULL), state(0), dir(0), animStart(0)
{
	setx (amx);
	sety (amy);
	alive = true;
	visible = true;
	solid = true;
	parent = game;
}

int Sprite::update()
{
	assert (parent);
	return 0;
}

void Sprite::kill()
{
	alive = false;
}

void Sprite::draw(const GraphicsContext &gc)
{
	int x = getx() + gc.xofst - parent->getViewPort()->getx();
	int y = gety() + gc.yofst - parent->getViewPort()->gety();
	int time = 0;
	if (anim != NULL)
		anim->drawFrame (gc.buffer, state, dir, time, x, y);
}

bool Sprite::hasXOverlap (Sprite *s2)
{
	int x1 = x;
	int dx1 = w;
	int x2 = s2->getx();
	int dx2 = s2->getw();
	return !(
			(x1 >= x2+dx2) || (x2 >= x1+dx1)
		);
}

bool Sprite::hasOverlap (Sprite *s2)
{
	int y1 = y;
	int dy1 = h;
	int y2 = s2->gety();
	int dy2 = s2->geth();
	return hasXOverlap(s2) &&
		!(
			(y1 >= y2+dy2) || (y2 >= y1+dy1)
		);
}

int Sprite::getTileStackFlags(int mx, int my)
{
	int result = 0;
	TEG_MAP *map = parent->getMap();
	if (!map) return 0; // no map found !!!

	if (mx < 0 || my < 0 || mx >= map->w || my >= map->h)
	{
		return 0;
	}
	else
	{
		int i1, i2, f1, f2;
		i1 = teg_mapget (map, 0, mx, my);
		i2 = teg_mapget (map, 1, mx, my);
		if (i1 >= 0) f1 = map->tilelist->tiles[i1].flags; else f1 = 0;
		if (i2 >= 0) f2 = map->tilelist->tiles[i2].flags; else f2 = 0;

		// check for solids
		if (f1 == 1 || f2 == 1) result |= TS_SOLID;

		return result;
	}
}

int Sprite::try_move (double dx, double dy)
{
	// check with tilemap background, but only if object is solid.
	if (!solid)
	{
		x += dx;
		y += dy;
		return 0;
	}

	double dxleft = dx, dyleft= dy;
	int ddx = dx > 0 ? 1 : -1;
	int ddy = dy > 0 ? 1 : -1;
	double trydx, trydy;
	int result = 0;

	while ((fabs(dxleft) > 0 || fabs (dyleft) > 0))
	{
		bool valid = true;
		if (fabs(dxleft) > fabs(dyleft))
		{
			trydy = 0;
			if (fabs(dxleft) >= 1)
				trydx = ddx;
			else
				trydx = dxleft;
		}
		else
		{
			trydx = 0;
			if (fabs(dyleft) >= 1)
				trydy = ddy;
			else
				trydy = dyleft;
		}

		// check if (x +  |trydx, y + trydy) is valid
		int mx1, my1, mx2, my2;
		int ix, iy;
		TEG_MAP *map = parent->getMap();
		mx1 = ((int)x + trydx) / map->tilelist->tilew;
		my1 = ((int)y + trydy) / map->tilelist->tileh;
		mx2 = ((int)x + trydx + w - 1) / map->tilelist->tilew;
		my2 = ((int)y + trydy + h - 1) / map->tilelist->tileh;

		// loop through all map positions we touch with the solid region
		for (ix = mx1; ix <= mx2; ++ix)
		{
			for (iy = my1; iy <= my2; ++iy)
			{
				// see if there is a solid tile at this position
				if (getTileStackFlags (ix, iy) & TS_SOLID)
				{
					valid = false;
				}
			}
		}

		if (!valid)
		{
			if (trydx == 0)
			{
				result |= Y_BLOCK;
				dyleft = 0;
			}
			else
			{
				result |= X_BLOCK;
				dxleft = 0;
			}
		}
		else
		{
			x += trydx;
			dxleft -= trydx;
			y += trydy;
			dyleft -= trydy;
		}
	}
	return result;
}

void Smoke::reset()
{
	particles.clear();
}

void Smoke::draw(const GraphicsContext &gc)
{
	BITMAP *buffer = gc.buffer;
	int xofst = viewPort->getx();
	int yofst = viewPort->gety();

	drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	for (list<Particle>::iterator i = particles.begin(); i != particles.end(); ++i)
	{
		if (i->alive)
		{
			set_trans_blender (0, 0, 0, 200 - (i->life * 2));
			circlefill (buffer, i->x - xofst, i->y - yofst, i->r, WHITE);
		}
	}
	drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
}

class MyParticleRemover
{
   public:
	  bool operator()(Particle &o)
	  {
		 if (!o.alive)
		 {
			return 1;
		 }
		 return 0;
	  }
};

int Smoke::update()
{
	for (list<Particle>::iterator i = particles.begin(); i != particles.end(); ++i)
	{
		if (i->alive)
		{
			i->life++;
			if (i->life > 100) { i->alive = false; }
			i->y += i->dy;
			i->x += i->dx;
			i->r *= i->fr;
		}
	}

	// remove all that are not alive!
	particles.remove_if (MyParticleRemover());
	return 0;
}

void Smoke::add (int x, int y)
{
	Particle p;
	p.x = x;
	p.y = y;
	p.dy = -0.3f;
	p.dx = ((rand() % 20) - 10) * 0.05f;
	p.life = 0;
	p.alive = true;
	p.r = 3.0f;
	p.fr = 1.03;
	particles.push_back (p);
}
