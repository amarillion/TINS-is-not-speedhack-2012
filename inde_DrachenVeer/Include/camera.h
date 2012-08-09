#ifndef CAMERA_H
#define CAMERA_H

#include <defs.h>
#include <object.h>

#define CAMERA_TETHER 200

struct _camera
{
  float x, y;
  float vx, vy;
  float ax, ay;
  int isAnchored, isDetached, returnToAnchor;
  float returnToAnchorTime;
  object *anchor;
};
typedef struct _camera camera;

float camera_get_x ( camera * _ptr ); void camera_set_x ( camera * _ptr, float _value );
float camera_get_y ( camera * _ptr ); void camera_set_y ( camera * _ptr, float _value );
float camera_get_vx ( camera * _ptr ); void camera_set_vx ( camera * _ptr, float _value );
float camera_get_vy ( camera * _ptr ); void camera_set_vy ( camera * _ptr, float _value );
float camera_get_ax ( camera * _ptr ); void camera_set_ax ( camera * _ptr, float _value );
float camera_get_ay ( camera * _ptr ); void camera_set_ay ( camera * _ptr, float _value );
int camera_get_isAnchored ( camera * _ptr ); void camera_set_isAnchored ( camera * _ptr, int _value );
int camera_get_isDetached ( camera * _ptr ); void camera_set_isDetached ( camera * _ptr, int _value );
int camera_get_returnToAnchor ( camera * _ptr ); void camera_set_returnToAnchor ( camera * _ptr, int _value );
object * camera_get_anchor ( camera * _ptr ); void camera_set_anchor ( camera * _ptr, object * _value );

camera *camera_create (float x, float y);
void camera_destroy (camera * cam);
void camera_update (camera * cam, float dt);
void camera_detach (camera * cam);
void center_camera(void *ptr);
#endif //CAMERA_H
