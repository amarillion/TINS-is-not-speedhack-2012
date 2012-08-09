#include <allegro.h>
#include "component.h"

#ifndef LAYER_H
#define LAYER_H

class Engine;

class Layer : public IComponent
{
	protected:
		/**
			ViewPort is optional
		*/
		Engine &parent;
	public:

		Layer (Engine &engine);
		
		virtual void foreground();
		virtual void background();
		virtual void stop();

		virtual void draw(BITMAP *dest);

		Engine &getParent() { return parent; }
};

#endif
