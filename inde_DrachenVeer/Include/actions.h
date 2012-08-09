/*
 * actions.h
 *
 *  Created on: Aug 5, 2012
 *      Author: nikhil
 */

#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <object.h>

int action_direction_right (object *hero, int key_up);
int action_direction_left (object *hero, int key_up);
int action_jump (object *hero, int key_up);
int action_crouch (object *hero, int key_up);
int action_attack (object *hero, int key_up);
int action_idle (object *hero, int key_up);

#endif /* ACTIONS_H_ */
