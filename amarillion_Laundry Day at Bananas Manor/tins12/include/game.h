#ifndef GAME_H
#define GAME_H

#include <allegro.h>
#include <list>
#include <map>
#include <vector>
#include <alfont.h>
#include "button.h"
#include "component.h"
#include "IContainer.h"

class SpriteLayer;
class Engine;
class Resources;
class Game;
class Anim;

class Player;
class SpriteEx;
class GraphicsContext;

// tile stack properties
#define TS_SOLID 0x01
#define TS_SOFT 0x04

#define Y_BLOCK 1
#define X_BLOCK 2

class Sprite : public IComponent
{
	friend class Game;
	friend class Player;
protected:
	Game* parent;
	virtual void kill ();
	Anim *anim;
	int state;
	int dir;
	int animStart;
	bool solid;
private:
	int waitTimer;
	void setParent (Game *_parent);

public:
	int getTileStackFlags(int mx, int my);
	int try_move (double dx, double dy);
	Sprite (Game * game, int amx, int amy);
	void setAnim (Anim *_anim) { anim = _anim; }
	virtual ~Sprite () {}

	virtual void draw(const GraphicsContext &gc);
	
	virtual int update();
	bool hasOverlap (Sprite *s2);
	bool hasXOverlap (Sprite *s2);
};

enum GameState
{
	GS_LEVEL_INTRO, GS_PAUSE, GS_GAMEOVER, GS_PLAYING };

// general constants
const int START_HP = 19; /* PRIME */
const int START_LIVES = 3; /* PRIME */

// player movement constants
const int FLOOR = 640;
const int CEIL = 0;
const float GRAVITY_ACC = 1.0;
const int MAX_JUMPTIMER = 13; // number of ticks a jump can be sustained /* PRIME */
const float MAX_Y = 13.0; // maximum vertical speed, both up and down /* PRIME */
const float JUMP_SPEED = 13.0; // constant speed while pressing jump button /* PRIME */
const float AIR_HSPEED = 7.0; /* PRIME */
const int DEFAULT_SPRITE_W = 47; /* PRIME */
const int DEFAULT_SPRITE_H = 47; /* PRIME */
const double BAZOOKA_ACC = 0.3;
const int PLAYER_HP = 31; /* PRIME */

const int ANIM_NUM = 8;
const int HIT_ANIM_LENGTH = 20;

class DerivedViewPort : public ViewPort
{
private:
	ViewPort *parent;
	int factor;
public:
	DerivedViewPort (ViewPort *_parent, int _factor) : parent(_parent), factor(_factor) {}
	virtual int getx() { return parent->getx() / factor; }
	virtual int gety() { return parent->gety() / factor; }
};

class Particle
{
	public:
		float x;
		float y;
		float dx;
		float dy;
		float r;
		float fr;
		int life;
		bool alive;
};

class Smoke : public IComponent
{
	std::list<Particle> particles;

	public:
		void reset();
		virtual void draw(const GraphicsContext &gc);
		virtual int update();
		void add (int x, int y);
};

class Game : public IContainer
{
	TEG_MAP *map;
	ViewPort aViewPort;
	DerivedViewPort bViewPort;
	DerivedViewPort cViewPort;
	std::list <Sprite*> sprites;


	// per-game, initialized in init
	int lives;

	void updateObjects();

	Button bEsc;
	Button bCheat;
	Engine *engine;

public:
	// per-level, initialized in initLevel
	Player *player; //may be NULL // TODO turn into weak ptr.

	enum { MSG_PLAYER_DIED = 4001, MSG_PLAYER_WINLEVEL }; /* PRIME */
	static std::map<std::string, Anim*> anims;
	ALFONT_FONT *gamefont;	
	ALFONT_FONT *smallfont;

	boost::shared_ptr<Smoke> smoke;

	virtual ~Game() { killAll(); }
	virtual void killAll();
	virtual void done();
	Game (Engine *engine);
	
	void init();
	void initLevel();
	void initGame();

	std::list<Sprite*> &getSprites() { return sprites; }
	void add(Sprite *o);
	int getLives() { return lives; }

	virtual int update();
	virtual void draw(const GraphicsContext &gc);
	virtual void handleMessage(int code);

	Engine *getParent() { return engine; }
	TEG_MAP *getMap() { return map; }
};

#endif
