#include <camera.h>

#define K_STIFFNESS 64.0f
#define K_DAMPING 12.0f
#define K_VELOCITY 32.0f
#define K_MAX_CLAMP 200.0f
#define CAMERA_ANCHOR_DELAY 1.0f

float camera_get_x ( camera * _ptr ) { if(_ptr) return _ptr->x; { float a = {0}; return a; } }; void camera_set_x ( camera * _ptr, float _value ) { if(_ptr) _ptr->x = _value; };
float camera_get_y ( camera * _ptr ) { if(_ptr) return _ptr->y; { float a = {0}; return a; } }; void camera_set_y ( camera * _ptr, float _value ) { if(_ptr) _ptr->y = _value; };
float camera_get_vx ( camera * _ptr ) { if(_ptr) return _ptr->vx; { float a = {0}; return a; } }; void camera_set_vx ( camera * _ptr, float _value ) { if(_ptr) _ptr->vx = _value; };
float camera_get_vy ( camera * _ptr ) { if(_ptr) return _ptr->vy; { float a = {0}; return a; } }; void camera_set_vy ( camera * _ptr, float _value ) { if(_ptr) _ptr->vy = _value; };
float camera_get_ax ( camera * _ptr ) { if(_ptr) return _ptr->ax; { float a = {0}; return a; } }; void camera_set_ax ( camera * _ptr, float _value ) { if(_ptr) _ptr->ax = _value; };
float camera_get_ay ( camera * _ptr ) { if(_ptr) return _ptr->ay; { float a = {0}; return a; } }; void camera_set_ay ( camera * _ptr, float _value ) { if(_ptr) _ptr->ay = _value; };
int camera_get_isAnchored ( camera * _ptr ) { if(_ptr) return _ptr->isAnchored; { int a = {0}; return a; } }; void camera_set_isAnchored ( camera * _ptr, int _value ) { if(_ptr) _ptr->isAnchored = _value; };
int camera_get_isDetached ( camera * _ptr ) { if(_ptr) return _ptr->isDetached; { int a = {0}; return a; } }; void camera_set_isDetached ( camera * _ptr, int _value ) { if(_ptr) _ptr->isDetached = _value; };
int camera_get_returnToAnchor ( camera * _ptr ) { if(_ptr) return _ptr->returnToAnchor; { int a = {0}; return a; } }; void camera_set_returnToAnchor ( camera * _ptr, int _value ) { if(_ptr) _ptr->returnToAnchor = _value; };

camera *
camera_create (float x, float y)
{
  camera *cam = (camera *) malloc (sizeof (camera));
  cam->x = x;
  cam->y = y;
  cam->vx = 0.0f;
  cam->vy = 0.0f;
  cam->ax = 0.0f;
  cam->ay = 0.0f;
  cam->isAnchored = 0.0f;
  cam->isDetached = 0.0f;
  cam->returnToAnchor = 0.0f;
  cam->returnToAnchorTime = 0.0f;
  return cam;
}

void
camera_destroy (camera * cam)
{
  if (cam)
    free (cam);
}

void
camera_set_anchor (camera * cam, object * anchor)
{
  if (cam)
    {
      cam->anchor = anchor;
      if (cam->anchor)
	{
	  cam->isAnchored = 1;
	  cam->isDetached = 0;
	  cam->returnToAnchorTime = 0.0f;
	}
      else
	{
	  cam->isAnchored = 0;
	  cam->isDetached = 0;
	  cam->returnToAnchor = 0;
	  cam->returnToAnchorTime = 0.0f;
	}
    }
}

void
camera_update (camera * cam, float dt)
{
  if (!cam)
    return;
  if (0 == cam->isAnchored)
    return;
  if (0.0f >= dt)
    return;
  if (1 == cam->isDetached)
    {
      float now = al_get_time ();
      if (now < cam->returnToAnchorTime)
	return;
      else
	{
	  cam->isDetached = 0;
	  cam->returnToAnchorTime = 0.0f;
	  cam->vx = 0.0f;
	  cam->vy = 0.0f;
	  cam->ax = 0.0f;
	  cam->ay = 0.0f;
	}
    }
  {
    float stretch[2], vel[2];
    stretch[0] = cam->anchor->x - cam->x;
    stretch[1] = cam->anchor->y - cam->y;
    vel[0] = cam->anchor->vx - cam->vx;
    vel[1] = cam->anchor->vy - cam->vy;
  //Clamp
    
//Damped spring with added velocity component
    cam->ax =
      (K_STIFFNESS * stretch[0] + K_VELOCITY * vel[0] - K_DAMPING * cam->vx);
    cam->ay =
      (K_STIFFNESS * stretch[1] + K_VELOCITY * vel[1] - K_DAMPING * cam->vy);
    if(cam->ax < -K_MAX_CLAMP)
	cam->ax = -K_MAX_CLAMP;
    if(cam->ax > K_MAX_CLAMP)
	cam->ax = K_MAX_CLAMP;
    if(cam->ay < -K_MAX_CLAMP)
	cam->ay = -K_MAX_CLAMP;
    if(cam->ay > K_MAX_CLAMP)
	cam->ay = K_MAX_CLAMP;
  }
  cam->vx += cam->ax * dt;
  cam->vy += cam->ay * dt;
  cam->x += cam->vx * dt;
  cam->y += cam->vy * dt;
}

void
camera_detach (camera * cam)
{
  if (!cam)
    return;
  cam->isDetached = 1;
  if (1 == cam->returnToAnchor)
    cam->returnToAnchorTime = al_get_time () + CAMERA_ANCHOR_DELAY;
}
