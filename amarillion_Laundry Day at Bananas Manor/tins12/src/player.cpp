#include "player.h"
#include "color.h"
#include "engine.h"
#include <math.h>

SpriteEx::SpriteEx (Game *game, SpriteType st, int x, int y, int _w, int _h, int _subtype) : Sprite (game, x, y)
{
	subtype = _subtype;
	hp = 0;
	dx = 0;
	dy = 0;
	air = true;
	spriteType = st;
	gravity = true;
	w = _w;
	h = _h;

	switch (spriteType)
	{
	case ST_PLAYER: setAnim(game->anims["Raul"]); break;
	case ST_ENEMY:
		switch (subtype % 4)
		{
			case ELECTRICAT: setAnim(game->anims["Electricat"]); break;
			case SLINKYCAT: setAnim(game->anims["Slinkycat"]); break;
			case SPIDERCAT: setAnim(game->anims["Spidercat"]); break;
			case DRAGONCAT: setAnim(game->anims["Dragoncat"]); break;
		}
		break;
	case ST_BULLET:
		switch (subtype % 4)
		{
			case Bullet::WPN_ROCK: setAnim(game->anims["Rock"]); break;
			case Bullet::WPN_ICE: setAnim(game->anims["Ice"]); break;
			case Bullet::WPN_BAZOOKA: setAnim(game->anims["Bullet"]); break;
			case Bullet::WPN_LASER: setAnim(game->anims["Laser"]); break;
		}
		break;
	case ST_ENEMY_BULLET:
		switch (subtype)
		{
			case EBullet::FIRE: setAnim(game->anims["Fire bubble"]); break;
			case EBullet::ENERGY: setAnim(game->anims["Enemy bullet"]); break;
		}
		break;
	default: /* do nothing */ break;
	}

	if (anim != NULL)
	{
		w = anim->sizex;
		h = anim->sizey;
	}

}

int SpriteEx::update()
{
	IComponent::update();
	list<Sprite*>::iterator i;
	list<Sprite*> sprites = parent->getSprites();

	if (gravity)
	{
		dy += GRAVITY_ACC;
		if (dy > MAX_Y) dy = MAX_Y;
		if (dy < -MAX_Y) dy = -MAX_Y;
	}

	int block = try_move(dx, dy);

	if ((block & Y_BLOCK) > 0)
	{
		onCol(ST_TILE, NULL, DIR_VERTICAL);
		air = false;
		dy = 0;
	}
	else
	{
		air = true;
	}
	if ((block & X_BLOCK) > 0)
	{
		onCol(ST_TILE, NULL, DIR_HORIZONTAL);
	}

	if (y > FLOOR)
	{
		onCol(ST_FLOOR, NULL, DIR_VERTICAL);
	}

	if (y < CEIL)
	{
		y = CEIL;
		onCol(ST_BOUNDS, NULL, DIR_VERTICAL);
	}

	if (x < 0)
	{
		x = 0;
		onCol(ST_BOUNDS, NULL, DIR_HORIZONTAL);
	}

	int mapw = teg_pixelw(parent->getMap());
	if ((x + w) >= mapw)
	{
		x = mapw - w;
		onCol(ST_BOUNDS, NULL, DIR_HORIZONTAL);
	}

	return 0;
}

void SpriteEx::onCol(SpriteType st, Sprite *s, int dir)
{
	SpriteEx *se = dynamic_cast<SpriteEx *>(s);

	if (st == ST_FLOOR)
	{
		// we died
		kill();
	}

}

void Player::setState (bool hit)
{
	if (hit)
		state = 4;
	else
		switch (currentWeapon)
		{
		case Bullet::WPN_ROCK: state = 0; break;
		case Bullet::WPN_ICE: state = 1; break;
		case Bullet::WPN_BAZOOKA: state = 3; break;
		case Bullet::WPN_LASER: state = 2; break;
		}
}

