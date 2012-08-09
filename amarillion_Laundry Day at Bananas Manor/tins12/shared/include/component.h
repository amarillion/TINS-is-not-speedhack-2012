#ifndef COMPONENT_H
#define COMPONENT_H

#include "graphicscontext.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include "motion.h"
#include "DrawStrategy.h"
#include <boost/enable_shared_from_this.hpp>

struct ALFONT_FONT;

class ViewPort
{
private:
	int xofst, yofst;
public:
	virtual int getx() { return xofst; }
	virtual int gety() { return yofst; }
	virtual void setx(int val) { xofst = val; }
	virtual void sety(int val) { yofst = val; }
	virtual ~ViewPort() {}
};

class IComponent : public boost::enable_shared_from_this<IComponent>
{
protected:
	// if awake is set to false, update() will not be called.
	bool awake;
	// if alive is set to false, this component will be removed
	// neither update() nor draw() will be called.
	bool alive;
	// if visible is set to false, draw() will not be called
	bool visible;
#ifdef USE_ALFONT
	ALFONT_FONT *sfont;
#else
	FONT *sfont;
#endif

	//TODO: perhaps the Motion should get the counter and not the component.
	int counter;
	VisPtr vis;
	IMotionPtr motion;
	ViewPort *viewPort;

	double x, y;
	int w, h;

	static int destructorCount;
	static int constructorCount;
public:
	enum { MSG_FOCUS = 2000, MSG_UNFOCUS, MSG_KILL };

	IComponent() : awake(true), alive(true), visible(true), sfont(NULL), counter(0), vis(), motion(), viewPort(NULL), x(0), y(0), w(0), h(0)
	{ constructorCount++; }
	virtual ~IComponent();
	virtual int update() { counter++; return 0; }
	virtual void draw(const GraphicsContext &gc);

	void setLocation (double _x, double _y, int _w, int _h) { x = _x; y = _y; w = _w; h = _h; }
	void setDimension (int _w, int _h) { w = _w; h = _h; }

	void setMotion(const IMotionPtr &value) { motion = value; }
	IMotionPtr getMotion() { return motion; }
	void setVis(const VisPtr &value) { vis = value; vis->setParent(this); }
	VisPtr getVis() { return vis; }

	void setViewport (ViewPort *viewport) { this->viewPort = viewport; }
	ViewPort *getViewPort() { return viewPort; }

	int getCounter() { return counter; }

	virtual void handleMessage(int msg);
	bool isAwake() { return awake; }
	bool isAlive() { return alive; }
	bool isVisible() { return visible; }
	void setAwake(bool value) { awake = value; }
	void setVisible(bool value) { visible = value; }
	void kill() { alive = false; }

	void setFont(ALFONT_FONT *font) { this->sfont = font; }
	ALFONT_FONT *getFont() { return sfont; }

	double gety() { return y; }
	double getx() { return x; }
	int getw () const { return w; }
	int geth () const { return h; }
	void sety(double _y) { y = _y; }
	void setx(double _x) { x = _x; }

	static void printDebugInfo();
};

typedef boost::shared_ptr<IComponent> IComponentPtr;

#endif
