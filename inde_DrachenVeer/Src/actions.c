/*
 * actions.c
 *
 *  Created on: Aug 5, 2012
 *      Author: nikhil
 */

#include <physics.h>
#include <object.h>
#include <level.h>
#include <gameglobal.h>

//Move right
int
action_direction_right (object *hero, int key_up)
{
  if (hero)
  {
    if (!key_up)
	{
	  physics_setdirection (levels[curlevel]->physics, hero->physicsdata,
				DIR_RIGHT);
	  hero->facing = OBJECT_FACING_E;
	  sprite_set_state (hero->sprinstance, SPRITESET_WALK);
	}
    else
	{
	  physics_setdirection (levels[curlevel]->physics, hero->physicsdata,
				DIR_NONE);
	  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	}
    return 1;
  }
  return 0;
}

//Move right
int
action_direction_left (object *hero, int key_up)
{
  if (hero)
  {
    if (!key_up)
	{
	  physics_setdirection (levels[curlevel]->physics, hero->physicsdata,
				DIR_LEFT);
	  hero->facing = OBJECT_FACING_W;
	  sprite_set_state (hero->sprinstance, SPRITESET_WALK);
	}
    else
	{
	  physics_setdirection (levels[curlevel]->physics, hero->physicsdata,
				DIR_NONE);
	  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	}
    return 1;
  }
  return 0;
}

//Jump
int
action_jump (object *hero, int key_up)
{
  if (hero)
  {
    if (!key_up)
	{
	  physics_setjump (levels[curlevel]->physics, hero->physicsdata, 1);
	  sprite_set_state (hero->sprinstance, SPRITESET_JUMP);
	}
    else
	{
	  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	}
    return 1;
  }
  return 0;
}

//Jump
int
action_crouch (object *hero, int key_up)
{
  if (hero)
  {
    if (!key_up)
	{
	  physics_setjump (levels[curlevel]->physics, hero->physicsdata, 0);
	  sprite_set_state (hero->sprinstance, SPRITESET_CROUCH);
	}
    else
	{
	  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	}
    return 1;
  }
  return 0;
}

//Attack
int
action_attack (object *hero, int key_up)
{
  if (hero)
  {
    if (!key_up)
	{
	  sprite_set_state (hero->sprinstance, SPRITESET_ATTACK);
	}
    else
	{
	  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	}
    return 1;
  }
  return 0;
}

int
action_idle (object *hero, int key_up) {
  if (hero)
  {
	sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
	hero->vx_desired = 0.0f;
	return 1;
  }
  return 0;
}