void Player::hit(int subtype, double delta)
{
	setState (true);
	hitTimer = HIT_ANIM_LENGTH;

	// make a little jump
	dy = -6;
	if (delta > 0) dx = -6; else dx = 6;

	switch (subtype)
	{
	case ELECTRICAT:
	case SLINKYCAT:
	case SPIDERCAT:
		hp -= 4;
		break;
	case DRAGONCAT:
		hp -= 6;
		break;
	case EBullet::ENERGY:
		hp -= 2;
		break;
	case EBullet::FIRE:
		hp -= 3;
		break;
	}

	if (hp <= 0)
	{
		hp = 0;
		die();
	}

}

void Player::onCol(SpriteType st, Sprite *s, int dir)
{
	SpriteEx *se = dynamic_cast<SpriteEx *>(s);
	switch (st)
	{
	case ST_ENEMY:
	case ST_ENEMY_BULLET:
		if (hitTimer == 0)
		{
			hit(se->getSubType(), se->getx() - x);
		}
		break;
	case ST_FLOOR:
		die();
		break;
	case ST_TILE:
		// if going down, we landed
		if (dir == DIR_VERTICAL && dy > 0)
		{
			air = false;
			dy = 0;
			jumpTimer = 0;
		}
		break;
	case ST_BOUNDS:
		if (dir == DIR_HORIZONTAL && dx > 0)
		{
			winLevel();
		}
		break;
	default: /* do nothing */ break;
	}
}


void SpriteEx::draw(const GraphicsContext &gc)
{
	int color;
	switch (spriteType)
	{
	case ST_PLAYER: color = YELLOW; break;
	case ST_ENEMY: color = GREEN; break;
	case ST_BULLET: color = GREY; break;
	case ST_ENEMY_BULLET: color = RED; break;
	default: color = BLACK; break;
	}

	int x = getx() + gc.xofst;
	x -= parent->getViewPort()->getx();
	int y = gety() + gc.yofst;
	y -= parent->getViewPort()->gety();
	int time = parent->getParent()->getCounter();

	if (anim != NULL)
	{
		anim->drawFrame (gc.buffer, state, dir, time, x, y);
	}
	else
	{
		rectfill (gc.buffer, x, y, x + w, y + h, color);
	}

#ifdef DEBUG
	if (parent->getParent()->isDebug())
	{
		rect (gc.buffer, x, y, x + w, y + h, color);
		alfont_textprintf_ex(gc.buffer, parent->smallfont, x, y - 16, GREEN, -1, "hp: %i", hp);
	}
#endif
}

Player::Player (Game *game, int x, int y) : SpriteEx (game, ST_PLAYER, x, y)
{
	gravity = true;
	state = 0;
	hitTimer = 0;
	currentWeapon = 0;
	shootTimer = 0;
	hp = PLAYER_HP;
	btn = game->getParent()->getButton();
}

void Player::kill()
{
	SpriteEx::kill();
	parent->player = NULL;
}

void Player::die()
{
	// create a timer
	parent->setTimer(50, Game::MSG_PLAYER_DIED);
	Explosion *e = new Explosion(parent, getx(), gety(), 100);
	parent->add (e);
	kill(); // kill this sprite
}

void Player::winLevel()
{
	// create a timer
	parent->setTimer(50, Game::MSG_PLAYER_WINLEVEL);
}

int Player::update()
{
	//	float speed = air ? AIR_HSPEED : LAND_HSPEED;
	if (btn[btnLeft].getState() && hitTimer == 0)
	{
		dx = -AIR_HSPEED;
		dir = 0;
	}
	else if (btn[btnRight].getState() && hitTimer == 0)
	{
		dx = AIR_HSPEED;
		dir = 1;
	}
	else if (hitTimer == 0)
	{
		if (!air) dx = 0;
	}

	if (hitTimer > 0)
	{
		hitTimer--;
		if (hitTimer == 0)
		{
			setState (false);
		}
	}

	if (btn[btnUp].getState() && hitTimer == 0)
	{
		if (!air)
		{
			parent->getParent()->playSample("Sound4");
			jumpTimer = MAX_JUMPTIMER;
			air = true;
		}

		// while jump timer is counting
		if (jumpTimer > 0)
		{
			// constant acceleration
			dy = -JUMP_SPEED;
			jumpTimer--;
		}
	}
	else
	{
		jumpTimer = 0;
	}

	SpriteEx::update();

	// handle shooting
	if (shootTimer > 0) shootTimer--;
	if (btn[btnAction].justPressed() && shootTimer == 0 && hitTimer == 0)
	{
		shoot();
	}

	// handle weapon cycling
	if (btn[btnDown].justPressed())
	{
		cycleWeapons();
	}

	return 0;
}

void Player::shoot()
{
	int ox = getx() + ((dir == 0) ? -1 : getw() + 1);
	int oy = gety() + 20;

	parent->getParent()->playSample("Sound3");
	double dxsign = (dir == 0) ? -1 : 1;
	switch (currentWeapon)
	{
		case Bullet::WPN_ROCK:
		{
			Bullet *b = new Bullet (parent, Bullet::WPN_ROCK, ox, oy, 13 * dxsign, -7);
			parent->add(b);
			shootTimer = 19;
			break;
		}
		case Bullet::WPN_ICE:
		{
			Bullet *b = new Bullet (parent, Bullet::WPN_ICE, ox, oy, 13 * dxsign, -7);
			parent->add(b);
			shootTimer = 19;
			break;
		}
		case Bullet::WPN_BAZOOKA:
		{
			Bullet *b = new Bullet (parent, Bullet::WPN_BAZOOKA, ox, oy, 7 * dxsign, 0);
			parent->add(b);
			shootTimer = 37;
			break;
		}
		case Bullet::WPN_LASER:
		{
			Bullet *b = new Bullet (parent, Bullet::WPN_LASER, ox, oy, 17 * dxsign, 0);
			parent->add(b);
			shootTimer = 7;
			break;
		}
	}
}

bool Player::isWeaponAvailable(int wpn)
{
	assert (wpn >=0 && wpn <= 3);
	if (wpn == 0)
		return true;
	LevelState level = parent->getParent()->getLevelState();
	if (level.isLevelCleared(wpn -1))
	{
		return true;
	}
	return false;
}

void Player::cycleWeapons()
{
	LevelState level = parent->getParent()->getLevelState();
	int newWeapon = (currentWeapon + 1) % 4;
	while (!isWeaponAvailable(newWeapon))
	{
		newWeapon = (newWeapon + 1) % 4;
	}

	if (currentWeapon != newWeapon)
	{
		parent->getParent()->playSample("Sound2");
		currentWeapon = newWeapon;
		setState (false);
	}
	else
	{
		parent->getParent()->playSample("Sound1");
	}
}

void Player::draw(const GraphicsContext &gc)
{
	SpriteEx::draw(gc);
	alfont_textprintf_right_ex (gc.buffer, parent->gamefont, gc.buffer->w, 0, CYAN, 0, "HP: %02i", hp);
#ifdef DEBUG
	if (parent->getParent()->isDebug())
	{
//		alfont_textprintf_ex (gc.buffer, parent->gamefont, 0, 48, WHITE, -1, "air %i, dx: %3.2f dy: %3.2f ddy: %3.2", air ? 1 : 0, dx, dy);
//
//		if (parent->getViewPort())
//			alfont_textprintf_ex (gc.buffer, parent->gamefont, 0, 64, GREEN, -1, "viewport %i, x: %3.2f", parent->getViewPort()->getx(), x);
	}
#endif
}


const int MAX_ENEMY_TYPE = 5;

Enemy::Enemy (Game *game, int x, int y, int _type) : SpriteEx (game, ST_ENEMY, x, y, 0, 0, _type)
{
	enemyType = (_type % MAX_ENEMY_TYPE);
	hittimer = 0;
	estate = 0;
	period = 1;
	vsign = 1.0;
	hsign = (rand() % 100 > 50) ? 1.0 : -1.0;

	destx = 0;
	desty = 0;
	bulletTimer = 0;
	jumpTimer = 0;
	state = 0;

	switch (enemyType)
	{
	case ELECTRICAT: /* ElectriCat */
		gravity = true;
		hp = 3; /* PRIME */
		dx = 4;
		dy = 0;
		break;
	case SLINKYCAT:
		gravity = true;
		hp = 3; /* PRIME */
		dx = 3;
		dy = 0;
		break;
	case SPIDERCAT:
		gravity = false;
		solid = false;
		hp = 5; /* PRIME */
		dx = 4;
		dy = 0;
		period = 101; /* PRIME */
		break;
	case DRAGONCAT: /* DragonCat */
		hp = 31; /* PRIME */
		gravity = false;
		solid = false;
		dx = 3;
		dy = 0;
		period = 301; /* PRIME */
		break;
	}

	dx *= hsign;
}

void Enemy::spawn(int val)
{
	Player *player = parent->player;
	if (player == NULL) return; // no sense in shooting when player is already dead.

	double dx = player->getx() - x;
	double dy = player->gety() - y;

	if (fabs (dx) > 400) return; // too far away.

	double len = sqrt (dx * dx + dy * dy);

	if (val == EBullet::ENERGY)
	{
		EBullet *b = new EBullet(parent, val, x, y, dx / len, dy / len);
		parent->add (b);
	}
	else
	{
		EBullet *b = new EBullet(parent, val, x, y, dx / len, 0);
		parent->add (b);
	}
}

void Enemy::update1()
{
	if (!air)
	{
		// check if we are moving onto empty space
		TEG_MAP *map = parent->getMap();
		int xx = x + ((dx > 0) ? getw() : 0);
		int yy = y + geth() + 1;
		int mx = xx / map->tilelist->tilew;
		int my = yy / map->tilelist->tileh;

		int tileflags = getTileStackFlags(mx, my);
		if ((tileflags & TS_SOLID) == 0)
		{
			// turn around
			dx = -dx;
		}
	}

	if (bulletTimer >=  301)
	{
		spawn(EBullet::ENERGY);
		bulletTimer = 0;
	}
	else
	{
		bulletTimer++;
	}
}

void Enemy::update2()
{
	if (bulletTimer >=  200)
	{
		spawn(EBullet::ENERGY);
		bulletTimer = 0;
	}
	else
	{
		bulletTimer++;
	}

	// jump
	if (!air)
	{
		dy = -13;
	}
}

void Enemy::update3()
{
	if (bulletTimer >=  150)
	{
		spawn(EBullet::ENERGY);
		bulletTimer = 0;
	}
	else
	{
		bulletTimer++;
	}

	if (state != 2)
	{
		dx = 5 * cos(float(phase * 2) * 6.282f / float(period));
		dy = -5 * sin(float(phase) * 6.282f / float(period));
	}
}

void Enemy::moveTo (double _x, double _y, double speed)
{
	destx = _x;
	desty = _y;
	double xx = destx - x;
	double yy = desty - y;
	double len = sqrt(xx * xx + yy * yy);
	dx = xx / len * speed;
	dy = yy / len * speed;
}

void Enemy::draw (const GraphicsContext &gc)
{
	SpriteEx::draw (gc);
	if (subtype == DRAGONCAT)
	{
		alfont_textprintf_centre_ex (gc.buffer, parent->gamefont, gc.buffer->w / 2, 0, RED, 0,
				"ENEMY: %i", hp);
#ifdef DEBUG
		if (parent->getParent()->isDebug())
		{
			alfont_textprintf_ex (gc.buffer, parent->smallfont, 0, 0, RED, 0,
					"estate %i, dx %3.2f, dy %3.2f, destx %3.2f, desty %3.2f", estate, dx, dy, destx, desty);
			int xofst = parent->getViewPort()->getx();
			int yofst = parent->getViewPort()->gety();
			line (gc.buffer, x - xofst, y - yofst, destx - xofst, desty - yofst, GREEN);
		}
#endif
	}
}

bool Enemy::nearDest ()
{
	return ((fabs (x - destx) < 8) && (fabs (y - desty) < 8));
}

void Enemy::update4()
{
	if (state == 2)
	{
		if (bulletTimer >=  20)
		{
			Explosion *e = new Explosion(parent, getx(), gety(), 30);
			parent->add (e);
			bulletTimer = 0;
		}
		else
		{
			bulletTimer++;
		}

		return; // no further update
	}

	switch (estate)
	{
	case 0: /* Make large circles */
		dx = 5 * cos(float(phase) * 6.282f / float(period));
		dy = -5 * sin(float(phase) * 6.282f / float(period));

		jumpTimer++;
		if (jumpTimer > 500)
		{
			destx = teg_pixelw(parent->getMap()) / 2;
			desty = 32;
			jumpTimer = 0;
			estate = 1;
		}
		if (bulletTimer >=  100)
		{
			spawn(EBullet::FIRE);
			bulletTimer = 0;
		}
		else
		{
			bulletTimer++;
		}
		break;
	case 1: /* Move slowly to top-center of map */
		moveTo (destx, desty, 4);
		if (nearDest())
		{
			if (parent->player == NULL)
			{
				destx = teg_pixelw(parent->getMap()) / 2;
				desty = teg_pixelh(parent->getMap()) / 2;
				estate = 3;
			}
			else
			{
				destx = parent->player->getx();
				desty = parent->player->gety();
				estate = 2;
			}
		}
		break;
	case 2: /* Dive towards player */
		moveTo (destx, desty, 12);
		if (nearDest())
		{
			destx = getx();
			desty = gety() - 100;
			estate = 3;
		}
		break;
	case 3: /* Move up 100 */
		moveTo (destx, desty, 2);
		if (nearDest())
		{
			estate = 0;
		}
		break;
	}
}

int Enemy::update()
{
	if (hittimer > 0)
	{
		hittimer--;
		if (hittimer == 0)
		{
			state = 0;
		}
	}

	if (parent->player != NULL)
	{
		double delta = x - parent->player->getx();
		if (fabs (delta) > 1000)
			return 0; // out of range, don't update.
	}

	phase = (phase + 1) % period;

	SpriteEx::update();
	switch (enemyType)
	{
	case 0:
		update1();
		break;
	case 1:
		update2();
		break;
	case 2:
		update3();
		break;
	case 3:
		update4();
		break;
	}

	return 0;
}

void Enemy::kill()
{
	SpriteEx::kill();
	if (subtype == DRAGONCAT)
	{
		parent->setTimer(50, Game::MSG_PLAYER_WINLEVEL);
	}
}

void Enemy::onCol (SpriteType st, Sprite *s, int dir)
{
	if (st == ST_FLOOR && gravity)
	{
		kill();
	}
	if (state == 2) return; // no need to handle collission
	if (st == ST_BULLET)
	{
		SpriteEx *se = dynamic_cast<SpriteEx *>(s);
		assert (se != NULL);
		switch (se->getSubType())
		{
		case Bullet::WPN_ROCK: hp -= 2; break;
		case Bullet::WPN_ICE: hp -= 3; break;
		case Bullet::WPN_LASER: hp -= 1; break;
		case Bullet::WPN_BAZOOKA: hp -= 5; break;
		}
		if (hp <= 0)
		{
			solid = false;
			gravity = true;
			state = 2;
		}
		else
		{
			state = 1;
			hittimer = 5;
		}
	}
	else if (st == ST_TILE || st == ST_BOUNDS || st == ST_ENEMY)
	{
		if (subtype != DRAGONCAT)
		{
			if (dir == DIR_VERTICAL)
			{
				// reverse direction
				dy = -dy;
				vsign = -vsign;
			}
			else
			{
				// reverse direction
				dx = -dx;
			}
		}
	}
}

EBullet::EBullet(Game * game, int type, int x, int y, double _dx, double _dy)
	: SpriteEx (game, ST_ENEMY_BULLET, x, y, DEFAULT_SPRITE_W, DEFAULT_SPRITE_H, type),
	  timer (0)
{
	dx = _dx;
	dy = _dy;
	switch (subtype)
	{
	case FIRE:
		gravity = true;
		solid = true;
		timer = 301;
		break;
	case ENERGY:
		gravity = false;
		solid = false;
		break;
	}
}

int EBullet::update()
{
	SpriteEx::update();
	switch (subtype)
	{
	case FIRE:
		timer--;
		if (timer <= 0) kill();
		break;
	case ENERGY:
		// just continue moving
		break;
	}
	return 0;
}

void EBullet::onCol (SpriteType st, Sprite *s, int dir)
{
	switch (st)
	{
		case ST_PLAYER:
		case ST_BOUNDS:
			kill();
			break;
		default:
			break;
	}
}

Bullet::Bullet(Game * game, int type, int x, int y, double _dx, double _dy)
	: SpriteEx (game, ST_BULLET, x, y, DEFAULT_SPRITE_W, DEFAULT_SPRITE_H, type)
{
	dx = _dx;
	dy = _dy;

	switch (subtype)
	{
	case WPN_ROCK:
		solid = true;
		gravity = true;
		break;
	case WPN_ICE:
		solid = true;
		gravity = true;
		break;
	case WPN_BAZOOKA:
		solid = true;
		gravity = false;
		break;
	case WPN_LASER:
		solid = false;
		gravity = false;
		break;
	}
}

void Bullet::onCol(SpriteType st, Sprite *s, int dir)
{
	switch (st)
	{
		case ST_TILE:
		{
			if (subtype == WPN_LASER || subtype == WPN_ROCK)
			{
				gravity = true;
				solid = false;
				if (dx > 0) dx = -2; else dx = 2; // bounce a little to the other side
				dy = -3; // bounce up a little
			}
			else if (subtype == WPN_BAZOOKA)
			{
				Explosion *expl = new Explosion(parent, x, y, 50);
				parent->add(expl);
				kill();
			}
		}
		break;
		case ST_BOUNDS:
			kill();
			break;
		case ST_ENEMY:
			if (subtype == WPN_LASER || subtype == WPN_ROCK)
			{
				// simply gets absorbed
				kill();
			}
			else if (subtype == WPN_BAZOOKA)
			{
				Explosion *expl = new Explosion(parent, x, y, 50);
				parent->add(expl);
				kill();
			}
			break;
		default:
			break;
	}
}

int Bullet::update()
{
	SpriteEx::update();
	if (subtype == WPN_BAZOOKA)
	{
		if (counter % 3 == 0) /* PRIME */
		{
			parent->smoke->add(x, y);
		}
		if (dx > 0)
			dx += BAZOOKA_ACC;
		else
			dx -= BAZOOKA_ACC;
	}
	return 0;
}

Explosion::Explosion(Game *e, int x, int y, int life) : Sprite (e, x, y)
{
	solid = false;
	color = YELLOW;
	maxLive = life;
	size = 1;
	tLive = 0;
}

void Explosion::draw(const GraphicsContext &gc)
{
	int x = getx() - parent->getViewPort()->getx();
	int y = gety() - parent->getViewPort()->gety();

	drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	set_trans_blender (0, 0, 0, 128);

	circlefill (gc.buffer, x, y, size, RED);
	circlefill (gc.buffer, x, y, size * 2 / 3, YELLOW);

	drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
}

int Explosion::update()
{
	size++;
	if (++tLive > maxLive)
	{
		kill();
	}
	return 0;
}
